using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Text.Json;

namespace NAC.Loaders {
    static class I18N {
        static readonly List<I18NItem> I18NItems = new List<I18NItem>();
        static readonly List<I18NLang> I18NLangs = new List<I18NLang>();
        static FileStream HeaderFile = null;
        static FileStream SourceFile = null;
        public static Dictionary<string, string> FontDB = new Dictionary<string, string> {
            { "ar", "Segoe UI" },
            { "el", "Segoe UI" },
            { "zh-Hans", "微软雅黑" },
            { "zh-Hant", "Microsoft JhengHei UI" },
            { "mn", "Mongolian Baiti" },
            { "ja", "Meiryo UI" },
            { "ko", "Malgun Gothic" },
            { "he", "Gisha" },
            { "th", "Leelawadee UI" }
        };
        public class I18NItem {
            public string Key;
            public int Hash;
            public Dictionary<string, string> Entries;
            public I18NItem(string szKey, Dictionary<string, string> dictEntries) {
                Hash = 0;
                foreach (var ch in szKey)
                    if (char.IsWhiteSpace(ch))
                        throw new ApplicationException();
                    else
                        Hash = 65599 * Hash + ch;
                Key = szKey;
                Entries = dictEntries;
            }
        }
        public class I18NLang {
            public CultureInfo Info;
            public bool Default;
            public string LangSymbol;
            public string ItemsSymbol;
            public string ParentLangSymbol;
            public List<KeyValuePair<int, string>> ItemTable = new List<KeyValuePair<int, string>>();
            public I18NLang(CultureInfo CI) {
                Info = CI;
                Default = false;
                LangSymbol = "NAC_I18N_Lang_" + Info.Name.ToString().Replace('-', '_');
                ItemsSymbol = LangSymbol + "_Items";
                ParentLangSymbol = string.Empty;
            }
            public void Output() {

                // Find parent language
                I18NLang ParentLang = this;
                CultureInfo ParentCL = Info;
                do {
                    ParentCL = ParentCL.Parent;
                    ParentLang = I18NLangs.Find(x => x.Info.Name == ParentCL.Name);
                    if (ParentLang != null) {
                        ParentLangSymbol = ParentLang.LangSymbol;
                        break;
                    }
                } while (ParentCL.Name != "");

                // Match Font

                ParentCL = Info;
                string FontName = string.Empty;
                do {
                    if (FontDB.ContainsKey(ParentCL.Name)) {
                        FontName = FontDB[ParentCL.Name];
                        break;
                    }
                    ParentCL = ParentCL.Parent;
                } while (ParentCL.Name != "");

                string SymbolDef;
                // Items
                SymbolDef = string.Format("I18N_ITEM {0}[{1:D}]", ItemsSymbol, I18NItems.Count);
                RTL.WriteOutput(HeaderFile, "EXTERN_C " + SymbolDef + ";");
                RTL.WriteOutput(SourceFile, SymbolDef + " = {");
                I18NItems.ForEach(GItem => {
                    var Item = ItemTable.Find(x => x.Key == GItem.Hash);
                    var ItemDef = string.Format("\t{{0x{0:X8}, {1}}}", GItem.Hash, Item.Value == null ? "NULL" : "TEXT(\"" + RTL.EscapeCString(Item.Value) + "\")");
                    if (GItem != I18NItems[^1])
                        ItemDef += ',';
                    RTL.WriteOutput(SourceFile, ItemDef);
                });
                RTL.WriteOutput(SourceFile, "};");
                // Language
                SymbolDef = "I18N_LANGUAGE " + LangSymbol;
                RTL.WriteOutput(HeaderFile, "EXTERN_C " + SymbolDef + ";");
                string[] NameParts = { "0", "0", "0" }, SplitNames = Info.Name.Split('-');
                for (uint i = 0; i < SplitNames.Length; i++)
                    NameParts[i] = "('" + SplitNames[i] + "')";
                string[] Members = {
                    "{" + string.Join(", ", NameParts) + "}",
                    Info.LCID.ToString(),
                    ParentLangSymbol == string.Empty ? "NULL" : ("&" + ParentLangSymbol),
                    "TEXT(\"" + Info.NativeName + "\")",
                    FontName == string.Empty ? "NULL" : ("TEXT(\"" + FontName) + "\")",
                    Info.TextInfo.IsRightToLeft.ToString().ToUpper(),
                    ItemsSymbol
                };
                RTL.WriteOutput(SourceFile, SymbolDef + " = { " + string.Join(", ", Members) + " };\r\n");
            }
        }
        public static bool IsModified(string ProjectDir, CO_I18N ConfigObject, DateTime LastOutputTime) {
            // Gather all the config files
            var I18NConfFiles = Directory.GetFiles(ProjectDir, ConfigObject.Pattern);

            // Skip build according to last modify time
            bool bModified = false;
            foreach (var I18NConfFile in I18NConfFiles) {
                if (File.GetLastWriteTime(I18NConfFile) > LastOutputTime) {
                    bModified = true;
                    break;
                }
            }
            if (!bModified)
                Console.WriteLine("I18N is up-to-date");
            return bModified;
        }
        public static bool Process(string ProjectDir, FileStream fsHeaderFile, FileStream fsSourceFile, CO_I18N ConfigObject) {

            Stopwatch Elapsed = new Stopwatch();
            Elapsed.Start();
            Console.WriteLine("I18N Loader starts...");

            // Search I18N configuration files according to the specified pattern
            if (string.IsNullOrWhiteSpace(ConfigObject.Pattern))
                RTL.RaiseError(100, "Invalid search pattern of I18N configuration files");

            // Initialize
            HeaderFile = fsHeaderFile;
            SourceFile = fsSourceFile;

            // Process each file
            var I18NConfFiles = Directory.GetFiles(ProjectDir, ConfigObject.Pattern);
            foreach (var I18NConfFile in I18NConfFiles) {
                Console.WriteLine("I18N Loader - Compiling " + I18NConfFile);
                Dictionary<string, Dictionary<string, string>> TempI18NItems = null;

                // Resolve I18N config file
                try {
                    TempI18NItems = JsonSerializer.Deserialize<Dictionary<string, Dictionary<string, string>>>(File.ReadAllText(I18NConfFile));
                } catch {
                    RTL.RaiseError(101, "Resolve I18N config file (" + I18NConfFile + ") failed");
                }
                foreach (var TempI18NItem in TempI18NItems)
                    try {
                        I18NItems.Add(new I18NItem(TempI18NItem.Key, TempI18NItem.Value));
                    } catch {
                        RTL.RaiseError(103, "Invalid key name: \"" + TempI18NItem.Key + "\" in \"" + I18NConfFile + "\"");
                    }
            }

            // Compile
            RTL.WriteOutput(HeaderFile, "#define NAC_I18N_ITEM_COUNT " + I18NItems.Count.ToString() + "\r\n");
            RTL.WriteOutput(HeaderFile, "typedef enum _I18N_ITEM_INDEX {");
            I18NItems.ForEach(I18NItem => {
                RTL.WriteOutput(HeaderFile, "\tI18NIndex_" + I18NItem.Key + (I18NItem == I18NItems[^1] ? string.Empty : ","));
                foreach (var I18NItemEntry in I18NItem.Entries) {
                    // Match language existed
                    CultureInfo TempCI = null;
                    I18NLang TempLang;
                    try {
                        TempCI = new CultureInfo(I18NItemEntry.Key);
                    } catch {
                        RTL.RaiseError(105, "Invalid culture name: " + I18NItemEntry.Key);
                    }
                    TempLang = I18NLangs.Find(x => x.Info.Name == TempCI.Name);
                    // Add language if not exists yet
                    if (TempLang == null) {
                        TempLang = new I18NLang(TempCI);
                        if (I18NItemEntry.Key == ConfigObject.Default)
                            TempLang.Default = true;
                        I18NLangs.Add(TempLang);
                    }
                    // Add language item
                    TempLang.ItemTable.Add(new KeyValuePair<int, string>(I18NItem.Hash, I18NItemEntry.Value));
                }
            });
            RTL.WriteOutput(HeaderFile, "} I18N_ITEM_INDEX, * PI18N_ITEM_INDEX;\r\n");
            // Output languages
            var LangSymbols = new List<string>();
            var DefaultLang = I18NLangs.Find(x => x.Default == true);
            if (DefaultLang != null) {
                I18NLangs.Remove(DefaultLang);
                I18NLangs.Insert(0, DefaultLang);
            }
            I18NLangs.ForEach(Lang => {
                LangSymbols.Add("&" + Lang.LangSymbol);
                Lang.Output();
            });
            string SymbolDef = string.Format("PI18N_LANGUAGE NAC_I18N_Lang_Table[{0:D}]", I18NLangs.Count);
            RTL.WriteOutput(HeaderFile, "EXTERN_C " + SymbolDef + ";\r\n");
            RTL.WriteOutput(SourceFile, SymbolDef + " = { " + string.Join(", ", LangSymbols) + " };\r\n");

            Elapsed.Stop();
            Console.WriteLine("I18N Loader finished, elapsed {0:G}ms", Elapsed.Elapsed.TotalMilliseconds);
            return true;
        }
    }
}
