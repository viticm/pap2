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
using FSEye;
using System.IO;
using Resources;
using FSEye.Security;
using FSEye.WebControls;

public partial class Customization_cabinet : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!Page.IsPostBack)
        {
            if (!WebUtil.CheckPrivilege(WebConfig.FunctionCustomizationCabinet, OpType.READ, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }

            foreach (string cabinetName in AdminServer.TheInstance.GameServerManager.CabinetList)
            {
                if (cabinetName != string.Empty)
                    DropDownListCabinet.Items.Add(cabinetName);
            }
            InitGroupList();
            if (DropDownListCabinet.Items.Count > 0)
            {
                DropDownListCabinet.SelectedIndex = 0;
                InitCabinetList();
            }

        }
    }

    private void InitCabinetList()
    {
        ListBoxCabinet.Items.Clear();
        ListBoxGroup.Items.Clear();
        if (DropDownListCabinet.Items.Count > 0)
        {
            foreach (ServerGroup group in AdminServer.TheInstance.GameServerManager.TopServerGroups)
            {
                AddServerGroup(group,DropDownListCabinet.SelectedItem.Text);
            }
        }
    }


    private void InitGroupList()
    {
        ListBoxCabinet.Items.Clear();
        ListBoxGroup.Items.Clear();
        foreach (ServerGroup group in AdminServer.TheInstance.GameServerManager.TopServerGroups)
        {
            AddServerGroup(group,null);
        }
    }

    private void AddServerGroup(ServerGroup group,string cabinetName)
    {
        if (group == null) return;

        if (group.GroupType == ServerGroup.Type.Server)
        {
            if (group.Cabinet != string.Empty)
            {                
                if (cabinetName != string.Empty && cabinetName == group.Cabinet)
                {
                    ListBoxCabinet.Items.Add(new ListItem(group.Name, group.Id.ToString()));
                }
            }
            else
            {
                ListBoxGroup.Items.Add(new ListItem(group.Name, group.Id.ToString()));
            }
        }
        else
        {
            foreach (ServerGroup childGroup in group.List)
            {
                if (childGroup.GroupType == ServerGroup.Type.Server)
                {
                    if (childGroup.Cabinet != string.Empty)
                    {                       
                        if (cabinetName != string.Empty && cabinetName == childGroup.Cabinet)
                        {
                            ListBoxCabinet.Items.Add(new ListItem(childGroup.Name, childGroup.Id.ToString()));
                        }
                    }
                    else
                    {
                        ListBoxGroup.Items.Add(new ListItem(childGroup.Name, childGroup.Id.ToString()));
                    }
                }
                else
                {
                    AddServerGroup(childGroup, cabinetName);
                }
            }
        }
    }

    protected void ButtonInsert_Click(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (ListBoxGroup.Items.Count <= 0) return;
        if (ListBoxGroup.SelectedValue == string.Empty) return;

        int goupId = int.Parse(ListBoxGroup.SelectedValue);
        ServerGroup group = AdminServer.TheInstance.GameServerManager.GetGameServerGroup(goupId);
        group.SetGroupCabinet(DropDownListCabinet.SelectedValue);

        InitCabinetList();
    }

    protected void ButtonRemove_Click(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (ListBoxCabinet.Items.Count <= 0) return;
        if (ListBoxCabinet.SelectedValue == string.Empty) return;

        int goupId = int.Parse(ListBoxCabinet.SelectedValue);
        ServerGroup group = AdminServer.TheInstance.GameServerManager.GetGameServerGroup(goupId);
        group.SetGroupCabinet(String.Empty);

        InitCabinetList();
    }

    protected void DropDownListCabinet_SelectedIndexChanged(object sender, EventArgs e)
    {
        InitCabinetList();
    }
}
