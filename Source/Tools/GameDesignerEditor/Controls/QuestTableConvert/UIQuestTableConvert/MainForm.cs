using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

using System.Data.SqlClient;

using DevComponents.DotNetBar;

namespace UIQuestTableConvert
{
    public partial class MainForm : Office2007Form
    {
        private string connectString = "Server = jx3web; DataBase = s3design_rtm; Uid = s3design; Password = davidbowie;";
        private SqlConnection conn;

        public MainForm()
        {
            InitializeComponent();
            Init();
        }

        private void Init()
        {
            conn = new SqlConnection(connectString);
            folderNameBox.Text = Application.StartupPath;
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
            else
            {
                MessageBox.Show(string.Format("文件{0}不存在！", filePath), "读取数据文件",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            return content;
        }

        /// <summary>
        /// 创建数据表
        /// </summary>
        /// <param name="content">内容</param>
        /// <returns>数据表</returns>
        private DataTable CreateDataTable(string content)
        {
            DataTable table = new DataTable();
            List<string> fieldList = new List<string>();

            bool firstLine = true;
            string[] lines = content.Split(new char[] { '\n' });

            foreach (string s in lines)
            {
                string line = s.TrimEnd(new char[] { '\r' });

                if (line != "") // 跳过空的数据行
                {
                    string[] data = line.Split(new char[] { '\t' });

                    if (firstLine)
                    {
                        firstLine = false;

                        for (int i = 0; i < data.Length; i++)
                        {
                            DataColumn dataColumn = new DataColumn(data[i]);
                            table.Columns.Add(dataColumn);
                            fieldList.Add(data[i]);
                        }
                    }
                    else
                    {
                        DataRow dataRow = table.NewRow();

                        for (int i = 0; i < data.Length; i++)
                        {
                            dataRow[i] = data[i];
                        }

                        table.Rows.Add(dataRow);
                    }
                }

            }

            return table;
        }

        /// <summary>
        /// 选择文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bBrowse_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Title = "请选择任务数据表所在位置";
            openFileDialog.Filter = "任务数据表|*.tab";

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                fileNameBox.Text = openFileDialog.FileName;
            }
        }

        /// <summary>
        /// 开始拆分
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bConvert_Click(object sender, EventArgs e)
        {
            string fileName = fileNameBox.Text;
            string folderName = folderNameBox.Text;

            if (!string.IsNullOrEmpty(fileName) && !string.IsNullOrEmpty(folderName))
            {
                string content = GetFileContent(fileName);

                if (content != null)
                {
                    DataTable questTable = CreateDataTable(content);

                    ExportManager exportManager = new ExportManager(conn, folderName, questTable);
                    exportManager.Export();
                }
            }
            else
            {
                MessageBox.Show("请先确定拆分数据表的参数！", "拆分任务数据表",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 退出程序
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        /// <summary>
        /// 浏览输出目录
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bBrowseFolder_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();
            folderBrowserDialog.SelectedPath = Application.StartupPath;
            folderBrowserDialog.Description = "请选择文件输出目录";

            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                folderNameBox.Text = folderBrowserDialog.SelectedPath;
            }
        }
    }
}