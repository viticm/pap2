using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;


namespace testDll
{
    class Program
    {
        static void Main(string[] args)
        {
            string str1 = "";
            string str2 = "";
            string szLua = @"
require('rex_pcre')
function OnCodeSenseEx(szPreWord, nPos, nIsDelete, szSetting)
    return '(' .. szPreWord .. ',' ..
            nPos .. ',' ..
            nIsDelete .. ',' ..        
            szSetting ..    
            ')' ;
end
";
            LuaRuntime.LuaInit(szLua, ref str1);
            Console.WriteLine(str1);
            LuaRuntime.LuaRun("OnCodeSenseEx", "pl", 2, 0, "setting_text", ref str2);
            Console.WriteLine(str2);
            Console.ReadKey();
        }
    }

    public static class LuaRuntime
    {
        [DllImport("LuaRuntime.dll", EntryPoint = "LuaInit", CharSet = CharSet.Ansi)]
        public static extern bool LuaInit(string strCode, ref string szOutput);

        [DllImport("LuaRuntime.dll", EntryPoint = "LuaRun" , CharSet = CharSet.Ansi)]
        public static extern bool LuaRun(string szFuncName, string szCode, int nPos, int nIsDelete, string szSetting, ref string szOutput);
    };
}
