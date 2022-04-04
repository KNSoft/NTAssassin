﻿using System;
using System.IO;
using System.Reflection;
using System.Text.Json;

namespace NAC {
    static class Program {
        static readonly Assembly Asm = Assembly.GetExecutingAssembly();
        static readonly string FullNameWithVersion = Asm.GetCustomAttribute<AssemblyProductAttribute>().Product + " V" + Asm.GetName().Version.ToString();
        static readonly string ConfigFile = Asm.GetName().Name + ".config.json";
        static string ProjectDir;
        static string IntDir;
        static CConfigObject ConfigObject;
        static void ShowHelp() {
            // Show banner
            Console.WriteLine(Asm.GetCustomAttribute<AssemblyCompanyAttribute>().Company + " - " + FullNameWithVersion);
            Console.WriteLine();
            // Show syntax
            Console.WriteLine("Syntax: " + Asm.GetName().Name + " ProjectDir IntDir");
            Console.WriteLine("  ProjectDir: Path to the project directory, which contains " + Asm.GetName().Name + " config file (" + ConfigFile + ")");
            Console.WriteLine("  IntDir: Path to the intermediate directory, equals to $(IntDir) variable in Visual Studio");
            Console.WriteLine();
            // Exit
            Environment.Exit(0);
        }
        static int Main(string[] args) {

            // Resolve command line arguments+
            if (args.Length != 2 || args[0] == "/?" || args[0].ToUpper() == "/HELP")
                ShowHelp();
            ProjectDir = args[0];
            IntDir = args[1];
            if (ProjectDir[^1] != '\\')
                ProjectDir += '\\';
            if (IntDir[^1] != '\\')
                IntDir += '\\';
            if (!Directory.Exists(ProjectDir) || !Directory.Exists(IntDir))
                RTL.RaiseError(1, "Specified ProjectDir (" + ProjectDir + ") or IntDir (" + IntDir + ") not exists");

            // Read config file
            FileInfo HeaderFile = null, SourceFile = null;
            DateTime SrcLastModifyTime, DstLastModifyTime;

            string ConfigText = string.Empty, HeaderFilePath = string.Empty, SourceFilePath = string.Empty;
            try {
                ConfigText = File.ReadAllText(ProjectDir + ConfigFile);
            } catch {
                RTL.RaiseError(2, "Read config file (" + ProjectDir + ConfigFile + ") failed");
            }
            try {
                ConfigObject = JsonSerializer.Deserialize<CConfigObject>(ConfigText);
                HeaderFilePath = ProjectDir + ConfigObject.Output.HeaderFile + ".tmp";
                SourceFilePath = ProjectDir + ConfigObject.Output.SourceFile + ".tmp";
                if (Path.IsPathFullyQualified(HeaderFilePath) && Path.IsPathFullyQualified(SourceFilePath) && File.Exists(ProjectDir + ConfigObject.Input.NTAHeader)) {
                    HeaderFilePath = Path.GetFullPath(HeaderFilePath);
                    SourceFilePath = Path.GetFullPath(SourceFilePath);
                    HeaderFile = new FileInfo(HeaderFilePath);
                    SourceFile = new FileInfo(SourceFilePath);
                } else
                    throw new ApplicationException();
            } catch {
                RTL.RaiseError(3, "Invalid configurations");
            }

            // Create output file stream
            try {
                SrcLastModifyTime = File.GetLastWriteTime(ProjectDir + ConfigFile);
                DstLastModifyTime = File.GetLastWriteTime(IntDir + ConfigObject.Output.SourceFile.Substring(0, ConfigObject.Output.SourceFile.LastIndexOf('.')) + ".obj");
                if (SrcLastModifyTime > DstLastModifyTime)
                    DstLastModifyTime = DateTime.MinValue;
            } catch {
                SrcLastModifyTime = DstLastModifyTime = DateTime.MinValue;
            }

            FileStream fsHeader = null, fsSource = null;
            string FileComment = string.Format("// This file generated by {0}\r\n// Do not modify manually\r\n\r\n", FullNameWithVersion);
            string IncludeHeader = "#include \"" + ConfigObject.Input.NTAHeader + "\"";
            try {
                fsHeader = HeaderFile.Open(FileMode.Create, FileAccess.Write, FileShare.Read);
                fsSource = SourceFile.Open(FileMode.Create, FileAccess.Write, FileShare.Read);
            } catch {
                RTL.RaiseError(4, "Create output files failed");
            }
            // Write UTF-8 BOM
            var BOM = new byte[] { 0xEF, 0xBB, 0xBF };
            fsHeader.Write(BOM);
            fsSource.Write(BOM);

            RTL.WriteOutput(fsHeader, FileComment + "#pragma once\r\n\r\n" + IncludeHeader + "\r\n");
            RTL.WriteOutput(fsSource, FileComment + IncludeHeader + "\r\n");

            // Call loaders
            bool bModified = false;
            if (ConfigObject.Loaders.I18N != null)
                bModified |= Loaders.I18N.IsModified(ProjectDir, ConfigObject.Loaders.I18N, DstLastModifyTime);
            if (!bModified && ConfigObject.Loaders.SymExtract != null)
                bModified |= Loaders.SymExtract.IsModified(ProjectDir, ConfigObject.Loaders.SymExtract, DstLastModifyTime);
            if (!bModified && ConfigObject.Loaders.Binary != null)
                bModified |= Loaders.Binary.IsModified(ProjectDir, ConfigObject.Loaders.Binary, DstLastModifyTime);

            if (bModified) {
                if (ConfigObject.Loaders.I18N != null)
                    Loaders.I18N.Process(ProjectDir, fsHeader, fsSource, ConfigObject.Loaders.I18N);
                if (ConfigObject.Loaders.SymExtract != null)
                    Loaders.SymExtract.Process(ProjectDir, fsHeader, fsSource, ConfigObject.Loaders.SymExtract);
                if (ConfigObject.Loaders.Binary != null)
                    Loaders.Binary.Process(ProjectDir, fsHeader, fsSource, ConfigObject.Loaders.Binary);
            }

            // Cleanup
            fsHeader.Dispose();
            fsSource.Dispose();

            // Write file
            string DstHeader = Path.GetFullPath(ProjectDir + ConfigObject.Output.HeaderFile);
            string DstSource = Path.GetFullPath(ProjectDir + ConfigObject.Output.SourceFile);
            if (!File.Exists(DstHeader))
            {
                var fs = File.Create(DstHeader);
                fs.Dispose();
            }
            if (bModified)
                HeaderFile.Replace(DstHeader, null);
            else
                HeaderFile.Delete();
            if (!File.Exists(DstSource))
            {
                var fs = File.Create(DstSource);
                fs.Dispose();
            }
            if (bModified)
                SourceFile.Replace(DstSource, null);
            else
                SourceFile.Delete();

            return 0;
        }
    }
}
