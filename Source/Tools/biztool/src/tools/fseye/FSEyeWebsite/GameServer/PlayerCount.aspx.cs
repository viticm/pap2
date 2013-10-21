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
using ZedGraph;
using Resources;
using FSEye.PlugIn;
using FSEye.Security;
using FSEye;

public partial class GameServer_PlayerCount : BasePage
{
	int _dayIndex = 0;

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerPlayerCount, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (!IsPostBack)
		{
			string serverIdText = Request.Params[WebConfig.ParamServerId];
            if (serverIdText != null)
            {
                int serverId = int.Parse(serverIdText);
                GameServer server = TheAdminServer.GameServerManager.GetGameServer(serverId);
                ServerGroupDropDownList.SelectedServerGroup = server.Group;
                ServerDropDownList.CreateServerList(server.Group);
                ServerDropDownList.SelectedGameServer = server;
            }
            else
			{
                ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
            }

			_dayIndex = 0;
			Session[WebConfig.SessionDayIndex] = _dayIndex;
		}
		else
		{
			_dayIndex = (int)Session[WebConfig.SessionDayIndex];
		}

		RefreshDayButton();
	}

	protected void ButtonRefresh_Click(object sender, EventArgs e)
	{
	}

	protected void ButtonPreviousDay_Click(object sender, EventArgs e)
	{
		_dayIndex--;
		Session[WebConfig.SessionDayIndex] = _dayIndex;
		RefreshDayButton();
	}

	protected void ButtonNextDay_Click(object sender, EventArgs e)
	{
		_dayIndex++;
		Session[WebConfig.SessionDayIndex] = _dayIndex;
		RefreshDayButton();
	}

	protected void ButtonNow_Click(object sender, EventArgs e)
	{
		_dayIndex = 0;
		Session[WebConfig.SessionDayIndex] = _dayIndex;
		RefreshDayButton();
	}

	void RefreshDayButton()
	{
		ButtonNextDay.Enabled = (_dayIndex < 0);
		ButtonNow.Enabled = (_dayIndex != 0);
	}

	protected void ZedGraphWebPlayerCount_RenderGraph(ZedGraph.Web.ZedGraphWeb webObject, Graphics g, MasterPane masterPane)
	{
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.READ, Session))
        {
            return;
        }
        
        GameServer server = ServerDropDownList.SelectedGameServer;

        //不再只对GameServer进行人数的查询，而是对所有的Server进行查询
        if (server == null || server.Type != GameServer.ServerType.gameserver && server.Type != GameServer.ServerType.gamecenter)
        {
            ZedGraphWebPlayerCount.Visible = false;
            return;
        }


        Queue<LordControl.PlayerCountInfo> playerCountInfoQueue = server.GetPlugInData(CurrentUser.Id, LordControl.PlugInGuid, LordControl.DataKeyPlayerCountQueue) as Queue<LordControl.PlayerCountInfo>;
		if (playerCountInfoQueue == null)
		{
			ZedGraphWebPlayerCount.Visible = false;
			return;
		}

		LordControl.PlayerCountInfo[] infoArray = playerCountInfoQueue.ToArray();
		double[] countArray = new double[infoArray.Length];
		double[] timeArray = new double[infoArray.Length];

		for (int i = 0; i < infoArray.Length; i++)
		{
			LordControl.PlayerCountInfo info = infoArray[i];
			countArray[i] = info.Count;
			timeArray[i] = new XDate(info.Time);
		}

		GraphPane graphPane = masterPane[0];

		graphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);

		graphPane.Legend.Fill.IsVisible = false;
		graphPane.Legend.Border.IsVisible = false;
		graphPane.Legend.FontSpec.Fill.IsVisible = false;

		graphPane.XAxis.Title.Text = StringDef.Time;
		graphPane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;
		graphPane.XAxis.Type = AxisType.Date;		
		graphPane.XAxis.Scale.FontSpec.Size = 11;

		graphPane.YAxis.Title.Text = StringDef.PlayerCount;
        //graphPane.YAxis.MajorGrid.IsVisible = true;
        //graphPane.YAxis.MajorGrid.DashOff = 0;
        //graphPane.YAxis.MajorGrid.Color = Color.Gray;
        //graphPane.YAxis.MinorGrid.IsVisible = true;
        //graphPane.YAxis.MinorGrid.Color = Color.LightGray;
        //graphPane.YAxis.MinorGrid.DashOff = 0;
		graphPane.YAxis.Scale.Min = 0;

		graphPane.Title.Text = string.Format("{0} - {1}", StringDef.PlayerCount, StringDef.Last1Hour);

		graphPane.AddCurve(StringDef.PlayerCount, timeArray, countArray, Color.Red, SymbolType.Diamond);
	}

	protected void ZedGraphWebPlayerCountStatistic_RenderGraph(ZedGraph.Web.ZedGraphWeb webObject, Graphics g, MasterPane masterPane)
	{
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.READ, Session))
        {
            return;
        }
        
        GameServer server = ServerDropDownList.SelectedGameServer;

        //不再只对GameServer进行人数的查询，而是对所有的Server进行查询
        if (server == null || server.Type != GameServer.ServerType.gameserver && server.Type != GameServer.ServerType.gamecenter)
        {
            ZedGraphWebPlayerCount.Visible = false;
            return;
        }

		LordControl.PlayerCountStatisticInfo[] infoArray = server.GetPlugInData(CurrentUser.Id,LordControl.PlugInGuid, LordControl.DataKeyPlayerCountStatistic, DateTime.Now.Subtract(new TimeSpan(1 - _dayIndex, 0, 0, 0)), DateTime.Now.Subtract(new TimeSpan(-_dayIndex, 0, 0, 0))) as LordControl.PlayerCountStatisticInfo[];
		if (infoArray == null)
		{
			ZedGraphWebPlayerCount.Visible = false;
			return;
		}

		double[] maxCountArray = new double[infoArray.Length];
		double[] minCountArray = new double[infoArray.Length];
		double[] averageCountArray = new double[infoArray.Length];
		double[] timeArray = new double[infoArray.Length];

		for (int i = 0; i < infoArray.Length; i++)
		{
			LordControl.PlayerCountStatisticInfo info = infoArray[i];			
			maxCountArray[i] = info.MaxCount;
			minCountArray[i] = info.MinCount;
			averageCountArray[i] = info.AverageCount;
			timeArray[i] = new XDate(info.Time);
		}

		GraphPane graphPane = masterPane[0];

		graphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);

		graphPane.Legend.Fill.IsVisible = false;
		graphPane.Legend.Border.IsVisible = false;
		graphPane.Legend.FontSpec.Fill.IsVisible = false;

		graphPane.XAxis.Title.Text = StringDef.Time;
		graphPane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;
		graphPane.XAxis.Type = AxisType.Date;
		graphPane.XAxis.Scale.FontSpec.Size = 11;

		graphPane.YAxis.Title.Text = StringDef.PlayerCount;
        //graphPane.YAxis.MajorGrid.IsVisible = true;
        //graphPane.YAxis.MajorGrid.DashOff = 0;
        //graphPane.YAxis.MajorGrid.Color = Color.Gray;
        //graphPane.YAxis.MinorGrid.IsVisible = true;
        //graphPane.YAxis.MinorGrid.Color = Color.LightGray;
        //graphPane.YAxis.MinorGrid.DashOff = 0;
		graphPane.YAxis.Scale.Min = 0;

		graphPane.Title.Text = string.Format("{0} [ {1}  {2} ]", StringDef.PlayerCount, DateTime.Now.Subtract(new TimeSpan(1 - _dayIndex, 0, 0, 0)), DateTime.Now.Subtract(new TimeSpan(-_dayIndex, 0, 0, 0)));

		graphPane.AddCurve(StringDef.Maximum, timeArray, maxCountArray, Color.Red, SymbolType.Triangle);
		graphPane.AddCurve(StringDef.Minimum, timeArray, minCountArray, Color.Green, SymbolType.TriangleDown);
		graphPane.AddCurve(StringDef.Average, timeArray, averageCountArray, Color.Orange, SymbolType.Diamond);
	}

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }
}
