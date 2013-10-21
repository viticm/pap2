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
using FSEye;
using Resources;
using FSEye.FS2Lib;
using FSEye.Security;

public partial class GameInfo_Item : BasePage
{
    const int DefaultRecordPerPage = 50;
    int _recordPerPage = DefaultRecordPerPage;

    DateTime _start;
    DateTime _end;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticItemStatistic, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
            StartDate.SelectedDate = DateTime.Today.AddMonths(-1);
            EndDate.SelectedDate = DateTime.Now;
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
        PanelResult.Visible = false;
    }

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }

    protected void ButtonStatistic_Click(object sender, EventArgs e)
    {
        Query(0);
    }

    void Query(int offset)
    {
        try
        {
            int serverId = ServerDropDownList.SelectedServerId;
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

            ArrayList tempParamList = new ArrayList();
            ArrayList paramList = new ArrayList();
            StringBuilder searchCondition = new StringBuilder();

            switch (ListBoxWay.SelectedValue)
            {
                case "All":
                    searchCondition.Append(string.Format("WHERE (LogEvent='{0}' OR LogEvent='{1}')", LogEvent.BuyStatistic, LogEvent.NpcDropStatistic));
                    break;
                case "Buy":
                    searchCondition.Append(string.Format("WHERE LogEvent='{0}'", LogEvent.BuyStatistic));
                    break;
                case "Drop":
                    searchCondition.Append(string.Format("WHERE LogEvent='{0}'", LogEvent.NpcDropStatistic));
                    break;
            }

            string itemId = TextBoxItemId.Text.Trim();
            if (itemId == null || itemId.Length == 0)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.ItemID);
                return;
            }

            searchCondition.Append(string.Format(" AND {0}='{1}' ", FS2TableString.LogFieldLogKey2, itemId));

            _start = StartDate.SelectedDate;
            if (_start == DateTime.MinValue)
            {
                LabelOpMsg.Text = StringDef.ParameterInputError;
                return;
            }
            _end = EndDate.SelectedDate;
            if (_end == DateTime.MinValue)
            {
                LabelOpMsg.Text = StringDef.ParameterInputError;
                return;
            }
            searchCondition.Append(string.Format(" AND {0}>='{1}' AND {0} <'{2}'", FS2TableString.LogFieldLogTime, _start, _end));

            string baseCmdString = string.Format("SELECT LogKey1,LogKey2,LogEvent,SUM(LogKey6) AS SubSum FROM {0} {1} GROUP BY LogKey1",
                "{0}", searchCondition.ToString());
            string addTableCmdString;
            WebUtil.AddTableNameToCmd(CurrentUser.Id, server, baseCmdString, tempParamList, _start, _end, out addTableCmdString, out paramList);
            if (addTableCmdString.Length == 0)
            {
                //
                return;
            }

            int limitCount = _recordPerPage;
            string limitStatement = string.Format(" LIMIT {0},{1}", offset, limitCount);

            string cmdString = "SELECT LogKey1,LogKey2,LogEvent,SUM(SubSum) AS Total FROM ({0}) AS A GROUP BY LogKey1 ORDER BY Total DESC {1}";

            SqlCommand cmd = new SqlCommand(string.Format(cmdString, addTableCmdString, limitStatement), paramList.ToArray());
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                result.SetFieldType(new SqlDataType[]{
                                    SqlDataType.String,
                                    SqlDataType.String,
                                    SqlDataType.Int32,
                                    SqlDataType.Int32
                                    });
                object[] record = null;
                ArrayList infos = new ArrayList();
                while ((record = result.ReadRecord()) != null)
                {
                    SearchItemInfo info = new SearchItemInfo();
                    info.sourceId = record[0] as string;
                    info.id = record[1] as string;
                    info.way = (int)record[2];
                    info.count = (int)record[3];
                    infos.Add(info);
                }

                uint total = 0;
                string baseCmdTotalString = string.Format("SELECT SUM(LogKey6) AS SubSum FROM {0} {1}",
                    "{0}", searchCondition.ToString());
                string addTableCmdTotalString;
                WebUtil.AddTableNameToCmd(CurrentUser.Id, server, baseCmdTotalString, tempParamList, _start, _end, out addTableCmdTotalString, out paramList);

                if (addTableCmdTotalString.Length != 0)
                {
                    string cmdTotalString = "SELECT SUM(SubSum) FROM ({0}) AS A";
                    SqlCommand sqlTotal = new SqlCommand(string.Format(cmdTotalString, addTableCmdTotalString), paramList.ToArray());
                    SqlResult sqlTotalResult = WebUtil.QueryGameServerDb(CurrentUser.Id, server, sqlTotal);

                    if (sqlTotalResult != null && sqlTotalResult.Success)
                    {
                        sqlTotalResult.SetFieldType(new SqlDataType[]{
                                SqlDataType.UInt32
                            });
                        record = sqlTotalResult.ReadRecord();
                        if (record != null && record[0] != null) total = (uint)record[0];
                    }
                }

                ViewState[WebConfig.SessionQueryLogOffset] = offset;

                ButtonPreviousPage.Enabled = (offset > 0);
                ButtonFirstPage.Enabled = (offset > 0);
                ButtonNextPage.Enabled = (infos.Count >= limitCount);

                if (infos.Count != 0)
                {
                    PanelResult.Visible = true;
                    CreateSearchResultList((SearchItemInfo[])infos.ToArray(typeof(SearchItemInfo)), total);
                    LabelResult.Text = string.Format(StringDef.LabelStatisticResult, server.Group.Name,
                                    server.Name, StringDef.Item, StringDef.Drop);
                }
                else
                {
                    PanelResult.Visible = false;
                    LabelOpMsg.Text = StringDef.NoMatchingRecord;
                }
            }
            else 
            {
                if (result == null)
                    LabelOpMsg.Text = StringDef.QueryTimeOut;
                else
                    LabelOpMsg.Text = StringDef.OperationFail;
            }
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = ex.Message;
            PanelResult.Visible = false;
        }
    }

    bool CreateSearchResultList(SearchItemInfo[] infos, uint totalNum)
    {
        if (infos != null)
        {
            foreach (SearchItemInfo info in infos)
            {
                TableRow row = new TableRow();

                TableCell cell = new TableCell();
                FS2ItemData item = FS2GameDataManager.TheInstance.GetItemData(info.id);
                if (item != null)
                {
                    string colorStr = "Black";
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

                    cell.Text = string.Format("<span onmouseover='{0}' style='color:{1}'>{2}</span>",
                            string.Format("updateRoleItemInfo(\"{0}\")", item.TemplateId), colorStr,
                            item.Name);
                }
                else cell.Text = info.id;
                row.Cells.Add(cell);

                cell = new TableCell();
                switch (info.way)
                {
                    case LogEvent.BuyStatistic:
                        cell.Text = StringDef.NotAvailable;
                        break;
                    case LogEvent.NpcDropStatistic:
                        FS2NpcData npc = FS2GameDataManager.TheInstance.GetNpcData(int.Parse(info.sourceId));
                        if (npc != null) cell.Text = string.Format("{0}[{1}]", npc.Name, npc.TemplateId);
                        else cell.Text = StringDef.NotAvailable;
                        break;
                }                
                row.Cells.Add(cell);                

                cell = new TableCell();
                switch (info.way)
                {
                    case LogEvent.BuyStatistic:
                        cell.Text = StringDef.Buy;
                        break;
                    case LogEvent.NpcDropStatistic:
                        cell.Text = StringDef.Drop;
                        break;
                }                
                row.Cells.Add(cell);

                cell = new TableCell();
                cell.Text = info.count.ToString();
                row.Cells.Add(cell);

                cell = new TableCell();
                if (totalNum != 0)
                {
                    cell.Text = Decimal.Round((decimal)info.count / totalNum * 100, 3).ToString() + "%";   
                }
                else
                {
                    cell.Text = info.count.ToString();
                }                
                row.Cells.Add(cell);

                TableSearchItem.Rows.Add(row);
            }
            TableRow rowHead = new TableRow();
            rowHead.Font.Bold = true;
            TableCell cellHead = new TableCell();
            cellHead.ColumnSpan = 5;
            cellHead.Text = string.Format("{0} : {1}", StringDef.Total, totalNum);
            rowHead.Cells.Add(cellHead);
            TableSearchItem.Rows.Add(rowHead);

            return true;
        }
        return false;
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
        if (items != null&&items.Count!=0)
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

    protected void SelectedItemIdChanged(object sender, EventArgs e)
    {
        if (RadioButtonListItemId.SelectedIndex != -1)
            TextBoxItemId.Text = RadioButtonListItemId.SelectedValue;
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

public class SearchItemInfo
{
    public string id = string.Empty;

    public int count = 0;

    public int way;

    public string sourceId = string.Empty;

    public SearchItemInfo()
    {
    }
}