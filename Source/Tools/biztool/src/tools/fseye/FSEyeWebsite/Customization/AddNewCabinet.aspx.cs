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
using FSEye.Security;
using FSEye;

public partial class Customization_AddNewCabinet : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!Page.IsPostBack)
        {
            if (!WebUtil.CheckPrivilege(WebConfig.FunctionCustomizationAddNewCabinet, OpType.READ, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }

            foreach(string cabinetName in AdminServer.TheInstance.GameServerManager.CabinetList)
            {
                if (cabinetName != String.Empty)
                    ListBoxCabinet.Items.Add(cabinetName);
            }
        }
        
        LabelSuccess.Visible = false;
        LabelOpMsg.Visible = false;
    }

    protected void LinkButtonDeleteCabinet_Click(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
        {
            LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            LabelOpMsg.Visible = true;
            return;
        }

        if (ListBoxCabinet.SelectedItem == null)
        {
            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + "未选中要删除的机柜";
            LabelOpMsg.Visible = true;
            return;
        }

        string cabinetName = ListBoxCabinet.SelectedItem.Text;

        try
        {
            if (AdminServer.TheInstance.GameServerManager.CabinetList.Contains(cabinetName))
            {
                AdminServer.TheInstance.GameServerManager.CabinetList.Remove(cabinetName);
                ListItem item = ListBoxCabinet.Items.FindByText(cabinetName);
                ListBoxCabinet.Items.Remove(item);

                foreach (ServerGroup group in AdminServer.TheInstance.GameServerManager.ServerGroups)
                {
                    if (group.Cabinet == cabinetName)
                        group.SetGroupCabinet(String.Empty);
                }

                LabelSuccess.Text = "操作成功：删除机柜 " + cabinetName;
                LabelSuccess.Visible = true;
            }
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + ex.Message;
            LabelOpMsg.Visible = true;
            return;
        }
    }

    protected void LinkButtonAddCabinet_Click(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
        {
            LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            LabelOpMsg.Visible = true;
            return;
        }

        string newCabinetName = TextBoxAddCabinet.Text.Trim();

        try
        {
            if (newCabinetName != String.Empty)
            {
                if (AdminServer.TheInstance.GameServerManager.CabinetList.Contains(newCabinetName))
                {
                    LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + "机柜" + newCabinetName + "已存在";
                    LabelOpMsg.Visible = true;
                    return;
                }
                else
                {
                    AdminServer.TheInstance.GameServerManager.CabinetList.Add(newCabinetName);
                    ListBoxCabinet.Items.Add(newCabinetName);

                    LabelSuccess.Text = "操作成功：添加机柜 " + newCabinetName;
                    LabelSuccess.Visible = true;
                }
            }
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + ex.Message;
            LabelOpMsg.Visible = true;
            return;
        }
    }

    protected void LinkButtonRenameCabinet_Click(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
        {
            LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            LabelOpMsg.Visible = true;
            return;
        }

        if (ListBoxCabinet.SelectedItem == null)
        {
            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + "未选中要重命名的机柜";
            LabelOpMsg.Visible = true;
            return;
        }

        string cabinetName = ListBoxCabinet.SelectedItem.Text;

        string newCabinetName = TextBoxRenameCabinet.Text.Trim();

        try
        {
            if (newCabinetName != String.Empty)
            {
                if (AdminServer.TheInstance.GameServerManager.CabinetList.Contains(newCabinetName))
                {
                    LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + "机柜" + newCabinetName + "已存在";
                    LabelOpMsg.Visible = true;
                    return;
                }
                else
                {
                    int index = AdminServer.TheInstance.GameServerManager.CabinetList.IndexOf(cabinetName);
                    AdminServer.TheInstance.GameServerManager.CabinetList[index] = newCabinetName;

                    ListItem item = ListBoxCabinet.Items.FindByText(cabinetName);
                    if (item != null)
                        item.Text = newCabinetName;

                    foreach (ServerGroup group in AdminServer.TheInstance.GameServerManager.ServerGroups)
                    {
                        if (group.Cabinet == cabinetName)
                            group.SetGroupCabinet(newCabinetName);
                    }

                    LabelSuccess.Text = "操作成功：重命名机柜 " + cabinetName + " 为 " + newCabinetName;
                    LabelSuccess.Visible = true;
                }
            }
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + ex.Message;
            LabelOpMsg.Visible = true;
            return;
        }
    }
}
