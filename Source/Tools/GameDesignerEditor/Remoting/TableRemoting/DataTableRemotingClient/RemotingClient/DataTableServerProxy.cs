using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Runtime.Remoting;
using System.Configuration;
using DataTableRemotingCommon;
namespace DataTableRemotingClient
{
    public static class DataTableServerProxy
    {
        private static NetClient mNetClient;
        private static IRemotingSystem mRemotingSystem;
        private static DataTableProxy mDataTableProxy;
        private static BroadcastSystem mBroadcastSystem;
        private static IMonitorRemoting mMonitorRemoting;

        private static string mDBName;
        public static bool IsInit = false;

        /// <summary>
        /// 初始化远程对象
        /// </summary>
        /// <param name="config">Remoting配置文件</param>
        public static void InitServerProxy(string config, string dbname)
        {
            mDBName = dbname;
            mNetClient = GetClientInfo(config);
            mNetClient.Database = dbname;
            RemotingConfiguration.Configure(config, false);
            string remotingUrl = ConfigurationManager.AppSettings["RemotingSystem"];
            mRemotingSystem = (IRemotingSystem)Activator.GetObject(typeof(IRemotingSystem), remotingUrl);
            mRemotingSystem.ConnectServer(mNetClient);
            BroadcastSystem broadcastSystem = GetBroadcastSystem();
            broadcastSystem.AddConnectEventHandler(OnConnect);
            TableHelper.EnableRemoting(true);
            IsInit = true;
        }


        public static void DisconnectServer()
        {
            mRemotingSystem.DisconnectServer(mNetClient.ClientKey);
        }
       
        public static string GetDBSourceString()
        {
            string source = mRemotingSystem.GetDBSourceString(mNetClient.Database);
            return source;
        }

        /// <summary>
        /// 获取数据表远程代理
        /// </summary>
        /// <returns></returns>
        public static DataTableProxy GetDataTableProxy()
        {
            if (mDataTableProxy == null)
            {
                IDataTableRemoting remoting = mRemotingSystem.GetDataTableRemoting(mNetClient.ClientKey);
                mDataTableProxy = new DataTableProxy(remoting);
            }
            return mDataTableProxy;
        }

        /// <summary>
        /// 获取广播远程对象
        /// </summary>
        /// <returns></returns>
        public static BroadcastSystem GetBroadcastSystem()
        {
            if (mBroadcastSystem == null)
            {
                IBroadcastRemoting remoting = mRemotingSystem.GetBroadcastRemoting(mNetClient.ClientKey);
                mBroadcastSystem = new BroadcastSystem(remoting);
            }
            return mBroadcastSystem;
        }

        /// <summary>
        /// 获取监视器远程对象
        /// </summary>
        /// <returns></returns>
        public static IMonitorRemoting GetMonitorRemoting()
        {
            if (mMonitorRemoting == null)
            {
                mMonitorRemoting = mRemotingSystem.GetMonitorRemoting(mNetClient.ClientKey);
            }
            return mMonitorRemoting;
        }


        private static NetClient GetClientInfo(string config)
        {
            string host = Dns.GetHostName();
            IPAddress[] ips = Dns.GetHostAddresses(host);
            string ip = ips[0].ToString();
            for (int i = 0; i < ips.Length; ++i)
            {
                if (ips[i].AddressFamily != System.Net.Sockets.AddressFamily.InterNetworkV6)
                {
                    ip = ips[i].ToString();
                    break;
                }
            }

            string port = ConfigurationManager.AppSettings["port"];
            string tool = ConfigurationManager.AppSettings["tool"];
            NetClient client = new NetClient(ip, Convert.ToInt32(port), tool);
            client.HostName = host;
            return client;
        }

        private static void OnConnect(object sender, ConnectEventArgs args)
        {
            
        }
    }
}
