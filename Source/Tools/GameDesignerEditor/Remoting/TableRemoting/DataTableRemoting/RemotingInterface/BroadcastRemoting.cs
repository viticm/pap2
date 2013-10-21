using System;
using System.Collections.Generic;
using System.Text;
using DataTableRemotingCommon;
using DataTableRemoting;
namespace DataTableRemoting.RemotingInterface
{
    public class BroadcastRemoting : MarshalByRefObject, IBroadcastRemoting
    {
        private event ConnectEventHandler mConnectEventHandler;
        private event LogToMonitorHandler mLogMonitorHandler;
        private string mClientKey;

        public BroadcastRemoting(string clientKey)
        {
            mClientKey = clientKey;
            BroadcastSystem.OnConnectEvent += OnConnectEvent;
            BroadcastSystem.LogToMonitorEvent += OnLogToMonitorEvent;

            LogHelper help = TableHelper.GetLogHelper(mClientKey);
            string message = "get new BroadcastRemoting.";
            help.LogMessage(mClientKey, message, LogType.Program);

        }

        #region Connect Event Interface

        public void OnConnectEvent(object sender, ConnectEventArgs args)
        {
            if (mConnectEventHandler != null)
            {
                try
                {
                    mConnectEventHandler(sender, args);
                }
                catch
                {
                    RemotingSystem.Instance.DisconnectServer(mClientKey);
                }
            }
        }

        public void AddConnectEventHandler(ConnectEventHandler handler)
        {
            mConnectEventHandler += handler;
        }

        public void DelConnectEventHandler(ConnectEventHandler handler)
        {
            mConnectEventHandler -= handler;
        }

        #endregion

        #region LogToMonitor Event

        public void OnLogToMonitorEvent(string message, LogType type)
        {
            if (mLogMonitorHandler != null)
            {
                try
                {
                    mLogMonitorHandler(message, type);
                }
                catch(Exception ex)
                {
                    mLogMonitorHandler = null;
                    LogHelper help = TableHelper.GetLogHelper(mClientKey);
                    help.LogMessage(mClientKey, "LogToMonitor Error: " + ex.Message, LogType.NetWork);
                }
            }
        }

        public void AddLogToMonitorHandler(LogToMonitorHandler handler)
        {
            mLogMonitorHandler += handler;
        }

        public void DelLogToMonitorHandler(LogToMonitorHandler handler)
        {
            mLogMonitorHandler -= handler;
        }

        #endregion

        public void Release()
        {
            mConnectEventHandler = null;
            BroadcastSystem.OnConnectEvent -= OnConnectEvent;
            BroadcastSystem.LogToMonitorEvent -= OnLogToMonitorEvent;
        }

        public override object InitializeLifetimeService()
        {
            return null;
        }
    }
}
