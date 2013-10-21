using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

using DBManager;

namespace FlowChartScriptConvert
{
    public partial class MainForm : Form
    {
        private DataBaseManager dataBaseManager;
        private enum ConvertState { Normal, BlockRemark };
        private ConvertState convertState = ConvertState.Normal;
        private string scriptFilePath; // 脚本路径
        private string mapName; // 地图名称

        public MainForm()
        {
            InitializeComponent();
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
                MessageBox.Show(string.Format("保存文件{0}时产生IO异常：" + ex.Message, fileName), "保存数据文件",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 转换行文本
        /// </summary>
        /// <param name="line">行文本</param>
        /// <returns>新文本</returns>
        private string ConvertLine(string line)
        {
            string newLine = line;

            CodeConvert codeConvert = CodeConvert.GetCodeConvert();
            codeConvert.ConvertTableName = "stringTable";
            List<string> wordList = codeConvert.GetStringList(line);

            if (wordList.Count > 0) // 需要字符串替换
            {
                DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();

                foreach (string s in wordList)
                {
                    if (!s.Contains(",") && !line.Contains(string.Format("context, {0}", s))) // 参数类型不用替换
                    {                        
                        string id = dataBaseManager.InsertIntoFlowChartLS(s.Replace("'", "''"), mapName);
                        newLine = newLine.Replace(s, string.Format("{0}[{1}]", codeConvert.ConvertTableName, id));
                    }                    
                }
            }

            return newLine;
        }

        /// <summary>
        /// 保存ls文件
        /// </summary>
        /// <param name="filePath">文件路径</param>
        private void SaveLSFile(string filePath)
        {
            string sqlString = string.Format("SELECT * FROM sys_script_lsfile WHERE classification = '{0}'", mapName);
            DataTable lsTable = dataBaseManager.GetDataTable(sqlString, dataBaseManager.Connection_Jx3web);
            string lsContent = CodeConvert.ConvertLsFileContent(lsTable.Select(string.Format("classification = '{0}'", mapName)));
            SaveDataToFile(filePath, lsContent);                        
        }

        /// <summary>
        /// 开始转换
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            scriptFilePath = textBoxX1.Text;
            mapName = textBoxX2.Text;

            if (scriptFilePath != "" && mapName != "")
            {
                string content = GetFileContent(scriptFilePath);
                
                if (content != null)
                {
                    dataBaseManager = DataBaseManager.GetDataBaseManager();

                    StringBuilder newContent = new StringBuilder();
                    string[] lines = content.Split(new string[] { "\r\n" }, StringSplitOptions.None);

                    // 增加加载字符串table的调用
                    string includeLsFile = string.Format("Include('scripts/Map/{0}/include/scenestrings.ls')", mapName);

                    if (lines[0] != includeLsFile)
                    {
                        newContent.AppendLine(includeLsFile);
                    }

                    int insertIndex = -1;

                    for (int i = 0; i < lines.Length; i++)
                    {
                        string s = lines[i];
                        string line = s.Trim(new char[] { '\t', ' ' });
                        string newLine = null;

                        if (lines[i] == "using('Event')")
                        {
                            if (lines[i + 2] != "local stringTable = SCENE_STRING_TABLE")
                            {
                                insertIndex = i + 2;
                            }
                        }

                        // 插入对字符串表的引用
                        if (i == insertIndex)
                        {
                            newContent.AppendLine("local stringTable = SCENE_STRING_TABLE");
                            newContent.AppendLine();
                        }

                        switch (convertState)
                        {
                            case ConvertState.Normal:
                                {
                                    if (line.StartsWith("--[["))
                                    {
                                        convertState = ConvertState.BlockRemark;
                                        newContent.AppendLine(s);
                                    }
                                    else if (line.StartsWith("--"))
                                    {
                                        newContent.AppendLine(s);
                                    }
                                    else
                                    {                                        
                                        newLine = ConvertLine(s);
                                        newContent.AppendLine(newLine);
                                    }

                                    break;
                                }
                            case ConvertState.BlockRemark:
                                {
                                    if (line.StartsWith("--]]"))
                                    {
                                        convertState = ConvertState.Normal;
                                        newContent.AppendLine(s);
                                    }
                                    else
                                    {
                                        newContent.AppendLine(s);
                                    }

                                    break;
                                }
                        }
                    }

                    SaveDataToFile(scriptFilePath, newContent.ToString());

                    string lsFileFolder = Path.GetDirectoryName(scriptFilePath);
                    lsFileFolder = Path.Combine(lsFileFolder, "include");
                    string lsFilePath = Path.Combine(lsFileFolder, "scenestrings.ls");

                    SaveLSFile(lsFilePath);

                    MessageBox.Show(string.Format("文件：\r\n{0}\r\n{1}\r\n\r\n转换成功！", scriptFilePath, lsFilePath), "脚本转换", 
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                MessageBox.Show("参数不能为空！", "脚本转换", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 退出
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 浏览脚本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Title = "请选择要转换的脚本文件";
            openFileDialog.Filter = "脚本文件|*.lua";

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                textBoxX1.Text = openFileDialog.FileName;
                string[] dataArray = openFileDialog.FileName.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);

                if (dataArray.Length > 3)
                {
                    textBoxX2.Text = dataArray[dataArray.Length - 2];                    
                }
            }
        }
    }
}