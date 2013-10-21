using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace GameLuaEditor
{    
    class NetworkManager
    {
        private static NetworkManager networkManager;
        private string hostAddress = Helper.ServerIP /*"127.0.0.1"*/;
        private int udpSendPort = 6001;
        private int udpReceivePort = 5432;
        private Socket udpSendSocket = null;
        private Socket udpReceiveSocket = null;
        private IPEndPoint serverEndPoint = null;
        private IPEndPoint clientEndPoint = null;
        private EndPoint senderRemote = new IPEndPoint(IPAddress.Any, 0);
        private int bufferSize = 4096;
        private Thread udpReceiveThread = null;        
        private List<string> receivedMessageList = new List<string>(); // 收到的消息链表 

        public delegate void ProcessServerUdpMessage(string message);
        private ProcessServerUdpMessage processServerUdpMessage;

        private NetworkManager()
        {

        }

        /// <summary>
        /// 是否已经收到消息
        /// </summary>
        public bool HasReceivedMessage
        {
            get
            {
                return receivedMessageList.Count > 0;
            }
        }

        /// <summary>
        /// 收到的消息链表
        /// </summary>
        public List<string> ReceivedMessageList
        {
            get
            {
                List<string> list = new List<string>(receivedMessageList);
                return list;
            }
        }

        /// <summary>
        /// 处理接收到的UDP消息
        /// </summary>
        public ProcessServerUdpMessage CurrentProcessSerrverUdpMessage
        {
            set
            {
                processServerUdpMessage = value;
            }
        }

        /// <summary>
        /// 主机地址
        /// </summary>
        public string HostAddress
        {
            set
            {
                hostAddress = value;
            }

            get
            {
                return hostAddress;
            }
        }

        /// <summary>
        /// UDP发送端口
        /// </summary>
        public int UdpSendPort
        {
            set
            {
                udpSendPort = value;
            }

            get
            {
                return udpSendPort;
            }
        }

        /// <summary>
        /// UDP接收端口
        /// </summary>
        public int UdpReceivePort
        {
            set
            {
                udpReceivePort = value;
            }

            get
            {
                return udpReceivePort;
            }
        }

        /// <summary>
        /// 获取网络管理对象
        /// </summary>
        /// <returns>网络管理对象</returns>
        public static NetworkManager GetNetworkManager()
        {
            if (networkManager == null)
            {
                networkManager = new NetworkManager();
                networkManager.Init();
            }

            return networkManager;
        }        

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            InitUdpSocket();
        }

        /// <summary>
        /// 回收数据
        /// </summary>
        public void Uninit()
        {            
            StopReceiveUdpMessage();
            UnInitUdpSocket();
        }

        /// <summary>
        /// 清空接收到的消息链表
        /// </summary>
        public void ClearReceivedMessageList()
        {
            receivedMessageList.Clear();
        }

        /// <summary>
        /// 获取消息队列中指定的消息
        /// </summary>
        /// <param name="message">消息内容</param>
        /// <returns>指定的消息</returns>
        public string GetMessage(string message)
        {
            string returnMessage = null;
            int removeIndex = -1;

            for (int i = 0; i < receivedMessageList.Count; i++)
            {
                if (receivedMessageList[i] == message)
                {
                    removeIndex = i;
                    returnMessage = message;
                    break;
                }
            }

            if (removeIndex != -1)
            {
                receivedMessageList.RemoveAt(removeIndex);
            }

            return returnMessage;
        }

        /// <summary>
        /// 获取消息队列中有指定消息头的消息
        /// </summary>
        /// <param name="messageHead">消息头</param>
        /// <returns>消息</returns>
        public string GetMessageForHead(string messageHead)
        {
            string message = null;
            int removeIndex = -1;

            for (int i = 0; i < receivedMessageList.Count; i++)
            {
                if (receivedMessageList[i].StartsWith(messageHead))
                {
                    removeIndex = i;
                    message = receivedMessageList[i];
                    break;
                }
            }

            if (removeIndex != -1)
            {
                receivedMessageList.RemoveAt(removeIndex);
            }                

            return message;
        }

        /// <summary>
        /// 初始化UDP套接字
        /// </summary>
        public void InitUdpSocket()
        {
            serverEndPoint = new IPEndPoint(IPAddress.Parse(hostAddress), udpSendPort);
            udpSendSocket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);

            clientEndPoint = new IPEndPoint(IPAddress.Parse(hostAddress), udpReceivePort);
            udpReceiveSocket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            udpReceiveSocket.Bind(clientEndPoint);
        }

        /// <summary>
        /// 销毁UDP套接字
        /// </summary>
        public void UnInitUdpSocket()
        {
            if (udpSendSocket != null)
            {
                udpSendSocket.Close();
            }
            
            if (udpReceiveSocket != null)
            {
                udpReceiveSocket.Close();
            }            
        }

        /// <summary>
        /// 发送UDP消息
        /// </summary>
        /// <param name="message">消息</param>
        public void SendUdpMessage(string message)
        {
            byte[] buffer = Encoding.GetEncoding("gb2312").GetBytes(message);
            udpSendSocket.SendTo(buffer, serverEndPoint);
        }

        /// <summary>
        /// 接收UDP消息
        /// </summary>        
        public void BeginReceiveUdpMessage()
        {   
            if (udpReceiveThread == null)
            {
                udpReceiveThread = new Thread(new ThreadStart(ThreadReceiveUdpMessage));
                udpReceiveThread.Start();
            }
        }

        /// <summary>
        /// 停止接受UDP消息
        /// </summary>
        public void StopReceiveUdpMessage()
        {
            if (udpReceiveThread != null)
            {
                udpReceiveThread.Abort();
            }
        }

        /// <summary>
        /// 线程接收UDP消息
        /// </summary>
        private void ThreadReceiveUdpMessage()
        {
            while (true)
            {
                byte[] buffer = new byte[bufferSize];
                int count = udpReceiveSocket.ReceiveFrom(buffer, ref senderRemote);
                string message = Encoding.GetEncoding("gb2312").GetString(buffer, 0, count);
                Console.WriteLine(message);
                receivedMessageList.Add(message);
            }
        }
    }
}
