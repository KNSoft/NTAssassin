// Whole Program Optimization is not supported

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Runtime.InteropServices;

[StructLayout(LayoutKind.Sequential, Pack = 1)]
struct IMAGE_FILE_HEADER {
    public ushort Machine;
    public ushort NumberOfSections;
    public uint TimeDateStamp;
    public uint PointerToSymbolTable;
    public uint NumberOfSymbols;
    public ushort SizeOfOptionalHeader;
    public ushort Characteristics;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
struct IMAGE_SECTION_HEADER {
    public ulong Name;
    public uint VirtualSize;
    public uint VirtualAddress;
    public uint SizeOfRawData;
    public uint PointerToRawData;
    public uint PointerToRelocations;
    public uint PointerToLinenumbers;
    public ushort NumberOfRelocations;
    public ushort NumberOfLinenumbers;
    public uint Characteristics;
}

[StructLayout(LayoutKind.Explicit, Pack = 1)]
struct IMAGE_SYMBOL {
    [FieldOffset(0)]
    public ulong ShortName;
    [FieldOffset(0)]
    public uint Short;
    [FieldOffset(4)]
    public uint Long;
    [FieldOffset(8)]
    public uint Value;
    [FieldOffset(12)]
    public ushort SectionNumber;
    [FieldOffset(14)]
    public ushort Type;
    [FieldOffset(14)]
    public byte TypeMSB;
    [FieldOffset(15)]
    public byte TypeLSB;
    [FieldOffset(16)]
    public byte StorageClass;
    [FieldOffset(17)]
    public byte NumberOfAuxSymbols;
}

enum Machine {
    IMAGE_FILE_MACHINE_I386 = 0x14C,
    IMAGE_FILE_MACHINE_AMD64 = 0x8664
}

enum StorageClass {
    IMAGE_SYM_CLASS_EXTERNAL = 2
}

namespace NAC.Loaders {
    static class SymExtract {
        public static void InvalidSymFile(FileInfo SymFile) {
            RTL.RaiseError(202, "Invalid or not supported symbol file \"" + SymFile.FullName + "\" failed");
        }
        public class SymExtractSymbol {
            IMAGE_SYMBOL Info;
            string Name;
            bool IsFunction;
            public uint Value;
            public SymExtractSymbol(IMAGE_SYMBOL stInfo, string szName) {
                Info = stInfo;
                Name = szName;
                IsFunction = Info.TypeMSB == 0x20;
                Value = stInfo.Value;
            }
            public bool MatchName(string szName, Machine eMachine) {
                string TempName = szName;
                if (eMachine == Machine.IMAGE_FILE_MACHINE_I386) {
                    TempName = '_' + TempName;
                    if (IsFunction) {
                        TempName += '@';
                        if (!Name.StartsWith(TempName))
                            return false;
                        TempName = Name.Substring(TempName.Length);
                        if (string.IsNullOrWhiteSpace(TempName))
                            return false;
                        uint uParamSize;
                        try {
                            uParamSize = Convert.ToUInt32(TempName);
                        } catch {
                            return false;
                        }
                        return uParamSize % 4 == 0;
                    }
                }
                return Name == TempName;
            }
        }
        public class SymExtractSection {
            public int Number;
            public IMAGE_SECTION_HEADER Info;
            public List<SymExtractSymbol> Symbols = new List<SymExtractSymbol>();
            public SymExtractSection(int iNumber) {
                Number = iNumber;
            }
        }
        public class SymExtractFile {
            public Machine MachineType;
            public List<SymExtractSection> Sections = new List<SymExtractSection>();
            public FileStream fsSymFile = null;
            CO_SymbolExtract ConfigObject;
            unsafe public SymExtractFile(string ProjectDir, CO_SymbolExtract SymbolItem) {
                ConfigObject = SymbolItem;
                Console.WriteLine("SymExtract Loader - Compiling " + SymbolItem.File);
                // Open file
                FileInfo SymFile = null;
                try {
                    SymFile = new FileInfo(ProjectDir + SymbolItem.File);
                    fsSymFile = SymFile.OpenRead();
                } catch {
                    RTL.RaiseError(201, "Open symbol file \"" + SymFile.FullName + "\" failed");
                }
                // Read header
                var byteSymHeader = new byte[sizeof(IMAGE_FILE_HEADER)];
                RTL.ReadStream(fsSymFile, byteSymHeader, 0);
                IMAGE_FILE_HEADER SymHeader = RTL.RawToStruct<IMAGE_FILE_HEADER>(byteSymHeader);
                if (SymHeader.NumberOfSections == 0 ||
                    SymHeader.NumberOfSymbols == 0 ||
                    SymHeader.PointerToSymbolTable == 0 || (
                        SymHeader.Machine != (ushort)Machine.IMAGE_FILE_MACHINE_I386 &&
                        SymHeader.Machine != (ushort)Machine.IMAGE_FILE_MACHINE_AMD64
                        )
                    )
                    InvalidSymFile(SymFile);
                MachineType = (Machine)SymHeader.Machine;
                // Load symbols
                int SymTablePointer = Convert.ToInt32(SymHeader.PointerToSymbolTable);
                int StrTableOffset = SymTablePointer + Convert.ToInt32(SymHeader.NumberOfSymbols * Marshal.SizeOf(typeof(IMAGE_SYMBOL)));
                for (int i = 0; i < SymHeader.NumberOfSymbols; i++) {
                    var byteSymItem = new byte[sizeof(IMAGE_SYMBOL)];
                    RTL.ReadStream(fsSymFile, byteSymItem, SymTablePointer);
                    var SymItem = RTL.RawToStruct<IMAGE_SYMBOL>(byteSymItem);
                    if (SymItem.TypeLSB == 0 &&
                        SymItem.StorageClass == (byte)StorageClass.IMAGE_SYM_CLASS_EXTERNAL &&
                        SymItem.SectionNumber > 0 &&
                        SymItem.NumberOfAuxSymbols == 0
                        ) {
                        // Get symbol name
                        string SymName;
                        var byteChars = new List<byte>();
                        if (SymItem.Short == 0) {
                            int SymNameOffset = StrTableOffset + Convert.ToInt32(SymItem.Long);
                            for (int j = 0; j < 256; j++) {
                                byte[] byteChar = new byte[1];
                                RTL.ReadStream(fsSymFile, byteChar, SymNameOffset);
                                if (byteChar[0] == '\0')
                                    break;
                                byteChars.Add(byteChar[0]);
                                SymNameOffset++;
                            }
                        } else {
                            ulong SymShortName = SymItem.ShortName;
                            ulong charMask = 0x00000000000000FF;
                            var byte8Char = new byte[8];
                            for (int j = 0; j < byte8Char.Length; j++) {
                                byte8Char[byte8Char.Length - j - 1] = Convert.ToByte(SymShortName & charMask);
                                SymShortName >>= 8;
                            }
                            for (int j = 7; j >= 0; j--)
                                if (byte8Char[j] != 0)
                                    byteChars.Add(byte8Char[j]);
                                else
                                    break;
                        }
                        SymName = Encoding.UTF8.GetString(byteChars.ToArray());
                        // Add symbol to section
                        SymExtractSection Section = Sections.Find(x => x.Number == SymItem.SectionNumber);
                        if (Section == null) {
                            Section = new SymExtractSection(SymItem.SectionNumber);
                            Sections.Add(Section);
                        }
                        Section.Symbols.Add(new SymExtractSymbol(SymItem, SymName));
                    }
                    SymTablePointer += Marshal.SizeOf(typeof(IMAGE_SYMBOL));
                }
                // Fill section header and short symbols
                foreach (var Section in Sections) {
                    var byteSecHeader = new byte[sizeof(IMAGE_SECTION_HEADER)];
                    RTL.ReadStream(fsSymFile, byteSecHeader, sizeof(IMAGE_FILE_HEADER) + (Section.Number - 1) * sizeof(IMAGE_SECTION_HEADER));
                    Section.Info = RTL.RawToStruct<IMAGE_SECTION_HEADER>(byteSecHeader);
                    Section.Symbols.Sort((Sym1, Sym2) => {
                        if (Sym1.Value > Sym2.Value)
                            return 1;
                        else if (Sym1.Value == Sym2.Value)
                            return 0;
                        else
                            return -1;
                    });
                }

            }
            ~SymExtractFile() {
                fsSymFile?.Dispose();
            }
        }
        public static bool IsModified(string ProjectDir, CO_SymbolExtract[] ConfigObject, DateTime LastOutputTime) {
            // Skip build according to last modify time
            bool bModified = false;
            foreach (var SymbolItem in ConfigObject) {
                string SymFilePath = ProjectDir + SymbolItem.File;
                if (!File.Exists(SymFilePath))
                    RTL.RaiseError(200, "Symbol file \"" + SymFilePath + "\" not exists");
                if (File.GetLastWriteTime(SymFilePath) > LastOutputTime) {
                    bModified = true;
                    break;
                }
            }
            if (!bModified)
                Console.WriteLine("SymExtract is up-to-date");
            return bModified;
        }
        public static bool Process(string ProjectDir, FileStream fsHeaderFile, FileStream fsSourceFile, CO_SymbolExtract[] ConfigObject) {
            // Process each file
            foreach (var SymbolItem in ConfigObject) {
                SymExtractFile SymFile = new SymExtractFile(ProjectDir, SymbolItem);
                foreach (var Symbol in SymbolItem.Symbols) {
                    bool bSymbolFound = false;
                    foreach (var Section in SymFile.Sections) {
                        // Find symbol in section
                        var DstSymbol = Section.Symbols.Find(x => x.MatchName(Symbol.Name, SymFile.MachineType));
                        if (DstSymbol == null)
                            continue;
                        // Find next symbol
                        int iNextIndex = Section.Symbols.IndexOf(DstSymbol) + 1;
                        SymExtractSymbol NextSymbol = Section.Symbols.Count > iNextIndex ? Section.Symbols[iNextIndex] : null;
                        // Get offset, size and data
                        var Offset = Section.Info.PointerToRawData + DstSymbol.Value;
                        var Size = NextSymbol == null ? Section.Info.SizeOfRawData : NextSymbol.Value - DstSymbol.Value;
                        if (Size == 0)
                            RTL.RaiseError(202, "Size of symbol \"" + Symbol.Name + "\" is 0");
                        var Data = new byte[Size];
                        RTL.ReadStream(SymFile.fsSymFile, Data, Offset);
                        // Write to output
                        var SymbolDef = string.Format("BYTE {0}[{1:D}]", "SYM_" + (string.IsNullOrWhiteSpace(Symbol.Export) ? Symbol.Name : Symbol.Export), Size);
                        int RemainSize = Convert.ToInt32(Size);
                        uint BytesPerLine = 25;
                        RTL.WriteOutput(fsHeaderFile, "EXTERN_C " + SymbolDef + ";");
                        RTL.WriteOutput(fsSourceFile, SymbolDef + " = {");
                        while (RemainSize > 0) {
                            var LineSize = RemainSize > BytesPerLine ? BytesPerLine : Convert.ToUInt32(RemainSize);
                            var Bytes = new string[LineSize];
                            for (var j = 0; j < LineSize; j++)
                                Bytes[j] = string.Format("0x{0:X2}", Data[Size - RemainSize--]);
                            RTL.WriteOutput(fsSourceFile, "\t" + string.Join(", ", Bytes) + (RemainSize == 0 ? null : ","));
                        }
                        RTL.WriteOutput(fsSourceFile, "};\r\n");
                        bSymbolFound = true;
                        break;
                    }
                    if (!bSymbolFound)
                        RTL.RaiseError(203, "Symbol \"" + Symbol.Name + "\" not found in \"" + SymbolItem.File + "\"");
                }
            }
            return true;
        }
    }
}
