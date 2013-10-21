//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-9-14
//      File_base        : PlugIns
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 部分插件
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Globalization;
using System.Reflection;

namespace FSEye.PlugIn
{
    public class DefaultParameter
    {
        public const int UpdateNetworkLoadTickDo = 2;
        public const int UpdateCpuLoadTickDo = 2;
        public const int UpdateMemoryInfoTickDo = 2;
        public const int UpdateProcessInfoTickDo = 5;
        public const int UpdatePlugInInfoTickDo = 5;
        public const int UpdateDiskInfoTickDo = 5;
        public const int CheckPingTickDo = 50;
        public const int PingTimeOutTick = 55;
        public const string LogFile = @"D:\log.txt";
        public const int LordCheckPingTickDo = 15;
        public const int LordPingTimeOutTick = 33;
        public const string UpDateTimeDes = "更新的时间周期(秒)";
        public const string PingTimeOutTickDes = "超时时间(秒)";
        public const string PingTimeDes = "Ping的时间周期(秒)";
        public const string FilePathDes = "日志文件路径";//StringDef.LogFilePath;
    }

	#region UpdateNetworkCard
	/// <summary>
	/// 更新网卡信息
	/// </summary>
	/// <author>徐晓刚</author>
	public class UpdateNetworkCard : BasePlugIn
	{
		public const string PlugInGuid = "{895235f4-47d8-4ddb-af7c-fdb3591d437f}";

		int _updateNetworkLoadTickCurrent;

		int _updateNetworkLoadTickDo;

		public override object[] Parameters
        {
            set
            {
                _updateNetworkLoadTickDo = int.Parse(value[1].ToString());
            }
            get
            {
                return new object[]{
                    1,
                    _updateNetworkLoadTickDo
                };
            }
        }

		public override string[] ParaDescription
        {
            get
            {
                return new string[]{
                    DefaultParameter.UpDateTimeDes
                };
            }
        }

		public override string Name
		{
			get { return "UpdateNetworkCard"; }
		}

		public override string Author
		{
			get { return "xuxiaogang"; }
		}

		public override string Guid
		{
			get { return PlugInGuid; }
		}

        public UpdateNetworkCard()
        {
            _updateNetworkLoadTickDo = DefaultParameter.UpdateNetworkLoadTickDo;
        }

		public UpdateNetworkCard(int updateInterval)
		{
			_updateNetworkLoadTickDo = updateInterval;
		}

		public override void Initialize(GameServer server)
		{
			base.Initialize(server);

			_updateNetworkLoadTickCurrent = 0;
			_server.Connected += new ConnectedEventHandler(this.Connected);
			_server.Tick += new TickEventHandler(this.Tick);
			_server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
		}

		public override void Dispose()
		{
			_server.Connected -= new ConnectedEventHandler(this.Connected);
			_server.Tick -= new TickEventHandler(this.Tick);
			_server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
		}

		private void MessageReceived(GameServer server, IProtocol message)
		{
			switch ((ProtocolDef)message.ProtocolId)
			{
				case ProtocolDef.g2e_getcardload_def:
					{
						g2e_getcardload protocol = message as g2e_getcardload;
						netcardload[] loads = new netcardload[protocol.nCardCount];
						Array.Copy(protocol.Card, loads, loads.Length);
						server.UpdateNetworkLoad(loads);
					}
					break;
				case ProtocolDef.g2e_getnetinfo_def:
					{
						g2e_getnetinfo protocol = message as g2e_getnetinfo;
						netcardinfo[] infos = new netcardinfo[protocol.nCardCount];
						Array.Copy(protocol.Card, infos, infos.Length);
						server.UpdateNetcardInfo(protocol.szHostName, protocol.szSystemName, infos);
					}
					break;
				default:
					break;
			}
		}

		private void Tick(GameServer server)
		{
			if (server != null)
			{
				if (server.IsConnected)
				{
					_updateNetworkLoadTickCurrent++;
					if (_updateNetworkLoadTickCurrent >= _updateNetworkLoadTickDo)
					{
                        KProtocolHead protocol = new KProtocolHead();
                        protocol.ProtocolId = (ushort)ProtocolDef.e2g_getcardload_def;
						if (server.SendMessage(protocol))
						{
							_updateNetworkLoadTickCurrent = 0;
						}
					}
				}
			}
		}

		private void Connected(GameServer server)
		{
            KProtocolHead protocol = new KProtocolHead();
            protocol.ProtocolId = (ushort)ProtocolDef.e2g_getnetbaseinfo_def;
			server.SendMessage(protocol);
		}

		#region IPlugIn Members


        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

        public override object GetData(int userId, string dataKey, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		#endregion
	}
	#endregion

	#region UpdateCpu
	/// <summary>
	/// 更新CPU信息
	/// </summary>
	/// <author>徐晓刚</author>
	public class UpdateCpu : BasePlugIn
	{
		public const string PlugInGuid = "{7008f6d4-73ea-419f-8f08-5fc7f81740ea}";

		int _updateCpuLoadTickCurrent;

		int _updateCpuLoadTickDo;

		public override object[] Parameters
        {
            set
            {
                _updateCpuLoadTickDo = int.Parse(value[1].ToString());
            }
            get
            {
                return new object[]{
                    1,
                    _updateCpuLoadTickDo
                };
            }
        }

		public override string[] ParaDescription
        {
            get
            {
                return new string[]{
                    DefaultParameter.UpDateTimeDes
                };
            }
        }

		public override string Name
		{
			get { return "UpdateCpu"; }
		}

		public override string Author
		{
			get { return "xuxiaogang"; }
		}

		public override string Guid
		{
			get { return PlugInGuid; }
		}

        public UpdateCpu()
        {
            _updateCpuLoadTickDo = DefaultParameter.UpdateCpuLoadTickDo;
        }

		public UpdateCpu(int updateInterval)
		{
			_updateCpuLoadTickDo = updateInterval;
		}

		public override void Initialize(GameServer server)
		{
			base.Initialize(server);

			_updateCpuLoadTickCurrent = 0;
			_server.Connected += new ConnectedEventHandler(this.Connected);
			_server.Tick += new TickEventHandler(this.Tick);
			_server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
		}

		public override void Dispose()
		{
			_server.Connected -= new ConnectedEventHandler(this.Connected);
			_server.Tick -= new TickEventHandler(this.Tick);
			_server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
		}

		private void MessageReceived(GameServer server, IProtocol message)
		{
			switch ((ProtocolDef)message.ProtocolId)
			{
				case ProtocolDef.g2e_getcpuload_def:
					{
						g2e_getcpuload protocol = message as g2e_getcpuload;
						ushort[] loads = new ushort[protocol.nCPUCount];
						Array.Copy(protocol.nLoad, loads, loads.Length);
						server.UpdateCpuLoad(loads);
					}
					break;
				case ProtocolDef.g2e_getcpubaseinfo_def:
					{
						g2e_getcpubaseinfo protocol = message as g2e_getcpubaseinfo;
						cpubaseinfo[] infos = new cpubaseinfo[protocol.nCPUCount];
						Array.Copy(protocol.Processor, infos, infos.Length);
						server.UpdateCpuBasicInfo(infos);
					}
					break;
				default:
					break;
			}
		}

		private void Tick(GameServer server)
		{
			if (server != null)
			{
				if (server.IsConnected)
				{
					_updateCpuLoadTickCurrent++;
					if (_updateCpuLoadTickCurrent >= _updateCpuLoadTickDo)
					{
                        KProtocolHead protocol = new KProtocolHead();
                        protocol.ProtocolId = (ushort)ProtocolDef.e2g_getcpuload_def;
                        if (server.SendMessage(protocol))
						{
							_updateCpuLoadTickCurrent = 0;
						}
					}
				}
				else
				{
					_updateCpuLoadTickCurrent = 0;
				}
			}
		}

		private void Connected(GameServer server)
		{
            KProtocolHead protocol = new KProtocolHead();
            protocol.ProtocolId = (ushort)ProtocolDef.e2g_getcpubaseinfo_def;
			server.SendMessage(protocol);
		}

		#region IPlugIn Members


        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

        public override object GetData(int userId, string dataKey, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		#endregion
	}
	#endregion

	#region UpdateMemory
	/// <summary>
	/// 更新内存信息
	/// </summary>
	/// <author>徐晓刚</author>
	public class UpdateMemory : BasePlugIn
	{
		public const string PlugInGuid = "{63b1a0d7-11cc-4680-9b4a-04740dd4f2ea}";

		int _updateMemoryInfoTickCurrent;

		int _updateMemoryInfoTickDo;

		public override object[] Parameters
        {
            set
            {
                _updateMemoryInfoTickDo = int.Parse(value[1].ToString());
            }
            get
            {
                return new object[]{
                    1,
                    _updateMemoryInfoTickDo
                };
            }
        }

		public override string[] ParaDescription
        {
            get
            {
                return new string[]{
                    DefaultParameter.UpDateTimeDes
                };
            }
        }

		public override string Name
		{
			get { return "UpdateMemory"; }
		}

		public override string Author
		{
			get { return "xuxiaogang"; }
		}

		public override string Guid
		{
			get { return PlugInGuid; }
		}

        public UpdateMemory()
        {
            _updateMemoryInfoTickDo = DefaultParameter.UpdateMemoryInfoTickDo;
        }

		public UpdateMemory(int interval)
		{
			_updateMemoryInfoTickDo = interval;
		}

		public override void Initialize(GameServer server)
		{
			base.Initialize(server);

			_updateMemoryInfoTickCurrent = 0;
			_server.Tick += new TickEventHandler(this.Tick);
			_server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
		}

		public override void Dispose()
		{
			_server.Tick -= new TickEventHandler(this.Tick);
			_server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
		}

		private void MessageReceived(GameServer server, IProtocol message)
		{
			switch ((ProtocolDef)message.ProtocolId)
			{
				case ProtocolDef.g2e_getmeminfo_def:
					g2e_getmeminfo protocol = message as g2e_getmeminfo;
					server.UpdateMemoryInfo(protocol.nFreeMem, protocol.nTotalMem);
					break;
				default:
					break;
			}
		}

		private void Tick(GameServer server)
		{
			if (server != null)
			{
				if (server.IsConnected)
				{
					_updateMemoryInfoTickCurrent++;
					if (_updateMemoryInfoTickCurrent >= _updateMemoryInfoTickDo)
					{
                        KProtocolHead protocol = new KProtocolHead();
                        protocol.ProtocolId = (ushort)ProtocolDef.e2g_getmeminfo_def;
						if (server.SendMessage(protocol))
						{
							_updateMemoryInfoTickCurrent = 0;
						}
					}
				}
				else
				{
					_updateMemoryInfoTickCurrent = 0;
				}
			}
		}

		#region IPlugIn Members


        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

        public override object GetData(int userId, string dataKey, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		#endregion
	}
	#endregion

	#region UpdateProcess
	/// <summary>
	/// 更新进程信息
	/// </summary>
	/// <author>徐晓刚</author>
	public class UpdateProcess : BasePlugIn
	{
		public const string PlugInGuid = "{7025123c-4eee-4461-a183-a81535903a59}";

		int _updateProcessInfoTickCurrent;

        int _updateProcessInfoTickDo;

		public override object[] Parameters
        {
            set
            {
                _updateProcessInfoTickDo = int.Parse(value[1].ToString());
            }
            get
            {
                return new object[]{
                    1,
                    _updateProcessInfoTickDo
                };
            }
        }

		public override string[] ParaDescription
        {
            get
            {
                return new string[]{
                    DefaultParameter.UpDateTimeDes
                };
            }
        }

		public override string Name
		{
			get { return "UpdateProcess"; }
		}

		public override string Author
		{
			get { return "xuxiaogang"; }
		}

		public override string Guid
		{
			get { return PlugInGuid; }
		}

        public UpdateProcess()
        {
            _updateProcessInfoTickDo = DefaultParameter.UpdateProcessInfoTickDo;
        }

		public UpdateProcess(int updateInterval)
		{
			_updateProcessInfoTickDo = updateInterval;
		}

		public override void Initialize(GameServer server)
		{
			base.Initialize(server);

			_updateProcessInfoTickCurrent = 0;
			_server.Tick += new TickEventHandler(this.Tick);
			_server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
		}

		public override void Dispose()
		{
			_server.Tick -= new TickEventHandler(this.Tick);
			_server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
		}

		private void MessageReceived(GameServer server, IProtocol message)
		{
			switch ((ProtocolDef)message.ProtocolId)
			{
				case ProtocolDef.g2e_getprocinfo_def:
					g2e_getprocinfo protocol = message as g2e_getprocinfo;
					procinfo[] infos = new procinfo[protocol.nProcCount];
					Array.Copy(protocol.Proc, infos, infos.Length);
					server.UpdateProcessInfo(infos);
					break;
				default:
					break;
			}
		}

		private void Tick(GameServer server)
		{
			if (server != null)
			{
				if (server.IsConnected)
				{
					_updateProcessInfoTickCurrent++;
					if (_updateProcessInfoTickCurrent >= _updateProcessInfoTickDo)
					{
                        KProtocolHead protocol = new KProtocolHead();
                        protocol.ProtocolId = (ushort)ProtocolDef.e2g_getprocinfo_def;
						if (server.SendMessage(protocol))
						{
							_updateProcessInfoTickCurrent = 0;
						}
					}
				}
				else
				{
					_updateProcessInfoTickCurrent = 0;
				}
			}
		}

		#region IPlugIn Members


        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

        public override object GetData(int userId, string dataKey, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		#endregion
	}
	#endregion

	#region UpdatePlugIn
	/// <summary>
	/// 更新插件信息
	/// </summary>
	/// <author>徐晓刚</author>
	public class UpdatePlugIn : BasePlugIn
	{
		public const string PlugInGuid = "{91e63a4f-9ead-4127-bb5d-19f67622ab9d}";

		int _updatePlugInInfoTickCurrent;

		int _updatePlugInInfoTickDo;

		public override object[] Parameters
        {
            set
            {
                _updatePlugInInfoTickDo = int.Parse(value[1].ToString());
            }
            get
            {
                return new object[]{
                    1,
                    _updatePlugInInfoTickDo
                };
            }
        }

		public override string[] ParaDescription
        {
            get
            {
                return new string[]{
                    DefaultParameter.UpDateTimeDes
                };
            }
        }

		public override string Name
		{
			get { return "UpdatePlugIn"; }
		}

		public override string Author
		{
			get { return "xuxiaogang"; }
		}

		public override string Guid
		{
			get { return PlugInGuid; }
		}

        public UpdatePlugIn()
        {
            _updatePlugInInfoTickDo = DefaultParameter.UpdatePlugInInfoTickDo;
        }

		public UpdatePlugIn(int updateInterval)
		{
			_updatePlugInInfoTickDo = updateInterval;
		}

		public override void Initialize(GameServer server)
		{
			base.Initialize(server);

			_updatePlugInInfoTickCurrent = 0;
			_server.Tick += new TickEventHandler(this.Tick);
			_server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
		}

		public override void Dispose()
		{
			_server.Tick -= new TickEventHandler(this.Tick);
			_server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
		}

		private void MessageReceived(GameServer server, IProtocol message)
		{
			switch ((ProtocolDef)message.ProtocolId)
			{
				case ProtocolDef.g2e_getpluginfo_def:
					g2e_getpluginfo protocol = message as g2e_getpluginfo;
					pluginfo[] infos = new pluginfo[protocol.PlugCount];
					Array.Copy(protocol.PlugInfo, infos, infos.Length);
					server.UpdatePlugInInfo(infos);
					break;
				default:
					break;
			}
		}

		private void Tick(GameServer server)
		{
			if (server != null)
			{
				if (server.IsConnected)
				{
					_updatePlugInInfoTickCurrent++;
					if (_updatePlugInInfoTickCurrent >= _updatePlugInInfoTickDo)
					{
                        KProtocolHead protocol = new KProtocolHead();
                        protocol.ProtocolId = (ushort)ProtocolDef.e2g_getpluginfo_def;
						if (server.SendMessage(protocol))
						{
							_updatePlugInInfoTickCurrent = 0;
						}
					}
				}
				else
				{
					_updatePlugInInfoTickCurrent = 0;
				}
			}
		}

		#region IPlugIn Members


        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

        public override object GetData(int userId, string dataKey, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		#endregion
	}
	#endregion

	#region UpdateDisk
	/// <summary>
	/// 更新磁盘信息
	/// </summary>
	/// <author>徐晓刚</author>
	public class UpdateDisk : BasePlugIn
	{
		public const string PlugInGuid = "{c29ed7be-8d7f-4765-83fe-d2a8b581f446}";

		int _updateDiskInfoTickCurrent;

		int _updateDiskInfoTickDo;

		public override object[] Parameters
        {
            set
            {
                _updateDiskInfoTickDo = int.Parse(value[1].ToString());
            }
            get
            {
                return new object[]{
                    1,
                    _updateDiskInfoTickDo
                };
            }
        }

		public override string[] ParaDescription
        {
            get
            {
                return new string[]{
                    DefaultParameter.UpDateTimeDes
                };
            }
        }

		public override string Name
		{
			get { return "UpdateDisk"; }
		}

		public override string Author
		{
			get { return "xuxiaogang"; }
		}

		public override string Guid
		{
			get { return PlugInGuid; }
		}

        public UpdateDisk()
        {
            _updateDiskInfoTickDo = DefaultParameter.UpdateDiskInfoTickDo;
        }

		public UpdateDisk(int updateInterval)
		{
			_updateDiskInfoTickDo = updateInterval;
		}

		public override void Initialize(GameServer server)
		{
			base.Initialize(server);

			_updateDiskInfoTickCurrent = 0;
			_server.Tick += new TickEventHandler(this.Tick);
			_server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
		}

		public override void Dispose()
		{
			_server.Tick -= new TickEventHandler(this.Tick);
			_server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
		}

		private void MessageReceived(GameServer server, IProtocol message)
		{
			switch ((ProtocolDef)message.ProtocolId)
			{
				case ProtocolDef.g2e_getdiskinfo_def:
					g2e_getdiskinfo protocol = message as g2e_getdiskinfo;
					diskbaseinfo[] infos = new diskbaseinfo[protocol.nDiskCount];
					Array.Copy(protocol.Disk, infos, infos.Length);
					server.UpdateDiskInfo(infos);
					break;
				default:
					break;
			}
		}

		private void Tick(GameServer server)
		{
			if (server != null)
			{
				if (server.IsConnected)
				{
					_updateDiskInfoTickCurrent++;
					if (_updateDiskInfoTickCurrent >= _updateDiskInfoTickDo)
					{
                        KProtocolHead protocol = new KProtocolHead();
                        protocol.ProtocolId = (ushort)ProtocolDef.e2g_getdiskinfo_def;
 						if (server.SendMessage(protocol))
						{
							_updateDiskInfoTickCurrent = 0;
						}
					}
				}
				else
				{
					_updateDiskInfoTickCurrent = 0;
				}
			}
		}

		#region IPlugIn Members


        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

        public override object GetData(int userId, string dataKey, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		#endregion
	}
	#endregion

	#region Ping
	/// <summary>
	/// Ping
	/// </summary>
	/// <author>徐晓刚</author>
	public class Ping : BasePlugIn
	{
		public const string PlugInGuid = "{02959648-ed95-4220-bef7-a8983da81f18}";

		DateTime _lastPing;

		int _pingTimeOutTick;

		int _checkPingTickCurrent;

		int _checkPingTickDo;

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
			get { return "Ping"; }
		}

		public override string Author
		{
			get { return "xuxiaogang"; }
		}

		public override string Guid
		{
			get { return PlugInGuid; }
		}

        public Ping()
        {
            _checkPingTickDo = DefaultParameter.CheckPingTickDo;
            _pingTimeOutTick = DefaultParameter.PingTimeOutTick;
        }

		public Ping(int updateInterval, int timeoutSeconds)
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

			_checkPingTickCurrent = 0;
			_lastPing = DateTime.MinValue;
			_server.Connected += new ConnectedEventHandler(Connected);
			_server.Tick += new TickEventHandler(this.Tick);
			_server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
		}

		public override void Dispose()
		{
			_server.Connected -= new ConnectedEventHandler(Connected);
			_server.Tick -= new TickEventHandler(this.Tick);
			_server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
		}

		private void DoPing(GameServer server)
		{
			KProtocolHead protocol = new KProtocolHead();
            protocol.ProtocolId = (ushort)ProtocolDef.e2g_ping_def;
			server.SendMessage(protocol);
		}

		private void Connected(GameServer server)
		{
			lock (this)
			{
				_checkPingTickCurrent = 0;
				_lastPing = DateTime.MinValue;
			}
			DoPing(server);
		}

		private void MessageReceived(GameServer server, IProtocol message)
		{
            try
            {
                if (message.ToBytes().Length >= 6)
                {
                    switch ((ProtocolDef)BitConverter.ToUInt16(message.ToBytes(), 4))
                    {
                        case ProtocolDef.l2e_ping_def://Ping
                            {
                                _lastPing = DateTime.Now;
                            }
                            break;
                    }
                }
            }
            catch (Exception ex)
            {
                Util.DebugLog(string.Format("Exception in Server[{0}] PlugIns.cs OnMessageReceived : {1}", this.Name, ex.StackTrace));
            }
			
		}

		private void Tick(GameServer server)
		{
			if (server != null)
			{
				lock (server)
				{
					lock (this)
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

								DoPing(server);

                                if (server.GameServiceState != GameServer.ServiceState.ErrorStopped)
                                {
                                    //如果服务器正在启动，则重置一次_lastPing，以免被认为是启动失败
                                    if (server.GameServiceState == GameServer.ServiceState.Starting)
                                    {
                                        _lastPing = DateTime.Now;
                                    }

                                    if (((TimeSpan)(DateTime.Now - _lastPing)).TotalSeconds > _pingTimeOutTick)
                                    {
                                        if (server.GameServiceState == GameServer.ServiceState.Stopping || server.GameServiceState == GameServer.ServiceState.OKStopped)
                                        {
                                            if (server.PID > 0 || server.PID == -2)
                                            {
                                                string[] para = new string[1] { server.Id.ToString() };
                                                server.DoPlugInAction(1, E2gServerPID.PlugInGuid, E2gServerPID.actionKey, para);
                                            }
                                            else
                                                server.GameServiceState = GameServer.ServiceState.OKStopped;
                                        }
                                        else
                                        {
                                            server.GameServiceState = GameServer.ServiceState.ErrorStopped;
                                        }
                                       // server.Disconnect();
                                        _lastPing = DateTime.MinValue;
                                    }

                                }								
							}
						}
						else
						{
							_checkPingTickCurrent = 0;
							_lastPing = DateTime.MinValue;
						}
					}
				}
			}
		}

		#region IPlugIn Members


        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

        public override object GetData(int userId, string dataKey, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		#endregion
	}
	#endregion

	#region FileManage
	/// <summary>
	/// 文件管理
	/// </summary>
	/// <author>徐晓刚</author>
	public class FileManage : BasePlugIn
	{
		public const string PlugInGuid = "{f6038d00-c898-4f98-b143-bb6edc51ae17}";

		public override object[] Parameters
        {
            set {}
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
			get { return "FileManage"; }
		}

		public override string Author
		{
			get { return "huangbiao@2008"; }
		}

		public override string Guid
		{
			get { return PlugInGuid; }
		}

		public FileManage()
		{
		}

		public override void Initialize(GameServer server)
		{
			base.Initialize(server);

			_server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
		}

		public override void Dispose()
		{
			_server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
		}

		private void MessageReceived(GameServer server, IProtocol message)
		{
			switch ((ProtocolDef)message.ProtocolId)
			{
				case ProtocolDef.g2e_openfile_def:
				case ProtocolDef.g2e_seekfile_def:
				case ProtocolDef.g2e_writefile_def:
				case ProtocolDef.g2e_closefile_def:		
                case ProtocolDef.g2e_readfile_def:
					if (server.IsConnected)
					{
						IList fmTasks = server.FMTask;
						if (fmTasks.Count > 0)
						{
							FileManageTask task = (fmTasks[0] as FileManageTask);
							if (task.State == FileManageTask.TaskState.Processing || task.State == FileManageTask.TaskState.Aborting)
							{
								bool success = task.DoTask(server, message);
								if (success)
								{
									if (task.State == FileManageTask.TaskState.Completed || task.State == FileManageTask.TaskState.Stopped)
									{
										server.RemoveFMTask(0);
									}
								}
								else
								{
									server.RemoveFMTask(0);
								}
							}
						}
					}
					break;
				default:
					break;
			}
		}

		#region IPlugIn Members


        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

        public override object GetData(int userId, string dataKey, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		#endregion
	}
	#endregion

	#region MessageLog
	/// <summary>
	/// 消息日志，用于调试
	/// </summary>
	public class MessageLog : BasePlugIn
	{
		public const string PlugInGuid = "{ccdc26d6-9ca5-4f3f-84f1-4e5fa0493956}";

		StreamWriter _writer;

        string _fileName;

		bool _disposed;

        public string FileName
        {
            set { _fileName = value; }
            get { return _fileName; }
        }

		public override object[] Parameters
        {
            set { _fileName = value[1] as string; }
            get
            {
                return new object[]{

                    1,
                    _fileName
                };
            }
        }

		public override string[] ParaDescription
        {
            get
            {
                return new string[]{
                DefaultParameter.FilePathDes
            };
            }
        }

		public override string Name
		{
			get { return "MessageLog"; }
		}

		public override string Author
		{
			get { return "xuxiaogang"; }
		}

		public override string Guid
		{
			get { return PlugInGuid; }
		}

        public MessageLog()
        { 
            _disposed = false;
            _fileName = DefaultParameter.LogFile;
            //做一点修改否则会一直占用默认文件
            //_writer = new StreamWriter(new FileStream(_fileName, FileMode.Create));
        }

		public MessageLog(string logFile)
		{
			_disposed = false;
            _fileName = logFile;
			_writer = new StreamWriter(new FileStream(_fileName, FileMode.Create));
		}

		public override void Initialize(GameServer server)
		{
			base.Initialize(server);

			_server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
			_server.MessageSent += new MessageSentEventHandler(this.MessageSent);
			_server.Connected += new ConnectedEventHandler(this.Connected);
			_server.Disconnected += new DisconnectedEventHandler(this.Disconnected);
		}

		public override void Dispose()
		{
			lock (this)
			{
				if (!_disposed)
				{
					_server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
					_server.MessageSent -= new MessageSentEventHandler(this.MessageSent);
					_server.Connected -= new ConnectedEventHandler(this.Connected);
					_server.Disconnected -= new DisconnectedEventHandler(this.Disconnected);

					try
					{
						if (_writer != null)
						{
							_writer.Close();
						}
					}
					catch (Exception)
					{
					}
					finally
					{
						_writer = null;
					}

					_disposed = true;
				}
			}
		}

		private void MessageReceived(GameServer server, IProtocol message)
		{
			_writer.Write(DateTime.Now);
			_writer.Write(":(Receive) ");
			_writer.WriteLine((ProtocolDef)message.ProtocolId);
			_writer.Flush();
		}

		private void MessageSent(GameServer server, IProtocol message)
		{
			_writer.Write(DateTime.Now);
			_writer.Write(":(Send) ");
			_writer.WriteLine((ProtocolDef)message.ProtocolId);
			_writer.Flush();
		}

		private void Connected(GameServer server)
		{
			_writer.Write(DateTime.Now);
			_writer.Write(":<--- ");
			_writer.WriteLine("Connected from " + server.IpAddress + ":" + server.Port);
			_writer.Flush();
		}

		private void Disconnected(GameServer server)
		{
			_writer.Write(DateTime.Now);
			_writer.Write(":---> ");
			_writer.WriteLine("Disconnected from " + server.IpAddress + ":" + server.Port);
			_writer.Flush();
		}

		#region IPlugIn Members


		public override bool DoAction(int userId,string actionKey, ref object outArg, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

        public override object GetData(int userId, string dataKey, params object[] args)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		#endregion
	}
	#endregion    

    #region E2gSendfile
    /// <summary>
    /// Sendfile
    /// </summary>
    /// <author>huangbiao</author>
    public class E2gSendfile : BasePlugIn
    {
        public const string PlugInGuid = "{02959648-ed95-4220-bef7-a8989de1f18}";
        public const string actionKey = "WebSendFileToGuard";

        public string _fileName;
        public string _targetPath;

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
            get { return "e2gSendFile"; }
        }

        public override string Author
        {
            get { return "huangbiao"; }
        }

        public override string Guid
        {
            get { return PlugInGuid; }
        }

        public E2gSendfile()
        {   
        }

        public override void Initialize(GameServer server)
        {
            base.Initialize(server);
            _server.MessageSent += new MessageSentEventHandler(this.MessageSent);
        }

        public override void Dispose()
        {
            _server.MessageSent -= new MessageSentEventHandler(this.MessageSent);         
        }

        private void MessageSent(GameServer server, IProtocol message)
        {
        }

        #region IPlugIn Members

        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
        {
            _targetPath = args[2].ToString();   
            e2g_file protocol = new e2g_file();
            protocol.szData = args[1].ToString();
            protocol.wFileLen = (ushort)Encoding.Default.GetBytes(protocol.szData).Length;
            protocol.szPath = _targetPath;
            protocol.ServerID = UInt16.Parse(args[0].ToString());
            return _server.SendMessage(protocol); 
        }

        public override object GetData(int userId, string dataKey, params object[] args)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion
    }
    #endregion   

    #region E2gUpdateserver
    /// <summary>
    /// auto update server
    /// </summary>
    /// <author>huangbiao</author>
    public class E2gUpdateserver : BasePlugIn
    {
        public const string PlugInGuid = "{02259648-ed95-4f20-def7-a898ce13180}";
        public const string actionKey = "AutoUpdateServer";

        public string _fileName;
        public string _targetPath;

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
            get { return "E2gUpdateserver"; }
        }

        public override string Author
        {
            get { return "huangbiao"; }
        }

        public override string Guid
        {
            get { return PlugInGuid; }
        }

        public E2gUpdateserver()
        {
        }

        public override void Initialize(GameServer server)
        {
            base.Initialize(server);
            _server.MessageSent += new MessageSentEventHandler(this.MessageSent);
            _server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
        }

        public override void Dispose()
        {
            _server.MessageSent -= new MessageSentEventHandler(this.MessageSent);
            _server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
        }

        private void MessageSent(GameServer server, IProtocol message)
        {
        }

        #region IPlugIn Members

        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
        {
            e2g_updateserver protocol = new e2g_updateserver();
            protocol.m_strPack = args[0].ToString();
            protocol.m_strMD5 = args[1].ToString();
            byte[] bytes = BitConverter.GetBytes(int.Parse(args[2].ToString()));
            protocol.m_nCheck = bytes[0];
            return _server.SendMessage(protocol);
        }

        public override object GetData(int userId, string dataKey, params object[] args)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        private void MessageReceived(GameServer server, IProtocol message)
        {
            switch ((ProtocolDef)message.ProtocolId)
            {
                case ProtocolDef.g2e_updateserver_def:
                    {
                        g2e_updateserver protocol = message as g2e_updateserver;
                        string[] strMsg = new string[5];
                        strMsg[0] = protocol.m_nCurrentStep.ToString();
                        strMsg[1] = protocol.m_nStepCount.ToString();
                        strMsg[2] = protocol.m_nResult.ToString();
                        strMsg[3] = protocol.m_strMessage;
                        strMsg[4] = DateTime.Now.ToString();
                        //相同IP的gameserver
                        IList servers = AdminServer.TheInstance.GameServerManager.Servers;
                        foreach (GameServer svr in servers)
                        {
                            if (svr.IpAddress == server.IpAddress)
                            {
                                bool bFindFlag = false;
                                foreach (string[] str in svr.UpdateMsg)
                                {
                                    if (str[0] == strMsg[0] && str[1] == strMsg[1] && str[2] == strMsg[2] && str[3] == strMsg[3])
                                    {
                                        bFindFlag = true;
                                        break;
                                    }
                                }
                                if (!bFindFlag) svr.UpdateMsg.Add(strMsg);
                            }
                        }
                    }
                    break;            
                default:
                    break;
            }
        }

        #endregion
    }
    #endregion  

    #region E2gDownload
    /// <summary>
    /// download packet to guard
    /// </summary>
    /// <author>huangbiao</author>
    public class E2gDownload : BasePlugIn
    {
        public const string PlugInGuid = "{02259648-ed95-1118-def7-a898ce13180}";
        public const string actionKey = "GuardDownloadPacket";

        public string _fileName;
        public string _targetPath;

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
            get { return "E2gDownload"; }
        }

        public override string Author
        {
            get { return "huangbiao"; }
        }

        public override string Guid
        {
            get { return PlugInGuid; }
        }

        public E2gDownload()
        {
        }

        public override void Initialize(GameServer server)
        {
            base.Initialize(server);
            _server.MessageSent += new MessageSentEventHandler(this.MessageSent);
            _server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
        }

        public override void Dispose()
        {
            _server.MessageSent -= new MessageSentEventHandler(this.MessageSent);
            _server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
        }

        private void MessageSent(GameServer server, IProtocol message)
        {
        }

        #region IPlugIn Members

        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
        {
            e2g_ftpdownload protocol = new e2g_ftpdownload();
            protocol.m_strFtpHost = args[0].ToString();
            protocol.m_strFtpUser = args[1].ToString();
            protocol.m_strFtpPass = args[2].ToString();
            protocol.m_strFtpPath = args[3].ToString();
            return _server.SendMessage(protocol);
        }

        public override object GetData(int userId, string dataKey, params object[] args)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        private void MessageReceived(GameServer server, IProtocol message)
        {
            switch ((ProtocolDef)message.ProtocolId)
            {
                case ProtocolDef.g2e_ftpdownload_def:
                    {
                        g2e_ftpdownload protocol = message as g2e_ftpdownload;

                        server.DownloadMsg[0] = protocol.m_nProgress.ToString();
                        server.DownloadMsg[1] = protocol.m_nResult.ToString();
                        server.DownloadMsg[2] = protocol.m_strMessage.ToString();
                        ////相同IP的gameserver
                        //IList servers = AdminServer.TheInstance.GameServerManager.Servers;
                        //foreach (GameServer svr in servers)
                        //{
                        //    if (svr.IpAddress == server.IpAddress)
                        //    {
                        //        svr.DownloadMsg[0] = protocol.m_nProgress.ToString();
                        //        svr.DownloadMsg[1] = protocol.m_nResult.ToString();
                        //        svr.DownloadMsg[2] = protocol.m_strMessage.ToString();
                        //    }
                        //}
                    }
                    break;
                default:
                    break;
            }
        }

        #endregion
    }
    #endregion  

    #region E2gServerid
    /// <summary>
    /// download packet to guard
    /// </summary>
    /// <author>huangbiao</author>
    public class E2gServerid : BasePlugIn
    {
        public const string PlugInGuid = "{11189648-ed95-1118-def7-a898ce13180}";
        public const string actionKey = "SendServerID2Guard";
       
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
            get { return "E2gServerid"; }
        }

        public override string Author
        {
            get { return "huangbiao"; }
        }

        public override string Guid
        {
            get { return PlugInGuid; }
        }

        public E2gServerid()
        {
        }

        public override void Initialize(GameServer server)
        {
            base.Initialize(server);
            _server.MessageSent += new MessageSentEventHandler(this.MessageSent);
        }

        public override void Dispose()
        {
            _server.MessageSent -= new MessageSentEventHandler(this.MessageSent);
        }

        private void MessageSent(GameServer server, IProtocol message)
        {
        }

        #region IPlugIn Members

        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
        {
            e2g_serverid protocol = new e2g_serverid();
            protocol.nGoddess       = int.Parse(args[0].ToString());
            protocol.nBishop        = int.Parse(args[1].ToString());
            protocol.nGameCenter    = int.Parse(args[2].ToString());
            protocol.nLogServer     = int.Parse(args[3].ToString());
            protocol.nGameServer01  = int.Parse(args[4].ToString());;
            protocol.nGameServer02  = int.Parse(args[5].ToString());
            protocol.nGameServer03  = int.Parse(args[6].ToString());
            protocol.nGameServer04  = int.Parse(args[7].ToString());
            protocol.nGameServer05  = int.Parse(args[8].ToString());
            protocol.nGameServer06  = int.Parse(args[9].ToString());
            protocol.nGameServer07  = int.Parse(args[10].ToString());
            protocol.nGameServer08  = int.Parse(args[11].ToString());
            protocol.nGameServer09  = int.Parse(args[12].ToString());
            protocol.nGameServer10  = int.Parse(args[13].ToString());
            protocol.nGameServer11  = int.Parse(args[14].ToString());
            protocol.nGameServer12  = int.Parse(args[15].ToString());
            return _server.SendMessage(protocol);
        }

        public override object GetData(int userId, string dataKey, params object[] args)
        {
            throw new Exception("The method or operation is not implemented.");
        }        

        #endregion
    }
    #endregion  

    #region E2gServerPID
    /// <summary>
    /// get PID 
    /// </summary>
    /// <author>huangbiao</author>
    public class E2gServerPID : BasePlugIn
    {
        public const string PlugInGuid = "{111lm648-ed95-1o18-def7-a8hjce13180}";
        public const string actionKey = "GetServerPID";

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
            get { return "E2gServerPID"; }
        }

        public override string Author
        {
            get { return "huangbiao"; }
        }

        public override string Guid
        {
            get { return PlugInGuid; }
        }

        public E2gServerPID()
        {
        }

        public override void Initialize(GameServer server)
        {
            base.Initialize(server);
            _server.MessageReceived += new MessageReceivedEventHandler(this.MessageReceived);
        }

        public override void Dispose()
        {
            _server.MessageSent -= new MessageSentEventHandler(this.MessageSent);
            _server.MessageReceived -= new MessageReceivedEventHandler(this.MessageReceived);
        }

        private void MessageSent(GameServer server, IProtocol message)
        {
        }

        #region IPlugIn Members

        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
        {
            e2g_server_pid protocol = new e2g_server_pid();
            protocol.ServerID = UInt16.Parse(args[0].ToString());
            return _server.SendMessage(protocol);
        }

        public override object GetData(int userId, string dataKey, params object[] args)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        private void MessageReceived(GameServer server, IProtocol message)
        {
            switch ((ProtocolDef)message.ProtocolId)
            {
                case ProtocolDef.g2e_serverpid_def:
                    {
                        g2e_server_pid protocol = message as g2e_server_pid;
                        server.PID = protocol.nRetCode;
                    }
                    break;
                default:
                    break;
            }
        }
        #endregion
    }
    #endregion  

#region ExitOrder
    /// <summary>
    /// ExitOrder
    /// </summary>
    /// <author>huangbiao</author>
    public class ExitOrder : BasePlugIn
    {
        public const string PlugInGuid = "{02959648-ed95-4220-bef7-a8853de1f18}";
        public const string actionKey = "WebSendExitOrderToServer";

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
            get { return "ExitOrder"; }
        }

        public override string Author
        {
            get { return "huangbiao"; }
        }

        public override string Guid
        {
            get { return PlugInGuid; }
        }

        public ExitOrder()
        {   
        }

        public override void Initialize(GameServer server)
        {
            base.Initialize(server);
            _server.MessageSent += new MessageSentEventHandler(this.MessageSent);
        }

        public override void Dispose()
        {
            _server.MessageSent -= new MessageSentEventHandler(this.MessageSent);         
        }

        private void MessageSent(GameServer server, IProtocol message)
        {
        }

        #region IPlugIn Members

        public override bool DoAction(int userId, string actionKey, ref object outArg, params object[] args)
        {
            KProtocolHead protocol = new KProtocolHead();
            protocol.ServerID = UInt16.Parse(args[0].ToString());
            protocol.ProtocolId = (UInt16)ProtocolDef.e2l_exit_def;
            return _server.SendMessage(protocol); 
        }

        public override object GetData(int userId, string dataKey, params object[] args)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion
    }
    #endregion   
}
