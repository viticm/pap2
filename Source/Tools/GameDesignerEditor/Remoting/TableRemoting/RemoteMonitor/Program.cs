using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using DataTableRemotingClient;
using DataTableRemotingCommon;

namespace RemoteMonitor
{
    class Program
    {
        static string mTitle = string.Empty;
        static IMonitorRemoting mMonitor;
        static object mThreadLock = new object();
        static DateTime mTime;

        static void Main(string[] args) 
        {
            DataTableServerProxy.InitServerProxy(AppDomain.CurrentDomain.SetupInformation.ConfigurationFile, "monitor");
            IMonitorRemoting monitor = DataTableServerProxy.GetMonitorRemoting();
            mMonitor = monitor;
            BroadcastSystem broadcast = DataTableServerProxy.GetBroadcastSystem();
            broadcast.AddConnectEventHandler(OnConnect);
            broadcast.AddLogToMonitorHandler(OnLogToMonitor);
            Console.BufferHeight = 8000;
            Console.BufferWidth = 300;
            Console.ForegroundColor = ConsoleColor.DarkYellow;
            Console.WriteLine("Server connect success.");
            mTime = DateTime.Now;
            PrintInfo();
            while (ReadCommand())
            {
                ;
            }
            DataTableServerProxy.DisconnectServer();
        }

        static void OnConnect(object sender, ConnectEventArgs args)
        {
            Console.Title = (sender as NetClient).ClientKey + " " + mTitle;
            mTitle += ". ";
            if (mTitle.Length > 10)
            {
                mTitle = string.Empty;
            }
        }

        static void OnLogToMonitor(string message, LogType type)
        {
            lock (mThreadLock)
            {
                switch (type)
                {
                    case LogType.Modify:
                        Console.ForegroundColor = ConsoleColor.DarkRed;
                        break;
                    case LogType.NetWork:
                        Console.ForegroundColor = ConsoleColor.DarkYellow;
                        DateTime time = DateTime.Now;
                        TimeSpan span = time.Subtract(mTime);
                        if (span.Minutes > 60)
                            PrintInfo();
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
                Console.WriteLine(message);
                Console.ForegroundColor = ConsoleColor.DarkYellow;
            }
        }

        static void PrintInfo()
        {
            lock (mThreadLock)
            {
                Console.ForegroundColor = ConsoleColor.DarkYellow;
                Console.WriteLine("______________________________________________________________");

                string[] dbnames = mMonitor.GetDBNames();

                string message = string.Format("There are {0} databases in server.", dbnames.Length - 1);
                Console.WriteLine(message);

                int index = 0;
                foreach (string db in dbnames)
                {
                    if (db == "monitor")
                    {
                        continue;
                    }
                    string[] tables = mMonitor.GetTablenames(db);
                    Console.ForegroundColor = ConsoleColor.DarkRed;
                    message = string.Format("{0} have {1} tables in server buffer;", db, tables.Length);
                    Console.WriteLine(message);
                    Console.ForegroundColor = ConsoleColor.DarkGreen;
                    index = 0;
                    foreach (string tbl in tables)
                    {
                        int count = mMonitor.GetTableRowCount(tbl, db);

                        message = string.Format("   {2}.Table: {0} have {1} rows.", tbl, count, index);
                        Console.WriteLine(message);
                        index++;
                    }
                    
                    Console.WriteLine();
                }

                Console.ForegroundColor = ConsoleColor.DarkRed;
                string[] clientkeys = mMonitor.GetNetClientKeys();
                message = string.Format("There are {0} clients online.", clientkeys.Length);
                Console.WriteLine(message);
                index = 0;

                Console.ForegroundColor = ConsoleColor.DarkYellow;
                foreach (string key in clientkeys)
                {
                    NetClient client = mMonitor.GetNetClient(key);
                    string info = string.Format("   {0}. {1}: {2} {3}", index, client.HostName, client.Database, client.ModelName);
                    index++;
                    Console.WriteLine(info);
                }

                Console.WriteLine("______________________________________________________________");
            }

        }

        static bool ReadCommand()
        {
            string line = Console.ReadLine();
            bool bRet = true;
            if (line == "exit")
            {
                bRet = false;
            }
            else if (line == "print info")
            {
                PrintInfo();
            }
            else
            {
                try
                {
                    string[] args = line.Split(new char[] { ' ' });
                    string result = mMonitor.DoCommand(args).ToString();
                    if (result != null)
                    {
                        Console.WriteLine(result);
                    }
                }
                catch
                {
                    Console.WriteLine("Bad command!");
                }

            }
            return bRet;
        }

    }
}
