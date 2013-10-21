using System;
using System.Collections.Generic;
using System.Text;
using DataTableRemotingCommon;

namespace DataTableRemoting
{
    public static class NetClientManager
    {
        static Dictionary<string, NetClient> mNetClients;
        static object mThreadLock;

        static NetClientManager()
        {
            mNetClients = new Dictionary<string, NetClient>();
            mThreadLock = new object();
        }

        public static void AddClient(NetClient clinet)
        {
            lock (mThreadLock)
            {
                if (mNetClients.ContainsKey(clinet.ClientKey))
                {
                    DeleteClient(clinet.ClientKey);
                }

                clinet.LastActiveTime = DateTime.Now;
                mNetClients[clinet.ClientKey] = clinet;
            }
        }

        public static void DeleteClient(string clientKey)
        {
            lock (mThreadLock)
            {
                if (mNetClients.ContainsKey(clientKey))
                {
                    mNetClients.Remove(clientKey);
                }
            }
        }

        public static bool ExistClient(string clientKey)
        {
           
            bool result = mNetClients.ContainsKey(clientKey);
            return result;
        
        }

        public static NetClient GetNetClient(string clientKey)
        {
            NetClient client = null;
            if (mNetClients.ContainsKey(clientKey))
            {
                client = mNetClients[clientKey];
            }
            return client;
        }

        public static void Ping(string client)
        {
            lock (mThreadLock)
            {
                DateTime time = DateTime.Now;
                if (!mNetClients.ContainsKey(client))
                {
                    string ex = string.Format("not exist client {0}.", client);
                    throw new Exception(ex);
                }
                mNetClients[client].LastActiveTime = time;
                CheckActive(time);
            }
        }

        private static void CheckActive(DateTime time)
        {
            long time1 = time.Ticks;
            NetClient[] clients = new NetClient[mNetClients.Count];
            int index = 0;
            foreach (NetClient client in mNetClients.Values)
            {
                clients[index] = client;
                index++;
            }
            foreach (NetClient client in clients)
            {
                long time2 = client.LastActiveTime.Ticks;
                if (time1 - time2 > 10 * 4)
                {
                    RemotingInterface.RemotingSystem.Instance.DisconnectServer(client.ClientKey);
                }
            }
        }

        public static void CheckConnect()
        {
            NetClient client = new NetClient("Server", 9898, "Server");
            client.HostName = "server";
            BroadcastSystem.BroadcastConnectEvent(client,
               new ConnectEventArgs(true, client.HostName));
        }

        public static string[] GetClients()
        {
            string[] clients = new string[mNetClients.Count];
            int index = 0;
            foreach (NetClient client in mNetClients.Values)
            {
                clients[index] = client.HostName;
                index++;
            }
            return clients;
        }

        public static string[] GetClientKeys()
        {
            string[] result;
            lock (mThreadLock)
            {
                result = new string[mNetClients.Count];
                int i = 0;
                foreach (string key in mNetClients.Keys)
                {
                    result[i] = key;
                    i++;
                }
            }
            return result;
        }
    }
}
