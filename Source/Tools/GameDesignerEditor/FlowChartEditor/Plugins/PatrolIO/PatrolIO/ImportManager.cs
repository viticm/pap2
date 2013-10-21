using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Data.SqlClient;
using System.Data;

namespace PatrolIO
{
    class ImportManager
    {
        private string[] fields; // 列名数组
        private string[] values; // 行数据数组
        private List<string> valuesList = new List<string>(); // 行数据链表
        private bool loadedFields = false; // 是否已经添加过列名数组 
        private string path; // 文件路径

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="path">文件路径</param
        public ImportManager(string path)
        {
            this.path = path;
        }

        /// <summary>
        /// 列名数组
        /// </summary>
        public string[] Fields
        {
            get
            {
                return fields;
            }
        }

        /// <summary>
        /// 行数据数组
        /// </summary>
        public string[] Values
        {
            get
            {
                return values;
            }
        }

        /// <summary>
        /// 导入路径信息
        /// </summary>
        public bool LoadPatrolFromPath()
        {
            bool loadSucess = true; // 是否导入成功
            string indexFilePath; // 索引文件目录
            string relevateFileName; // 相对路径名
            string tempFolder; // 临时文件夹
            string patrolID; // 路径的ID

            try
            {
                string patrolInfoPath = Path.Combine(path, "settings\\map");
                DirectoryInfo di = new DirectoryInfo(patrolInfoPath);
                foreach(DirectoryInfo map in di.GetDirectories())
                {
                    tempFolder = Path.Combine(patrolInfoPath, map.Name);
                    tempFolder = Path.Combine(tempFolder, "PatrolPath");
                    relevateFileName = "settings\\map\\" + map.Name;
                    relevateFileName += "\\PatrolPath\\";
                    indexFilePath = Path.Combine(tempFolder, "PatrolPathList.tab");

                    FileInfo fi = new FileInfo(indexFilePath);
                    bool firstLine = true; // 是否已经读到第一行

                    if(fi.Exists)
                    {
                        StreamReader sr = new StreamReader(indexFilePath, Encoding.GetEncoding("gb2312"));
                        string text = sr.ReadLine();
                        string[] data;

                        while(text != null)
                        {                        
                            if(!firstLine)
                            {
                                data = text.Split(new char[] { '\t' }, StringSplitOptions.RemoveEmptyEntries);
                                patrolID = data[0];
                                if(data.Length == 4) // 处理新格式巡逻路径索引文件
                                {
                                    data = data[3].Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);  
                                }
                                else // 处理旧格式巡逻路径索引文件
                                {
                                    data = data[1].Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);
                                }
                                string fileName = Path.Combine(tempFolder, data[3]);
                                LoadPatrolInfo(fileName, relevateFileName + data[3], map.Name, patrolID);
                            }
                            else
                            {
                                firstLine = false;
                            }
                            text = sr.ReadLine();
                        }

                        sr.Close();
                    }
                    else
                    {
                        MessageBox.Show(string.Format("地图 {0} 的路径索引文件不存在！", map.Name));
                    }
                }                               
            }
            catch (IOException ex)
            {
                MessageBox.Show("在读取路径文件信息时产生IO异常: " + ex.ToString());
                loadSucess = false;
            }

            values = new string[valuesList.Count + 1];
            values[0] = "empty line";
            for (int i = 0; i < valuesList.Count; i++)
            {
                values[i + 1] = valuesList[i];
            }

            return loadSucess;
        }

        /// <summary>
        /// 从文件中读取数据
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="relevateFileName">相对路径名</param>
        /// <param name="mapName">地图名</param>
        /// <param name="patrolID">路径ID</param>
        private void LoadPatrolInfo(string fileName, string relevateFileName, string mapName, string patrolID)
        {
            string dataPath = Path.Combine(path, string.Format("data\\source\\maps\\{0}\\{0}.Map.Logical", mapName));
            FileInfo dataFile = new FileInfo(dataPath);

            if(dataFile.Exists)
            {
                ImportData importData = new ImportData(dataPath);
                
                FileInfo fi = new FileInfo(fileName);
                if (fi.Exists)
                {
                    try
                    {
                        StreamReader sr = new StreamReader(fileName, Encoding.GetEncoding("gb2312"));
                        string text = sr.ReadLine();
                        List<string> list = new List<string>();
                        string[] data;
                        int index = -1;
                        string appendText;
                        string patrolName;

                        while (text != null)
                        {
                            data = text.Split(new char[] { '\t' }, StringSplitOptions.RemoveEmptyEntries);

                            if (index == -1) // 读入第一行，即列数据
                            {
                                if (!loadedFields) // 还没有写列数组
                                {
                                    fields = new string[5 + data.Length];
                                    fields[0] = "FileName";
                                    fields[1] = "MapName";
                                    fields[2] = "PatrolID";                                    
                                    fields[3] = "PatrolName";
                                    fields[4] = "PatrolPointID";

                                    for (int i = 0; i < data.Length; i++)
                                    {
                                        fields[5 + i] = data[i];
                                    }

                                    loadedFields = true;
                                }
                            }
                            else // 读入行数据
                            {
                                patrolName = importData.GetKeyString(string.Format("WayPointSet{0}", int.Parse(patrolID) - 1), "szName");
                                appendText = string.Format("{0}\t{1}\t{2}\t{3}\t{4}\t", relevateFileName, mapName, patrolID, patrolName, index - 1);                       
                                list.Add(appendText + text);
                            }

                            index++;
                            text = sr.ReadLine();
                        }

                        valuesList.AddRange(list);
                    }
                    catch (IOException ex)
                    {
                        MessageBox.Show("在读取路径文件时产生IO异常: " + ex.ToString());
                    }
                }
            }
            else
            {
                FileInfo fi = new FileInfo(fileName);
                if (fi.Exists)
                {
                    try
                    {
                        StreamReader sr = new StreamReader(fileName, Encoding.GetEncoding("gb2312"));
                        string text = sr.ReadLine();
                        List<string> list = new List<string>();
                        string[] data;
                        int index = -1;
                        string appendText;
                        string patrolName;

                        while (text != null)
                        {
                            data = text.Split(new char[] { '\t' }, StringSplitOptions.RemoveEmptyEntries);

                            if (index == -1) // 读入第一行，即列数据
                            {
                                if (!loadedFields) // 还没有写列数组
                                {
                                    fields = new string[5 + data.Length];
                                    fields[0] = "FileName";
                                    fields[1] = "MapName";
                                    fields[2] = "PatrolID";
                                    fields[3] = "PatrolName";
                                    fields[4] = "PatrolPointID";

                                    for (int i = 0; i < data.Length; i++)
                                    {
                                        fields[5 + i] = data[i];
                                    }

                                    loadedFields = true;
                                }
                            }
                            else // 读入行数据
                            {
                                patrolName = string.Format("路径{0}", index + 1);
                                appendText = string.Format("{0}\t{1}\t{2}\t{3}\t{4}\t", relevateFileName, mapName, patrolID, patrolName, index - 1);
                                list.Add(appendText + text);
                            }

                            index++;
                            text = sr.ReadLine();
                        }

                        valuesList.AddRange(list);
                    }
                    catch (IOException ex)
                    {
                        MessageBox.Show("在读取路径文件时产生IO异常: " + ex.ToString());
                    }
                }
            }            
        } 

        /// <summary>
        /// 获取商店道具表所在的目录
        /// </summary>
        /// <returns>商店道具表所在的目录</returns>
        private string GetLoadPath()
        {
            string resultFolder = null;
            FolderBrowserDialog openFolderDialog1 = new FolderBrowserDialog();
            openFolderDialog1.Description = "请选择客户端所在的目录";
            openFolderDialog1.SelectedPath = path;
            if (openFolderDialog1.ShowDialog() == DialogResult.OK)
            {
                resultFolder = openFolderDialog1.SelectedPath;
            }

            return resultFolder;
        } 
    }
}
