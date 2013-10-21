//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 2007-7-2
//      File_base        : GameServerControl
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 游戏服务器控制插件
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using FSEye.DllWrap;
using FSEye.Security;

namespace FSEye.PlugIn
{
	/// <summary>
	/// 游戏服务器控制插件
	/// </summary>
	/// <author>徐晓刚</author>
	public class GameServerControl : BasePlugIn
	{
		public const string PlugInGuid = "{a90a90e3-57d3-4711-9e70-30c53c615d70}";

		public const string DataKeyGameProcessState = "DataKeyGameProcessState";
		public const string DataKeyGetStackTraceState = "DataKeyGetStackTraceState";
		public const string DataKeyStackTrace = "DataKeyStackTrace";
		public const string DataKeyGuardTask = "DataKeyGuardTask";
		public const string DataKeyLastSystemCommand = "DataKeyLastSystemCommand";
		public const string DataKeyLastSystemCommandInput = "DataKeyLastSystemCommandInput";
		public const string DataKeyLastSystemCommandOutput = "DataKeyLastSystemCommandOutput";
		public const string DataKeyUploadDirInfo = "DataKeyUploadDirInfo";
        public const string DataKeyGetServerTime = "DataKeyGetServerTime";
		public const string DataKeyStartGameScript = "DataKeyStartGameScript";
		public const string DataKeyStartGameInput = "DataKeyStartGameInput";
		public const string DataKeyStopGameScript = "DataKeyStopGameScript";
		public const string DataKeyBackupDbScript = "DataKeyBackupDbScript";
		public const string DataKeyRollbackDbScript = "DataKeyRollbackDbScript";

		public const string ActionKeyExeSysCmd = "ActionKeyExeSysCmd";
		public const string ActionKeyStartGame = "ActionKeyStartGame";
		public const string ActionKeyStopGame = "ActionKeyStopGame";
		public const string ActionKeyUpdateGame = "ActionKeyUpdateGame";
		public const string ActionKeyCheckGame = "ActionKeyCheckGame";
		public const string ActionKeyGetStackTrace = "ActionKeyGetStackTrace";
		public const string ActionKeyRefreshUploadDir = "ActionKeyRefreshUploadDir";
		public const string ActionKeyDeleteUploadDirFile = "ActionKeyDeleteUploadDirFile";
		public const string ActionKeyRenemeUploadDirFile = "ActionKeyRenemeUploadDirFile";
		public const string ActionKeyBackupDb = "ActionKeyBackupDb";
		public const string ActionKeyRollbackDb = "ActionKeyRollbackDb";
		public const string ActionKeyRestartDb = "ActionKeyRestartDb";
		public const string ActionKeyRestartServer = "ActionKeyRestartServer";
		public const string ActionKeyUpdateGuard = "ActionKeyUpdateGuard";
		public const string ActionKeyReconfigureGuard = "ActionKeyReconfigureGuard";
        public const string ActionKeySetServerTime = "ActionKeySetServerTime";
        public const string ActionKeyGetServerTime = "ActionKeyGetServerTime";
        public const string ActionKeyGetServerPID = "ActionKeyGetServerPID";

		public const int MaxSystemCommandOutputLength = 20000;
        public const int GetServerTimeTickInterval = 60;

		/// <summary>
		/// Guard任务
		/// </summary>
		public enum GuardTask
		{
			Unknown,
			Idle,
			DoNormalSysCmd,
			StartGameServer,
			UpdateGameServer,
			RefreshUploadDirFileInfo,
			CheckGame,
			GetStatckStrace,
            GetServerTime
		}

		/// <summary>
		/// 游戏进程状态
		/// </summary>
		public enum GameProcessState
		{
			Unknown,
			Checking,
			Exist,
			NotExist
		}

		/// <summary>
		/// 获取StackTrace状态
		/// </summary>
		public enum GetStackTraceState
		{
			Doing,
			Done
		}

		public delegate void ExeSysCmdComplete(GameServer server, bool success, string resultOutput);

		GameProcessState _GameProcessState = GameProcessState.Unknown;

		StringBuilder _stackTrace = new StringBuilder();

		GetStackTraceState _getStackTraceState = GetStackTraceState.Done;

		StringBuilder _updateGameResult = new StringBuilder();

		GuardTask _guardTask;

		string _lastSystemCommand;

		string _lastSystemCommandInput;

		StringBuilder _sysCmdOutput = new StringBuilder();

		string _uploadDirInfo;

        TimeSpan _serverTimeSpan = TimeSpan.MinValue;

		ExeSysCmdComplete _exeSysCmdComplete;

        int _tickCount = 0;

		public override object[] Parameters
		{
			set { }
			get
			{
				return new object[]{
                    0
                };
			}
		}

		public override string[] ParaDescription
		{
			get
			{
				return new string[]{
                    string.Empty
            };
			}
		}

		public override string Name
		{
			get { return "GameServerControl"; }
		}

		public override string Author
		{
			get { return "xuxiaogang"; }
		}

		public override string Guid
		{
			get { return PlugInGuid; }
		}

		public GameServerControl()
		{
		}

		public override void Initialize(GameServer server)
		{
			base.Initialize(server);

			_guardTask = GuardTask.Unknown;
			_server.Connected += new ConnectedEventHandler(this.Connected);
			_server.Disconnected += new DisconnectedEventHandler(this.Disconnected);
			_server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
            _server.Tick += new TickEventHandler(this.Tick);
		}

		public override void Dispose()
		{
			_server.Connected -= new ConnectedEventHandler(this.Connected);
			_server.Disconnected -= new DisconnectedEventHandler(this.Disconnected);
			_server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
            _server.Tick -= new TickEventHandler(this.Tick);
		}

		private void MessageReceived(GameServer server, IProtocol message)
		{
			switch ((ProtocolDef)message.ProtocolId)
			{
				case ProtocolDef.g2e_exesyscmd_def:
					{
						g2e_exesyscmd protocol = message as g2e_exesyscmd;
						FSEyeResult returnCode = (FSEyeResult)protocol.ReturnCode;

						switch (_guardTask)
						{
							case GuardTask.Unknown:
							case GuardTask.Idle:
                                //设置时间服务器断连后的回包处理
                                //if (returnCode == FSEyeResult.g2e_ExeSysCmd_done) break;
								//throw new Exception("State Exception");
							case GuardTask.StartGameServer:
								switch (returnCode)
								{
									case FSEyeResult.g2e_ExeSysCmd_done:
										_guardTask = GuardTask.Idle;
										break;
									case FSEyeResult.g2e_ExeSysCmd_busy:
										break;
									case FSEyeResult.g2e_ExeSysCmd_result:
										AppendSysCmdOutput(protocol.OutputBuff);
										break;
								}
								break;
							case GuardTask.DoNormalSysCmd:
								switch (returnCode)
								{
									case FSEyeResult.g2e_ExeSysCmd_done:
										_guardTask = GuardTask.Idle;                                                                        
										break;
									case FSEyeResult.g2e_ExeSysCmd_busy:
										break;
									case FSEyeResult.g2e_ExeSysCmd_result:
										AppendSysCmdOutput(protocol.OutputBuff);
										break;
								}
								break;
							case GuardTask.UpdateGameServer:
								switch (returnCode)
								{
									case FSEyeResult.g2e_ExeSysCmd_done:
										{
											_guardTask = GuardTask.Idle;
											if (_updateGameResult.ToString().Contains("fail"))
											{
												server.AppendMessage(string.Format("UpdateGameFail\nFailureMessage:\"{0}\"", _updateGameResult.ToString()), GameServer.MessageInfo.Type.Failure);

												//触发UpdateGameFail事件
												AutomationContext context = new AutomationContext();
												context.Message = string.Format("{0} UpdateGameFail\nFailureMessage:\"{1}\"", server.ToString(), _updateGameResult.ToString()); ;
												context.Server = server;
												AdminServer.TheInstance.StrategyManager.OnEvent(FSEyeEvent.UpdateGameFail, context);
											}
											else
											{
												server.AppendMessage("UpdateGameSucceed", GameServer.MessageInfo.Type.Success);

												//触发UpdateGameSucceed事件
												AutomationContext context = new AutomationContext();
												context.Message = string.Format("{0} UpdateGameSucceed", server.ToString());
												context.Server = server;
												AdminServer.TheInstance.StrategyManager.OnEvent(FSEyeEvent.UpdateGameSucceed, context);
											}
										}
										break;
									case FSEyeResult.g2e_ExeSysCmd_busy:
										break;
									case FSEyeResult.g2e_ExeSysCmd_result:
										_updateGameResult.Append(protocol.OutputBuff);
										break;
								}
								break;
							case GuardTask.RefreshUploadDirFileInfo:
								switch (returnCode)
								{
									case FSEyeResult.g2e_ExeSysCmd_done:
										_guardTask = GuardTask.Idle;
                                        break;
									case FSEyeResult.g2e_ExeSysCmd_busy:
										break;
									case FSEyeResult.g2e_ExeSysCmd_result:
                                        _uploadDirInfo += protocol.OutputBuff;
										break;
								}
								break;
							case GuardTask.CheckGame:
								switch (returnCode)
								{
									case FSEyeResult.g2e_ExeSysCmd_done:
										_guardTask = GuardTask.Idle;
										_GameProcessState = (protocol.OutputBuff.Length > 0) ? GameProcessState.Exist : GameProcessState.NotExist;
										break;
									case FSEyeResult.g2e_ExeSysCmd_busy:
										break;
									case FSEyeResult.g2e_ExeSysCmd_result:
										break;
								}
								break;
							case GuardTask.GetStatckStrace:
								switch (returnCode)
								{
									case FSEyeResult.g2e_ExeSysCmd_done:
										_guardTask = GuardTask.Idle;
										_getStackTraceState = GetStackTraceState.Done;
										break;
									case FSEyeResult.g2e_ExeSysCmd_busy:
										break;
									case FSEyeResult.g2e_ExeSysCmd_result:
										_stackTrace.Append(protocol.OutputBuff);
										break;
								}
								break;

                            ////2008-03 huangbiao 去掉getTime任务
                            case GuardTask.GetServerTime:
                            ////    switch (returnCode)
                            ////    {
                            ////        case FSEyeResult.g2e_ExeSysCmd_done:
                            ////            _guardTask = GuardTask.Idle;
                            ////            break;
                            ////        case FSEyeResult.g2e_ExeSysCmd_busy:
                            ////            GetServerTime(1);
                            ////            break;
                            ////        case FSEyeResult.g2e_ExeSysCmd_result:
                            ////            try
                            ////            {
                            ////                string dateText = protocol.OutputBuff.TrimEnd('\n').Replace('_', ' ');
                            ////                _serverTimeSpan = DateTime.Now.Subtract(DateTime.Parse(dateText));
                            ////            }
                            ////            catch (Exception ex)
                            ////            {
                            ////                throw (ex);
                            ////            }
                            ////            break;
                            ////    }
                                break;
							default:
								throw new Exception("State Exception");
						}

						switch (returnCode)
						{
							case FSEyeResult.g2e_ExeSysCmd_done:
								if (_exeSysCmdComplete != null)
								{
									_exeSysCmdComplete(_server, true, _sysCmdOutput.ToString());
									_exeSysCmdComplete = null;
								}
								break;
							case FSEyeResult.g2e_ExeSysCmd_busy:
								if (_exeSysCmdComplete != null)
								{
									_exeSysCmdComplete(_server, false, "busy");
									_exeSysCmdComplete = null;
								}
								break;
							case FSEyeResult.g2e_ExeSysCmd_result:
								break;
						}
					}
					break;
				case ProtocolDef.g2e_GetGuardDir_def:
					{
						g2e_GetGuardDir protocol = message as g2e_GetGuardDir;
						server.GuardDir = protocol.GuardDir;
					}
					break;
				case ProtocolDef.g2e_switchmode_def:
					{

						server.AppendMessage(StringDef.StopConfigGuard, GameServer.MessageInfo.Type.Normal);

                        AdminServer.TheInstance.GameServerManager.SendServerConfigFile(server);
                        
                        foreach (GameServer svr in AdminServer.TheInstance.GameServerManager._gameServerList)
                        {
                            if (svr.IpAddress == server.IpAddress)
                            {
                                svr.ConfigState = GameServer.ConfigGuardState.Configured;
                                AdminServer.TheInstance.GameServerManager.ModifyGameServer(svr, svr.Name, svr.Comment, svr.IpAddress, svr.GameDir, svr.Group, svr.LordConfig, svr.ConfigState, svr.ConfigContent, svr.Type, svr.FtpServer);

                            }
                        }
					}
					break;
				case ProtocolDef.g2e_config_def:
					{
						//TODO 提示重新配置Guard成功
						UpdateGuard(0);
					}
					break;
				default:
					break;
			}
		}

		private void Connected(GameServer server)
		{
            KProtocolHead protocol = new KProtocolHead();
            protocol.ProtocolId = (ushort)ProtocolDef.e2g_GetGuardDir_def;
            server.SendMessage(protocol);

			_guardTask = GuardTask.Idle;
			_GameProcessState = GameProcessState.Unknown;

            switch (server.ConfigState)
            {
                case GameServer.ConfigGuardState.Configuring:
                    SwitchGuardMode();
                    //2008-3 huangbiao 只有在传输完配置文件后才更换状态
                    //server.ConfigState = GameServer.ConfigGuardState.WaitResponse;
                    //SetServerTime(0, DateTime.Now, GetLinuxZoneInfoFile());
                    break;
                case GameServer.ConfigGuardState.None:
                    SwitchGuardMode();
                    server.ConfigState = GameServer.ConfigGuardState.WaitResponse;
                    //SetServerTime(0, DateTime.Now, GetLinuxZoneInfoFile());
                    break;
                case GameServer.ConfigGuardState.WaitResponse:
                    SwitchGuardMode();
                    break;
                case GameServer.ConfigGuardState.Configured:
                    //得到服务器时间
                    ////2008-3 huangbiao
                    ////GetServerTime(1);
                    break;
            }
		}

		private void Disconnected(GameServer server)
		{
			_guardTask = GuardTask.Unknown;
			_GameProcessState = GameProcessState.Unknown;
		}

        private void Tick(GameServer server)
        {
            ++_tickCount;
            if (_tickCount == GetServerTimeTickInterval)
            {
                ////GetServerTime(1);
                _tickCount = 0;
            }
        }

        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
		{
			switch (actionKey)
			{
				case ActionKeyExeSysCmd:
                    //if (args.Length >= 4) _exeSysCmdAutomation = args[3] as ExeSysCmdAutomation;
					ExeSysCmdComplete exeSysCmdComplete = null;
					if (args.Length == 4)
					{
						exeSysCmdComplete = args[3] as ExeSysCmdComplete;
					}
					return ExecuteSystemCommand(userId, args[0] as string, args[1] as string, (GuardTask)args[2], exeSysCmdComplete,true);
				case ActionKeyStartGame:
                    return StartGame(userId);                    
				case ActionKeyStopGame:
                    return StopGame(userId);                    
				case ActionKeyUpdateGame:
                    return UpdateGame(userId,args[0] as string);                                            
				case ActionKeyCheckGame:
					return CheckGame(userId);
				case ActionKeyGetStackTrace:
                    return GetStackStrace(userId);
				case ActionKeyRefreshUploadDir:
                    return RefreshUploadDir(userId,args[0] as string);
				case ActionKeyDeleteUploadDirFile:
                    return DeleteUploadDirFile(userId,args[0] as string);
				case ActionKeyRenemeUploadDirFile:
                    return RenemeUploadDirFile(userId,args[0] as string, args[1] as string);
				case ActionKeyUpdateGuard:
					return UpdateGuard(userId);
				case ActionKeyBackupDb:
					return BackupDb(userId, (int)args[0]);
				case ActionKeyRollbackDb:
					return RollbackDb(userId, (string)args[0]);
				case ActionKeyRestartDb:
					return RestartDb(userId);
				case ActionKeyRestartServer:
					return RestartServer(userId);
				case ActionKeyReconfigureGuard:
					return ReconfigreGuard(userId);
                case ActionKeySetServerTime:
                    return SetServerTime(userId, (DateTime)args[0],(string)args[1]);

                ////2008-3 huangbiao
                ////case ActionKeyGetServerTime:
                ////    return GetServerTime(userId);
			}

			throw new Exception(string.Format("ActionKey {0} does not exist.", actionKey));
		}

        public override object GetData(int userId, string dataKey, params object[] args)
		{
			switch (dataKey)
			{
				case DataKeyGuardTask:
					return _guardTask;
				case DataKeyGameProcessState:
					return _GameProcessState;
				case DataKeyStackTrace:
					return _stackTrace.ToString();
				case DataKeyGetStackTraceState:
					return _getStackTraceState;
				case DataKeyLastSystemCommand:
					return _lastSystemCommand;
				case DataKeyLastSystemCommandInput:
					return _lastSystemCommandInput;
				case DataKeyLastSystemCommandOutput:
					return _sysCmdOutput.ToString();
				case DataKeyUploadDirInfo:
					return _uploadDirInfo;
                case DataKeyGetServerTime:
                    if (_serverTimeSpan != TimeSpan.MinValue)
                        return DateTime.Now.Subtract(_serverTimeSpan);
                    else
						return DateTime.MinValue;
				case DataKeyRollbackDbScript:
					return CreateRollbackDbScript((string)args[0]);
				case DataKeyBackupDbScript:
					return CreateBackupDbScript((int)args[0]);
				case DataKeyStartGameScript:
					return CreateStartGameScript();
				case DataKeyStartGameInput:
					return CreateStartGameInput();
				case DataKeyStopGameScript:
                    return CreateStopGameScript();
			}

			throw new Exception(string.Format("DataKey {0} does not exist.", dataKey));
		}

		/// <summary>
		/// 启动游戏
		/// </summary>
		bool StartGame(int userId)
		{
            bool result = false;

			if (_server.IsConnected && (_server.GameServiceState == GameServer.ServiceState.ErrorStopped || _server.GameServiceState == GameServer.ServiceState.OKStopped))
			{
                string cmdText = CreateStartGameScript();
				if (cmdText != null)
				{
					string input = CreateStartGameInput();
					if (ExecuteSystemCommand(userId, cmdText, input, GuardTask.StartGameServer, false))
					{
                        _server.GameServiceState = GameServer.ServiceState.Starting;
                        result = true;
					}
				}
			}
            User user = AdminServer.TheInstance.SecurityManager.GetUser(userId);
            Security.LogSystem.TheInstance.WriteLog(userId,
                Security.OpType.EXECUTE,
                _server.Id,
                string.Format(StringDef.LogMessageStartGame,
                    user == null ? userId.ToString() : user.UserName,
                    _server.Name,
                    result ? StringDef.Success : StringDef.Fail)
            );
			return result;
		}

        string CreateStartGameScript()
		{            
			return ShellScriptManager.TheInstance.GetScript(_server.Type.ToString()+"_StartGame",_server.SystemName);            
		}

		string CreateStartGameInput()
		{
			return _server.GameDir;
		}

		/// <summary>
		/// 关闭游戏服务
		/// </summary>
		bool StopGame(int userId)
		{
            bool result = false;
            
            //TODO: 暂时不判断状态
			//if (_server.IsConnected && _server.GameServiceState == GameServer.ServiceState.Running)
            if (_server.IsConnected)
			{
                string cmdText = CreateStopGameScript();
                if (cmdText != null)
				{
                    string input = CreateStartGameInput();
                    if (_server.PID <= 0)
                        _server.PID = -2;
                    
                    if (cmdText.Trim().ToLower().IndexOf("e2l_exit_def") >= 0)
                    {
                        // 协议关闭
                        if (_server.DoPlugInAction(userId, ExitOrder.PlugInGuid, ExitOrder.actionKey, _server.Id))
                        {
                            _server.GameServiceState = GameServer.ServiceState.Stopping;
                            result = true;
                        }
                    }
                    else
                    {
                        // 命令行关闭
                        if (ExecuteSystemCommand(userId, cmdText, input, GuardTask.StartGameServer, false))
                        {
                            _server.GameServiceState = GameServer.ServiceState.Stopping;
                            result = true;
                        }
                    }
				}
            }
            User user = AdminServer.TheInstance.SecurityManager.GetUser(userId);
            Security.LogSystem.TheInstance.WriteLog(userId,
                Security.OpType.EXECUTE,
                _server.Id,
                string.Format(StringDef.LogMessageStopGame,
                    user == null ? userId.ToString() : user.UserName,
                    _server.Name,
                    result ? StringDef.Success : StringDef.Fail)
            );
			return result;
		}

		string CreateStopGameScript()
		{
			return ShellScriptManager.TheInstance.GetScript(_server.Type.ToString()+"_StopGame",_server.SystemName);
		}

		/// <summary>
		/// 更新游戏
		/// </summary>
		/// <param name="patchName">更新包的名字</param>
		bool UpdateGame(int userId,string patchName)
		{
            bool result = false;
			if (_server.IsConnected)
			{
				//TODO 这里要确保游戏进程不存在才能允许更新

                string cmdScript = ShellScriptManager.TheInstance.GetScript("UpdateGame", _server.SystemName);
				if (cmdScript != null)
				{                    
					string cmdText = string.Format(
						cmdScript,
						_server.GuardDir + GameServer.FilePathDelimiter + GameServer.UploadFileDir,
						patchName,
						_server.GameInstallDir);
					if (cmdText != null)
					{
						_stackTrace.Length = 0;
						if (ExecuteSystemCommand(userId,cmdText, string.Empty, GuardTask.UpdateGameServer, true))
						{
							_updateGameResult.Length = 0;

							_server.AppendMessage(string.Format("UpdateGameBegin\nPatchName:\"{0}\"", patchName), GameServer.MessageInfo.Type.Alert);

							//触发UpdateGameBegin事件
							AutomationContext context = new AutomationContext();
							context.Message = string.Format("{0} UpdateGameBegin\nPatchName:\"{1}\"", _server.ToString(), patchName); ;
							context.Server = _server;
							AdminServer.TheInstance.StrategyManager.OnEvent(FSEyeEvent.UpdateGameBegin, context);

                            result = true;
						}
					}
				}
            }

            User user = AdminServer.TheInstance.SecurityManager.GetUser(userId);
            Security.LogSystem.TheInstance.WriteLog(userId,
                Security.OpType.EXECUTE,
                _server.Id,
                string.Format(StringDef.LogMessageUpdateGame,
                    user == null ? userId.ToString() : user.UserName,
                    _server.Name,
                    result ? StringDef.Success : StringDef.Fail)
            );
			return result;
		}

		/// <summary>
		/// 检查游戏进程是否存在
		/// </summary>
		bool CheckGame(int userId)
		{
			if (_server.IsConnected)
			{
                string cmdText = ShellScriptManager.TheInstance.GetScript("CheckGame", _server.SystemName);
				if (cmdText != null)
				{
					if (ExecuteSystemCommand(userId,cmdText, string.Empty, GuardTask.CheckGame, true))
					{
						_GameProcessState = GameProcessState.Checking;
						return true;
					}
				}
			}

			return false;
		}

		/// <summary>
		/// 得到宕机堆栈
		/// </summary>
		/// <returns></returns>
		bool GetStackStrace(int userId)
		{
			if (_server.IsConnected)
			{
                string cmdText = ShellScriptManager.TheInstance.GetScript("BT", _server.SystemName);
				if (cmdText != null)
				{
					_stackTrace.Length = 0;
					if (ExecuteSystemCommand(userId,cmdText, _server.GameDir + "/\n", GuardTask.GetStatckStrace, true))
					{
						_getStackTraceState = GetStackTraceState.Doing;
						return true;
					}
				}
			}

			return false;
		}

		/// <summary>
		/// 执行系统命令
		/// </summary>
		bool ExecuteSystemCommand(int userId, string cmd, string input, GuardTask newTask, bool bNeedResult)
		{
			return ExecuteSystemCommand(userId, cmd, input, newTask, null, false, bNeedResult);
		}

		/// <summary>
		/// 执行系统命令（不记录日志）
		/// </summary>
		/// <param name="cmd">命令</param>
		/// <param name="input">输入</param>
		/// <param name="newTask">任务</param>
		bool ExecuteSystemCommand(int userId, string cmd, string input, GuardTask newTask, ExeSysCmdComplete exeSysCmdComplete, bool bNeedResult)
		{
			return ExecuteSystemCommand(userId, cmd, input, newTask, exeSysCmdComplete, false, bNeedResult);
		}

		/// <summary>
		/// 执行系统命令
		/// </summary>
		/// <param name="cmd">命令</param>
		/// <param name="input">输入</param>
		/// <param name="newTask">任务</param>
		/// <param name="needLog">是否需要记录日志</param>
        /// <param name="bNeedResult">是否需要反馈结果</param>
        bool ExecuteSystemCommand(int userId,string cmd, string input, GuardTask newTask, ExeSysCmdComplete exeSysCmdComplete, bool needLog, bool bNeedResult)
		{
			if (cmd == null)
				throw new ArgumentNullException("cmd");
			if (input == null)
				input = string.Empty;

            bool result = false;

            //TODO: if (_server.IsConnected && _guardTask == GuardTask.Idle)
			if (_server.IsConnected)
			{
				IProtocol realProtocol = null;
				if (cmd.Length >= 1024)
				{
					e2g_exesyscmd_large protocol = new e2g_exesyscmd_large();
					protocol.Command = cmd;
					protocol.InputBuff = input;
                    protocol.bNeedOutput = Convert.ToUInt16(bNeedResult);

					realProtocol = protocol;
				}
				else
				{
					e2g_exesyscmd protocol = new e2g_exesyscmd();
					protocol.Command = cmd;
					protocol.InputBuff = input;
                    protocol.bNeedOutput = Convert.ToUInt16(bNeedResult);

					realProtocol = protocol;
				}

				if (_server.SendMessage(realProtocol))
				{
					_lastSystemCommand = cmd;
					_lastSystemCommandInput = input;
					_sysCmdOutput.Length = 0;
					_exeSysCmdComplete = exeSysCmdComplete;

					_guardTask = newTask;
                    					
                    result = true;
				}
			}

			if (needLog)
			{
                User user = AdminServer.TheInstance.SecurityManager.GetUser(userId);
                Security.LogSystem.TheInstance.WriteLog(
                    userId,
                    Security.OpType.EXECUTE,
                    _server.Id,
                    string.Format(StringDef.LogMessageExecuteSystemCommand,
                        user == null ? userId.ToString() : user.UserName,
                        _server.Name,
                        cmd,
                        result ? StringDef.Success : StringDef.Fail)
                );
			}
			return result;
		}

		/// <summary>
		/// 附加系统命令输出
		/// </summary>
		/// <param name="output">系统命令输出</param>
		void AppendSysCmdOutput(string output)
		{
			_sysCmdOutput.Append(output);

			if (_sysCmdOutput.Length > MaxSystemCommandOutputLength)
			{
				_sysCmdOutput.Remove(0, output.Length);
			}
		}

		/// <summary>
		/// 刷新上传目录文件信息
		/// <summary>
		/// <returns>返回发送命令是否成功</returns>
		/// <author>姚瑶</author>
		bool RefreshUploadDir(int userId,string dirPath)
		{
			//string uploadDirPath = _server.GuardDir + GameServer.FilePathDelimiter + GameServer.UploadFileDir;
            string scriptText = ShellScriptManager.TheInstance.GetScript("ListFile", _server.SystemName);
			if (scriptText != null)
			{
                string cmdText = string.Format(scriptText, dirPath);
				if (ExecuteSystemCommand(userId, cmdText, string.Empty, GameServerControl.GuardTask.RefreshUploadDirFileInfo, true))
				{
                    _uploadDirInfo = string.Empty;
					return true;
				}
			}

			return false;
		}

		/// <summary>
		/// 删除游戏服务器文件（上传目录中的）
		/// </summary>
		/// <param name="fileName">文件名（不含路径）</param>
		/// <returns>返回发送命令是否成功</returns>
		/// <author>姚瑶</author>
		bool DeleteUploadDirFile(int userId,string fileName)
		{
            string scriptText = ShellScriptManager.TheInstance.GetScript("DeleteFile", _server.SystemName);
			if (scriptText != null)
			{
                string cmdText = string.Format(scriptText, fileName);
                return ExecuteSystemCommand(userId, cmdText, string.Empty, GameServerControl.GuardTask.DoNormalSysCmd, true);
			}
			return false;
		}

		/// <summary>
		/// 发送重命名游戏服务器端文件的请求（上传目录中的）
		/// </summary>
		/// <param name="oldName">旧文件名（不含路径）</param>
		/// <param name="newName">新文件名（不含路径）</param>
		/// <returns>返回发送命令是否成功</returns>
		/// <author>姚瑶</author>
		bool RenemeUploadDirFile(int userId,string oldName, string newName)
		{
            string scriptText = ShellScriptManager.TheInstance.GetScript("RenameFile", _server.SystemName);
			if (scriptText != null)
			{
                string cmdText = string.Format(scriptText, oldName, newName);
				if (ExecuteSystemCommand(userId,cmdText, string.Empty, GameServerControl.GuardTask.DoNormalSysCmd, true))
				{
					return true;
				}
			}
			return false;
		}

		bool UpdateGuard(int userId)
		{
			if (_server.IsConnected)
			{
				if (_server.GameServerState != GameServer.ServerState.Updating)
				{
                    KProtocolHead protocol = new KProtocolHead();
                    protocol.ProtocolId = (ushort)ProtocolDef.e2g_UpdateGuard_def;
					if (_server.SendMessage(protocol))
					{
						_server.GameServerState = GameServer.ServerState.Updating;
						return true;
					}
				}
			}

			return false;
		}

		bool BackupDb(int userId, int mode)
		{
			string cmd = CreateBackupDbScript(mode);
			return ExecuteSystemCommand(userId, cmd, null, GuardTask.DoNormalSysCmd, true);
		}

		string CreateBackupDbScript(int mode)
		{
			int dbBackupFlag = mode;
			string dbName = _server.LordConfig.DbName;
			string dbUser = _server.LordConfig.DbLoginName;
            string dbPassword = _server.LordConfig.DbLoginPwd;//romandou UtilDllWrap.DBDecrypt(_server.LordConfig.DbLoginPwd, SystemConfig.Current.DefaultEncoding);
			string dbBackupPath = _server.LordConfig.DbBackupPath;

			return ShellScriptManager.TheInstance.GetScript("DBBackUp", _server.SystemName,dbName, dbUser, dbPassword, dbBackupPath, dbBackupFlag);
		}

		bool RestartDb(int userId)
		{
            string cmd = ShellScriptManager.TheInstance.GetScript("DBReboot", _server.SystemName);
			bool result = ExecuteSystemCommand(userId, cmd, null, GuardTask.DoNormalSysCmd, true);
			return result;
		}

		bool RestartServer(int userId)
		{
            string cmd = ShellScriptManager.TheInstance.GetScript("Reboot", _server.SystemName);
			bool result = ExecuteSystemCommand(userId, cmd, null, GuardTask.DoNormalSysCmd, false);
			return result;
		}

		/// <summary>
		/// 切换Guard模式
		/// </summary>
		public void SwitchGuardMode()
		{
			e2g_switchmode switchMode = new e2g_switchmode();
			switchMode.Mode = 0;
            switchMode.wServer = (UInt16)_server.Id;
			_server.SendMessage(switchMode);
			_server.AppendMessage(StringDef.SwitchGuardMode, GameServer.MessageInfo.Type.Success);
		}

		/// <summary>
		/// 重新配置Guard
		/// </summary>
		bool ReconfigreGuard(int userId)
		{
			if (_server.IsConnected)
			{

                AdminServer.TheInstance.GameServerManager.SendServerConfigFile(_server);
				string fileContentString = null;
				using (MemoryStream configStream = new MemoryStream())
				{
					_server.LordConfig.WriteGuardConfig(configStream);
                    fileContentString = SystemConfig.Current.DefaultEncoding.GetString(configStream.ToArray()); //Encoding.ASCII.GetString(configStream.ToArray());
				}

				e2g_ConfigInfo configInfo = new e2g_ConfigInfo();
				configInfo.ConfigFileDataLength = (ushort)fileContentString.Length;
				configInfo.ConfigFileData = fileContentString;

				byte[] configInfoBytes = configInfo.ToBytes();

				e2g_config configProtocol = new e2g_config();
				Array.Copy(configInfoBytes, configProtocol.ConfigInfo, configInfoBytes.Length);

				if (_server.SendMessage(configProtocol))
				{
					_server.AppendMessage(StringDef.AttemptToConfigGuard, GameServer.MessageInfo.Type.Normal);
                    _server.Disconnect();
					return true;
				}
			}
            else
            {
                foreach (GameServer svr in AdminServer.TheInstance.GameServerManager.Servers)
                {
                    if (_server.IpAddress == svr.IpAddress)
                    {
                        svr.ConfigState = GameServer.ConfigGuardState.Configuring;

                    }
                }
                return true;
            }
			return false;
		}

		bool RollbackDb(int userId, string dumpFile)
		{
			string cmd = CreateRollbackDbScript(dumpFile);
			return ExecuteSystemCommand(userId, cmd, null, GuardTask.DoNormalSysCmd, false);
		}

		string CreateRollbackDbScript(string dumpFile)
		{
			string dbName = _server.LordConfig.DbName;
			string dbUser = _server.LordConfig.DbLoginName;
            string dbPassword = _server.LordConfig.DbLoginPwd;//romandou UtilDllWrap.DBDecrypt(_server.LordConfig.DbLoginPwd, SystemConfig.Current.DefaultEncoding);
			string dbRollbackFilePath = _server.LordConfig.DbBackupPath + "/" + dumpFile;

			return ShellScriptManager.TheInstance.GetScript("DBRollBack",_server.SystemName, dbName, dbUser, dbPassword, dbRollbackFilePath);
		}

        /// <summary>
        /// 设置服务器时间
        /// </summary>
        /// <param name="userId">用户Id</param>
        /// <param name="dateTime">时间</param>
        /// <returns></returns>
        bool SetServerTime(int userId, DateTime dateTime,string zoneFileName)
        {
            if (dateTime == DateTime.MinValue) return false;
            string scriptText = scriptText = ShellScriptManager.TheInstance.GetScript("SetTime",_server.SystemName, zoneFileName, dateTime.ToString("yyyy-MM-dd"), dateTime.ToString("HH:mm:ss"));
            if (scriptText != null)
            {
                if (ExecuteSystemCommand(userId, scriptText, string.Empty, GameServerControl.GuardTask.DoNormalSysCmd, true))
                {
                    //, new ExeSysCmdComplete(SetServerTimeComplete)
                    _server.AppendMessage(string.Format("SetGameServerTime\nServerTime:{0}", dateTime.ToString("yyyy-MM-dd HH:mm:ss)")), GameServer.MessageInfo.Type.Alert);
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// 设置服务器时间回调函数
        /// </summary>
        void SetServerTimeComplete(GameServer server, bool success, string resultOutput)
        {
            //if (success)
            //    GetServerTime(1);
        }

        /// <summary>
        /// 得到服务器时间
        /// </summary>
        /// <param name="userId">用户Id</param>
        /// <param name="dateTime">时间</param>
        /// <returns></returns>
        bool GetServerTime(int userId)
        {
            return true;
            //string scriptText = scriptText = ShellScriptManager.TheInstance.GetScript("GetTime", _server.SystemName);
            //if (scriptText != null)
            //{
            //    return ExecuteSystemCommand(userId, scriptText, string.Empty, GameServerControl.GuardTask.GetServerTime, true);
            //}
            //return false;
        }

        /// <summary>
        /// 根据FSEye本地时区设置服务器时区
        /// 返回Linux上时区文件名
        /// </summary>
        /// <returns></returns>
        public static string GetLinuxZoneInfoFile()
        {
            string timeZoneFile = string.Empty;
            switch (TimeZone.CurrentTimeZone.GetUtcOffset(DateTime.Now).Hours)
            {
                case -11:
                    break;
                case -10:
                    break;
                case -9:
                    break;
                case -8:
                    break;
                case -7:
                    break;
                case -6:
                    //timeZoneFile = "America/Mexico_City";     墨西哥城是-5区的?!
                    break;
                case -5:
                    timeZoneFile = "America/Lima";
                    break;
                case -4:
                    timeZoneFile = "America/Santiago";
                    break;
                case -3:
                    break;
                case -2:
                    break;
                case -1:
                    break;
                case 0:
                    timeZoneFile = "Greenwich";
                    break;
                case 1:
                    timeZoneFile = "Europe/Berlin";
                    break;
                case 2:
                    timeZoneFile = "Europe/Athens";
                    break;
                case 3:
                    timeZoneFile = "Europe/Moscow";
                    break;
                case 4:
                    break;
                case 5:
                    break;
                case 6:
                    timeZoneFile = "Asia/Dhaka";
                    break;
                case 7:
                    //河内
                    timeZoneFile = "Asia/Bangkok";
                    break;
                case 8:
                    timeZoneFile = "Asia/Shanghai";
                    break;
                case 9:
                    timeZoneFile = "Asia/Seoul";
                    break;
                case 10:
                    timeZoneFile = "Australia/Sydney";
                    break;
                case 11:
                    break;
                case 12:
                    timeZoneFile = "Pacific/Fiji";
                    break;
                case 13:
                    break;
            }
            //默认为UTC
            if (timeZoneFile.Length == 0) timeZoneFile = "UTC";
            return timeZoneFile;
        }
	}
}
