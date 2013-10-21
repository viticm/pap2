using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Windows.Forms;
using System.IO;

namespace ExportUITable
{
    public class ExportManager
    {
        private Dictionary<string, TableInfo> tableDictionary = new Dictionary<string, TableInfo>();
        private char[] commaCharArray = new char[] { ',' };
        private char[] tableCharArray = new char[] { '\t' };
        private char[] lineCharArray = new char[] { '\r', '\n' };
        private string configTableName = "UITableConfig.ini";
        private string rootDir;

        public ExportManager(string rootDir)
        {
            this.rootDir = rootDir;
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            string filePath = Path.Combine(Application.StartupPath, configTableName);
            string content = GetFileContent(filePath);

            if (!string.IsNullOrEmpty(content))
            {
                IniStructure iniStructure = IniStructure.ReadIniWithContent(content);
                string value = iniStructure.GetValue("Main", "TableCount");

                int tableCount = 0;                

                if (!int.TryParse(value, out tableCount))
                {
                    tableCount = 0;
                }

                for (int i = 0; i < tableCount; i++)
                {
                    string sectionName = string.Format("Table{0}", i);
                    string tableName = iniStructure.GetValue(sectionName, "TableName");
                    string tablePath = iniStructure.GetValue(sectionName, "FilePath");
                    string includeFieldInfo = iniStructure.GetValue(sectionName, "IncludeField");
                     
                    string[] fieldArray = includeFieldInfo.Split(commaCharArray, StringSplitOptions.RemoveEmptyEntries);
                    List<string> fieldList = new List<string>(fieldArray);

                    TableInfo tableInfo = new TableInfo(tablePath, fieldList);
                    tableDictionary.Add(tableName, tableInfo);
                }
            }            
        }

        /// <summary>
        /// 获取文件文本内容
        /// </summary>
        /// <param name="filePath">文件路径</param>
        /// <returns>文本内容</returns>
        private string GetFileContent(string filePath)
        {
            string content = null;

            if (File.Exists(filePath))
            {
                StreamReader reader = new StreamReader(filePath, Encoding.GetEncoding("gb2312"));
                content = reader.ReadToEnd();
                reader.Close();
            }

            return content;
        }

        /// <summary>
        /// 将数据写入文件
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <param name="content">文本内容</param>
        private void SaveDataToFile(string fileName, string content)
        {
            try
            {
                string filePath = Path.GetDirectoryName(fileName);
                FileInfo fi = new FileInfo(fileName);
                StreamWriter sw;

                if (!fi.Exists) // 文件不存在
                {
                    if (!Directory.Exists(filePath)) // 目录不存在
                    {
                        Directory.CreateDirectory(filePath); // 先创建目录，再创建文件
                    }

                    FileStream fs = fi.Create();
                    fs.Close();
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }
                else // 文件已经存在
                {
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }

                sw.Write(content);
                sw.Close();
            }
            catch (IOException ex)
            {
                Console.WriteLine(string.Format("保存文件{0}时产生IO异常：{1}", fileName, ex.Message));
            }
        }

        /// <summary>
        /// 导出数据表
        /// </summary>
        /// <param name="tableName">表名</param>
        /// <param name="fields">列数据</param>
        /// <param name="values">行数据</param>
        public void ExportTable(string tableName, ArrayList fields, string[] values)
        {
            if (tableDictionary.ContainsKey(tableName))
            {
                TableInfo tableInfo = tableDictionary[tableName];
                string filePath = Path.Combine(rootDir, tableInfo.FilePath);
                List<string> includeFieldList = tableInfo.IncludeFieldList;

                StringBuilder content = new StringBuilder();
                StringBuilder headLine = new StringBuilder();
                List<int> includeFieldIndexList = new List<int>();

                for (int i = 0; i < fields.Count; i++)
                {
                    string columnName = fields[i].ToString().TrimEnd(lineCharArray);

                    if (includeFieldList.Contains(columnName))
                    {
                        includeFieldIndexList.Add(i);
                        headLine.Append(string.Format("{0}\t", columnName));
                    }
                }

                headLine.Remove(headLine.Length - 1, 1);
                content.AppendLine(headLine.ToString());

                foreach (string s in values)
                {
                    if (!string.IsNullOrEmpty(s))
                    {
                        string[] dataArray = s.Split(tableCharArray);
                        StringBuilder line = new StringBuilder();

                        foreach (int i in includeFieldIndexList)
                        {
                            line.Append(string.Format("{0}\t", dataArray[i]));
                        }

                        line.Remove(line.Length - 1, 1);
                        content.AppendLine(line.ToString()); 
                    }                                       
                }

                SaveDataToFile(filePath, content.ToString());
            }
        }
    }
}
