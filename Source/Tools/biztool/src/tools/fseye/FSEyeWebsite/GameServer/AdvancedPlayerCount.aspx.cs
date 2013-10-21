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
using FSEye.Security;
using FSEye.PlugIn;
using ZedGraph;
using Resources;
using System.Drawing;
using System.Threading;

public partial class GameServer_AdvancedPlayerCount : BasePage
{
    LordControl.PlayerCountStatisticInfo[] _infoArray = null;
    DateTime _startTime;
    DateTime _endTime;

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerAdvancedPlayerCount, OpType.READ, Session))
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

            CalendarStartTime.SelectedDate = DateTime.Today.AddMonths(-1);
            CalendarEndTime.SelectedDate = DateTime.Now;
		}

        LabelOpMsg.Text = string.Empty;
		ZedGraphWebPlayerCount.Visible = false;
	}

	protected void ButtonShow_Click(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.READ, Session))
        {
            LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            return;
        }

        GameServer server = ServerDropDownList.SelectedGameServer;
        if(server == null)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
            return;
        }

        _startTime = CalendarStartTime.SelectedDate;
        if (_startTime == DateTime.MinValue)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.StartTime);
            return;
        }

        _endTime = CalendarEndTime.SelectedDate;
        if (_endTime == DateTime.MinValue)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.EndTime);
            return;
        }

        //为动画效果而做延迟
        Thread.Sleep(500);
        _infoArray = server.GetPlugInData(CurrentUser.Id, LordControl.PlugInGuid, LordControl.DataKeyPlayerCountStatistic, _startTime, _endTime) as LordControl.PlayerCountStatisticInfo[];
        if (_infoArray != null && _infoArray.Length != 0)
        {
            ZedGraphWebPlayerCount.Visible = true;            
        }
        else
        {
            LabelOpMsg.Text = StringDef.NoInfo;
            return;
        }
    }

	protected void ZedGraphWebPlayerCount_RenderGraph(ZedGraph.Web.ZedGraphWeb webObject, Graphics g, MasterPane masterPane)
    {
        double[] maxCountArray = new double[_infoArray.Length];
        double[] minCountArray = new double[_infoArray.Length];
        double[] averageCountArray = new double[_infoArray.Length];
        double[] timeArray = new double[_infoArray.Length];

        for (int i = 0; i < _infoArray.Length; i++)
        {
            LordControl.PlayerCountStatisticInfo info = _infoArray[i];
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

        graphPane.Title.Text = string.Format("{0} [ {1}  {2} ]", StringDef.PlayerCount, _startTime, _endTime);

        graphPane.AddCurve(StringDef.Maximum, timeArray, maxCountArray, Color.Red, SymbolType.Triangle);
        graphPane.AddCurve(StringDef.Minimum, timeArray, minCountArray, Color.Green, SymbolType.TriangleDown);
        graphPane.AddCurve(StringDef.Average, timeArray, averageCountArray, Color.Orange, SymbolType.Diamond);

        graphPane.AxisChange();
    }

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }
}
