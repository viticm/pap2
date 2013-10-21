using System;
using System.Collections.Generic;
using System.Text;
using DataTableRemotingCommon;

namespace DataTableRemotingClient
{
    class BroadcastEventWrapper : MarshalByRefObject
    {
        #region ConnectEventHandler

        public event ConnectEventHandler ConnectEvent;

        public void OnConnectEvent(object sender, ConnectEventArgs args)
        {
            if (ConnectEvent != null)
            {
                ConnectEvent(sender, args);
            }
        }


        #endregion

        #region LogToMonitor

        public event LogToMonitorHandler LogToMonitorEvent;

        public void OnLogToMonitorEvent(string message, LogType type)
        {
            if (LogToMonitorEvent != null)
            {
                LogToMonitorEvent(message, type);
            }
        }

        #endregion

        public void ClearEventHandlers()
        {

        }

        public override object InitializeLifetimeService()
        {
            return null;
        }
    }
}
