/*****************************************************/
/*          每个数据库对应一个LogHelper              */
/*****************************************************/


using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Data;
using DataTableRemotingCommon;

namespace DataTableRemoting
{

    enum ModifyOperate
    {
        Connect,
        Disconnect,
        AddRow,
        Modify,
        DeleteRow,
        ExecuteCommand,
        SaveTable
    }

    class LogHelper
    {
        private DataTable mLogTable;
        private TableUnitManager mTableUnitManager;
        private string mClientkey;

        private FileStream mFileStream;
        private StreamWriter mFileWriter;
        private int mLineCount = 0;
        private int mLineLimit = 100;
        private string mFileName;
        private object mFileLock = new object();

        private bool[] mConsoleFlag;
        private bool[] mFileFlag;

        private static object mLog2ServerLock = new object();

        public LogHelper(string clientkey)
        {
            mClientkey = clientkey;
            NetClient client = NetClientManager.GetNetClient(clientkey);
            if (client.Database != "monitor")
            {
                mTableUnitManager = TableHelper.GetTableUnitManager(clientkey);
                TableUnit unit = mTableUnitManager.GetTableUnit("sys_remote_log");
                mLogTable = unit.GetTable();
            }

            int length = Convert.ToInt32(LogType.END);
            mConsoleFlag = new bool[length];
            mFileFlag = new bool[length];
            for (int i = 0; i < length; ++i)
            {
                mConsoleFlag[i] = true;
                mFileFlag[i] = true;
            }

        }

        public void LogToTable(string clientkey, string tablename, ModifyOperate operate)
        {
            NetClient client = NetClientManager.GetNetClient(clientkey);
            string hostname = client.HostName;
            lock (mLogTable)
            {
                int index = mLogTable.Rows.Count;
                DataRow row = mLogTable.NewRow();
                row["id"] = index;
                row["hostname"] = hostname;
                row["tablename"] = tablename;
                row["operate"] = operate.ToString();
                row["edittime"] = DateTime.Now;
                mLogTable.Rows.Add(row);
                mTableUnitManager.MarkTableChanged("sys_remote_log");
                mTableUnitManager.SaveTable("sys_remote_log");
            }
        }

        public void SetModel(string clientkey, string model)
        {
            LogMessage(clientkey, model, LogType.NetWork);
            NetClient client = NetClientManager.GetNetClient(clientkey);
            if (client == null)
            {
                string message = string.Format("SetModel {0}: Not Exist {1}", model, clientkey);
                LogServerMessage(message, LogType.Program);
            }
            else
            {
                client.ModelName = model;
            }

        }
        public void LogMessage(string clientkey, string message, LogType type)
        {
            string time = DateTime.Now.ToLongTimeString();

            LogToConsole(clientkey, message, time, type);
            LogToMonitor(clientkey, message,time, type);
            LogToFile(clientkey, message, time, type);
        }

        public static void LogServerMessage(string message, LogType type)
        {
            string time = DateTime.Now.ToLongTimeString();
            LogToConsole(null, message, time, type);
            LogToMonitor(null, message, time, type);
            lock (mLog2ServerLock)
            {
                string format = string.Format("{0} {1}: {2}", time, "Server", message);
                string filename = DateTime.Now.ToLongDateString();
                string filepath = string.Format(".//logs//{0}", "SERVER");
                System.IO.Directory.CreateDirectory(filepath);
                filepath = string.Format("{0}//{1}.txt", filepath, filename);
                FileStream fs = new FileStream(filepath, FileMode.Append, FileAccess.Write);
                StreamWriter sw = new StreamWriter(fs);
                sw.WriteLine(format);
                sw.Flush();
                sw.Close();
            }
            
        }

        private static void LogToConsole(string clientkey, string message, string time, LogType type)
        {
            string hostname = "Server";
            if (clientkey != null)
            {
                NetClient client = NetClientManager.GetNetClient(clientkey);
                if (client != null)
                {
                    hostname = client.HostName;
                }
                else
                {
                    hostname = clientkey;
                }
            }
            string format = string.Format("{0} {1}: {2}", time, hostname, message);

            switch (type)
            {
                case LogType.Modify:
                    Console.ForegroundColor = ConsoleColor.DarkRed;
                    break;
                case LogType.NetWork:
                    Console.ForegroundColor = ConsoleColor.DarkYellow;
                    break;
                case LogType.SQLCommand:
                    Console.ForegroundColor = ConsoleColor.DarkMagenta;
                    break;
                case LogType.GetTableSql:
                    Console.ForegroundColor = ConsoleColor.DarkCyan;
                    break;
                case LogType.Program:
                    Console.ForegroundColor = ConsoleColor.Gray;
                    break;
                case LogType.ReadWriteTableLock:
                    Console.ForegroundColor = ConsoleColor.DarkMagenta;
                    break;
                default:
                    Console.ForegroundColor = ConsoleColor.DarkGreen;
                    break;
            }
            Console.WriteLine(format);
        }

        private static void LogToMonitor(string clientkey, string message, string time, LogType type)
        {
            string hostname = "Server";
            if (clientkey != null)
            {
                NetClient client = NetClientManager.GetNetClient(clientkey);
                if (client != null)
                {
                    hostname = client.HostName;
                }
                else
                {
                    hostname = clientkey;
                }
            }
            string format = string.Format("{0} {1}: {2}", time, hostname, message);
            BroadcastSystem.BroadcastLogToMonitorHandler(format, type);
        }

        private void LogToFile(string clientkey, string message, string time, LogType type)
        {
            lock (mFileLock)
            {
                string hostname = "Server";
                NetClient client = null;
                if (clientkey != null)
                {
                    client = NetClientManager.GetNetClient(clientkey);
                    if (client != null)
                    {
                        hostname = client.HostName;
                    }
                    else
                    {
                        hostname = clientkey;
                    }
                }
                else
                {
                    client = new NetClient(null, 0, null);
                    client.Database = "server";
                }

                string format = string.Format("{0, -12} {1} {2} {3}", type.ToString()+":", time, hostname, message);
                string filename = DateTime.Now.ToLongDateString();
                string filepath = string.Format(".//logs//{0}", client.Database.ToUpper());
                if (filename != mFileName || mFileStream == null || mFileWriter == null)
                {
                    CloseFile();
                    System.IO.Directory.CreateDirectory(filepath);
                    filepath = string.Format("{0}//{1}.txt", filepath, filename);
                    mFileName = filename;
                    mFileStream = new FileStream(filepath, FileMode.Append, FileAccess.Write);
                    mFileWriter = new StreamWriter(mFileStream);
                }
                mFileWriter.WriteLine(format);
                mLineCount++;
                if (mLineCount > mLineLimit)
                {
                    CloseFile();
                }
            }
        }

        private void CloseFile()
        {
            if (mFileWriter != null)
            {
                mFileWriter.Flush();
                mFileWriter.Close();
            }
            if (mFileStream != null)
            {
                mFileStream.Close();
            }
            mFileWriter = null;
            mFileStream = null;
            mLineCount = 0;
        }

        public void Release()
        {
            CloseFile();
        }
    }
}
