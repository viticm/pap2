//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-4
//      File_base        : Automations
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 自动化作业的具体实现类
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Web;
using System.Net;
using System.Net.Mail;
using System.Collections;
using FSEye.PlugIn;
using FSEye.Security;
using System.Drawing;
using ZedGraph;
using System.Data;
using FSEye.DAL;

namespace FSEye
{
	#region NullAutomation
	/// <summary>
	/// 空
	/// </summary>
	public class NullAutomation : Automation
	{
		public override string Guid
		{
			get { return "{49E6EA4F-ABE0-4e46-BCD2-23C5654929F4}"; }
		}

		public override string Name
		{
			get { return StringDef.NullAutomation; }
		}

		public override object[] Parameter
		{
            get { return new object[] { 0 }; }
			set { }
		}

        public override string[] ParameterDes
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public override string Description
        {
            get { return StringDef.NullAutomationDes; }
        }

		public override void Do(AutomationContext context)
		{
			_state = AutomationState.Done;
			_result = AutomationResult.Success;
		}
	}
	#endregion

	#region WaitAutomation
	/// <summary>
	/// 等待
	/// </summary>
	public class WaitAutomation : Automation
	{
		long _waitSeconds = 0;

		DateTime _waitFinishTime = DateTime.MinValue;

		public override string Guid
		{
			get { return "{AC1EF6F8-826C-4c3f-BA99-68F32BB92EA3}"; }
		}

		public override string Name
		{
            get { return StringDef.WaitAutomation; }
		}

        public override string Description
        {
            get { return StringDef.WaitAutomationDes; }
        }

        public override string[] ParameterDes
        {
            get
            {
                return new string[]{
                    StringDef.WaitTime
                };
            }
        }

		public override object[] Parameter
		{
			get
			{
                return new object[]{
                    1,
                    _waitSeconds
                    };
			}
			set
			{
				try
				{
                    if (value[1].GetType() == typeof(string)) _waitSeconds = long.Parse(value[1] as string);
                    else _waitSeconds = (long)value[1];
				}
				catch (Exception e)
				{
					throw new BadParameterException("The currect format of parameter should be: waitseconds", e);
				}
			}
		}

		public override void Do(AutomationContext context)
		{
			switch (_state)
			{
				case AutomationState.Init:
					{
						_waitFinishTime = DateTime.Now.AddSeconds(_waitSeconds);
						_state = AutomationState.Doing;
					}
					break;
				case AutomationState.Doing:
					{
						if (DateTime.Now >= _waitFinishTime)
						{
							_state = AutomationState.Done;
							_result = AutomationResult.Success;
						}
					}
					break;
			}
		}
	}
	#endregion

	#region SelectServerAutomation
	/// <summary>
	/// 选择服务器
	/// </summary>
	public class SelectServerAutomation : Automation
	{
        //const string ParameterAll = "all";

        //const string ParameterNone = "none";

        //const string ParameterServer = "s";

        //const string ParameterGroup = "g";

		const char ParameterDelimiter = ' ';

        //enum SelectType
        //{
        //    All,//选择所有
        //    None,//清除选择
        //    Custom//定制
        //}

        //SelectType _selectType = SelectType.Custom;

		IList<int> _serverIdList = new List<int>();

		IList<int> _groupIdList = new List<int>();

		public override string Guid
		{
			get { return "{ED479030-7008-4a2d-8800-F5F18328268B}"; }
		}

		public override string Name
		{
            get { return StringDef.SelectServerAutomation; }
		}

        public override string Description
        {
            get { return StringDef.SelectServerAutomation; }
        }

        public override string[] ParameterDes
        {
            get 
            {
                return new string[]{
                    StringDef.ServerId,
                    StringDef.ServerGroupId
                    };
            }
        }

        public override object[] Parameter
        {
            get
            {
                StringBuilder serverIdText = new StringBuilder();
                StringBuilder serverGroupIdText = new StringBuilder();

                if (_serverIdList.Count != 0)
                {
                    foreach (int serverId in _serverIdList)
                    {
                        serverIdText.Append(serverId);
                        serverIdText.Append(ParameterDelimiter);
                    }
                }
                if (_groupIdList.Count != 0)
                {
                    foreach (int gorupId in _groupIdList)
                    {
                        serverGroupIdText.Append(gorupId);
                        serverGroupIdText.Append(ParameterDelimiter);
                    }
                }
                string serverIds = serverIdText.Length == 0 ? string.Empty : serverIdText.ToString(0, serverIdText.Length - 1);
                string serverGroupIds = serverGroupIdText.Length == 0 ? string.Empty : serverGroupIdText.ToString(0, serverGroupIdText.Length - 1);
                return new object[]{
                    2,
                    serverIds,
                    serverGroupIds
                };
            }

            set
            {
                try
                {
                    StringBuilder errorMsg = new StringBuilder();
                    _serverIdList.Clear();
                    _groupIdList.Clear();

                    if (value.Length > 1)
                    {
                        string originalValue = value[1] as string;
                        if(originalValue!=null)
                        {
                            string[] serverArray = originalValue.Split(new char[] { ParameterDelimiter },StringSplitOptions.RemoveEmptyEntries);
                            if (serverArray.Length != 0)
                            {
                                foreach (string serverIdText in serverArray)
                                {
                                    int serverId = int.Parse(serverIdText);
                                    if (AdminServer.TheInstance.GameServerManager.GetGameServer(serverId) != null)
                                        _serverIdList.Add(serverId);
                                    else errorMsg.AppendFormat("ID {0} is not a server's id ", serverId);
                                }
                            }
                        }
                    }

                    if (value.Length > 2)
                    {
                        string[] serverGroupArray = (value[2] as string).Split(new char[] { ParameterDelimiter }, StringSplitOptions.RemoveEmptyEntries);
                        if (serverGroupArray.Length != 0)
                        {
                            foreach (string serverGroupIdText in serverGroupArray)
                            {
                                int groupId = int.Parse(serverGroupIdText);
                                if (AdminServer.TheInstance.GameServerManager.GetGameServerGroup(groupId) != null)
                                    _groupIdList.Add(groupId);
                                else errorMsg.AppendFormat("ID {0} is not a server group's id ", groupId);
                            }
                        }
                    }

                    if (_serverIdList.Count == 0 && _groupIdList.Count == 0) errorMsg.Append("At least one server's id or server group's id.");
                    if (errorMsg.Length != 0) throw new Exception(errorMsg.ToString());
                }
                catch (Exception e)
                {
                    throw new BadParameterException(e.Message,e);
                }
            }
        }

        //public override string Parameter
        //{
        //    get
        //    {
        //        StringBuilder text = new StringBuilder();

        //        if (_selectType == SelectType.All)
        //        {
        //            text.Append(ParameterAll);
        //        }
        //        else if (_selectType == SelectType.None)
        //        {
        //            text.Append(ParameterNone);
        //        }
        //        else if (_selectType == SelectType.Custom)
        //        {
        //            if (_serverId > 0)
        //            {
        //                text.Append(ParameterServer);
        //                text.Append(_serverId);
        //                text.Append(ParameterDelimiter);
        //            }
        //            else
        //            {
        //                foreach (int serverId in _serverIdList)
        //                {
        //                    text.Append(ParameterServer);
        //                    text.Append(serverId);
        //                    text.Append(ParameterDelimiter);
        //                }
        //                foreach (int groupId in _groupIdList)
        //                {
        //                    text.Append(ParameterGroup);
        //                    text.Append(groupId);
        //                    text.Append(ParameterDelimiter);
        //                }
        //            }

        //            if (text.Length > 0)
        //            {
        //                text.Remove(text.Length - ParameterDelimiter.Length, ParameterDelimiter.Length);
        //            }
        //        }

        //        return text.ToString();
        //    }
        //    set
        //    {
        //        try
        //        {
        //            _serverIdList.Clear();
        //            _groupIdList.Clear();

        //            if (value == null || value == ParameterAll)
        //            {
        //                _selectType = SelectType.All;
        //            }
        //            else if (value == ParameterNone)
        //            {
        //                _selectType = SelectType.None;
        //            }
        //            else
        //            {
        //                _selectType = SelectType.Custom;

        //                string[] parameterArray = value.Split(new string[] { ParameterDelimiter }, StringSplitOptions.None);
        //                if (parameterArray.Length == 1 && parameterArray[0].StartsWith(ParameterServer))
        //                {
        //                    _serverId = int.Parse(parameterArray[0].Substring(ParameterServer.Length));
        //                }
        //                else
        //                {
        //                    foreach (string parameter in parameterArray)
        //                    {
        //                        if (parameter.StartsWith(ParameterServer))
        //                        {
        //                            int serverId = int.Parse(parameter.Substring(ParameterServer.Length));
        //                            _serverIdList.Add(serverId);
        //                        }
        //                        else if (parameter.StartsWith(ParameterGroup))
        //                        {
        //                            int groupId = int.Parse(parameter.Substring(ParameterGroup.Length));
        //                            _groupIdList.Add(groupId);
        //                        }
        //                    }
        //                }
        //            }
        //        }
        //        catch (Exception e)
        //        {
        //            throw new BadParameterException("The currect format of parameter should be: s# s# g# g#", e);
        //        }
        //    }
        //}

		public override void Do(AutomationContext context)
		{
			GameServerManager gsm = AdminServer.TheInstance.GameServerManager;

            if (_serverIdList != null && _serverIdList.Count != 0)
            {
                foreach (int serverId in _serverIdList)
                {
                    GameServer server = gsm.GetGameServer(serverId);
                    if (server != null)
                    {
                        context.ServerList.Add(server);
                    }
                }
            }
            if (_groupIdList != null && _groupIdList.Count != 0)
            {
                foreach (int groupId in _groupIdList)
                {
                    ServerGroup group = gsm.GetGameServerGroup(groupId);
                    if (group != null)
                    {
                        context.ServerGroupList.Add(group);
                    }
                }
            }

            //switch (_selectType)
            //{
            //    case SelectType.Custom:
            //        {
            //            if (_serverId > 0)
            //            {
            //                GameServer server = gsm.GetGameServer(_serverId);
            //                if (server != null)
            //                {
            //                    context.Server = server;
            //                }
            //            }

            //            foreach (int serverId in _serverIdList)
            //            {
            //                GameServer server = gsm.GetGameServer(serverId);
            //                if (server != null)
            //                {
            //                    context.ServerList.Add(server);
            //                }
            //            }

            //            foreach (int groupId in _groupIdList)
            //            {
            //                ServerGroup group = gsm.GetGameServerGroup(groupId);
            //                if (group != null)
            //                {
            //                    context.ServerGroupList.Add(group);
            //                }
            //            }
            //        }
            //        break;
            //    case SelectType.All:
            //        {
            //            foreach (GameServer server in gsm.Servers)
            //            {
            //                context.ServerList.Add(server);
            //            }
            //        }
            //        break;
            //    case SelectType.None:
            //        {
            //            context.ClearAllServers();
            //        }
            //        break;
            //}
			
			_state = AutomationState.Done;
			_result = AutomationResult.Success;
		}
	}
	#endregion

	#region CheckServerAutomation
	/// <summary>
	/// 检查服务器
	/// </summary>
	public class CheckServerAutomation : Automation
	{
		const string CheckTypeConnected = "connected";

		const string CheckTypeDisconnected = "disconnected";

		const string CheckTypeGameRunning = "gamerunning";

		const string CheckTypeGameNotRunning = "gamenotrunning";

		const string CheckTypeGameStopped = "gamestopped";

		string _checkType;

		IList<int> _serverIdList = new List<int>();

		IList<int> _groupIdList = new List<int>();

		public override string Guid
		{
			get { return "{62EE6628-C72B-4582-89B5-52069F2AAAF3}"; }
		}

		public override string Name
		{
            get { return StringDef.CheckServerAutomation; }
		}

        public override string[] ParameterDes
        {
            get
            {
                return new string[]{
                    StringDef.CheckType
                };
            }
        }

        public override string Description
        {
            get { return StringDef.CheckServerAutomation; }
        }

        public override object[] Parameter
        {
            get
            {
                return new object[]{
                    1,
                    _checkType};
            }
            set
            {
                if (value[1].ToString().Equals(CheckTypeConnected, StringComparison.OrdinalIgnoreCase) ||
                    value[1].ToString().Equals(CheckTypeDisconnected, StringComparison.OrdinalIgnoreCase) ||
                    value[1].ToString().Equals(CheckTypeGameRunning, StringComparison.OrdinalIgnoreCase) ||
                    value[1].ToString().Equals(CheckTypeGameNotRunning, StringComparison.OrdinalIgnoreCase) ||
                    value[1].ToString().Equals(CheckTypeGameStopped, StringComparison.OrdinalIgnoreCase))
                {
                    _checkType = value[1].ToString().ToLowerInvariant();
                }
                else
                {
                    throw new BadParameterException("Parameter is one of these strings:connected,disconnected,gamerunning,gamenotrunning & gamestopped.", new Exception());
                }
            }
        }

		public override void Do(AutomationContext context)
		{
			GameServerManager gsm = AdminServer.TheInstance.GameServerManager;

			IList<GameServer> checkSuccessServerList = new List<GameServer>();
			IList<GameServer> allServerList = context.GetAllServers();

			foreach (GameServer server in allServerList)
			{
				switch (_checkType)
				{
					case CheckTypeConnected:
						{
							if (server.IsConnected)
							{
								checkSuccessServerList.Add(server);
							}
						}
						break;
					case CheckTypeDisconnected:
						{
							if (!server.IsConnected)
							{
								checkSuccessServerList.Add(server);
							}
						}
						break;
					case CheckTypeGameRunning:
						{
							if (server.GameServiceState == GameServer.ServiceState.Running)
							{
								checkSuccessServerList.Add(server);
							}
						}
						break;
					case CheckTypeGameNotRunning:
						{
							if (server.GameServiceState != GameServer.ServiceState.Running)
							{
								checkSuccessServerList.Add(server);
							}
						}
						break;
					case CheckTypeGameStopped:
						{
							if (server.GameServiceState == GameServer.ServiceState.ErrorStopped)
							{
								checkSuccessServerList.Add(server);
							}
						}						
						break;
				}
			}

			context.ClearAllServers();

			foreach (GameServer server in checkSuccessServerList)
			{
				context.ServerList.Add(server);
			}

			_state = AutomationState.Done;
			_result = AutomationResult.Success;
		}
	}
	#endregion

	#region StartGameAutomation
	/// <summary>
	/// 启动游戏
	/// </summary>
	public class StartGameAutomation : AbstractExeSysCmdAutomation
	{
		public override string Guid
		{
			get { return "{810A30F0-B0C8-4e73-86F0-F32C683FE626}"; }
		}

		public override string Name
		{
            get { return StringDef.StartGameAutomation; }
		}

        public override string Description
        {
            get { return StringDef.StartGameAutomationDes; }
        }

		public override object[] Parameter
		{
			get { 
                return new object[]{ 0 };
            }
			set { }
		}

        public override string[] ParameterDes
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

		protected override bool CreateScript(GameServer server, out string script, out string input)
		{
			if (server.GameServiceState == GameServer.ServiceState.ErrorStopped)
			{
				script = (string)server.GetPlugInData(0, GameServerControl.PlugInGuid, GameServerControl.DataKeyStartGameScript);
				input = (string)server.GetPlugInData(0, GameServerControl.PlugInGuid, GameServerControl.DataKeyStartGameInput);

				return (script != null);
			}
			else
			{
				script = null;
				input = null;

				return false;
			}
		}

		protected override void OnCommandComplete(GameServer server, bool success, string output)
		{
			if (success && (output == null || output.Length == 0))
			{
				server.GameServiceState = GameServer.ServiceState.Starting;
			}
			else
			{
				server.AppendMessage(StringDef.StartGameFailed + ":" + output, GameServer.MessageInfo.Type.Failure);
			}
		}
	}
	#endregion

	#region StopGameAutomation
	/// <summary>
	/// 关闭游戏
	/// </summary>
	public class StopGameAutomation : AbstractExeSysCmdAutomation
	{
		int _delay = 0;

		string _msg = string.Empty;

		public override string Guid
		{
			get { return "{A4102EA0-F1EB-4813-9C86-E06419D33013}"; }
		}

		public override string Name
		{
            get { return StringDef.StopGameAutomation; }
		}

        public override string Description
        {
            get { return StringDef.StopGameAutomationDes; }
        }

        public override string[] ParameterDes
        {
            get
            {
                return new string[]{
                    StringDef.Delay,
                    StringDef.Message 
                };
            }
        }

		public override object[] Parameter
		{
			get
			{
                return new object[]{
                        2,
                        _delay,
                        _msg};
			
			}
			set
			{
				try
				{
					if (value != null && value.Length > 0)
					{
                        if (value[1].GetType() == typeof(string)) _delay = int.Parse(value[1] as string);
                        else _delay = (int)value[1];
						_msg = value[2] as string;
					}
				}
				catch (Exception e)
				{
					throw new BadParameterException("The currect format of parameter should be: delay msg", e);
				}
			}
		}

		protected override bool CreateScript(GameServer server, out string script, out string input)
		{
			if (server.GameServiceState == GameServer.ServiceState.Running)
			{
				script = (string)server.GetPlugInData(0, GameServerControl.PlugInGuid, GameServerControl.DataKeyStopGameScript);
				input = null;

				return (script != null);
			}
			else
			{
				script = null;
				input = null;

				return false;
			}
		}

		protected override void OnCommandComplete(GameServer server, bool success, string output)
		{
			if (success && (output == null || output.Length == 0))
			{
				server.GameServiceState = GameServer.ServiceState.Stopping;
			}
			else
			{
				server.AppendMessage(StringDef.StopGameFailed + ":" + output, GameServer.MessageInfo.Type.Failure);
			}
		}
	}
	#endregion

	#region UpdateGameAutomation
	/// <summary>
	/// 更新游戏
	/// </summary>
	public class UpdateGameAutomation : Automation
	{
		string _patchName = string.Empty;

		public override string Guid
		{
			get { return "{D6AAC06B-702A-49ab-864F-AAE77A834C62}"; }
		}

		public override string Name
		{
            get { return StringDef.UpdateGameAutomation; }
		}

        public override string Description
        {
            get { return StringDef.UpdateGameAutomationDes; }
        }

		public override object[] Parameter
		{
			get
			{
                return new object[]{
                     1,
                    _patchName};
			}
			set
			{
				_patchName = value[1] as string;				
			}
		}

        public override string[] ParameterDes
        {
            get
            {
                return new string[]{
                    StringDef.PatchName
                };
            }
        }

		public override void Do(AutomationContext context)
		{
			bool success = true;

			IList<GameServer> serverList = context.GetAllServers();
			foreach (GameServer server in serverList)
			{
				object outArg = null;
                //#########################################################
                if (!server.DoPlugInAction(SecurityManager.SystemAutomationId, GameServerControl.PlugInGuid, GameServerControl.ActionKeyUpdateGame, ref outArg, new object[] { _patchName }))
				{
					success = false;
				}
			}

			if (success)
				_result = AutomationResult.Success;
			else
				_result = AutomationResult.Failure;

			_state = AutomationState.Done;
		}
	}
	#endregion

    #region SendMailAutomation
    /// <summary>
    /// 发送邮件
    /// </summary>
    public class SendMailAutomation : Automation
    {
        const string MailServerName = "bjmail.kingsoft.com";
        const string SenderMail = "fs2service@kingsoft.com";
        const string User = "fs2service";
		const string Password = "fs2superFS@";
        string _receiverMail;
        string _subject;
        IList _receiverMailList;

        string _mailContentType;
        IList _mailContentTypeList;

        public override string Guid
        {
            get { return "{eca2ad62-0238-4996-82c0-852009463d94}"; }
        }

        public override string Name
        {
            get { return StringDef.SendMailAutomation; }
        }

        public override string Description
        {
            get { return StringDef.SendMailAutomationDes; }
        }

        public override object[] Parameter
        {
            get
            {
                return new object[]{
                    3,
                    _receiverMail,
                    _subject,
                    _mailContentType};
            }
            set
            {
                try
                {                    
                    _receiverMail = value[1] as string;
                    if (_receiverMail != null && _receiverMail.Length != 0)
                    {
                        _receiverMailList = new ArrayList();
                        foreach (string receiver in _receiverMail.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries))
                        {
                            _receiverMailList.Add(receiver);
                        }
                    }
                    _subject = value[2] as string;

                    _mailContentType = value[3] as string;
                    if (_mailContentType != null && _mailContentType.Length != 0)
                    {
                        _mailContentTypeList = new ArrayList();
                        foreach (string str in _mailContentType.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries))
                        {
                            _mailContentTypeList.Add(str);
                        }
                    }
                    else
                    {
                        if (_mailContentTypeList == null)
                            _mailContentTypeList = new ArrayList();
                    }

                }
                catch (Exception e)
                {
                    throw new BadParameterException("The Parameter's Format should be: EMailAddress,Subject", e);
                }
            }
        }

        public override string[] ParameterDes
        {
            get
            {
                return new string[]{
                    StringDef.Receiver,
                    StringDef.Subject,
                    StringDef.MailContent
                };
            }
        }

        public override void Do(AutomationContext context)
        {
            //switch (_state)
            //{
            //    case AutomationState.Init:
            //        if (_receiverMailList != null && _receiverMailList.Count != 0)
            //        {
            //            SmtpClient mailClient = new SmtpClient(MailServerName);
            //            mailClient.Credentials = new NetworkCredential(User, Password);

            //            //先设置为成功，如果有异常则设置为失败，但并不影响向其它Mail的发送。
            //            _result = AutomationResult.Success;
            //            foreach (string receiverMail in _receiverMailList)
            //            {
            //                try
            //                {
            //                    MailMessage message = new MailMessage(SenderMail, receiverMail, _subject, context.Message);
            //                    message.SubjectEncoding = SystemConfig.Current.DefaultEncoding;
            //                    message.BodyEncoding = SystemConfig.Current.DefaultEncoding;
            //                    mailClient.Send(message);
            //                }
            //                catch (Exception)
            //                {
            //                    _result = AutomationResult.Failure;
            //                }
            //            }
            //        }
            //        _state = AutomationState.Done;

            //        break;
            //}   

            try
            {
                switch (_state)
                {
                    case AutomationState.Init:
                        if (_receiverMailList != null && _receiverMailList.Count != 0)
                        {
                            SmtpClient mailClient = new SmtpClient(MailServerName);
                            mailClient.Credentials = new NetworkCredential(User, Password);

                            if (_mailContentTypeList.Count == 0)
                            {
                                //先设置为成功，如果有异常则设置为失败，但并不影响向其它Mail的发送。
                                _result = AutomationResult.Success;
                                foreach (string receiverMail in _receiverMailList)
                                {
                                    try
                                    {
                                        MailMessage message = new MailMessage(SenderMail, receiverMail, _subject, context.Message);
                                        message.SubjectEncoding = SystemConfig.Current.DefaultEncoding;
                                        message.BodyEncoding = SystemConfig.Current.DefaultEncoding;
                                        mailClient.Send(message);
                                    }
                                    catch (Exception ex)
                                    {
                                        _result = AutomationResult.Failure;
                                        throw ex;
                                    }
                                }
                            }
                            else
                            {
                                string tableStyle = "<style type='text/css'> table, td { border-color: black; border-style: solid; border-width: 1px; border-spacing: 0; border-collapse: collapse; padding: 4px; } </style>";

                                string contentString = tableStyle;
                                ArrayList attachmentLocations = new ArrayList();

                                foreach (string str in _mailContentTypeList)
                                {
                                    switch (str)
                                    {
                                        case "PlayerCountOneDayStyleText":
                                            {
                                                contentString += "<div style=\"font-family:Arial\">各区服24小时内在线人数统计(文字)<br /><br /></div>";

                                                contentString += GetPlayerCountTextOneDay();

                                                break;
                                            }

                                        case "PlayerCountOneDayStylePic":
                                            {
                                                contentString += "<div style=\"font-family:Arial\">各区服24小时内在线人数统计(图表)<br /><br /></div>";

                                                foreach (GameServer server in AdminServer.TheInstance.GameServerManager.Servers)
                                                {
                                                    if (server.Type == GameServer.ServerType.gamecenter)
                                                    {
                                                        string fileName = string.Empty;

                                                        MemoryStream imageMemoryStream = CreatePlayerCountImageOneDay(server, out fileName);

                                                        if (imageMemoryStream != null)
                                                        {
                                                            contentString += "<div style=\"font-family:Arial\">"
                                                                            + server.Group.Name + "<br /><img src=\"cid:"
                                                                            + fileName + "\" alt=\"IMAGE HERE\" width=\"840\" height=\"450\"><br /><br /></div>";

                                                            Attachment attachment = new Attachment(imageMemoryStream, fileName, System.Net.Mime.MediaTypeNames.Image.Jpeg);

                                                            attachmentLocations.Add(attachment);
                                                        }
                                                    }
                                                }
                                                break;
                                            }

                                        case "PlayerCountOneMonthStyleText":
                                            {
                                                contentString += "<div style=\"font-family:Arial\">各区服一月内在线人数统计(文字)<br /><br /></div>";

                                                contentString += GetPlayerCountTextOneMonth();

                                                break;
                                            }

                                        case "PlayerCountOneMonthStylePic":
                                            {
                                                contentString += "<div style=\"font-family:Arial\">各区服一月内在线人数统计(图表)<br /><br /></div>";

                                                foreach (GameServer server in AdminServer.TheInstance.GameServerManager.Servers)
                                                {
                                                    if (server.Type == GameServer.ServerType.gamecenter)
                                                    {
                                                        string fileName = string.Empty;

                                                        MemoryStream imageMemoryStream = CreatePlayerCountImageOneMonth(server, out fileName);

                                                        if (imageMemoryStream != null)
                                                        {
                                                            contentString += "<div style=\"font-family:Arial\">"
                                                                            + server.Group.Name + "<br /><img src=\"cid:"
                                                                            + fileName + "\" alt=\"IMAGE HERE\" width=\"840\" height=\"450\"><br /><br /></div>";

                                                            Attachment attachment = new Attachment(imageMemoryStream, fileName, System.Net.Mime.MediaTypeNames.Image.Jpeg);

                                                            attachmentLocations.Add(attachment);
                                                        }
                                                    }
                                                }
                                                break;
                                            }
                                        case "ServerEventOneDay":
                                            {
                                                string summaryContent = GetServerEventSummaryOneDay();

                                                contentString += summaryContent;

                                                break;
                                            }
                                    }
                                }

                                //先设置为成功，如果有异常则设置为失败，但并不影响向其它Mail的发送。
                                _result = AutomationResult.Success;
                                foreach (string receiverMail in _receiverMailList)
                                {
                                    try
                                    {
                                        MailMessage message = new MailMessage(SenderMail, receiverMail, _subject, contentString);
                                        message.SubjectEncoding = SystemConfig.Current.DefaultEncoding;
                                        message.BodyEncoding = SystemConfig.Current.DefaultEncoding;
                                        message.IsBodyHtml = true;

                                        if (attachmentLocations.Count > 0)
                                        {
                                            foreach (Attachment attachment in attachmentLocations)
                                            {
                                                message.Attachments.Add(attachment);
                                            }
                                        }

                                        mailClient.Send(message);
                                    }
                                    catch (Exception ex)
                                    {
                                        _result = AutomationResult.Failure;
                                        throw ex;
                                    }
                                }
                            }
                        }
                        _result = AutomationResult.Success;
                        _state = AutomationState.Done;                    

                        break;
                }  
            }
            catch (Exception ex)
            {
                Util.DebugLog(ex.ToString());

                _state = AutomationState.Done;
                _result = AutomationResult.Failure;
            }
        }

        private string GetPlayerCountTextOneDay()
        {
            string contentString = String.Empty;

            foreach (GameServer server in AdminServer.TheInstance.GameServerManager.Servers)
            {
                if (server.Type == GameServer.ServerType.gamecenter)
                {
                    contentString += GetPlayerCountTextOneDayOneServer(server);
                }
            }

            return contentString;
        }

        private string GetPlayerCountTextOneDayOneServer(GameServer server)
        {
            LordControl.PlayerCountStatisticInfo[] _infoArray = null;
            DateTime _endTime = DateTime.Now;
            DateTime _startTime = _endTime.AddDays(-1);

            string contentString = String.Empty;

            _infoArray = server.GetPlugInData(0, LordControl.PlugInGuid, LordControl.DataKeyPlayerCountStatistic, _startTime, _endTime) as LordControl.PlayerCountStatisticInfo[];
            
            if (_infoArray.Length != 0)
            {
                string groupName = server.Group.Name;
                contentString += "<div style=\"font-family:Arial\">" + groupName + "</div>";

                contentString += "<table>";

                string timeString = "<tr><td>时间</td>";
                string maxString = "<tr><td>最大</td>";
                string aveString = "<tr><td>平均</td>";
                string minString = "<tr><td>最小</td>";

                foreach (LordControl.PlayerCountStatisticInfo info in _infoArray)
                {
                    timeString += "<td>" + info.Time.ToString("HH:mm") + "</td>";
                    maxString += "<td>" + info.MaxCount.ToString() + "</td>";
                    aveString += "<td>" + info.AverageCount.ToString() + "</td>";
                    minString += "<td>" + info.MinCount.ToString() + "</td>";
                }

                timeString += "</tr>";
                maxString += "</tr>";
                aveString += "</tr>";
                minString += "</tr>";

                contentString += timeString;
                contentString += maxString;
                contentString += aveString;
                contentString += minString;

                contentString += "</table>";

                contentString += "<br />";
            }

            return contentString;
        }

        private MemoryStream CreatePlayerCountImageOneDay(GameServer server, out string fileName)
        {            
            LordControl.PlayerCountStatisticInfo[] _infoArray = null;
            DateTime _endTime = DateTime.Now;
            DateTime _startTime = _endTime.AddDays(-1);

            _infoArray = server.GetPlugInData(0, LordControl.PlugInGuid, LordControl.DataKeyPlayerCountStatistic, _startTime, _endTime) as LordControl.PlayerCountStatisticInfo[];

            if (_infoArray.Length != 0)
            {
                double[] maxCountArray = new double[_infoArray.Length];
                double[] minCountArray = new double[_infoArray.Length];
                double[] averageCountArray = new double[_infoArray.Length];
                double[] timeArray = new double[_infoArray.Length];

                for (int i = 0; i < _infoArray.Length; i++)
                {
                    LordControl.PlayerCountStatisticInfo info = _infoArray[i];
                    maxCountArray[i] = info.MaxCount;
                    minCountArray[i] = info.MinCount;
                    averageCountArray[i] = info.AverageCount;
                    timeArray[i] = new XDate(info.Time);
                }

                GraphPane graphPane = new GraphPane(new Rectangle(0, 0, 840, 450), String.Empty, String.Empty, String.Empty);

                graphPane.Fill = new Fill(Color.FromArgb(212, 208, 200));

                graphPane.Legend.Fill.IsVisible = false;
                graphPane.Legend.Border.IsVisible = false;
                graphPane.Legend.FontSpec.Fill.IsVisible = false;

                graphPane.XAxis.Title.Text = "时间";
                graphPane.XAxis.MajorGrid.Color = Color.DarkGreen;
                graphPane.XAxis.Type = AxisType.Date;
                graphPane.XAxis.Scale.FontSpec.Size = 11;

                graphPane.YAxis.Title.Text = "玩家数量";
                //graphPane.YAxis.MajorGrid.IsVisible = true;
                //graphPane.YAxis.MajorGrid.DashOff = 0;
                //graphPane.YAxis.MajorGrid.Color = Color.Gray;
                //graphPane.YAxis.MinorGrid.IsVisible = true;
                //graphPane.YAxis.MinorGrid.Color = Color.LightGray;
                //graphPane.YAxis.MinorGrid.DashOff = 0;
                graphPane.YAxis.Scale.Min = 0;

                graphPane.Title.Text = string.Format("{0} [ {1}  {2} ]", "玩家数量", _startTime, _endTime);

                graphPane.AddCurve("最大", timeArray, maxCountArray, Color.Red, SymbolType.Triangle);
                graphPane.AddCurve("最小", timeArray, minCountArray, Color.Green, SymbolType.TriangleDown);
                graphPane.AddCurve("平均", timeArray, averageCountArray, Color.Orange, SymbolType.Diamond);

                graphPane.AxisChange();

                fileName = server.Group.Name + "-" + DateTime.Now.Date.ToString("yyyy-MM-dd") + "-(Day)" + ".jpg";

                MemoryStream imageMemoryStream = new MemoryStream();
                graphPane.GetImage().Save(imageMemoryStream, System.Drawing.Imaging.ImageFormat.Jpeg);
                imageMemoryStream.Position = 0;

                return imageMemoryStream;
            }
            else
            {
                fileName = String.Empty;
                return null;
            }
        }

        private string GetPlayerCountTextOneMonth()
        {
            string contentString = String.Empty;

            foreach (GameServer server in AdminServer.TheInstance.GameServerManager.Servers)
            {
                if (server.Type == GameServer.ServerType.gamecenter)
                {
                    contentString += GetPlayerCountTextOneMonthOneServer(server);
                }
            }

            return contentString;
        }

        private string GetPlayerCountTextOneMonthOneServer(GameServer server)
        {
            LordControl.PlayerCountStatisticInfo[] _infoArray = null;
            DateTime _endTime = DateTime.Now;
            DateTime _startTime = _endTime.AddMonths(-1);

            string contentString = String.Empty;

            _infoArray = server.GetPlugInData(0, LordControl.PlugInGuid, LordControl.DataKeyPlayerCountStatistic, _startTime, _endTime) as LordControl.PlayerCountStatisticInfo[];

            ArrayList playerCountArrayList = new ArrayList();

            if (_infoArray.Length != 0)
            {
                LordControl.PlayerCountStatisticInfo tempInfo = new LordControl.PlayerCountStatisticInfo();
                tempInfo.Time = _infoArray[0].Time;

                int countInOneDay = 0;

                for (int i = 0; i < _infoArray.Length; i++)
                {
                    LordControl.PlayerCountStatisticInfo info = _infoArray[i];

                    if (tempInfo.Time.Date == info.Time.Date)
                    {
                        if (info.MaxCount > tempInfo.MaxCount)
                            tempInfo.MaxCount = info.MaxCount;
                        if (info.MinCount < tempInfo.MinCount)
                            tempInfo.MinCount = info.MinCount;
                        tempInfo.AverageCount += info.AverageCount;
                        countInOneDay++;

                        if (i == _infoArray.Length - 1)
                        {
                            tempInfo.AverageCount /= countInOneDay;
                            playerCountArrayList.Add(tempInfo);
                        }
                    }
                    else
                    {
                        tempInfo.AverageCount /= countInOneDay;

                        playerCountArrayList.Add(tempInfo);

                        tempInfo = new LordControl.PlayerCountStatisticInfo();

                        tempInfo.Time = info.Time;
                        tempInfo.AverageCount = info.AverageCount;
                        tempInfo.MaxCount = info.MaxCount;
                        tempInfo.MinCount = info.MinCount;

                        countInOneDay = 1;

                        if (i == _infoArray.Length - 1)
                        {
                            playerCountArrayList.Add(tempInfo);
                        }
                    }
                }

                string groupName = server.Group.Name;
                contentString += "<div style=\"font-family:Arial\">" + groupName + "</div>";

                contentString += "<table>";

                string timeString = "<tr><td>时间</td>";
                string maxString = "<tr><td>最大</td>";
                string aveString = "<tr><td>平均</td>";
                string minString = "<tr><td>最小</td>";

                foreach (LordControl.PlayerCountStatisticInfo info in playerCountArrayList)
                {
                    timeString += "<td>" + info.Time.ToString("MM-dd") + "</td>";
                    maxString += "<td>" + info.MaxCount.ToString() + "</td>";
                    aveString += "<td>" + info.AverageCount.ToString() + "</td>";
                    minString += "<td>" + info.MinCount.ToString() + "</td>";
                }

                timeString += "</tr>";
                maxString += "</tr>";
                aveString += "</tr>";
                minString += "</tr>";

                contentString += timeString;
                contentString += maxString;
                contentString += aveString;
                contentString += minString;

                contentString += "</table>";

                contentString += "<br />";
            }

            return contentString;
        }

        private MemoryStream CreatePlayerCountImageOneMonth(GameServer server, out string fileName)
        {
            LordControl.PlayerCountStatisticInfo[] _infoArray = null;
            DateTime _endTime = DateTime.Now;
            DateTime _startTime = _endTime.AddMonths(-1);

            _infoArray = server.GetPlugInData(0, LordControl.PlugInGuid, LordControl.DataKeyPlayerCountStatistic, _startTime, _endTime) as LordControl.PlayerCountStatisticInfo[];

            if (_infoArray.Length != 0)
            {
                ArrayList playerCountArrayList = new ArrayList();

                LordControl.PlayerCountStatisticInfo tempInfo = new LordControl.PlayerCountStatisticInfo();
                tempInfo.Time = _infoArray[0].Time;

                int countInOneDay = 0;

                for (int i = 0; i < _infoArray.Length; i++)
                {
                    LordControl.PlayerCountStatisticInfo info = _infoArray[i];

                    if (tempInfo.Time.Date == info.Time.Date)
                    {
                        if (info.MaxCount > tempInfo.MaxCount)
                            tempInfo.MaxCount = info.MaxCount;
                        if (info.MinCount < tempInfo.MinCount)
                            tempInfo.MinCount = info.MinCount;
                        tempInfo.AverageCount += info.AverageCount;
                        countInOneDay++;

                        if (i == _infoArray.Length - 1)
                        {
                            tempInfo.AverageCount /= countInOneDay;
                            playerCountArrayList.Add(tempInfo);
                        }
                    }
                    else
                    {
                        tempInfo.AverageCount /= countInOneDay;

                        playerCountArrayList.Add(tempInfo);

                        tempInfo = new LordControl.PlayerCountStatisticInfo();

                        tempInfo.Time = info.Time;
                        tempInfo.AverageCount = info.AverageCount;
                        tempInfo.MaxCount = info.MaxCount;
                        tempInfo.MinCount = info.MinCount;

                        countInOneDay = 1;

                        if (i == _infoArray.Length - 1)
                        {
                            playerCountArrayList.Add(tempInfo);
                        }
                    }
                }

                double[] maxCountArray = new double[playerCountArrayList.Count];
                double[] minCountArray = new double[playerCountArrayList.Count];
                double[] averageCountArray = new double[playerCountArrayList.Count];
                double[] timeArray = new double[playerCountArrayList.Count];

                for (int i = 0; i < playerCountArrayList.Count; i++)
                {
                    LordControl.PlayerCountStatisticInfo info = playerCountArrayList[i] as LordControl.PlayerCountStatisticInfo;
                    maxCountArray[i] = info.MaxCount;
                    minCountArray[i] = info.MinCount;
                    averageCountArray[i] = info.AverageCount;
                    timeArray[i] = new XDate(info.Time);
                }

                GraphPane graphPane = new GraphPane(new Rectangle(0, 0, 840, 450), String.Empty, String.Empty, String.Empty);

                graphPane.Fill = new Fill(Color.FromArgb(212, 208, 200));

                graphPane.Legend.Fill.IsVisible = false;
                graphPane.Legend.Border.IsVisible = false;
                graphPane.Legend.FontSpec.Fill.IsVisible = false;

                graphPane.XAxis.Title.Text = "时间";
                graphPane.XAxis.MajorGrid.Color = Color.DarkGreen;
                graphPane.XAxis.Type = AxisType.Date;
                graphPane.XAxis.Scale.FontSpec.Size = 11;

                graphPane.YAxis.Title.Text = "玩家数量";
                //graphPane.YAxis.MajorGrid.IsVisible = true;
                //graphPane.YAxis.MajorGrid.DashOff = 0;
                //graphPane.YAxis.MajorGrid.Color = Color.Gray;
                //graphPane.YAxis.MinorGrid.IsVisible = true;
                //graphPane.YAxis.MinorGrid.Color = Color.LightGray;
                //graphPane.YAxis.MinorGrid.DashOff = 0;
                graphPane.YAxis.Scale.Min = 0;

                graphPane.Title.Text = string.Format("{0} [ {1}  {2} ]", "玩家数量", _startTime, _endTime);

                graphPane.AddCurve("最大", timeArray, maxCountArray, Color.Red, SymbolType.Triangle);
                graphPane.AddCurve("最小", timeArray, minCountArray, Color.Green, SymbolType.TriangleDown);
                graphPane.AddCurve("平均", timeArray, averageCountArray, Color.Orange, SymbolType.Diamond);

                graphPane.AxisChange();

                fileName = server.Group.Name + "-" + DateTime.Now.Date.ToString("yyyy-MM-dd") + "-(Month)" + ".jpg";

                MemoryStream imageMemoryStream = new MemoryStream();
                graphPane.GetImage().Save(imageMemoryStream, System.Drawing.Imaging.ImageFormat.Jpeg);
                imageMemoryStream.Position = 0;

                return imageMemoryStream;
            }
            else
            {
                fileName = String.Empty;
                return null;
            }
        }

        private string GetServerEventSummaryOneDay()
        {
            string summaryTitle = "<div style=\"font-family:Arial\"><br />各服务器24小时内事件记录<br /><br /></div>";
            DateTime endTime = DateTime.Now;
            DateTime startTime = endTime.AddDays(-1);
            string summaryContent = String.Empty;

            foreach (GameServer server in AdminServer.TheInstance.GameServerManager.Servers)
            {
                using (IBlazeDatabase db = DbFactory.GetDatabase())
                {
                    IBlazeTable table = db.GetTable(TableString.ServerEventTableName);
                    DataSet ds = new DataSet();
                    table.Get(ds,
                        FilterFactory.CreateAndFilter(
                            FilterFactory.CreateEqualFilter(TableString.ServerEventFieldServerId, server.Id),
                            FilterFactory.CreateAndFilter(
                                FilterFactory.CreateLargerEqualFilter(TableString.ServerEventFieldEventTime, startTime),
                                FilterFactory.CreateLesserEqualFilter(TableString.ServerEventFieldEventTime, endTime)
                                )
                            )
                    );

                    DataTable dt = ds.Tables[TableString.ServerEventTableName];

                    if (dt.Rows.Count != 0)
                    {
                        summaryContent += "<div style=\"font-family:Arial\"><br />"
                                        + server.Name + "<br /></div>";

                        foreach (DataRow dr in dt.Rows)
                        {
                            DateTime timeDateTime = (DateTime)dr[TableString.ServerEventFieldEventTime];
                            string timeString = timeDateTime.ToString("yyyy-MM-dd HH:mm:ss");
                            string eventTypeString = String.Empty;
                            GameServer.ServerEventType eventType = (GameServer.ServerEventType)dr[TableString.ServerEventFieldEventType];


                            switch (eventType)
                            {
                                case GameServer.ServerEventType.GameStart:
                                    eventTypeString = "正常启动";
                                    break;
                                case GameServer.ServerEventType.GameStartFail:
                                    eventTypeString = "启动失败";
                                    break;
                                case GameServer.ServerEventType.GameStop:
                                    eventTypeString = "正常关闭";
                                    break;
                                case GameServer.ServerEventType.GameStartByAccident:
                                    eventTypeString = "意外启动";
                                    break;
                                case GameServer.ServerEventType.GameStopByAccident:
                                    eventTypeString = "意外关闭";
                                    break;

                                default:
                                    eventTypeString = "未知事件";
                                    break;
                            }

                            summaryContent += "<div style=\"font-family:Arial\">"
                                            + timeString + "&nbsp;&nbsp;&nbsp;" + eventTypeString + "<br /></div>";
                        }
                    }
                }
            }

            if (summaryContent.Length == 0)
            {
                summaryContent = "<div style=\"font-family:Arial\"><br />无记录<br /></div>";
            }

            string summary = summaryTitle + summaryContent;
            
            return summary;
        }


    }
    #endregion
	
	#region KillGameAutomation
	/// <summary>
	/// 杀掉游戏
	/// </summary>
	/// <remarks>彻底清除游戏进程，不保证游戏正常退出</remarks>
	public class KillGameAutomation : Automation
	{
		public override string Guid
		{
			get { return "{C2746340-43AA-4694-BB48-E128D0831D68}"; }
		}

		public override string Name
		{
            get { return StringDef.KillGameAutomation; }
		}

		public override object[] Parameter
		{
            get { return new object[] { 0 }; }
			set { }
		}

        public override string Description
        {
            get { return StringDef.KillGameAutomationDes; }
        }

        public override string[] ParameterDes
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

		public override void Do(AutomationContext context)
		{
			IList<GameServer> serverList = context.GetAllServers();
			foreach (GameServer server in serverList)
			{
			}

			_state = AutomationState.Done;
			_result = AutomationResult.Success;
		}
	}
	#endregion

	#region CreateServerListFilesAutomation
	/// <summary>
    /// 创建服务器列表相关文件
	/// </summary>
	/// <remarks>包括serverlist.ini,servverlist.ini.crc,serverlist.pak</remarks>
	/// <author>徐晓刚</author>
	public class CreateServerListFilesAutomation : Automation
	{
        //string _fileDir;

		public override string Guid
		{
			get { return "{95D74633-91A5-483a-A3A9-CFB4357B3CB6}"; }
		}

		public override string Name
		{
			get { return StringDef.CreateServerListFilesAutomation; }
		}

        public override string Description
        {
            get { return StringDef.CreateServerListFilesAutomationDesNew; }
        }

        public override object[] Parameter
        {
            get
            {
                return new object[]{
                0
                };
            }
            set { }
        }

        public override string[] ParameterDes
        {
            get { throw new Exception("The method or operation is not implemented."); }

        }

        public override void Do(AutomationContext context)
        {
            try
            {
                switch (_state)
                {
                    case AutomationState.Init:
                        AdminServer.TheInstance.GameServerManager.CreateServerListFiles(
                            AdminServer.TheInstance.GameServerManager.ServerMap.NetList,
                            SystemConfig.Current.ServerListFileDir,
                            SystemConfig.Current.DefaultEncoding
                        );                        
                        _result = AutomationResult.Success;
                        _state = AutomationState.Done;
                        break;
                }
            }
            catch (Exception ex)
            {
                Util.DebugLog(ex.ToString());

                _state = AutomationState.Done;
                _result = AutomationResult.Failure;
            }
        }
	}
	#endregion

	
	#region LogAutomation
	/// <summary>
	/// 记录日志
	/// </summary>
	public class LogAutomation : Automation
	{
		public override string Guid
		{
			get { return "{945AF9F8-CB1E-4ad9-8EF8-AC90363AA536}"; }
		}

		public override string Name
		{
			get { return StringDef.LogAutomation; }
		}

		public override object[] Parameter
		{
            get { return new object[] { 0 }; }
			set { }
		}

        public override string Description
        {
            get { return StringDef.LogAutomationDes; }
        }

        public override string[] ParameterDes
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

		public override void Do(AutomationContext context)
		{
			int operatorId = AdminServer.TheInstance.AutomationManager.SecurityObject.Id;
			int targetId = 0;
			GameServer server = context.Server;
			if (server != null)
			{
				targetId = server.SecurityObject.Id;
			}
			string desc = context.Message;
			if (desc == null)
				desc = string.Empty;
			LogSystem.TheInstance.WriteLog(operatorId, OpType.READ, targetId, desc);

			_state = AutomationState.Done;
			_result = AutomationResult.Success;
		}
	}
	#endregion

	#region GetStackTraceAutomation
	/// <summary>
	/// 获取当机堆栈
	/// </summary>
	public class GetStackTraceAutomation : Automation
	{
		public static TimeSpan GetStackTraceTimeout = new TimeSpan(0, 0, 15);

		class GetStackTraceUnit
		{
			public GameServer Server;
			public bool IsDone;
			public DateTime StartTime;

			public GetStackTraceUnit(GameServer server)
			{
				Server = server;
				IsDone = false;
			}
		}

		IList<GetStackTraceUnit> _checkingGameServerList;

		public override string Guid
		{
			get { return "{6432197E-E633-4a0f-8849-C6DCFF9CCD1F}"; }
		}

		public override string Name
		{
			get { return StringDef.GetStackTraceAutomation; }
		}

        public override string Description
        {
            get { return StringDef.GetStackTraceAutomationDes; }
        }

        public override string[] ParameterDes
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

		public override object[] Parameter
		{
            get { return new object[] { 0 }; }
			set { }
		}

		public override void Do(AutomationContext context)
		{
			switch (_state)
			{
				case AutomationState.Init:
					{
						_checkingGameServerList = new List<GetStackTraceUnit>();
						IList<GameServer> serverList = context.GetAllServers();
						foreach (GameServer server in serverList)
						{
							object outArg = null;
                            //#########################################################
                            if (server.DoPlugInAction(SecurityManager.SystemAutomationId, GameServerControl.PlugInGuid, GameServerControl.ActionKeyGetStackTrace, ref outArg))
							{
								GetStackTraceUnit unit = new GetStackTraceUnit(server);
								unit.StartTime = DateTime.Now;
								unit.IsDone = false;
								_checkingGameServerList.Add(unit);
							}
						}

						_state = AutomationState.Doing;
					}
					break;
				case AutomationState.Doing:
					{						
						foreach (GetStackTraceUnit unit in _checkingGameServerList)
						{
                            //################################################
							GameServerControl.GetStackTraceState getStackTraceState = (GameServerControl.GetStackTraceState)unit.Server.GetPlugInData(-1,GameServerControl.PlugInGuid, GameServerControl.DataKeyGetStackTraceState);
							if (getStackTraceState == GameServerControl.GetStackTraceState.Done)
							{
								unit.IsDone = true;
                                context.Message += string.Format("\nStackTrace of {0}:\n{1}", unit.Server.ToString(), unit.Server.GetPlugInData(-1,GameServerControl.PlugInGuid, GameServerControl.DataKeyStackTrace) as string);
							}
							else if (DateTime.Now.Subtract(unit.StartTime) > GetStackTraceTimeout)
							{
								unit.IsDone = true;
							}
						}

						for (int i = _checkingGameServerList.Count - 1; i >= 0; i--)
						{
							if (_checkingGameServerList[i].IsDone)
							{
								_checkingGameServerList.RemoveAt(i);
							}
						}

						if (_checkingGameServerList.Count == 0)
						{
							_state = AutomationState.Done;
							_result = AutomationResult.Success;
						}
					}
					break;
			}
		}
	}
	#endregion

	#region SendSystemMessageAutomation
	/// <summary>
	/// 发送系统消息
	/// </summary>
	public class SendSystemMessageAutomation : Automation
	{
		string _message;

		public override string Guid
		{
			get { return "{D7882010-6F30-46f3-BEF3-1231E1D1FEA0}"; }
		}

		public override string Name
		{
			get { return StringDef.SendSystemMessageAutomation; }
		}

		public override object[] Parameter
		{
			get
			{
                return new object[]{
                    1,
                    _message};
			}
			set
			{
				_message = value[1] as string;
			}
		}

        public override string Description
        {
            get { return StringDef.SendSystemMessageAutomationDes; }
        }

        public override string[] ParameterDes
        {
            get
            {
                return new string[]{
                    StringDef.Message
                };
            }
        }

		public override void Do(AutomationContext context)
		{
			IList<GameServer> servers = context.GetAllServers();
			foreach(GameServer server in servers)
			{
                //################################################
                server.DoPlugInAction(SecurityManager.SystemAutomationId, LordControl.PlugInGuid, LordControl.ActionKeySayToWorld, _message);
			}

			_state = AutomationState.Done;
			_result = AutomationResult.Success;
		}
	}
	#endregion

	#region FilterServerAutomation
	/// <summary>
	/// 过滤服务器
	/// </summary>
	/// <remarks>过滤掉上下文中的服务器</remarks>
	public class FilterServerAutomation : Automation
	{
		GameServer.ServiceState[] _gameState = null;

        GameServer.ServerState[] _serverState = null;

		public override string Guid
		{
			get { return "{C085B153-A0F2-44cf-89FF-A52E563A2DFB}"; }
		}

		public override string Name
		{
            get { return StringDef.FilterServerAutomation; }
		}

        public override string[] ParameterDes
        {
            get
            {
                return new string[] { 
                    StringDef.GameState,
                    StringDef.ServerState
                };
            }
        }

        public override string Description
        {
            get { return StringDef.FilterServerAutomationDes; }
        }

        public override object[] Parameter
        {
            get
            {
                StringBuilder gameStateText = new StringBuilder();
                StringBuilder serverStateText = new StringBuilder();
                if (_gameState != null && _gameState.Length > 0)
                {
                    foreach (GameServer.ServiceState gameState in _gameState)
                    {
                        gameStateText.Append(gameState.ToString());
                        gameStateText.Append(' ');
                    }
                    gameStateText.Remove(gameStateText.Length - 1, 1);
                }

                if (_serverState != null && _serverState.Length > 0)
                {
                    foreach (GameServer.ServerState serverState in _serverState)
                    {
                        serverStateText.Append(serverState.ToString());
                        serverStateText.Append(' ');
                    }
                    serverStateText.Remove(serverStateText.Length - 1, 1);
                }

                string gameStateString = gameStateText.Length == 0 ? string.Empty : gameStateText.ToString();
                string serverStateString = serverStateText.Length == 0 ? string.Empty : serverStateText.ToString();

                return new object[] { 
                    2,
                    gameStateString,
                    serverStateString
                };
            }

            set
            {
                try
                {
                    if ((value[1] as string).Trim().Length != 0)
                    {
                        IList<GameServer.ServiceState> gameStateList = new List<GameServer.ServiceState>();
                        
                        string[] gameStateListString = (value[1] as string).Split(' ');
                        foreach (string gameState in gameStateListString)
                        {
                            if (gameState.Length != 0)
                                gameStateList.Add(GameServer.ParseServiceState(gameState));
                        }

                        if (gameStateList.Count > 0)
                        {
                            _gameState = new GameServer.ServiceState[gameStateList.Count];
                            gameStateList.CopyTo(_gameState, 0);
                        }
                    }

                    if ((value[2] as string).Trim().Length != 0)
                    {
                        IList<GameServer.ServerState> serverStateList = new List<GameServer.ServerState>();
                        string[] serverStateListString = (value[2] as string).Split(' ');
                        foreach (string serverState in serverStateListString)
                        {
                            if (serverState.Length != 0)
                                serverStateList.Add(GameServer.ParseServerState(serverState));
                        }
                        if (serverStateList.Count > 0)
                        {
                            _serverState = new GameServer.ServerState[serverStateList.Count];
                            serverStateList.CopyTo(_serverState, 0);
                        }
                    }

                    //string[] options = value.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                    //if (options != null && options.Length > 0)
                    //{
                    //    IList<GameServer.ServiceState> gameStateList = new List<GameServer.ServiceState>();
                    //    IList<GameServer.ServerState> serverStateList = new List<GameServer.ServerState>();
                    //    foreach (string option in options)
                    //    {
                    //        if (option.StartsWith("GameState="))
                    //        {
                    //            gameStateList.Add(GameServer.ParseServiceState(option.Substring(10)));
                    //        }
                    //        else if (option.StartsWith("ServerState="))
                    //        {
                    //            serverStateList.Add(GameServer.ParseServerState(option.Substring(12)));
                    //        }
                    //        else
                    //        {
                    //            throw new Exception("Invalid parameter.");
                    //        }
                    //    }    
                }
                catch (Exception e)
                {
                    throw new BadParameterException("Invalid parameter", e);
                }
            }
        }

		public override void Do(AutomationContext context)
		{
			IList<GameServer> toDelServerList = new List<GameServer>();
			IList<GameServer> servers = context.GetAllServers();
			foreach (GameServer server in servers)
			{
				bool matchGameState = true;
				bool matchServerState = true;
				if (_gameState != null && _gameState.Length > 0)
				{
					matchGameState = false;
					foreach (GameServer.ServiceState gameState in _gameState)
					{
						if (server.GameServiceState == gameState)
						{
							matchGameState = true;
							break;
						}
					}
				}
				if (_serverState != null && _serverState.Length > 0)
				{
					matchServerState = false;
					foreach (GameServer.ServerState serverState in _serverState)
					{
						if (server.GameServerState == serverState)
						{
							matchServerState = true;
							break;
						}
					}
				}

				if (!(matchServerState && matchGameState))
					toDelServerList.Add(server);
			}

			foreach (GameServer server in toDelServerList)
			{
				context.RemoveGameServer(server);
			}

			_result = AutomationResult.Success;
			_state = AutomationState.Done;
		}
	}
	#endregion

    #region FileTranferAutomation
    /// <summary>
    /// 传输(上传)文件Automation
    /// </summary>
    public class FileTransferAutomation : Automation
    {
        class FileTransferUnit
        {
            public GameServer Server;
            public bool IsDone;
            public bool IsSuccess;

            public FileTransferUnit(GameServer server)
            {
                Server = server;
                IsDone = false;
                IsSuccess = false;
            }
        }

        Hashtable _fileTransferUnitHashtable;

        int _failCount = 0;

        string _sourceFileName;
        string _targetFileName;

        public override string Guid
		{
            get { return "{2F563907-09FD-43bd-9ED3-7E0E8BC2FF5C}"; }
		}

		public override string Name
		{
			get { return StringDef.FileTransferAutomation; }
		}

        public override string[] ParameterDes
        {
            get
            {
                return new string[]{
                    StringDef.FileName
                };
            }
        }

        public override object[] Parameter
        {
            get
            {
                return new object[]{
                    1,
                    _sourceFileName};
            }
            set
            {
                try
                {
                    _sourceFileName = value[1] as string;
                    _targetFileName = _sourceFileName.Replace(' ', '_').Replace('\t', '_');
                }
                catch (Exception e)
                {
                    throw new BadParameterException("The Parameter's Format should be: GameServer's ID,SourceFileNames", e);
                }
            }
        }

        public override string Description
        {
            get { return StringDef.FileTransferAutomationDes; }
        }

        public override void Do(AutomationContext context)
        {
            switch (_state)
            {
                case AutomationState.Init:
                    _fileTransferUnitHashtable = new Hashtable();

                    IList<GameServer> serverList = context.GetAllServers();
                    foreach (GameServer server in serverList)
                    {
                        FileTransferUnit unit = new FileTransferUnit(server);
                        _fileTransferUnitHashtable.Add(server.Id, unit);
                        server.AddFMTask(new FileTranTask(this, _sourceFileName, _targetFileName));
                    }

                    _state = AutomationState.Doing;
                    break;
                case AutomationState.Doing:
                    foreach (GameServer server in context.GetAllServers())
                    {
                        FileTransferUnit unit = _fileTransferUnitHashtable[server.Id] as FileTransferUnit;
                        if (unit != null && unit.IsDone)
                        {
                            if (!unit.IsSuccess) ++_failCount;
                            _fileTransferUnitHashtable.Remove(unit.Server.Id);
                        }
                    }

                    if (_fileTransferUnitHashtable.Count == 0)
                    {
                        _state = AutomationState.Done;
                        if (_failCount == 0) _result = AutomationResult.Success;
                        else _result = AutomationResult.Failure;
                    }
                    break;
            }
        }

        public void FinishUnit(GameServer server, bool isSuccess)
        {
            if (server != null)
            {
                FileTransferUnit unit = _fileTransferUnitHashtable[server.Id] as FileTransferUnit;
                if (unit != null)
                {
                    unit.IsDone = true;
                    unit.IsSuccess = isSuccess;
                }
            }
        }
    }
    #endregion

    #region FileDownloadAutomation
    /// <summary>
    /// 传输(上传)文件Automation
    /// </summary>
    public class FileDownloadAutomation : Automation
    {
        class FileDownloadUnit
        {
            public GameServer Server;
            public bool IsDone;
            public bool IsSuccess;

            public FileDownloadUnit(GameServer server)
            {
                Server = server;
                IsDone = false;
                IsSuccess = false;
            }
        }

        Hashtable _fileDownloadUnitHashtable;
               
        string _sourceFileName;
        string _targetFileName;

        int _failCount = 0;

        public override string Guid
        {
            get { return "{D2CAE48D-5654-4521-BA2A-20026050B9B5}"; }
        }

        public override string Name
        {
            get { return StringDef.FileDownloadAutomation; }
        }

        public override string[] ParameterDes
        {
            get
            {
                return new string[]{
                    StringDef.FileName
                };
            }
        }

        public override object[] Parameter
        {
            get
            {
                return new object[]{
                    1,                    
                    _sourceFileName};
            }
            set
            {
                try
                {
                    _sourceFileName = value[1] as string;
                    string filterSourceName = _sourceFileName.Replace(' ', '_').Replace('\t', '_');
                    _targetFileName = filterSourceName.Substring(filterSourceName.LastIndexOf('/') + 1);
                }
                catch (Exception e)
                {
                    throw new BadParameterException("The Parameter's Format should be: SourceFileNames", e);
                }
            }
        }

        public override string Description
        {
            get { return StringDef.FileDownloadAutomationDes; }
        }

        public override void Do(AutomationContext context)
        {
            switch (_state)
            {
                case AutomationState.Init:
                    _fileDownloadUnitHashtable = new Hashtable();
                    IList<GameServer> serverList = context.GetAllServers();
                    foreach (GameServer server in serverList)
                    {
                        FileDownloadUnit unit = new FileDownloadUnit(server);
                        _fileDownloadUnitHashtable.Add(server.Id, unit);

                        //生成一个Automation相关的FileDownloadTask然后加入File队列中，传输默认为覆盖选项。
                        server.AddFMTask(new FileDownloadTask(this, _sourceFileName, _targetFileName, true));                        
                    }

                    _state = AutomationState.Doing;
                    break;
                case AutomationState.Doing:
                    foreach (GameServer server in context.GetAllServers())
                    {
                        FileDownloadUnit unit = _fileDownloadUnitHashtable[server.Id] as FileDownloadUnit;
                        if (unit != null && unit.IsDone)
                        {
                            if (!unit.IsSuccess) ++_failCount;
                            _fileDownloadUnitHashtable.Remove(unit.Server.Id);
                        }
                    }

                    if (_fileDownloadUnitHashtable.Count == 0)
                    {
                        _state = AutomationState.Done;
                        if (_failCount == 0) _result = AutomationResult.Success;
                        else _result = AutomationResult.Failure;
                    }
                    break;
            }
        }

        public void FinishUnit(GameServer server, bool isSuccess)
        {
            if (server != null)
            {
                FileDownloadUnit unit = _fileDownloadUnitHashtable[server.Id] as FileDownloadUnit;
                if (unit != null)
                {
                    unit.IsDone = true;
                    unit.IsSuccess = isSuccess;
                }
            }
        }
    }
    #endregion

	#region AbstractExeSysCmdAutomation
	/// <summary>
	/// 抽象的执行系统命令的Automation
	/// </summary>
	public abstract class AbstractExeSysCmdAutomation : Automation
	{
		int _completeCount;
		bool _exeSysCmdCompleted;
		IList<ExeSysCmdUnit> _unitList;

		class ExeSysCmdUnit
		{
			public GameServer Server = null;
			public string Output = null;
			public bool Success = false;
			public bool Done = false;
		}

		protected abstract bool CreateScript(GameServer server, out string script, out string input);

		protected virtual void OnCommandSent(GameServer server)
		{
			//Do nothing
		}

		protected virtual void OnCommandComplete(GameServer server, bool success, string output)
		{
			//Do nothing
		}

		public override void Do(AutomationContext context)
		{
			try
			{
				switch (_state)
				{
					case AutomationState.Init:
						{
							_exeSysCmdCompleted = false;
							_completeCount = 0;

							IList<GameServer> serverList = context.GetAllServers();
							_unitList = new List<ExeSysCmdUnit>();
							foreach (GameServer server in serverList)
							{
								string script, input;
								if (CreateScript(server, out script, out input))
								{
									if (server.DoPlugInAction(
										SecurityManager.SystemAutomationId,
										GameServerControl.PlugInGuid,
										GameServerControl.ActionKeyExeSysCmd,
										script,
										input,
										FSEye.PlugIn.GameServerControl.GuardTask.DoNormalSysCmd,
										new GameServerControl.ExeSysCmdComplete(ExeSysCmdComplete)))
									{
										OnCommandSent(server);

										ExeSysCmdUnit exeunit = new ExeSysCmdUnit();
										exeunit.Server = server;
										_unitList.Add(exeunit);
									}
								}
							}

							_result = (_unitList.Count == serverList.Count) ? AutomationResult.Success : AutomationResult.Failure;
							if (_unitList.Count > 0)
								_state = AutomationState.Doing;
							else
								_state = AutomationState.Done;
						}
						break;
					case AutomationState.Doing:
						{
							if (_exeSysCmdCompleted)
							{
								bool allSuccess = true;
								StringBuilder exeCmdOuput = new StringBuilder();
								foreach (ExeSysCmdUnit unit in _unitList)
								{
									if (unit.Done)
									{
										exeCmdOuput.AppendLine(unit.Output);
										if (!unit.Success)
										{
											allSuccess = false;
										}
									}
								}

								context.Message = exeCmdOuput.ToString();

								if (!allSuccess)
									_result = AutomationResult.Failure;
								_state = AutomationState.Done;
							}
						}
						break;
				}
			}
			catch (Exception)
			{
				_result = AutomationResult.Failure;
				_state = AutomationState.Done;
			}
		}

		void ExeSysCmdComplete(GameServer server, bool success, string resultOutput)
		{
			if (server == null)
				throw new ArgumentNullException("server");
			if (resultOutput == null)
				throw new ArgumentNullException("resultOutput");

			for (int i = 0; i < _unitList.Count; i++)
			{
				ExeSysCmdUnit unit = _unitList[i];
				if (!unit.Done && unit.Server.Id == server.Id)
				{
					unit.Output = resultOutput;
					unit.Success = success;
					unit.Done = true;
					_completeCount++;
					OnCommandComplete(server, success, resultOutput);

					break;
				}
			}

			if (_completeCount >= _unitList.Count)
			{
				_exeSysCmdCompleted = true;
			}
		}
	}
	#endregion

    #region ExeSysCmdAutomation
    /// <summary>
    /// 执行系统命令的Automation
    /// </summary>
    public class ExeSysCmdAutomation : AbstractExeSysCmdAutomation
    {
        string _scriptText;

		string _input="";

		public override string Guid
        {
            get { return "{4B100E4A-D400-4e7e-9096-F7F25E3740D5}"; }
        }

        public override string Name
        {
            get { return StringDef.ExeSysCmdAutomation; }
        }

        public override object[] Parameter
        {
            get
            {
                return new object[] { 1, _scriptText };
            }
            set
            {
                try
                {                    
                    _scriptText = value[1] as string;

					if (_scriptText == null || _scriptText.Length == 0)
						throw new BadParameterException(); 
                }
                catch (Exception e)
                {
                    throw new BadParameterException("The Parameter's Format should be: Script Text", e);
                }
            }
        }

        public override string[] ParameterDes
        {
            get
            {
                return new string[] {
                    StringDef.ScriptText
                };
            }
        }

        public override string Description
        {
            get { return StringDef.ExeSysCmdAutomationDes; }
        }

		protected override bool CreateScript(GameServer server, out string script, out string input)
		{
			script = _scriptText;
			input = _input;

			return (script != null);
		}
	}
    #endregion

    #region CreateLordConfigAutomation
    /// <summary>
    /// 创建Lord配置文件(当前配置文件)的Automation
    /// </summary>
    public class CreateLordConfigAutomation : Automation
    {
        class CreateLordConfigUnit
        {
            public GameServer Server;
			public bool IsDone;
            public bool IsSuccess;

			public CreateLordConfigUnit(GameServer server)
			{
                Server = server;
                IsDone = false;
                IsSuccess = false;
			}
        }

        int _failCount = 0;

        IList<CreateLordConfigUnit> _createLordConfigUnitList;

        public override string Guid
        {
            get { return "{D4ABE425-3654-4922-B6BC-AC37D7C93BA1}"; }
        }

        public override string Name
        {
            get { return StringDef.CreateLordConfigAutomation; }
        }

        public override string Description
        {
            get { return StringDef.CreateLordConfigAutomationDes; }
        }

        public override object[] Parameter
        {
            get { return new object[] { 0 }; }
            set { }
        }

        public override string[] ParameterDes
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public override void Do(AutomationContext context)
        {
            switch (_state)
            {
                case AutomationState.Init:
                    {
                        _createLordConfigUnitList = new List<CreateLordConfigUnit>();
                        IList<GameServer> serverList = context.GetAllServers();

                        FileStream fileStream = null;
                        string fileName = string.Empty;

                        foreach (GameServer server in serverList)
                        {
                            CreateLordConfigUnit unit = new CreateLordConfigUnit(server);
                            _createLordConfigUnitList.Add(unit);
                            try
                            {   
                                fileName = SystemConfig.Current.AdminServerUploadFileRootPath + unit.Server.Id + "_" + unit.Server.Name + "_guard.cfg";
                                fileStream = new FileStream(fileName, FileMode.OpenOrCreate);
                                server.LordConfig.WriteLordConfig(fileStream);
                                unit.IsDone = true;
                                unit.IsSuccess = true;
                            }
                            catch (Exception)
                            {
                                unit.IsDone = true;
                                unit.IsSuccess = false;
                            }
                            finally
                            {
                                if (fileStream != null) fileStream.Close();
                            }                            
                        }

                        _state = AutomationState.Doing;
                    }
                    break;
                case AutomationState.Doing:
                    {   
                        for (int i = _createLordConfigUnitList.Count - 1; i >= 0; i--)
                        {
                            if (_createLordConfigUnitList[i].IsDone)
                            {
                                if (!_createLordConfigUnitList[i].IsSuccess) ++_failCount;
                                _createLordConfigUnitList.RemoveAt(i);
                            }
                        }

                        if (_createLordConfigUnitList.Count == 0)
                        {
                            _state = AutomationState.Done;
                            if (_failCount == 0) _result = AutomationResult.Success;
                            else _result = AutomationResult.Failure;
                        }
                    }
                    break;
            }
        }
    }
    #endregion

    #region DeleteLocalFileAutomation
    /// <summary>
    /// 删除FSEye本地文件的Automation
    /// </summary>
    public class DeleteLocalFileAutomation:Automation
    {
        string _fileName;

        public override string Guid
        {
            get { return "{C3AB618B-E4CD-4bcf-992E-543BDE4A08A0}"; }
        }

        public override string Name
        {
            get { return StringDef.DeleteLocalFileAutomation; }
        }

        public override object[] Parameter
        {
            get
            {
                return new object[]{
                    1,
                    _fileName};
            }
            set
            {
                try
                {
                    _fileName = value[1] as string;
                }
                catch (Exception e)
                {
                    throw new BadParameterException("The Parameter's Format should be: File's name", e);
                }
            }
        }

        public override string[] ParameterDes
        {
            get
            {
                return new string[]{
                    StringDef.FileName
                };
            }
        }

        public override string Description
        {
            get { return StringDef.DeleteLocalFileAutomationDes; }
        }

        public override void Do(AutomationContext context)
        {
            try
            {
                if (_state == AutomationState.Init)
                {
                    File.Delete(SystemConfig.Current.AdminServerUploadFileRootPath + _fileName);
                    Finish(AutomationResult.Success);
                }
            }
            catch (Exception)
            {
                Finish(AutomationResult.Failure);
            }
        }

        public void Finish(AutomationResult result)
        {
            _result = result;
            _state = AutomationState.Done;
        }
    }
    #endregion

	#region BackupDatabaseAutomation
	/// <summary>
	/// 备份数据库Automation
	/// </summary>
	public class BackupDatabaseAutomation : AbstractExeSysCmdAutomation
	{
		int _backupMode;

		public override string Guid
		{
			get { return "{AFBB6A55-B7E1-490a-BB85-F0919A9F081A}"; }
		}

		public override string Name
		{
			get { return StringDef.BackupDatabaseAutomation; }
		}

		public override object[] Parameter
		{
			get
			{
				return new object[] { 1, _backupMode };
			}

			set
			{
				try
				{
					_backupMode = int.Parse(value[1] as string);
				}
				catch (Exception e)
				{
					throw new BadParameterException("The Parameter's Format should be: BackupMode. InnerException: ", e);
				}
			}
		}

		public override string[] ParameterDes
		{
			get
			{
				return new string[] {
                    StringDef.BackupMode
                };
			}
		}

		public override string Description
		{
			get { return StringDef.BackupDatabaseAutomationDes; }
		}

		protected override bool CreateScript(GameServer server, out string script, out string input)
		{
			script = (string)server.GetPlugInData(0, GameServerControl.PlugInGuid, GameServerControl.DataKeyBackupDbScript, _backupMode);
			input = null;

			return (script != null);
		}
	}
	#endregion

    #region SendSMSAutomation
    /// <summary>
    /// 发送短信
    /// </summary>
    public class SendSMSAutomation : Automation
    {

        string _url;
        string _command;
        string _phone;
        string _content;

        public override string Guid
        {
            get { return "{E432197E-E633-4a0f-8849-C6DCFF9CCD1F}"; }
        }

        public override string Name
        {
            get { return StringDef.SendSMSAutomation; }
        }

        public override string Description
        {
            get { return StringDef.SendSMSAutomationDes; }
        }

        public override object[] Parameter
        {
            get
            {
                return new object[]{
                   2,                    
                    _phone,
                    _content};
            }
            set
            {
                try
                {
                    _phone = value[1] as string;
                    _content = value[2] as string; //存到库里面的是经过Server.UrlEncode ?s
                }
                catch (Exception e)
                {
                    throw new BadParameterException("The Parameter's Format should be: Phone,SMS", e);
                }
            }
        }

        public override string[] ParameterDes
        {
            get
            {
                return new string[]{
                    StringDef.Phone,
                    StringDef.SmsContent
                };
            }
        }

        public override void Do(AutomationContext context)
        {
            switch (_state)
            {
                case AutomationState.Init:

                    string postData = string.Empty;
                    HttpWebRequest webreq = null;
                    HttpWebResponse webres = null;
                    Stream responseStream = null;
                    StreamReader sr = null;
                    String resContent = string.Empty;

                    _url = SystemConfig.Current.SMSUrl;
                    _command = SystemConfig.Current.SMSCommand;

                    postData = string.Format("command={0}&phone={1}&content={2}", _command, _phone, System.Web.HttpUtility.UrlEncode(context.Message + _content,Encoding.GetEncoding("gb2312")));
                    postData = _url + "?" + postData; 
                    webreq = (HttpWebRequest)HttpWebRequest.Create(postData);
                    webreq.Method = "GET";
                    webreq.KeepAlive = false;
                    
                    try
                    {
                        webres = (HttpWebResponse)webreq.GetResponse();
                    }
                    catch (WebException ex)
                    {
                        webres = (HttpWebResponse)ex.Response;
                    }
                        if (((int)webres.StatusCode) >= 400)
                        {
                            //出错
                            Util.DebugLog("SMS WEB FAILURE!" + webres.StatusCode.ToString());
                        }
                        else
                        {
                            responseStream = webres.GetResponseStream();
                            sr = new StreamReader(responseStream, Encoding.Default);
                            resContent = sr.ReadToEnd();
                            Util.DebugLog("SMS: " + resContent + " " + postData);
                            webres.Close();
                            sr.Close();
                        }
                    
                    _state = AutomationState.Done;

                    break;
            }
        }
    }
    #endregion

	#region RollbackDatabaseAutomation
	/// <summary>
	/// 回滚数据库Automation
	/// </summary>
	public class RollbackDatabaseAutomation : AbstractExeSysCmdAutomation
	{
		string _backupFileName;

		public override string Guid
		{
			get { return "{B9ED3837-E44E-4ec4-97DB-9EE55A86536B}"; }
		}

		public override string Name
		{
			get { return StringDef.RollbackDatabaseAutomation; }
		}

		public override object[] Parameter
		{
			get
			{
				return new object[] { 1, _backupFileName };
			}

			set
			{
				try
				{
					_backupFileName = (string)value[1];
				}
				catch (Exception e)
				{
					throw new BadParameterException("The Parameter's Format should be: BackupFileName. InnerException: ", e);
				}
			}
		}

		public override string[] ParameterDes
		{
			get
			{
				return new string[] {
					StringDef.BackupFileName
                };
			}
		}

		public override string Description
		{
			get { return StringDef.RollbackDatabaseAutomationDes; }
		}

		protected override bool CreateScript(GameServer server, out string script, out string input)
		{
			script = (string)server.GetPlugInData(0, GameServerControl.PlugInGuid, GameServerControl.DataKeyRollbackDbScript, _backupFileName);
			input = null;

			return (script != null);
		}
	}
	#endregion

    #region BatchTaskAutomation
    public class BatchTaskAutomation : Automation
    {
        int _taskId = 0;

        public override string Guid
        {
            get { return "{70B48EA9-B689-436d-BDAA-D72299A74E8F}"; }
        }

        public override string Name
        {
            get { return StringDef.BatchTaskAutomation; }
        }

        public override object[] Parameter
        {
            get
            {
                return new object[] { 1, _taskId };
            }

            set
            {
                try
                {
                    if(value[1].GetType() == typeof(string))
                    {
                        _taskId = int.Parse(value[1] as string);
                    }
                    else if(value[1].GetType() == typeof(int))
                    {
                        _taskId = (int)value[1];
                    }
                }
                catch (Exception e)
                {
                    throw new BadParameterException("The Parameter's Format should be: TaskId. InnerException: ", e);
                }
            }
        }

        public override string[] ParameterDes
        {
            get
            {
                return new string[] {
					"批处理任务ID"
                };
            }
        }

        public override string Description
        {
            get { return "批处理任务"; }
        }

        public override void Do(AutomationContext context)
        {
            switch (_state)
            {
                case AutomationState.Init:
                    {
                        BatchTask task = AdminServer.TheInstance.BatchTaskManager.GetBatchTask(_taskId);
                        if (task != null)
                        {
                            try
                            {
                                task.Reset();
                                task.Start(new BatchTask.BatchTaskComplete(BatchTaskComplete), task);
                            }
                            catch (Exception)
                            {
                                _state = AutomationState.Done;
                                _result = AutomationResult.Failure;
                            }
                            _state = AutomationState.Doing;
                        }
                        else
                        {
                            _state = AutomationState.Done;
                            _result = AutomationResult.Failure;
                        }
                    }
                    break;
            }
        }

        public void BatchTaskComplete(object callBackObject)
        {
            BatchTask task = callBackObject as BatchTask;
            if (task != null)
            {
                _state = AutomationState.Done;
                _result = task.Success ? AutomationResult.Success : AutomationResult.Failure;
            }
        }
    }
    #endregion

    #region UploadServerListFilesToFtpAutomation
    /// <summary>
    /// 上传服务器列表相关文件到FTP
    /// </summary>
    public class UploadServerListFilesToFtpAutomation : Automation
    {
        string _ftpServerIdString;
        int _timeOutSeconds = 300;
        IList _ftpServerIdIList;

        public IList FtpServerIdList
        {
            set { _ftpServerIdIList = value; }
            
            get { return _ftpServerIdIList; }
        }

        public int TimeOutSeconds
        {
            set { _timeOutSeconds = value; }

            get { return _timeOutSeconds; }
        }

        public override string Guid
        {
            get { return "{2A034C09-D437-4b6a-8338-B76AA8F5D543}"; }
        }

        public override string Name
        {
            get { return StringDef.UploadServerListFilesToFtpAutomation; }
        }

        public override string Description
        {
            get { return StringDef.UploadServerListFilesToFtpAutomationDes; }
        }

        public override object[] Parameter
        {
            get
            {
                return new object[]{
                2,
                _ftpServerIdString,
                _timeOutSeconds};
            }
            set
            {
                try
                {
                    _ftpServerIdString = value[1] as string;
                    if (_ftpServerIdString != null && _ftpServerIdString.Length != 0)
                    {
                        _ftpServerIdIList = new ArrayList();
                        foreach (string ftpServerId in _ftpServerIdString.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries))
                        {
                            _ftpServerIdIList.Add(int.Parse(ftpServerId));
                        }
                    }

                    string timeoutSecondsString = value[2] as string;
                    if ((timeoutSecondsString != null) && (timeoutSecondsString != String.Empty))
                        _timeOutSeconds = int.Parse(timeoutSecondsString);
                }
                catch (Exception e)
                {
                    throw new BadParameterException("The Parameter's Format does not meet the requirement", e);
                }
            }
        }

        public override string[] ParameterDes
        {
            get
            {
                return new string[]{
                    StringDef.FtpServerId,
                    "连接超时时间"
                };
            }
        }

        public override void Do(AutomationContext context)
        {
            try
            {
                switch (_state)
                {
                    case AutomationState.Init:
                        string serverListFilePath = SystemConfig.Current.ServerListFileDir + SystemConfig.Current.PakFileName;
                        string serverListCRCFilePath = SystemConfig.Current.ServerListFileDir + SystemConfig.Current.ServerListCRCFileName;

                        if ((File.Exists(serverListFilePath)) && (File.Exists(serverListCRCFilePath)))
                        {
                            if (_ftpServerIdIList != null)
                            {
                                foreach (int ftpServerId in _ftpServerIdIList)
                                {
                                    
                                        FTPServer ftpServer = AdminServer.TheInstance.FTPClient.GetFTPServer(ftpServerId);

                                        if (ftpServer != null)
                                        {
                                            try
                                            {
                                                FileInfo serverListFile = new FileInfo(serverListFilePath);
                                                UploadFile(serverListFile, ftpServer, _timeOutSeconds);

                                                FileInfo serverListCRCFile = new FileInfo(serverListCRCFilePath);
                                                UploadFile(serverListCRCFile, ftpServer, _timeOutSeconds);
                                            }                                            
                                            catch (Exception ex)
                                            {
                                                Util.DebugLog(ex.ToString() + SystemConfig.Current.DefaultNewLine +  "FTPServerIP: " + ftpServer.Address);
                                            }
                                        }
                                        else
                                        {
                                            Util.DebugLog("the ftpServer whose serverId is " + ftpServerId.ToString() + " does not exist, deleted maybe?");
                                        }
                                    
                                }
                            }
                        }

                        _result = AutomationResult.Success;
                        _state = AutomationState.Done;
                        break;
                }
            }
            catch(Exception ex)
            {
                Util.DebugLog(ex.ToString());

                _state = AutomationState.Done;
                _result = AutomationResult.Failure;
            }
        }

        static private void UploadFile(FileInfo uploadFile, FTPServer ftpServer, int timeOutSeconds)
        {
            string uploadUrl = ftpServer.FTP_URL_PREFIX + "serverlist/" + uploadFile.Name.Replace('#', '_');

            // Get the object used to communicate with the server.
            FtpWebRequest request = (FtpWebRequest)WebRequest.Create(uploadUrl);
            request.Method = WebRequestMethods.Ftp.UploadFile;

            //set the UserName and Password of this request
            request.Credentials = new NetworkCredential(ftpServer.UserName, ftpServer.Password);

            //set Timeout property (in milliseconds)(default is 300000, which is 5 minutes)
            if (timeOutSeconds > 0)    
                request.ReadWriteTimeout = timeOutSeconds * 1000;

            // Copy the contents of the file to the request stream.
            StreamReader sourceStream = new StreamReader(uploadFile.FullName, SystemConfig.Current.DefaultEncoding);
            byte[] fileContents = SystemConfig.Current.DefaultEncoding.GetBytes(sourceStream.ReadToEnd());
            sourceStream.Close();
            request.ContentLength = fileContents.Length;

            Stream requestStream = request.GetRequestStream();
            requestStream.Write(fileContents, 0, fileContents.Length);
            requestStream.Close();

            FtpWebResponse response = (FtpWebResponse)request.GetResponse();
            response.Close();
        }
    }
    #endregion
}