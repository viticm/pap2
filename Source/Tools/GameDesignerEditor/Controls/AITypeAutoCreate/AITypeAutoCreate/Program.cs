using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Forms;
using LuaInterface;

namespace AITypeAutoCreate
{
    class Program
    {
        static void Main(string[] args)
        {
            Helper helper = Helper.GetHelper();
            helper.RecordLogText("开始准备合并AIType.tab文件");
            string fileName = Path.Combine(Application.StartupPath, @"config.ini");

            if (File.Exists(fileName))
            {
                string content = helper.GetFileContent(fileName);
                IniStructure iniStructure = IniStructure.ReadIniWithContent(content);

                string rootPath = iniStructure.GetValue("General", "RootDirectory");
                string parameterCountString = iniStructure.GetValue("General", "ParameterCount");
                int parameterCount = int.Parse(parameterCountString);

                // 需要导出一张旧的AIType表作为中间文件
                fileName = Path.Combine(rootPath, @"settings\AIType.tab");
                content = helper.GetFileContent(fileName);
                
                StringBuilder defaultLine = new StringBuilder();
                defaultLine.Append("0\t");

                for (int i = 0; i < parameterCount; i++)
                {
                    defaultLine.Append(string.Format("{0}\t", iniStructure.GetValue("DefaultValue", string.Format("ParameterValue{0}", i))));
                }

                defaultLine.Remove(defaultLine.Length - 1, 1);

                string newContent = CreateNewAITypeContent(content, defaultLine.ToString());
                helper.SaveData(fileName, newContent);

                string mapCountString = iniStructure.GetValue("General", "MapCount");
                int mapCount = int.Parse(mapCountString);                   
                List<string> mapList = new List<string>();

                for (int i = 0; i < mapCount; i++)
                {
                    string mapEnable = iniStructure.GetValue("MapList", string.Format("Enable{0}", i));

                    if (mapEnable == "1")
                    {
                        mapList.Add(iniStructure.GetValue("MapList", string.Format("MapName{0}", i)));
                    }
                }

                try
                {
                    Lua lua = new Lua(); // lua虚拟机
                    string luaFile = Path.Combine(Application.StartupPath, "exportScript.lua");
                    lua.DoFile(luaFile);

                    LuaFunction function = lua.GetFunction("ExportAIData");
                    string filePath = Path.Combine(rootPath, @"settings\NpcTemplate.tab");
                    function.Call(filePath, mapList);

                    helper.RecordLogText("AIType.tab合并完成");
                }
                catch (Exception ex)
                {
                    helper.RecordLogText("合并AIType文件时产生异常：" + ex.Message);
                }               

                fileName = Path.Combine(Application.StartupPath, "log.txt");
                helper.SaveData(fileName, helper.LogText);
            }
            else
            {
                helper.RecordLogText(string.Format("配置文件{0}不存在", fileName));
            }            
        }
       
        private static string CreateNewAITypeContent(string content, string defaultLine)
        {
            StringBuilder newContent = new StringBuilder();
            bool templateAIType = true;
            string[] lines = content.Split(new string[]{ "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
            
            for (int i = 0; i < lines.Length; i++)
            {
                switch (i)
                {
                    case 0:
                        {
                            newContent.AppendLine(lines[i]);
 
                            break;
                        }
                    case 1:
                        {
                            newContent.AppendLine(defaultLine);
                            newContent.AppendLine(lines[i]); 

                            break;
                        }
                    default:
                        {
                            if (templateAIType)
                            {
                                string[] fields = lines[i].Split(new char[] { '\t' }, StringSplitOptions.RemoveEmptyEntries);

                                if (int.Parse(fields[0]) < 10000)
                                {
                                    newContent.AppendLine(lines[i]);
                                }
                                else
                                {
                                    templateAIType = false;
                                }
                            }

                            break;
                        }
                }            
            }

            return newContent.ToString();
        }
    }
}
