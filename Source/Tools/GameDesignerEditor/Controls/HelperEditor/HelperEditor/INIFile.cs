using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace HelperEditor
{
    class INIFile
    {
        /// <summary>
        /// INI文件名称（带路径)
        /// </summary>
        public string filePath;

        // 声明读写INI文件的API函数
        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);

        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

        /// <summary>
        /// 类的构造函数
        /// </summary>
        /// <param  name="INIPath">INI文件名</param>  
        public INIFile(string INIPath)
        {
            filePath = INIPath;
        }
        /// <summary>
        /// 写INI文件
        /// </summary>
        /// <param  name="Section">Section</param>
        /// <param  name="Key">Key</param>
        /// <param  name="value">value</param>
        public void SetValue(string Section, string Key, string value)
        {
            WritePrivateProfileString(Section, Key, value, this.filePath);
        }
        
        /// <summary>
        /// 读取INI文件指定部分
        /// </summary>
        /// <param  name="Section">Section</param>
        /// <param  name="Key">Key</param>
        /// <returns>String</returns>  
        public string GetValue(string Section, string Key)
        {
            StringBuilder temp = new StringBuilder(1024);
            int i = GetPrivateProfileString(Section, Key, "读取错误", temp, 1024, this.filePath);
            return temp.ToString();
        }
    }
}
