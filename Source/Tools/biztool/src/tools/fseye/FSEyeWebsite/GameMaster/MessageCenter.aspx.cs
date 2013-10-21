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
using FSEye.PlugIn;

public partial class GameMaster_MessageCenter : BasePage
{
	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterMessageCenter, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}
        if (!IsPostBack)
        {
            //ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
        }
        LabelOpMsg.Text = string.Empty;
	}

	protected void ButtonSendMessage_Click(object sender, EventArgs e)
	{
        GameServer[] selectedServers = GameServerCheckBoxList.SelectedGameServers;
        if (selectedServers == null || selectedServers.Length == 0)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
            return;   
        }

        string msg = TextBoxMessage.Text.Trim();
        if (msg == null || msg.Length == 0)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Message);
            return;
        }

        int totalSucessCount = 0;
        StringBuilder failMsg = new StringBuilder();
        foreach (GameServer server in selectedServers)
        {
            if (server.DoPlugInAction(CurrentUser.Id, LordControl.PlugInGuid, LordControl.ActionKeySayToWorld, msg))
            {
                ++totalSucessCount;
            }
            else
            {
                failMsg.AppendFormat(" [{0}]", server.Name);
            }
        }
        LabelOpMsg.Text = StringDef.SendSuccess + StringDef.Colon + string.Format("{0}/{1}", totalSucessCount, selectedServers.Length);
        if (failMsg.Length != 0)
        {
            LabelOpMsg.Text += "<br />";
            LabelOpMsg.Text += StringDef.SendFail + StringDef.Colon + StringDef.GameServer + failMsg.ToString();
        }
	}
}