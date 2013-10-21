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
using System.Drawing;
using System.Text;
using ZedGraph;
using ZedGraph.Web;
using Resources;
using FSEye;
using FSEye.Security;
using FSEye.FS2Lib;

public partial class GameInfo_NpcDeathStatistic : BasePage
{
	const int EntryPerPage = 50;
    IList<NpcStatisticInfo> _recordList = null;
    DateTime _start;
    DateTime _end;

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticNpcDeathStatistic, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (!IsPostBack)
		{            
            StartDate.SelectedDate = DateTime.Today.AddMonths(-1);
            EndDate.SelectedDate = DateTime.Now;
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
		}
        LabelOpMsg.Text = string.Empty;
        PanelResult.Visible = false;
	}

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
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
                    offset -= EntryPerPage;
                    break;
                case "NextPage":
                    offset += EntryPerPage;
                    break;
            }
            Statistic(offset);
        }
    }

	protected void ButtonStatistic_Click(object sender, EventArgs e)
	{
        Statistic(0);
	}

    private void Statistic(int offset)
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

        if (_start > _end)
        {
            LabelOpMsg.Text = StringDef.EndTimeError;
            return;
        }

        switch (ListBoxViewContent.SelectedValue)
        {
            case "NpcBeKilled":
                _recordList = QueryNpc(server, _start, _end, LogEvent.NpcKilledByPlayerStatistic, offset);
                break;
            case "NpcKill":
                _recordList = QueryNpc(server, _start, _end, LogEvent.NpcKillPlayerStatistic, offset);
                break;
        }

        if (_recordList == null)
        {
            PanelResult.Visible = false;
            LabelOpMsg.Text = StringDef.OperationFail;
        }
        else if (_recordList.Count == 0)
        {
            PanelResult.Visible = false;
            LabelOpMsg.Text = StringDef.NoMatchingRecord;
        }
        else
        {
            PanelResult.Visible = true;
        }
    }
        
    IList<NpcStatisticInfo> QueryNpc(GameServer server, DateTime startTime, DateTime endTime, int logEvent,int offset)
	{
        string baseCmdString = string.Format("SELECT LogKey1, SUM(LogKey6) AS SubSum FROM {0} WHERE {1}='{2}' AND {3}>='{4}' AND {3}<'{5}' GROUP BY LogKey1",
                "{0}", FS2TableString.LogFieldLogEvent, logEvent, FS2TableString.LogFieldLogTime,
                _start.ToString("yyyy-MM-dd HH:mm:ss"), _end.ToString("yyyy-MM-dd HH:mm:ss"));
        string addTableCmdString;
        WebUtil.AddTableNameToCmd(CurrentUser.Id, server, baseCmdString, _start, _end, out addTableCmdString);

        if (addTableCmdString.Length == 0)
        {
            return null;
        }

        string cmdText = string.Format("SELECT LogKey1, SUM(SubSum) AS ByKillCount FROM ({0}) AS A GROUP BY LogKey1 ORDER BY ByKillCount DESC LIMIT {1},{2};",
                                        addTableCmdString, offset, EntryPerPage);
        SqlResult queryResult = WebUtil.QueryGameServerDb(CurrentUser.Id,server, new SqlCommand(cmdText));
		if (queryResult != null && queryResult.Success)
		{
			queryResult.SetFieldType(new SqlDataType[] {
                            SqlDataType.String, 
                            SqlDataType.Int32
                            });

            IList<NpcStatisticInfo> infos = new List<NpcStatisticInfo>();
			object[] record = queryResult.ReadRecord();
			while (record != null)
			{
				NpcStatisticInfo info = new NpcStatisticInfo();
				info.TemaplteId = (string)record[0];
				info.Count = (int)record[1];
				infos.Add(info);

				record = queryResult.ReadRecord();
			}

            ViewState[WebConfig.SessionQueryLogOffset] = offset;

            ButtonPreviousPage.Enabled = (offset > 0);
            ButtonFirstPage.Enabled = (offset > 0);
            ButtonNextPage.Enabled = (infos.Count >= EntryPerPage);

            return infos;
		}
		else
		{
            if (queryResult == null)
                LabelOpMsg.Text = StringDef.QueryTimeOut;
            else
                LabelOpMsg.Text = StringDef.OperationFail;
            return null;
		}
	}    	

    protected void ZedGraphWebExpStatistic_RenderGraph(ZedGraph.Web.ZedGraphWeb webObject, Graphics g, MasterPane masterPane)
    {
        Color statColor = Color.Blue;
        string statTitle = string.Empty;
        string statXAxisTitle = string.Empty;
        switch (ListBoxViewContent.SelectedValue)
        {
            case "NpcBeKilled":
                statColor = Color.Orange;
                statTitle = StringDef.NpcBeKilledStatistic;
                statXAxisTitle = StringDef.NpcBeKilledCount;
                break;
            case "NpcKill":
                statColor = Color.Red;
                statTitle = StringDef.NpcKillPlayerStatistic;
                statXAxisTitle = StringDef.NpcKillPlayerCount;
                break;
        }

        GraphPane graphPane = masterPane[0];
        graphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);
        graphPane.Title.Text = statTitle;
        graphPane.Title.FontSpec.Family = StringDef.DefaultFontFamily;
        graphPane.Title.FontSpec.Size = 10;
        graphPane.Legend.IsVisible = false;
        graphPane.BarSettings.Base = BarBase.Y;

        graphPane.XAxis.Title.Text = statXAxisTitle;
        graphPane.XAxis.Title.FontSpec.Family = StringDef.DefaultFontFamily;
        graphPane.XAxis.Title.FontSpec.Size = 6.2f;
        graphPane.XAxis.MajorGrid.IsVisible = true;
        graphPane.XAxis.MajorGrid.DashOff = 0;
        graphPane.XAxis.MajorGrid.Color = Color.Gray;
        graphPane.XAxis.MinorGrid.IsVisible = true;
        graphPane.XAxis.MinorGrid.Color = Color.LightGray;
        graphPane.XAxis.MinorGrid.DashOff = 0;
        graphPane.XAxis.Scale.FontSpec.Size = 5.6f;
        graphPane.XAxis.Scale.FontSpec.Family = StringDef.DefaultFontFamily;

        //graphPane.YAxis.Title.Text = StringDef.NpcTemplate;
        graphPane.YAxis.Title.FontSpec.Family = StringDef.DefaultFontFamily;
        graphPane.YAxis.MajorTic.IsBetweenLabels = true;
        graphPane.YAxis.Scale.IsPreventLabelOverlap = false;
        graphPane.YAxis.Scale.AlignH = AlignH.Center;
        graphPane.YAxis.Scale.FontSpec.Size = 5.6f;
        graphPane.YAxis.Scale.FontSpec.Family = StringDef.DefaultFontFamily;
        graphPane.YAxis.Scale.IsReverse = true;
        graphPane.YAxis.Type = AxisType.Text;

        double[] countList = new double[_recordList.Count];
        string[] templateList = new string[_recordList.Count];
        for (int i = 0; i < _recordList.Count; i++)
        {
            NpcStatisticInfo info = _recordList[i];
            countList[i] = info.Count;
            FS2NpcData npcData = FS2GameDataManager.TheInstance.GetNpcData(int.Parse(info.TemaplteId));
            if (npcData != null)
            {
                templateList[i] = npcData.ToString();
            }
            else
            {
                templateList[i] = info.TemaplteId;
            }
        }
        BarItem barItem = graphPane.AddBar(StringDef.NpcBeKilledCount, countList, null, Color.Blue);
        barItem.Bar.Fill = new Fill(statColor);
        graphPane.YAxis.Scale.TextLabels = templateList;
        masterPane.AxisChange();
        BarItem.CreateBarLabels(graphPane, false, string.Empty, StringDef.DefaultFontFamily, 5.6f, TextObj.Default.FontColor, false, false, false);
    }


    //void NpcBeKilledStatistic(System.Drawing.Graphics g, ZedGraph.MasterPane masterPane)
    //{
    //    GameServer server = ServerDropDownList.SelectedGameServer;

    //    DateTime startTime = StartDate.SelectedDate;
    //    DateTime endTime = EndDate.SelectedDate;
    //    IList<NpcStatisticInfo> recordList = QueryNpcBeKilled(server, startTime, endTime);
    //    if (recordList == null || recordList.Count == 0) return;


    //}

    //void NpcKillPlayerStatistic(System.Drawing.Graphics g, ZedGraph.MasterPane masterPane)
    //{
    //    GameServer server = ServerDropDownList.SelectedGameServer;
    //    //加入权限检查
    //    if (!WebUtil.CheckPrivilege(server.SecurityObject, OpType.READ, Session))
    //    {
    //        Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
    //    }
    //    DateTime startTime = StartDate.SelectedDate;
    //    DateTime endTime = EndDate.SelectedDate;
    //    IList<NpcStatisticInfo> recordList = QueryNpcKillPlayer(server, startTime, endTime);
    //    if (recordList == null || recordList.Count == 0) return;

    //    GraphPane graphPane = masterPane[0];
    //    graphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);
    //    graphPane.Title.Text = StringDef.NpcKillPlayerStatistic;
    //    graphPane.Title.FontSpec.Family = StringDef.DefaultFontFamily;
    //    graphPane.Legend.IsVisible = false;
    //    graphPane.BarSettings.Base = BarBase.Y;

    //    graphPane.XAxis.Title.Text = StringDef.NpcKillPlayerCount;
    //    graphPane.XAxis.Title.FontSpec.Family = StringDef.DefaultFontFamily;
    //    graphPane.XAxis.Scale.FontSpec.Size = 10;
    //    graphPane.XAxis.Scale.FontSpec.Family = StringDef.DefaultFontFamily;
    //    graphPane.XAxis.MajorGrid.IsVisible = true;
    //    graphPane.XAxis.MajorGrid.DashOff = 0;
    //    graphPane.XAxis.MajorGrid.Color = Color.Gray;
    //    graphPane.XAxis.MinorGrid.IsVisible = true;
    //    graphPane.XAxis.MinorGrid.Color = Color.LightGray;
    //    graphPane.XAxis.MinorGrid.DashOff = 0;

    //    graphPane.YAxis.Title.Text = StringDef.NpcTemplate;
    //    graphPane.YAxis.Title.FontSpec.Family = StringDef.DefaultFontFamily;
    //    graphPane.YAxis.MajorTic.IsBetweenLabels = true;
    //    graphPane.YAxis.Scale.IsPreventLabelOverlap = false;
    //    graphPane.YAxis.Scale.FontSpec.Size = 10;
    //    graphPane.YAxis.Scale.FontSpec.Family = StringDef.DefaultFontFamily;
    //    graphPane.YAxis.Scale.IsReverse = true;
    //    graphPane.YAxis.Type = AxisType.Text;

    //    double[] countList = new double[recordList.Count];
    //    string[] templateList = new string[recordList.Count];
    //    for (int i = 0; i < countList.Length; i++)
    //    {
    //        NpcStatisticInfo info = recordList[i];
    //        countList[i] = info.Count;
    //        FS2NpcData npcData = FS2GameDataManager.TheInstance.GetNpcData(int.Parse(info.TemaplteId));
    //        if (npcData != null)
    //        {
    //            templateList[i] = npcData.ToString();
    //        }
    //        else
    //        {
    //            templateList[i] = info.TemaplteId;
    //        }
    //    }
    //    BarItem barItem = graphPane.AddBar(StringDef.NpcKillPlayerCount, countList, null, Color.Red);
    //    barItem.Bar.Fill = new Fill(Color.Red);
    //    graphPane.YAxis.Scale.TextLabels = templateList;
    //}
}

public class NpcStatisticInfo
{
    public string TemaplteId;
    public int Count;

    public NpcStatisticInfo()
    { }
}