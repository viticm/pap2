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
using System.IO;
using System.Globalization;
using System.Drawing;
using System.Text;
using FSEye;
using FSEye.DAL;
using FSEye.Security;

public partial class Common_Main : System.Web.UI.MasterPage
{
	protected void Page_Init(object sender, EventArgs e)
	{
		try
		{
			if (Session[WebConfig.SessionCurrentUser] == null)
			{
				Response.Redirect(WebConfig.PageLogin, true);
			}
			else
			{
				UserInfo userInfo = Session[WebConfig.SessionCurrentUser] as UserInfo;
				if (!AdminServer.TheInstance.SecurityManager.ValidateLoginUser(userInfo.Id, userInfo.LoginId))
				{
					Session.Remove(WebConfig.SessionCurrentUser);
					Response.Redirect(WebConfig.PageLogin, true);
				}
			}
		}
		catch (Exception)
		{
			Response.Redirect(WebConfig.PageLogin, true);
		}
	}

	protected void Page_Load(object sender, EventArgs e)
	{
		UserInfo userInfo = Session[WebConfig.SessionCurrentUser] as UserInfo;
		LabelPageCreateTime.Text = Resources.StringDef.PageCreateTime + ": " + DateTime.Now.ToString();
		LiteralUserName.Text = userInfo.RealName;

        LabelWebVersion.Text = "ÍøÕ¾°æ±¾ºÅ:" + SystemConfig.Current.WebSiteVersion;
        
        
		if (WebUtil.CheckPrivilege(AdminServer.TheInstance.SecurityManager.Root, OpType.EXECUTE, Session))
		{
			LinkSystemControl.Visible = true;
		}
		else
		{
			LinkSystemControl.Visible = false;
		}

		if (Request.Params["fromSelf"] == null)
		{
			if (!AdminServer.TheInstance.IsRunning)
			{
				Response.Redirect("~/Common/ServiceUnavaiable.aspx?fromSelf=true");
			}
		}

		bool showSiteMap = true;
		if (Session[WebConfig.SessionShowSiteMap] != null)
			showSiteMap = (bool)Session[WebConfig.SessionShowSiteMap];
		ShowSiteMap(showSiteMap);
	}
    
	public void ButtonHideSiteMap_Click(object sender, EventArgs e)
	{
		ShowSiteMap(false);
	}

	public void ButtonShowSiteMap_Click(object sender, EventArgs e)
	{
		ShowSiteMap(true);
	}

	void ShowSiteMap(bool show)
	{
        Session[WebConfig.SessionShowSiteMap] = show;
        PanelSiteMap.Visible = show;
        ButtonShowSiteMap.Visible = !show;
	}

	protected void ButtonLogout_Click(object sender, EventArgs e)
	{
		if (Session[WebConfig.SessionCurrentUser] != null)
		{
			try
			{
				UserInfo info = Session[WebConfig.SessionCurrentUser] as UserInfo;
				AdminServer.TheInstance.SecurityManager.Logout(info.Id);
				Session.Remove(WebConfig.SessionCurrentUser);
				Session.Abandon();
			}
			catch (Exception)
			{
				Response.Redirect(WebConfig.PageLogin);
			}
		}

		Response.Redirect(WebConfig.PageHome);
	}
}
