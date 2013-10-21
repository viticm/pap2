using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.IO;
using System.Windows.Forms;

namespace IniFileIO
{
    class ExportMultiFile
    {
        private ArrayList fields; // 列名链表
        private string[] values; // 数据行数组
        private List<string> excludeFields = new List<string>(); // 不导出的列名
        private List<int> excludeIndexList = new List<int>(); // 不导出的序号链表
        private List<string> includeFields = new List<string>(); // 导出的列名
        private List<int> includeIndexList = new List<int>(); // 导出的序号链表        

        private string path = null; // 文件保存路径
        private List<string> exportedFiles = new List<string>(); // 已经导出的文件名列表
        private string exportPath; // 导出文件的路径

        private bool autoExport = false; // 是否自动导出 

        // 导出的类型
        private enum ExportType { None, Exclude, Include };
        private ExportType exportType = ExportType.None;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="fields">列名链表</param>
        /// <param name="values">数据行数组</param>
        public ExportMultiFile(ArrayList fields, string[] values)
        {
            this.fields = fields;
            this.values = values;
        }

        /// <summary>
        /// 是否自动导出
        /// </summary>
        public bool AutoExport
        {
            set
            {
                autoExport = value;
            }
        }

        /// <summary>
        /// 导出文件的路径属性
        /// </summary>
        public string ExportPath
        {
            get
            {
                return exportPath;
            }
        }

        /// <summary>
        /// 设置不导出的列
        /// </summary>
        /// <param name="s">不导出的列名</param>
        public void AddExcludeFields(string s)
        {
            excludeFields.Add(s);
            exportType = ExportType.Exclude;            
        }

        /// <summary>
        /// 设置导出的列
        /// </summary>
        /// <param name="s">导出的列名</param>
        public void AddIncludeFields(string s)
        {
            includeFields.Add(s);
            exportType = ExportType.Include;
        }

        /// <summary>
        /// 设置文件保存路径
        /// </summary>
        /// <param name="path">文件保存路径</param>
        public void SetPath(string path)
        {
            this.path = path;
            this.exportPath = path;
        }

        /// <summary>
        /// 导出文件
        /// </summary>
        /// <param name="folderName">包含文件夹名的列</param>
        /// <param name="fileName">包含文件名的列</param>
        /// <returns>是否导出成功</returns>
        public bool ExportFiles(string folderName, string fileName)
        {
            Hashtable fileTable = new Hashtable();
            string resultFolder = "";

            FolderBrowserDialog openFolderDialog1 = new FolderBrowserDialog();
            openFolderDialog1.Description = "请选择导出文件的保存目录";
            openFolderDialog1.ShowNewFolderButton = true;
            if (path == null)
            {
                openFolderDialog1.RootFolder = Environment.SpecialFolder.MyComputer;
            }
            else
            {
                openFolderDialog1.SelectedPath = path;
            }

            if (openFolderDialog1.ShowDialog() == DialogResult.OK)
            {
                resultFolder = openFolderDialog1.SelectedPath + "\\";
                exportPath = openFolderDialog1.SelectedPath;

                string headline = string.Empty;
                int fileNameIndex = -1;
                int folderNameIndex = -1;

                for (int i = 0; i < fields.Count; i++)
                {
                    string field = (fields[i] as string).TrimEnd(new char[] { '\r', '\n' });
                    if (excludeFields.Contains(field))
                    {
                        excludeIndexList.Add(i);
                    }
                    else
                    {
                        headline = headline + field + "\t";
                    }

                    if (field == fileName)
                    {
                        fileNameIndex = i;
                    }
                    else if(field == folderName)
                    {
                        folderNameIndex = i;
                    }
                }

                if (fileNameIndex == -1 || folderNameIndex == -1) // 没有找到导出文件名所在列
                {
                    MessageBox.Show("没有找到导出文件夹和文件名所在列!");
                    return false;
                }

                headline = headline.TrimEnd(new char[] { '\r', '\n', '\t' }); // 去掉冗余信息

                foreach (string s in values) // 添加所有数据行到hash表中
                {
                    if (s == "") // 忽略空行
                    {
                        continue;
                    }
                    string[] data = s.Split('\t');
                    string fullPath = Path.Combine(resultFolder, Path.Combine(data[folderNameIndex], data[fileNameIndex]));

                    if (fileTable[fullPath] != null)
                    {
                        List<string> lines = fileTable[fullPath] as List<string>;
                        string t = string.Empty;
                        for (int i = 0; i < data.Length; i++)
                        {
                            if (!excludeIndexList.Contains(i))
                            {
                                t = t + data[i] + "\t";
                            }
                        }
                        //lines.Add(t.TrimEnd('\t'));
                        lines.Add(t.Substring(0, t.Length - 1));
                    }
                    else
                    {
                        List<string> lines = new List<string>();
                        lines.Add(headline);
                        string t = string.Empty;
                        for (int i = 0; i < data.Length; i++)
                        {
                            if (!excludeIndexList.Contains(i))
                            {
                                t = t + data[i] + "\t";
                            }
                        }
                        //lines.Add(t.TrimEnd('\t'));
                        lines.Add(t.Substring(0, t.Length - 1));
                        fileTable[fullPath] = lines;
                    }
                }

                List<string> fileNameList = new List<string>();
                foreach (string s in fileTable.Keys)
                {
                    fileNameList.Add(s);
                }

                // 选择要导出的文件
                ChooseFileForm cForm = new ChooseFileForm("请选择要导出的文件", fileNameList);
                DialogResult result = cForm.ShowDialog();
                if (result != DialogResult.OK)
                {
                    return false;
                }

                // 导出文件
                foreach (string s in fileTable.Keys)
                {
                    if (fileNameList.Contains(s))
                    {
                        List<string> lines = fileTable[s] as List<string>;
                        SaveData(s, lines);
                    }
                }

                // 显示导出文件列表
                string message = "下列文件:\n\n";
                foreach (string s in fileNameList)
                {
                    message = message + string.Format("{0}.tab\n", s);
                }
                message = message + "\n\n";
                message = message + "已经成功导出!";
                MessageBox.Show(message);
            }
 
            return true;             
        }

        /// <summary>
        /// 导出文件
        /// </summary>
        /// <param name="fileName">包含文件名的列</param>
        /// <returns>是否导出成功</returns>
        public bool ExportFiles(string fileName)
        {
            Hashtable fileTable = new Hashtable();

            if (exportType == ExportType.None)
            {
                MessageBox.Show("没有设置导出数据的类型！", "导出数据",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            if (!autoExport)
            {
                FolderBrowserDialog openFolderDialog = new FolderBrowserDialog();
                openFolderDialog.Description = "请选择导出文件的保存目录";
                openFolderDialog.ShowNewFolderButton = true;
                
                if (string.IsNullOrEmpty(path))
                {
                    openFolderDialog.RootFolder = Environment.SpecialFolder.MyComputer;
                }
                else
                {
                    openFolderDialog.SelectedPath = path;
                }

                if (openFolderDialog.ShowDialog() == DialogResult.OK)
                {
                    path = openFolderDialog.SelectedPath;
                }
                else
                {
                    return false;
                }
            }                                      

            StringBuilder headLine = new StringBuilder();
            int fileNameIndex = -1;

            for (int i = 0; i < fields.Count; i++)
            {
                string field = (fields[i] as string).TrimEnd(new char[] { '\r', '\n' });

                switch (exportType)
                {
                    case ExportType.Exclude:
                        {
                            if (excludeFields.Contains(field))
                            {
                                excludeIndexList.Add(i);
                            }
                            else
                            {                                    
                                headLine.Append(string.Format("{0}\t", field));
                            }

                            break;
                        }
                    case ExportType.Include:
                        {
                            if (includeFields.Contains(field))
                            {
                                includeIndexList.Add(i);
                                headLine.Append(string.Format("{0}\t", field));
                            }

                            break;
                        }
                }

                if (field == fileName)
                {
                    fileNameIndex = i;
                }
            }

            if (fileNameIndex == -1) // 没有找到导出文件名所在列
            {
                MessageBox.Show("没有找到导出文件名所在列！");
                return false;
            }

            headLine.Remove(headLine.Length - 1, 1);                

            foreach (string s in values) // 添加所有数据行到hash表中
            {
                if (string.IsNullOrEmpty(s)) // 忽略空行
                {
                    continue;
                }

                string[] data = s.Split('\t');

                if (fileTable[Path.Combine(path, data[fileNameIndex])] != null)
                {
                    List<string> lines = fileTable[Path.Combine(path, data[fileNameIndex])] as List<string>;
                    StringBuilder line = new StringBuilder();

                    for (int i = 0; i < data.Length; i++)
                    {
                        switch (exportType)
                        {
                            case ExportType.Exclude:
                                {
                                    if (!excludeIndexList.Contains(i))
                                    {                                            
                                        line.Append(string.Format("{0}\t", data[i]));
                                    }

                                    break;
                                }
                            case ExportType.Include:
                                {
                                    if (includeIndexList.Contains(i))
                                    {
                                        line.Append(string.Format("{0}\t", data[i]));
                                    }

                                    break;
                                }
                        }                            
                    }

                    line.Remove(line.Length - 1, 1);
                    lines.Add(line.ToString());
                }
                else
                {
                    List<string> lines = new List<string>();
                    lines.Add(headLine.ToString());
                    StringBuilder line = new StringBuilder();

                    for (int i = 0; i < data.Length; i++)
                    {
                        switch (exportType)
                        {
                            case ExportType.Exclude:
                                {
                                    if (!excludeIndexList.Contains(i))
                                    {
                                        line.Append(string.Format("{0}\t", data[i]));
                                    }

                                    break;
                                }
                            case ExportType.Include:
                                {
                                    if (includeIndexList.Contains(i))
                                    {
                                        line.Append(string.Format("{0}\t", data[i]));
                                    }

                                    break;
                                }
                        }
                    }

                    line.Remove(line.Length - 1, 1);
                    lines.Add(line.ToString());
                    fileTable[Path.Combine(path, data[fileNameIndex])] = lines;
                }
            }

            List<string> fileNameList = new List<string>();

            foreach (string s in fileTable.Keys)
            {
                fileNameList.Add(s);
            }

            // 选择要导出的文件
            if (!autoExport)
            {
                ChooseFileForm cForm = new ChooseFileForm("请选择要导出的文件", fileNameList);
                DialogResult result = cForm.ShowDialog();

                if (result != DialogResult.OK)
                {
                    return false;
                }
            }            

            // 导出文件
            foreach (string s in fileTable.Keys)
            {
                if (fileNameList.Contains(s))
                {
                    List<string> lines = fileTable[s] as List<string>;
                    SaveData(s, lines);
                }
            }

            // 显示导出文件列表
            string message = "下列文件:\n\n";

            foreach (string s in fileNameList)
            {
                message = message + string.Format("{0}\n", s);
            }

            message = message + "\n\n";
            message = message + "已经成功导出!";
            MessageBox.Show(message);            

            return true;
        }

        /// <summary>
        /// 保存默认数据行
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="values">数据内容</param>
        private void SaveData(string fileName, List<string> values)
        {
            try
            {
                FileInfo fi = new FileInfo(fileName);
                StreamWriter sw;

                if (!fi.Exists) // 文件不存在
                {                    
                    string path = Path.GetDirectoryName(fileName); // 获取目录
                    if(!Directory.Exists(path)) // 目录不存在
                    {
                        Directory.CreateDirectory(path); // 先创建目录，再创建文件
                    }
                    FileStream fs = fi.Create();
                    fs.Close();
                    // sw = fi.CreateText(); // 新建文本
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }
                else
                {
                    if(!exportedFiles.Contains(fileName)) // 第一次写文件
                    {
                        // sw = fi.CreateText();
                        sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                        exportedFiles.Add(fileName);
                    }
                    else // 添加新行
                    {
                        // sw = fi.AppendText(); // 添加文本
                        sw = new StreamWriter(fileName, true, Encoding.GetEncoding("gb2312"));
                    }              
                }
                 
                foreach (string s in values)
                {
                    sw.WriteLine(s);
                }
                sw.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("对文件{0}进行IO操作时产生异常: " + ex.ToString(), fileName));
            }
        }
    }
}
