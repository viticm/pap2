using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.IO;
using System.Data.SqlClient;
using System.Windows.Forms;
using DataBaseIO;

namespace QueryContextData
{
    class Program
    {
        static void Main(string[] args)
        {
            Helper.ConnectionString = "Server = jx3web; DataBase = s3design_rtm; Uid = s3design; Password = davidbowie;";
            DataBaseManager dbm = DataBaseManager.GetDataBaseManager();
            dbm.Init(Helper.ConnectionString);
            SqlConnection Conn = dbm.conn;

            string strSql = "select * from sys_script_lsfile";
            DataTable lsTable = dbm.GetDataTable(strSql);

            lsTable.Columns.Add("context");

            strSql = "select id, path, data from sys_script_script";
            DataTable ScriptTable = dbm.GetDataTable(strSql);
            string strContextTable = string.Empty;
            int nContextID = -1;
            string strContext = string.Empty;
            bool bNewContext = true;
            Console.Write("成功获取数据库数据...\n开始查询上下文关系...\n");
            foreach (DataRow row in ScriptTable.Rows)
            {
                string ScriptString = row["data"].ToString();
                if (ScriptString != "")
                {
                    ScriptString = Encoding.UTF8.GetString((byte[])row["data"]);
                }
                string strPath = row["path"].ToString();
                string PathKey = "\\Map\\";
                int nIndex = strPath.IndexOf(PathKey);
                if (nIndex == -1)
                {
                    continue;
                }
                else
                {
                    nIndex += PathKey.Length;
                }
                //Path = Path.Substring(nIndex, Path.Length - nIndex);

                //查找上下文相关
                string classification = GetClassification(strPath);

                string tableName = "SCENE_STRING_TABLE";

                if (!IsScriptInMap(strPath) && !ScriptString.Contains("SCENE_STRING_TABLE")) // 兼容旧格式
                {
                    tableName = string.Format("{0}_STRING_TABLE", classification.ToUpper());
                }

                for (int i = 0; ; )
                {
                    if (bNewContext)
                    {
                        if (nContextID >= 0)
                        {
                            strContext = strContext + "\t" + strPath.Substring(nIndex, strPath.Length - nIndex - 4);
                            strContextTable = strContextTable + strContext + "\r\n";
                        }
                        nContextID++;
                        strContext = nContextID.ToString() + "\t";

                        bNewContext = false;
                    }

                    if (ScriptString == "")
                    {
                        break;
                    }

                    i = ScriptString.IndexOf("_STRING_TABLE[", i + 1);
                    if (i == - 1)
                    {
                        break;
                    }
                    else 
                    {
                        i += "_STRING_TABLE[".Length;
                    }
                    int nEndIndex0 = ScriptString.IndexOf(")", i + 1);
                    int nEndIndex1 = ScriptString.IndexOf(",", i + 1);
                    int nNextIndex = ScriptString.IndexOf("_STRING_TABLE[", i + 1);

                    if ((nEndIndex0 >= 0 || nEndIndex1 >= 0)
                        && nNextIndex >= 0)
                    {
                        if (nEndIndex0 < nNextIndex || nEndIndex1 < nNextIndex)
                        {
                            //新的上下文
                            bNewContext = true;

                        }
                    }
                    else if ((nEndIndex0 >= 0 || nEndIndex1 >= 0) || nNextIndex < 0)
                    {
                        bNewContext = true;
                    }
                    else
                    {
                        MessageBox.Show("ASSERT!");
                    }


                    string strID = ScriptString.Substring(i, ScriptString.IndexOf("]", i) - i);

                    DataRow dr = lsTable.Rows.Find(strID);
                    string strContextColumn = dr["context"].ToString();
                    //if (strContextColumn != "")
                    //{
                    //    ;
                    //}
                    strContextColumn = strContextColumn + nContextID + ",";
                    dr["context"] = strContextColumn;
                    //strID = ScriptString.Substring(i - 10, 30);
                    strContext = strContext + strID + ",";
                }
                   


            }
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
            //lsTable.Rows.Find();
            Console.Write("正在生成数据文件，请等待...\n");
            //int nCount = lsTable.Rows.Count;
            string strFile = string.Empty;
            StringBuilder sb = new StringBuilder();


            foreach (DataRow erow in lsTable.Rows)
            {
                sb.Append(erow["id"].ToString());
                sb.Append('\t');
                sb.Append(erow["content"].ToString());
                sb.Append('\t');
                sb.Append(erow["context"].ToString());
                sb.Append("㊣\r\n");

                //strFile += erow["id"].ToString() + "\t";
                //strFile += erow["content"].ToString() + "\t";
                //strFile += erow["context"].ToString() + "㊣\r\n";
            }

            //strFile += strContextTable;
            sb.Append(strContextTable);
            
            FileFolderHelper.StringToFile(sb.ToString(), Application.StartupPath + "\\ContextData.txt");
            Console.Write("完成...\n");
        }

        private static string GetClassification(string path)
        {
            string[] as_Path = path.Split(new string[] { "\\" }, StringSplitOptions.RemoveEmptyEntries);
            string classification = "未分类";

            if (as_Path.Length >= 3 && as_Path[0] == "scripts" && as_Path[1] != "Map")
            {
                classification = as_Path[1];
            }
            else if (as_Path.Length < 3 || as_Path[0] != "scripts" || as_Path[1] != "Map")
            {
                ;
            }
            else
            {
                classification = as_Path[2];
            }

            return classification;
        }

        private static bool IsScriptInMap(string scriptPath)
        {
            bool scriptInMap = scriptPath.StartsWith(@"scripts\Map");

            return scriptInMap;
        }     
    } 
}

