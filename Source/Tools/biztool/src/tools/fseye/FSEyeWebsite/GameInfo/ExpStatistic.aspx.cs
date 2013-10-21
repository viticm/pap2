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
using ZedGraph;
using ZedGraph.Web;
using Resources;
using FSEye;
using FSEye.Security;
using FSEye.FS2Lib;

public partial class GameInfo_ExpStatistic : BasePage
{
    DateTime _start;
    DateTime _end;

	string _roleName;
	string _roleGuid;

	DateTime _createTime;

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticExpStatistic, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

        if (!IsPostBack)
        {
            if (Request.Params[WebConfig.ParamServerId] != null && Request.Params[WebConfig.ParamRoleName] != null)
            {
                try
                {
                    int serverId = int.Parse(Request.Params[WebConfig.ParamServerId]);
                    GameServer server = TheAdminServer.GameServerManager.GetGameServer(serverId);
                    ServerGroupDropDownList.SelectedServerGroup = server.Group;
                    ServerDropDownList.CreateServerList(server.Group);
                    ServerDropDownList.SelectedGameServer = server;
                    TextBoxRoleName.Text = Server.UrlDecode(Request.Params[WebConfig.ParamRoleName]);
                }
                catch (Exception)
                {
                    //Response.Redirect("~/GameMaster/PlayerWho.aspx");
                }
            }
            else
            {                
                ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
            }

            StartDate.SelectedDate = DateTime.Today.AddMonths(-1);
            EndDate.SelectedDate = DateTime.Now;            
        }

        ZedGraphWebExpStatistic.Visible = false;
        LabelOpMsg.Text = string.Empty;
	}

	protected void ButtonStatistic_Click(object sender, EventArgs e)
	{
        try
        {
            GameServer server = ServerDropDownList.SelectedGameServer;
            if (server == null)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
                return;
            }

            if (!WebUtil.CheckPrivilege(server.SecurityObject, OpType.READ, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }

            if (!server.IsConnected)
            {
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                return;
            }			

            _roleName = TextBoxRoleName.Text.Trim();
            if (_roleName == null || _roleName.Length == 0)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.RoleName);
                return;
            }

            //判断起始时间是否理
            _start = StartDate.SelectedDate;
            if (_start == DateTime.MinValue)
            {
                LabelOpMsg.Text = StringDef.ParameterInputError + StringDef.Colon + StringDef.Start;
                return;
            }
            _end = EndDate.SelectedDate;
            if (_end == DateTime.MinValue)
            {
                LabelOpMsg.Text = StringDef.ParameterInputError + StringDef.Colon + StringDef.EndTime;
                return;
            }
            if (_start >= _end)
            {               
                LabelOpMsg.Text = StringDef.EndTimeError;
                return;
            }

            _createTime = DateTime.MinValue;

            string cmdText = string.Format("SELECT {0},{1} FROM {2} WHERE {3}='?'",
                FS2TableString.RolesfirstFieldGUID,
                FS2TableString.RolesfirstFieldCreateDate,
                FS2TableString.RolesfirstTableName,
                FS2TableString.RolesfirstFieldRoleName);

            SqlResult findRoleGuidResult = WebUtil.QueryGameServerDb(CurrentUser.Id, server, new SqlCommand(cmdText, _roleName));
            if (findRoleGuidResult != null && findRoleGuidResult.Success)
            {
                findRoleGuidResult.SetFieldType(new SqlDataType[] { SqlDataType.String, SqlDataType.DateTime });
                object[] record = findRoleGuidResult.ReadRecord();
                if (record != null)
                {
                    _roleGuid = record[0] as string;
                    _createTime = (DateTime)record[1];
                }
            }
            
            if (_roleGuid == null)
            {
                LabelOpMsg.Text = string.Format(StringDef.RoleWithNameNotExist, _roleName);
                return;
            }

            ZedGraphWebExpStatistic.Visible = true;
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = ex.Message;
        }
	}

	protected void ZedGraphWebExpStatistic_RenderGraph(ZedGraph.Web.ZedGraphWeb webObject, Graphics g, MasterPane masterPane)
	{
		switch (ListBoxViewContent.SelectedIndex)
		{
			case 0:
				ExpByPlayTime(g, masterPane);				
				break;
			case 1:
				ExpByDate(g, masterPane);
				break;
		}
	}

	void ExpByPlayTime(System.Drawing.Graphics g, ZedGraph.MasterPane masterPane)
	{
        try
        {
            GameServer server = ServerDropDownList.SelectedGameServer;
            if (server == null)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
                return;
            }
            //加入权限检查
			if (!WebUtil.CheckPrivilege(server.SecurityObject, OpType.READ, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }
            if (!server.IsConnected)
            {
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                return;
            }

            List<int> playTimeList = new List<int>();
            List<int> reachLevelList = new List<int>();

            string baseCmdString = string.Format("SELECT LogKey2, LogKey6,LogTime FROM {0} WHERE {1}='{2}' AND {3}='{4}'",
                "{0}", 
                FS2TableString.LogFieldLogEvent, 
                LogEvent.PlayerUpgrade, 
                FS2TableString.LogFieldLogKey1, 
                _roleGuid);

            string addTableCmdString;
            WebUtil.AddTableNameToCmd(CurrentUser.Id, server, baseCmdString, _start, _end, out addTableCmdString);
            if (addTableCmdString.Length == 0)
            {
                return;
            }

            string cmdString = string.Format("SELECT LogKey2,LogKey6 FROM ({0}) AS A ORDER BY LogTime ASC", addTableCmdString);
            SqlCommand cmd = new SqlCommand(cmdString);
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                result.SetFieldType(new SqlDataType[] { 
                        SqlDataType.String, 
                        SqlDataType.Int32 
                        });
                object[] record = null;
                while ((record = result.ReadRecord()) != null)
                {
                    reachLevelList.Add(int.Parse((string)record[0]));
                    playTimeList.Add((Int32)record[1]);
                }
            }
            else
            {
                if (result == null)
                    LabelOpMsg.Text = StringDef.QueryTimeOut;
                else
                    LabelOpMsg.Text = StringDef.OperationFail;
            }

            int count = playTimeList.Count;
            double[] playTimeArray = new double[count + 1];
            double[] reachLevelArray = new double[count + 1];
            playTimeArray[0] = 0;
            reachLevelArray[0] = 1;
            for (int i = 0; i < count; i++)
            {
                playTimeArray[i + 1] = playTimeList[i] / 60;
                reachLevelArray[i + 1] = reachLevelList[i];
            }

            GraphPane graphPane = masterPane[0];

            graphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);

            graphPane.Legend.Fill.IsVisible = false;
            graphPane.Legend.Border.IsVisible = false;
            graphPane.Legend.FontSpec.Fill.IsVisible = false;

            graphPane.XAxis.Title.Text = string.Format("{0} [{1}]", StringDef.PlayedTime, StringDef.Minute);
            graphPane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;
            graphPane.XAxis.MinorTic.Size = 0;
            graphPane.XAxis.Scale.FontSpec.Size = 12;

            graphPane.YAxis.Title.Text = StringDef.Level;
            graphPane.YAxis.MajorGrid.IsVisible = true;
            graphPane.YAxis.MajorGrid.DashOff = 0;
            graphPane.YAxis.MajorGrid.Color = Color.Gray;
            graphPane.YAxis.MinorGrid.IsVisible = true;
            graphPane.YAxis.MinorGrid.Color = Color.LightGray;
            graphPane.YAxis.MinorGrid.DashOff = 0;
            graphPane.Title.Text = string.Format("[{0}] {1}", _roleName, StringDef.LevelCurve);

            graphPane.AddCurve(StringDef.Level, playTimeArray, reachLevelArray, Color.Red, SymbolType.None);
        }
        catch (Exception ex)
        {
            LabelOpMsg.Visible = true;
            LabelOpMsg.Text = ex.Message;
            ZedGraphWebExpStatistic.Visible = false;
        }
	}

	void ExpByDate(System.Drawing.Graphics g, ZedGraph.MasterPane masterPane)
	{
        try
        {
            GameServer server = ServerDropDownList.SelectedGameServer;

            if (server == null)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
                return;
            }
            //加入权限检查
            if (!WebUtil.CheckPrivilege(server.SecurityObject, OpType.READ, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }
            if (!server.IsConnected)
            {
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                return;
            }

            List<DateTime> dateList = new List<DateTime>();
            List<int> expList = new List<int>();

            string baseCmdString = string.Format("SELECT DATE({0}) AS LogDate, SUM({1}) AS ExpSum FROM {2} WHERE {3}='{4}' AND {5}='{6}' GROUP BY LogDate",
                        FS2TableString.LogFieldLogTime,
                        FS2TableString.LogFieldLogKey6,
                        "{0}",
                        FS2TableString.LogFieldLogEvent,
                        LogEvent.GetExperience,
                        FS2TableString.LogFieldLogKey1,
                        _roleGuid);
            string addTableCmdString;
            WebUtil.AddTableNameToCmd(CurrentUser.Id, server, baseCmdString, _start, _end, out addTableCmdString);
            if (addTableCmdString.Length == 0)
            {
                return;
            }

            SqlCommand cmd = new SqlCommand(string.Format("SELECT LogDate,ExpSum FROM ({0}) AS A ORDER BY LogDate ASC", addTableCmdString));
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                result.SetFieldType(new SqlDataType[] { SqlDataType.DateTime, SqlDataType.Int32 });
                object[] record = null;
                while ((record = result.ReadRecord()) != null)
                {
                    dateList.Add((DateTime)record[0]);
                    expList.Add((Int32)record[1]);
                }
            }
            else
            {
                if (result == null)
                    LabelOpMsg.Text = StringDef.QueryTimeOut;
                else
                    LabelOpMsg.Text = StringDef.OperationFail;
            }

            TimeSpan actualTime = DateTime.Now.Subtract(_start > _createTime ? _start : _createTime);
            int days = actualTime.Days + 1;
            double[] dateArray = new double[days];
            double[] expArray = new double[days];
            for (int date = 0; date < days; date++)
            {
                DateTime dateTime = _createTime.AddDays(date);
                dateArray[date] = new XDate(dateTime.Year, dateTime.Month, dateTime.Day);

                double expAddToday = 0;
                for (int j = 0; j < dateList.Count; j++)
                {
                    DateTime date2 = dateList[j];
                    if (date2.Date == dateTime.Date)
                    {
                        expAddToday += expList[j];
                    }
                }

                expArray[date] = expAddToday;
                if (date > 0)
                    expArray[date] += expArray[date - 1];
            }

            GraphPane graphPane = masterPane[0];


            graphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);

            graphPane.Legend.Fill.IsVisible = false;
            graphPane.Legend.Border.IsVisible = false;
            graphPane.Legend.FontSpec.Fill.IsVisible = false;

            graphPane.XAxis.Title.Text = StringDef.Date;
            graphPane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;
            graphPane.XAxis.Type = AxisType.DateAsOrdinal;
            graphPane.XAxis.MinorTic.Size = 0;
            graphPane.XAxis.Scale.MajorStep = 1;
            graphPane.XAxis.Scale.FontSpec.Size = 12;
            graphPane.XAxis.Scale.MajorUnit = DateUnit.Day;
            graphPane.XAxis.Scale.Format = "MM-dd";

            graphPane.YAxis.MajorGrid.IsVisible = true;
            graphPane.YAxis.MajorGrid.DashOff = 0;
            graphPane.YAxis.MajorGrid.Color = Color.Gray;
            graphPane.YAxis.MinorGrid.IsVisible = true;
            graphPane.YAxis.MinorGrid.Color = Color.LightGray;
            graphPane.YAxis.MinorGrid.DashOff = 0;

            graphPane.Title.Text = string.Format("[{0}] {1}", _roleName,StringDef.ExperienceCurve);
            graphPane.YAxis.Title.Text = StringDef.Exp;

            graphPane.AddCurve(StringDef.Exp, dateArray, expArray, Color.Red, SymbolType.None);
        }
        catch (Exception ex)
        {
            LabelOpMsg.Visible = true;
            LabelOpMsg.Text = ex.Message;
            ZedGraphWebExpStatistic.Visible = false;
        }
	}

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }
}