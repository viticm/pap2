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
using FSEye.FS2Lib;
using FSEye;
using FSEye.Security;

public partial class GameMaster_SendMail : BasePage
{
	protected void Page_Load(object sender, EventArgs e)
	{
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterSendMail, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
        }
        LabelOpMsg.Text = string.Empty;
	}
    
    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }

    protected void ButtonSearchItemId_Click(object sender, EventArgs e)
    {
        RadioButtonListItemId.Items.Clear();
        string itemName = TextBoxItemName.Text;
        if (itemName == null || itemName.Trim().Length == 0)
        {
            LabelSearchId.Visible = true;
            LabelSearchId.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.ItemName);
            return;
        }
        IList items = FS2GameDataManager.TheInstance.GetItemId(itemName);
        if (items != null && items.Count != 0)
        {
            foreach (FS2ItemData item in items)
            {
                ListItem listItem = new ListItem(string.Format("{0}[{1}]", item.Name, item.TemplateId), item.TemplateId);
                RadioButtonListItemId.Items.Add(listItem);
            }
            LabelSearchId.Visible = false;
        }
        else
        {
            LabelSearchId.Visible = true;
            LabelSearchId.Text = StringDef.NoInfo;
        }
    }

    protected void CheckBoxItem_CheckedChanged(object sender, EventArgs e)
    {
        //TableItem.Enabled = CheckBoxItem.Checked;
        TableItem.Visible = CheckBoxItem.Checked;
    }

    protected void SelectedItemIdChanged(object sender, EventArgs e)
    {
        if (RadioButtonListItemId.SelectedIndex != -1)
            TextBoxItemId.Text = RadioButtonListItemId.SelectedValue;
    }

    protected void LinkButtonSendMail_Click(object sender, EventArgs e)
    {
        try
        {
            ArrayList paramList = new ArrayList();
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
            string receiver = TextBoxInputReceiver.Text.Trim();
            if (receiver == string.Empty)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Receiver);
                return;
            }
            paramList.Add(receiver);

            string subject = TextBoxInputSubject.Text.Trim();
            if (subject == string.Empty)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Subject);
                return;
            }
            paramList.Add(subject);

            string content = TextBoxInputContent.Text.Trim();
            paramList.Add(content);

            int costMoney = int.Parse(TextBoxInputCostMoney.Text.Trim());
            int postMoney = int.Parse(TextBoxInputPostMoney.Text.Trim());

            paramList.Add(costMoney);
            paramList.Add(postMoney);

            //物品相关
            if (CheckBoxItem.Checked)
            {
                string itemCountText = TextBoxInputItemCount.Text.Trim();
                if (itemCountText.Length == 0)
                {
                    LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Count);
                    return;
                }
                int itemCount = int.Parse(itemCountText);
                string[] itemIds = TextBoxItemId.Text.Trim().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                if (itemIds.Length != 4)
                {
                    LabelOpMsg.Text = StringDef.ParameterInputError + StringDef.Colon + StringDef.ItemID;
                    return;
                }
                paramList.Add(itemIds[0]);
                paramList.Add(itemIds[1]);
                paramList.Add(itemIds[2]);
                paramList.Add(itemIds[3]);

                paramList.Add(itemCount);
            }

            if (GMUtil.SendMail(CurrentUser.Id, server.Id, paramList.ToArray()))
                LabelOpMsg.Text = StringDef.SendSuccess;
            else
                LabelOpMsg.Text = StringDef.SendFail;
        }
        catch (FormatException)
        {
            LabelOpMsg.Text = StringDef.ParameterInputError;
            return;
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = ex.Message;
            return;
        }
    }
}
