using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;
using System.IO;
using System.Windows.Forms;

namespace UIQuestTableConvert
{
    public class ExportManager
    {
        private SqlConnection conn; // sql连接
        private string rootPath; // 根目录
        private DataTable questTable; // 任务数据表
        private string questUIFileName = "QuestsUI.tab";
        private string questNewFileName = "Quests.tab";
        private string questOriginFileName = "QuestsOrigin.tab";
        private string questClassFileName = "QuestClass.tab";
        private List<string> uiFieldList = new List<string>();
        private List<string> keyFieldList = new List<string>();

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="rootPath">客户端目录</param>
        /// <param name="questTable">任务数据表</param>
        public ExportManager(SqlConnection conn, string rootPath, DataTable questTable)
        {
            this.conn = conn;
            this.rootPath = rootPath;
            this.questTable = questTable;

            Init();
        }

        private void Init()
        {
            keyFieldList.Add("QuestID");
            keyFieldList.Add("QuestName");

            uiFieldList.Add("QuestID");
            uiFieldList.Add("QuestName");
            uiFieldList.Add("Objective");
            uiFieldList.Add("Description");
            uiFieldList.Add("DunningDialogue");
            uiFieldList.Add("UnfinishedDialogue");
            uiFieldList.Add("FinishedDialogue");
            uiFieldList.Add("Difficulty");
            uiFieldList.Add("StartGossipType");
            uiFieldList.Add("StartGossip1");
            uiFieldList.Add("StartGossip2");
            uiFieldList.Add("StartGossip3");
            uiFieldList.Add("StartGossip4");
            uiFieldList.Add("EndGossipType");
            uiFieldList.Add("EndGossip1");
            uiFieldList.Add("EndGossip2");
            uiFieldList.Add("EndGossip3");
            uiFieldList.Add("EndGossip4");
            uiFieldList.Add("QuestValueStr1");
            uiFieldList.Add("QuestValueStr2");
            uiFieldList.Add("QuestValueStr3");
            uiFieldList.Add("QuestValueStr4");
            uiFieldList.Add("QuestValueStr5");
            uiFieldList.Add("QuestValueStr6");
            uiFieldList.Add("QuestValueStr7");
            uiFieldList.Add("QuestValueStr8");
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

        private bool SaveFile(string filePath, string content)
        {
            bool saveSuccess = true;

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
                saveSuccess = false;
            }

            return saveSuccess;
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
            List<string> fileNameList = new List<string>();
            StringBuilder headLine = new StringBuilder();
            string sqlString = "SELECT * FROM tbl_quests";

            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }

            // 导出任务表
            StringBuilder content = new StringBuilder();

            foreach (DataColumn dataColumn in questTable.Columns)
            {
                string columnName = dataColumn.ColumnName;

                if (!uiFieldList.Contains(columnName) || keyFieldList.Contains(columnName))
                {
                    headLine.Append(string.Format("{0}\t", columnName));
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
                    string columnName = dataColumn.ColumnName;

                    if (columnName == "Class") // Class列要特殊处理
                    {
                        line.Append(string.Format("{0}\t", GetClassID(value)));
                    }
                    else if (!uiFieldList.Contains(columnName) || keyFieldList.Contains(columnName))
                    {
                        line.Append(string.Format("{0}\t", ConverValue(value)));
                    }
                }

                line.Remove(line.Length - 1, 1);
                content.AppendLine(line.ToString());
            }

            string filePath = Path.Combine(rootPath, string.Format(@"{0}", questNewFileName));
            bool saveSuccess = SaveFile(filePath, content.ToString());

            if (saveSuccess)
            {
                fileNameList.Add(filePath);
            }

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

            filePath = Path.Combine(rootPath, string.Format(@"{0}", questClassFileName));            
            saveSuccess = SaveFile(filePath, content.ToString());
            
            if (saveSuccess)
            {
                fileNameList.Add(filePath);
            }

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

            filePath = Path.Combine(rootPath, string.Format(@"{0}", questOriginFileName));
            saveSuccess = SaveFile(filePath, content.ToString());

            if (saveSuccess)
            {
                fileNameList.Add(filePath);
            }

            // 导出任务UI表
            headLine = new StringBuilder();
            content = new StringBuilder();

            foreach (DataColumn dataColumn in questTable.Columns)
            {
                string columnName = dataColumn.ColumnName;

                if (uiFieldList.Contains(columnName))
                {
                    headLine.Append(string.Format("{0}\t", columnName));
                }                
            }

            headLine.Remove(headLine.Length - 1, 1);
            content.AppendLine(headLine.ToString());

            foreach (DataRow dataRow in questTable.Rows)
            {
                StringBuilder line = new StringBuilder();

                foreach (DataColumn dataColumn in questTable.Columns)
                {
                    string columnName = dataColumn.ColumnName;

                    if (uiFieldList.Contains(columnName))
                    {
                        string value = dataRow[dataColumn].ToString();
                        line.Append(string.Format("{0}\t", ConverValue(value)));
                    }                    
                }

                line.Remove(line.Length - 1, 1);
                content.AppendLine(line.ToString());
            }

            filePath = Path.Combine(rootPath, string.Format(@"{0}", questUIFileName));
            saveSuccess = SaveFile(filePath, content.ToString());

            if (saveSuccess)
            {
                fileNameList.Add(filePath);
            }

            if (conn.State == ConnectionState.Open)
            {
                conn.Close();
            }

            StringBuilder saveMessage = new StringBuilder();
            saveMessage.AppendLine("成功拆分任务表并生成下面文件：");
            
            foreach (string fileName in fileNameList)
            {
                saveMessage.AppendLine(fileName);
            }

            MessageBox.Show(saveMessage.ToString(), "拆分任务表文件",
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
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
