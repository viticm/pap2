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
using System.Collections.Generic;
using System.Text;
using Resources;
using FSEye;
using FSEye.Security;
using FSEye.PlugIn;

public partial class GameMaster_ChatMonitor : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterChatMonitor, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }
		
        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
        }
		
        LabelOpMsg.Text = string.Empty;
    }

    protected void SelectedServerGroupChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }
    
    protected void LinkButtonSendMsg_Click(object sender, EventArgs e)
    {
        SendMessage();
    }

    private void SendMessage()
    {
        string msg = TextBoxMessage.Text.Trim();
        if (msg == null || msg.Length == 0)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Message);
            return;
        }
        GameServer server = ServerDropDownList.SelectedGameServer;
        if (server == null)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
            return;
        }
        if (!server.IsConnected)
        {
            LabelOpMsg.Text = StringDef.NoConnectionAlert;
            return;
        }
        if (server.DoPlugInAction(CurrentUser.Id, LordControl.PlugInGuid, LordControl.ActionKeySayToWorld, msg))
            LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.SendMessage;
        else
            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.SendMessage;
    }

    //protected void ServerDropDownList_IndexChanged(object sender, EventArgs e)
    //{
    //    GameServer server = ServerDropDownList.SelectedGameServer;
    //    if (server != null && server.IsConnected)
    //        server.DoPlugInAction(CurrentUser.Id, LordControl.PlugInGuid, LordControl.ActionKeyMonitorGlobalChat, true);
    //}

    //protected void LinkButton_Click(object sender, EventArgs e)
    //{
    //    GameServer server = ServerDropDownList.SelectedGameServer;
    //    if (server != null)
    //    {
    //        switch ((sender as LinkButton).CommandName)
    //        {
    //            case "Start":
    //                if (server.DoPlugInAction(CurrentUser.Id, LordControl.PlugInGuid, LordControl.ActionKeyMonitorGlobalChat, true))
    //                {
    //                    LabelOpMsg.Text = StringDef.StartMonitor;
    //                    //TimerRefresh.Interval = 5000;
    //                    //TimerRefresh.Enabled = true;
    //                }
    //                else
    //                    LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.StartMonitor;
    //                break;
    //            case "Stop":
    //                if (server.DoPlugInAction(CurrentUser.Id, LordControl.PlugInGuid, LordControl.ActionKeyMonitorGlobalChat, false))
    //                {
    //                    LabelOpMsg.Text = StringDef.StopMonitor;
    //                    //TimerRefresh.Enabled = false;
    //                }
    //                else
    //                    LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.StopMonitor;
    //                break;
    //            //case "Clear":
    //            //    DivChatContent.InnerHtml = string.Empty;
    //            //    break;
    //        }            
    //    }
    //}

    //protected void RefreshGameMessage()
    //{
    //    GameServer server = ServerDropDownList.SelectedGameServer;
    //    if (server != null)
    //    {
    //        Queue<LordControl.GameMessage> gameMessageQueue = server.GetPlugInData(CurrentUser.Id, LordControl.PlugInGuid, LordControl.DataKeyGameMessageQueue) as Queue<LordControl.GameMessage>;
    //        if (gameMessageQueue != null && gameMessageQueue.Count != 0)
    //        {
    //            DivChatContent.InnerHtml = GameDataRender.RenderChatMsg(gameMessageQueue, CheckBoxOriginalData.Checked, server.Id);                
    //        }
    //    }
    //}

    //protected void TimerRefresh_Tick(object sender, EventArgs e)
    //{
    //    RefreshGameMessage();
    //}
}