namespace NAC {
    public class CO_Input {
        public string NTAHeader { get; set; }
    }
    public class CO_I18N {
        public string Pattern { get; set; }
        public string Default { get; set; }
    }
    public class CO_SymbolExtract_Symbol {
        public string Name { get; set; }
        public string Export { get; set; }
    }
    public class CO_SymbolExtract {
        public string File { get; set; }
        public CO_SymbolExtract_Symbol[] Symbols { get; set; }
    }
    public class CO_Binary
    {
        public string File { get; set; }
        public string Export { get; set; }
    }
    public class CO_Loaders {
        public CO_I18N I18N { get; set; }
        public CO_SymbolExtract[] SymExtract { get; set; }
        public CO_Binary[] Binary { get; set; }
    }
    public class CO_Output {
        public string HeaderFile { get; set; }
        public string SourceFile { get; set; }
    }
    class CConfigObject {
        public CO_Input Input { get; set; }
        public CO_Loaders Loaders { get; set; }
        public CO_Output Output { get; set; }
    }
}
