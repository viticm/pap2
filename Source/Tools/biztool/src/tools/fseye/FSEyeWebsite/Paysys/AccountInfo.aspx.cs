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
using Resources;
using FSEye.PaySysLib;
using FSEye.PlugIn;
using FSEye;
using FSEye.Security;

public partial class Paysys_AccountInfo : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionPaysysAccountInfo, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }
        
        if (Request.Params[WebConfig.ParamAccount] != null)
        {
            TextBoxInputAccount.Text = Request.Params[WebConfig.ParamAccount];
        }

        if (!IsPostBack)
        {
            //添加确认提示
            LinkButtonFreeze.Attributes.Add("onclick",
                string.Format("return msgConfirm('{0}');", string.Format(StringDef.MsgConfirm, StringDef.Freeze + StringDef.Account)));
            LinkButtonUnFreeze.Attributes.Add("onclick",
                string.Format("return msgConfirm('{0}');", string.Format(StringDef.MsgConfirm, StringDef.Unfreeze + StringDef.Account)));
            LinkButtonSavePassword.Attributes.Add("onclick",
                string.Format("return msgConfirm('{0}');", string.Format(StringDef.MsgConfirm, StringDef.SavePassword)));
            LinkButtonResetPassword.Attributes.Add("onclick",
                string.Format("return msgConfirm('{0}');", string.Format(StringDef.MsgConfirm, StringDef.ResetPassword)));
        }

        LabelOpMsg.Text = string.Empty;
        LabelOpResult.Text = string.Empty;

    }

    /// <summary>
    /// 查询帐号信息
    /// </summary>
    protected void LinkButtonQuery_Click(object sender, EventArgs e)
    {
        string accountName = TextBoxInputAccount.Text.Trim();
        if (accountName.Length == 0)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Account);
            return;
        }
        string zoneName = PaysysZoneList.SelectedZone;
        AccountInfo accountInfo = TheAdminServer.PaySysAgent.GetAccountInfo(accountName, zoneName);
        if (accountInfo == null)
        {
            LabelOpMsg.Text = StringDef.MsgAccountNotExist;
            PanelResult.Visible = false;
            return;
        }
        ShowAccountInfo(accountInfo);
        PanelResult.Visible = true;
    }

    private void ShowAccountInfo(AccountInfo accountInfo)
    {
        LiteralResult.Text = string.Format("[{0}]{1} {2}", accountInfo.ZoneName, accountInfo.AccountName,StringDef.AccountInfo);
        TextAccount.Text = accountInfo.AccountName;
        TextZoneName.Text = accountInfo.ZoneName;
        TextEndDate.Text = accountInfo.EndDate.ToString();
        TextLeftSecond.Text = accountInfo.LeftSecond.ToString();
        TextLastLoginTime.Text = accountInfo.LastLoginTime.ToString();
        TextLastLoginIP.Text = accountInfo.LastLoginIP.ToString();
        TextLastLogoutTime.Text = accountInfo.LastLogoutTime.ToString();
        TextLeftCoin.Text = accountInfo.LeftCoin.ToString();
        TextLeftSecond.Text = accountInfo.LeftSecond.ToString();
        TextActiveIP.Text = accountInfo.ActiveIP.ToString();
        TextActiveTime.Text = accountInfo.ActiveTime.ToString();
        TextActiveType.Text = accountInfo.ActiveType.ToString();
        TextExtPoint0.Text = accountInfo.ExtPoint0.ToString();
        TextExtPoint1.Text = accountInfo.ExtPoint1.ToString();
        TextExtPoint2.Text = accountInfo.ExtPoint2.ToString();
        TextExtPoint3.Text = accountInfo.ExtPoint3.ToString();
        TextExtPoint4.Text = accountInfo.ExtPoint4.ToString();
        TextExtPoint5.Text = accountInfo.ExtPoint5.ToString();
        TextExtPoint6.Text = accountInfo.ExtPoint6.ToString();
        TextExtPoint7.Text = accountInfo.ExtPoint7.ToString();
        TextState.Text = TheAdminServer.PaySysAgent.GetAccountState(accountInfo.AccountName).ToString();
        TextGatewayInfo.Text = TheAdminServer.PaySysAgent.GetGatewayByAccount(accountInfo.AccountName);

        HyperLinkSetPassword.NavigateUrl = string.Format("~/PaySys/AccountPassword.aspx?{0}={1}",
            WebConfig.ParamAccount, accountInfo.AccountName);
    }

    protected void LinkButtonFreeze_Click(object sender, EventArgs e)
    {
        LinkButton lb = sender as LinkButton;
        if(lb == null)return;

        if (TextAccount.Text == null || TextAccount.Text.Length == 0)
        {
            return;
        }
        FSEye.PaySysLib.OperationResult result;
        switch (lb.CommandName)
        {
            case "Freeze":
                result = GMUtil.SetAccountFrozen(CurrentUser.Id, TextAccount.Text, true);
                if (result != FSEye.PaySysLib.OperationResult.Success)
                {
                    LabelOpResult.Text = result.ToString();
                }
                else
                {
                    //更新状态信息
                    TextState.Text = TheAdminServer.PaySysAgent.GetAccountState(TextAccount.Text).ToString();
                    string gatewayInfo = TheAdminServer.PaySysAgent.GetGatewayByAccount(TextAccount.Text);
                    TextGatewayInfo.Text = gatewayInfo;

                    //如果Gateway信息不是Offline则得到其serverId然后对其上角色进行强制退出操作
                    if (!gatewayInfo.Equals("Offline", StringComparison.OrdinalIgnoreCase))
                    {
                        GameServer server = GMUtil.GetAccountOnlineServer(CurrentUser.Id, TextAccount.Text);
                        if (server != null)
                        {
                            IList roleNameList = GMUtil.GetRoleNameByAccount(CurrentUser.Id, server, TextAccount.Text, true);
                            if (roleNameList.Count != 0)
                            {
                                foreach (string roleName in roleNameList)
                                {
                                    server.DoPlugInAction(CurrentUser.Id, LordControl.PlugInGuid, LordControl.ActionKeyExecuteGMCommand, roleName, WebConfig.GMCommandKickPlayer);
                                }
                            }
                        }
                    }
                }
                break;
            case "UnFreeze":
                result = GMUtil.SetAccountFrozen(CurrentUser.Id, TextAccount.Text, false);
                if (result != FSEye.PaySysLib.OperationResult.Success)
                {
                    LabelOpResult.Text = result.ToString();
                }
                else
                {
                    //更新状态信息
                    TextState.Text = TheAdminServer.PaySysAgent.GetAccountState(TextAccount.Text).ToString();
                    string gatewayInfo = TheAdminServer.PaySysAgent.GetGatewayByAccount(TextAccount.Text);
                    TextGatewayInfo.Text = gatewayInfo;
                }
                break;
        }
    }

    protected void LinkButtonPassword_Click(object sender, EventArgs e)
    {
        LinkButton lb = sender as LinkButton;
        if (lb != null)
        {
            string accountName;
            string accountPassword;
            switch (lb.CommandName)
            {
                case "SavePass":
                    accountName = TextAccount.Text;
                    accountPassword = TheAdminServer.PaySysAgent.GetPassword(accountName);
                    if (accountPassword == null || accountPassword.Length == 0)
                    {
                        LabelOpResult.Text = StringDef.OperationFail + StringDef.Colon + StringDef.SavePassword;
                        return;
                    }
                    if (!TheAdminServer.PaysysManager.SaveAccountPassword(accountName, accountPassword))
                    {
                        LabelOpResult.Text = StringDef.OperationFail + StringDef.Colon + StringDef.SavePassword;
                    }
                    else
                    {
                        LabelOpResult.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.SavePassword;
                    }
                    break;
                case "ResetPass":
                    accountName = TextAccount.Text;
                    accountPassword = TheAdminServer.PaysysManager.GetSavedPassword(accountName);
                    if (accountPassword == null || accountPassword.Length == 0)
                    {
                        LabelOpResult.Text = StringDef.OperationFail + StringDef.Colon + StringDef.ResetPassword;
                        return;
                    }
                    FSEye.PaySysLib.OperationResult result = TheAdminServer.PaySysAgent.SetPassword(accountName, accountPassword, true); 
                    if (result != FSEye.PaySysLib.OperationResult.Success)
                    {
                        LabelOpResult.Text = StringDef.OperationFail + StringDef.Colon + result.ToString();
                    }
                    else
                    {
                        LabelOpResult.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.ResetPassword;
                    }
                    break;
            }
        }
    }
}