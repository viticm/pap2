//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-8
//      File_base        : AdminServer
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 管理服务器
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Threading;
using FSEye.DAL;
using FSEye.Security;
using FSEye.Net;
using FSEye.PlugIn;
using FSEye.FS2Lib;
using FSEye.PaySysLib;

namespace FSEye
{
	public sealed class AdminServer : ISecurity, IDisposable
	{	
		static AdminServer g_theInstance = new AdminServer();

		const string SecurityObjectServiceSwitch = "System/ServiceSwitch";

        const string SecurityObjectAutomationManager = "/FSEye/System/AutomationManager";

		FSEyeObject _securityObject;

		SecurityManager _securityManager;

		ConnectionManager _connectionManager;

		MessageEngine _messageEngine;

		GameServerManager _gameServerManager;

		GameServerMonitor _gameServerMonitor;

		AutomationManager _automationManager;

		StrategyManager _strategyManager;

		ScheduledTaskManager _scheduledTaskManager;

        FTPClient _FTPClient;

		IPaySysAgent _paysysAgent;

		BatchTaskManager _batchTaskManager;

        PaysysManager _paysysManager;

        IBShopManager _ibShopManager;

		bool _isRunning;

		bool _disposed;

		public static AdminServer TheInstance
		{
			get { return g_theInstance; }
		}

		public FSEyeObject SecurityObject
		{
			get { return _securityObject; }
			set { _securityObject = value; }
		}
        
		private AdminServer()
		{
			_disposed = false;
			_isRunning = false;
			_securityManager = new SecurityManager();
			_gameServerManager = new GameServerManager();
			_connectionManager = new ConnectionManager();
			_messageEngine = new MessageEngine();
			_gameServerMonitor = new GameServerMonitor();			
			_automationManager = new AutomationManager();
			_strategyManager = new StrategyManager();
			_scheduledTaskManager = new ScheduledTaskManager();
            _FTPClient = new FTPClient();
			_batchTaskManager = new BatchTaskManager();
            _paysysManager = new PaysysManager();
            _ibShopManager = new IBShopManager();
		}

		public void Initialize()
		{			
			InitializeDatabase(false);
			Util.DebugLog("AdminServer Initialize: Database Done.");

			_securityManager.Startup();
			_securityObject = SecurityManager.Root;
			Util.DebugLog("AdminServer Initialize: SecurityManager Done.");

            InitAutomations();
			Util.DebugLog("AdminServer Initialize: Automation Done.");

			InitStrategy();
			Util.DebugLog("AdminServer Initialize: Strategy Done.");

			_paysysAgent = new OraclePaySysAgent(SystemConfig.Current.PaySysDbConnectionString);
			_paysysAgent.Error += new EventHandler(_paysysAgent_Error);
			Util.DebugLog("AdminServer Initialize: Paysys Done.");

			LogSystem.TheInstance.LogDeviceList.Add(new MySqlLogDevice(SystemConfig.Current.DbConnectionString));
			Util.DebugLog("AdminServer Initialize: LogSystem Done.");

			try
			{
				FS2GameDataManager.TheInstance.Init(SystemConfig.Current.GameSettingDir, SystemConfig.Current.DefaultEncoding);
				if (SystemConfig.Current.PreloadGameData)
				{
					FS2GameDataManager.TheInstance.Load();
				}
				
				Util.DebugLog("AdminServer Initialize: FS2GameDataManager Done.");
			}
			catch (Exception ex)
			{
				Util.DebugLog(ex.ToString());
			}
		}

        void _paysysAgent_Error(object sender, EventArgs e)
        {
            FSEye.PaySysLib.ErrorEventArgs error = e as FSEye.PaySysLib.ErrorEventArgs;
            Util.DebugLog(error.ErrorMessage);
        }

		~AdminServer()
		{
			Dispose();
		}

		public void Dispose()
		{
			lock (this)
			{
				if (!_disposed)
				{
					GC.SuppressFinalize(this);

					_securityManager.Dispose();
					_gameServerManager.Dispose();
					_messageEngine.Dispose();
					_connectionManager.Dispose();
					_automationManager.Dispose();
					_strategyManager.Dispose();
					_scheduledTaskManager.Dispose();                    
                    _FTPClient.Dispose();
					_paysysAgent.Dispose();
					_batchTaskManager.Dispose();
                    _paysysManager.Dispose();

					_disposed = true;
				}
			}
		}

		/// <summary>
		/// 是否在运行
		/// </summary>
		public bool IsRunning
		{
			get { return _isRunning; }
		}

		#region 系统主要模块对象
		/// <summary>
		/// 安全管理器
		/// </summary>
		public SecurityManager SecurityManager
		{
			get { return _securityManager; }
		}

		/// <summary>
		/// 游戏服务器管理器
		/// </summary>
		public GameServerManager GameServerManager
		{
			get { return _gameServerManager; }
		}
		
		/// <summary>
		/// 服务器管理器
		/// </summary>
		public ConnectionManager ConnectionManager
		{
			get { return _connectionManager; }
		}

		/// <summary>
		/// 游戏服务器监视器
		/// </summary>
		public GameServerMonitor GameServerMonitor
		{
			get { return _gameServerMonitor; }
		}

		/// <summary>
		/// 自动化管理器
		/// </summary>
		public AutomationManager AutomationManager
		{
			get { return _automationManager; }
		}

		/// <summary>
		/// 策略管理器
		/// </summary>
		public StrategyManager StrategyManager
		{
			get { return _strategyManager; }
		}

		/// <summary>
		/// 计划任务管理器
		/// </summary>
		public ScheduledTaskManager ScheduledTaskManager
		{
			get { return _scheduledTaskManager; }
		}

        public FTPClient FTPClient
        {
            get { return _FTPClient; }
        }

		/// <summary>
		/// PaySys代理
		/// </summary>
		public IPaySysAgent PaySysAgent
		{
			get { return _paysysAgent; }
		}

		/// <summary>
		/// 批处理任务管理器
		/// </summary>
		public BatchTaskManager BatchTaskManager
		{
			get { return _batchTaskManager; }
		}

        public PaysysManager PaysysManager
        {
            get { return _paysysManager; }
        }

        public IBShopManager IBShopManager
        {
            get { return _ibShopManager; }
        }

		#endregion

		#region 系统功能函数
		/// <summary>
		/// 开启服务
		/// </summary>
		public void StartService(User user)
		{
            try
            {
                lock (this)
                {
                    if (!_isRunning)
                    {
                        _gameServerManager.Startup();
                        _messageEngine.Startup();
                        _connectionManager.ClientConnect += new ClientConnectEventHandler(_gameServerManager.GameServerConnected);
                        _connectionManager.ClientDisconnect += new ClientDisconnectEventHandler(_gameServerManager.GameServerDisconnected);
                        try
                        {
                            _connectionManager.Startup(SystemConfig.Current.AdminServerListenPort, SystemConfig.Current.MaxConnectionCount);
                        }
                        catch (Exception ex)
                        {
                            throw (ex);
                        }
                        _FTPClient.Startup();

                        _gameServerMonitor.Start();

                        //从Initialize中移动到这里
                        _strategyManager.Load();
                        _scheduledTaskManager.Load();
                        _batchTaskManager.Load();

                        _automationManager.Working = true;
                        _strategyManager.On = true;
                        _scheduledTaskManager.On = true;
                        //插件信息初始化
                        ServerPlugInFactory.Initialize();

						if (SystemConfig.Current.EnablePaysys)
						{
							_paysysAgent.Start();
						}

						if (user != null)
						{
							FSEyeObject serviceSwitch = SecurityManager.Get(SecurityObject.FullPath + SecurityManager.ObjectPathDelimiter + SecurityObjectServiceSwitch);
							LogSystem.TheInstance.WriteLog(user.SecurityObject.Id, OpType.WRITE, serviceSwitch.Id, string.Format(StringDef.LogMessageStartService, user.UserName));
						}
                        _isRunning = true;
                    }
                }
            }
            catch (Exception e)
            {
                Util.DebugLog(e.Message);
                Util.DebugLog(e.StackTrace);
            }
		}

		/// <summary>
		/// 关闭服务
		/// </summary>
		public void StopService(User user)
		{
			lock (this)
			{
				if (_isRunning)
				{
					_strategyManager.On = false;
					_gameServerMonitor.Stop();
					_automationManager.Working = false;
					_connectionManager.Shutdown();
					_connectionManager.ClientConnect -= new ClientConnectEventHandler(_gameServerManager.GameServerConnected);
					_connectionManager.ClientDisconnect -= new ClientDisconnectEventHandler(_gameServerManager.GameServerDisconnected);					
					_messageEngine.Shutdown();
					_gameServerManager.Shutdown();
                    _FTPClient.Shutdown();
					_paysysAgent.Stop();

					FSEyeObject serviceSwitch = SecurityManager.Get(SecurityObject.FullPath + SecurityManager.ObjectPathDelimiter + SecurityObjectServiceSwitch);
					LogSystem.TheInstance.WriteLog(user.SecurityObject.Id, OpType.WRITE, serviceSwitch.Id, string.Format(StringDef.LogMessageStopService, user.UserName));
					_isRunning = false;
				}
			}
		}

		/// <summary>
		/// 初始化数据库
		/// </summary>
		private void InitializeDatabase(bool rebuild)
		{
			DbFactory.DbConnectionString = SystemConfig.Current.DbConnectionString;
			DbFactory.DefaultDbName = SystemConfig.Current.DefaultDbName;

			ColumnConfiguration[] colCfgsObj = new ColumnConfiguration[] {
			    new ColumnConfiguration( TableString.ObjectFieldId, BlazeDbType.INT4, true ),
			    new ColumnConfiguration( TableString.ObjectFieldOwner, BlazeDbType.INT4 ),
				new ColumnConfiguration( TableString.ObjectFieldParentId, BlazeDbType.INT4 ),
			    new ColumnConfiguration( TableString.ObjectFieldName, BlazeDbType.STRING),
			    new ColumnConfiguration( TableString.ObjectFieldComment, BlazeDbType.STRING)
			};
			ColumnConfiguration[] colCfgsUser = new ColumnConfiguration[] {
			    new ColumnConfiguration( TableString.UserFieldId, BlazeDbType.INT4, true ),
			    new ColumnConfiguration( TableString.UserFieldUserName, BlazeDbType.SHORT_STRING, false, true ),
			    new ColumnConfiguration( TableString.UserFieldRealName, BlazeDbType.SHORT_STRING),
			    new ColumnConfiguration( TableString.UserFieldPassword, BlazeDbType.STRING)
			};
			ColumnConfiguration[] colCfgsGroup = new ColumnConfiguration[] {
			    new ColumnConfiguration( TableString.UserGroupFieldId, BlazeDbType.INT4, true ),
			    new ColumnConfiguration( TableString.UserGroupFieldGroupName, BlazeDbType.SHORT_STRING, false, true )
			};
			ColumnConfiguration[] colCfgsUserGroup = new ColumnConfiguration[] {
			    new ColumnConfiguration( TableString.UserInGroupFieldUserId, BlazeDbType.INT4, true),
			    new ColumnConfiguration( TableString.UserInGroupFieldGroupId, BlazeDbType.INT4, true ),
			    new ColumnConfiguration( TableString.UserInGroupFieldEstablishDatetime, BlazeDbType.DATETIME )
			};
			ColumnConfiguration[] colCfgsAce = new ColumnConfiguration[] {
			    new ColumnConfiguration( TableString.AceFieldAceId, BlazeDbType.INT4, false,true,true,true,null ),
			    new ColumnConfiguration( TableString.AceFieldObjectId, BlazeDbType.INT4 ),
			    new ColumnConfiguration( TableString.AceFieldOperatorId, BlazeDbType.INT4 ),
				new ColumnConfiguration( TableString.AceFieldOperation, BlazeDbType.INT4),
			};
			ColumnConfiguration[] colCfgsGameServer = new ColumnConfiguration[] {
			    new ColumnConfiguration( TableString.GameServerFieldId, BlazeDbType.INT4, true ),
			    new ColumnConfiguration( TableString.GameServerFieldNetAddress, BlazeDbType.SHORT_STRING),				
				new ColumnConfiguration( TableString.GameServerFieldGameDir, BlazeDbType.LONG_STRING),
				new ColumnConfiguration( TableString.GameServerFieldLordPort, BlazeDbType.INT4),
				new ColumnConfiguration( TableString.GameServerFieldMaxPlayer, BlazeDbType.INT4),
				new ColumnConfiguration( TableString.GameServerFieldIntranetMask, BlazeDbType.SHORT_STRING),
				new ColumnConfiguration( TableString.GameServerFieldInternetMask, BlazeDbType.SHORT_STRING),
				new ColumnConfiguration( TableString.GameServerFieldPaysysIp, BlazeDbType.SHORT_STRING),
				new ColumnConfiguration( TableString.GameServerFieldPaysysPort, BlazeDbType.INT4),
				new ColumnConfiguration( TableString.GameServerFieldPaysysLoginName, BlazeDbType.SHORT_STRING),
				new ColumnConfiguration( TableString.GameServerFieldPaysysLoginPwd, BlazeDbType.LONG_STRING),				
				new ColumnConfiguration( TableString.GameServerFieldDbHost, BlazeDbType.SHORT_STRING),
				new ColumnConfiguration( TableString.GameServerFieldDbName, BlazeDbType.SHORT_STRING),
				new ColumnConfiguration( TableString.GameServerFieldDbLoginName, BlazeDbType.SHORT_STRING),
				new ColumnConfiguration( TableString.GameServerFieldDbLoginPwd, BlazeDbType.LONG_STRING),
				new ColumnConfiguration( TableString.GameServerFieldDbBackupDir, BlazeDbType.LONG_STRING),
				new ColumnConfiguration( TableString.GameServerFieldFSEyeIp, BlazeDbType.SHORT_STRING),
				new ColumnConfiguration( TableString.GameServerFieldFSEyePort, BlazeDbType.INT4),
				new ColumnConfiguration( TableString.GameServerFieldHasBeenConfigGuard, BlazeDbType.INT4),
				new ColumnConfiguration( TableString.GameServerFieldServerType, BlazeDbType.INT4),
                new ColumnConfiguration( TableString.GameServerFieldCfgString, BlazeDbType.LONG_STRING),
                new ColumnConfiguration( TableString.GameServerFieldFtpDownloadServer, BlazeDbType.INT4),
                new ColumnConfiguration( TableString.GameServerFieldGmcIp, BlazeDbType.STRING),
                new ColumnConfiguration( TableString.GameServerFieldGmcAccount, BlazeDbType.STRING)
			};
			ColumnConfiguration[] colCfgsServerGroup = new ColumnConfiguration[] {
			    new ColumnConfiguration( TableString.ServerGroupFieldId, BlazeDbType.INT4, true ),
			    new ColumnConfiguration( TableString.ServerGroupFieldType, BlazeDbType.INT4),
                new ColumnConfiguration( TableString.ServerGroupFieldGroupEnable, BlazeDbType.INT2),
                new ColumnConfiguration( TableString.ServerGroupFieldMaxPlayers, BlazeDbType.INT4),
                new ColumnConfiguration( TableString.ServerGroupFieldMaxPlayersLogTime, BlazeDbType.DATETIME),
                new ColumnConfiguration( TableString.ServerGroupFieldCabinet,BlazeDbType.LONG_STRING),
                new ColumnConfiguration( TableString.ServerGroupFieldGroupVersion, BlazeDbType.STRING )
			};
            ColumnConfiguration[] colCfgsServerRegion = new ColumnConfiguration[] {
			    new ColumnConfiguration( TableString.ServerRegionFieldId , BlazeDbType.INT4, true ),			    
                new ColumnConfiguration( TableString.ServerRegionFieldMaxPlayers, BlazeDbType.INT4),
                new ColumnConfiguration( TableString.ServerRegionFieldMaxPlayersLogTime, BlazeDbType.DATETIME),
                new ColumnConfiguration( TableString.ServerRegionFieldRegionName , BlazeDbType.LONG_STRING),
                new ColumnConfiguration( TableString.ServerRegionFieldRegionType , BlazeDbType.INT2)
			};
			ColumnConfiguration[] colCfgsGameLog = new ColumnConfiguration[] {
				new ColumnConfiguration( TableString.GameLogFieldId, BlazeDbType.INT4, true ),
				new ColumnConfiguration( TableString.GameLogFieldGameServerId, BlazeDbType.INT4 ),
				new ColumnConfiguration( TableString.GameLogFieldDate, BlazeDbType.DATE ),
				new ColumnConfiguration( TableString.GameLogFieldTotalMoney, BlazeDbType.INT8 ),
				new ColumnConfiguration( TableString.GameLogFieldTotalMoneyInBox, BlazeDbType.INT8 )
			};
            ColumnConfiguration[] colCfgsServerPlugIn = new ColumnConfiguration[] {
				new ColumnConfiguration( TableString.ServerPlugInFieldId, BlazeDbType.INT4, true ),
				new ColumnConfiguration( TableString.ServerPlugInFieldGameServerId, BlazeDbType.INT4 ),
				new ColumnConfiguration( TableString.ServerPlugInFieldPlugInGuid, BlazeDbType.STRING ),
				new ColumnConfiguration( TableString.ServerPlugInFieldParameter1,BlazeDbType.INT2),
				new ColumnConfiguration( TableString.ServerPlugInFieldParameter2, BlazeDbType.INT2 ),
                new ColumnConfiguration( TableString.ServerPlugInFieldParameter3, BlazeDbType.STRING ),
                new ColumnConfiguration( TableString.ServerPlugInFieldPlugInName, BlazeDbType.STRING )
			};
			ColumnConfiguration[] strategy = new ColumnConfiguration[] {
				new ColumnConfiguration( TableString.StrategyFieldId, BlazeDbType.INT4, true ),
				new ColumnConfiguration( TableString.StrategyFieldEvent, BlazeDbType.INT4),
				new ColumnConfiguration( TableString.StrategyFieldAutomation, BlazeDbType.BLOB),				
				new ColumnConfiguration( TableString.StrategyFieldEnabled, BlazeDbType.INT2)
			};
			ColumnConfiguration[] scheduledTask = new ColumnConfiguration[] {
				new ColumnConfiguration( TableString.ScheduledTaskFieldId, BlazeDbType.INT4, true ),
				new ColumnConfiguration( TableString.ScheduledTaskFieldTaskData, BlazeDbType.BLOB)
			};
			//ColumnConfiguration[] task = new ColumnConfiguration[] {
			//    new ColumnConfiguration( TableString.TaskFieldId, BlazeDbType.INT4, true ),
			//    new ColumnConfiguration( TableString.TaskFieldName, BlazeDbType.STRING),
			//    new ColumnConfiguration( TableString.TaskFieldAutomation, BlazeDbType.BLOB),	
			//    new ColumnConfiguration( TableString.TaskFieldInterval,BlazeDbType.INT4),
			//    new ColumnConfiguration( TableString.TaskFieldPeriodUnit, BlazeDbType.INT4),                
			//    new ColumnConfiguration( TableString.TaskFieldStart, BlazeDbType.DATETIME),
			//    new ColumnConfiguration( TableString.TaskFieldEnd, BlazeDbType.DATETIME),
			//    new ColumnConfiguration( TableString.TaskFieldSelectedDay, BlazeDbType.STRING),
			//    new ColumnConfiguration( TableString.TaskFieldSelectedMonth, BlazeDbType.STRING),
			//    new ColumnConfiguration( TableString.TaskFieldRepeatPeriod, BlazeDbType.INT4),
			//    new ColumnConfiguration( TableString.TaskFieldRepeatPersistenceTime, BlazeDbType.INT4),
			//    new ColumnConfiguration( TableString.StrategyFieldEnabled, BlazeDbType.INT2)
			//};
            ColumnConfiguration[] updateServer = new ColumnConfiguration[] {
				new ColumnConfiguration( TableString.UpdateServerFieldId, BlazeDbType.INT4, true ),
                new ColumnConfiguration( TableString.UpdateServerFieldAddress, BlazeDbType.SHORT_STRING),
                new ColumnConfiguration( TableString.UpdateServerFieldPort, BlazeDbType.INT2),
				new ColumnConfiguration( TableString.UpdateServerFieldUsername,BlazeDbType.STRING),
                new ColumnConfiguration( TableString.UpdateServerFieldPassword,BlazeDbType.STRING),
                new ColumnConfiguration( TableString.UpdateServerFieldLiteral,BlazeDbType.STRING),
                new ColumnConfiguration( TableString.UpdateServerFieldUpdateServerType,BlazeDbType.INT2)
			};
			ColumnConfiguration[] playerCount = new ColumnConfiguration[] {
				new ColumnConfiguration( TableString.PlayerCountFieldServerId, BlazeDbType.INT4, true ),
                new ColumnConfiguration( TableString.PlayerCountFieldRecordTime, BlazeDbType.DATETIME, true),
                new ColumnConfiguration( TableString.PlayerCountFieldMaxPlayerCount, BlazeDbType.INT2),
				new ColumnConfiguration( TableString.PlayerCountFieldMinPlayerCount, BlazeDbType.INT2),
                new ColumnConfiguration( TableString.PlayerCountFieldAveragePlayerCount, BlazeDbType.INT2)
			};

            ColumnConfiguration[] gmOpLog = new ColumnConfiguration[] {
				new ColumnConfiguration( TableString.GMOpLogFieldId, BlazeDbType.INT4, false,true,true,true,null),
                new ColumnConfiguration( TableString.GMOpLogFieldUserId,BlazeDbType.INT4),
                new ColumnConfiguration( TableString.GMOpLogFieldOperation,BlazeDbType.INT2),
                new ColumnConfiguration( TableString.GMOpLogFieldTarget,BlazeDbType.BLOB),
                new ColumnConfiguration( TableString.GMOpLogFieldServerId,BlazeDbType.INT4),
                new ColumnConfiguration( TableString.GMOpLogFieldDescription,BlazeDbType.BLOB),
                new ColumnConfiguration( TableString.GMOpLogFieldLogTime,BlazeDbType.DATETIME),
                new ColumnConfiguration( TableString.GMOpLogFieldTargetType,BlazeDbType.INT2)
			};

            ColumnConfiguration[] accountInfo = new ColumnConfiguration[] {
				new ColumnConfiguration( TableString.AccountInfoFieldId, BlazeDbType.INT4, false,true,true,true,null),
                new ColumnConfiguration( TableString.AccountInfoFieldName,BlazeDbType.STRING,false,true,false,false,null),
                new ColumnConfiguration( TableString.AccountInfoFieldPassword,BlazeDbType.STRING)
			};

            ColumnConfiguration[] ibShop = new ColumnConfiguration[] {
				new ColumnConfiguration( TableString.IbShopFieldId, BlazeDbType.INT4, false,true,true,true,null),
                new ColumnConfiguration( TableString.IbShopFieldTimeStamp,BlazeDbType.DATETIME),
                new ColumnConfiguration( TableString.IbShopFieldScript,BlazeDbType.BLOB)
			};

            ColumnConfiguration[] batchTask = new ColumnConfiguration[] {
				new ColumnConfiguration( TableString.BatchTaskId, BlazeDbType.INT4,true),
                new ColumnConfiguration( TableString.BatchTaskAutomation,BlazeDbType.BLOB),
                new ColumnConfiguration( TableString.BatchTaskStep,BlazeDbType.INT4),
                new ColumnConfiguration( TableString.BatchTaskServerIds,BlazeDbType.BLOB)
			};

            ColumnConfiguration[] serverEvent = new ColumnConfiguration[] {
                new ColumnConfiguration(TableString.ServerEventFieldServerId, BlazeDbType.INT4, true),
                new ColumnConfiguration(TableString.ServerEventFieldEventTime, BlazeDbType.DATETIME, true),
                new ColumnConfiguration(TableString.ServerEventFieldEventType, BlazeDbType.INT2)
            };

            ColumnConfiguration[] serverOperationLog = new ColumnConfiguration[] {
                new ColumnConfiguration(TableString.ServerOperationLogFieldId, BlazeDbType.INT8,false,true,true,true,null),
                new ColumnConfiguration(TableString.ServerOperationLogFieldOperationId, BlazeDbType.INT8),
                new ColumnConfiguration(TableString.ServerOperationLogFieldUserId, BlazeDbType.INT4),
                new ColumnConfiguration(TableString.ServerOperationLogFieldServerOrGroup, BlazeDbType.INT2),
                new ColumnConfiguration(TableString.ServerOperationLogFieldTargetId, BlazeDbType.INT4),
                new ColumnConfiguration(TableString.ServerOperationLogFieldOperationType, BlazeDbType.INT2),
                new ColumnConfiguration(TableString.ServerOperationLogFieldOperationResult, BlazeDbType.INT2),
                new ColumnConfiguration(TableString.ServerOperationLogFieldDescription, BlazeDbType.LONG_STRING),
                new ColumnConfiguration(TableString.ServerOperationLogFieldLogTime, BlazeDbType.DATETIME),
                new ColumnConfiguration(TableString.ServerOperationLogFieldFatherRegion, BlazeDbType.LONG_STRING)
            };

            ColumnConfiguration[] serverMap = new ColumnConfiguration[] {
				new ColumnConfiguration( TableString.ServerMapFieldId, BlazeDbType.INT4, true),
                new ColumnConfiguration( TableString.ServerMapFieldParentId, BlazeDbType.INT4),
                new ColumnConfiguration( TableString.ServerMapFieldName, BlazeDbType.STRING),
				new ColumnConfiguration( TableString.ServerMapFieldMapType,BlazeDbType.INT4),
                new ColumnConfiguration( TableString.ServerMapFieldIndex,BlazeDbType.INT4),
                new ColumnConfiguration( TableString.ServerMapFieldFlag,BlazeDbType.INT4),
                new ColumnConfiguration( TableString.ServerMapFieldMapServerId,BlazeDbType.INT4)
			};

			DbConfiguration dbCfg = new DbConfiguration(
				DbFactory.DefaultDbName,
				new TableConfiguration[] {
			        new TableConfiguration(TableString.ObjectTableName, colCfgsObj),
			        new TableConfiguration(TableString.UserTableName, colCfgsUser),
			        new TableConfiguration(TableString.UserGroupTableName, colCfgsGroup),
			        new TableConfiguration(TableString.UserInGroupTableName, colCfgsUserGroup),
			        new TableConfiguration(TableString.AceTableName, colCfgsAce),
					new TableConfiguration(TableString.GameServerTableName, colCfgsGameServer),
					new TableConfiguration(TableString.ServerGroupTableName, colCfgsServerGroup),
                    new TableConfiguration(TableString.ServerRegionTableName, colCfgsServerRegion),
					new TableConfiguration(TableString.GameLogTableName, colCfgsGameLog),
                    new TableConfiguration(TableString.ServerPlugInTableName, colCfgsServerPlugIn),
					new TableConfiguration(TableString.StrategyTableName, strategy),
					new TableConfiguration(TableString.ScheduledTaskTableName, scheduledTask),
                    new TableConfiguration(TableString.UpdateServerTableName, updateServer),
					new TableConfiguration(TableString.PlayerCountTableName, playerCount),
                    new TableConfiguration(TableString.GMOpLogTableName,gmOpLog),
                    new TableConfiguration(TableString.AccountInfoTableName,accountInfo),
                    new TableConfiguration(TableString.IbShopTableName,ibShop),
                    new TableConfiguration(TableString.BatchTaskTableName,batchTask),
                    new TableConfiguration(TableString.ServerEventTableName,serverEvent),
                    new TableConfiguration(TableString.ServerOperationLogTableName,serverOperationLog),
                    new TableConfiguration(TableString.ServerMapTableName,serverMap)
			        }
				);
			
			IBlazeDatabase db = DbFactory.GetDatabase();
			db.InitDatabase(dbCfg, rebuild);
		}

		private void InitAutomations()
		{
			AutomationManager.Register(new NullAutomation());
			AutomationManager.Register(new WaitAutomation());
			AutomationManager.Register(new SelectServerAutomation());
			AutomationManager.Register(new CheckServerAutomation());
			AutomationManager.Register(new StartGameAutomation());
			AutomationManager.Register(new StopGameAutomation());
			AutomationManager.Register(new UpdateGameAutomation());
            AutomationManager.Register(new SendMailAutomation());
			AutomationManager.Register(new KillGameAutomation());
			AutomationManager.Register(new CreateServerListFilesAutomation());
			AutomationManager.Register(new LogAutomation());
			AutomationManager.Register(new GetStackTraceAutomation());
			AutomationManager.Register(new SendSystemMessageAutomation());
			AutomationManager.Register(new FilterServerAutomation());
            AutomationManager.Register(new CreateLordConfigAutomation());
            AutomationManager.Register(new FileTransferAutomation());
            AutomationManager.Register(new FileDownloadAutomation());
            AutomationManager.Register(new ExeSysCmdAutomation());
            AutomationManager.Register(new DeleteLocalFileAutomation());
			AutomationManager.Register(new BackupDatabaseAutomation());
			AutomationManager.Register(new RollbackDatabaseAutomation());
            AutomationManager.Register(new BatchTaskAutomation());
            AutomationManager.Register(new SendSMSAutomation());
            AutomationManager.Register(new UploadServerListFilesToFtpAutomation());

            AutomationManager.SecurityObject = SecurityManager.Get(SecurityObjectAutomationManager);
		}

		private void InitStrategy()
		{
			IList<FSEyeEvent> eventList = StrategyManager.EventList;
			eventList.Add(FSEyeEvent.TestEvent);
			eventList.Add(FSEyeEvent.GameStart);
			eventList.Add(FSEyeEvent.GameStop);
			eventList.Add(FSEyeEvent.ServerConnected);
			eventList.Add(FSEyeEvent.ServerDisconnected);
			eventList.Add(FSEyeEvent.UpdateGameBegin);
			eventList.Add(FSEyeEvent.UpdateGameSucceed);
			eventList.Add(FSEyeEvent.UpdateGameFail);
			eventList.Add(FSEyeEvent.GameStartByAccident);
			eventList.Add(FSEyeEvent.GameStopByAccident);
			eventList.Add(FSEyeEvent.ModuleDatabaseError);
			eventList.Add(FSEyeEvent.ModulePaysysError);
		}

		#region 文件管理
		/// <summary>
		/// 列出上传的文件
		/// </summary>
		/// <param name="fileInfo">用于返回的文件信息组</param>
		public FSEyeResult ListUploadedFiles(out FileInfo[] fileInfo)
		{
			try
			{
				DirectoryInfo di = new DirectoryInfo(SystemConfig.Current.AdminServerUploadFileRootPath);
				fileInfo = di.GetFiles();
				return FSEyeResult.fseye_success;
			}
			catch (Exception)
			{
				fileInfo = null;
				return FSEyeResult.as_err;
			}
		}

		/// <summary>
		/// 删除上传文件
		/// </summary>
		/// <param name="fileName">要删除的文件名</param>
		public FSEyeResult DeleteUploadedFile(string fileName)
		{
			try
			{
				FileInfo fi = new FileInfo(SystemConfig.Current.AdminServerUploadFileRootPath + fileName);
				fi.Delete();
				return FSEyeResult.fseye_success;
			}
			catch (Exception)
			{
				return FSEyeResult.as_err;
			}
		}


        public string GetFileMD5(string fileName)
        {
            try
            {
                FileStream get_file = new FileStream(SystemConfig.Current.AdminServerUploadFileRootPath + fileName, FileMode.Open, FileAccess.Read, FileShare.Read);
                byte[] pdfFile = new byte[get_file.Length];
                get_file.Read(pdfFile, 0, (int)get_file.Length);//将文件流读取到Buffer中
                get_file.Close();

                return MD5Buffer(pdfFile, 0, pdfFile.Length);//对Buffer中的字节内容算MD5
            }
            catch (Exception ex)
            {
                return ex.Message;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="pdfFile"></param>
        /// <param name="index"></param>
        /// <param name="count"></param>
        /// <returns></returns>
        private string MD5Buffer(byte[] pdfFile, int index, int count)
        {
            System.Security.Cryptography.MD5CryptoServiceProvider get_md5 = new System.Security.Cryptography.MD5CryptoServiceProvider();
            byte[] hash_byte = get_md5.ComputeHash(pdfFile, index, count);

            string result = System.BitConverter.ToString(hash_byte);
            result = result.Replace("-", "");
            return result;
        }

        private string MD5String(string str)
        {
            byte[] MD5Source = System.Text.Encoding.ASCII.GetBytes(str);
            return MD5Buffer(MD5Source, 0, MD5Source.Length);

        }
		#endregion
		#endregion
	}
}
