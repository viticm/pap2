using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace QueryContextData
{
    public class IniIO
    {
        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);
        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

        public static void IniWriteValue(string Section, string Key, string Value, string filepath)    //对ini文件进行写操作的函数
        {
            long i = 0;
            i = WritePrivateProfileString(Section, Key, Value, filepath);
        }

        public static string IniReadValue(string Section, string Key, string filepath)                 //对ini文件进行读操作的函数
        {
            StringBuilder temp = new StringBuilder(255);
            int i = GetPrivateProfileString(Section, Key, "", temp, 255, filepath);
            return temp.ToString();
        }
    }
}
