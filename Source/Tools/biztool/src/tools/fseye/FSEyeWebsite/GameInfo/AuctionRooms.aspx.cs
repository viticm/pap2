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

public partial class GameInfo_AuctionRooms : BasePage
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

    ICollection CreateDataSource(ArrayList itemList)
    {
        try
        {
            DataTable dataTable = new DataTable();
            DataRow dataRow;

            dataTable.Columns.Add(new DataColumn("Seller", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Goods", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Buyer", typeof(String)));
            dataTable.Columns.Add(new DataColumn("StartTime", typeof(String)));
            dataTable.Columns.Add(new DataColumn("FinishTime", typeof(String)));
            dataTable.Columns.Add(new DataColumn("CurrentPrice", typeof(String)));
            dataTable.Columns.Add(new DataColumn("PassPrice", typeof(String)));

            if (itemList != null && itemList.Count != 0)
            {
                foreach (AuctionRoomItem item in itemList)
                {
                    dataRow = dataTable.NewRow();
                    dataRow[0] = string.Format("<a href='../GameMaster/RoleDetail.aspx?{0}={1}&{2}={3}' target='_blank' >{4}</a>",
                                                WebConfig.ParamServerId,
                                                (int)ViewState[WebConfig.ParamServerId],
                                                WebConfig.ParamRoleName,
                                                HttpUtility.UrlEncode(item.SellerName),
                                                item.SellerName);
					if (item.ItemData != null)
						dataRow[1] = string.Format("<div onmouseover='updateRoleItemInfo(\"{0}\")'>{1}</div>", item.ItemData.TemplateId, item.GoodsName);
					else
						dataRow[1] = item.GoodsName;
                    if (item.BuyerName != null && item.BuyerName.Length != 0)
                        dataRow[2] = string.Format("<a href='../GameMaster/RoleDetail.aspx?{0}={1}&{2}={3}' target='_blank' >{4}</a>",
                                                WebConfig.ParamServerId,
                                                (int)ViewState[WebConfig.ParamServerId],
                                                WebConfig.ParamRoleName,
                                                HttpUtility.UrlEncode(item.BuyerName),
                                                item.BuyerName);
                    else
                        dataRow[2] = StringDef.NotAvailable;
                    dataRow[3] = item.StartTime.ToString("yyyy-MM-dd HH:mm:ss");
                    dataRow[4] = item.FinishTime.ToString("yyyy-MM-dd HH:mm:ss");
                    dataRow[5] = GameDataRender.RenderMoney(item.CurrentPrice);
                    dataRow[6] = GameDataRender.RenderMoney(item.PassPrice);

                    dataTable.Rows.Add(dataRow);
                }
                return new DataView(dataTable);
            }
            return null;
        }
        catch (Exception)
        {
            return null;
        }        
    }

    protected void LinkButtonQuery_Click(object sender, EventArgs e)
    {
        Query(0);
    }

    protected void Query(int offset)
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
            StringBuilder searchCondition = new StringBuilder();

            string seller = TextBoxSeller.Text.Trim();
            if (seller.Length != 0)
            {
                if (CheckBoxSeller.Checked)
                    searchCondition.Append(string.Format(" AND {0}='?'", FS2TableString.AuctionFieldSellerName));
                else
                    searchCondition.Append(string.Format(" AND {0} LIKE '%?%'", FS2TableString.AuctionFieldSellerName));
                paramList.Add(seller);
            }

            string buyer = TextBoxBuyer.Text.Trim();
            if (buyer.Length != 0)
            {
                if(CheckBoxBuyer.Checked)
                    searchCondition.AppendFormat(" AND {0}='?'",FS2TableString.AuctionFieldBuyerName);
                else
                    searchCondition.AppendFormat(" AND {0} LIKE '%?%'", FS2TableString.AuctionFieldBuyerName);
                paramList.Add(buyer);
            }

            string itemName = TextBoxItemName.Text.Trim();
            if (itemName.Length != 0)
            {
                if(CheckBoxItemName.Checked)
                    searchCondition.AppendFormat(" AND {0}='?'",FS2TableString.AuctionFieldGoodsName);
                else
                    searchCondition.AppendFormat(" AND {0} LIKE '%?%'", FS2TableString.AuctionFieldGoodsName);
                paramList.Add(itemName);
            }

            //if (StartDate.Selected)
            //{
            //    searchCondition.AppendFormat(" AND {0}>='{1}'", FS2TableString.LogFieldLogTime, StartDate.SelectedDate);
            //}
            //if (EndDate.Selected)
            //{
            //    searchCondition.AppendFormat(" AND {0}<'{1}'", FS2TableString.LogFieldLogTime, EndDate.SelectedDate.AddDays(1));
            //}
            
            if (searchCondition.Length > 0)
            {
                searchCondition.Remove(0, 4);
                searchCondition.Insert(0, " WHERE");
            }

            string orderByType = string.Empty;
            switch (ListBoxOrderByType.SelectedIndex)
            {
                case 0:
                    orderByType = "ASC";
                    break;
                case 1:
                    orderByType = "DESC";
                    break;
            }
            string orderByStatement = string.Empty;
            switch (ListBoxOrderBy.SelectedIndex)
            {
                case 0:
                    orderByStatement = string.Format(" ORDER BY {0} {1}", FS2TableString.AuctionFieldCurrentPrice, orderByType);
                    break;
                case 1:
                    orderByStatement = string.Format(" ORDER BY {0} {1}", FS2TableString.AuctionFieldPassPrice, orderByType);
                    break;
            }

            int limitCount = _recordPerPage;
            string limitStatement = string.Format(" LIMIT {0},{1}", offset, limitCount);

            string cmdText = "SELECT {0} FROM {1} {2} {3} {4}";
            string cmdFieldText = string.Format("{0},{1},{2},{3},{4},{5},{6},{7}",
                                                FS2TableString.AuctionFieldSellerName,
                                                FS2TableString.AuctionFieldGoodsName,
                                                FS2TableString.AuctionFieldBuyerName,
                                                FS2TableString.AuctionFieldStartTime,
                                                FS2TableString.AuctionFieldFinishTime,
                                                FS2TableString.AuctionFieldCurrentPrice,
                                                FS2TableString.AuctionFieldPassPrice,
                                                FS2TableString.AuctionFieldItemData);

            SqlCommand cmd = new SqlCommand(string.Format(cmdText, cmdFieldText, FS2TableString.AuctionTableName, searchCondition.ToString(), orderByStatement, limitStatement), paramList.ToArray());
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                result.SetFieldType(new SqlDataType[]{
                                    SqlDataType.String,
                                    SqlDataType.String,
                                    SqlDataType.String,
                                    SqlDataType.DateTime,
                                    SqlDataType.DateTime,
                                    SqlDataType.UInt32,
                                    SqlDataType.UInt32,
                                    SqlDataType.Blob
                                    });

                object[] record = null;
                ArrayList infos = new ArrayList();

                while ((record = result.ReadRecord()) != null)
                {
                    AuctionRoomItem info = new AuctionRoomItem();
                    info.SellerName = record[0] as string;
                    info.GoodsName = record[1] as string;
                    info.BuyerName = record[2] as string;
                    info.StartTime = (DateTime)record[3];
                    info.FinishTime = (DateTime)record[4];
                    info.CurrentPrice = (uint)record[5];
                    info.PassPrice = (uint)record[6];
                    info.ItemData = FS2ItemDataInfo.ParseItem((byte[])record[7]);

                    infos.Add(info);
                }

                ViewState[WebConfig.SessionQueryLogOffset] = offset;

                ButtonPreviousPage.Enabled = (offset > 0);
                ButtonFirstPage.Enabled = (offset > 0);
                ButtonNextPage.Enabled = (infos.Count >= limitCount);

                ViewState[WebConfig.ParamServerId] = server.Id;
                if (infos.Count != 0)
                {
                    DataGridAuctionItem.DataSource = CreateDataSource(infos);
                    DataGridAuctionItem.DataBind();
                    PanelResult.Visible = true;
                }                
                else
                {
                    LabelOpMsg.Text = StringDef.NoMatchingRecord;
                    PanelResult.Visible = false;
                }
            }
            else
            {
                if (result == null)
                    LabelOpMsg.Text = StringDef.QueryTimeOut;
                else
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


public class AuctionRoomItem
{
    public string SellerName;

    public string BuyerName;

    public string GoodsName;

    public DateTime StartTime;

    public DateTime FinishTime;

    public uint CurrentPrice;

    public uint PassPrice;

    public FS2ItemDataInfo ItemData;

    public AuctionRoomItem()
    { }
}