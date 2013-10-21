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
using FSEye.FS2Lib;
using FSEye;

public partial class GameMaster_ItemBank : BasePage
{
    const int DefaultRecordPerPage = 50;
    int _recordPerPage = DefaultRecordPerPage;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
        }
        else
        {
            try
            {
                _recordPerPage = int.Parse(ListBoxRecordPerPage.SelectedValue);
            }
            catch (Exception)
            {
                _recordPerPage = DefaultRecordPerPage;
            }
        }
        LabelOpMsg.Text = string.Empty;
    }

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }


    protected void SelectedItemIdChanged(object sender, EventArgs e)
    {
        if (RadioButtonListItemId.SelectedIndex != -1)
            TextBoxItemId.Text = RadioButtonListItemId.SelectedValue;
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
            LabelSearchId.Text = StringDef.MsgNoInfo;
        }
    }

    ICollection CreateDataSource(ArrayList statInfos)
    {
        DataTable dataTable = new DataTable();
        DataRow dataRow;

        dataTable.Columns.Add(new DataColumn("Item", typeof(String)));
        dataTable.Columns.Add(new DataColumn("Count", typeof(UInt32)));
        dataTable.Columns.Add(new DataColumn("TemplateId", typeof(String)));

        foreach (ItemBankStatInfo info in statInfos)
        {
            dataRow = dataTable.NewRow();
            FS2ItemData item = FS2GameDataManager.TheInstance.GetItemData(info.ItemTemplateId);
            if (item != null)
            {
                string colorStr = string.Empty;
                switch (item.Quality)
                {
                    case FS2ItemQuality.White:
                        colorStr = "Black";
                        break;
                    case FS2ItemQuality.Blue:
                        colorStr = "#3165FF";
                        break;
                    case FS2ItemQuality.Green:
                        colorStr = "#00FB00";
                        break;
                    case FS2ItemQuality.Yellow:
                        colorStr = "Yellow";
                        break;
                    case FS2ItemQuality.Orange:
                        colorStr = "#F8952C";
                        break;
                }
                dataRow[0] = string.Format("<span onmouseover='{0}' style='color:{1};'>{2}</span>",
                                            string.Format("updateRoleItemInfo(\"{0}\")", item.TemplateId),
                                            colorStr,
                                            item.Name);
            }
            else
            {
                dataRow[0] = info.ItemTemplateId;
            }
            dataRow[1] = info.Count;
            dataRow[2] = info.ItemTemplateId;

            dataTable.Rows.Add(dataRow);
        }
        return new DataView(dataTable);
    }

    protected void ButtonStatistic_Click(object sender, EventArgs e)
    {
        Query(0);
    }

    private void Query(int offset)
    {
        try
        {
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
            ArrayList paramList = new ArrayList();
            StringBuilder cmdTextBuilder = new StringBuilder();
            cmdTextBuilder.AppendFormat("SELECT {1},{2},{3},{4},{5} FROM {6} ",
                FS2TableString.ItemBankFieldPlayerName, FS2TableString.ItemBankFieldGenre, FS2TableString.ItemBankFieldDetail,
                FS2TableString.ItemBankFieldParticular, FS2TableString.ItemBankFieldLevel, FS2TableString.ItemBankFieldItemCount,
                FS2TableString.ItemBankTableName);

            StringBuilder conditionStr = new StringBuilder();

            string itemId = TextBoxItemId.Text.Trim();
            if (itemId.Length != 0)
            {
                string[] ids = itemId.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                if (ids.Length != 4)
                {
                    LabelOpMsg.Text = StringDef.ParameterInputError;
                    return;
                }

                conditionStr.AppendFormat("AND {0}='{1}' AND {2}='{3}' AND {4}='{5}' AND {6}={7} ",
                    FS2TableString.ItemBankFieldGenre, ids[0], FS2TableString.ItemBankFieldDetail, ids[1],
                    FS2TableString.ItemBankFieldParticular, ids[2], FS2TableString.ItemBankFieldLevel, ids[3]);
            }
            
            string roleName = TextBoxRoleName.Text.Trim();
            if (roleName != null && roleName.Length != 0)
            {
                conditionStr.AppendFormat("AND {0}='?' ", FS2TableString.ItemBankFieldPlayerName);
                paramList.Add(roleName);
            }

            if (conditionStr.Length != 0)
            {
                conditionStr.Remove(0, 3);
                conditionStr.Insert(0, "WHERE");
                cmdTextBuilder.Append(conditionStr.ToString());
            }

            cmdTextBuilder.AppendFormat("GROUP BY CONCAT({0},{1},{2},{3})", FS2TableString.ItemBankFieldGenre,
                FS2TableString.ItemBankFieldDetail, FS2TableString.ItemBankFieldParticular, FS2TableString.ItemBankFieldLevel);

            int limitCount = _recordPerPage;
            cmdTextBuilder.AppendFormat(" LIMIT {0},{1}", offset, limitCount);

            SqlCommand cmd = new SqlCommand(cmdTextBuilder.ToString(), paramList.ToArray());
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                result.SetFieldType(new SqlDataType[]{
                                SqlDataType.Int32,
                                SqlDataType.Int32,
                                SqlDataType.Int32,
                                SqlDataType.Int32,
                                SqlDataType.UInt32
                                });

                object[] record = null;
                ArrayList statInfos = new ArrayList();
                while ((record = result.ReadRecord()) != null)
                {
                    ItemBankStatInfo info = new ItemBankStatInfo();

                    info.ItemTemplateId = string.Concat((int)record[0], ',', (int)record[1], ',', (int)record[2], ',', (int)record[3]);
                    info.Count = (UInt32)record[4];

                    statInfos.Add(info);
                }

                ViewState[WebConfig.SessionQueryLogOffset] = offset;

                ButtonPreviousPage.Enabled = (offset > 0);
                ButtonFirstPage.Enabled = (offset > 0);
                ButtonNextPage.Enabled = (statInfos.Count >= limitCount);

                if (statInfos.Count != 0)
                {
                    PanelResult.Visible = true;
                    if (TextBoxRoleName.Text.Trim().Length != 0)
                    {
                        LabelResult.Text = string.Format(StringDef.LabelStatisticResult, server.Group.Name,
                            server.Name, roleName, StringDef.Item);
                    }
                    else
                    {
                        LabelResult.Text = string.Format(StringDef.LabelStatisticResult, server.Group.Name,
                            server.Name, string.Empty, StringDef.Item);
                    }
                    
                    DataGridStatInfo.DataSource = CreateDataSource(statInfos);
                    DataGridStatInfo.DataBind();
                }
                else
                {
                    PanelResult.Visible = false;
                }
            }
            else
            {                
                LabelOpMsg.Text = StringDef.OperationFail;
                PanelResult.Visible = false;
            }
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = ex.Message;
            PanelResult.Visible = false;
        }
    }

    protected void ButtonPage_Click(object sender, EventArgs e)
    {
        Button bt = sender as Button;
        if (bt != null)
        {
            int offset = (int)ViewState[WebConfig.SessionQueryLogOffset];
            switch (bt.CommandName)
            {
                case "FirstPage":
                    offset = 0;
                    break;
                case "PreviousPage":
                    offset -= _recordPerPage;
                    break;
                case "NextPage":
                    offset += _recordPerPage;
                    break;
            }
            Query(offset);
        }
    }
}

public class ItemBankStatInfo
{
    public string ItemTemplateId;

    public UInt32 Count;

    public ItemBankStatInfo()
    { }
}