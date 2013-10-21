using System;
using System.Data;
using System.Configuration;
using System.Collections;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Text;
using Resources;
using FSEye;
using FSEye.Security;

public partial class Common_SystemLogin : BasePage
{
	/// <summary>
	/// ×î¶àµÇÂ¼´íÎó¼ÆÊý
	/// </summary>
	public const int MaxLoginErrorCount = 3;

	protected void Page_Load(object sender, EventArgs e)
	{
		TextBoxUserName.Focus();

		if (Session["CurrentUser"] != null)
		{
            Response.Redirect("~/GameServer/ServerOverviewAndMonitor.aspx");
		}

		LabelLoginError.Visible = false;
	}

	protected void ButtonLogin_Click(object sender, EventArgs e)
	{
		//ÊÇ·ñ´¦ÓÚ¡°½ûÖ¹µÇÂ½×´Ì¬¡±
		if (Session[WebConfig.SessionLoginForbiddenTime] != null)
		{
			DateTime forbiddenTime = (DateTime)Session[WebConfig.SessionLoginForbiddenTime];
			if (DateTime.Now > forbiddenTime)
			{
				Session.Remove(WebConfig.SessionLoginForbiddenTime);
			}
			else
			{				
				StringBuilder errorMsg = new StringBuilder();
				errorMsg.AppendFormat(StringDef.LoginForbiddenUntilTime, MaxLoginErrorCount, forbiddenTime);
				LabelLoginError.Text = errorMsg.ToString();
				LabelLoginError.Visible = true;

				return;
			}
		}

		string userName = TextBoxUserName.Text;
		string password = TextBoxPassword.Text;
		string clientAddress = Request.UserHostAddress;

		User user = null;
		try
		{
			user = TheAdminServer.SecurityManager.Login(userName, password, clientAddress);

			if (user != null)
			{
				UserInfo info = new UserInfo();
				info.Id = user.SecurityObject.Id;
				info.UserName = user.UserName;
				info.RealName = user.RealName;
				info.LoginTime = DateTime.Now;
				info.LoginId = user.LoginId;

				CurrentUser = info;
			}

			if (CurrentUser != null)//µÇÂ¼³É¹¦
			{
				//StringBuilder responseUrl = new StringBuilder();
				//string https = Request.ServerVariables["HTTPS"];
				//string serverName = Request.ServerVariables["SERVER_NAME"];
				//string serverPort = Request.ServerVariables["SERVER_PORT"];
				//string scriptName = Request.ServerVariables["SCRIPT_NAME"];
				//if (scriptName != null)
				//{
				//    int lastIndex = scriptName.LastIndexOf('/');
				//    scriptName = scriptName.Remove(lastIndex);
				//}			
				//responseUrl.Append("http://");
				//if (serverName != null)
				//    responseUrl.Append(serverName);
				//if (serverPort != null)
				//{
				//    if (https == null || https.Length == 0)
				//        responseUrl.Append(":" + serverPort);
				//}
				//if (scriptName != null)
				//    responseUrl.Append(scriptName);
				//responseUrl.Append("/HomePage.aspx");

				//Response.Redirect(responseUrl.ToString());

                Response.Redirect("~/GameServer/ServerOverviewAndMonitor.aspx");
			}
			else//µÇÂ¼Ê§°Ü
			{
				int loginErrorCount = 0;
				object loginErrorCountObj = Session[WebConfig.SessionLoginErrorCount];
				if (loginErrorCountObj == null)
				{
					loginErrorCount = 1;
				}
				else
				{
					loginErrorCount = (int)loginErrorCountObj;
					loginErrorCount++;
				}

				StringBuilder errorMsg = new StringBuilder();

				//Á¬ÐøµÇÂ¼Ê§°Ü
				if (loginErrorCount >= MaxLoginErrorCount)
				{
					DateTime forbiddenTime = DateTime.Now.AddMinutes(1);
					Session[WebConfig.SessionLoginForbiddenTime] = forbiddenTime;
					Session.Remove(WebConfig.SessionLoginErrorCount);
					errorMsg.AppendFormat(StringDef.LoginForbiddenUntilTime, MaxLoginErrorCount, forbiddenTime);
				}
				else
				{
					Session[WebConfig.SessionLoginErrorCount] = loginErrorCount;
					errorMsg.AppendFormat(StringDef.UserNameOrPasswordError, loginErrorCount, MaxLoginErrorCount);
				}

				LabelLoginError.Text = errorMsg.ToString();
				LabelLoginError.Visible = true;
			}
		}
		catch (SecurityException ex)
		{
			LabelLoginError.Text = ex.Message;
			LabelLoginError.Visible = true;
		}		
	}
}
