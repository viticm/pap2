using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Collections;
using System.Data.SqlClient;
using System.IO;
using System.Windows.Forms;
using System.Diagnostics;

namespace GameLuaEditor
{
    class IntelligentSenseManager
    {
        private static IntelligentSenseManager intelligentSenseManager;
        private Hashtable apiTable = new Hashtable();
        private DataTable classTable = null;
        private DataTable codeTable = null;
        private DataTable returnTable = null;
        private DataTable argumentTable = null;

        private IntelligentSenseManager()
        {

        }

        /// <summary>
        /// 获取智能感知数据管理对象
        /// </summary>
        /// <returns>智能感知数据管理对象</returns>
        public static IntelligentSenseManager GetIntelligentSenseManager()
        {
            if (intelligentSenseManager == null)
            {
                intelligentSenseManager = new IntelligentSenseManager();
                intelligentSenseManager.Init();
            }

            return intelligentSenseManager;
        }

        /// <summary>
        /// API数据
        /// </summary>
        public Hashtable APITable
        {
            get
            {
                return apiTable;
            }
        }

        /// <summary>
        /// 更新API文件
        /// </summary>
        private void UpdateAPIFile()
        {
            bool requireUpdate = true;
            string fileName = Path.Combine(Application.StartupPath, @"Plugins\LuaCheck\API.lua");

            if (File.Exists(fileName))
            {
                DateTime fileWriteTime = File.GetLastWriteTime(fileName);

                if (fileWriteTime.Date == DateTime.Now.Date)
                {
                    requireUpdate = false;
                }
            }

            // 1天只更新一次API数据文件
            if (requireUpdate)
            {
                string connectString = "Server = jx3web; DataBase = SIMDB; Uid = sim; Password = 1234;";
                SqlConnection connection = new SqlConnection(connectString);

                connection.Open();

                string sqlString = "SELECT * FROM SIM_ClassTable";
                classTable = Helper.GetDataTable(sqlString, connection);

                sqlString = "SELECT * FROM SIM_CodeList";
                codeTable = Helper.GetDataTable(sqlString, connection);

                sqlString = "SELECT * FROM SIM_ReturnList";
                returnTable = Helper.GetDataTable(sqlString, connection);

                sqlString = "SELECT * FROM SIM_ArgList";
                argumentTable = Helper.GetDataTable(sqlString, connection);

                connection.Close();

                // 遍历对象
                foreach (DataRow dataRow in classTable.Rows)
                {
                    string classID = dataRow["ClassID"].ToString().Trim();
                    string className = dataRow["ClassName"].ToString().Trim();
                    string classProperty = dataRow["ClassProperty"].ToString();
                    string classFunction = dataRow["ClassFunction"].ToString();

                    Hashtable data = new Hashtable();
                    Hashtable propertyTable = new Hashtable();
                    Hashtable functionTable = new Hashtable();
                    // data["_Info"] = GetInfoTable(classTable, dataRow);

                    if (classProperty == "True")
                    {
                        data["'Property'"] = propertyTable;
                    }

                    if (classFunction == "True")
                    {
                        data["'Function'"] = functionTable;
                    }

                    data["'Name'"] = TranslateNilValue(className, false);
                    data["'IsConst'"] = dataRow["ClassType"].ToString().ToLower();

                    apiTable[string.Format("'{0}'", className)] = data;

                    // 遍历对象的方法和属性
                    DataRow[] rows = codeTable.Select(string.Format("ParentClass = '{0}'", classID));
                    bool constType = (bool)dataRow["ClassType"]; // 常量类型需要特殊处理
                    Hashtable constTable = new Hashtable();

                    for (int i = 0; i < rows.Length; i++)
                    {
                        DataRow row = rows[i];
                        int index = i + 1;
                        string codeID = row["CodeID"].ToString().Trim();
                        string codeName = row["SenseWord"].ToString().Trim();
                        string category = row["Category"].ToString().Trim();
                        string field = row["Field"].ToString().Trim();

                        Hashtable codeData = new Hashtable();
                        // codeData["_Info"] = GetInfoTable(codeTable, row);
                        codeData["'Name'"] = TranslateNilValue(codeName, false);
                        codeData["'Abstract'"] = TranslateNilValue(row["Summary"].ToString(), false);
                        codeData["'Type'"] = "'function'";
                        codeData["'Class'"] = TranslateNilValue(className, false);
                        codeData["'CanServerUse'"] = field.Contains("SERVER").ToString().ToLower();
                        codeData["'CanClientUse'"] = field.Contains("CLIENT").ToString().ToLower();
                        codeData["'Detail'"] = TranslateNilValue(TranslateHtmlCode(row["Info"].ToString()), true);
                        codeData["'Index'"] = index.ToString();

                        if (category == "0") // 是方法
                        {
                            Hashtable paramInfo = GetParameterInfo(codeID);

                            if (paramInfo != null)
                            {
                                codeData["'Param'"] = paramInfo;
                            }
                            else
                            {
                                codeData["'Param'"] = "nil";
                            }

                            Hashtable returnInfo = GetReturnInfo(codeID);

                            if (returnInfo != null)
                            {
                                codeData["'Return'"] = returnInfo;
                            }
                            else
                            {
                                codeData["'Return'"] = "nil";
                            }

                            functionTable[string.Format("'{0}'", codeName)] = codeData;
                        }
                        else // 是属性
                        {
                            propertyTable[string.Format("'{0}'", codeName)] = codeData;
                        }
                    }
                }

                string apiContent = ConstructLuaTable(apiTable);            
                Helper.WriteStringToFile(apiContent, fileName);
            }            
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 初始化智能感知数据
            UpdateAPIFile();
                        
            Process p = new Process();
            p.StartInfo.FileName = Application.StartupPath + "\\LuaEditLogicHeader.exe";
            p.StartInfo.UseShellExecute = false;
            p.StartInfo.CreateNoWindow = true;
            p.Start();
        }

        /// <summary>
        /// 转换空值字符串
        /// </summary>
        /// <param name="value">旧字符串</param>
        /// <param name="multiLine">是否是多行文本</param>
        /// <returns>新字符串</returns>
        private string TranslateNilValue(string value, bool multiLine)
        {
            string newValue = value.Trim();

            if (newValue == "")
            {
                newValue = "nil";
            }
            else
            {                
                if (multiLine)
                {
                    newValue = string.Format("[[{0}]]", newValue.Replace("[", "").Replace("]", ""));
                }
                else
                {
                    newValue = string.Format("'{0}'", newValue.Replace("'", ""));
                }                
            }

            return newValue;
        }        

        /// <summary>
        /// 创建lua数据表
        /// </summary>
        /// <param name="infoTable">哈希数据表</param>
        /// <returns>文本内容</returns>
        private string ConstructLuaTable(Hashtable infoTable)
        {
            StringBuilder content = new StringBuilder();
            int level = 0;
            content.AppendLine("APIInfo = {");
            ConstructLuaTable(infoTable, content, ref level);
            content.AppendLine("}");

            return content.ToString();
        }

        /// <summary>
        /// 创建lua数据表
        /// </summary>
        /// <param name="infoTable">哈希数据表</param>
        /// <param name="content">文本内容</param>
        /// <param name="level">当前层级</param>
        private void ConstructLuaTable(Hashtable infoTable, StringBuilder content, ref int level)
        {
            level++;

            // 自动缩进
            StringBuilder spaceBuilder = new StringBuilder();
            for (int i = 0; i < level; i++)
            {
                spaceBuilder.Append("    ");
            }            

            foreach (string key in infoTable.Keys)
            {
                Hashtable childTable = infoTable[key] as Hashtable;

                if (childTable != null)
                {
                    content.AppendLine(string.Format("{0}[{1}] = {{", spaceBuilder.ToString(), key));
                    ConstructLuaTable(childTable, content, ref level);
                    content.AppendLine(string.Format("{0}}},", spaceBuilder.ToString()));
                }
                else
                {
                    content.AppendLine(string.Format("{0}[{1}] = {2},", spaceBuilder.ToString(), key, infoTable[key] as string));
                }
            }

            level--;
        }

        /// <summary>
        /// 转换html编码
        /// </summary>
        /// <param name="detail">源字符串</param>
        /// <returns>转换后的字符串</returns>
        private string TranslateHtmlCode(string detail)
        {
            string newDetail = detail;

            // 特殊处理换行标记
            newDetail = newDetail.Replace("<P>", "").Replace("<\\P>", "\r\n");
            newDetail = newDetail.Replace("<BR>", "\r\n");           

            // 别的网页标签全部忽略
            StringBuilder conent = new StringBuilder();
            int ignoreLevel = 0;

            foreach (char c in newDetail.ToCharArray())
            {
                switch(c)
                {
                    case '<':
                        {
                            ignoreLevel++;
                            break;
                        }
                    case '>':
                        {
                            ignoreLevel--;
                            break;
                        }
                    default:
                        {
                            if (ignoreLevel == 0)
                            {
                                conent.Append(c);
                            }

                            break;
                        }
                }
            }

            return conent.ToString();
        }

        private string GetReturnType(string returnString)
        {
            string returnType = "Object";

            switch (returnString)
            {
                case "t":
                    {
                        returnType = "Table";
                        break;
                    }
                case "dw":
                    {
                        returnType = "DWord";
                        break;
                    }
                case "nil":
                    {
                        returnType = "Null";
                        break;
                    }
                case "n":
                    {
                        returnType = "Integer";
                        break;
                    }
                case "b":
                    {
                        returnType = "Boolean";
                        break;
                    }
                case "player":
                    {
                        returnType = "Player";
                        break;
                    }
                case "sz":
                    {
                        returnType = "String";
                        break;
                    }
                case "npc":
                    {
                        returnType = "Npc";
                        break;
                    }
                case "doodad":
                    {
                        returnType = "Doodad";
                        break;
                    }
                case "shop":
                    {
                        returnType = "Shop";
                        break;
                    }
                case "scene":
                    {
                        returnType = "Scene";
                        break;
                    }
                case "item":
                    {
                        returnType = "Item";
                        break;
                    }
                case "char":
                    {
                        returnType = "Character";
                        break;
                    }
                case "buff":
                    {
                        returnType = "Buff";
                        break;
                    }
                /* 数据库表内容重复
                case "n":
                    {
                        returnType = "Const";
                        break;
                    }
                */
                case "skill":
                    {
                        returnType = "Skill";
                        break;
                    }
                case "e":
                    {
                        returnType = "Enum";
                        break;
                    }
            }

            return returnType;
        }

        /// <summary>
        /// 获取方法的返回值信息
        /// </summary>
        /// <param name="codeID">方法的ID</param>
        /// <returns>参数信息</returns>
        private Hashtable GetReturnInfo(string codeID)
        {
            Hashtable infoTable = null;

            DataRow[] rows = returnTable.Select(string.Format("CodeID = '{0}'", codeID), "RECID");

            if (rows.Length > 0)
            {
                infoTable = new Hashtable();
            }

            for (int i = 0; i < rows.Length; i++)
            {
                DataRow row = rows[i];
                int index = i + 1;
                Hashtable data = new Hashtable();

                data["'Info'"] = TranslateNilValue(TranslateHtmlCode(row["ReturnInfo"].ToString()), true);
                data["'ReturnType'"] = TranslateNilValue(GetReturnType(row["ReturnClass"].ToString()), false);
                infoTable[index.ToString()] = data;
            }

            return infoTable;
        }

        /// <summary>
        /// 获取方法的参数信息
        /// </summary>
        /// <param name="codeID">方法的ID</param>
        /// <returns>参数信息</returns>
        private Hashtable GetParameterInfo(string codeID)
        {
            Hashtable infoTable = null;

            DataRow[] rows = argumentTable.Select(string.Format("CodeID = '{0}'", codeID, "RECID"));
            
            if (rows.Length > 0)
            {
                infoTable = new Hashtable();
            }

            for (int i = 0; i < rows.Length; i++)
            {
                DataRow row = rows[i];
                int index = i + 1;                
                Hashtable data = new Hashtable();

                data["'Name'"] = TranslateNilValue(row["ArgValue"].ToString(), false);
                data["'Info'"] = TranslateNilValue(row["ArgInfo"].ToString(), true);
                data["'Optional'"] = row["Optional"].ToString().ToLower();
                // data["_Info"] = GetInfoTable(argumentTable, row);
                infoTable[index.ToString()] = data;
            }

            return infoTable;
        }

        /// <summary>
        /// 获取信息数据表
        /// </summary>
        /// <param name="dataTable">数据表</param>
        /// <param name="dataRow">数据行</param>
        /// <returns>信息表</returns>
        private Hashtable GetInfoTable(DataTable dataTable, DataRow dataRow)
        {
            Hashtable infoTable = new Hashtable();

            foreach (DataColumn dataColumn in dataTable.Columns)
            {
                infoTable[dataColumn.ColumnName] = dataRow[dataColumn].ToString();
            }

            return infoTable;
        }
    }
}
