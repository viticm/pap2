using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;
using System.IO;

namespace ClientDiff
{
    public partial class Form1 : Form
    {

        public Form1()
        {
            InitializeComponent();
        }

        public Form1(string[] args)
        {
            InitializeComponent();
            if (args.Length == 2) // svn 整合
            {
                CompareFile(args[0], args[1]);
                this.Dispose();
            }
            else if (args.Length == 0) // 批量导出
            {
                string client1 = string.Empty;
                string client2 = string.Empty;
                string files = string.Empty;

                // 读取ini文件
                string strIniFile = Application.StartupPath + "/批量输出.ini";
                StringBuilder sb = new StringBuilder(25600);
                FileFolderHelper.ReadIniFile("main", "client1", "", sb, 255, strIniFile);
                client1 = sb.ToString();
                FileFolderHelper.ReadIniFile("main", "client2", "", sb, 255, strIniFile);
                client2 = sb.ToString();
                FileFolderHelper.ReadIniFile("main", "files", "", sb, 25600, strIniFile);
                files = sb.ToString();
                string[] arrFiles = files.Split(',');
                textOutputCfg.Text = string.Empty;
                foreach (string file in arrFiles)
                {
                    textOutputCfg.Text += file;
                    textOutputCfg.Text += "\r\n";
                }
            }
        }
        
        /// <summary>
        /// 比较稳健
        /// </summary>
        /// <param name="filePath1">源比较文件路径</param>
        /// <param name="filePath2">目标比较文件路径</param>
        public void CompareFile(string filePath1, string filePath2)
        {
            List<string> keyList = GetKeyList(filePath1);
            KeyConfigForm kForm = new KeyConfigForm(keyList);

            if (kForm.ShowDialog() == DialogResult.OK)
            {
                string postfix = "tab";
                string keys = kForm.KeyString;
                string namefield = kForm.DisplayField;
                string rowcondition = kForm.RowCondition;
                string output = string.Empty;
                string outputbasic = string.Empty;
                List<string> diffCols1 = new List<string>();
                List<string> diffCols2 = new List<string>();
                DiffGenerator.DiffGen(filePath1, filePath2, postfix, keys, namefield, rowcondition, ref output, ref outputbasic, ref diffCols1, ref diffCols2);

                // diffView
                FileInfo fi = new FileInfo(filePath1);
                DiffViewer.DiffView(fi.Name, postfix, keys, namefield, output, outputbasic, diffCols1, diffCols2);
            }
        }

        private List<string> GetKeyList(string filePath)
        {
            StreamReader sr = new StreamReader(filePath, Encoding.GetEncoding("gb2312"));
            string headLine = sr.ReadLine();
            sr.Close();

            string[] keys = headLine.TrimEnd(new char[] { '\r' }).Split(new char[] { '\t' }, StringSplitOptions.RemoveEmptyEntries);
            List<string> keyList = new List<string>();
            keyList.AddRange(keys);

            return keyList;
        }
    }
}