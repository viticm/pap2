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
using FSEye.FS2Lib;
using FSEye.Security;

public partial class GameInfo_Npc : BasePage
{
    const int DefaultRecordPerPage = 50;
    int _recordPerPage = DefaultRecordPerPage;

    DateTime _start;
    DateTime _end;
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticNpcDropItemStatistic, OpType.READ, Session))
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

    protected void ButtonSearchNpcId_Click(object sender, EventArgs e)
    {
        RadioButtonListNpcId.Items.Clear();
        string npcName = TextBoxNpcName.Text;
        if (npcName == null || npcName.Trim().Length == 0)
        {
            LabelSearchId.Visible = true;
            LabelSearchId.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.NpcName);
            return;
        }
        IList npcs = FS2GameDataManager.TheInstance.GetNpcId(npcName);
        if (npcs != null && npcs.Count != 0)
        {
            foreach (FS2NpcData npc in npcs)
            {
                ListItem listItem = new ListItem(string.Format("{0}[{1}]", npc.Name, npc.TemplateId), npc.TemplateId.ToString());
                RadioButtonListNpcId.Items.Add(listItem);
            }
            LabelSearchId.Visible = false;
        }
        else
        {
            LabelSearchId.Visible = true;
            LabelSearchId.Text = StringDef.NoInfo;
        }
    }

    protected void SelectedNpcIdChanged(object sender, EventArgs e)
    {
        if (RadioButtonListNpcId.SelectedIndex != -1)
            TextBoxNpcId.Text = RadioButtonListNpcId.SelectedValue;
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

            searchCondition.Append(string.Format("WHERE LogEvent='{0}'", LogEvent.NpcDropStatistic));

            string npcId = TextBoxNpcId.Text.Trim();
            if (npcId.Length == 0)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.NpcID);
                return;
            }
            searchCondition.Append(string.Format(" AND {0}='{1}' ", FS2TableString.LogFieldLogKey1,npcId));

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
            searchCondition.Append(string.Format(" AND {0}>='{1}' AND {0}<'{2}'", FS2TableString.LogFieldLogTime, _start.ToString("yyyy-MM-dd HH:mm:ss"), _end.ToString("yyyy-MM-dd HH:mm:ss")));

            string baseCmdString = string.Format("SELECT LogKey2,SUM(LogKey6) AS SubSum FROM {0} {1} GROUP BY LogKey2",
                "{0}",searchCondition.ToString());
            string addTableCmdString;
            WebUtil.AddTableNameToCmd(CurrentUser.Id, server, baseCmdString, tempParamList, _start, _end, out addTableCmdString, out paramList);

            if (addTableCmdString.Length == 0)
            {
                return;
            }

            int limitCount = _recordPerPage;
            string limitStatement = string.Format(" LIMIT {0},{1}", offset, limitCount);

            string cmdString = "SELECT LogKey2,SUM(SubSum) AS Total FROM ({0}) AS A GROUP BY LogKey2 ORDER BY Total DESC {1}";

            SqlCommand cmd = new SqlCommand(string.Format(cmdString, addTableCmdString, limitStatement), paramList.ToArray());
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                result.SetFieldType(new SqlDataType[]{
                                    SqlDataType.String,
                                    SqlDataType.UInt32
                                    });
                object[] record = null;
                ArrayList infos = new ArrayList();                
                while ((record = result.ReadRecord()) != null)
                {
                    NpcDropItem info = new NpcDropItem();
                    info.itemTemplateId = record[0] as string;
                    info.num = (UInt32)record[1];
                    infos.Add(info);                    
                }

                //²éÑ¯×ÜÊý
                uint total = 0;
                string baseCmdTotalString = string.Format("SELECT SUM(LogKey6) AS SubSum FROM {0} {1}",
                    "{0}", searchCondition.ToString());
                string addTableCmdTotalString;
                WebUtil.AddTableNameToCmd(CurrentUser.Id, server, baseCmdTotalString, _start, _end, out addTableCmdTotalString);

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
                    CreateSearchResultList((NpcDropItem[])infos.ToArray(typeof(NpcDropItem)), total);
                    LabelResult.Text = string.Format(StringDef.LabelStatisticResult, server.Group.Name,
                        server.Name, string.Empty, StringDef.NpcDropItem);
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

    bool CreateSearchResultList(NpcDropItem[] infos,uint total)
    {
        if (infos != null && infos.Length != 0)
        {
            foreach (NpcDropItem info in infos)
            {
                TableRow row = new TableRow();
                TableCell cell = new TableCell();
                if (info.itemTemplateId != null && info.itemTemplateId.Length != 0)
                {
                    FS2ItemData itemData = FS2GameDataManager.TheInstance.GetItemData(info.itemTemplateId);
                    if (itemData != null)
                    {
                        string colorStr = "Black";
                        switch (itemData.Quality)
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

                        cell.Text = string.Format("<span onmouseover='{0}' style='color:{1};'>{2}</span>",
                            string.Format("updateRoleItemInfo(\"{0}\")", itemData.TemplateId), colorStr,
                            itemData.Name);
                    }
                    else cell.Text = info.itemTemplateId;
                }
                row.Cells.Add(cell);

                cell = new TableCell();
                cell.Text = info.num.ToString();
                row.Cells.Add(cell);

                cell = new TableCell();
                if (total != 0)
                {
                    cell.Text = Decimal.Round((decimal)info.num / total, 3) * 100 + "%";
                }
                else
                    cell.Text = "0";
                row.Cells.Add(cell);
                TableSearchNpc.Rows.Add(row);
            }
            TableRow rowTail = new TableRow();
            TableCell cellTail = new TableCell();
            cellTail.ColumnSpan = 3;
            cellTail.Font.Bold = true;
            FS2NpcData NpcData = FS2GameDataManager.TheInstance.GetNpcData(int.Parse(TextBoxNpcId.Text));
            if (NpcData != null)
                cellTail.Text = string.Format("[{0}]{1}", NpcData.Name, StringDef.DropItem);
            cellTail.Text += StringDef.Total + StringDef.Colon + total;
            rowTail.Cells.Add(cellTail);
            TableSearchNpc.Rows.Add(rowTail);
        }
        return true;
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

public class NpcDropItem
{
    public int npcId = 0;

    public string itemTemplateId = string.Empty;

    public UInt32 num = 0;

    public NpcDropItem()
    {
    }
}