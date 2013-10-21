using System;
using System.Collections.Generic;
using System.Text;

namespace DataTableRemotingCommon
{
    [Serializable]
    public class ConnectEventArgs : EventArgs
    {
        private bool mLogin;
        private string mMessage;

        public ConnectEventArgs(bool login, string message)
        {
            mLogin = login;
            mMessage = message;
        }

        public bool Login { get { return mLogin; } }
        public string Message { get { return mMessage; } }
    }

    public delegate void ConnectEventHandler(object sender, ConnectEventArgs args);

    public delegate void LogToMonitorHandler(string message, LogType type);
}
