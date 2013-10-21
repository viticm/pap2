//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-8
//      File_base        : BasePage
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : 基本页类
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Drawing;
using System.Globalization;
using System.Threading;
using FSEye.Security;
using FSEye.FS2Lib;
using FSEye;

/// <summary>
/// 基本页类
/// </summary>
/// <author>徐晓刚</author>
public class BasePage : System.Web.UI.Page
{
	/// <summary>
	/// 角色基本信息
	/// </summary>
	protected class RoleBasicInfo
	{
		public int Id = 0;

		public string Name = string.Empty;

		public string Tong = string.Empty;

		public bool IsMale = true;

		public FS2RoleType Type = FS2RoleType.Jiashi;

		public SByte Level = 0;

		public string Account = string.Empty;

		public int TotalMoney = 0;

        public int WeekOnlineTime = 0;

		public RoleBasicInfo()
		{
		}
	}

	public BasePage()
	{
	}	

	public AdminServer TheAdminServer
	{
		get { return AdminServer.TheInstance; }
	}

	public UserInfo CurrentUser
	{
		get { return (UserInfo)Session["CurrentUser"]; }
		set { Session["CurrentUser"] = value; }
	}

	public void ClearMessage()
	{
		Label messageLabel = (Master.FindControl("LabelOpResultMessage") as Label);
		if(messageLabel!=null)messageLabel.Visible = false;
	}
	
	public void ShowMessage(string message, MessageType type)
	{
		Label messageLabel = (Master.FindControl("LabelOpResultMessage") as Label);
		//Label messageLabel = FindControl("LabelOperationResult") as Label;
		if (messageLabel != null)
		{
			messageLabel.Visible = true;
			messageLabel.Text = message;

			string cssClass = CssConfig.MessageNormal;
			switch (type)
			{
				case MessageType.Success:
					cssClass = CssConfig.MessageSuccess;
					break;
				case MessageType.Failure:
					cssClass = CssConfig.MessageFailure;
					break;
				case MessageType.Attention:
					cssClass = CssConfig.MessageAttention;
					break;
				case MessageType.Normal:
					cssClass = CssConfig.MessageNormal;
					break;
			}
			messageLabel.CssClass = cssClass;
		}
	}
}