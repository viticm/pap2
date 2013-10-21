using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Windows.Forms;

namespace ScriptViewer
{
    class LuaRuntime
    {
        private static LuaRuntime luaRuntime; // lua虚拟机对象

        /// <summary>
        /// 私有构造函数
        /// </summary>
        private LuaRuntime()
        {

        }

        /// <summary>
        /// 获取lua虚拟机对象
        /// </summary>
        /// <returns>lua虚拟机对象</returns>
        public static LuaRuntime GetLuaRunTime()
        {
            if (luaRuntime == null)
            {
                luaRuntime = new LuaRuntime();                
                luaRuntime.Init();
            }

            return luaRuntime;
        }

        /// <summary>
        /// 初始化lua虚拟机
        /// </summary>
        private void Init()
        {
            string strLuaFile = "";
            strLuaFile = Path.Combine(Application.StartupPath, @"Plugins\LuaCheck\IntelliSense.lua") ;  
          
            if (File.Exists(strLuaFile))
            {
                StreamReader sr = new StreamReader(strLuaFile, Encoding.GetEncoding("gb2312"));
                string strOut = "";
                bool bret = LuaRuntime.LuaInit(sr.ReadToEnd(), ref strOut);
                sr.Close();
            }
        }

        [DllImport("Plugins\\luacheck\\LuaRuntime.dll", EntryPoint = "LuaInit", CharSet = CharSet.Ansi)]
        public static extern bool LuaInit(string strCode, ref string szOutput);

        [DllImport("Plugins\\luacheck\\LuaRuntime.dll", EntryPoint = "LuaRun", CharSet = CharSet.Ansi)]
        public static extern bool LuaRun(string szFuncName, string szCode, int nPos, int nIsDelete, string szSetting, ref IntPtr szOutput);
    }
}
