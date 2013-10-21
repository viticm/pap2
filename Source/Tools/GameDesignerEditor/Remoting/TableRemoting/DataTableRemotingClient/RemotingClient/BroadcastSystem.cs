using System;
using System.Collections.Generic;
using System.Text;
using DataTableRemotingCommon;

namespace DataTableRemotingClient
{
    public class BroadcastSystem
    {
        private BroadcastEventWrapper mBroadcastEventWrapper;

        private IBroadcastRemoting mBroadCastRemoting;


        public BroadcastSystem(IBroadcastRemoting remoting)
        {
            mBroadCastRemoting = remoting;
            mBroadcastEventWrapper = new BroadcastEventWrapper();
            mBroadCastRemoting.AddConnectEventHandler(mBroadcastEventWrapper.OnConnectEvent);
            mBroadCastRemoting.AddLogToMonitorHandler(mBroadcastEventWrapper.OnLogToMonitorEvent);
        }

        #region Connect Event Wrapper

        public void AddConnectEventHandler(ConnectEventHandler handler)
        {
            mBroadcastEventWrapper.ConnectEvent += handler;
        }

        public void DelConnectEventHandler(ConnectEventHandler handler)
        {
            mBroadcastEventWrapper.ConnectEvent -= handler;
        }

        #endregion

        #region LogToMonitor Event Wrapper

        public void AddLogToMonitorHandler(LogToMonitorHandler handler)
        {
            mBroadcastEventWrapper.LogToMonitorEvent += handler;
        }

        public void DelLogToMonitorHandler(LogToMonitorHandler handler)
        {
            mBroadcastEventWrapper.LogToMonitorEvent -= handler;
        }

        #endregion
    }
}
