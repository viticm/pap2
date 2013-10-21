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

namespace QueryContextData
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

        // 保存string到文件
        private static void StringToFile(string strContent, string strFileName)
        {
            StringToFile(strContent, strFileName, Encoding.GetEncoding("gb2312")); // 解决乱码关键在于Encoding
        }

        public static void WriteStringToFile(string strContent, string strFileName)
        {
            StringToFile(strContent, strFileName); // 解决乱码关键在于Encoding
        }

        private static void StringToFile(string strContent, string strFileName, Encoding encoding)
        {
            if (!File.Exists(strFileName))
            {
                if (strFileName.IndexOf("\\") == -1) return;
                Directory.CreateDirectory(strFileName.Substring(0, strFileName.LastIndexOf("\\")));
                File.Create(strFileName).Close();
            }

            StreamWriter writer = new StreamWriter(strFileName, false, encoding);
            writer.Write(strContent);
            writer.Close();
        }

        // 读取文件到String
        public static string FileToString(string strFileName)
        {
            if (!File.Exists(strFileName))
            {
                return "";
            }

            StreamReader reader = new StreamReader(strFileName, Encoding.GetEncoding("gb2312"));
            string strRet = reader.ReadToEnd();
            reader.Close();

            return strRet;
        }

        // 执行外部命令
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
            SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable tb = ds.Tables[0];
            return tb;
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
