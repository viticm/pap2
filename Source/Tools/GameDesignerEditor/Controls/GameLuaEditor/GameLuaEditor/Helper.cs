using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Collections;
using System.Data.SqlClient;
using System.Data;
using System.Drawing;
using System.Text.RegularExpressions;

namespace GameLuaEditor
{
    public static class Helper
    {
        public static string ConnectionString = "";
        public static string ClientPath = "";
        public static string GameLuaEditorSetting = "";
        public static string ServerPath = ""; // 服务端所在目录
        public static string ServerIP = "127.0.0.1"; // 服务端的IP地址
        public static int ReceiveMessageDelay = 64; // 接收消息延迟
        public static int ReceiveMessageRetryTime = 3; // 接收消息超时重试次数
        public static int BreakPointWaitCircle = 256; // 调试断点触发检查间隔
        public static Hashtable apiTable = new Hashtable(); // 脚本API数据表
        public static Font ScriptFont = new Font("Sim Sun", 8, FontStyle.Regular); // 脚本的字体
        public static Color ScriptForeColor = Color.Black; // 脚本的颜色

    #region InputBox
        static Form f;
        public static string InputBox(string strDefault, string strTitle)
        {
            f = new Form();
            RichTextBox text1 = new RichTextBox();
            Button buttonOK = new Button();
            Button buttonCancel = new Button();
            

            text1.Text = strDefault;
            if(strDefault.IndexOf(".") != -1)
            {
                text1.SelectionStart = 0;
                text1.SelectionLength = strDefault.LastIndexOf(".");
            }
            text1.Left = 10;
            text1.Top = 10;
            text1.Size = new System.Drawing.Size(410, 22);

            buttonOK.Text = "确定";
            buttonOK.Top = 40;
            buttonOK.Left = 250;

            buttonCancel.Text = "取消";
            buttonCancel.Top = 40;
            buttonCancel.Left = 340;

            f.Controls.Add(text1);
            f.Controls.Add(buttonOK);
            f.Controls.Add(buttonCancel);
            f.AcceptButton = buttonOK;
            f.CancelButton = buttonCancel;
            f.MaximizeBox = false;
            f.MinimizeBox = false;
            f.ShowIcon = false;
            f.ShowInTaskbar = false;
            f.StartPosition = FormStartPosition.CenterScreen;
            buttonOK.Click += new EventHandler(buttonOK_Click);
            buttonCancel.Click += new EventHandler(buttonCancel_Click);
            f.Text = strTitle;
            f.Size = new System.Drawing.Size(430, 100);
            DialogResult dr = f.ShowDialog();
            if(dr == DialogResult.OK )
            {
                return text1.Text;
            }
            else
            {
                return null;
            }
        }

        static void buttonCancel_Click(object sender, EventArgs e)
        {
            f.DialogResult = DialogResult.Cancel;
            f.Visible = false;
        }

        static void buttonOK_Click(object sender, EventArgs e)
        {
            f.DialogResult = DialogResult.OK;
            f.Visible = false;
        }
    #endregion

        /// <summary>
        /// 获取脚本的字符串ID链表
        /// </summary>
        /// <param name="content">脚本内容</param>
        /// <param name="tableName">字符串表名称</param>
        /// <returns>字符串ID链表</returns>
        public static List<string> GetScriptID(string content, string tableName)
        {
            List<string> idList = new List<string>();

            if (content != null)
            {
                string regexPattern = string.Format(@"({0}\[(?<MatchResult>\d*)\])", tableName);
                Regex r = new Regex(regexPattern);
                MatchCollection matches = r.Matches(content);

                foreach (Match match in matches)
                {
                    idList.Add(match.Result("${MatchResult}"));
                }
            }

            return idList;
        }

        /// <summary>
        /// 保存文本到文件
        /// </summary>
        /// <param name="content">文本内容</param>
        /// <param name="fileName">文件名</param>
        private static void StringToFile(string content, string fileName)
        {
            StringToFile(content, fileName, Encoding.GetEncoding("gb2312")); // 解决乱码关键在于Encoding
        }

        /// <summary>
        /// 保存文本到文件
        /// </summary>
        /// <param name="content">文本内容</param>
        /// <param name="fileName">文件名</param>
        public static void WriteStringToFile(string content, string fileName)
        {
            StringToFile(content, fileName); // 解决乱码关键在于Encoding
        }

        /// <summary>
        /// 保存文本文件
        /// </summary>
        /// <param name="content">文本内容</param>
        /// <param name="fileName">文件名</param>
        /// <param name="encoding">编码</param>
        private static void StringToFile(string content, string fileName, Encoding encoding)
        {
            if (!File.Exists(fileName))
            {
                if (fileName.IndexOf("\\") != -1)
                {
                    Directory.CreateDirectory(fileName.Substring(0, fileName.LastIndexOf("\\")));
                    File.Create(fileName).Close();
                }                
            }

            StreamWriter writer = new StreamWriter(fileName, false, encoding);
            writer.Write(content);
            writer.Close();
        }
        
        /// <summary>
        /// 读取文件内容
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <returns>文件内容</returns>
        public static string FileToString(string fileName)
        {
            string content = "";

            if (File.Exists(fileName))
            {
                StreamReader reader = new StreamReader(fileName, Encoding.GetEncoding("gb2312"));
                content = reader.ReadToEnd();
                reader.Close();
            }            

            return content;
        }
        
        /// <summary>
        /// 执行外部命令
        /// </summary>
        /// <param name="cmd">命令内容</param>
        /// <param name="arg">命令参数</param>
        /// <param name="sync">命令消息</param>
        public static void Execute(string cmd, string arg, string sync)
        {
            Process p = new Process();
            p.StartInfo.FileName = cmd;
            p.StartInfo.Arguments = arg;
            p.StartInfo.UseShellExecute = false;
            p.StartInfo.RedirectStandardInput = true;
            p.StartInfo.RedirectStandardOutput = true;
            p.StartInfo.RedirectStandardError = true;
            p.StartInfo.CreateNoWindow = true;
            p.Start();

            if ("false" == sync)
            {
                p.WaitForExit();
                p.Close();
            }
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <param name="connection">sql连接</param>
        /// <returns>数据表</returns>
        public static DataTable GetDataTable(string sqlString, SqlConnection conn)
        {
            SqlDataAdapter adapter = new SqlDataAdapter(sqlString, conn);
            adapter.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet dataSet = new DataSet();
            adapter.Fill(dataSet);
            DataTable table = dataSet.Tables[0];
            return table;
        }

        /// <summary>
        /// 检查进程是否存在
        /// </summary>
        /// <param name="processName">进程名</param>
        /// <param name="checkMultiProcess">是否检查多个同名进程</param>
        /// <returns>进程是否存在</returns>
        public static bool CheckProcessExist(string processName, bool checkMultiProcess)
        {
            bool exist = false;
            int count = 0;

            foreach (System.Diagnostics.Process p in System.Diagnostics.Process.GetProcesses())
            {
                if (p.ProcessName == processName)
                {                                        
                    if (checkMultiProcess)
                    {
                        count++;
                    }
                    else
                    {
                        exist = true;
                        break;
                    }                    
                }
            }

            if (checkMultiProcess)
            {
                if (count > 1)
                {
                    exist = true;
                }
            }

            return exist;
        }
    }
}
