using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using LuaInterface;
using System.Collections;

namespace GameLuaEditor
{
    public partial class ImportForm : Form
    {
        private string[] as_maps = new string[] {"万花","纯阳", "稻香村", "洛道", "少林", "扬州" };
        //private string[] as_maps = new string[] {"稻香村"};

        public ImportForm()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            button1.Enabled = false;
            foreach(string strMap in as_maps)
            {
                Export(strMap);
            }
            MessageBox.Show("成功");
            button1.Enabled = true;
        }
        private void Export(string strMap)
        {
            string strPath = @"D:\Kingsoft\sword3-products\trunk\client\scripts\Map\" + strMap;
            Lua mLua = new Lua();
            mLua.DoFile(strPath + @"\include\对话.ls");

            string[] as_head = new string[]
            { 
                "COMMON_NPC_STRING",
                "COMMON_DOODAD_STRING",
                "COMMON_ITEM_STRING",
                "COMMON_TRAP_STRING",
                "COMMON_OTHER_STRING",

                "WANHUA_DOODAD_STRING",
                "WANHUA_ITEM_STRING",
                "WANHUA_COMMON_NPC_STRING"
            };           

            //遍历文件
            foreach(FileInfo fi in new DirectoryInfo(strPath).GetFiles("*.*", SearchOption.AllDirectories))
            {
                string strCode = Helper.FileToString(fi.FullName);
                if(fi.Name.ToLower().EndsWith(".lua"))
                {
                    //遍历表名
                    foreach(string strHead in as_head)
                    {
                        LuaTable lt_head = mLua.GetTable(strHead);
                        if (lt_head == null) continue;
                        //遍历某个表
                        foreach (string strKey in lt_head.Keys)
                        {
                            if (lt_head[strKey] is LuaTable)
                            {
                                string strPreDim = "";
                                LuaTable lt = lt_head[strKey] as LuaTable;
                                strPreDim = "LS_" + strKey + " = {";
                                foreach(object substring in lt.Values)
                                {
                                    strPreDim += "\r\n\t\"" + substring.ToString().Replace("\n", "\\n") + "\",";
                                }
                                if(strCode.IndexOf(strHead + "[\"" + strKey + "\"]") != -1)
                                    strCode = strPreDim.TrimEnd(new char[] { ',' }) + "\r\n}\r\n" + strCode;
                                strCode = strCode.Replace("math.random(", "Random(");
                                strCode = strCode.Replace(strHead + "[\"" + strKey + "\"]", "LS_" + strKey);                         
                            }
                            else if(lt_head[strKey] is string)
                            {
                                strCode = strCode.Replace(strHead + "[\"" + strKey + "\"]", "\"" + lt_head[strKey].ToString().Replace("\n", "\\n") + "\"");
                            }
                        }
                    }
                    strCode = strCode.Replace("Include(\"scripts/Map/" + strMap + "/include/对话.ls\");", "");
                }
                else if (fi.Name == "对话.ls")
                {
                    //donothing
                    continue;
                }
                else if (fi.Name.ToLower().EndsWith(".ls"))
                {
                    //对于用户自定义ls，不管了
                }
                else
                {
                    //SVN文件，不导
                    continue;
                }
                
                //转换后的东东存成文件
                string strOutPath = fi.FullName.Replace(strPath, @"D:\xxx\" + strMap);
                Helper.WriteStringToFile(strCode, strOutPath);
            }


        }

        private void button2_Click(object sender, EventArgs e)
        {
            button2.Enabled = false;
            string strR = "";
            DataBaseIO.DataBaseManager dbm = DataBaseIO.DataBaseManager.GetDataBaseManager();
            DataTable dt = dbm.GetScriptInformation();
            foreach (FileInfo fi in new DirectoryInfo(@"d:\xxx").GetFiles("*.*", SearchOption.AllDirectories))
            {
                string strName = fi.FullName.Replace(@"d:\xxx", @"scripts\Map");
                string strCode = Helper.FileToString(fi.FullName);
                if (dt.Select("path='" + strName.ToLower() + "'").Length > 0 ||
                    dt.Select("path='" + strName.ToUpper() + "'").Length > 0 )
                {
                    strR += strName + "\r\n";
                    continue;
                }
                //关键部位！！去掉注释就可以导入了
                //dbm.CreateScriptData(strName);
                //string strView = "":
                //bool bret = dbm.SaveScriptData(strName, strCode, out strView);
                //if(bret)
                //    File.Delete(fi.FullName);
            }
            MessageBox.Show("成功");
            button2.Enabled = true;
        }
    }
}