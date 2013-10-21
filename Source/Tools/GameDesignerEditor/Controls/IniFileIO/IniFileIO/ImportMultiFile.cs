using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Forms;

namespace IniFileIO
{
    class ImportMultiFile
    {
        private string rootPath; // 导入根目录
        private string[] fields; // 列数据数组
        private string[] values; // 行数据数组
        private string importPath; // 当前导入文件的目录
        private int beginLine = 1; // 非第一个文件开始读取的行序号

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="rootPath">导入根目录</param>
        public ImportMultiFile(string rootPath)
        {
            this.rootPath = rootPath;
        }

        /// <summary>
        /// 列数据数组属性
        /// </summary>
        public string[] Fields
        {
            get
            {
                return fields;
            }
        }

        /// <summary>
        /// 行数据数组属性
        /// </summary>
        public string[] Values
        {
            get
            {
                return values;
            }
        }

        /// <summary>
        /// 当前导入文件的目录
        /// </summary>
        public string ImportPath
        {
            get
            {
                return importPath;
            }
        }

        /// <summary>
        /// 非第一个文件开始读取的行序号
        /// </summary>
        public int BeginLine
        {
            set
            {
                beginLine = value;
            }
        }

        /// <summary>
        /// 导入文件
        /// </summary>
        /// <param name="fileName">记录文件名的列名</param>
        /// <returns>是否导入成功</returns>
        public bool ImportFiles(string fileName)
        {
            bool success = true; // 是否导入成功
            bool firstFile = true; // 是否正在读取第一个文件

            FolderBrowserDialog openFolderDialog1 = new FolderBrowserDialog();
            openFolderDialog1.Description = "请选择导入文件所在的目录";
            openFolderDialog1.ShowNewFolderButton = false;

            if (rootPath == null)
            {
                openFolderDialog1.RootFolder = Environment.SpecialFolder.MyComputer;
            }
            else
            {
                openFolderDialog1.SelectedPath = rootPath;
            }

            if (openFolderDialog1.ShowDialog() == DialogResult.OK)
            {
                importPath = openFolderDialog1.SelectedPath;
                DirectoryInfo di = new DirectoryInfo(importPath);
                FileInfo[] files = di.GetFiles();
                List<string> data; // 行数据
                string[] headLine; // 保存列数据的第一行
                List<string> valueList = new List<string>(); // 保存行数据的链表
                List<string> fileNameList = new List<string>(); // 准备导入的文件链表

                foreach(FileInfo fi in files)
                {
                    fileNameList.Add(fi.FullName);          
                }

                
                ChooseFileForm cForm = new ChooseFileForm("请选择要导入的文件", fileNameList);
                DialogResult result = cForm.ShowDialog();

                if(result == DialogResult.OK) // 用户确认要导入的文件
                {
                    fileNameList = cForm.FileNameList;
                    foreach(string s in fileNameList)
                    {
                        if (firstFile) // 判断是否正在读取第一个文件
                        {
                            data = ReadFile(s);
                            headLine = data[0].Split(new char[] { '\t' });

                            fields = new string[headLine.Length + 1];
                            for (int i = 0; i < headLine.Length; i++)
                            {
                                fields[i] = headLine[i];
                            }
                            fields[headLine.Length] = fileName;

                            valueList.Add(data[0] + "\t" + fileName);
                            for (int i = 1; i < data.Count; i++)
                            {
                                valueList.Add(data[i] + "\t" + Path.GetFileName(s));
                            }

                            firstFile = false;
                        }
                        else
                        {
                            data = ReadFile(s);

                            for (int i = beginLine; i < data.Count; i++)
                            {
                                valueList.Add(data[i] + "\t" + Path.GetFileName(s));
                            }
                        }
                    }

                    values = new string[valueList.Count];
                    for (int i = 0; i < valueList.Count; i++)
                    {
                        values[i] = valueList[i];
                    }
                }
                else
                {
                    success = false;
                } 
            }
            else
            {
                success = false;
            }

            return success;
        }

        /// <summary>
        /// 获取根文件夹名
        /// </summary>
        /// <param name="fileName">文件全路径名</param>
        /// <returns>根文件夹名</returns>
        private string GetFolderName(string fileName)
        {
            string[] data = fileName.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);
            if(data.Length > 1)
            {
                return data[data.Length - 2];
            }
            else
            {
                return "";
            }
        }

        /// <summary>
        /// 导入文件
        /// </summary>
        /// <param name="folderName">记录文件夹名的列名</param>
        /// <param name="fileName">记录文件名的列名</param>
        /// <returns>是否导入成功</returns>
        public bool ImportFiles(string folderName, string fileName)
        {
            bool success = true; // 是否导入成功
            bool firstFile = true; // 是否正在读取第一个文件

            FolderBrowserDialog openFolderDialog1 = new FolderBrowserDialog();
            openFolderDialog1.Description = "请选择导入文件所在的目录";
            openFolderDialog1.ShowNewFolderButton = false;

            if (rootPath == null)
            {
                openFolderDialog1.RootFolder = Environment.SpecialFolder.MyComputer;
            }
            else
            {
                openFolderDialog1.SelectedPath = rootPath;
            }

            if (openFolderDialog1.ShowDialog() == DialogResult.OK)
            {
                importPath = openFolderDialog1.SelectedPath;
                DirectoryInfo directory = new DirectoryInfo(importPath);
                DirectoryInfo[] folders = directory.GetDirectories();
                List<string> data; // 行数据
                string[] headLine; // 保存列数据的第一行
                List<string> valueList = new List<string>(); // 保存行数据的链表
                List<string> fileNameList = new List<string>(); // 准备导入的文件链表

                foreach (DirectoryInfo di in folders)
                {
                    FileInfo[] files = di.GetFiles();
                    foreach(FileInfo fi in files)
                    {
                        fileNameList.Add(fi.FullName);
                    }  
                }

                ChooseFileForm cForm = new ChooseFileForm("请选择要导入的文件", fileNameList);
                DialogResult result = cForm.ShowDialog();

                if (result == DialogResult.OK) // 用户确认要导入的文件
                {
                    fileNameList = cForm.FileNameList;
                    foreach (string s in fileNameList)
                    {
                        if (firstFile) // 判断是否正在读取第一个文件
                        {
                            data = ReadFile(s);
                            headLine = data[0].Split(new char[] { '\t' });

                            fields = new string[headLine.Length + 2];
                            for (int i = 0; i < headLine.Length; i++)
                            {
                                fields[i] = headLine[i];
                            }
                            fields[headLine.Length] = folderName;
                            fields[headLine.Length + 1] = fileName;

                            valueList.Add(data[0] + "\t" + folderName + "\t" + fileName);
                            for (int i = 1; i < data.Count; i++)
                            {
                                valueList.Add(data[i] + "\t" + GetFolderName(s) + "\t" + Path.GetFileName(s));
                            }

                            firstFile = false;
                        }
                        else
                        {
                            data = ReadFile(s);

                            for (int i = beginLine; i < data.Count; i++)
                            {
                                valueList.Add(data[i] + "\t" + GetFolderName(s) + "\t" + Path.GetFileName(s));
                            }
                        }
                    }

                    values = new string[valueList.Count];
                    for (int i = 0; i < valueList.Count; i++)
                    {
                        values[i] = valueList[i];
                    }
                }
            }
            else
            {
                success = false;
            }

            return success;
        }

        /// <summary>
        /// 读取文件中的数据
        /// </summary>
        /// <param name="fileName">带全路径的文件名</param>
        /// <returns>文件中的数据链表</returns>
        public List<string> ReadFile(string fileName)
        {
            List<string> dataList = new List<string>(); // 读取的数据链表

            try
            {
                StreamReader sr = new StreamReader(fileName, Encoding.GetEncoding("gb2312"));
                string text = sr.ReadLine();

                while(text != null)
                {                   
                    dataList.Add(text);
                    text = sr.ReadLine();
                }
            }
            catch(IOException ex)
            {
                MessageBox.Show(string.Format("在对文件{0}进行读取操作是产生IO异常: ", fileName) + ex.ToString());
            }

            return dataList;
        }
    }
}
