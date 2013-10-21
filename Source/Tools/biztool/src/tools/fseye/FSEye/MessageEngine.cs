//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-8
//      File_base        : MessageEngine
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 消息引擎
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.IO;
using FSEye.Net;
using FSEye.DllWrap;

namespace FSEye
{
	/// <summary>
	/// 消息引擎
	/// </summary>
	/// <author>徐晓刚</author>
	class MessageEngine : IDisposable
	{
		public enum EngineState
		{
			Stopped,
			Running,
			Quit
		}
		
		Thread _processThread;

		EngineState _state;

		bool _disposed = false;

		public MessageEngine()
		{
			_state = EngineState.Stopped;
			_processThread = new Thread(new ThreadStart(MessageProcess));
			_processThread.Start();
		}

		public void Startup()
		{
			if (_disposed)
			{
				throw new ObjectDisposedException(this.ToString());
			}
			
			_state = EngineState.Running;
		}

		public void Shutdown()
		{
			if (_disposed)
			{
				throw new ObjectDisposedException(this.ToString());
			}

			_state = EngineState.Stopped;
		}

		public void Dispose()
		{
			lock (this)
			{
				if (!_disposed)
				{
					_state = EngineState.Quit;
					_processThread = null;

					GC.SuppressFinalize(this);

					_disposed = true;
				}
			}
		}

		/// <summary>
		/// 消息处理
		/// </summary>
		private void MessageProcess()
		{
			while (_state != EngineState.Quit)
			{
				if (_state == EngineState.Running)
				{
					IList servers = AdminServer.TheInstance.GameServerManager.Servers;
					lock (servers.SyncRoot)
					{
						for (int i = 0; i < servers.Count; i++)
						{
                            
							GameServer server = servers[i] as GameServer;
							if (server.IsConnected)
                            {
                               
									byte[] msg = AdminServer.TheInstance.ConnectionManager.ReciveFrom(server.ConnectionId);
                                    try
                                    {
									if (msg != null && msg.Length >= 2)
									{
                                        KProtocolHead prohead = new KProtocolHead();
                                        prohead.FromBytes(msg);
                                        if (prohead.ServerID == 0)
                                        {
                                            //Dispatch(server, msg);//romandouUtilDllWrap.RC4Encrypt(msg, server.CommunicationKey));

                                            for (int j = 0; j < servers.Count; j++)
                                            {
                                                GameServer tmpserver = servers[j] as GameServer;
                                                if (tmpserver.ConnectionId == server.ConnectionId)
                                                {
                                                    Dispatch(tmpserver, msg);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            for (int j = 0; j < servers.Count; j++)
                                            {
                                                GameServer tmpserver = servers[j] as GameServer;
                                                if (tmpserver.Id == prohead.ServerID)
                                                {
                                                    Dispatch(tmpserver, msg);
                                                    break;
                                                }
                                            }
                                        }
									}
								}
                                catch (Exception ex)
                                {
                                    Util.DebugLog("Normal protocol parse exception: " + ex.StackTrace);
                                    //AdminServer.TheInstance.ConnectionManager.Disconnect(server.ConnectionId);
                                }
							}
						}
					}

					IList authenticationList = AdminServer.TheInstance.GameServerManager.AuthenticationList;
					lock (authenticationList.SyncRoot)
					{
						if (authenticationList.Count > 0)
						{
							ArrayList invalidList = new ArrayList();
							foreach (GameServerAuthentication authentication in authenticationList)
							{
								authentication.Tick();
								if (authentication.Valid)
								{
									try
									{
										byte[] msg = AdminServer.TheInstance.ConnectionManager.ReciveFrom(authentication.ConnectionId);
										if (msg != null && msg.Length >= 2)
										{
											DispatchAuthentication(authentication, msg);//romandou UtilDllWrap.RC4Encrypt(msg, authentication.CommunicationKey));
										}
									}
									catch (Exception ex)
									{
										Util.DebugLog("Authentication protocol parse exception: " + ex.StackTrace);
										AdminServer.TheInstance.ConnectionManager.Disconnect(authentication.ConnectionId);
									}
								}
								else
								{
									invalidList.Add(authentication);
								}
							}

							if (invalidList.Count > 0)
							{
								foreach (GameServerAuthentication authentication in invalidList)
								{
									authentication.Dispose();
									authenticationList.Remove(authentication);
								}
							}
						}
					}
				}
				Thread.Sleep(1);
			}
		}

		/// <summary>
		/// 分发消息
		/// </summary>
		/// <param name="msgBuff">消息</param>
		private void Dispatch(GameServer server, byte[] msgBuff)
		{
			if (server == null || msgBuff == null)
				return;

            if (msgBuff.Length <= 2)
            {
                return;
            }


			ProtocolDef protocalId = (ProtocolDef)BitConverter.ToUInt16(msgBuff, 0);
			IProtocol message = null;
			switch (protocalId)
			{
				#region 文件管理
				case ProtocolDef.g2e_openfile_def://打开文件
                    message = new g2e_openfile();
                    message.FromBytes(msgBuff);
					break;
				case ProtocolDef.g2e_seekfile_def://移动到文件中的位置
					message = new g2e_seekfile();
					break;
				case ProtocolDef.g2e_writefile_def://写入文件
					message = new g2e_writefile();
					break;
				case ProtocolDef.g2e_closefile_def://关闭文件
					message = new g2e_closefile();
					break;
                case ProtocolDef.g2e_readfile_def: //读文件
                    message = new g2e_readfile();
                    break;
				#endregion

				#region 游戏服务器信息
				case ProtocolDef.g2e_getcpubaseinfo_def://取得CPU基本信息
					message = new g2e_getcpubaseinfo();
					break;
				case ProtocolDef.g2e_getcpuload_def://取得CPU负载
					message = new g2e_getcpuload();
					break;
				case ProtocolDef.g2e_getdiskinfo_def://取得磁盘信息
                    message = new g2e_getdiskinfo();
					break;
				case ProtocolDef.g2e_getmeminfo_def://取得内存信息
                    message = new g2e_getmeminfo();
					break;
				case ProtocolDef.g2e_getprocinfo_def://取得进程信息
                    message = new g2e_getprocinfo();
					break;
				case ProtocolDef.g2e_getcardload_def://取得网卡负载
                    message = new g2e_getcardload();					
					break;
				case ProtocolDef.g2e_getnetinfo_def://取得网卡信息
                    message = new g2e_getnetinfo();
					break;
                case ProtocolDef.g2e_getpluginfo_def://取得插件信息
                    message = new g2e_getpluginfo();
                    break;				
				#endregion

				#region 其他
				case ProtocolDef.g2e_ping_def://Ping
                    message = new KProtocolHead();
                    break;
				case ProtocolDef.g2e_exesql_def:
					message = new g2e_exesql();
					break;
				case ProtocolDef.g2e_exesyscmd_def:
					message = new g2e_exesyscmd();
					break;
				case ProtocolDef.g2e_GetGuardDir_def:
					message = new g2e_GetGuardDir();
					break;
				case ProtocolDef.g2e_switchmode_def:
					message = new KProtocolHead();
					break;
				case ProtocolDef.g2e_config_def:
					message = new KProtocolHead();
					break;
                case ProtocolDef.g2e_updateserver_def:
                    message = new g2e_updateserver();
                    break;
                case ProtocolDef.g2e_ftpdownload_def:
                    message = new g2e_ftpdownload();
                    break;
                case ProtocolDef.g2e_serverpid_def:
                    message = new g2e_server_pid();
                    break;                        
				#endregion

				#region 与游戏的通讯
			
                case ProtocolDef.l2e_header_def:
                    {
                        if (msgBuff.Length <= 4)
                            return;
                        ProtocolDef subProtocolId = (ProtocolDef)BitConverter.ToUInt16(msgBuff, 4);

                        switch (subProtocolId)
                        {
                            // ahpho
                            case ProtocolDef.l2e_update_custom_info_def:
                                message = new l2e_update_custom_info();
                                break;
                            case ProtocolDef.l2e_PlayerCount_def:
                                message = new l2e_PlayerCount();
                                break;
                            case ProtocolDef.l2e_ExeGMCmd_def:
                                message = new l2e_ExeGMCmd();
                                break;
                            case ProtocolDef.l2e_GetBasicInfo_def:
                                message = new l2e_GetBasicInfo();
                                break;
                            case ProtocolDef.l2e_Who_def:
                                message = new l2e_Who();
                                break;
                            case ProtocolDef.l2e_GetGlobalVariable_def:
                                message = new l2e_GetGlobalVariable();
                                break;
                            case ProtocolDef.l2e_GetGameStartTime_def:
                                message = new l2e_GetGameStartTime();
                                break;
                            case ProtocolDef.l2e_ReportError_def:
                                message = new l2e_ReportError();
                                break;
                            case ProtocolDef.l2e_info_def:
                                message = new l2e_info();
                                break;
                            case ProtocolDef.l2e_info_large_def:
                                message = new l2e_info_large();
                                break;
                            case ProtocolDef.l2e_ping_def:
                                message = new KSubProtocolHead();
                                break;
                            default:
                                throw new Exception("收到未在MessageEngine中注册的协议包");
                        }
                    }
                    break;
				#endregion

				default:
					//TODO 记录没有出现过的协议包，开发阶段抛出异常
					throw new Exception("收到未在MessageEngine中注册的协议包");
			}

			if (server != null && message != null)
			{
                message.FromBytes(msgBuff);
				AdminServer.TheInstance.GameServerManager.ProcessMessage(server, message);
			}
		}

		/// <summary>
		/// 分发认证消息
		/// </summary>
		/// <param name="authentication">服务器认证</param>
		/// <param name="msgBuff">消息</param>
		private void DispatchAuthentication(GameServerAuthentication authentication, byte[] msgBuff)
		{
			if (authentication == null || msgBuff == null)
				return;

			ProtocolDef protocalId = (ProtocolDef)BitConverter.ToUInt16(msgBuff, 0);
			IProtocol message = null;
			switch (protocalId)
			{
				case ProtocolDef.g2e_Authentication_def:
					message = new g2e_Authentication();
                    message.FromBytes(msgBuff);
					break;
				//default:
				//throw new Exception("收到不应该在认证阶段发送的协议包");
			}

			if (message != null)
			{
				authentication.Authentication(message);
			}
		}
	}
}
