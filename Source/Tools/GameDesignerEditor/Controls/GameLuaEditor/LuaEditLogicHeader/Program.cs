using System;
using System.Collections.Generic;
using System.Text;
using LuaInterface;

namespace LuaEditLogicHeader
{
    class Program
    {
        static void Main(string[] args)
        {
            HeaderBuilder hb = new HeaderBuilder();
            hb.Init();
            hb.Run();
        }
    }
}
