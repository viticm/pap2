using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.Net;

using DBManager;

namespace FlowChartEditor
{
    class LogManager
    {
        private static LogManager logManager; // 日志管理器对象
        private LogForm logForm; // 日志查看窗体
        private string logTableName = "DiagramLog"; // 日志数据表 


        /// <summary>
        /// 私有构造函数
        /// </summary>
        private LogManager()
        {

        }

        /// <summary>
        /// 获取日志管理器对象
        /// </summary>
        /// <returns>日志管理器对象</returns>
        public static LogManager GetLogManager()
        {
            if (logManager == null)
            {
                logManager = new LogManager();
            }

            return logManager;
        }

        /// <summary>
        /// 保存日志
        /// </summary>
        /// <param name="operation">当前操作</param>
        /// <param name="description">操作描述</param>
        /// <param name="success">是否操作成功</param>
        public void SaveLog(string operation, string description, bool success)
        {
            string hostName = Dns.GetHostName();
            string strSuccess = "0";

            if (success)
            {
                strSuccess = "1";
            }

            string sqlString = string.Format("INSERT INTO {0} ([HostName], [Operation], [OperateTime], [Success], [Description]) VALUES ('{1}', '{2}', GetDate(), {3}, '{4}')",
                                             logTableName, hostName, operation, strSuccess, description);
            DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
            dataBaseManager.ExecuteSqlCommand(sqlString);          
        }

        /// <summary>
        /// 查看日志
        /// </summary>
        public void ViewLog()
        {
            if (logForm == null)
            {
                logForm = new LogForm();
            }

            logForm.Reload();
            logForm.ShowDialog();
        }
    }
}
