using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace DataTableRemoting
{
    public class INIFileHelper
    {
        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filename);
        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder sb, int size, string filename);

        private string mFileName = string.Empty;

        private static INIFileHelper mCfgINIHelper;

        public INIFileHelper(string filename)
        {
            mFileName = filename;
        }

        public long INIWriteFile(string section, string key, string val)
        {
            return INIFileHelper.INIWriteFile(section, key, val, mFileName);
        }

        public string INIReadFile(string section, string key)
        {
            return INIFileHelper.INIReadFile(section, key, mFileName);
        }

        public static long INIWriteFile(string section, string key, string val, string filename)
        {
            long nRet = WritePrivateProfileString(section, key, val, filename);
            return nRet;
        }

        public static string INIReadFile(string section, string key, string filename)
        {
            StringBuilder sb = new StringBuilder();
            int nRet = GetPrivateProfileString(section, key, "", sb, 555, filename);
            string result = sb.ToString();
            return result.Trim();
        }

        public static INIFileHelper CfgINIHelper
        {
            get 
            {
                if (mCfgINIHelper == null)
                {
                    mCfgINIHelper = new INIFileHelper(".//ServerConfig.ini");
                }
                return mCfgINIHelper;
            }
        }

    }
}
