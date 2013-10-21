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

public partial class GameInfo_Mail : BasePage
{
    private const int DefaultRecordPerPage = 50;
    int _recordPerPage = DefaultRecordPerPage;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticMailStatistic, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
            LiteralRemove.Text = StringDef.Sender + StringDef.FilterNegative;

            StartDate.SelectedDate = DateTime.Today;
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

    protected void ButtonFirstPage_Click(object sender, EventArgs e)
    {
        Query(0);
    }

    protected void ButtonPreviousPage_Click(object sender, EventArgs e)
    {
        int offset = (int)Session[WebConfig.SessionQueryLogOffset];
        if (offset > 0)
        {
            offset -= _recordPerPage;
        }
        if (offset < 0)
            offset = 0;

        Query(offset);
    }

    protected void ButtonNextPage_Click(object sender, EventArgs e)
    {
        int offset = (int)Session[WebConfig.SessionQueryLogOffset];
        offset += _recordPerPage;

        Query(offset);
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
            ArrayList paramList = new ArrayList();
            StringBuilder searchCondition = new StringBuilder();

            //searchCondition.Append(string.Format(" AND {0}>='{1}' AND {0} <'{2}'", FS2TableString.LogFieldLogTime, StartDate.SelectedDate, EndDate.SelectedDate.AddDays(-1)));

            if (StartDate.Selected)
            {
                searchCondition.Append(string.Format(" AND {0}>='{1}'", FS2TableString.MailFieldPostTime, StartDate.SelectedDate));
            }

            if (EndDate.Selected)
            {
                searchCondition.Append(string.Format(" AND {0}<'{1}'", FS2TableString.MailFieldPostTime, EndDate.SelectedDate));
            }

            string sender = TextBoxSender.Text;
            WebUtil.ValidateValueString(sender);
            if (sender.Length > 0)
            {
                if (CheckBoxSender.Checked)
                {
                    searchCondition.Append(string.Format(" AND {0}='?'", FS2TableString.MailFieldSender));
                    paramList.Add(sender);
                }
                else
                {
                    searchCondition.Append(string.Format(" AND {0} LIKE '%?%'", FS2TableString.MailFieldSender));
                    paramList.Add(sender);
                }                
            }

            string receiver = TextBoxReceiver.Text;
            WebUtil.ValidateValueString(receiver);
            if (receiver.Length > 0)
            {
                if (CheckBoxReceiver.Checked)
                {                        
                    searchCondition.Append(string.Format(" AND {0}='?'", FS2TableString.MailFieldReceiver));
                    paramList.Add(receiver);
                }
                else
                {
                    searchCondition.Append(string.Format(" AND {0} LIKE '%?%'", FS2TableString.MailFieldReceiver));
                    paramList.Add(receiver);
                }
            }

            string filterNegative = TextBoxFilter.Text.Trim();
            if (filterNegative != null && filterNegative.Length != 0)
            {
                if (CheckBoxFilter.Checked)
                {
                    searchCondition.Append(string.Format(" AND {0}!='?'", FS2TableString.MailFieldSender));
                    paramList.Add(filterNegative);
                }
                else
                {
                    searchCondition.Append(string.Format(" AND {0} NOT LIKE '%?%'", FS2TableString.MailFieldSender));
                    paramList.Add(filterNegative);
                }
            }

            string filterNegative1 = TextBoxFilter1.Text.Trim();
            if (filterNegative1 != null && filterNegative1.Length != 0)
            {
                if (CheckBoxFilter1.Checked)
                {
                    searchCondition.Append(string.Format(" AND {0}!='?'", FS2TableString.MailFieldSender));
                    paramList.Add(filterNegative1);
                }
                else
                {
                    searchCondition.Append(string.Format(" AND {0} NOT LIKE '%?%'", FS2TableString.MailFieldSender));
                    paramList.Add(filterNegative1);
                }
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
                    orderByStatement = string.Format(" ORDER BY {0} {1}", FS2TableString.MailFieldPostTime, orderByType);
                    break;
                case 1:
                    orderByStatement = string.Format(" ORDER BY {0} {1}", FS2TableString.MailFieldSender, orderByType);
                    break;
                case 2:
                    orderByStatement = string.Format(" ORDER BY {0} {1}", FS2TableString.MailFieldReceiver, orderByType);
                    break;
            }

            if (searchCondition.Length != 0)
            {
                searchCondition.Remove(0, 4);
                searchCondition.Insert(0, "WHERE ");
            }

            int limitCount = _recordPerPage;
            string limitStatement = string.Format("LIMIT {0},{1}", offset, limitCount);

            string cmdString = "SELECT {0} FROM {1} {2} {3} {4}";
            string cmdFieldString = string.Format("{0},{1},{2},{3}",FS2TableString.MailFieldSender,FS2TableString.MailFieldReceiver,
                                                   FS2TableString.MailFieldPostTime,FS2TableString.MailFieldMailData);
            string cmdCountFieldString = "COUNT(*)";

            SqlCommand cmd = new SqlCommand(string.Format(cmdString, cmdFieldString, FS2TableString.MailTableName, searchCondition.ToString(),orderByStatement, limitStatement), paramList.ToArray());
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                Session[WebConfig.SessionQueryLogOffset] = offset;
                result.SetFieldType(new SqlDataType[]{
                                    SqlDataType.String,
                                    SqlDataType.String,
                                    SqlDataType.DateTime,
                                    SqlDataType.String
                                    });
                object[] record = null;
                ArrayList infos = new ArrayList();
                while ((record = result.ReadRecord()) != null)
                {
                    MailInfo info = new MailInfo();
                    info.sender = record[0] as string;
                    info.receiver = record[1] as string;
                    info.postTime = (DateTime)record[2];
                    info.mailData = record[3] as string;

                    infos.Add(info);
                }

                ButtonPreviousPage.Enabled = (offset > 0);
                ButtonFirstPage.Enabled = (offset > 0);
                ButtonNextPage.Enabled = (infos.Count >= limitCount);


                //单独查询总数,只有第一页显示
                if (offset == 0)
                {
                    SqlCommand cmdCount = new SqlCommand(string.Format(cmdString, cmdCountFieldString, FS2TableString.MailTableName, searchCondition.ToString(), string.Empty, string.Empty), paramList.ToArray());
                    if (!server.IsConnected)
                    {
                        LabelOpMsg.Text = StringDef.NoConnectionAlert;
                        return;
                    }
                    SqlResult resultCount = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmdCount);
                    if (resultCount.ResultState == SqlResult.State.Done && resultCount.Success)
                    {
                        resultCount.SetFieldType(new SqlDataType[]{
                                             SqlDataType.Int32
                    });
                        object[] count = resultCount.ReadRecord();
                        TableRow rowHead = new TableRow();
                        TableCell cellHead = new TableCell();
                        cellHead.Font.Bold = true;
                        cellHead.ColumnSpan = 4;
                        cellHead.Text = StringDef.Total + StringDef.Colon + (int)count[0];
                        rowHead.Cells.Add(cellHead);
                        TableSearchMailList.Rows.Add(rowHead);
                    }
                }
                bool success = CreateSearchResultList((MailInfo[])infos.ToArray(typeof(MailInfo)));
                PanelResult.Visible = success;

                if (!success)
                {
                    LabelOpMsg.Text = StringDef.NoMatchingRecord;
                }
                else
                {
                    LabelResult.Text = string.Format(StringDef.LabelStatisticResult, server.Group.Name, server.Name,
                                            string.Empty, StringDef.Mail);
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

    bool CreateSearchResultList(MailInfo[] infos)
    {
        if (infos != null)
        {
            foreach (MailInfo info in infos)
            {
                TableRow row = new TableRow();

                TableCell cell = new TableCell();
                cell.Text = info.sender;
                row.Cells.Add(cell);

                cell = new TableCell();
                cell.Text = info.receiver;
                row.Cells.Add(cell);

                cell = new TableCell();
                cell.Text = info.postTime.ToString();
                row.Cells.Add(cell);

                cell = new TableCell();
                if (info.mailData.Length >= 30)
                {
                    cell.Text = info.mailData.Substring(0, 30) + "......";
                    cell.ToolTip = info.mailData;
                }
                else
                    cell.Text = info.mailData.Length == 1 ? "&nbsp;" : info.mailData;
                row.Cells.Add(cell);

                TableSearchMailList.Rows.Add(row);
            }
            return true;
        }
        return false;
    }
}

public class MailInfo
{
    public string sender;

    public string receiver;

    public DateTime postTime;

    public string mailData;

    public MailInfo()
    {        
    }
}