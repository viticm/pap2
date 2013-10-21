using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
namespace ExecLua
{
    class Program
    {
        static void Main(string[] args)
        {

            LuaBuilder lb1;
            string str = "";
            if (args.Length > 0)
            {
                str = args[0];
                LuaBuilder lb = new LuaBuilder(str);
                lb.Run();
            }
            else
            {
                Console.WriteLine("没有命令行参数！");
                //lb1 = new LuaBuilder("F:\\策划工具\\Plugins\\LuaCheck\\复件 tmp.lua");
                //lb1.Run();
            }
                return;
        }
    }
}
