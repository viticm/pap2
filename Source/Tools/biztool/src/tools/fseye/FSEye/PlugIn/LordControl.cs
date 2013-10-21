//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-3-20
//      File_base        : LordControl
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : Lord（游戏服务器）控制
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Collections;
using FSEye.DAL;
using FSEye.Security;

namespace FSEye.PlugIn
{
	public class LordControl : BasePlugIn
	{
		/// <summary>
		/// 玩家数量信息
		/// </summary>
		public class PlayerCountInfo
		{
			public int Count;
			public DateTime Time;
		}

		/// <summary>
		/// 玩家数量统计信息
		/// </summary>
		public class PlayerCountStatisticInfo
		{
			public int MaxCount = int.MinValue;
			public int MinCount = int.MaxValue;
			public int AverageCount = 0;
			public DateTime Time = DateTime.MinValue;
		}

		/// <summary>
		/// 游戏数据库状态
		/// </summary>
		public enum DatabaseStatus
		{
			Unknown,
			OK,
			Error
		}

		/// <summary>
		/// Paysys状态
		/// </summary>
		public enum PaysysStatus
		{
			Unknown,
			OK,
			Error
		}

		/// <summary>
		/// 游戏消息
		/// </summary>
		public class GameMessage
		{
			public string Sender;
			public string Receiver;
			public string Message;
			public DateTime Time;
		}

		public const string PlugInGuid = "{3b2635c2-64ed-48c3-8ae8-25eed4ddbc88}";

		public const string DataKeyGameRunningTime = "DataKeyGameRunningTime";
		public const string DataKeyPlayerCountQueue = "DataKeyPlayerCountQueue";
		public const string DataKeyCurrentPlayerCount = "DataKeyCurrentPlayerCount";
        //ahpho
        public const string DataKeyCustomInfo = "DataKeyCustomInfo";
        public const string DataKeyCurrentPlayerOfflineCount = "DataKeyCurrentPlayerOfflineCount";
		public const string DataKeyPlayerCountStatistic = "DataKeyPlayerCountStatistic";
		public const string DataKeyPlayerWho = "DataKeyPlayerWho";
		public const string DataKeyModuleErrorCode = "DataKeyModuleErrorCode";
		public const string DataKeyGameMessageQueue = "DataKeyGameMessageQueue";

        public const string DataKeyGMResult = "DataKeyGMResult";

        public const string DataKeyUserGMResult = "DataKeyUserGMResult";

		public const string ActionKeyGetGameRunningTime = "ActionKeyGetGameRunningTime";
		public const string ActionKeyExecuteGMCommand = "ActionKeyExecuteGMCommand";    //执行GM指令
		public const string ActionKeyRefreshPlayerWho = "ActionKeyRefreshPlayerWho";    //刷新在线玩家
		public const string ActionKeySayToWorld = "ActionKeySayToWorld";                //世界频道说话
		public const string ActionKeyMonitorGlobalChat = "ActionKeyMonitorGlobalChat";  //公聊监督
		public const string ActionKeySetModule = "ActionKeySetModule";

		public delegate void ModuleStateChangedDelegate(int module, int oldStateCode, int newStateCode);

		public event ModuleStateChangedDelegate ModuleStateChanged;             //游戏模块状态变化

		const int PlayerCountQueueLength = 60;

		const int GameMessageQueueLength = 100;

		const int MaxModuleCount = 10;                                          //最大游戏模块数量（用以显示模块错误码）

		const int UnknownErrorCode = -2;

        const int ChatMonitorTimeOutSeconds = 30;                               //公聊监控超时时间(秒)

		DateTime _lastPing;

		int _pingTimeOutTick;

		int _checkPingTickCurrent;

		int _checkPingTickDo;

		DateTime _gameRunningRefreshTime;

		TimeSpan _gameRunningTime;

        bool _chatMonitorSwitch = false;                                        //公聊监控开关

        DateTime _chatMonitorTimeStamp = DateTime.Now;                          //公聊监控时间戳

		bool _startingForgive = true;

        // ahpho
        string[] _customInfoStrs;

		Queue<PlayerCountInfo> _playerCountQueue = new Queue<PlayerCountInfo>(PlayerCountQueueLength);

        Queue<PlayerCountInfo> _playerOfflineCountQueue = new Queue<PlayerCountInfo>(PlayerCountQueueLength);

		int _lastPlayerCountStatisticHour = -1;

		IList _playerWhoList = ArrayList.Synchronized(new ArrayList());

		int[] _moduleErrorCodes = new int[MaxModuleCount];

		Queue<GameMessage> _gameMessageQueue = new Queue<GameMessage>(GameMessageQueueLength);

        private Object thisLock = new Object();
        
        //新增字段，从_GMResultCache取数据时的互斥锁
        object _syncRoot = new object();

        //新增hashtable,key是userId,value是_GMCount(_GMResultCache的key)
        //真正的指令结果通过value去_GMResultCache取
        Hashtable _GMResultIndexByUserId;

        //_GMResultCache的key
        uint _GMCount;
        
        //新增hashtable，存储GM指令返回的消息
        Hashtable _GMResultCache;

        public override object[] Parameters
        {
            set
            {
                _checkPingTickDo = int.Parse(value[1].ToString());
                _pingTimeOutTick = int.Parse(value[2].ToString());
            }
            get
            {
                return new object[]{
                    2,
                    _checkPingTickDo,
                    _pingTimeOutTick
                };
            }
        }
        
		public override string[] ParaDescription
        {
            get
            {
                return new string[]{
                    DefaultParameter.PingTimeDes,
                    DefaultParameter.PingTimeOutTickDes
            };
            }
        }

		public override string Name
		{
			get { return "LordControl"; }
		}

		public override string Guid
		{
			get { return PlugInGuid; }
		}

		public override string Author
		{
			get { return "xuxiaogang"; }
		}

        public LordControl()
        {
            _checkPingTickDo = DefaultParameter.LordCheckPingTickDo;
            _pingTimeOutTick = DefaultParameter.LordPingTimeOutTick;
        }

		public LordControl(int updateInterval, int timeoutSeconds)
		{
			if (updateInterval <= 0)
			{
				throw new ArgumentException("updateInterval");
			}

			if (timeoutSeconds <= 0)
			{
				throw new ArgumentException("timeoutSeconds");
			}

			_checkPingTickDo = updateInterval;
			_pingTimeOutTick = timeoutSeconds;
		}

		public override void Initialize(GameServer server)
		{
			base.Initialize(server);

			ResetModuleStateCode();
			_checkPingTickCurrent = 0;
			_lastPing = DateTime.MinValue;
			_gameRunningRefreshTime = DateTime.MinValue;
			_server.Connected += new ConnectedEventHandler(this.Connected);
			_server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
			_server.Disconnected += new DisconnectedEventHandler(this.Disconnected);
			_server.Tick += new TickEventHandler(this.Tick);
			_server.GameStateChanged += new GameStateChangeEventHandler(GameStateChanged);

            _GMCount = 1;
            _GMResultIndexByUserId = Hashtable.Synchronized(new Hashtable());
            _GMResultCache = Hashtable.Synchronized(new Hashtable());            

            //this.AppendGameMessage("我是呵呵","global","<I=1049600|0|346163|[中金创药]><N=70J啦。。要的MMMM>");
		}

		void GameStateChanged(GameServer server, GameServer.ServiceState originalState, GameServer.ServiceState newState)
		{
			if (newState == GameServer.ServiceState.Running)
			{
                GetGameRunningTime(SecurityManager.SystemAutomationId);
			}
			else
			{	
				_gameRunningRefreshTime = DateTime.MinValue;
			}

			if (newState == GameServer.ServiceState.OKStopped
				|| newState == GameServer.ServiceState.ErrorStopped)
			{
				ResetModuleStateCode();
			}
		}

		public override void Dispose()
		{
			_server.Connected -= new ConnectedEventHandler(this.Connected);
			_server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
			_server.Disconnected -= new DisconnectedEventHandler(this.Disconnected);
			_server.Tick -= new TickEventHandler(this.Tick);
			_server.GameStateChanged -= new GameStateChangeEventHandler(GameStateChanged);
		}

		private void Connected(GameServer server)
		{
            lock (thisLock)
			{
				_checkPingTickCurrent = 0;
				_lastPing = DateTime.MinValue;
				_gameRunningRefreshTime = DateTime.MinValue;
				ResetModuleStateCode();
				
				DoGetPalyerCount();
			}
		}

		private void Disconnected(GameServer server)
		{
            lock (thisLock)
			{
				_checkPingTickCurrent = 0;
				_lastPing = DateTime.MinValue;
				ResetModuleStateCode();

				server.GameServiceState = GameServer.ServiceState.OKStopped;
			}
		}

		private void MessageReceived(GameServer server, IProtocol message)
        {
            ISubProtocol pSubProtocol = message.GetSubProtocol();
            if (pSubProtocol == null ||
                (ProtocolDef)message.ProtocolId != ProtocolDef.l2e_header_def)
            {
                return;
            }
            byte[] data = message.ToBytes();
            switch ((ProtocolDef)pSubProtocol.SubProtocolID)
            {
                // ahpho
                case ProtocolDef.l2e_update_custom_info_def:
                    {
                        l2e_update_custom_info protocal = new l2e_update_custom_info();
                        protocal.FromBytes(data);
                        _customInfoStrs = protocal.GetInfoStrs();
                    }
                    break;
                case ProtocolDef.l2e_PlayerCount_def:
                    {
                        l2e_PlayerCount protocol = new l2e_PlayerCount();
                        protocol.FromBytes(data);
                        if (protocol.GetTotalCount() >= 0)
                        {
                            UpdatePlayerCount(protocol.GetTotalCount(),server.Group);                            
                        }

                        if (protocol.GetOffliveCount() >= 0)
                            UpdateOfflineCount(protocol.GetOffliveCount());
                    }
                    break;
                
                //GM指令接收的消息
                case ProtocolDef.l2e_ExeGMCmd_def:
                    {
                        l2e_ExeGMCmd protocol = new l2e_ExeGMCmd();
                        protocol.FromBytes(data);
                        //l2e_ExeGMCmd protocol = (l2e_ExeGMCmd)message;
                        if (protocol.ReturnCode == 1)
                        {
                            UpdateGMResultCache(protocol.nSessionID, protocol.szResult);
                        }
                        else
                        {
                            UpdateGMResultCache(protocol.nSessionID, "GM指令操作失败");
                        }                          
                    }
                    break;
                case ProtocolDef.l2e_GetBasicInfo_def:
                    {
                        l2e_GetBasicInfo protocol = new l2e_GetBasicInfo();
                        protocol.FromBytes(data);
                    }
                    break;
                case ProtocolDef.l2e_Who_def:
                    {
                        l2e_Who protocol = new l2e_Who();
                        protocol.FromBytes(data);
                        UpdatePlayerWho(protocol.PlayerList);
                    }
                    break;
                case ProtocolDef.l2e_GetGlobalVariable_def:
                    {
                        l2e_GetGlobalVariable protocol = new l2e_GetGlobalVariable();
                        protocol.FromBytes(data);
                        server.GameSetting.UpdateGlobalVariable(protocol.VariableIndex, protocol.VariableValue);
                    }
                    break;
                case ProtocolDef.l2e_GetGameStartTime_def:
                    {
                        l2e_GetGameStartTime protocol = new l2e_GetGameStartTime();
                        protocol.FromBytes(data);
                        int seconds = int.Parse(protocol.GameStartTime);
                        _gameRunningRefreshTime = DateTime.Now;
                        _gameRunningTime = new TimeSpan(0, 0, seconds);
                    }
                    break;
                case ProtocolDef.l2e_ReportError_def:
                    {
                        l2e_ReportError protocol = new l2e_ReportError();
                        protocol.FromBytes(data);
                        SetModuleStateCode(protocol.Module, protocol.ErrorCode);
                    }
                    break;
                case ProtocolDef.l2e_info_def:
                    {
                        l2e_info protocol = new l2e_info();
                        protocol.FromBytes(data);
                        Hashtable infoPackage = Util.ConvertKeyValuePairToHashtable(protocol.Info);
                        if (infoPackage != null)
                            GameInfoReceived(infoPackage);
                    }
                    break;
                case ProtocolDef.l2e_info_large_def:
                    {
                        l2e_info_large protocol = new l2e_info_large();
                        protocol.FromBytes(data);
                        Hashtable infoPackage = Util.ConvertKeyValuePairToHashtable(protocol.InfoLarge);
                        if (infoPackage != null)
                            GameInfoReceived(infoPackage);
                    }
                    break;
            }
		}

        //将消息加入到hashtable中
        void UpdateGMResultCache(uint sessionId, string message)
        {
            lock (_GMResultCache.SyncRoot)
            {
                if (_GMResultCache.Contains(sessionId) == false)
                {
                    _GMResultCache.Add(sessionId, message);
                }
            }
        }

        string GetGMResult(params object[] args)
        {
            if (args == null || args.Length != 1 || args[0].GetType() != typeof(uint))
                throw new ArgumentException();

            uint sessionId = (uint)args[0];

            lock (_GMResultCache.SyncRoot)
            {
                if (_GMResultCache.Contains(sessionId))
                {
                    string result = _GMResultCache[sessionId] as string;
                    _GMResultCache.Remove(sessionId);
                    return result;
                }
            }

            return null;
        }

        string GetUserGMResult(params object[] args)
        {
            if (args == null || args.Length != 1 || args[0].GetType() != typeof(int))
                throw new ArgumentException();
            
            int userId = (int)args[0];

            lock (_GMResultIndexByUserId.SyncRoot)
            {
                if (_GMResultIndexByUserId.Contains(userId))
                {
                    uint key = (uint)_GMResultIndexByUserId[userId];

                    string gmResult = GetGMResult(key);
                    _GMResultIndexByUserId.Remove(userId);
                    
                    return gmResult;                    
                }
            }

            return null;
        }

        private void Tick(GameServer server)
		{
            lock (server)
            {
                lock (thisLock)
                {
                    if (server.IsConnected)
                    {
                        _checkPingTickCurrent++;

                        if (_checkPingTickCurrent >= _checkPingTickDo)
                        {
                            _checkPingTickCurrent = 0;

                            if (_lastPing == DateTime.MinValue)
                            {
                                _lastPing = DateTime.Now;
                            }

                            DoGetPalyerCount();
                        }
                        //公聊监控
                        if (_chatMonitorSwitch)
                        {
                            //超时则停止监控
                            TimeSpan span = DateTime.Now.Subtract(_chatMonitorTimeStamp);
                            if (span.TotalSeconds >= ChatMonitorTimeOutSeconds)
                            {
                                MonitorGlobalChat(1, false);
                                //失败也设置标记位，否则遇到一直发送不成功的情况则会一直发送。
                                _chatMonitorSwitch = false;
                                _gameMessageQueue.Clear();
                            }
                        }
                    }
                    else
                    {
                        if (server.GameServiceState == GameServer.ServiceState.Running || server.GameServiceState == GameServer.ServiceState.ErrorStopped)
                             server.GameServiceState = GameServer.ServiceState.ErrorStopped;
                        else
                            server.GameServiceState = GameServer.ServiceState.OKStopped;
                        
                        server.UpdateOrDownState = GameServer.UpdateState.Nothing;
                        _checkPingTickCurrent = 0;
                        _lastPing = DateTime.MinValue;
                    }
                }
            }
		}

		private void DoGetPalyerCount()
		{
            KSubProtocolHead protocol = new KSubProtocolHead();
            protocol.ProtocolId = (ushort)ProtocolDef.e2l_header_def;
            protocol.SubProtocolID = (ushort)ProtocolDef.e2l_PlayerCount_def;
			_server.SendMessage(protocol);
		}

        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
		{
             //记录了日志 by Yy
             switch (actionKey)
             {
                 case ActionKeyGetGameRunningTime:
                     return GetGameRunningTime(userId);                        
                 case ActionKeyExecuteGMCommand:
                     return ExecuteGMCommand(userId,args[0] as string, (bool)args[1], args[2] as string, (bool)args[3], args[4] as ArrayList, ref outArg);                        
                 case ActionKeySayToWorld:
                     return SayToWorld(userId,args[0] as string);                                                
                 case ActionKeyRefreshPlayerWho:
                     return RefreshPlayerWho(userId,Convert.ToInt16(args[0]));
				 case ActionKeyMonitorGlobalChat:                     
					 return MonitorGlobalChat(userId, (bool)args[0]);
				 case ActionKeySetModule:
					 SetModuleStateCode((int)args[0], (int)args[1]);
					 return true;

             }
			throw new Exception(string.Format("ActionKey {0} does not exist.", actionKey));
		}

        public override object GetData(int userId, string dataKey, params object[] args)
		{
			switch (dataKey)
			{
				case DataKeyGameRunningTime:
					{
						if (_gameRunningRefreshTime != DateTime.MinValue)
							return (DateTime.Now - _gameRunningRefreshTime + _gameRunningTime);
						else
							return TimeSpan.MinValue;
					}
				case DataKeyPlayerCountQueue:
					return _playerCountQueue;
                // ahpho
                case DataKeyCustomInfo:
                    return _customInfoStrs;
				case DataKeyCurrentPlayerCount:
					{
                        if (_playerCountQueue.Count == 0 )
							return 0;
						else
							return _playerCountQueue.ToArray()[_playerCountQueue.Count - 1].Count;
					}                
                case DataKeyCurrentPlayerOfflineCount:
					{
                        if (_playerOfflineCountQueue.Count == 0)
							return 0;
						else
                            return _playerOfflineCountQueue.ToArray()[_playerOfflineCountQueue.Count - 1].Count;
					}
				case DataKeyPlayerCountStatistic:
					return GetPlayerCountInfo((DateTime)args[0], (DateTime)args[1]);
				case DataKeyPlayerWho:
					return _playerWhoList;
				case DataKeyModuleErrorCode:
					{
						int moduleIndex = (int)args[0];
						if (moduleIndex >= 0 && moduleIndex < MaxModuleCount)
						{
							return _moduleErrorCodes[moduleIndex];
						}
					}
					break;
				case DataKeyGameMessageQueue:                    
                    if (!_chatMonitorSwitch)
                        MonitorGlobalChat(userId, true);
                    _chatMonitorTimeStamp = DateTime.Now;
                    return _gameMessageQueue;
                
                case DataKeyGMResult:
                    return GetGMResult(args);
                case DataKeyUserGMResult:
                    return GetUserGMResult(args);
			}

			throw new Exception(string.Format("DataKey {0} does not exist.", dataKey));
		}

		bool GetGameRunningTime(int userId)
		{
            bool result = false;
            User user = AdminServer.TheInstance.SecurityManager.GetUser(userId);
			if (_server.IsConnected)
            {
                KSubProtocolHead protocol = new KSubProtocolHead();
                protocol.SubProtocolID = (ushort)ProtocolDef.e2l_GetGameStartTime_def;
                result = _server.SendMessage(protocol);
			}
            Security.LogSystem.TheInstance.WriteLog(
                userId,
                OpType.READ,
                _server.Id,
                string.Format(StringDef.LogMessageGetGameRunningTime,
                user == null ? userId.ToString() : user.UserName,
                _server.Name,
                result ? StringDef.Success : StringDef.Fail)
            );

			return result;
		}

		static Random s_random = new Random();

		void UpdatePlayerCount(int playerCount,ServerGroup thisGroup)
		{
			if (_playerCountQueue.Count >= PlayerCountQueueLength)
				_playerCountQueue.Dequeue();

			if (_server.GameServiceState != GameServer.ServiceState.Stopping)
			{				
				_server.GameServiceState = GameServer.ServiceState.Running;
                if (_server.PID <= 0)
                {
                    string[] para = new string[1] { _server.Id.ToString() };
                    _server.DoPlugInAction(SecurityManager.SystemAutomationId, E2gServerPID.PlugInGuid, E2gServerPID.actionKey, para);
                }
			}

			//TODO 调试用
			//playerCount = s_random.Next(2000);

			DateTime now = DateTime.Now;

			PlayerCountInfo info = new PlayerCountInfo();
			info.Count = playerCount;
			info.Time = now;
			_playerCountQueue.Enqueue(info);
								
			if (_lastPlayerCountStatisticHour != now.Hour)
			{
				DoPlayerCountStatistic(now);
			}

			_startingForgive = true;
			_lastPing = now;

            //记录历史最高人数
            if (playerCount > thisGroup.MaxPlayer)
            {
                thisGroup.MaxPlayer = playerCount;
                thisGroup.SetGroupMaxPlays(playerCount);
            }
		}

        void UpdateOfflineCount(int playerOfflineCount)
        {
            if (_playerOfflineCountQueue.Count >= PlayerCountQueueLength)
                _playerOfflineCountQueue.Dequeue();

            PlayerCountInfo info = new PlayerCountInfo();
            info.Count = playerOfflineCount;
            info.Time = DateTime.Now;
            _playerOfflineCountQueue.Enqueue(info);
        }

		void DoPlayerCountStatistic(DateTime now)
		{
			PlayerCountStatisticInfo statisticInfo = new PlayerCountStatisticInfo();
			if (_playerCountQueue.Count > 0)
			{
				foreach (PlayerCountInfo info in _playerCountQueue)
				{
					if (info.Count > statisticInfo.MaxCount)
						statisticInfo.MaxCount = info.Count;
					if (info.Count < statisticInfo.MinCount)
						statisticInfo.MinCount = info.Count;
					statisticInfo.AverageCount += info.Count;
				}
				statisticInfo.AverageCount /= _playerCountQueue.Count;
			}
			statisticInfo.Time = now;
			AddPlayerCountInfo(statisticInfo);
			_lastPlayerCountStatisticHour = now.Hour;
		}

		/// <summary>
		/// 添加玩家数量信息
		/// </summary>
		void AddPlayerCountInfo(PlayerCountStatisticInfo info)
		{
			if (info == null)
				throw new ArgumentNullException("info");

			using (IBlazeDatabase db = DbFactory.GetDatabase())
			{
				IBlazeTable table = db.GetTable(TableString.PlayerCountTableName);
				string[] fieldNames = {
					TableString.PlayerCountFieldServerId,
					TableString.PlayerCountFieldRecordTime,
					TableString.PlayerCountFieldMaxPlayerCount,
					TableString.PlayerCountFieldMinPlayerCount,
					TableString.PlayerCountFieldAveragePlayerCount
				};
				object[] fieldValues = {
					_server.Id,
					info.Time,
					info.MaxCount,
					info.MinCount,
					info.AverageCount
				};
				table.Add(fieldNames, fieldValues);
			}
		}

		/// <summary>
		/// 得到玩家数量信息
		/// </summary>
		PlayerCountStatisticInfo[] GetPlayerCountInfo(DateTime startTime, DateTime endTime)
		{
			using (IBlazeDatabase db = DbFactory.GetDatabase())
			{
				IBlazeTable table = db.GetTable(TableString.PlayerCountTableName);
				DataSet ds = new DataSet();
				table.Get(ds,
					FilterFactory.CreateAndFilter(
						FilterFactory.CreateEqualFilter(TableString.PlayerCountFieldServerId, _server.Id),
						FilterFactory.CreateAndFilter(
							FilterFactory.CreateLargerEqualFilter(TableString.PlayerCountFieldRecordTime, startTime),
							FilterFactory.CreateLesserEqualFilter(TableString.PlayerCountFieldRecordTime, endTime)
							)
						)
					);

				DataTable dt = ds.Tables[TableString.PlayerCountTableName];
				int rowCount = dt.Rows.Count;
				PlayerCountStatisticInfo[] infos = new PlayerCountStatisticInfo[rowCount];
				for (int i = 0; i < rowCount; i++)
				{
					DataRow row = dt.Rows[i];
					PlayerCountStatisticInfo info = new PlayerCountStatisticInfo();
					info.Time = (DateTime)row[TableString.PlayerCountFieldRecordTime];
					info.MaxCount = (int)row[TableString.PlayerCountFieldMaxPlayerCount];
					info.MinCount = (int)row[TableString.PlayerCountFieldMinPlayerCount];
					info.AverageCount = (int)row[TableString.PlayerCountFieldAveragePlayerCount];
					infos[i] = info;
				}

				return infos;
			}
		}

        /// <summary>
        /// 获取某日的最大人数
        /// </summary>
        /// <param name="day"></param>
        /// <returns></returns>
        int GetDayMaxPlayer(DateTime day)
        {
            DateTime startTime = DateTime.Parse(day.ToShortDateString() + " 00:00:00");
            DateTime endTime = DateTime.Parse(day.ToShortDateString() + " 23:59:59");
            int maxPlayer = 0;
            int rowMax = 0;
            using (IBlazeDatabase db = DbFactory.GetDatabase())
            {
                IBlazeTable table = db.GetTable(TableString.PlayerCountTableName);
                DataSet ds = new DataSet();
                table.Get(ds,
                    FilterFactory.CreateAndFilter(
                        FilterFactory.CreateEqualFilter(TableString.PlayerCountFieldServerId, _server.Id),
                        FilterFactory.CreateAndFilter(
                            FilterFactory.CreateLargerEqualFilter(TableString.PlayerCountFieldRecordTime, startTime),
                            FilterFactory.CreateLesserEqualFilter(TableString.PlayerCountFieldRecordTime, endTime)
                            )
                        )
                    );

                DataTable dt = ds.Tables[TableString.PlayerCountTableName];
  
                for (int i = 0; i < dt.Rows.Count; i++)
                {
                    DataRow row = dt.Rows[i];
                    PlayerCountStatisticInfo info = new PlayerCountStatisticInfo();

                    rowMax = (int)row[TableString.PlayerCountFieldMaxPlayerCount];
                    if (maxPlayer < rowMax) maxPlayer = rowMax;
                }

                return maxPlayer ;
            }
        }

		/// <summary>
		/// 执行GM指令
		/// </summary>
		/// <param name="playerName">执行GM指令的玩家名字,不能为null</param>
        /// <param name="isAccountName">玩家名字是否为账号名，若值为true则是账号名，若值为false则是角色名</param>
		/// <param name="command">GM指令内容,不能为null</param>
        /// <param name="isExecutedByGameServer">该GM指令是否直接发给GS执行,若值为true则发给GS,若值为false则不发给GS</param>
        /// <param name="gameServerArrayList">如果isExecutedByGameServer为true,则这里就是要发给的GS的编号列表,可以为null</param>
		bool ExecuteGMCommand(int userId,string playerName, bool isAccountName, string command, bool isExecutedByGameServer, ArrayList gameServerArrayList, ref object outArg)
        {
            if (playerName == null)
                throw new ArgumentNullException("playerName");

            if (command == null)
                throw new ArgumentNullException("command");

            bool result = false;
            User user = AdminServer.TheInstance.SecurityManager.GetUser(userId);

            if (_server.IsConnected && _server.GameServiceState == GameServer.ServiceState.Running)
            {
                e2l_ExeGMCmd protocol = new e2l_ExeGMCmd();
                //if (playerName != String.Empty)
                //{
                //    protocol.PlayerName = playerName;
                //    protocol.GCorGS = 1;
                //    protocol.RoleOrAccount = 0;
                //}
                //else
                //{
                //    protocol.PlayerName = "";
                //    protocol.GCorGS = 0;
                //    protocol.RoleOrAccount = 0;
                //}

                protocol.PlayerName = playerName;
                protocol.RoleOrAccount = Convert.ToUInt16(isAccountName);
                protocol.Command = command;
                protocol.GCorGS = Convert.ToUInt16(isExecutedByGameServer);

                if ((gameServerArrayList != null) && (gameServerArrayList.Count > 0))
                    protocol.GSList = gameServerArrayList;
               
                //生成sessionId，并传给引用参数
                uint id = 0;
                lock (_syncRoot)
                {
                    id = _GMCount;
                    _GMCount++;
                }
                protocol.nSessionID = id;
                protocol.wServer = (ushort)_server.Id;
                outArg = id;

                result = _server.SendMessage(protocol);

                //更新_GMResultIndexByUserId
                if (result)
                {
                    lock (_GMResultIndexByUserId.SyncRoot)
                    {
                        if (_GMResultIndexByUserId.Contains(userId) == false)
                        {
                            _GMResultIndexByUserId.Add(userId, id);
                        }
                        else
                        {
                            int oldId = (int)_GMResultIndexByUserId[userId];

                            if (_GMResultCache.Contains(oldId))
                                _GMResultCache.Remove(oldId);

                            _GMResultIndexByUserId[userId] = id;
                        }
                    }
                }
            }

            Security.LogSystem.TheInstance.WriteLog(userId,
                OpType.EXECUTE,
                _server.Id,
                string.Format(StringDef.LogMessageExecuteGMCommand,
                            user == null ? userId.ToString() : user.UserName,
                            _server.Name,
                            playerName == null || playerName.Length == 0 ? string.Empty : string.Format("[{0}]", playerName),
                            command,
                            result ? StringDef.Success : StringDef.Fail)
            );

            return result;
        }

		/// <summary>
		/// 世界频道说话
		/// </summary>
		/// <param name="message">说话内容</param>
		bool SayToWorld(int userId,string message)
		{
			if (message == null)
				throw new ArgumentNullException(message);
			if (SystemConfig.Current.DefaultEncoding.GetByteCount(message) >= 256)
				throw new ArgumentException("Message is too long. 255 byte is the limitation.");

            bool result = false;
            User user = AdminServer.TheInstance.SecurityManager.GetUser(userId);

			if (_server.IsConnected && _server.GameServiceState == GameServer.ServiceState.Running)
			{
				e2l_SayToWorld protocol = new e2l_SayToWorld();
				protocol.Message = message;
                result = _server.SendMessage(protocol);
			}

            Security.LogSystem.TheInstance.WriteLog(
                userId,
                OpType.EXECUTE,
                _server.Id,
                string.Format(StringDef.LogMessageSayToWorld,
                            user == null ? userId.ToString() : user.UserName,
                            _server.Name,
                            message,
                            result ? StringDef.Success : StringDef.Fail)
            );
			return result;
		}

		/// <summary>
		/// 查看有哪些玩家在线
		/// </summary>
		/// <param name="offset">偏移</param>
		bool RefreshPlayerWho(int userId,int offset)
		{
            bool result = false;
            User user = AdminServer.TheInstance.SecurityManager.GetUser(userId);
            if (_server.IsConnected && _server.GameServiceState == GameServer.ServiceState.Running)
            {
                e2l_Who protocol = new e2l_Who();
                protocol.Offset = (ushort)offset;
                result = _server.SendMessage(protocol);
            }

            Security.LogSystem.TheInstance.WriteLog(
                userId,
                OpType.EXECUTE,
                _server.Id,
                string.Format(StringDef.LogMessageRefreshPlayerWho,
                    user == null ? userId.ToString() : user.UserName,
                    _server.Name,
                    result ? StringDef.Success : StringDef.Fail)
            );

            return result;
		}

		void UpdatePlayerWho(l2e_Who_PlayerInfo[] playerList)
		{
			lock (_playerWhoList.SyncRoot)
			{
				_playerWhoList.Clear();
				foreach (l2e_Who_PlayerInfo info in playerList)
				{
					_playerWhoList.Add(info);
				}
			}
		}

		/// <summary>
		/// 重置模块状态码
		/// </summary>
		void ResetModuleStateCode()
		{
			for (int i = 0; i < MaxModuleCount; i++)
			{
				SetModuleStateCode(i, UnknownErrorCode);
			}
		}

		/// <summary>
		/// 设置模块状态码
		/// </summary>
		void SetModuleStateCode(int moduleIndex, int newStateCode)
		{
			if (moduleIndex >= 0 && moduleIndex < MaxModuleCount)
			{
				if (_moduleErrorCodes[moduleIndex] != newStateCode)
				{
					int originalStateCode = _moduleErrorCodes[moduleIndex];
					_moduleErrorCodes[moduleIndex] = newStateCode;

					if (ModuleStateChanged != null)
					{
						ModuleStateChanged(moduleIndex, originalStateCode, newStateCode);
					}

					if (newStateCode == -1 || newStateCode > 0)
					{
						AutomationContext context = new AutomationContext();						
						context.Server = _server;

						switch (moduleIndex)
						{
							case 0:
								{
									context.Message = string.Format(StringDef.GameModuleError, _server.ToString(), "Paysys", newStateCode);
									AdminServer.TheInstance.StrategyManager.OnEvent(FSEyeEvent.ModulePaysysError, context);									
								}
								break;
							case 1:
								{
									context.Message = string.Format(StringDef.GameModuleError, _server.ToString(), "Database", newStateCode);
									AdminServer.TheInstance.StrategyManager.OnEvent(FSEyeEvent.ModuleDatabaseError, context);
								}
								break;
						}
					}
				}
			}
		}

		void AppendGameMessage(string sender, string receiver, string message)
		{
			if (_gameMessageQueue.Count >= GameMessageQueueLength)
				_gameMessageQueue.Dequeue();

			GameMessage msg = new GameMessage();
			msg.Sender = sender;
			msg.Receiver = receiver;
			msg.Message = message;
			msg.Time = DateTime.Now;

			_gameMessageQueue.Enqueue(msg);
		}

		bool MonitorGlobalChat(int userId, bool on)
		{
			string cmd = string.Format("?gm ds MonitorGlobalChat({0})", on ? 1 : 0);            
            
            object noMeaning = null;
            bool result = ExecuteGMCommand(userId, String.Empty, false, cmd, false, null, ref noMeaning);

            //改变监控开关 如果停止监控则清除队列
            if (result)
            {                
                _chatMonitorSwitch = on;

                if (on)
                    _chatMonitorTimeStamp = DateTime.Now;
                //else
                //    _gameMessageQueue.Clear();
            }

            return result;
		}

		void GameInfoReceived(Hashtable infoPackage)
		{
			if (infoPackage == null)
				return;

			string content = infoPackage["content"] as string;
			switch(content)
			{
				case "ChatMessage":
					{
						string sender = infoPackage["sender"] as string;
						string receiver = infoPackage["receiver"] as string;
						string message = Encoding.Default.GetString(Convert.FromBase64String(infoPackage["message"] as string));
						AppendGameMessage(sender, receiver, message);

                        //设置公聊开关为开启状态
                        _chatMonitorSwitch = true;
					}
					break;
			}
		}
	}
}