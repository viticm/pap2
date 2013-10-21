using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Collections;
using System.Reflection;

namespace IniFileIO
{
    public class Program
    {
        private List<string> fields = new List<string>(); // 列名链表
        private List<string> values = new List<string>(); // 行数据链表
        private List<string> files = new List<string>(); // 文件名链表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="fields">列名链表</param>
        /// <param name="values">行数据数组</param>
        public Program(ArrayList fields, string[] values)
        {
            int idIndex = 0;
            // 初始化列名链表
            for (int i = 0; i < fields.Count; i++)
            {
                if(fields[i] == null || fields[i].ToString() == "")
                {
                    continue;
                }
                if(fields[i].ToString() != "id\r\n")
                {
                    this.fields.Add(fields[i].ToString());
                }
                else
                {
                    idIndex = i; 
                }
            }
            // 初始化行数据链表
            foreach(string s in values)
            {
                if(s == "")
                {
                    continue;
                }
                string[] data = s.Split('\t');
                string value = "";
                for(int i = 0; i < data.Length; i++)
                {
                    if(i != idIndex)
                    {
                        value = value + data[i] + '\t';
                    }
                }
                value = value.TrimEnd('\t');
                this.values.Add(value);
            }
        }

        /// <summary>
        /// 重构要保存的文件
        /// </summary>
        /// <param name="path">文件路径</param>
        public void RestructFiles(string path)
        {
            string resultFolder = "";
            FolderBrowserDialog openFolderDialog1 = new FolderBrowserDialog();
            openFolderDialog1.Description = "请选择导出文件的保存目录";
            openFolderDialog1.ShowNewFolderButton = true;
            openFolderDialog1.RootFolder = Environment.SpecialFolder.MyComputer;
            if (openFolderDialog1.ShowDialog() == DialogResult.OK)
            {
                resultFolder = openFolderDialog1.SelectedPath + "\\";

                // 计算有效属性的个数
                int attributeNumber = 0; // 有效属性个数
                string[] datas = values[0].Split('\t');
                foreach (string s in datas)
                {
                    if (s == "-1")
                    {
                        attributeNumber++;
                    }
                }
                attributeNumber--;

                List<string> fileNameList = new List<string>(); // 文件名链表
                foreach(string s in values)
                {
                    string[] tempData = s.Split('\t');
                    string tempFileName = tempData[0];
                    if(!fileNameList.Contains(tempFileName))
                    {
                        fileNameList.Add(tempFileName);
                    }
                }
                ChooseFileForm cForm = new ChooseFileForm("请选择要导入的文件", fileNameList);
                DialogResult result = cForm.ShowDialog();
                if(result != DialogResult.OK)
                {
                    return;
                }

                foreach (string s in values)
                {
                    if (s.Contains("-1\t")) // 读取默认数据行
                    {
                        string[] data = s.Split('\t');
                        List<string> list = new List<string>();
                        list.Add("[Main]");
                        if(!fileNameList.Contains(data[0]))
                        {
                            continue;
                        }
                        string fileName = resultFolder + data[0];
                        files.Add(fileName);
                        for (int i = 2; i < fields.Count - attributeNumber; i++)
                        {
                            list.Add(fields[i].ToString() + "=" + data[i]);
                        }
                        list.Add(""); // 添加新行
                        SaveSpecialData(fileName, list); // 保存默认数据行
                    }
                    else // 读取普通数据行
                    {
                        string[] data = s.Split('\t');
                        List<string> list = new List<string>();
                        list.Add("[" + data[1] + "]");
                        if (!fileNameList.Contains(data[0]))
                        {
                            continue;
                        }
                        string fileName = resultFolder + data[0];
                        for (int i = fields.Count - attributeNumber; i < fields.Count; i++)
                        {
                            list.Add(fields[i].ToString() + "=" + data[i]);
                        }
                        list.Add(""); // 添加新行
                        SaveNormalData(fileName, list); // 保存普通数据行
                    }
                }
                string message = "下列文件:\n\n";
                foreach (string s in files)
                {
                    message = message + string.Format("{0}\n", s);
                }
                message = message + "\n\n";
                message = message + "已经成功导出!";
                MessageBox.Show(message);
            }
        }

        /// <summary>
        /// 保存默认数据行
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="values">数据内容</param>
        private void SaveSpecialData(string fileName, List<string> values)
        {
            try
            {
                FileInfo fi = new FileInfo(fileName);
                if(!fi.Exists) // 文件已经存在
                {
                    MessageBox.Show(string.Format("文件{0}不存在,将新建该文件!", fileName));
                    FileStream fs = fi.Create();
                    fs.Close();
                }
                StreamWriter sw = fi.CreateText();
                foreach(string s in values)
                {
                    sw.WriteLine(s);
                }
                sw.Close();
            }
            catch(Exception ex)
            {
                MessageBox.Show(string.Format("对文件{0}进行IO操作时产生异常: " + ex.ToString(), fileName));
            }
        }

        /// <summary>
        /// 保存普通数据行
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="values">数据内容</param>
        private void SaveNormalData(string fileName, List<string> values)
        {
            try
            {
                FileInfo fi = new FileInfo(fileName);
                if (!fi.Exists) // 文件已经存在
                {
                    MessageBox.Show(string.Format("文件{0}不存在!", fileName));
                    return;
                }
                StreamWriter sw = fi.AppendText();
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
