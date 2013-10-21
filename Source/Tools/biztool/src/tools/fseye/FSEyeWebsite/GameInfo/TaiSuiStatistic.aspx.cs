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
using System.Drawing;
using ZedGraph;
using Resources;
using FSEye;
using FSEye.FS2Lib;
using FSEye.Security;

public partial class GameInfo_TaiSuiStatistic : BasePage
{
    TaiSuiUseInfo[] _statInfo;
    int _totalCount;
    DateTime _start;
    DateTime _end;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticTaiSuiStatistic, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
            StartDate.SelectedDate = DateTime.Today.AddMonths(-1);
            EndDate.SelectedDate = DateTime.Now;
        }

        LabelOpMsg.Text = string.Empty;
        PanelResult.Visible = false;
        ZedGraphWebStatistic.Visible = false;
    }

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }

    protected void ButtonStatistic_Click(object sender, EventArgs e)
    {
        Query();
    }

    void Query()
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

            searchCondition.Append(string.Format(" WHERE {0} = '{1}' ", FS2TableString.LogFieldLogEvent, LogEvent.TaiSuiStatistic));

            string roleName = TextBoxRoleName.Text.Trim();
            WebUtil.ValidateValueString(roleName);
            if (roleName.Length > 0)
            {
                string subTable = "(SELECT GUID FROM rolesfirst WHERE RoleName='?')";
                searchCondition.Append(string.Format(" AND {0} IN {1} ", FS2TableString.LogFieldLogKey1,subTable));
                paramList.Add(roleName);
            }

            string type = string.Empty;
            switch (RadioButtonListType.SelectedValue)
            {
                case "Day":
                    type = "DATE(LogTime)";
                    break;
                case "Month":
                    type = "DATE_FORMAT(LogTime,'%Y-%m')";
                    break;
            }

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
            if (_start >= _end)
            {
                LabelOpMsg.Text = StringDef.EndTimeError;
                return;
            }

            searchCondition.AppendFormat(" AND {0}>='{1}' AND {0}<'{2}'", FS2TableString.LogFieldLogTime,
                _start.ToString("yyyy-MM-dd HH:mm:ss"), _end.ToString("yyyy-MM-dd HH:mm:ss"));

            string baseCmdString = string.Format("SELECT {0},SUM(LogKey6) FROM {1} {2} GROUP BY {0}",
                type, "{0}", searchCondition.ToString());

            string cmdString;
            WebUtil.AddTableNameToCmd(CurrentUser.Id, server, baseCmdString, paramList, _start, _end, out cmdString, out paramList);
            if (cmdString.Length == 0)
            {
                //
                return;
            }

            SqlCommand cmd = new SqlCommand(cmdString, paramList.ToArray());
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                result.SetFieldType(new SqlDataType[]{
                                    SqlDataType.DateTime,
                                    SqlDataType.Int32
                                    });

                object[] record = null;
                ArrayList infos = new ArrayList();
                int total = 0;
                while ((record = result.ReadRecord()) != null)
                {
                    TaiSuiUseInfo info = new TaiSuiUseInfo();
                    info.date = (DateTime)record[0];
                    info.num = (int)record[1];

                    total += info.num;
                    infos.Add(info);
                }

                if (infos.Count != 0)
                {
                    ZedGraphWebStatistic.Visible = true;
                    PanelResult.Visible = true;
                    _statInfo = (TaiSuiUseInfo[])infos.ToArray(typeof(TaiSuiUseInfo));
                    _totalCount = total;

                    //CreateTableHead();
                    //CreateSearchResultList((TaiSuiUseInfo[])infos.ToArray(typeof(TaiSuiUseInfo)), total);
                    LabelResult.Text = string.Format(StringDef.LabelStatisticResult, server.Group.Name, server.Name,
                        roleName.Length == 0 ? string.Empty : string.Format("[{0}]{1}", roleName, StringDef.Use), StringDef.TaiSui);
                }
                else
                {
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

    //void CreateTableHead()
    //{
    //    if(TextBoxRoleName.Text!=null&&TextBoxRoleName.Text.Trim().Length!=0)
    //    {
    //        TableRow rowHead = new TableRow();

    //        TableHeaderCell cellHead = new TableHeaderCell();
    //        cellHead.Text = StringDef.RoleName;
    //        rowHead.Cells.Add(cellHead);

    //        cellHead = new TableHeaderCell();
    //        cellHead.Text = StringDef.Date;
    //        rowHead.Cells.Add(cellHead);

    //        cellHead = new TableHeaderCell();
    //        cellHead.Text = StringDef.Count;
    //        rowHead.Cells.Add(cellHead);

    //        cellHead = new TableHeaderCell();
    //        cellHead.Text = StringDef.Percentage;
    //        rowHead.Cells.Add(cellHead);

    //        TableSearchTaiSuiList.Rows.Add(rowHead);
    //    }
    //    else 
    //    {
    //        TableRow rowHead = new TableRow();

    //        TableHeaderCell cellHead = new TableHeaderCell();
    //        cellHead.Text = StringDef.Date;
    //        rowHead.Cells.Add(cellHead);

    //        cellHead = new TableHeaderCell();
    //        cellHead.Text = StringDef.Count;
    //        rowHead.Cells.Add(cellHead);

    //        cellHead = new TableHeaderCell();
    //        cellHead.Text = StringDef.Percentage;
    //        rowHead.Cells.Add(cellHead);

    //        TableSearchTaiSuiList.Rows.Add(rowHead);
    //    }
    //}

    //void CreateSearchResultList(TaiSuiUseInfo[] infos, int total)
    //{
    //    if (infos != null)
    //    {
    //        if (TextBoxRoleName.Text != null && TextBoxRoleName.Text.Trim().Length != 0)
    //        {
    //            foreach (TaiSuiUseInfo info in infos)
    //            {
    //                TableRow row = new TableRow();
    //                TableCell cell = new TableCell();
    //                cell.Text = TextBoxRoleName.Text.Trim();
    //                row.Cells.Add(cell);
    //                cell = new TableCell();
    //                cell.Text = info.date.ToShortDateString();
    //                row.Cells.Add(cell);
    //                cell = new TableCell();
    //                cell.Text = info.num.ToString();
    //                row.Cells.Add(cell);
    //                cell = new TableCell();
    //                cell.Text = total == 0 ? "0" : Decimal.Round((decimal)info.num / total, 2) * 100 + "%";
    //                row.Cells.Add(cell);
    //                TableSearchTaiSuiList.Rows.Add(row);
    //            }
    //        }
    //        else
    //        {
    //            foreach (TaiSuiUseInfo info in infos)
    //            {
    //                TableRow row = new TableRow();
    //                TableCell cell = new TableCell();
    //                cell.Text = info.date.ToShortDateString();
    //                row.Cells.Add(cell);
    //                cell = new TableCell();
    //                cell.Text = info.num.ToString();
    //                row.Cells.Add(cell);
    //                cell = new TableCell();
    //                cell.Text = total == 0 ? "0" : Decimal.Round((decimal)info.num / total, 2) * 100 + "%";
    //                row.Cells.Add(cell);
    //                TableSearchTaiSuiList.Rows.Add(row);
    //            }
    //        }
    //        TableRow rowTail = new TableRow();
    //        TableCell cellTail = new TableCell();
    //        cellTail.ColumnSpan = TextBoxRoleName.Text != null && TextBoxRoleName.Text.Trim().Length != 0 ? 4 : 3;
    //        cellTail.Font.Bold = true;
    //        cellTail.Text = StringDef.Total + StringDef.Colon + total;
    //        rowTail.Cells.Add(cellTail);

    //        TableSearchTaiSuiList.Rows.Add(rowTail);
    //    }
    //}

    protected void ZedGraphWebTaiSuiStatistic_RenderGraph(ZedGraph.Web.ZedGraphWeb webObject,Graphics g, MasterPane masterPane)
    {
        if (_statInfo != null)
        {
            //整理数据
            double[] timeArray = null;
            double[] counts = null;

            if (RadioButtonListType.SelectedValue.Equals("Day"))
            {
                //按天
                TimeSpan span = _end.Subtract(_start);
                timeArray = new double[span.Days + 1];
                counts = new double[span.Days + 1];

                for (int index = 0; index != span.Days + 1; ++index)
                {
                    DateTime tempDate = _start.Date.AddDays(index);
                    timeArray[index] = new XDate(tempDate);
                    foreach (TaiSuiUseInfo info in _statInfo)
                    {
                        //如果相等赋num,如果比date的日期小则结束
                        if (tempDate.Date.Equals(info.date))
                        {
                            counts[index] = info.num;
                            break;
                        }
                        else if (tempDate.Date < info.date)
                        {
                            break;
                        }
                    }
                }
            }
            else
            {
                //按月
                int monthLength = (_end.Year - _start.Year) * 12 + (_end.Month - _start.Month);
                timeArray = new double[monthLength + 1];
                counts = new double[monthLength + 1];

                for (int index = 0; index != monthLength + 1; ++index)
                {
                    DateTime tempDate = _start.Date.AddMonths(index);
                    timeArray[index] = new XDate(tempDate);
                    foreach (TaiSuiUseInfo info in _statInfo)
                    {
                        //如果相等赋num,如果比date的日期小则结束
                        if (tempDate.ToString("yyyyMM").Equals(info.date.ToString("yyyyMM")))
                        {
                            counts[index] = info.num;
                            break;
                        }
                        else if (tempDate.Date < info.date)
                        {
                            break;
                        }
                    }
                }
            }

            GraphPane graphPane = masterPane[0];

            //绘制图表
            graphPane.Title.Text = string.Format("{0} {1} {2} {3}", _start.ToShortDateString(), StringDef.To, _end.ToShortDateString(),
                StringDef.TaiSui + StringDef.Total + StringDef.Colon + _totalCount.ToString());
            graphPane.Title.IsVisible = true;
            graphPane.Title.FontSpec.Size = 14;

            graphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);
            graphPane.Legend.FontSpec.Fill.IsVisible = false;
            graphPane.Legend.FontSpec.Size = 10.5f;
            graphPane.Legend.Fill.IsVisible = false;
            graphPane.Legend.Border.IsVisible = false;

            graphPane.YAxis.Title.Text = StringDef.UseCount;
            graphPane.YAxis.Title.FontSpec.IsBold = false;
            graphPane.YAxis.Title.FontSpec.Size = 10.5f;
            graphPane.YAxis.Scale.FontSpec.Size = 10.5f;
            graphPane.YAxis.MajorGrid.IsVisible = true;
            graphPane.YAxis.MajorGrid.DashOff = 0;
            graphPane.YAxis.MajorGrid.Color = Color.Gray;
            graphPane.YAxis.MinorGrid.IsVisible = true;
            graphPane.YAxis.MinorGrid.Color = Color.LightGray;
            graphPane.YAxis.MinorGrid.DashOff = 0;

            graphPane.XAxis.Title.Text = StringDef.Date;
            graphPane.XAxis.Title.FontSpec.IsBold = false;
            graphPane.XAxis.Title.FontSpec.Size = 10.5f;
            graphPane.XAxis.Scale.MajorUnit = DateUnit.Day;
            graphPane.XAxis.MinorGrid.IsVisible = false;            
            graphPane.XAxis.Type = AxisType.DateAsOrdinal;
            if (RadioButtonListType.SelectedValue.Equals("Day"))
                graphPane.XAxis.Scale.Format = "MM-dd";
            else
                graphPane.XAxis.Scale.Format = "yyyy-MM";
            graphPane.XAxis.Scale.FontSpec.Size = 10.5f;
            //graphPane.XAxis.Scale.FontSpec.Angle = 45;

            graphPane.BarSettings.Type = BarType.Cluster;

            BarItem barItem = graphPane.AddBar(StringDef.UseCount, timeArray, counts, Color.Blue);
            masterPane.AxisChange(g);
            BarItem.CreateBarLabels(graphPane, false, string.Empty, TextObj.Default.FontFamily, 10.5f,
                TextObj.Default.FontColor, false, false, false);
        }
    }
}

public class TaiSuiUseInfo
{
    public int num = 0;

    public DateTime date = DateTime.MinValue;

    public TaiSuiUseInfo()
    {
    }
}