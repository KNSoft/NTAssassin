using System;
using System.IO;
using System.Text;
using System.Runtime.InteropServices;

namespace NAC {
    static class RTL {
        /// <summary>
        /// Report fatal error and exit
        /// </summary>
        /// <param name="Code">Error code, is also the exit code</param>
        /// <param name="Msg">Error message</param>
        public static void RaiseError(int Code, string Msg) {
            Console.WriteLine("NAC: fatal error NAC{0:D3}: {1}", Code, Msg);
            Environment.Exit(Code);
        }

        /// <summary>
        /// Write string to output FileStream, with UTF-8 encoding and line terminator
        /// </summary>
        /// <param name="Output">Output FileStream</param>
        /// <param name="Content">String content</param>
        public static void WriteOutput(FileStream Output, string Content) {
            Output.Write(Encoding.UTF8.GetBytes(Content + "\r\n"));
        }

        /// <summary>
        /// Escape C special characters
        /// </summary>
        /// <param name="Str">Input string</param>
        /// <returns>New string escaped</returns>
        public static string EscapeCString(string Str) {
            return Str.Replace("\0", "\\0");
        }

        /// <summary>
        /// Convert RAW bytes to struct
        /// </summary>
        /// <param name="RawBytes">RAW bytes, size should not be smaller than the struct</param>
        /// <param name="StrcutType">Type of struct</param>
        /// <returns>Struct object</returns>
        public static T RawToStruct<T>(byte[] RawBytes) where T : new() {
            Type anyType = new T().GetType();
            int RawSize = Marshal.SizeOf(anyType);
            if (RawSize > RawBytes.Length)
                throw new OverflowException();
            IntPtr buffer = Marshal.AllocHGlobal(RawSize);
            Marshal.Copy(RawBytes, 0, buffer, RawSize);
            T obj = (T)Marshal.PtrToStructure(buffer, anyType);
            Marshal.FreeHGlobal(buffer);
            return obj;
        }

        /// <summary>
        /// Read file stream
        /// </summary>
        /// <param name="fs">File stream</param>
        /// <param name="RawBytes">Buffer to receive data</param>
        /// <param name="Offset">Offset to read of file</param>
        public static void ReadStream(FileStream fs, byte[] RawBytes, long Offset) {
            int ReadBytes;
            try {
                fs.Seek(Offset, SeekOrigin.Begin);
                ReadBytes = fs.Read(RawBytes);
                if (ReadBytes != RawBytes.Length)
                    throw new ApplicationException();
            } catch {
                RaiseError(1000, "Invalid file data to read");
            }
            
        }
    }
}
