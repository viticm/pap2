using System;
using System.Collections.Generic;
using System.Text;

namespace DataTableRemotingCommon
{
    [Serializable]
    public class NetClient
    {
        private string mIPAddress;
        private string mHostName;
        private string mUserName;
        private string mToolName;
        private string mModelName;
        private int mPort;

        private DateTime mDateTime;

        private string mDatabase;

        public NetClient(string ip, int port, string tool)
        {
            mIPAddress = ip;
            mPort = port;
            mToolName = tool;
            mHostName = string.Empty;
            mUserName = string.Empty;
            mDateTime = DateTime.Now;
            mModelName = string.Empty;
        }

        public void SetClientInfo(string host, string user)
        {
            mHostName = host;
            mUserName = user;
        }

        public override bool Equals(object obj)
        {
            bool result = false;
            if (obj is NetClient)
            {
                result = (obj as NetClient).ClientKey == ClientKey;
            }
            return result;
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public override string ToString()
        {
            return ClientKey;
        }

        public static bool operator ==(NetClient client1, NetClient client2)
        {
            return Object.Equals(client1, client2);
        }

        public static bool operator !=(NetClient client1, NetClient client2)
        {
            return !Object.Equals(client1, client2);
        }

        public string ClientKey { get { return string.Format("{0}:{1}", mIPAddress, mPort); } }

        public string IPAddress { get { return mIPAddress; } set { mIPAddress = value; } }
        public string HostName { get { return mHostName; } set { mHostName = value; } }
        public string UserName { get { return mUserName; } set { mUserName = value; } }
        public int Port { get { return mPort; } set { mPort = value; } }
        public string Database { get { return mDatabase; } set { mDatabase = value; } }
        public DateTime LastActiveTime { get { return mDateTime; } set { mDateTime = value; } }
        public string ModelName { get { return mModelName; } set { mModelName = value; } }
    }
}
