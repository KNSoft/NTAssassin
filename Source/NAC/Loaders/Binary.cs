using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Text.Json;

namespace NAC.Loaders
{
    static class Binary
    {
        public static bool IsModified(string ProjectDir, CO_Binary[] ConfigObject, DateTime LastOutputTime)
        {
            // Skip build according to last modify time
            bool bModified = false;
            foreach (var BinaryItem in ConfigObject)
            {
                string BinFilePath = ProjectDir + BinaryItem.File;
                if (!File.Exists(BinFilePath))
                    RTL.RaiseError(200, "Binary file \"" + BinFilePath + "\" not exists");
                if (File.GetLastWriteTime(BinFilePath) > LastOutputTime)
                {
                    bModified = true;
                    break;
                }
            }
            if (!bModified)
                Console.WriteLine("Binary is up-to-date");
            return bModified;
        }
        public static bool Process(string ProjectDir, FileStream fsHeaderFile, FileStream fsSourceFile, CO_Binary[] ConfigObject)
        {

            Stopwatch Elapsed = new Stopwatch();
            Elapsed.Start();
            Console.WriteLine("Binary Loader starts...");

            // Process each file
            foreach (var BinaryItem in ConfigObject)
            {
                Console.WriteLine("Binary Loader - Compiling " + BinaryItem.File);
                FileStream fsBinFile = null;
                FileInfo BinFile = null;
                try
                {
                    BinFile = new FileInfo(ProjectDir + BinaryItem.File);
                    fsBinFile = BinFile.OpenRead();
                }
                catch
                {
                    RTL.RaiseError(301, "Open binary file \"" + BinFile.FullName + "\" failed");
                }
                if (BinFile.Length <= 0)
                {
                    RTL.RaiseError(302, "Binary file \"" + BinFile.FullName + "\" is empty!");
                }
                var Data = new byte[BinFile.Length];
                RTL.ReadStream(fsBinFile, Data, 0);
                var SymbolDef = string.Format("BYTE {0}[{1:D}]", "BIN_" + BinaryItem.Export, BinFile.Length);
                int RemainSize = Convert.ToInt32(BinFile.Length);
                uint BytesPerLine = 25;
                RTL.WriteOutput(fsHeaderFile, "EXTERN_C " + SymbolDef + ";");
                RTL.WriteOutput(fsSourceFile, SymbolDef + " = {");
                while (RemainSize > 0)
                {
                    var LineSize = RemainSize > BytesPerLine ? BytesPerLine : Convert.ToUInt32(RemainSize);
                    var Bytes = new string[LineSize];
                    for (var j = 0; j < LineSize; j++)
                        Bytes[j] = string.Format("0x{0:X2}", Data[BinFile.Length - RemainSize--]);
                    RTL.WriteOutput(fsSourceFile, "\t" + string.Join(", ", Bytes) + (RemainSize == 0 ? null : ","));
                }
                RTL.WriteOutput(fsSourceFile, "};\r\n");
            }

            Elapsed.Stop();
            Console.WriteLine("Binary Loader finished, elapsed {0:G}ms", Elapsed.Elapsed.TotalMilliseconds);
            return true;
        }
    }
}
