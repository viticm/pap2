using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;
using System.IO;

namespace CustomLoadTree
{
    public class ExportManager
    {
        private SqlConnection conn; // sql连接
        private string rootPath; // 根目录
        private string uiQuestFileName = "Quests.tab";
        private string questOriginFileName = "QuestsOrigin.tab";
        private string uiClassRelationFileName = "QuestClass.tab";
        private List<string> excludeQuestFieldList = new List<string>();

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="rootPath">客户端目录</param>
        public ExportManager(SqlConnection conn, string rootPath)
        {
            this.conn = conn;
            this.rootPath = rootPath;

            Init();
        }

        private void Init()
        {
            // excludeQuestFieldList.Add("QuestName");
            excludeQuestFieldList.Add("Objective");
            excludeQuestFieldList.Add("Description");
            excludeQuestFieldList.Add("DunningDialogue");
            excludeQuestFieldList.Add("UnfinishedDialogue");
            excludeQuestFieldList.Add("FinishedDialogue");
            excludeQuestFieldList.Add("Difficulty");
            excludeQuestFieldList.Add("StartGossipType");
            excludeQuestFieldList.Add("StartGossip1");
            excludeQuestFieldList.Add("StartGossip2");
            excludeQuestFieldList.Add("StartGossip3");
            excludeQuestFieldList.Add("StartGossip4");
            excludeQuestFieldList.Add("EndGossipType");
            excludeQuestFieldList.Add("EndGossip1");
            excludeQuestFieldList.Add("EndGossip2");
            excludeQuestFieldList.Add("EndGossip3");
            excludeQuestFieldList.Add("EndGossip4");
            excludeQuestFieldList.Add("QuestValueStr1");
            excludeQuestFieldList.Add("QuestValueStr2");
            excludeQuestFieldList.Add("QuestValueStr3");
            excludeQuestFieldList.Add("QuestValueStr4");
            excludeQuestFieldList.Add("QuestValueStr5");
            excludeQuestFieldList.Add("QuestValueStr6");
            excludeQuestFieldList.Add("QuestValueStr7");
            excludeQuestFieldList.Add("QuestValueStr8");
        }

        private string ConverValue(string value)
        {
            string newValue = value;

            switch (value)
            {
                case "True":
                    {
                        newValue = "1";
                        break;
                    }
                case "False":
                    {
                        newValue = "0";
                        break;
                    }
                default:
                    {
                        break;
                    }
            }

            return newValue;
        }

        private string GetClassID(string className)
        {
            string classID = null;

            string sqlString = string.Format("SELECT ID FROM tbl_quest_class WHERE Name = '{0}'", className);
            SqlCommand cmd = conn.CreateCommand();
            cmd.CommandText = sqlString;
            object executeResult = cmd.ExecuteScalar();

            if (executeResult != null)
            {
                classID = executeResult.ToString();
            }
            else
            {
                sqlString = "SELECT Max(ID) FROM tbl_quest_class";
                cmd.CommandText = sqlString;
                executeResult = cmd.ExecuteScalar();

                if (executeResult != null)
                {
                    string maxID = executeResult.ToString();

                    if (maxID == "")
                    {
                        maxID = "0";
                    }

                    int newID = int.Parse(maxID) + 1;
                    sqlString = string.Format("INSERT INTO tbl_quest_class (ID, Name) VALUES ('{0}', '{1}')",
                                              newID, className);

                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    classID = newID.ToString();
                }
            }

            return classID;
        }

        private void SaveFile(string filePath, string content)
        {
            try
            {
                string fileFolder = Path.GetDirectoryName(filePath);

                if (!File.Exists(filePath)) // 文件不存在
                {
                    if (!Directory.Exists(fileFolder)) // 目录不存在
                    {
                        Directory.CreateDirectory(fileFolder); // 先创建目录，再创建文件
                    }

                    File.Create(filePath).Close();
                }

                StreamWriter sw = new StreamWriter(filePath, false, Encoding.GetEncoding("gb2312"));
                sw.Write(content);
                sw.Close();
            }
            catch (IOException ex)
            {
                ;
            }
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">sql语句</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sqlString)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];

            return tbl;
        }

        /// <summary>
        /// 导出任务文件
        /// </summary>
        private void ExportQuestFile()
        {            
            StringBuilder headLine = new StringBuilder();

            string sqlString = "SELECT * FROM tbl_quests";

            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }

            // 导出任务表
            DataTable questTable = GetDataTable(sqlString);
            StringBuilder content = new StringBuilder();

            foreach (DataColumn dataColumn in questTable.Columns)
            {
                if (!excludeQuestFieldList.Contains(dataColumn.ColumnName))
                {
                    headLine.Append(string.Format("{0}\t", dataColumn.ColumnName));
                }
            }

            headLine.Remove(headLine.Length - 1, 1);
            content.AppendLine(headLine.ToString());

            foreach (DataRow dataRow in questTable.Rows)
            {
                StringBuilder line = new StringBuilder();

                foreach (DataColumn dataColumn in questTable.Columns)
                {
                    string value = dataRow[dataColumn].ToString();

                    if (dataColumn.ColumnName == "Class") // Class列要特殊处理
                    {
                        line.Append(string.Format("{0}\t", GetClassID(value)));
                    }
                    else if (!excludeQuestFieldList.Contains(dataColumn.ColumnName))
                    {
                        line.Append(string.Format("{0}\t", ConverValue(value)));
                    }
                }

                line.Remove(line.Length - 1, 1);
                content.AppendLine(line.ToString());
            }

            string filePath = Path.Combine(rootPath, string.Format(@"settings\{0}", uiQuestFileName));
            SaveFile(filePath, content.ToString());

            // 导出Class表
            sqlString = "SELECT * FROM tbl_quest_class";
            DataTable classTable = GetDataTable(sqlString);
            content = new StringBuilder();

            content.AppendLine("ID\tName");
            content.AppendLine("0\t");

            foreach (DataRow dataRow in classTable.Rows)
            {
                content.AppendLine(string.Format("{0}\t{1}", dataRow["ID"].ToString(), dataRow["Name"].ToString()));
            }

            filePath = Path.Combine(rootPath, string.Format(@"ui\Scheme\Case\{0}", uiClassRelationFileName));            
            SaveFile(filePath, content.ToString());

            // 导出任务原表
            headLine = new StringBuilder();
            content = new StringBuilder();

            foreach (DataColumn dataColumn in questTable.Columns)
            {
                headLine.Append(string.Format("{0}\t", dataColumn.ColumnName));
            }

            headLine.Remove(headLine.Length - 1, 1);
            content.AppendLine(headLine.ToString());

            foreach (DataRow dataRow in questTable.Rows)
            {
                StringBuilder line = new StringBuilder();

                foreach (DataColumn dataColumn in questTable.Columns)
                {
                    string value = dataRow[dataColumn].ToString();

                    line.Append(string.Format("{0}\t", ConverValue(value)));
                }

                line.Remove(line.Length - 1, 1);
                content.AppendLine(line.ToString());
            }

            filePath = Path.Combine(rootPath, string.Format(@"settings\{0}", questOriginFileName));
            SaveFile(filePath, content.ToString());

            if (conn.State == ConnectionState.Open)
            {
                conn.Close();
            }
        }

        /// <summary>
        /// 导出数据表
        /// </summary>
        /// <returns>是否导出成功</returns>
        public bool Export()
        {
            bool exportSuccess = true;

            ExportQuestFile();

            return exportSuccess;
        }
    }
}
