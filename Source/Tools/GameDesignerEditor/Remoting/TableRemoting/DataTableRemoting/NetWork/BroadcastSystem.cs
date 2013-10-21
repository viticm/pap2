using System;
using System.Collections.Generic;
using System.Text;
using DataTableRemotingCommon;

namespace DataTableRemoting
{
    public static class BroadcastSystem
    {
        private static event ConnectEventHandler mConnectEventHandler;

        private static event LogToMonitorHandler mLogToMonitorHandler;

        public static void BroadcastConnectEvent(object sender, ConnectEventArgs args)
        {
            if (mConnectEventHandler != null)
            {
                mConnectEventHandler(sender, args);
            }
        }

        public static ConnectEventHandler OnConnectEvent
        {
            get { return mConnectEventHandler; }
            set { mConnectEventHandler = value; }
        }

        public static void BroadcastLogToMonitorHandler(string message, LogType type)
        {
            if (mLogToMonitorHandler != null)
            {
                mLogToMonitorHandler(message, type);
            }
        }

        public static LogToMonitorHandler LogToMonitorEvent
        {
            get { return mLogToMonitorHandler; }
            set { mLogToMonitorHandler = value; }
        }
    }
}
