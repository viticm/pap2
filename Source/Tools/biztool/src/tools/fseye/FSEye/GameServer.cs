//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 2007-4
//      File_base        : GameServer
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 游戏服务器
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Data;
using System.IO;
using FSEye.Security;
using FSEye.Net;
using FSEye.DAL;
using FSEye.PlugIn;
using System.Net;
using System.Net.Sockets;
using System.Xml;
using FSEye.DllWrap;

namespace FSEye
{
	#region GameServer
	/// <summary>
	/// 游戏服务器类
	/// </summary>
	/// <author>徐晓刚</author>
	public class GameServer : ISecurity, IDisposable
	{
		#region 常量       

		/// <summary>
		/// 服务器状态，指服务器机器的状态
		/// </summary>
		public enum ServerState
		{
			Unknown,
			Restarting,
			Running,
			Updating,  //表示update guard
            Downloading //下载更新包到本机
		}

		/// <summary>
        /// 游戏服务状态，指游戏服务进程的状态；对于服务器组来说显示颜色优先级别从高到低
		/// </summary>
        public enum ServiceState
        {
            Starting,
            Stopping,
            //Updating, //表示guard解压更新包覆盖本地游戏服务器文件
            ErrorStopped,  //断开，指服务器程序主动和WEB断开，非人工从web关闭
            OKStopped,  //停止，指web主动关闭服务器程序
            Running,
            Timeout,
            Dead,
            Restarting
        }

        public enum UpdateState
        {
            Nothing,
            Downloading,
            Downloaded,
            Updating,
            Updated,
        }

        //关闭顺序  
        public enum StopOrder
        {
            gameserver,
            gamecenter,            
            bishop,
            logserver,
            goddess
        }

       //启动顺序
        public enum StartOrder
        {
            goddess, 
            logserver,
            bishop,
            gamecenter,
            gameserver
        }

		/// <summary>
		/// 服务器类型
		/// </summary>
		public enum ServerType
		{
			unidentified,
			gameserver,
			databaseserver,
            bishop,
            goddess,
            gamecenter,
            paysys,
            logserver
		}

        /// <summary>
        /// 服务器事件类型
        /// </summary>
        public enum ServerEventType
        {
            UnknownEvent = 0,
            GameStart,
            GameStartFail,
            GameStop,
            GameStartByAccident,
            GameStopByAccident
        }

        /// <summary>
        /// 对服务器的操作类型
        /// </summary>
        public enum ServerOperationType
        {
            Start = 0,
            Close,
            Update,
            Download,
            UpdateGuard,
            UpdateConfigFile
        }
  
		public enum OSType
		{
			Windows = 0,
			Linux
		}

        /// <summary>
        /// 服务器状态（易读版，与接进游戏端）
        /// </summary>
        public enum ReadableState
        {
            Stop,       //维护
            Good,       //良好
            Busy,       //繁忙
            Full        //爆满
        }

		public static ServerState ParseServerState(string state)
		{
            if (state.Equals("Unknown", StringComparison.OrdinalIgnoreCase))
                return ServerState.Unknown;
            else if (state.Equals("Restarting", StringComparison.OrdinalIgnoreCase))
                return ServerState.Restarting;
            else if (state.Equals("Running", StringComparison.OrdinalIgnoreCase))
                return ServerState.Running;
            else if (state.Equals("Updating", StringComparison.OrdinalIgnoreCase))
                return ServerState.Updating;
            else if (state.Equals("Downloading", StringComparison.OrdinalIgnoreCase))
                return ServerState.Downloading;
            else throw new Exception("Parse server state error.");			
		}

		public static ServiceState ParseServiceState(string state)
		{
            if (state.Equals("Unknown", StringComparison.OrdinalIgnoreCase))
                return ServiceState.OKStopped;
            else if (state.Equals("Stopped", StringComparison.OrdinalIgnoreCase))
                return ServiceState.ErrorStopped;
            else if (state.Equals("Starting", StringComparison.OrdinalIgnoreCase))
                return ServiceState.Starting;
            else if (state.Equals("Running", StringComparison.OrdinalIgnoreCase))
                return ServiceState.Running;
            else if (state.Equals("Stopping", StringComparison.OrdinalIgnoreCase))
                return ServiceState.Stopping;
            else if (state.Equals("Timeout", StringComparison.OrdinalIgnoreCase))
                return ServiceState.Timeout;
            else if (state.Equals("Dead", StringComparison.OrdinalIgnoreCase))
                return ServiceState.Dead;
            else if (state.Equals("Restarting", StringComparison.OrdinalIgnoreCase))
                return ServiceState.Restarting;
//            else if (state.Equals("Updating", StringComparison.OrdinalIgnoreCase))
  //              return ServiceState.Updating;
            else throw new Exception("Parse service state error.");			
		}

		//消息信息
		public class MessageInfo
		{
			public enum Type
			{
				Normal,
				Success,
				Failure,
				Alert
			}

			public string Message;
			public DateTime Time;
			public Type MessageType;

			public override string ToString()
			{
				return Time.ToString() + " " + Message;
			}
		}

		private const int MAX_HISTORY_RECORD_COUNT = 60;

		public const int MAX_GAME_INFO_HISTORY_RECORD_COUNT = 60;

		public const int MAX_MEMORY_INFO_HISTORY_RECORD_COUNT = 60;

		public const int MAX_CPU_COUNT = 10;
		
		public const string FilePathDelimiter = @"/";

        public const string FilePathWinDelimiter = @"\";

		public const string UploadFileDir = "file";

		public const string ExtractPatchDir = "server";

        /*状态阈值点*/
        public const double BusyPlayerCountPercent = 0.6;

        public const double FullPlayerCountPercent = 0.8;

        public const ReadableState DefaultReadableState = ReadableState.Stop;

		public const int RecentMessageQueueLength = 10;

		#endregion

		#region 变量

		bool _disposed = false;

		string _hostName;

		string _systemName;

		string _ipAddress;

		string _macAddress;

		int _port;

        int _serverID;

        int _ftpServer;

		OSType _operationSystem;

		string _gameDir;

		string _guardDir;

        string _configContent;

		long _connectionId;

		byte[] _communicationKey;

		bool _connected;

		bool _isIdle;

		ServerState _serverState;

		ServiceState _serviceState;

		IList _fileManageTaskList;

		IList _cpuInfos;

		IList _networkCards;

		IList _diskInfos;

		IList _processInfoList;

		IList _plugInInfoList;

		Queue _memoryInfoQueue;

		Queue _gameInfoQueue;

		Hashtable _installedPlugInMap;
		
		IList _installedPlugInList;

		FSEyeObject _securityObject;

		object _syncRoot = new object();

		ServerGroup _group;

		LordConfig _lordConfig;

		GameSetting _gameSetting;

		Queue<MessageInfo> _recentMessageQueue;

        IList _updateMsg;

        string[] _downloadMsg;

        int _PID =-2;
        
        CheckResult _lastCheckResult;

		/// <summary>
		/// 配置Guard状态
		/// </summary>
		public enum ConfigGuardState
		{
			None = 0,
			Configuring,
			WaitResponse,
			Configured
		}

		ConfigGuardState _configGuardState = ConfigGuardState.None;

		ServerType _serverType;
		
		#endregion

		#region 事件

		//已连接事件
		public event ConnectedEventHandler Connected;

		//已断开事件
		public event DisconnectedEventHandler Disconnected;

		//系统时钟事件
		public event TickEventHandler Tick;

		//收到消息事件
		public event MessageReceivedEventHandler MessageReceived;

		//发送消息事件
		public event MessageSentEventHandler MessageSent;

		//游戏服务状态改变事件
		public event GameStateChangeEventHandler GameStateChanged;

		#endregion

		#region 属性
        //下载进度
        public double DownloadProgress
        {
            get {
                if (_downloadMsg == null)
                    return 0;
                else
                    return double.Parse(_downloadMsg[0]) / 100.00;
                
            }
        }

		public FSEyeObject SecurityObject
		{
			get { return _securityObject; }
			set { _securityObject = value; }
		}

		public int Id
		{
			get { return _securityObject.Id; }
		}

        public int PID
        {
            get { return _PID; }
            set { _PID = value; }
        }

		public string Name
		{
			get { return _securityObject.Name; }
		}

		public string Comment
		{
			get { return _securityObject.Comment; }
		}

		public string HostName
		{
			get { return _hostName; }
		}

		public string SystemName
		{
			get { return _systemName; }
		}

		public long ConnectionId
		{
			get { return _connectionId; }
		}

		public byte[] CommunicationKey
		{
			get { return _communicationKey; }
			set { _communicationKey = value; }
		}

		public OSType OperationSystem
		{
			get { return _operationSystem; }
		}

        public UInt16 SameGuardServerCount
        {
            get
            {
                UInt16 iServerCount = 0;
                string sServerIDs = "";
                foreach (GameServer server in AdminServer.TheInstance.GameServerManager.Servers)
                {
                    if (_ipAddress == server.IpAddress )
                    {
                        if (iServerCount >= 0)
                            sServerIDs = sServerIDs + "," + server.Id.ToString();
                        else
                            sServerIDs = server.Id.ToString();

                        iServerCount++;
                    }
                }
                return iServerCount;
            }
        }

        public string SameGuardServerIDs
        {
            get
            {
                int iServerCount = 0;
                string sServerIDs = "";
                foreach (GameServer server in AdminServer.TheInstance.GameServerManager.Servers)
                {
                    if (_ipAddress == server.IpAddress)
                    {
                        if (iServerCount >= 0)
                            sServerIDs = sServerIDs + "," + server.Id.ToString();
                        else
                            sServerIDs = server.Id.ToString();

                        iServerCount++;
                    }
                }
                return sServerIDs;
            }
        }

		public ServerGroup Group
		{
			get { return _group; }
			set { _group = value; }
		}
        public string ConfigContent
		{
            get { return _configContent; }
            set { _configContent = value; }
		}
        
		/// <summary>
		/// CPU信息
		/// </summary>
		public IList CpuInfos
		{
			get { return _cpuInfos; }
		}

		/// <summary>
		/// 当前CPU的平均负载
		/// </summary>
		/// <return>如果没有有效信息，返回-1</return>
		public int CurrentCpuLoad
		{
			get
			{
				int averageLoad = 0;
				foreach (CpuInfo info in _cpuInfos)
				{
					if (info.Loads.Length > 0)
					{
						averageLoad += info.Loads[info.Loads.Length - 1];
					}
					else
					{
						return -1;
					}
				}
				if (_cpuInfos.Count > 0)
				{
					averageLoad /= _cpuInfos.Count;
				}
				else
				{
					return -1;
				}
				return averageLoad;
			}
		}

		/// <summary>
		/// 当前内存信息
		/// </summary>
		public MemoryInfo CurrentMemInfo
		{
			get
			{
				if (_memoryInfoQueue.Count > 0)
				{
					object[] infos = _memoryInfoQueue.ToArray();
					return (MemoryInfo)infos[infos.Length - 1];
				}
				else
				{
					return null;
				}
			}
		}

        public double CurrentNetCardsInfo
        {
            get
            {
                if (_networkCards != null && _networkCards.Count != 0)
                {
                    uint total = 0;
                    foreach (NetworkCardInfo info in _networkCards)
                    {
                        total += info.RXRate+info.TXRate;
                    }
                    return (double)total / ((double)_networkCards.Count*1024*1024*10);
                }
                return 0;
            }
        }

		/// <summary>
		/// 网卡信息
		/// </summary>
		public IList NetworkCards
		{
			get { return _networkCards; }
		}

		/// <summary>
		/// 磁盘信息
		/// </summary>
		public IList DiskInfos
		{
			get { return _diskInfos; }
			set { _diskInfos = value; }
		}

		/// <summary>
		/// 文件管理任务信息
		/// </summary>
		public IList FMTask
		{
			get { return _fileManageTaskList; }
		}

        /// <summary>
        /// guard在本机执行更新
        /// </summary>
        public IList UpdateMsg
        {
            get { return _updateMsg; }
            set { _updateMsg.Add(value); }
        }

        /// <summary>
        /// 更新包下载信息
        /// </summary>
        public string[] DownloadMsg
        {
            get { return _downloadMsg; }
            set { 
                _downloadMsg[0] =value[0];
                _downloadMsg[1] = value[1];
                _downloadMsg[2] = value[2];
            }
        }

		/// <summary>
		/// 进程信息
		/// </summary>
		public IList ProcessInfos
		{
			get { return _processInfoList; }
		}

		/// <summary>
		/// 插件信息
		/// </summary>
		public IList PlugInInfos
		{
			get { return _plugInInfoList; }
		}

		/// <summary>
		/// 安装上的插件列表
		/// </summary>
		/// <remarks>请对这个列表中的对象进行只读访问</remarks>
		public IList InstalledPlugInList
		{
			get { return _installedPlugInList; }
		}

		/// <summary>
		/// 内存信息
		/// </summary>
		public MemoryInfo[] MemoryInfos
		{
			get
			{
				object[] objs = _memoryInfoQueue.ToArray();
				MemoryInfo[] infos = new MemoryInfo[objs.Length];
				Array.Copy(objs, infos, objs.Length);
				return infos;
			}
		}

		/// <summary>
		/// 网络地址
		/// </summary>
		public string IpAddress
		{
			get { return _ipAddress; }
			set { _ipAddress = value; }
		}

        /// <summary>
        /// 下载更新包的FTP
        /// </summary>
        public int FtpServer
        {
            get { return _ftpServer; }
            set { _ftpServer = value; }                
        }

		/// <summary>
		/// MAC地址
		/// </summary>
		public string MacAddress
		{
			get { return _macAddress; }
			set { _macAddress = value; }
		}

		/// <summary>
		/// 连接端口
		/// </summary>
		public int Port
		{
			get { return _port; }
			set { _port = value; }
		}

        /// <summary>
        /// 服务器ID；0表示GUARD
        /// </summary>
        public int ServerID
        {
            get { return _serverID; }
            set { _serverID = value; }
        }

		/// <summary>
		/// 是否连接
		/// </summary>
		public bool IsConnected
		{
			get { return _connected; }
			set { _connected = value; }
		}

		/// <summary>
		/// 游戏目录
		/// </summary>
		public string GameDir
		{
			get { return _gameDir; }
			set { _gameDir = value; }
		}

		/// <summary>
		/// 游戏安装目录
		/// </summary>
		public string GameInstallDir
		{
			get
			{
				if (_gameDir != null)
				{
                    int endIndex = _gameDir.LastIndexOf(FilePathWinDelimiter);
					return _gameDir.Substring(0, endIndex);
				}
				else
				{
					return null;
				}
			}
		}

		/// <summary>
		/// Guard目录
		/// </summary>
		public string GuardDir
		{
			get { return _guardDir; }
			set { _guardDir = value; }
		}

		/// <summary>
		/// 当前作业
		/// </summary>
		/// <return>如果有任务，返回任务的名称；如果没有，返回空字符串</return>
		public string CurrentTask
		{
			get
			{
				if (_fileManageTaskList.Count > 0)
				{
					return _fileManageTaskList[0].ToString();
				}
				else
				{
					return string.Empty;
				}
			}
		}

		/// <summary>
		/// 当前任务进度
		/// </summary>
		public float CurrentTaskProgress
		{
			get
			{
				if (_fileManageTaskList.Count > 0)
				{
					return (_fileManageTaskList[0] as FileManageTask).CompletePercentage;
				}
				else
				{
					return 0F;
				}
			}
		}

		/// <summary>
		/// 是否空闲
		/// </summary>
		public bool IsIdle
		{
			get
			{
				return _isIdle;
			}
		}

		/// <summary>
		/// 游戏服务器状态
		/// </summary>
		public ServiceState GameServiceState
		{
			get { return _serviceState; }
			set
			{
				if (_serviceState != value)
				{
					ServiceState originalState = _serviceState;
					_serviceState = value;

					if (this.GameStateChanged != null)
					{
						this.GameStateChanged(this, originalState, value);
					}
				}
			}
		}

        public UpdateState UpdateOrDownState;


		/// <summary>
		/// 游戏服务器状态
		/// </summary>
		public ServerState GameServerState
		{
			get { return _serverState; }
			set { _serverState = value; }
		}

        /// <summary>
        /// 服务器状态（易读版）
        /// </summary>
        public ReadableState ServerReadableState
        {
            get
            {
                ReadableState state = ReadableState.Stop;
                switch (this.GameServiceState)
                {
                    case ServiceState.Running:

                        ServerGroup group = this.Group;

                        GameServer gameCenter = null;

                        foreach (GameServer server in group.List)
                        {
                            if (server.Type == ServerType.gamecenter)
                            {
                                gameCenter = server;
                                break;
                            }
                        }

                        if (gameCenter != null)
                        {
                            if (gameCenter.GameServiceState == ServiceState.Running)
                            {
                                int playerCount = (int)gameCenter.GetPlugInData(AdminServer.TheInstance.AutomationManager.SecurityObject.Id, LordControl.PlugInGuid, LordControl.DataKeyCurrentPlayerCount);
                                if (this.LordConfig != null && this.LordConfig.MaxPlayer != 0)
                                {
                                    double percent = (double)playerCount / (double)this.LordConfig.MaxPlayer;
                                    if (percent < BusyPlayerCountPercent)
                                    {
                                        state = ReadableState.Good;
                                    }
                                    else if (percent < FullPlayerCountPercent)
                                    {
                                        state = ReadableState.Busy;
                                    }
                                    else
                                    {
                                        state = ReadableState.Full;
                                    }
                                }
                                else
                                {
                                    state = ReadableState.Stop;
                                }
                                
                                break;
                            }
                            else
                            {
                                state = ReadableState.Stop;

                                break;
                            }
                        }
                        else
                        {
                            state = ReadableState.Stop;
                            break;
                        }

                    default:
                        state = ReadableState.Stop;

                        break;
                }
                return state;
            }
        }

		/// <summary>
		/// Lord配置
		/// </summary>
		public LordConfig LordConfig
		{
			get { return _lordConfig; }
		}

		/// <summary>
		/// 游戏设定
		/// </summary>
		public GameSetting GameSetting
		{
			get { return _gameSetting; }
		}

		/// <summary>
		/// 近期消息队列
		/// </summary>
		public MessageInfo[] RecentMessages
		{
			get
			{
				lock (_recentMessageQueue)
				{
					return _recentMessageQueue.ToArray();
				}
			}
		}

		/// <summary>
		/// 最新的消息
		/// </summary>
		public MessageInfo LatestMessage
		{
			get
			{
				lock (_recentMessageQueue)
				{
					MessageInfo[] msgArray = _recentMessageQueue.ToArray();
					if (msgArray.Length > 0)
						return msgArray[msgArray.Length - 1];
					else
						return null;
				}
			}
		}

		/// <summary>
		/// 配置Guard状态
		/// </summary>
		public ConfigGuardState ConfigState
		{
			get { return _configGuardState; }
			set { _configGuardState = value; }
		}

		/// <summary>
		/// 服务器类型
		/// </summary>
		public ServerType Type
		{
			get { return _serverType; }
			set { _serverType = value; }
		}

        /// <summary>
        /// 最近一次的告警信息结果
        /// </summary>
        public CheckResult LastCheckResult
        {
            get { return _lastCheckResult; }
            set { _lastCheckResult = value; }
        }
		#endregion

		#region 其他
		/// <summary>
		/// 构造
		/// </summary>
		public GameServer()
		{
			_fileManageTaskList = ArrayList.Synchronized(new ArrayList());
			_cpuInfos = ArrayList.Synchronized(new ArrayList());
			_memoryInfoQueue = Queue.Synchronized(new Queue());
			_diskInfos = ArrayList.Synchronized(new ArrayList());
			_networkCards = ArrayList.Synchronized(new ArrayList());
			_processInfoList = ArrayList.Synchronized(new ArrayList());
			_plugInInfoList = ArrayList.Synchronized(new ArrayList());
			_gameInfoQueue = Queue.Synchronized(new Queue());
			_installedPlugInList = ArrayList.Synchronized(new ArrayList());
			_installedPlugInMap = Hashtable.Synchronized(new Hashtable());
			_serverState = ServerState.Unknown;
			_lordConfig = new LordConfig();
            _updateMsg = ArrayList.Synchronized(new ArrayList());
            _downloadMsg = new string[3] { "0", "1", "2" };

			_gameSetting = new GameSetting(this);
			_recentMessageQueue = new Queue<MessageInfo>(RecentMessageQueueLength);
            _lastCheckResult = new CheckResult();
		}

		/// <summary>
		/// 初始化
		/// </summary>
		public void Initialize()
		{
			_isIdle = true;
			_serverState = ServerState.Unknown;
			_serviceState = ServiceState.OKStopped;

			InstallPlugIn(new FSEye.PlugIn.Ping(10, 40));
			InstallPlugIn(new FSEye.PlugIn.UpdateCpu(5));
			InstallPlugIn(new FSEye.PlugIn.UpdateMemory(5));
			InstallPlugIn(new FSEye.PlugIn.UpdateNetworkCard(10));
			InstallPlugIn(new FSEye.PlugIn.UpdateDisk(10));
			InstallPlugIn(new FSEye.PlugIn.UpdateProcess(10));
			InstallPlugIn(new FSEye.PlugIn.UpdatePlugIn(10));
			InstallPlugIn(new FSEye.PlugIn.FileManage());
			InstallPlugIn(new FSEye.PlugIn.GameServerDb());
			InstallPlugIn(new FSEye.PlugIn.GameServerControl());
            InstallPlugIn(new FSEye.PlugIn.E2gSendfile());
            InstallPlugIn(new FSEye.PlugIn.ExitOrder());
            InstallPlugIn(new FSEye.PlugIn.E2gUpdateserver());
            InstallPlugIn(new FSEye.PlugIn.E2gDownload());
            InstallPlugIn(new FSEye.PlugIn.E2gServerid());
            InstallPlugIn(new FSEye.PlugIn.E2gServerPID());

          //  if (this.Type == ServerType.GameServer)
            InstallPlugIn(new FSEye.PlugIn.LordControl(10, 40));

			//InstallPlugIn(new FSEye.PlugIn.MessageLog(string.Format("D:\\fseyelog\\protocol_{0}.txt", Name)));
			//TODO 暂时屏蔽掉插件配置
			//LoadServerPlugIn();
		}

		/// <summary>
		/// 发送数据
		/// </summary>
		public bool SendData(byte[] data)
		{
			return AdminServer.TheInstance.ConnectionManager.SendTo(_connectionId, data);//romandouUtilDllWrap.RC4Encrypt(data, _communicationKey));
		}

		/// <summary>
		/// 发送消息
		/// </summary>
		public bool SendMessage(IProtocol message)
		{            
            if (IsConnected && SendData(message.ToBytes()))
            {
                if (MessageSent != null)
                {
                    MessageSent(this, message);
                }
                return true;
            }
            else
            {
                return false;
            }
		}

		public void AddFMTask(FileManageTask task)
		{
			if (task != null)
			{
				lock (_fileManageTaskList.SyncRoot)
				{
					_fileManageTaskList.Add(task);
					task.State = FileManageTask.TaskState.Queuing;
					if (_fileManageTaskList.Count == 1)
					{
                        //改了一下,当文件任务不成功时从任务列表中删除
						if(!task.Start(this))RemoveFMTask(0);
						_isIdle = false;
					}
				}
			}
		}

		public void RemoveFMTask(int index)
		{
			lock (_fileManageTaskList.SyncRoot)
			{
				if (index >= 0 && index < _fileManageTaskList.Count)
				{
					FileManageTask task = _fileManageTaskList[index] as FileManageTask;
					if (task != null)
					{
						if (task.State == FileManageTask.TaskState.Processing || task.State == FileManageTask.TaskState.Paused)
						{
							task.Stop(this);
						}
						else
						{
							_fileManageTaskList.RemoveAt(index);
							task.Dispose();

							if (_fileManageTaskList.Count == 0)
							{
								_isIdle = true;
							}
							else
							{
								if (index == 0)
								{
									(_fileManageTaskList[0] as FileManageTask).Start(this);
								}
							}
						}
					}
				}
			}
		}

		public void PauseFMTask()
		{
		}

		public void ContinueTasks()
		{
			lock (_fileManageTaskList.SyncRoot)
			{
				if (_fileManageTaskList.Count > 0)
				{
					(_fileManageTaskList[0] as FileManageTask).Continue(this);
				}
			}
		}

		public void PauseTasks()
		{
			lock (_fileManageTaskList.SyncRoot)
			{
				if (_fileManageTaskList.Count > 0)
				{
					(_fileManageTaskList[0] as FileManageTask).Pause(this);
				}
			}
		}

		public void Disconnect()
		{
			if (_connected)
			{
				AdminServer.TheInstance.ConnectionManager.Disconnect(_connectionId);
			}
		}

		private void ClearHistoryInfo()
		{
			_diskInfos.Clear();
			_memoryInfoQueue.Clear();
			_networkCards.Clear();
			_cpuInfos.Clear();
			_processInfoList.Clear();
			_plugInInfoList.Clear();
			_gameInfoQueue.Clear();
            _diskInfos.Clear();
		}

		public void Dispose()
		{
			if (!_disposed)
			{
				_disposed = true;
				UninstallAllPlugIn();
				if (_connected)
				{
					Disconnect();
				}
                //释放所有文件资源
                lock (_fileManageTaskList.SyncRoot)
                {
                    if (_fileManageTaskList.Count > 0)
                    {
                        (_fileManageTaskList[0] as FileManageTask).Dispose();
                    }
                }
			}
		}

		~GameServer()
		{
			Dispose();
		}

		public override string ToString()
		{
			return string.Format("GameServer {0} ({1})", this.Name, this.Comment);
		}

		/// <summary>
		/// 添加消息
		/// </summary>
		public void AppendMessage(string message, GameServer.MessageInfo.Type type)
		{
			if (message == null)
				throw new ArgumentNullException("message");

			lock (_recentMessageQueue)
			{
				if (_recentMessageQueue.Count >= RecentMessageQueueLength)
					_recentMessageQueue.Dequeue();

				MessageInfo info = new MessageInfo();
				info.Message = message;
				info.Time = DateTime.Now;
				info.MessageType = type;
				_recentMessageQueue.Enqueue(info);
			}
		}
		#endregion

		#region 事件
		/// <summary>
		/// 已连接
		/// </summary>
		public void OnConnected(long connectionId, byte[] communicationKey, int port)
		{
            try
            {
                lock (_syncRoot)
                {
                    _connected = true;
                    _port = port;
                    _connectionId = connectionId;
                    if (communicationKey != null)
                    {
                        _communicationKey = new byte[communicationKey.Length];
                        Array.Copy(communicationKey, _communicationKey, communicationKey.Length);
                    }
                    _serverState = ServerState.Running;
                    ContinueTasks();

                    AppendMessage("Connected", GameServer.MessageInfo.Type.Success);

                    AutomationContext context = new AutomationContext();
                    context.Message = string.Format("{0} Connected", this.ToString());
                    context.Server = this;
                    AdminServer.TheInstance.StrategyManager.OnEvent(FSEyeEvent.ServerConnected, context);

                    if (Connected != null)
                    {
                        Connected(this);
                    }
                }
            }
            catch (Exception ex)
            {
                Util.DebugLog(string.Format("Exception in Server[{0}] OnConnected : {1}", this.Name, ex.Message));
            }
		}

		/// <summary>
		/// 断开连接
		/// </summary>
		public void OnDisconnected()
		{
            try
            {
                lock (_syncRoot)
                {
                    _connected = false;
                    _port = 0;
                    _serverState = ServerState.Unknown;
                    PauseTasks();
                    ClearHistoryInfo();

                    AppendMessage("Disconnected", GameServer.MessageInfo.Type.Failure);

                    AutomationContext context = new AutomationContext();
                    context.Message = string.Format("{0} Disconnected", this.ToString());
                    context.Server = this;
                    AdminServer.TheInstance.StrategyManager.OnEvent(FSEyeEvent.ServerDisconnected, context);

                    if (Disconnected != null)
                    {
                        Disconnected(this);
                    }
                }
            }
            catch (Exception ex)
            {
                Util.DebugLog(string.Format("Exception in Server[{0}] OnDisconnected : {1}", this.Name, ex.Message));
            }
		}

		/// <summary>
		/// 系统时钟
		/// </summary>
		public void OnTick(object sender, System.Timers.ElapsedEventArgs e)
		{
            try
            {
                if (Tick != null)
                {
                    Tick(this);
                }
            }
            catch (Exception ex)
            {
                Util.DebugLog(string.Format("Exception in Server[{0}] OnTick : {1}", this.Name, ex.Message));
            }
		}

		/// <summary>
		/// 收到消息
		/// </summary>
		public void OnMessageReceived(IProtocol message)
		{
            try
            {
                if (MessageReceived != null)
                {
                    MessageReceived(this, message);
                }
            }
            catch (Exception ex)
            {
                Util.DebugLog(string.Format("Exception in Server[{0}] OnMessageReceived : {1}", this.Name, ex.Message));
            }
		}

		/// <summary>
		/// 游戏启动
		/// </summary>
		public void OnGameStart()
		{

		}
		#endregion

		#region 插件相关
		/// <summary>
		/// 安装插件
		/// </summary>
		public void InstallPlugIn(IPlugIn plugIn)
		{
			if (plugIn != null)
			{
				_installedPlugInList.Add(plugIn);
				_installedPlugInMap.Add(plugIn.Guid, plugIn);
				plugIn.Initialize(this);
			}
		}

		/// <summary>
		/// 卸载插件
		/// </summary>
		/// <param name="plugIn"></param>
		public void UnInstallPlugIn(IPlugIn plugIn)
		{
			if (plugIn != null)
			{
				_installedPlugInList.Remove(plugIn);
				_installedPlugInMap.Remove(plugIn.Guid);
				plugIn.Dispose();
			}
		}


		/// <summary>
		/// 卸载所有插件
		/// </summary>
		void UninstallAllPlugIn()
		{
			lock (_installedPlugInList.SyncRoot)
			{
				foreach (IPlugIn plugIn in _installedPlugInList)
				{
					plugIn.Dispose();
				}

				_installedPlugInList.Clear();
				_installedPlugInMap.Clear();
			}
		}

		/// <summary>
		/// 读取GameServer的插件配置并安装
		/// </summary>
		void LoadServerPlugIn()
		{
			lock (this)
			{
				IBlazeDatabase db = null;
				try
				{
					db = DbFactory.GetDatabase();
					IBlazeTable table = db.GetTable(TableString.ServerPlugInTableName);

					DataSet data = new DataSet();
					table.Get(data, FilterFactory.CreateEqualFilter(TableString.ServerPlugInFieldGameServerId, this.Id));
					DataRowCollection rows = data.Tables[TableString.ServerPlugInTableName].Rows;
					for (int i = 0; i < rows.Count; i++)
					{
						DataRow row = rows[i];
						string plugInGuid = row[TableString.ServerPlugInFieldPlugInGuid] as string;
						int para1 = (int)row[TableString.ServerPlugInFieldParameter1];
						int para2 = (int)row[TableString.ServerPlugInFieldParameter2];
						string para3 = row[TableString.ServerPlugInFieldParameter3] as string;
						InstallPlugIn(ServerPlugInFactory.GetPlugIn(plugInGuid, para1, para2, para3));
					}
				}
				catch (Exception)
				{
				}
				finally
				{
					if (db != null)
					{
						db.Close();
					}
				}
			}
		}

		/// <summary>
		/// 得到插件数据
		/// </summary>
        /// <param name="userId">用户ID</param>
		/// <param name="plugInGuid">插件GUID</param>
		/// <param name="dataKey">数据键</param>
		/// <param name="args">参数</param>
		public object GetPlugInData(int userId,string plugInGuid, string dataKey, params object[] args)
		{
			if (plugInGuid == null)
				throw new ArgumentNullException("plugInGuid");
			if (dataKey == null)
				throw new ArgumentNullException("dataKey");

            /*if (this.Type != ServerType.GameServer && plugInGuid.Equals(LordControl.PlugInGuid))
            {
                //只有GameServer才含有LordControl插件
                return null;
            }*/

			IPlugIn plugIn = _installedPlugInMap[plugInGuid] as IPlugIn;
			if (plugIn == null)
			{
				throw new Exception(string.Format("PlugIn {0} does not exist.", plugInGuid));
			}
            
			return plugIn.GetData(userId,dataKey, args);
		}


		/// <summary>
		/// 执行插件行为
		/// </summary>
        /// <param name="userId">用户ID</param>
		/// <param name="plugInGuid">插件GUID</param>
		/// <param name="actionKey">行为键</param>
		/// <param name="args">参数</param>
		public bool DoPlugInAction(int userId,string plugInGuid, string actionKey, params object[] args)
		{
			object outArg = null;
			return DoPlugInAction(userId,plugInGuid, actionKey, ref outArg, args);
		}

		/// <summary>
		/// 执行插件行为
		/// </summary>
        /// <param name="userId">用户ID</param>
		/// <param name="plugInGuid">插件GUID</param>
		/// <param name="actionKey">行为键</param>
		/// <param name="outArg">传出参数</param>
		/// <param name="args">参数</param>
		public bool DoPlugInAction(int userId,string plugInGuid, string actionKey, ref object outArg, params object[] args)
		{
			if (plugInGuid == null)
				throw new ArgumentNullException("plugInGuid");
			if (actionKey == null)
				throw new ArgumentNullException("actionKey");

            //TODO:似乎我们不需要下面代码
            //if (this.Type != ServerType.GameServer && plugInGuid.Equals(LordControl.PlugInGuid))
            //{
            //    //只有GameServer才含有LordControl插件
            //    return false;
            //}

			IPlugIn plugIn = _installedPlugInMap[plugInGuid] as IPlugIn;
			if (plugIn == null)
			{
                throw new Exception(string.Format("PlugIn {0} does not exist.", plugInGuid));
			}

			return plugIn.DoAction(userId,actionKey, ref outArg, args);
		}
		#endregion

		#region 更新信息
		public void UpdateCpuBasicInfo(cpubaseinfo[] infos)
		{
			lock (_cpuInfos.SyncRoot)
			{
				_cpuInfos.Clear();
				for (int i = 0; i < infos.Length; i++)
				{
					_cpuInfos.Add(new CpuInfo(infos[i]));
				}
			}
		}

		public void UpdateCpuLoad(ushort[] loads)
		{
			lock (_cpuInfos.SyncRoot)
			{
				if (loads.Length == _cpuInfos.Count)
				{
					for (int i = 0; i < loads.Length; i++)
					{
						(_cpuInfos[i] as CpuInfo).UpdateLoad(loads[i]);
					}
				}
			}
		}

		public void UpdateProcessInfo(procinfo[] infos)
		{
			if (infos != null)
			{
				lock (_processInfoList.SyncRoot)
				{
					_processInfoList.Clear();
					for (int i = 0; i < infos.Length; i++)
					{
						_processInfoList.Add(new ProcessInfo(infos[i]));
					}
				}
			}
		}

		public void UpdatePlugInInfo(pluginfo[] infos)
		{
			if (infos != null)
			{
				lock (_plugInInfoList.SyncRoot)
				{
					_plugInInfoList.Clear();
					for (int i = 0; i < infos.Length; i++)
					{
						_plugInInfoList.Add(new PlugInInfo(infos[i]));
					}
				}
			}
		}

		public void UpdateMemoryInfo(uint free, uint total)
		{
			lock (_memoryInfoQueue.SyncRoot)
			{
				if (_memoryInfoQueue.Count >= MAX_MEMORY_INFO_HISTORY_RECORD_COUNT)
				{
					_memoryInfoQueue.Dequeue();
				}
				_memoryInfoQueue.Enqueue(new MemoryInfo(free, total));
			}
		}

		public void UpdateDiskInfo(diskbaseinfo[] infos)
		{
			lock (_diskInfos.SyncRoot)
			{
				_diskInfos.Clear();
				for (int i = 0; i < infos.Length; i++)
				{
					_diskInfos.Add(new DiskInfo(infos[i]));
				}
			}
		}

		public void UpdateNetcardInfo(string hostName, string systemName, netcardinfo[] cardInfos)
		{
			lock (_syncRoot)
			{
				_hostName = hostName;
				_systemName = systemName;
				_operationSystem = OSType.Linux;
				if (_systemName != null)
				{
					if (_systemName.Contains("windows"))
					{
						_operationSystem = OSType.Windows;
					}
					else if (_systemName.Contains("linux"))
					{
						_operationSystem = OSType.Linux;
					}
				}

				lock (_networkCards.SyncRoot)
				{
					_networkCards.Clear();
					for (int i = 0; i < cardInfos.Length; i++)
					{
						NetworkCardInfo card = new NetworkCardInfo(cardInfos[i]);
						_networkCards.Add(card);
					}
				}
			}
		}

		public void UpdateNetworkLoad(netcardload[] cardLoads)
		{
			lock (_networkCards.SyncRoot)
			{
				if (cardLoads != null && cardLoads.Length == _networkCards.Count)
				{
					for (int i = 0; i < cardLoads.Length; i++)
					{
						NetworkCardInfo cardInfo = _networkCards[i] as NetworkCardInfo;
						cardInfo.UpdateLoad(cardLoads[i]);
					}
				}
			}
		}

		public void UpdateGameServiceState(ServiceState state)
		{
			lock (_syncRoot)
			{
				_serviceState = state;
				//TODO 也许需要提醒其他的对象
			}
		}
		#endregion
	}
	#endregion

	#region CpuInfo
	/// <summary>
	/// CPU信息
	/// </summary>
	public class CpuInfo : cpubaseinfo
	{
		public const int LoadQueueMaxLength = 100;

		Queue<ushort> _loadQueue;

		public CpuInfo()
		{
			_loadQueue = new Queue<ushort>(LoadQueueMaxLength);
		}

		public CpuInfo(cpubaseinfo info)
			: this()
		{
			this.szCPUName = info.szCPUName;
			this.szCPUVendor = info.szCPUVendor;
			this.szCPUDesc = info.szCPUDesc;
			this.nCPULoad = info.nCPULoad;
		}

		public string Name
		{
			get { return szCPUName; }
		}

		public string Vendor
		{
			get { return szCPUVendor; }
		}

		public string Desc
		{
			get { return szCPUDesc; }
		}

		public ushort[] Loads
		{
			get { return _loadQueue.ToArray(); }
		}

		public void UpdateLoad(ushort load)
		{
			if (_loadQueue.Count >= LoadQueueMaxLength)
			{
				_loadQueue.Dequeue();
			}
			_loadQueue.Enqueue(load);
		}
	}
	#endregion

	#region NetworkCardInfo
	/// <summary>
	/// 网卡信息
	/// </summary>
	public class NetworkCardInfo : netcardinfo
	{
		public const int LoadQueueMaxLength = 100;

		Queue<netcardload> _loadQueue;

		public string _desc;

		public string _ip;

		public string _mac;

		public string _mask;

		uint _rxSize;

		uint _txSize;

		uint _rxRate;

		uint _txRate;

		public NetworkCardInfo(netcardinfo info)
		{
			this._desc = info.szCardDesc;
			this._ip = info.szIP;
			this._mac = info.szMac;
			this._mask = info.szMask;
			_loadQueue = new Queue<netcardload>();
		}

		public string Desc
		{
			get { return _desc; }
		}

		public string Ip
		{
			get { return _ip; }
		}

		public string Mac
		{
			get { return _mac; }
		}

		public string Mask
		{
			get { return _mask; }
		}

		public uint RXSize
		{
			get { return _rxSize; }
		}

		public uint TXSize
		{
			get { return _txSize; }
		}

		public uint RXRate
		{
			get { return _rxRate; }
		}

		public uint TXRate
		{
			get { return _txRate; }
		}

		public netcardload[] Loads
		{
			get { return _loadQueue.ToArray(); }
		}

		public void UpdateLoad(netcardload load)
		{
			if (_loadQueue.Count >= LoadQueueMaxLength)
			{
				_loadQueue.Dequeue();
			}
			_loadQueue.Enqueue(load);
			_txSize = load.nTXSize;
			_rxSize = load.nRXSize;
			_txRate = load.nTXRate;
			_rxRate = load.nRXRate;
		}
	}
	#endregion

	#region DiskInfo
	/// <summary>
	/// 磁盘信息
	/// </summary>
	public class DiskInfo : diskbaseinfo
	{
		public DiskInfo(diskbaseinfo info)
		{
			this.szDiskDesc = info.szDiskDesc;
			this.nFreeSize = info.nFreeSize;
			this.nTotalSize = info.nTotalSize;
		}

		public string Name
		{
			get { return base.szDiskDesc; }
		}

		public uint FreeSpace
		{
			get { return base.nFreeSize; }
		}

		public uint TotalSpace
		{
			get { return base.nTotalSize; }
		}
	}
	#endregion

	#region ProcessInfo
	/// <summary>
	/// 进程信息
	/// </summary>
	public class ProcessInfo : procinfo
	{
		public ProcessInfo(procinfo info)
		{
			if (info != null)
			{
				this.nPID = info.nPID;
				this.nCPUTime = info.nCPUTime;
				this.nMemUse = info.nMemUse;
				this.nThreadCount = info.nThreadCount;
				this.nVMSize = info.nVMSize;
				this.szProcName = info.szProcName;
			}
			else
			{
				this.nPID = 0;
				this.nCPUTime = 0;
				this.nMemUse = 0;
				this.nThreadCount = 0;
				this.nVMSize = 0;
				this.szProcName = string.Empty;
			}
		}

		public uint Pid
		{
			get { return this.nPID; }
		}

		public uint CpuTime
		{
			get { return this.nCPUTime; }
		}

		public uint MemUse
		{
			get { return this.nMemUse; }
		}

		public uint VmSize
		{
			get { return this.nVMSize; }
		}

		public uint ThreadCount
		{
			get { return this.nThreadCount; }
		}

		public string ProcName
		{
			get { return this.szProcName; }
		}
	}
	#endregion

	#region PlugInInfo
	/// <summary>
	/// 插件信息
	/// </summary>
	public class PlugInInfo
	{
		private string _guid;

		private string _desc;

		private string _author;

		private string _moduleInfo;

		private string _path;

		private string _version;

		public string Desc
		{
			get { return _desc; }
		}

		public string Author
		{
			get { return _author; }
		}

		public string Guid
		{
			get { return _guid; }
		}

		public string ModuleInfo
		{
			get { return _moduleInfo; }
		}

		public string Path
		{
			get { return _path; }
		}

		public string Version
		{
			get { return _version; }
		}

		public PlugInInfo(pluginfo info)
		{
			_desc = info.szDesc;
			_author = info.szAuthor;
			_guid = info.szGUID;
			_moduleInfo = info.szModHAndModE;
			_path = info.szPath;
			_version = info.szVersion;
		}
	}
	#endregion

	#region MemoryInfo
	/// <summary>
	/// 内存信息
	/// </summary>
	public class MemoryInfo
	{
		uint _freeSize;

		uint _totalSize;

		public MemoryInfo(uint free, uint total)
		{
			_freeSize = free;
			_totalSize = total;
		}

		public uint FreeSize
		{
			get { return _freeSize; }
		}

		public uint TotalSize
		{
			get { return _totalSize; }
		}

		public uint UsedSize
		{
			get { return _totalSize - _freeSize; }
		}

		public float FreePercentage
		{
			get { return ((float)_freeSize) / _totalSize; }
		}

		public float UsedPercentage
		{
			get { return ((float)(_totalSize - _freeSize)) / _totalSize; }
		}
	}
	#endregion

    #region GameCenterConfig
    /// <summary>
    /// GameCenter配置
    /// </summary>
    public class GameCenterConfig
    {
        public static string DefaultConfigFileName = "DefaultGameServerConfig.xml";
        public string ConfigString = "";
    }
    #endregion

    #region GameServerConfig
    /// <summary>
    /// GameServer配置
    /// </summary>
    public class GameServerConfig
    {
        public static string DefaultConfigFileName = "DefaultGameServerConfig.xml";
        public string ConfigString = "";
    }
    #endregion

    #region GoddessConfig
    /// <summary>
    /// Goddess配置
    /// </summary>
    public class GoddessConfig
    {
        public static string DefaultConfigFileName = "DefaultGoddessConfig.xml";
        public string ConfigString = "";
    }
    #endregion

    #region BishopConfig
    /// <summary>
    /// BishopConfig配置
    /// </summary>
    public class BishopConfig
    {
        public static string DefaultConfigFileName = "DefaultBishopConfig.xml";
        public string ConfigString = "";
    }
    #endregion

    #region GuardConfig
    /// <summary>
	/// Lord配置
	/// </summary>
	public class LordConfig
	{
        public static string DefaultConfigFileName = "DefaultGuardConfig.xml";

		public int ServerCount=0;
        public int MaxPlayer = SystemConfig.Current.ServerListServerGroupMaxPlayer;
		public string IntranetMask;
		public string InternetMask;
        public string ServerIDs = "";

		public string PaysysIp;
		public int PaysysPort;
		public string PaysysLoginName;
		public string PaysysLoginPwd;

		public string DbHost;
		public string DbName;
		public string DbLoginName;
		public string DbLoginPwd = string.Empty;
		public string DbBackupPath;

		public string FSEyeIp;
		public int FSEyePort;
        public string BindIp;
        

        //new fields
        public string GMCIp;
        public string GMCAccount;
        public const int GMCPort = 7777;
        
        #region 简单默认设置
        const int DefaultServerCount = 0;
        const int DefaultMaxPlayer = 7000;
        const string DefaultIntranetMask = "255.255.255.0";
        const string DefaultInternetMask = "255.255.255.1";
        const string DefaultServerID = "";

        const string DefaultPaysysIp = "192.168.1.1";
        const int DefaultPaysysPort = 3333;
        const string DefaultPaysysLoginName = "paysys";
        const string DefaultPaysysLoginPwd = "";

        const string DefaultDbHost = "192.168.1.1";
        const string DefaultDbName = "fsonline2";
        const string DefaultDbLoginName = "zola";
        const string DefaultDbLoginPwd = "";
        const string DefaultDbBackupPath = "/tmp";
        #endregion

        public LordConfig()
        {
        }

        /// <summary>
        /// 这个Copy好象只是浅拷贝
        /// </summary>
        /// <param name="config"></param>
        public void Copy(LordConfig config)
		{
            if (config.ServerCount >= 0)
                ServerCount = config.ServerCount;
            if (config.MaxPlayer >= 0)
			    MaxPlayer = config.MaxPlayer;
            if (config.IntranetMask != null)
			    IntranetMask = config.IntranetMask;
            if (config.InternetMask != null)
			    InternetMask = config.InternetMask;
            if (config.ServerIDs != null)
                ServerIDs = config.ServerIDs;
            if (config.PaysysIp != null)
			    PaysysIp = config.PaysysIp;
            if (config.PaysysPort>=0)
			    PaysysPort = config.PaysysPort;
            if (config.PaysysLoginName != null)
			    PaysysLoginName = config.PaysysLoginName;
            if (config.PaysysLoginPwd != null)
			    PaysysLoginPwd = config.PaysysLoginPwd;
            if (config.DbHost != null)
			    DbHost = config.DbHost;
            if (config.DbName != null)
			    DbName = config.DbName;
            if (config.DbLoginName != null)
			    DbLoginName = config.DbLoginName;
            if ((config.DbLoginPwd != null) && (config.DbLoginPwd.Length != 0))
			    DbLoginPwd = config.DbLoginPwd;
            if (config.DbBackupPath != null)
			    DbBackupPath = config.DbBackupPath;
            if (config.FSEyeIp != null)
			    FSEyeIp = config.FSEyeIp;
            if (config.FSEyePort >=0)
			    FSEyePort = config.FSEyePort;
            if ((config.BindIp != null) && (config.BindIp.Length != 0))
                BindIp = config.BindIp;
            if ((config.GMCIp != null) && (config.GMCIp.Length != 0))
                GMCIp = config.GMCIp;
            if ((config.GMCAccount != null) && (config.GMCAccount.Length != 0))
                GMCAccount = config.GMCAccount;
			//FSEyeLoginPwd = config.FSEyeLoginPwd;
		}

        /// <summary>
        /// 从配置文件中读取默认配置
        /// </summary>
        /// <returns>从文件中读取配置的标识,为true则配置从文件中读取,为false则用LordConfig中的简单默认配置</returns>
        public static LordConfig LoadFromDefaultConfig()
        {
            
            if (File.Exists(SystemConfig.Current.GameSettingDir + DefaultConfigFileName))
            {
                FileStream fileStream = new FileStream(SystemConfig.Current.GameSettingDir + DefaultConfigFileName, FileMode.OpenOrCreate);
                XmlTextReader reader = null;
                LordConfig lordConfig = new LordConfig();
                try
                {
                    reader = new XmlTextReader(fileStream);
                    if (reader != null)
                    {
                        reader.Read();
                        lordConfig.FSEyeIp = reader.GetAttribute("FSEyeIp");
                        lordConfig.FSEyePort =  int.Parse(reader.GetAttribute("FSEyePort"));

                        try
                        {
                            lordConfig.PaysysIp = reader.GetAttribute("PaysysIp");
                            lordConfig.PaysysPort = int.Parse(reader.GetAttribute("PaysysPort"));
                            lordConfig.PaysysLoginName = reader.GetAttribute("PaysysLoginName");
							lordConfig.PaysysLoginPwd = UtilDllWrap.DBEncrypt(reader.GetAttribute("PaysysLoginPwd"), SystemConfig.Current.DefaultEncoding);
                        }
                        catch (Exception)
                        { }

                        try
                        {
                            lordConfig.DbHost = reader.GetAttribute("DbHost");
                            lordConfig.DbName = reader.GetAttribute("DbName");
                            lordConfig.DbBackupPath = reader.GetAttribute("DbBackupPath");
                            lordConfig.DbLoginName = reader.GetAttribute("DbLoginName");
                            lordConfig.DbLoginPwd = reader.GetAttribute("DbLoginPwd");// UtilDllWrap.DBEncrypt(reader.GetAttribute("DbLoginPwd"), SystemConfig.Current.DefaultEncoding);
                        }
                        catch (Exception)
                        { }
                    }
                }
                catch (Exception)
                {                    
                }
                finally
                {
                    if (reader != null) reader.Close();
                }

                return lordConfig;
            }
            else
            {
                //默认简单配置
                LordConfig lordConfig = new LordConfig();
                lordConfig.ServerCount = DefaultServerCount;
                lordConfig.PaysysIp = DefaultPaysysIp;
                lordConfig.PaysysPort = DefaultPaysysPort;
                lordConfig.PaysysLoginName = DefaultPaysysLoginName;
                lordConfig.PaysysLoginPwd = DefaultPaysysLoginPwd;
                lordConfig.DbHost = DefaultDbHost;
                lordConfig.DbName = DefaultDbName;
                lordConfig.DbLoginName = DefaultDbLoginName;
                lordConfig.DbLoginPwd = DefaultDbLoginPwd;
                lordConfig.DbBackupPath = DefaultDbBackupPath;
                lordConfig.FSEyeIp = SystemConfig.Current.AdminServerListenIP;
                lordConfig.FSEyePort = SystemConfig.Current.AdminServerListenPort;
                return lordConfig;
            }

        }

        /// <summary>
        /// 保存为默认设置,写入fseyesetting目录下DefaultGuardConfig.xml
        /// </summary>
        /// <returns></returns>
        public static bool SaveAsDefaultConfig(LordConfig lord)
        {
            FileStream fileStream = new FileStream(SystemConfig.Current.GameSettingDir + DefaultConfigFileName, FileMode.OpenOrCreate);
            XmlTextWriter writer = null;
            try
            {
                fileStream.SetLength(0);
                writer = new XmlTextWriter(fileStream, Encoding.Default);
                writer.WriteStartElement("DefaultLordConfig");

                if (lord.PaysysIp != null && lord.PaysysIp.Length != 0)
                {
                    writer.WriteAttributeString("PaysysIp", lord.PaysysIp);
                    writer.WriteAttributeString("PaysysPort", lord.PaysysPort.ToString());
                    writer.WriteAttributeString("PaysysLoginName", lord.PaysysLoginName);
					if (lord.PaysysLoginPwd != null && lord.PaysysLoginPwd.Length != 0) writer.WriteAttributeString("PaysysLoginPwd", UtilDllWrap.DBEncrypt(lord.PaysysLoginPwd, SystemConfig.Current.DefaultEncoding));
                }
                if (lord.DbHost != null && lord.DbHost.Length != 0)
                {
                    writer.WriteAttributeString("DbHost", lord.DbHost);
                    writer.WriteAttributeString("DbName", lord.DbName);
                    writer.WriteAttributeString("DbBackupPath", lord.DbBackupPath);
                    writer.WriteAttributeString("DbLoginName", lord.DbLoginName);
					if (lord.DbLoginPwd != null && lord.DbLoginPwd.Length != 0) writer.WriteAttributeString("DbLoginPwd", UtilDllWrap.DBEncrypt(lord.DbLoginPwd, SystemConfig.Current.DefaultEncoding));
                }
                writer.WriteAttributeString("FSEyeIp", lord.FSEyeIp);
                writer.WriteAttributeString("FSEyePort", lord.FSEyePort.ToString());

                writer.WriteEndElement();
                return true;
            }
            catch (Exception)
            {
                return false;
            }
            finally
            {
                if (writer != null) writer.Close();
            }
        }

        /// <summary>
        /// 写Lord配置
        /// </summary>		
        public void WriteLordConfig(Stream outputStream)
        {
            WriteLordConfig(outputStream, null);
        }

		/// <summary>
		/// 写Lord配置
		/// </summary>
		/// <remarks>指定编码</remarks>
		public void WriteLordConfig(Stream outputStream, Encoding encoding)
		{
			if (outputStream == null)
				throw new ArgumentNullException("outputStream");

			using (TextWriter writer = new StreamWriter(outputStream, (encoding != null ? encoding : SystemConfig.Current.DefaultEncoding)))
			{
				writer.NewLine = SystemConfig.Current.DefaultNewLine;

				writer.WriteLine("#//////////////////////////////////////////////////////////////////////");
				writer.WriteLine("#//      Kingsoft Copyright (C) 2007");
				writer.WriteLine("#//      Created_datetime : " + DateTime.Now);
				writer.WriteLine("#//      File_base        : lord");
				writer.WriteLine("#//      File_ext         : .cfg");
				writer.WriteLine("#//      Author           : huangbiao");
				writer.WriteLine("#//      Description      : Lord Config File");
				writer.WriteLine("#//////////////////////////////////////////////////////////////////////");
				writer.WriteLine();
                
                WriteServerConfigSection(writer);

				writer.WriteLine();

				WritePaysysConfigSection(writer);
								
				writer.WriteLine();

				WriteDbConfigSection(writer);
                writer.WriteLine();
			}
		}

		/// <summary>
		/// 写Guard配置
		/// </summary>
		public void WriteGuardConfig(Stream outputStream)
		{
			WriteGuardConfig(outputStream, null);
		}

		/// <summary>
		/// 写Guard配置
		/// </summary>
		/// <remarks>指定编码</remarks>
		public void WriteGuardConfig(Stream outputStream, Encoding encoding)
		{
			if (outputStream == null)
				throw new ArgumentNullException("outputStream");

			using (TextWriter writer = new StreamWriter(outputStream, (encoding != null ? encoding : SystemConfig.Current.DefaultEncoding)))
			{
				writer.NewLine = SystemConfig.Current.DefaultNewLine;

				WriteFSEyeConfigSection(writer);
                writer.WriteLine();
                WritePaysysConfigSection(writer);
                writer.WriteLine();
                WriteDbConfigSection(writer);
                writer.WriteLine();
                WriteServerConfigSection(writer);
                writer.WriteLine();
                WriteGMCConfigSection(writer);
			}
		}

        void WriteGMCConfigSection(TextWriter writer)
        {
            foreach (GameServer server in AdminServer.TheInstance.GameServerManager.Servers)
            {
                if ((server.IpAddress == this.BindIp) && (server.Type == GameServer.ServerType.gamecenter))
                {
                    if ((server.LordConfig.GMCIp != null) && (server.LordConfig.GMCIp.Length != 0) && (server.LordConfig.GMCAccount != null) && (server.LordConfig.GMCAccount.Length != 0))
                    {
                        writer.WriteLine("[GMC]");
                        writer.WriteLine("IP=" + server.LordConfig.GMCIp);
                        writer.WriteLine("Port=" + LordConfig.GMCPort.ToString());
                        writer.WriteLine("Account=" + server.LordConfig.GMCAccount);
                        writer.WriteLine("Password=" + "1234");
                        writer.WriteLine("GCServer=" + server.Id.ToString());
                    }
                    else
                    {
                        writer.WriteLine("[GMC]");
                        writer.WriteLine("出现异常，GameCenter的GMC地址和GMC帐号为空");
                    }
                        
                    return;
                }
            }
        }

		void WriteServerConfigSection(TextWriter writer)
		{
            writer.WriteLine("[Server]");
            writer.WriteLine("Count=" + ServerCount);
            
            if (ServerIDs == null || ServerIDs.Length == 0)
            {
                return;
            }
            else
            {
                string[] ServerIDArray = ServerIDs.Split(',');
                for (int i = 1; i <= ServerCount; i++)
                {
                    writer.WriteLine("Server" + i.ToString() + "=" + ServerIDArray[i-1].ToString());
                }
            }
		}

		void WritePaysysConfigSection(TextWriter writer)
		{
			if (PaysysIp == null || PaysysIp.Length == 0)
				return;

			writer.WriteLine("[Paysys]");
			if (PaysysIp.Length > 0)
				writer.WriteLine("IP=" + PaysysIp);
			else
				throw new Exception("Data missing: Paysys.IP");
			if (PaysysPort > 0)
				writer.WriteLine("Port=" + PaysysPort);
			else
				throw new Exception("Data missing: Paysys.Port");
			if (PaysysLoginName.Length > 0)
				writer.WriteLine("LoginName=" + PaysysLoginName);
			else
				throw new Exception("Data missing: Paysys.LoginName");
			if (PaysysLoginPwd.Length > 0)
				writer.WriteLine("LoginPwd=" + PaysysLoginPwd);
			else
				throw new Exception("Data missing: Paysys.LoginPwd");
		}

		void WriteDbConfigSection(TextWriter writer)
		{
			if (DbHost == null || DbHost.Length == 0)
				return;

			writer.WriteLine("[DBSecure]");
			if (DbHost.Length > 0)
				writer.WriteLine("HostName=" + DbHost);
			else
				throw new Exception("Data missing: DBSecure.HostName");
			if (DbName.Length > 0)
				writer.WriteLine("DBName=" + DbName);
			else
				throw new Exception("Data missing: DBSecure.DBName");
			if (DbLoginName.Length > 0)
				writer.WriteLine("LoginName=" + DbLoginName);
			else
				throw new Exception("Data missing: DBSecure.LoginName");
			if (DbLoginPwd.Length > 0)
				writer.WriteLine("LoginPwd=" + DbLoginPwd);
			else
				throw new Exception("Data missing: DBSecure.LoginPwd");
		}

		void WriteFSEyeConfigSection(TextWriter writer)
		{
			writer.WriteLine("[FSEye]");
            if (FSEyeIp.ToString() != string.Empty && FSEyePort.ToString() != string.Empty)
            {
                if (FSEyeIp.Length > 0)
                    writer.WriteLine("IP=" + FSEyeIp);
                else
                    throw new Exception("Data missing: FSEye.IP");
                if (FSEyePort > 0)
                    writer.WriteLine("Port=" + FSEyePort);
                else
                    throw new Exception("Data missing: FSEye.Port");
                writer.WriteLine("BindIp=" + BindIp);               
                writer.WriteLine("CreateTime=" + DateTime.Now.ToString());
            }
		}
	}
	#endregion

	#region GameSetting
	/// <summary>
	/// 游戏设定
	/// </summary>
	public class GameSetting : IDisposable
	{
		GameServer _server;

		int[] _globalVariableArray = new int[100];

		/// <summary>
		/// 全局变量
		/// </summary>
		public int[] GlobalVariable
		{
			get { return _globalVariableArray; }
		}

		public GameSetting(GameServer server)
		{
			if (server == null)
				throw new ArgumentNullException("server");

			_server = server;
			_server.MessageReceived += new MessageReceivedEventHandler(ServerMessageReceived);
		}

		void ServerMessageReceived(GameServer server, IProtocol message)
		{
            if ((ProtocolDef)message.ProtocolId == ProtocolDef.l2e_header_def)
            {
                switch ((ProtocolDef)BitConverter.ToUInt16(message.ToBytes(), 4))
                {
                    case ProtocolDef.l2e_GetGlobalVariable_def:
                        {
                            l2e_GetGlobalVariable protocol = message as l2e_GetGlobalVariable;
                            UpdateGlobalVariable(protocol.VariableIndex, protocol.VariableValue);
                        }
                        break;
                    case ProtocolDef.l2e_ping_def:
                        {
                            
                            server.GameServiceState = GameServer.ServiceState.Running;
                            if (server.PID <= 0)
                            {
                                string[] para = new string[1] { server.Id.ToString() };
                                server.DoPlugInAction(SecurityManager.SystemAutomationId, E2gServerPID.PlugInGuid, E2gServerPID.actionKey, para);
                            }
                            break;
                        }
                }
            }
		}

		/// <summary>
		/// 刷新所有的全局变量
		/// </summary>
		/// <returns></returns>
		public bool RefreshAllGlobalVariables()
		{
			return false;
		}

		/// <summary>
		/// 刷新全局变量
		/// </summary>
		/// <param name="variableIndex">变量Index</param>
		public bool RefreshGlobalVariable(ushort variableIndex)
		{
			if (_server.IsConnected)
			{
				if (_server.GameServiceState == GameServer.ServiceState.Running)
				{
					e2l_GetGlobalVariable protocol = new e2l_GetGlobalVariable();
					protocol.VariableIndex = variableIndex;

					return _server.SendMessage(protocol);
				}
			}

			return false;
		}

		/// <summary>
		/// 设置全局变量
		/// </summary>
		/// <param name="variableIndex">变量Index</param>
		/// <param name="variableValue">变量值</param>
		/// <returns></returns>
		public bool SetGlobalVariable(ushort variableIndex, int variableValue)
		{
			if (_server.IsConnected)
			{
				if (_server.GameServiceState == GameServer.ServiceState.Running)
				{
					e2l_SetGlobalVariable protocol = new e2l_SetGlobalVariable();
					protocol.VariableIndex = variableIndex;
					protocol.VariableValue = variableValue;

					return _server.SendMessage(protocol);
				}
			}

			return false;
		}

		public void UpdateGlobalVariable(ushort variableIndex, int variableValue)
		{
			if (variableIndex >= _globalVariableArray.Length)
			{
				//TODO 扩大数组
			}

			_globalVariableArray[variableIndex] = variableValue;
		}

		public void Dispose()
		{
			_server.MessageReceived -= new MessageReceivedEventHandler(ServerMessageReceived);          
		}
	}
	#endregion

	#region Delegate
	public delegate void ConnectedEventHandler(GameServer server);

	public delegate void DisconnectedEventHandler(GameServer server);

	public delegate void TickEventHandler(GameServer server);

	public delegate void MessageReceivedEventHandler(GameServer server, IProtocol message);

	public delegate void MessageSentEventHandler(GameServer server, IProtocol message);

	public delegate void GameStateChangeEventHandler(GameServer server, GameServer.ServiceState originalState, GameServer.ServiceState newState);
	#endregion
}