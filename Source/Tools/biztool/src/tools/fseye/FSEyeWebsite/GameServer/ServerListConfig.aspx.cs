using System;
using System.Data;
using System.Configuration;
using System.Collections;
using System.Collections.Generic;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Text;
using System.Drawing;
using Resources;
using FSEye.Security;
using FSEye;

public partial class GameServer_ServerListConfig : BasePage
{
    private const int MaxNetGroupCount = 20;
    private const int MaxRegionGroupCount = 5;

    public static Color[] DefaultNetColor = new Color[]{
        Color.White,
        Color.LightGray,
        Color.GhostWhite,
        Color.Fuchsia,
        Color.FromArgb(200,169,112),        
        Color.DarkCyan,        
        Color.Ivory,
        Color.HotPink,
        Color.LightBlue,
        Color.LightSkyBlue,
        Color.Bisque
    };

    public static Color[] DefaultRegionColor = new Color[]{        
        Color.FromArgb(194,158,27),
        Color.FromArgb(48,133,133),
        Color.FromArgb(180,180,180),
        Color.FromArgb(27,138,194),
        Color.FromArgb(51,77,102),//"#334D66";
        Color.FromArgb(128,125,0),//#807D00";
        Color.Ivory,
        Color.Lime,        
        Color.Orange,
        Color.Plum,        
        Color.Olive,
        Color.Bisque,
        Color.Fuchsia,
        Color.Crimson,
        Color.Brown,        
        Color.Gray,        
        Color.BlueViolet,        
        Color.Aqua,        
        Color.ForestGreen,
        Color.GhostWhite,
        Color.Tomato
    };

    List<MapNode> _netList = null;
    bool _isModified = false;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerServerListConfig, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }       

        if (!IsPostBack)
        {
            LoadServerMapList();
            BindTable();
            BindTableNet();
            BindTableRegion();

            //Common_ServerGroupDropDownList serverGroupDropDownList = (Common_ServerGroupDropDownList)DataGridServerMap.FindControl("DropDownListServerGroup");
            //serverGroupDropDownList.Refresh();
        }

        _netList = Session["Store"] as List<MapNode>;
        if (ViewState["Modify"] != null)
        {
            _isModified = (bool)ViewState["Modify"];
        }

        TotalReport();

        ClearLabelMessage();
    }

    private void SetModified(bool modified)
    {
        ViewState["Modify"] = modified;
        if (modified)
        {
            LabelTitle.Text = StringDef.ServerListConfig + string.Format("<span style='color:red;'>{0}</span>", StringDef.NotSaveYet);
        }
        else
        {
            LabelTitle.Text = StringDef.ServerListConfig;
        }        
    }

    private void TotalReport()
    {
        if (_netList != null)
        {
            int totalNetCount = _netList.Count;
            int totalRegionCount = 0;
            int totalServerCount = 0;
            foreach (MapNode net in _netList)
            {
                totalRegionCount += net.Children.Length;
                foreach (MapNode region in net.Children)
                {
                    totalServerCount += region.Children.Length;
                }
            }

            LabelTotalReport.Text = string.Format(StringDef.ServerListReportFormatString, totalNetCount, totalRegionCount, totalServerCount);
        }
    }

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        Common_ServerGroupDropDownList DropDownListServerGroup = DataGridServerMap.Items[DataGridServerMap.EditItemIndex].Cells[5].FindControl("DropDownListServerGroup") as Common_ServerGroupDropDownList;
        Common_ServerDropDownList DropDownListServer = DataGridServerMap.Items[DataGridServerMap.EditItemIndex].Cells[5].FindControl("DropDownListServer") as Common_ServerDropDownList;
        CheckBoxList CheckBoxListServerFlag = DataGridServerMap.Items[DataGridServerMap.EditItemIndex].Cells[4].FindControl("CheckBoxListServerFlag") as CheckBoxList;

        ServerGroup group = DropDownListServerGroup.SelectedServerGroup;
        if (group != null)
        {
            DropDownListServer.CreateServerList(group, GameServer.ServerType.bishop);
            
            SetFlagCheckBoxListByGroupEnabled(CheckBoxListServerFlag, group);
        }
    }

    private Color SwitchNetColor(Color currColor)
    {
        if (currColor == DefaultNetColor[0])
        {
            return DefaultNetColor[1];
        }
        else
        {
            return DefaultNetColor[0];
        }
    }

    private Color SwitchRegionColor(Color currColor)
    {
        if (currColor == DefaultRegionColor[0])
        {
            return DefaultRegionColor[1];
        }
        else
        {
            return DefaultRegionColor[0];
        }
    }

    private void LoadServerMapList()
    {
        try
        {
            //重新load一次 在此页面修改的List不影响到内存里的
            if (!TheAdminServer.GameServerManager.ServerMap.LoadServerMap(ref _netList))
            {
                ShowLabelMessage(StringDef.MsgLoadServerListError, MessageType.Failure);
            }

            if (_netList != null)
            {
                Session["Store"] = _netList;
                SetModified(false);
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    private void CreateFlagCheckBoxList(CheckBoxList list, int nodeFlag)
    {
        if (list != null)
        {
            foreach (ServerMap.ServerMapFlag flag in (ServerMap.ServerMapFlag[])Enum.GetValues(typeof(ServerMap.ServerMapFlag)))
            {
                switch (flag)
                {
                    case ServerMap.ServerMapFlag.Hide:
                        {
                            ListItem item = new ListItem(StringDef.Hide, ((int)flag).ToString());
                            if ((nodeFlag & (int)flag) > 0)
                            {
                                item.Selected = true;
                            }
                            list.Items.Add(item);
                        }
                        break;
                    case ServerMap.ServerMapFlag.New:
                        {
                            ListItem item = new ListItem(StringDef.NewBlown, ((int)flag).ToString());
                            if ((nodeFlag & (int)flag) > 0)
                            {
                                item.Selected = true;
                            }
                            list.Items.Add(item);
                        }
                        break;
                    case ServerMap.ServerMapFlag.Recommend:
                        {
                            ListItem item = new ListItem(StringDef.Recommend, ((int)flag).ToString());
                            if ((nodeFlag & (int)flag) > 0)
                            {
                                item.Selected = true;
                            }
                            list.Items.Add(item);
                        }
                        break;
                }
            }
        }
    }

    private int GetFlagFromCheckBoxList(CheckBoxList CheckBoxListServerFlag)
    {
        int flag = 0;
        if (CheckBoxListServerFlag != null)
        {
            foreach (ListItem item in CheckBoxListServerFlag.Items)
            {
                if (item.Selected)
                {
                    flag |= int.Parse(item.Value);
                }
            }
        }
        return flag;
    }

    private string GetFlagDesc(int flag)
    {
        StringBuilder flagDescSb = new StringBuilder();
        if ((flag & (int)ServerMap.ServerMapFlag.New) > 0)
        {
            flagDescSb.Append(StringDef.NewBlown);
            flagDescSb.Append(' ');
        }
        if ((flag & (int)ServerMap.ServerMapFlag.Hide) > 0)
        {
            flagDescSb.Append(StringDef.Hide);
            flagDescSb.Append(' ');
        }
        if ((flag & (int)ServerMap.ServerMapFlag.Recommend) > 0)
        {
            flagDescSb.Append(StringDef.Recommend);
            flagDescSb.Append(' ');
        }
        if (flagDescSb.Length > 0)
        {
            return flagDescSb.ToString(0, flagDescSb.Length - 1);
        }
        else
        {
            return StringDef.None;
        }
    }

    private ICollection CreateDataSource()
    {
        try
        {
            DataTable dataTable = new DataTable();
            DataRow dataRow = null;
            dataTable.Columns.Add(new DataColumn("Id", typeof(Int32)));
            dataTable.Columns.Add(new DataColumn("Net", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Region", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Name", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Flag", typeof(String)));
            dataTable.Columns.Add(new DataColumn("ServerName", typeof(String)));
            dataTable.Columns.Add(new DataColumn("ServerIp", typeof(String)));
            dataTable.Columns.Add(new DataColumn("FlagDesc", typeof(String)));            

            _netList = Session["Store"] as List<MapNode>;
            if (_netList != null)
            {
                Color netColor = DefaultNetColor[0];
                Color regionColor = DefaultRegionColor[0];
                for (int netIndex = 0; netIndex < _netList.Count; ++netIndex)
                {
                    MapNode net = _netList[netIndex];
                    Color currNetColor = netColor;
                    string netColorDesc = WebUtil.GetColorWebStyle(currNetColor);
                    string currNetName = null;
                    for (int regionIndex = 0; regionIndex < net.Children.Length; ++regionIndex)
                    {
                        MapNode region = net.Children[regionIndex];
                        Color currRegionColor = regionColor;
                        string regionColorDesc = WebUtil.GetColorWebStyle(currRegionColor);
                        for (int serverIndex = 0; serverIndex < region.Children.Length; ++serverIndex)
                        {
                            MapNode mapServer = region.Children[serverIndex];
                            dataRow = dataTable.NewRow();
                            dataRow[0] = mapServer.ID;

                            string netName = null;
                            if (currNetName == null)
                            {
                                netName = net.Name;
                                currNetName = net.Name;
                            }
                            else
                            {
                                netName = string.Empty;
                            }
                            dataRow[1] = net.Name;//string.Format("<div style='background-color:{0};text-align:center;height:100%;'><div style='padding:5px;'>{1}</div></div>", netColorDesc, net.Name);//netName);
                            dataRow[2] = region.Name;// string.Format("<div style='background-color:{0};text-align:center;height:100%;'><div style='padding:5px;'>{1}</div></div>", regionColorDesc, region.Name);//serverIndex == 0 ? region.Name : string.Empty);
                            dataRow[3] = mapServer.Name;
                            dataRow[4] = mapServer.Flag.ToString();
                            GameServer server = TheAdminServer.GameServerManager.GetGameServer(mapServer.MapServerId);
                            if (server != null)
                            {
                                dataRow[5] = string.Format("[{0}]{1}", server.Group.Name, server.Name);
                                dataRow[6] = server.IpAddress.ToString();
                            }
                            else
                            {
                                dataRow[5] = StringDef.NotAvailable;
                                dataRow[6] = StringDef.NotAvailable;
                            }
                            dataRow[7] = GetFlagDesc(mapServer.Flag);

                            dataTable.Rows.Add(dataRow);

                            if (netColor == currNetColor)
                            {
                                netColor = SwitchNetColor(netColor);
                            }
                            if (currRegionColor == regionColor)
                            {
                                regionColor = SwitchRegionColor(regionColor);
                            }
                        }
                    }
                }
            }

            return dataTable.DefaultView;
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            return null;
        }
    }

    private void BindTable()
    {
        try
        {
            DataGridServerMap.DataSource = CreateDataSource();
            DataGridServerMap.DataBind();
        }
        catch(Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    #region ServerMap
    protected void DataGridServerMap_DeleteCommand(Object source, DataGridCommandEventArgs e)
    {
        try
        {
            MapNode selNode = GetMapNode(int.Parse(e.Item.Cells[0].Text), _netList);
            if (selNode != null)
            {
                selNode.Parent.RemoveChild(selNode);

                SetModified(true);

                BindTable();
                TotalReport();
            }
            else
            {
                ShowLabelMessage(string.Format(StringDef.MsgCannotBeNone, StringDef.Server), MessageType.Failure);
            }
            DataGridServerMap.EditItemIndex = -1;
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());

            ShowLabelMessage(StringDef.OperationFail, MessageType.Failure);
        }
    }

    protected void DataGridServerMap_CancelCommand(Object source, DataGridCommandEventArgs e)
    {
        DataGridServerMap.EditItemIndex = -1;
        BindTable();
    }

    protected void DataGridServerMap_EditCommand(Object source, DataGridCommandEventArgs e)
    {
        try
        {
            DataGridServerMap.EditItemIndex = e.Item.ItemIndex;
            BindTable();
            SetInfo(DataGridServerMap.Items[DataGridServerMap.EditItemIndex]);
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            ShowLabelMessage(ex.Message, MessageType.Failure);
        }
    }

    protected void DataGridServerMap_UpdateCommand(Object source, DataGridCommandEventArgs e)
    {
        try
        {
            DropDownList DropDownListRegion = e.Item.Cells[2].FindControl("DropDownListRegionEdit") as DropDownList;
            Common_ServerDropDownList DropDownListServer = e.Item.Cells[6].FindControl("DropDownListServer") as Common_ServerDropDownList;
            TextBox TextBoxName = e.Item.Cells[3].Controls[0] as TextBox;
            CheckBoxList CheckBoxListServerFlag = e.Item.Cells[5].FindControl("CheckBoxListServerFlag") as CheckBoxList;

            int nodeId = int.Parse(e.Item.Cells[0].Text);

            int flag = GetFlagFromCheckBoxList(CheckBoxListServerFlag);
            int serverId = DropDownListServer.SelectedServerId;
            string name = TextBoxName.Text.Trim();
            if (name.Length == 0)
            {
                ShowLabelMessage(string.Format(StringDef.MsgCannotBeNone, StringDef.Name), MessageType.Failure);
                return;
            }

            if(DropDownListRegion.SelectedIndex == -1)
            {
                ShowLabelMessage(string.Format(StringDef.MsgCannotBeNone, StringDef.ServerListRegion), MessageType.Failure);
                return;
            }
            MapNode parentNode = GetMapNode(int.Parse(DropDownListRegion.SelectedValue), _netList);


            if (nodeId == -1)
            {
                //添加
                int maxId = GetMaxMapNodeId(_netList);
                if (maxId != -1)
                {
                    parentNode.AddChild(new MapNode(maxId + 1, ServerMap.ServerMapType.Server, flag, name, serverId));
                }
                else
                {
                    ShowLabelMessage(string.Format(StringDef.MsgCannotBeNone, StringDef.ServerListRegion), MessageType.Failure);
                }
            }
            else
            {
                //修改
                MapNode selNode = GetMapNode(nodeId, _netList);
                if (selNode != null)
                {
                    if (selNode.Parent == parentNode)
                    {
                        selNode.MapServerId = serverId;
                        selNode.Flag = flag;
                        selNode.Name = name;
                    }
                    else
                    {
                        selNode.MapServerId = serverId;
                        selNode.Flag = flag;
                        selNode.Name = name;

                        selNode.Parent.RemoveChild(selNode);
                        parentNode.AddChild(selNode);
                    }
                }
            }

            SetModified(true);

            DataGridServerMap.EditItemIndex = -1;
            BindTable();

            TotalReport();
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            ShowLabelMessage(ex.Message, MessageType.Failure);
        }
    }

    protected void DataGridServerMap_ItemCommand(Object source, DataGridCommandEventArgs e)
    {
        try
        {
            int nodeId = int.Parse(e.Item.Cells[0].Text);
            MapNode selNode = GetMapNode(nodeId, _netList);
            if (selNode != null)
            {
                LinkButton lb = e.CommandSource as LinkButton;
                if (lb != null)
                {
                    switch (lb.CommandName)
                    {
                        case "MoveUp":
                            selNode.Parent.MoveChildForward(selNode);
                            SetModified(true);
                            break;
                        case "MoveDown":
                            selNode.Parent.MoveChildBackward(selNode);
                            SetModified(true);
                            break;
                        case "MoveTop":
                            selNode.Parent.MoveChildToTop(selNode);
                            SetModified(true);
                            break;
                        case "MoveBottom":
                            selNode.Parent.MoveChildToBottom(selNode);
                            SetModified(true);
                            break;
                    }

                    BindTable();
                }
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            ShowLabelMessage(ex.Message, MessageType.Failure);
        }
    }

    protected void DataGridServerMap_ItemDataBound(Object sender, DataGridItemEventArgs e)
    {
        try
        {
            if (e.Item.ItemIndex != -1)
            {
                MapNode currNode = GetMapNode(int.Parse(e.Item.Cells[0].Text), _netList);
                if (currNode != null)
                {
                    LinkButton lbDelete = e.Item.Cells[8].Controls[0] as LinkButton;
                    if (lbDelete != null)
                    {
                        lbDelete.Attributes.Add("onclick", string.Format("return msgConfirm('{0}');", StringDef.MsgDeleteConfirmShort));
                    }
                    LinkButton lbMoveUp = e.Item.Cells[10].Controls[0] as LinkButton;
                    if (lbMoveUp != null)
                    {
                        lbMoveUp.Enabled = currNode.Parent.FirstChild != currNode;
                    }

                    LinkButton lbMoveDown = e.Item.Cells[11].Controls[0] as LinkButton;
                    if (lbMoveDown != null)
                    {
                        lbMoveDown.Enabled = currNode.Parent.LastChild != currNode;
                    }

                    LinkButton lbMoveTop = e.Item.Cells[12].Controls[0] as LinkButton;
                    if (lbMoveTop != null)
                    {
                        lbMoveTop.Enabled = currNode.Parent.FirstChild != currNode;
                    }

                    LinkButton lbMoveBottom = e.Item.Cells[13].Controls[0] as LinkButton;
                    if (lbMoveBottom != null)
                    {
                        lbMoveBottom.Enabled = currNode.Parent.LastChild != currNode;
                    }

                    int index = _netList.IndexOf(currNode.Parent.Parent);                    
                    if (index != -1)
                    {
                        e.Item.Cells[1].BackColor = DefaultNetColor[index % 2];
                        int regionIndex = -1;
                        for (int tempIndex = 0; tempIndex < currNode.Parent.Parent.Children.Length; ++tempIndex)
                        {
                            if (currNode.Parent.ID == currNode.Parent.Parent.Children[tempIndex].ID)
                            {
                                regionIndex = tempIndex;
                                break;
                            }
                        }
                        if (regionIndex != -1)
                        {
                            for (int cellIndex = 2; cellIndex < e.Item.Cells.Count; ++cellIndex)
                            {
                                if (currNode.Hide == true)
                                {
                                    e.Item.Cells[cellIndex].BackColor = Color.Gray;
                                }
                                else
                                {
                                    e.Item.Cells[cellIndex].BackColor = DefaultRegionColor[regionIndex % 2];
                                }
                            }
                        }
                    }
                }
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    private void SetInfo(DataGridItem editItem)
    {
        DropDownList DropDownListNet = editItem.Cells[1].FindControl("DropDownListNetEdit") as DropDownList;
        DropDownList DropDownListRegion = editItem.Cells[2].FindControl("DropDownListRegionEdit") as DropDownList;

        Common_ServerGroupDropDownList DropDownListServerGroup = editItem.Cells[6].FindControl("DropDownListServerGroup") as Common_ServerGroupDropDownList;
        Common_ServerDropDownList DropDownListServer = editItem.Cells[6].FindControl("DropDownListServer") as Common_ServerDropDownList;

        CheckBoxList CheckBoxListServerFlag = editItem.Cells[5].FindControl("CheckBoxListServerFlag") as CheckBoxList;

        MapNode serverNode = GetMapNode(int.Parse(editItem.Cells[0].Text), _netList);
        if (serverNode != null)
        {            
            CreateDropDownListNet(DropDownListNet);

            //选择Net和Region
            DropDownListNet.SelectedIndex = -1;
            foreach (ListItem item in DropDownListNet.Items)
            {
                if (int.Parse(item.Value) == serverNode.Parent.Parent.ID)
                {
                    item.Selected = true;
                    break;
                }
            }
            CreateDropDownListRegion(DropDownListRegion, serverNode.Parent.Parent);
            DropDownListRegion.SelectedIndex = -1;
            foreach (ListItem item in DropDownListRegion.Items)
            {
                if (int.Parse(item.Value) == serverNode.Parent.ID)
                {
                    item.Selected = true;
                    break;
                }
            }

            //Server
            GameServer server = TheAdminServer.GameServerManager.GetGameServer(serverNode.MapServerId);
            if (server != null)
            {
                DropDownListServerGroup.Refresh();
                DropDownListServerGroup.SelectedServerGroup = server.Group;
                DropDownListServer.CreateServerList(server.Group, GameServer.ServerType.bishop);
                DropDownListServer.SelectedGameServer = server;
            }

            CreateFlagCheckBoxList(CheckBoxListServerFlag, serverNode.Flag);

            SetFlagCheckBoxListByGroupEnabled(CheckBoxListServerFlag, TheAdminServer.GameServerManager.GetGameServer(serverNode.MapServerId).Group);
        }
        else
        {
            CreateDropDownListNet(DropDownListNet);
            CreateDropDownListRegion(DropDownListRegion, int.Parse(DropDownListNet.SelectedValue));

            DropDownListServerGroup.Refresh();
            DropDownListServer.CreateServerList(DropDownListServerGroup.SelectedServerGroup, GameServer.ServerType.bishop);

            CreateFlagCheckBoxList(CheckBoxListServerFlag, 0);

            SetFlagCheckBoxListByGroupEnabled(CheckBoxListServerFlag, DropDownListServerGroup.SelectedServerGroup);
        }
    }

    private void SetFlagCheckBoxListByGroupEnabled(CheckBoxList list, ServerGroup group)        
    {
        ListItem item = list.Items.FindByValue(((int)ServerMap.ServerMapFlag.Hide).ToString());
        if (group.group_enable == ServerGroup.GroupEnable.Close)
        {
            item.Selected = true;
            item.Enabled = false;
        }
        else
        {
            item.Selected = false;
            item.Enabled = true;
        }
    }

    protected void LinkButtonAddServer_Click(object sender, EventArgs e)
    {
        try
        {
            DataTable dataTable = (CreateDataSource() as DataView).Table;
            DataRow newRow = dataTable.NewRow();
            newRow["Id"] = -1;
            newRow["Net"] = "Net";
            newRow["Region"] = "Region";
            newRow["Name"] = "Name";
            newRow["Flag"] = 0;
            newRow["ServerName"] = "ServerName";
            newRow["ServerIp"] = "ServerIp";
            newRow["FlagDesc"] = StringDef.None;

            dataTable.Rows.Add(newRow);

            DataGridServerMap.EditItemIndex = dataTable.Rows.Count - 1;

            DataGridServerMap.DataSource = dataTable.DefaultView;
            DataGridServerMap.DataBind();

            SetInfo(DataGridServerMap.Items[DataGridServerMap.EditItemIndex]);
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            ShowLabelMessage(ex.Message, MessageType.Failure);
        }
    }

    protected void LinkButtonShowNet_Click(object sender, EventArgs e)
    {
        LinkButton lb = sender as LinkButton;
        if (lb != null)
        {
            if (lb.Text.Equals(StringDef.ShowNet))
            {
                PanelNet.Visible = true;
                lb.Text = StringDef.HideNet;
                BindTableNet();
            }
            else
            {
                PanelNet.Visible = false;
                lb.Text = StringDef.ShowNet;
            }
        }
    }

    protected void LinkButtonShowRegion_Click(object sender, EventArgs e)
    {
        LinkButton lb = sender as LinkButton;
        if (lb != null)
        {
            if (lb.Text.Equals(StringDef.ShowRegion))
            {
                PanelRegion.Visible = true;
                lb.Text = StringDef.HideRegion;
                BindTableRegion();
            }
            else
            {
                PanelRegion.Visible = false;
                lb.Text = StringDef.ShowRegion;
            }
        }
    }

    #endregion

    protected void CreateDropDownListNet(DropDownList list)
    {
        if (list != null)
        {
            foreach (MapNode net in _netList)
            {
                list.Items.Add(new ListItem(net.Name, net.ID.ToString()));
            }
        }
    }

    protected void CreateDropDownListRegion(DropDownList list, MapNode netNode)
    {
        if (list != null && netNode != null)
        {
            list.Items.Clear();
            foreach (MapNode region in netNode.Children)
            {
                list.Items.Add(new ListItem(region.Name, region.ID.ToString()));
            }
        }
    }

    protected void CreateDropDownListRegion(DropDownList list, int nodeId)
    {
        if (list != null)
        {
            MapNode node = GetMapNode(nodeId, _netList);
            if (node != null)
            {
                CreateDropDownListRegion(list, node);
            }
        }
    }

    protected void DropDownListNetEdit_SelectedIndexChanged(object sender, EventArgs e)
    {
        try
        {
            DropDownList DropDownListNetEdit = sender as DropDownList;

            if (DropDownListNetEdit != null && DropDownListNetEdit.SelectedIndex != -1)
            {
                MapNode netNode = GetMapNode(int.Parse(DropDownListNetEdit.SelectedValue), _netList);
                if (netNode != null)
                {
                    DropDownList DropDownListRegionEdit = DataGridServerMap.Items[DataGridServerMap.EditItemIndex].Cells[2].FindControl("DropDownListRegionEdit") as DropDownList;
                    if (DropDownListRegionEdit != null)
                    {
                        DropDownListRegionEdit.Items.Clear();
                        foreach (MapNode region in netNode.Children)
                        {
                            DropDownListRegionEdit.Items.Add(new ListItem(region.Name, region.ID.ToString()));
                        }
                    }
                }
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    private int GetMaxMapNodeId(List<MapNode> netList)
    {
        int maxId = -1;
        if (netList != null)
        {
            foreach (MapNode net in netList)
            {
                if (net.ID > maxId)
                {
                    maxId = net.ID;
                }
                foreach (MapNode region in net.Children)
                {
                    if (region.ID > maxId)
                    {
                        maxId = region.ID;
                    }
                    foreach (MapNode mapServer in region.Children)
                    {
                        if (mapServer.ID > maxId)
                        {
                            maxId = mapServer.ID;
                        }
                    }
                }
            }
        }
        return maxId;
    }

    private MapNode GetMapNode(int mapNodeId, List<MapNode> netList)
    {
        foreach (MapNode net in netList)
        {
            if (net.ID == mapNodeId)
            {
                return net;
            }
            foreach (MapNode region in net.Children)
            {
                if (region.ID == mapNodeId)
                {
                    return region;
                }
                foreach (MapNode mapServer in region.Children)
                {
                    if (mapServer.ID == mapNodeId)
                    {
                        return mapServer;
                    }
                }
            }
        }
        return null;
    }

    /// <summary>
    /// 保存整体配置
    /// </summary>
    protected void LinkButtonSave_Click(object sender,EventArgs e)
    {
        try
        {
            //对此页面的写权限
            if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerServerListConfig, OpType.WRITE, Session))
            {
                ShowLabelMessage(StringDef.NotEnoughPrivilege, MessageType.Failure);
                return;
            }

            if (TheAdminServer.GameServerManager.ServerMap.SaveServerMapList(_netList))
            {
                SetModified(false);
                ShowLabelMessage(StringDef.OperationSucceed + StringDef.Colon + StringDef.SaveWholeConfiguration, MessageType.Success);
            }
            else
            {
                ShowLabelMessage(StringDef.OperationFail + StringDef.Colon + StringDef.SaveWholeConfiguration, MessageType.Failure);
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            ShowLabelMessage(ex.Message, MessageType.Failure);
        }
    }

    #region Net
    protected ICollection CreateDataSourceNet()
    {
        try
        {
            _netList = Session["Store"] as List<MapNode>;

            DataTable dataTable = new DataTable();
            DataRow dataRow = null;
            dataTable.Columns.Add(new DataColumn("Id", typeof(Int32)));            
            dataTable.Columns.Add(new DataColumn("Name", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Flag", typeof(String)));
            dataTable.Columns.Add(new DataColumn("FlagDesc", typeof(String)));

            if (_netList != null)
            {
                foreach (MapNode net in _netList)
                {
                    dataRow = dataTable.NewRow();
                    dataRow[0] = net.ID;
                    dataRow[1] = net.Name;
                    dataRow[2] = net.Flag.ToString();
                    dataRow[3] = GetFlagDesc(net.Flag);

                    dataTable.Rows.Add(dataRow);                    
                }
            }

            return dataTable.DefaultView;
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            return null;
        }
    }

    private void BindTableNet()
    {
        try
        {
            DataGridNet.DataSource = CreateDataSourceNet();
            DataGridNet.DataBind();
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    //protected void DataGridNet_DeleteCommand(Object source, DataGridCommandEventArgs e)
    //{
    //    try
    //    {
    //        MapNode selNode = GetMapNode(int.Parse(e.Item.Cells[0].Text), _netList);
    //        if (selNode != null)
    //        {
    //            _netList.Remove(selNode);

    //            BindTable();
    //            BindTableNet();
    //            BindTableRegion();
    //            TotalReport();
    //        }
    //        else
    //        {
    //            ShowLabelMessage(string.Format(StringDef.MsgCannotBeNone, StringDef.Server), MessageType.Failure);
    //        }
    //    }
    //    catch (Exception ex)
    //    {
    //        FSEye.Util.DebugLog(ex.ToString());

    //        ShowLabelMessage(StringDef.OperationFail, MessageType.Failure);
    //    }
    //}

    protected void DataGridNet_EditCommand(Object source, DataGridCommandEventArgs e)
    {
        try
        {
            DataGridNet.EditItemIndex = e.Item.ItemIndex;
            BindTableNet();

            SetNetInfo(DataGridNet.Items[DataGridNet.EditItemIndex]);
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            ShowLabelMessage(ex.Message, MessageType.Failure);
        }
    }

    private void SetNetInfo(DataGridItem editItem)
    {
        CheckBoxList CheckBoxListServerFlag = DataGridNet.Items[DataGridNet.EditItemIndex].Cells[3].FindControl("CheckBoxListNetFlag") as CheckBoxList;
        MapNode selNode = GetMapNode(int.Parse(editItem.Cells[0].Text), _netList);

        if (selNode != null)
        {
            CreateFlagCheckBoxList(CheckBoxListServerFlag, selNode.Flag);
        }
        else
        {
            CreateFlagCheckBoxList(CheckBoxListServerFlag, 0);
        }
    }

    protected void DataGridNet_CancelCommand(Object source, DataGridCommandEventArgs e)
    {
        DataGridNet.EditItemIndex = -1;
        BindTableNet();
    }

    protected void DataGridNet_UpdateCommand(Object source, DataGridCommandEventArgs e)
    {
        try
        {
            TextBox TextBoxName = e.Item.Cells[1].Controls[0] as TextBox;
            CheckBoxList CheckBoxListNetFlag = e.Item.Cells[3].FindControl("CheckBoxListNetFlag") as CheckBoxList;

            int nodeId = int.Parse(e.Item.Cells[0].Text);
            int flag = GetFlagFromCheckBoxList(CheckBoxListNetFlag);// int.Parse(e.Item.Cells[2].Text);
            string name = TextBoxName.Text.Trim();
            if (name.Length == 0)
            {
                ShowLabelMessage(string.Format(StringDef.MsgCannotBeNone, StringDef.Name), MessageType.Failure);
                return;
            }

            MapNode selNode = GetMapNode(nodeId, _netList);

            if (nodeId == -1)
            {
                //添加
                ////对第一级分组的添加做权限判断特例 很大的权限
                //if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.SecurityObject, OpType.WRITE, Session))
                //{
                //    ShowLabelMessage(WebConfig.PageNotEnoughPrivilege, MessageType.Failure);
                //    return;
                //}

                //对第一级分组的数量做限至
                if (_netList.Count >= MaxNetGroupCount)
                {
                    ShowLabelMessage(StringDef.MsgNetGroupBeyondMax, MessageType.Failure);
                    return;
                }

                int maxId = GetMaxMapNodeId(_netList);
                if (maxId != -1)
                {
                    _netList.Add(new MapNode(maxId + 1, ServerMap.ServerMapType.Net, flag, name, -1));
                }
                else
                {
                    //初始Id为1
                    _netList.Add(new MapNode(1, ServerMap.ServerMapType.Net, flag, name, -1));
                }
            }
            else
            {
                //修改
                selNode.Flag = flag;
                selNode.Name = name;
            }

            SetModified(true);

            DataGridNet.EditItemIndex = -1;

            BindTable();
            BindTableNet();
            TotalReport();
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            ShowLabelMessage(ex.Message, MessageType.Failure);
        }
    }

    protected void DataGridNet_ItemCommand(Object source, DataGridCommandEventArgs e)
    {
        try
        {
            int nodeId = int.Parse(e.Item.Cells[0].Text);
            MapNode selNode = GetMapNode(nodeId, _netList);
            if (selNode != null)
            {
                LinkButton lb = e.CommandSource as LinkButton;
                if (lb != null)
                {
                    int index = _netList.IndexOf(selNode);
                    switch (lb.CommandName)
                    {
                        case "MoveUp":
                            if (index != -1 && index != 0)
                            {
                                _netList.Reverse(index - 1, 2);
                            }
                            SetModified(true);
                            BindTable();
                            BindTableNet();
                            BindTableRegion();
                            break;
                        case "MoveDown":
                            if (index != -1 && index < _netList.Count - 1)
                            {
                                _netList.Reverse(index, 2);
                            }
                            SetModified(true);
                            BindTable();
                            BindTableNet();
                            BindTableRegion();
                            break;
                        case "MoveTop":
                            if (index != -1 && index != 0)
                            {
                                while (index != 0)
                                {
                                    _netList.Reverse(index - 1, 2);
                                    index--;
                                }
                            }
                            SetModified(true);
                            BindTable();
                            BindTableNet();
                            BindTableRegion();
                            break;
                        case "MoveBottom":
                            if (index != -1 && index < _netList.Count - 1)
                            {
                                while (index != _netList.Count - 1)
                                {
                                    _netList.Reverse(index, 2);
                                    index++;
                                }
                            }
                            SetModified(true);
                            BindTable();
                            BindTableNet();
                            BindTableRegion();
                            break;
                        //case DataGrid.CancelCommandName:
                        //    DataGridServerMap.EditItemIndex = -1;
                        //    BindTableNet();
                        //    break;
                        case DataGrid.DeleteCommandName:
                            _netList.Remove(selNode);
                            SetModified(true);
                            BindTable();
                            BindTableNet();
                            BindTableRegion();
                            TotalReport();
                            DataGridNet.EditItemIndex = -1;
                            break;
                    }
                }
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            ShowLabelMessage(ex.Message, MessageType.Failure);
        }
    }

    protected void LinkButtonAddNet_Click(object sender, EventArgs e)
    {
        try
        {
            DataTable dataTable = (CreateDataSourceNet() as DataView).Table;
            DataRow newRow = dataTable.NewRow();
            newRow["Id"] = -1;
            newRow["Name"] = "Name";
            newRow["Flag"] = 0;
            newRow["FlagDesc"] = StringDef.None;

            dataTable.Rows.Add(newRow);

            DataGridNet.EditItemIndex = dataTable.Rows.Count - 1;

            DataGridNet.DataSource = dataTable.DefaultView;
            DataGridNet.DataBind();

            SetNetInfo(DataGridNet.Items[DataGridNet.EditItemIndex]);
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            ShowLabelMessage(ex.Message, MessageType.Failure);
        }
    }

    protected void DataGridNet_ItemDataBound(Object sender, DataGridItemEventArgs e)
    {
        try
        {
            if (e.Item.ItemIndex != -1)
            {
                MapNode currNode = GetMapNode(int.Parse(e.Item.Cells[0].Text), _netList);
                if (currNode != null)
                {
                    LinkButton lbDelete = e.Item.Cells[4].Controls[0] as LinkButton;
                    if (lbDelete != null)
                    {
                        lbDelete.Attributes.Add("onclick", string.Format("return msgConfirm('{0}');", StringDef.MsgDeleteConfirmShort));
                        //lbDelete.Enabled = false;
                    }
                    int index = _netList.IndexOf(currNode);
                    if (index != -1)
                    {
                        LinkButton lbMoveUp = e.Item.Cells[6].Controls[0] as LinkButton;
                        if (lbMoveUp != null && index == 0)
                        {
                            lbMoveUp.Enabled = false;
                        }

                        LinkButton lbMoveDown = e.Item.Cells[7].Controls[0] as LinkButton;
                        if (lbMoveDown != null && index == _netList.Count - 1)
                        {
                            lbMoveDown.Enabled = false;
                        }

                        LinkButton lbMoveTop = e.Item.Cells[8].Controls[0] as LinkButton;
                        if (lbMoveTop != null && index == 0)
                        {
                            lbMoveTop.Enabled = false;
                        }

                        LinkButton lbMoveBottom = e.Item.Cells[9].Controls[0] as LinkButton;
                        if (lbMoveBottom != null && index == _netList.Count - 1)
                        {
                            lbMoveBottom.Enabled = false;
                        }

                        e.Item.BackColor = DefaultNetColor[index % 2];
                    }
                }
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }
    #endregion

    #region Region
    protected ICollection CreateDataSourceRegion()
    {
        try
        {
            DataTable dataTable = new DataTable();
            DataRow dataRow = null;
            dataTable.Columns.Add(new DataColumn("Id", typeof(Int32)));
            dataTable.Columns.Add(new DataColumn("Net", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Name", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Flag", typeof(String)));
            dataTable.Columns.Add(new DataColumn("FlagDesc", typeof(String)));

            _netList = Session["Store"] as List<MapNode>;
            
            if (_netList != null)
            {
                Color netColor = DefaultNetColor[0];
                Color regionColor = DefaultRegionColor[0];
                
                foreach (MapNode net in _netList)
                {
                    Color currNetColor = netColor;
                    string netColorDesc = WebUtil.GetColorWebStyle(currNetColor);                

                    foreach (MapNode region in net.Children)
                    {
                        Color currRegionColor = regionColor;
                        string regionColorDesc = WebUtil.GetColorWebStyle(currRegionColor);

                        dataRow = dataTable.NewRow();
                        dataRow[0] = region.ID;
                        dataRow[1] = net.Name;// string.Format("<div style='background-color:{0};text-align:center;height:100%;'><div style='padding:5px;'>{1}</div></div>", netColorDesc, net.Name);
                        dataRow[2] = region.Name;// string.Format("<div style='background-color:{0};text-align:center;height:100%;'><div style='padding:5px;'>{1}</div></div>", regionColorDesc, region.Name);
                        dataRow[3] = region.Flag.ToString();
                        dataRow[4] = GetFlagDesc(region.Flag);

                        dataTable.Rows.Add(dataRow);

                        if (netColor == currNetColor)
                        {
                            netColor = SwitchNetColor(netColor);
                        }
                        if (currRegionColor == regionColor)
                        {
                            regionColor = SwitchRegionColor(regionColor);
                        }
                    }
                }
            }

            return dataTable.DefaultView;
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            return null;
        }
    }

    private void BindTableRegion()
    {
        try
        {
            DataGridRegion.DataSource = CreateDataSourceRegion();
            DataGridRegion.DataBind();
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    protected void LinkButtonAddRegion_Click(object sender, EventArgs e)
    {
        try
        {
            DataTable dataTable = (CreateDataSourceRegion() as DataView).Table;
            DataRow newRow = dataTable.NewRow();
            newRow["Id"] = -1;
            newRow["Net"] = "Net";
            newRow["Name"] = "Name";
            newRow["Flag"] = 0;
            newRow["FlagDesc"] = StringDef.None;

            dataTable.Rows.Add(newRow);

            DataGridRegion.EditItemIndex = dataTable.Rows.Count - 1;

            DataGridRegion.DataSource = dataTable.DefaultView;
            DataGridRegion.DataBind();

            SetRegionInfo(DataGridRegion.Items[DataGridRegion.EditItemIndex]);
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            ShowLabelMessage(ex.Message, MessageType.Failure);
        }
    }

    protected void DataGridRegion_DeleteCommand(Object source, DataGridCommandEventArgs e)
    {
        try
        {
            MapNode selNode = GetMapNode(int.Parse(e.Item.Cells[0].Text), _netList);
            if (selNode != null)
            {
                selNode.Parent.RemoveChild(selNode);

                SetModified(true);

                BindTable();
                BindTableNet();
                BindTableRegion();
                TotalReport();
            }
            else
            {
                ShowLabelMessage(string.Format(StringDef.MsgCannotBeNone, StringDef.Server), MessageType.Failure);
            }
            DataGridRegion.EditItemIndex = -1;
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());

            ShowLabelMessage(StringDef.OperationFail, MessageType.Failure);
        }
    }

    protected void DataGridRegion_CancelCommand(Object source, DataGridCommandEventArgs e)
    {
        DataGridRegion.EditItemIndex = -1;
        BindTableRegion();
    }

    protected void DataGridRegion_EditCommand(Object source, DataGridCommandEventArgs e)
    {
        try
        {
            DataGridRegion.EditItemIndex = e.Item.ItemIndex;
            BindTableRegion();
            SetRegionInfo(DataGridRegion.Items[DataGridRegion.EditItemIndex]);
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            ShowLabelMessage(ex.Message, MessageType.Failure);
        }
    }

    protected void DataGridRegion_UpdateCommand(Object source, DataGridCommandEventArgs e)
    {
        try
        {
            DropDownList DropDownListNet = e.Item.Cells[1].FindControl("DropDownListNet") as DropDownList;
            TextBox TextBoxName = e.Item.Cells[2].Controls[0] as TextBox;
            CheckBoxList CheckBoxListRegionFlag = e.Item.Cells[4].FindControl("CheckBoxListRegionFlag") as CheckBoxList;

            int nodeId = int.Parse(e.Item.Cells[0].Text);
            int flag = GetFlagFromCheckBoxList(CheckBoxListRegionFlag);// int.Parse(e.Item.Cells[3].Text);
            string name = TextBoxName.Text.Trim();
            if (name.Length == 0)
            {
                ShowLabelMessage(string.Format(StringDef.MsgCannotBeNone, StringDef.Name), MessageType.Failure);
                return;
            }

            MapNode parentNode = GetMapNode(int.Parse(DropDownListNet.SelectedValue), _netList);

            if (nodeId == -1)
            {
                //对第二级分组的数量做限至
                if (parentNode.Children.Length >= MaxRegionGroupCount)
                {
                    ShowLabelMessage(StringDef.MsgRegionGroupBeyondMax, MessageType.Failure);
                    return;
                }

                //添加
                int maxId = GetMaxMapNodeId(_netList);
                if (maxId != -1)
                {
                    parentNode.AddChild(new MapNode(maxId + 1, ServerMap.ServerMapType.Region, flag, name, -1));
                }
                else
                {
                    ShowLabelMessage(string.Format(StringDef.MsgCannotBeNone, StringDef.ServerListNet), MessageType.Failure);
                }
            }
            else
            {
                //修改
                MapNode selNode = GetMapNode(nodeId, _netList);
                if (selNode != null)
                {
                    if (selNode.Parent == parentNode)
                    {
                        selNode.Flag = flag;
                        selNode.Name = name;
                    }
                    else
                    {
                        selNode.Flag = flag;
                        selNode.Name = name;

                        selNode.Parent.RemoveChild(selNode);
                        parentNode.AddChild(selNode);
                    }
                }
            }

            SetModified(true);

            DataGridRegion.EditItemIndex = -1;

            BindTable();
            BindTableNet();
            BindTableRegion();
            TotalReport();
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            ShowLabelMessage(ex.Message, MessageType.Failure);
        }
    }

    protected void DataGridRegion_ItemCommand(Object source, DataGridCommandEventArgs e)
    {
        try
        {
            int nodeId = int.Parse(e.Item.Cells[0].Text);
            MapNode selNode = GetMapNode(nodeId, _netList);
            if (selNode != null)
            {
                LinkButton lb = e.CommandSource as LinkButton;
                if (lb != null)
                {
                    switch (lb.CommandName)
                    {
                        case "MoveUp":
                            selNode.Parent.MoveChildForward(selNode);
                            SetModified(true);
                            BindTable();
                            BindTableNet();
                            BindTableRegion();
                            break;
                        case "MoveDown":
                            selNode.Parent.MoveChildBackward(selNode);
                            SetModified(true);
                            BindTable();
                            BindTableNet();
                            BindTableRegion();
                            break;
                    }                    
                }
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
            ShowLabelMessage(ex.Message, MessageType.Failure);
        }
    }

    private void SetRegionInfo(DataGridItem editItem)
    {
        DropDownList DropDownListNet = editItem.Cells[1].FindControl("DropDownListNet") as DropDownList;
        CheckBoxList CheckBoxListRegionFlag = editItem.Cells[4].FindControl("CheckBoxListRegionFlag") as CheckBoxList;
        TextBox TextBoxRegionName = editItem.Cells[2].Controls[0] as TextBox;

        MapNode selNode = GetMapNode(int.Parse(editItem.Cells[0].Text), _netList);
        if (selNode != null)
        {
            CreateDropDownListNet(DropDownListNet);

            //选择Net和Region
            DropDownListNet.SelectedIndex = -1;
            foreach (ListItem item in DropDownListNet.Items)
            {
                if (int.Parse(item.Value) == selNode.Parent.ID)
                {
                    item.Selected = true;
                    break;
                }
            }

            CreateFlagCheckBoxList(CheckBoxListRegionFlag, selNode.Flag);

            TextBoxRegionName.Text = selNode.Name;
        }
        else
        {
            CreateDropDownListNet(DropDownListNet);
            CreateFlagCheckBoxList(CheckBoxListRegionFlag, 0);

            TextBoxRegionName.Text = "Name";
        }
    }

    protected void DataGridRegion_ItemDataBound(Object sender, DataGridItemEventArgs e)
    {
        try
        {
            if (e.Item.ItemIndex != -1)
            {
                MapNode currNode = GetMapNode(int.Parse(e.Item.Cells[0].Text), _netList);
                if (currNode != null)
                {
                    LinkButton lbDelete = e.Item.Cells[5].Controls[0] as LinkButton;
                    if (lbDelete != null)
                    {
                        lbDelete.Attributes.Add("onclick", string.Format("return msgConfirm('{0}');", StringDef.MsgDeleteConfirmShort));
                    }
                    LinkButton lbMoveUp = e.Item.Cells[7].Controls[0] as LinkButton;
                    if (lbMoveUp != null)
                    {
                        lbMoveUp.Enabled = currNode.Parent.FirstChild != currNode;
                    }

                    LinkButton lbMoveDown = e.Item.Cells[8].Controls[0] as LinkButton;
                    if (lbMoveDown != null)
                    {
                        lbMoveDown.Enabled = currNode.Parent.LastChild != currNode;
                    }
                    int index = _netList.IndexOf(currNode.Parent);
                    if (index != -1)
                    {
                        e.Item.BackColor = DefaultNetColor[index % 2];
                    }
                }
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }
    #endregion

    protected void LinkButtonCreateServerListFile_Click(object sender, EventArgs e)
    {
        try
        {
            //对此页面的写权限
            if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerServerListConfig, OpType.WRITE, Session))
            {
                ShowLabelMessage(StringDef.NotEnoughPrivilege, MessageType.Failure);
                return;
            }

            TheAdminServer.GameServerManager.CreateServerListFiles(
                TheAdminServer.GameServerManager.ServerMap.NetList,
                SystemConfig.Current.ServerListFileDir,
                SystemConfig.Current.DefaultEncoding
            );
            ShowLabelMessage(StringDef.OperationSucceed + StringDef.Colon + StringDef.CreateServerListFileSuccess, MessageType.Success);
        }
        catch (Exception ex)
        {
            ShowLabelMessage(ex.Message, MessageType.Failure);
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    private void ShowLabelMessage(string message, MessageType type)
    {
        Label messageLabel = LableOpResultMessageUpdatePanel.FindControl("LabelOpResultMessage") as Label;
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

    private void ClearLabelMessage()
    {
        Label messageLabel = (LableOpResultMessageUpdatePanel.FindControl("LabelOpResultMessage") as Label);
        if (messageLabel != null) messageLabel.Visible = false;
    }
}