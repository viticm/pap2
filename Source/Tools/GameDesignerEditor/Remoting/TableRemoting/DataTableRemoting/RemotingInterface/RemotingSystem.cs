using System;
using System.Collections.Generic;
using System.Text;
using DataTableRemotingCommon;
using DataTableRemoting;

namespace DataTableRemoting.RemotingInterface
{
    public class RemotingSystem : MarshalByRefObject, IRemotingSystem
    {
        private Dictionary<string, DataTableRemoting> mDataTableRemoting;

        private Dictionary<string, BroadcastRemoting> mBroadcastRemoting;

        private Dictionary<string, MonitorRemoting> mMonitorRemoting;

        public static RemotingSystem Instance;

        public RemotingSystem()
        {
            mDataTableRemoting = new Dictionary<string, DataTableRemoting>();
            mBroadcastRemoting = new Dictionary<string, BroadcastRemoting>();
            mMonitorRemoting = new Dictionary<string, MonitorRemoting>();
            Instance = this;

            Console.WriteLine("Create RemotingSystem!");
        }

        public IDataTableRemoting GetDataTableRemoting(string clientKey)
        {
            if (!mDataTableRemoting.ContainsKey(clientKey))
            {
                mDataTableRemoting[clientKey] = new DataTableRemoting(clientKey);
            }

            return mDataTableRemoting[clientKey];
        }



        public IBroadcastRemoting GetBroadcastRemoting(string clientKey)
        {
            if (!mBroadcastRemoting.ContainsKey(clientKey))
            {
                mBroadcastRemoting[clientKey] = new BroadcastRemoting(clientKey);
            }
            return mBroadcastRemoting[clientKey];
        }

        public IMonitorRemoting GetMonitorRemoting(string clientKey)
        {
            if (!mMonitorRemoting.ContainsKey(clientKey))
            {
                mMonitorRemoting[clientKey] = new MonitorRemoting();
            }
            return mMonitorRemoting[clientKey];
        }

        public void ConnectServer(NetClient client)
        {
            if (NetClientManager.ExistClient(client.ClientKey))
            {
                DisconnectServer(client.ClientKey);
            }
            NetClientManager.AddClient(client);

            LogHelper help = TableHelper.GetLogHelper(client.ClientKey);
            help.LogMessage(client.ClientKey, "Connect " + client.Database, LogType.NetWork);
        }

        public void DisconnectServer(string clientKey)
        {
            
            NetClient client = NetClientManager.GetNetClient(clientKey);
            
            

            lock (mBroadcastRemoting)
            {

                if (mBroadcastRemoting.ContainsKey(clientKey))
                {
                    mBroadcastRemoting[clientKey].Release();
                    mBroadcastRemoting.Remove(clientKey);
                }
            }
            lock (mDataTableRemoting)
            {
                if (mDataTableRemoting.ContainsKey(clientKey))
                {
                    try
                    {
                        mDataTableRemoting[clientKey].Release();
                    }
                    catch (Exception ex)
                    {
                        throw ex;
                    }
                    finally
                    {
                        mDataTableRemoting.Remove(clientKey);
                    }
                }
            }

            lock (mMonitorRemoting)
            {
                if (mMonitorRemoting.ContainsKey(clientKey))
                {
                    mMonitorRemoting.Remove(clientKey);
                }
            }

            if (client == null)
            {
                return;
            }

            string hostname = client.HostName;
            LogHelper help = TableHelper.GetLogHelper(clientKey);
            help.LogMessage(clientKey, "Disconnect " + client.Database, LogType.NetWork);

            NetClientManager.DeleteClient(clientKey);

        }

        public string GetDBSourceString(string database)
        {
            string source = TableHelper.GetDBSourceString(database);
            return source;
        }

        public override object InitializeLifetimeService()
        {
            return null;
        }
    }
}
