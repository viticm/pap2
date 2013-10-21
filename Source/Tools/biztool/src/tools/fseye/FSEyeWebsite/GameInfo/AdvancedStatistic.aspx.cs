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
using System.Drawing;
using System.Collections.Generic;
using System.Text;
using ZedGraph;
using ZedGraph.Web;
using Resources;
using FSEye;
using FSEye.Security;
using FSEye.FS2Lib;

public partial class GameInfo_AdvancedStatistic : BasePage
{
	enum ViewContent
	{
		Exp,
		Money,
		Item,
		PlayerDeath,
		Quest,
		UseSkill
	}

	enum ViewType
	{
		ByDay,
		ByMonth,		
		ByHour
	}

	//物品相关
	static string[] ItemCountTypes = {
		"购买",
		"出售", 
		"使用",
		"拾取",
		"交易（失去）",
		"交易（得到）",
		"邮件（失去）",
		"邮件（得到）",
		"拍卖",
		"销毁",
		"系统（给予）",
		"系统（收回）"
	};
	static int[] ItemCountEventIds = {
		37,
		38,
		39,
		40,
		41,
		42,
		43,
		44,
		45,
		46,
		47,
		48
	};
	static Color[] ItemCountColors = {
		Color.FromArgb(255, 0, 0),
		Color.FromArgb(255, 0, 255),
		Color.FromArgb(153, 0, 255),		
		Color.FromArgb(51, 0, 255),
		Color.FromArgb(0, 255, 255),		
		Color.FromArgb(0, 255, 102),
		Color.FromArgb(0, 255, 0),
		Color.FromArgb(102, 255, 0),
		Color.FromArgb(153, 255, 0),
		Color.FromArgb(255, 255, 0),
		Color.FromArgb(255, 153, 0),
		Color.FromArgb(255, 102, 0)
	};
	
	//任务相关
	static string[] QuestCountTypes = {
		"接受",
		"放弃", 
		"完成"
	};
	static int[] QuestCountEventIds = {
		19,
		20,
		21
	};
	static Color[] QuestCountColors = {
		Color.Blue,
		Color.Red,
		Color.Green
	};

	const int EventIdAddExp = 13;
	const int EventIdAddMoney = 14;	
	const int EventIdRemoveMoney = 15;
	const int EventIdPlayerDeath = 17;
	const int EventIdUseSkill = 23;

	ViewContent _viewContent;

	ViewType _viewType;

	ChartType _chartType;

	bool _useCache = false;

    DateTime _startDate;

    DateTime _endDate;

    string _roleGuid;

    IList<AnalyseResult> _analyseResultList = new List<AnalyseResult>();

    string _statisticDesc;

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticAdvancedStatistic, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (!IsPostBack)
		{
			ClearAllCacheData();
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);

            StartDate.SelectedDate = DateTime.Today.AddMonths(-1);
            EndDate.SelectedDate = DateTime.Now;

			foreach (string itemCountType in ItemCountTypes)
			{
				ListItem item = new ListItem(itemCountType);
				item.Selected = true;
				CheckBoxListShowItemCount.Items.Add(item);
			}

			foreach (string questCountType in QuestCountTypes)
			{
				ListItem item = new ListItem(questCountType);
				item.Selected = true;
				CheckBoxListShowQuestCount.Items.Add(item);
			}
		}
				
		try
		{
			_viewContent = (ViewContent)int.Parse(ListBoxViewContent.SelectedValue);
			_viewType = (ViewType)int.Parse(ListBoxViewType.SelectedValue);
			_chartType = (ChartType)int.Parse(ListBoxChartType.SelectedValue);
		}
		catch (Exception)
		{
			_viewContent = ViewContent.Exp;
			_viewType = ViewType.ByMonth;
			_chartType = ChartType.Bar;
		}

        LabelOpMsg.Text = string.Empty;
		ZedGraphWebExpStatistic.Visible = false;
	}

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }

	protected void ListBoxViewType_OnSelectedIndexChanged(object sender, EventArgs e)
	{
        //DateTime now = DateTime.Now;
        //switch (_viewType)
        //{
        //    case ViewType.ByDay:
        //        {
        //            StartDate.SelectedDate = new DateTime(now.Year, now.Month, 1);
        //            EndDate.SelectedDate =now;
        //        }
        //        break;
        //    case ViewType.ByMonth:
        //        {
        //            StartDate.SelectedDate = new DateTime(now.Year, 1, 1);
        //            EndDate.SelectedDate = now;
        //        }
        //        break;
        //}
	}

	protected void ListBoxViewContent_SelectedIndexChanged(object sender, EventArgs e)
	{
        PanelViewExpOptions.Visible = false;
		PanelViewMoneyOptions.Visible = false;
		PanelViewItemOptions.Visible = false;
		PanelViewPlayerDeathOptions.Visible = false;
		PanelViewQuestOptions.Visible = false;
		PanelViewSkillOptions.Visible = false;

		switch (_viewContent)
		{
			case ViewContent.Exp:
                PanelViewExpOptions.Visible = true;
				break;
			case ViewContent.Money:
				PanelViewMoneyOptions.Visible = true;
				break;
			case ViewContent.Item:
				PanelViewItemOptions.Visible = true;
				break;
			case ViewContent.PlayerDeath:
				PanelViewPlayerDeathOptions.Visible = true;
				break;
			case ViewContent.Quest:
				PanelViewQuestOptions.Visible = true;
				break;
			case ViewContent.UseSkill:
				PanelViewSkillOptions.Visible = true;
				break;
		}

		Session["AdvancedStatistic_CanUseCache"] = false;
	}

	protected void ListBoxChartType_OnSelectedIndexChanged(object sender, EventArgs e)
	{
		if (Session["AdvancedStatistic_CanUseCache"] != null && (bool)Session["AdvancedStatistic_CanUseCache"])
		{
			_useCache = true;
			ZedGraphWebExpStatistic.Visible = true;
		}
	}

	protected void LinkButtonStatistic_Click(object sender, EventArgs e)
	{
		ClearAllCacheData();

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
            _startDate = StartDate.SelectedDate;
            if (_startDate == DateTime.MinValue)
            {
                LabelOpMsg.Text = StringDef.ParameterInputError + StringDef.Colon + StringDef.StartDate;
                return;
            }
            _endDate = EndDate.SelectedDate;
            if (_endDate == DateTime.MinValue)
            {
                LabelOpMsg.Text = StringDef.ParameterInputError + StringDef.Colon + StringDef.StartDate;
                return;
            }
            if (_startDate > _endDate)
            {
                LabelOpMsg.Text = StringDef.EndTimeError;
                return;
            }

            string roleName = TextBoxRoleName.Text.Trim();
            if (roleName.Length != 0)
            {
                _roleGuid = WebUtil.FindRoleGuidByName(CurrentUser.Id, server, roleName);
                if (_roleGuid == null || _roleGuid.Length == 0)
                {
                    LabelOpMsg.Text = string.Format(StringDef.RoleWithNameNotExist, roleName);
                    return;
                }
                _statisticDesc = String.Format(StringDef.RoleWithName, roleName);
            }
            else
            {
                _statisticDesc = StringDef.AllRoles;
            }

            switch (this._viewContent)
            {
                case ViewContent.Exp:
                    AnalyseResult expResult = AnalyseData(server, 13, string.Empty);
                    if (expResult != null) _analyseResultList.Add(expResult);
                    break;
                case ViewContent.Item:
                    string itemTemplateIdFilter = null;
                    if (TextBoxItemTemplateId.Text.Trim().Length > 0)
                    {
                        itemTemplateIdFilter = string.Format("LogKey2='{0}'", TextBoxItemTemplateId.Text.Trim());
                    }

                    for (int i = 0; i < ItemCountTypes.Length; i++)
                    {
                        ListItem item = CheckBoxListShowItemCount.Items[i];
                        if (item.Selected)
                        {
                            AnalyseResult itemResult = AnalyseData(server, ItemCountEventIds[i], itemTemplateIdFilter);
                            if (itemResult != null) _analyseResultList.Add(itemResult);
                        }
                    }
                    break;
                case ViewContent.Money:
                    AnalyseResult addMoneyResult = null;
                    AnalyseResult removeMoneyResult = null;
                    if (CheckBoxShowAddMoney.Checked || CheckBoxShowChangeMoney.Checked)
                    {
                        addMoneyResult = AnalyseData(server, EventIdAddMoney, null);
                    }
                    if (CheckBoxShowRemoveMoney.Checked || CheckBoxShowChangeMoney.Checked)
                    {
                        removeMoneyResult = AnalyseData(server, EventIdRemoveMoney, null);
                    }
                    if (addMoneyResult != null) _analyseResultList.Add(addMoneyResult);
                    if (removeMoneyResult != null) _analyseResultList.Add(removeMoneyResult);
                    break;
                case ViewContent.PlayerDeath:
                    AnalyseResult playerKillDeathResult = AnalyseData(server, EventIdPlayerDeath, "LENGTH(LogKey2)>=32");
		            AnalyseResult npcKillDeathResult = AnalyseData(server, EventIdPlayerDeath, "LENGTH(LogKey2)<32");
                    if (playerKillDeathResult != null)
                    {
                        _analyseResultList.Add(playerKillDeathResult);
                        Session["AdvancedStatistic_PlayerDeathStatistic_PlayerKill"] = playerKillDeathResult;
                        Session["AdvancedStatistic_CanUseCache"] = true;
                    }
                    if (npcKillDeathResult != null)
                    {
                        _analyseResultList.Add(npcKillDeathResult);
                        Session["AdvancedStatistic_PlayerDeathStatistic_NpcKill"] = npcKillDeathResult;
                        Session["AdvancedStatistic_CanUseCache"] = true;
                    }
                    break;
                case ViewContent.Quest:
                    string questFilter = null;

                    if (TextBoxQuestId.Text.Trim().Length > 0)
                    {
                        try
                        {
                            int questId = int.Parse(TextBoxQuestId.Text.Trim());
                            if (questId < 0) throw new Exception();

                            questFilter = string.Format("LogKey2='{0}'", questId);
                        }
                        catch (Exception)
                        {
                            LabelOpMsg.Text = StringDef.ParameterInputError;
                            return;
                        }
                    }
                    for (int i = 0; i < QuestCountTypes.Length; i++)
                    {
                        ListItem item = CheckBoxListShowQuestCount.Items[i];
                        if (item.Selected)
                        {
                            AnalyseResult questResult = AnalyseData(server, QuestCountEventIds[i], questFilter);
                            if (questResult != null)
                            {
                                _analyseResultList.Add(questResult);
                                Session[string.Format("AdvancedStatistic_QuestStatistic_{0}", i)] = questResult;
                                Session["AdvancedStatistic_CanUseCache"] = true;
                            }
                        }
                    }
                    break;
                case ViewContent.UseSkill:
                    string skillFilter = null;
                    try
                    {
                        FS2SkillData selectedSkill = SkillDropDownList1.SelectedSkill;
                        if (selectedSkill != null)
                        {
                            skillFilter = string.Format("LogKey2='{0}'", selectedSkill.TemplateId);
                        }
                    }
                    catch (Exception)
                    {
                        LabelOpMsg.Text = StringDef.ParameterInputError;
                        return;
                    }
                    AnalyseResult useSkillResult = AnalyseData(server, EventIdUseSkill, skillFilter);
                    if (useSkillResult != null) _analyseResultList.Add(useSkillResult);
                    break;
            }
            if (_analyseResultList != null && _analyseResultList.Count !=0)
            {
                ZedGraphWebExpStatistic.Visible = true;
            }
            else
            {
                LabelOpMsg.Text = StringDef.OperationFail;
            }
		}
		catch(Exception ex)
		{
			LabelOpMsg.Text = ex.Message;
		}
	}

    AnalyseResult AnalyseData(GameServer server, string roleGuid, DateTime startTime, DateTime endTime, ViewType viewType, int eventIdAddMoney, string addition)
    {
        return null;
    }

	AnalyseResult AnalyseData(GameServer server, int logEventId,string additionalFilter)
	{
        double[] statisticValues = null;
		double[] statisticTimes = null;
		double[] statisticCounts = null;

        bool bQuitFlag = false;

        StringBuilder whereStatement = new StringBuilder();
        whereStatement.AppendFormat(" WHERE {0}={1} ", FS2TableString.LogFieldLogEvent, logEventId);
        
        if (_roleGuid != null)
		{
            whereStatement.AppendFormat(" AND {0}='{1}'", FS2TableString.LogFieldLogKey1, _roleGuid);
		}

        whereStatement.AppendFormat(" AND {0} >= '{1}' AND {0} <= '{2}'",
            FS2TableString.LogFieldLogTime, 
            _startDate,
            _endDate);

        if (additionalFilter != null && additionalFilter.Length > 0)
            whereStatement.AppendFormat(" AND {0} ", additionalFilter);

        string baseCmdString = string.Empty;
        switch (_viewType)
        {
            case ViewType.ByMonth:
                baseCmdString = string.Format("SELECT DATE_FORMAT(LogTime,'%Y-%m'),SUM(LogKey6),COUNT(*) FROM {0} {1} GROUP BY LogEvent",
                            "{0}",
                            whereStatement.ToString());

                int monthCount = 12 * (_endDate.Year - _startDate.Year) + (_endDate.Month - _startDate.Month) + 1;
                statisticValues = new double[monthCount];
                statisticTimes = new double[monthCount];
                statisticCounts = new double[monthCount];

                for (int index = 0; index != statisticValues.Length; ++index)
                {
                    DateTime tempDate = _startDate.AddMonths(index);
                    statisticTimes[index] = new XDate(tempDate.Year, tempDate.Month, 1);
                }

                break;
            case ViewType.ByDay:
                baseCmdString = string.Format("SELECT DATE(LogTime), SUM(LogKey6), COUNT(*) FROM {0} {1} GROUP BY DATE(LogTime)",
                            "{0}",
                            whereStatement.ToString());

                TimeSpan span = _endDate.Subtract(_startDate);                
                int dayCount = span.Days + 1;
                statisticValues = new double[dayCount];
                statisticTimes = new double[dayCount];
                statisticCounts = new double[dayCount];
                for (int index = 0; index != statisticTimes.Length; ++index)
                {
                    DateTime tempDate = _startDate.Date.AddDays(index);
                    statisticTimes[index] = new XDate(tempDate);
                }
                break;
            case ViewType.ByHour:
                bQuitFlag = true;                
                break;
            default:
                bQuitFlag = true;                
                break;
        }

        if (bQuitFlag) return null;

        string addTableCmdString;
        WebUtil.AddTableNameToCmd(CurrentUser.Id, server, baseCmdString, _startDate, _endDate, out addTableCmdString);
        if (addTableCmdString.Length == 0)
        {
            return null;
        }        

        SqlResult sqlResult = WebUtil.QueryGameServerDb(CurrentUser.Id, server, new SqlCommand(addTableCmdString));
        if (sqlResult != null && sqlResult.Success)
        {
            sqlResult.SetFieldType(new SqlDataType[] { 
                        SqlDataType.DateTime,
                        SqlDataType.Int64,
                        SqlDataType.Int32
                    });

            object[] record = sqlResult.ReadRecord();

            switch (_viewType)
            {
                case ViewType.ByMonth:
                    while (record != null)
                    {
                        DateTime date = (DateTime)record[0];
                        long sum = (long)record[1];
                        int count = (int)record[2];

                        int monthIndex = 12 * (date.Year - _startDate.Year) + (date.Month - _startDate.Month);
                        statisticTimes[monthIndex] = new XDate(date);
                        statisticValues[monthIndex] = sum;
                        statisticCounts[monthIndex] = count;

                        record = sqlResult.ReadRecord();
                    }
                    break;
                case ViewType.ByDay:                    
                    while (record != null)
                    {
                        DateTime date = (DateTime)record[0];
                        long sum = (long)record[1];
                        int count = (int)record[2];

                        TimeSpan indexSpan = date.Subtract(_startDate.Date);
                        statisticTimes[indexSpan.Days] = new XDate(date);
                        statisticValues[indexSpan.Days] = sum;
                        statisticCounts[indexSpan.Days] = count;

                        record = sqlResult.ReadRecord();
                    }
                    break;
            }
        }
        else
        {
            return null;
        }

		AnalyseResult result = new AnalyseResult();
		result.Values = statisticValues;
		result.Times = statisticTimes;
		result.Counts = statisticCounts;

		return result;
	}

	protected void ZedGraphWebExpStatistic_RenderGraph(ZedGraph.Web.ZedGraphWeb webObject, Graphics g, MasterPane masterPane)
	{
        try
        {
            switch (_viewContent)
            {
                case ViewContent.Exp:
                    ExpStatistic(g, masterPane);
                    break;
                case ViewContent.Money:
                    MoneyStatistic(g, masterPane);
                    break;
                case ViewContent.Item:
                    ItemStatistic(g, masterPane);
                    break;
                case ViewContent.PlayerDeath:
                    PlayerDeathStatistic(g, masterPane);
                    break;
                case ViewContent.Quest:
                    QuestStatistic(g, masterPane);
                    break;
                case ViewContent.UseSkill:
                    UseSkillStatistic(g, masterPane);
                    break;
            }
        }
        catch (Exception e)
        {
            LabelOpMsg.Text = e.Message;
            LabelOpMsg.Visible = true;
        }
	}

	protected void ButtonRefreshItemCount_Click(object sender, EventArgs e)
	{
		if (Session["AdvancedStatistic_CanUseCache"] != null && (bool)Session["AdvancedStatistic_CanUseCache"])
		{
			_useCache = true;
			ZedGraphWebExpStatistic.Visible = true;
		}
	}

	protected void ButtonRefreshQuestCount_Click(object sender, EventArgs e)
	{
		if (Session["AdvancedStatistic_CanUseCache"] != null && (bool)Session["AdvancedStatistic_CanUseCache"])
		{
			_useCache = true;
			ZedGraphWebExpStatistic.Visible = true;
		}
	}

	protected void SelectAllItemCount_Click(object sender, EventArgs e)
	{
		foreach (ListItem item in CheckBoxListShowItemCount.Items)
		{
			item.Selected = true;
		}
	}

	protected void SelectNoneItemCount_Click(object sender, EventArgs e)
	{
		foreach (ListItem item in CheckBoxListShowItemCount.Items)
		{
			item.Selected = false;
		}
	}

	void ExpStatistic(System.Drawing.Graphics g, ZedGraph.MasterPane masterPane)
	{
        bool showSymbol = true;
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
		switch (_viewType)
		{
			case ViewType.ByMonth:
				graphPane.XAxis.Scale.MajorUnit = DateUnit.Month;
				break;
			case ViewType.ByDay:
				graphPane.XAxis.Scale.MajorUnit = DateUnit.Day;
				break;
		}

		graphPane.YAxis.MajorGrid.IsVisible = true;
		graphPane.YAxis.MajorGrid.DashOff = 0;
		graphPane.YAxis.MajorGrid.Color = Color.Gray;
		graphPane.YAxis.MinorGrid.IsVisible = true;
		graphPane.YAxis.MinorGrid.Color = Color.LightGray;
		graphPane.YAxis.MinorGrid.DashOff = 0;

		graphPane.Title.Text = String.Format("{0} - {1}", StringDef.ExpStatistic, _statisticDesc);
		graphPane.YAxis.Title.Text = StringDef.Exp;

		if (_analyseResultList != null && _analyseResultList.Count!=0)
		{
            double[] timeArray = _analyseResultList[0].Times;
            double[] expArray = _analyseResultList[0].Values;

			if (_chartType == ChartType.Bar)
			{
				BarItem barItemMoney = graphPane.AddBar(StringDef.AddExp, timeArray, expArray, Color.Black);
				barItemMoney.Bar.Fill = new Fill(Color.Blue);
			}
			else if (_chartType == ChartType.Line)
			{
				graphPane.AddCurve(StringDef.AddExp, timeArray, expArray, Color.Red, (showSymbol ? WebConfig.GraphSymbols[0] : SymbolType.None));
			}
		}		
	}

	void MoneyStatistic(System.Drawing.Graphics g, ZedGraph.MasterPane masterPane)
	{
		bool showSymbol = true;
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
		switch (_viewType)
		{
			case ViewType.ByMonth:
				graphPane.XAxis.Scale.MajorUnit = DateUnit.Month;
				break;
			case ViewType.ByDay:
				graphPane.XAxis.Scale.MajorUnit = DateUnit.Day;
				break;
		}

		graphPane.YAxis.MajorGrid.IsVisible = true;
		graphPane.YAxis.MajorGrid.DashOff = 0;
		graphPane.YAxis.MajorGrid.Color = Color.Gray;
		graphPane.YAxis.MinorGrid.IsVisible = true;
		graphPane.YAxis.MinorGrid.Color = Color.LightGray;
		graphPane.YAxis.MinorGrid.DashOff = 0;

		graphPane.Title.Text = String.Format("{0} - {1}", StringDef.MoneyStatistic, _statisticDesc);
		graphPane.YAxis.Title.Text = StringDef.Money;

        AnalyseResult addMoneyResult = null;
        AnalyseResult removeMoneyResult = null;
        if (_analyseResultList != null && _analyseResultList.Count != 0)
        {
            int index = 0;
            if (CheckBoxShowAddMoney.Checked || CheckBoxShowChangeMoney.Checked)
            {
                addMoneyResult = _analyseResultList[index];
                ++index;
            }
            if (CheckBoxShowRemoveMoney.Checked || CheckBoxShowChangeMoney.Checked)
            {
                removeMoneyResult = _analyseResultList[index];
            }
        }

		if (CheckBoxShowChangeMoney.Checked)
		{
			double[] changeMoneyValues = new double[addMoneyResult.Values.Length];
			double[] changeMoneyTimes = new double[addMoneyResult.Times.Length];
			if (addMoneyResult.Values.Length == removeMoneyResult.Values.Length)
			{
				for (int i = 0; i < changeMoneyValues.Length; i++)
				{
					changeMoneyValues[i] = addMoneyResult.Values[i] + removeMoneyResult.Values[i];
					changeMoneyTimes[i] = addMoneyResult.Times[i];
				}
			}

			if (_chartType == ChartType.Bar)
			{
				graphPane.BarSettings.Type = BarType.SortedOverlay;

				if (CheckBoxShowAddMoney.Checked)
				{
					BarItem barItemAddMoney = graphPane.AddBar(StringDef.AddMoney, addMoneyResult.Times, addMoneyResult.Values, Color.Black);
					barItemAddMoney.Bar.Fill = new Fill(Color.Green);
				}
				if (CheckBoxShowRemoveMoney.Checked)
				{
					BarItem barItemRemoveMoney = graphPane.AddBar(StringDef.RemoveMoney, removeMoneyResult.Times, removeMoneyResult.Values, Color.Black);
					barItemRemoveMoney.Bar.Fill = new Fill(Color.Red);
				}

				BarItem barItemChangeMoney = graphPane.AddBar(StringDef.ChangeMoney, changeMoneyTimes, changeMoneyValues, Color.Black);
				barItemChangeMoney.Bar.Fill = new Fill(Color.Blue);
			}
			else if (_chartType == ChartType.Line)
			{
				if (CheckBoxShowAddMoney.Checked)
				{
					graphPane.AddCurve(StringDef.AddMoney, addMoneyResult.Times, addMoneyResult.Values, Color.Green, (showSymbol ? WebConfig.GraphSymbols[0] : SymbolType.None));
				}
				if (CheckBoxShowRemoveMoney.Checked)
				{
					graphPane.AddCurve(StringDef.RemoveMoney, removeMoneyResult.Times, removeMoneyResult.Values, Color.Red, (showSymbol ? WebConfig.GraphSymbols[1] : SymbolType.None));
				}

				graphPane.AddCurve(StringDef.ChangeMoney, changeMoneyTimes, changeMoneyValues, Color.Blue, (showSymbol ? WebConfig.GraphSymbols[2] : SymbolType.None));
			}
		}
		else
		{
			if (_chartType == ChartType.Bar)
			{
				graphPane.BarSettings.Type = BarType.SortedOverlay;

				if (CheckBoxShowAddMoney.Checked)
				{
					BarItem barItemAddMoney = graphPane.AddBar(StringDef.AddMoney, addMoneyResult.Times, addMoneyResult.Values, Color.Black);
					barItemAddMoney.Bar.Fill = new Fill(Color.Green);
				}
				if (CheckBoxShowRemoveMoney.Checked)
				{
					BarItem barItemRemoveMoney = graphPane.AddBar(StringDef.RemoveMoney, removeMoneyResult.Times, removeMoneyResult.Values, Color.Black);
					barItemRemoveMoney.Bar.Fill = new Fill(Color.Red);
				}
			}
			else if (_chartType == ChartType.Line)
			{
				if (CheckBoxShowAddMoney.Checked)
				{
					graphPane.AddCurve(StringDef.AddMoney, addMoneyResult.Times, addMoneyResult.Values, Color.Green, (showSymbol ? WebConfig.GraphSymbols[0] : SymbolType.None));
				}
				if (CheckBoxShowRemoveMoney.Checked)
				{
					graphPane.AddCurve(StringDef.RemoveMoney, removeMoneyResult.Times, removeMoneyResult.Values, Color.Red, (showSymbol ? WebConfig.GraphSymbols[1] : SymbolType.None));
				}
			}
		}		
	}

	void ClearItemStatisticData()
	{
		for (int i = 0; i < ItemCountTypes.Length; i++)
		{
			Session.Remove(string.Format("AdvancedStatistic_ItemStatistic_{0}", i));
		}
	}

	void ItemStatistic(System.Drawing.Graphics g, ZedGraph.MasterPane masterPane)
	{   
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
		switch (_viewType)
		{
			case ViewType.ByMonth:
				graphPane.XAxis.Scale.MajorUnit = DateUnit.Month;
				break;
			case ViewType.ByDay:
				graphPane.XAxis.Scale.MajorUnit = DateUnit.Day;
				break;
		}

		graphPane.YAxis.MajorGrid.IsVisible = true;
		graphPane.YAxis.MajorGrid.DashOff = 0;
		graphPane.YAxis.MajorGrid.Color = Color.Gray;
		graphPane.YAxis.MinorGrid.IsVisible = true;
		graphPane.YAxis.MinorGrid.Color = Color.LightGray;
		graphPane.YAxis.MinorGrid.DashOff = 0;

		graphPane.Title.Text = String.Format("{0} - {1}", "物品数量统计", _statisticDesc);
		graphPane.YAxis.Title.Text = "数量";

		string itemTemplateIdFilter = null;
		if (TextBoxItemTemplateId.Text.Length > 0)
		{
			itemTemplateIdFilter = string.Format("LogKey2='{0}'", TextBoxItemTemplateId.Text);
		}

        int index = 0;

		for (int i = 0; i < ItemCountTypes.Length; i++)
		{
			ListItem item = CheckBoxListShowItemCount.Items[i];
			if (item.Selected)
			{
				AnalyseResult result = null;
				if (_useCache)
				{
					result = Session[string.Format("AdvancedStatistic_ItemStatistic_{0}", i)] as AnalyseResult;
				}
				if (result == null)
				{
					result = _analyseResultList[index];
                    ++index;
					Session[string.Format("AdvancedStatistic_ItemStatistic_{0}", i)] = result;
					Session["AdvancedStatistic_CanUseCache"] = true;
				}
				if (_chartType == ChartType.Bar)
				{
					BarItem barItem = graphPane.AddBar(ItemCountTypes[i], result.Times, result.Values, ItemCountColors[i]);
					barItem.Bar.Fill = new Fill(ItemCountColors[i]);
				}
				else if (_chartType == ChartType.Line)
				{
					LineItem lineItem = graphPane.AddCurve(ItemCountTypes[i], result.Times, result.Values, ItemCountColors[i]);
				}
			}
		}

		if (_chartType == ChartType.Bar)
		{
			graphPane.BarSettings.Type = BarType.SortedOverlay;
		}
		else if (_chartType == ChartType.Line)
		{
		}
	}

	void ClearPlayerDeathStatisticData()
	{
		Session.Remove("AdvancedStatistic_PlayerDeathStatistic_PlayerKill");
		Session.Remove("AdvancedStatistic_PlayerDeathStatistic_NpcKill");
	}

	void PlayerDeathStatistic(System.Drawing.Graphics g, ZedGraph.MasterPane masterPane)
	{
		bool showSymbol = true;
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
		switch (_viewType)
		{
			case ViewType.ByMonth:
				graphPane.XAxis.Scale.MajorUnit = DateUnit.Month;
				break;
			case ViewType.ByDay:
				graphPane.XAxis.Scale.MajorUnit = DateUnit.Day;
				break;
		}

		graphPane.YAxis.MajorGrid.IsVisible = true;
		graphPane.YAxis.MajorGrid.DashOff = 0;
		graphPane.YAxis.MajorGrid.Color = Color.Gray;
		graphPane.YAxis.MinorGrid.IsVisible = true;
		graphPane.YAxis.MinorGrid.Color = Color.LightGray;
		graphPane.YAxis.MinorGrid.DashOff = 0;

		graphPane.Title.Text = String.Format("{0} - {1}", StringDef.PlayerDeath, _statisticDesc);
		graphPane.YAxis.Title.Text = StringDef.DeathCount;

		AnalyseResult playerKillDeathResult = null;
		AnalyseResult npcKillDeathResult = null;

        if (_analyseResultList != null && _analyseResultList.Count != 0)
        {
            int index = 0;
            if (CheckBoxShowPlayerKill.Checked)
            {
                if (_useCache)
                {
                    playerKillDeathResult = Session["AdvancedStatistic_PlayerDeathStatistic_PlayerKill"] as AnalyseResult;
                }
                if (playerKillDeathResult == null)
                {
                    playerKillDeathResult = _analyseResultList[index];
                    ++index;
                    Session["AdvancedStatistic_PlayerDeathStatistic_PlayerKill"] = playerKillDeathResult;
                    Session["AdvancedStatistic_CanUseCache"] = true;
                }
            }
            if (CheckBoxShowNpcKill.Checked)
            {
                if (_useCache)
                {
                    npcKillDeathResult = Session["AdvancedStatistic_PlayerDeathStatistic_NpcKill"] as AnalyseResult;
                }
                if (npcKillDeathResult == null)
                {
                    npcKillDeathResult = _analyseResultList[index];
                    Session["AdvancedStatistic_PlayerDeathStatistic_NpcKill"] = npcKillDeathResult;
                    Session["AdvancedStatistic_CanUseCache"] = true;
                }
            }
        }

		if (_chartType == ChartType.Bar)
		{
			graphPane.BarSettings.Type = BarType.Stack;

			if (playerKillDeathResult != null)
			{
				BarItem barPlayerKillDeathCount = graphPane.AddBar(StringDef.PlayerDeathByPlayer, playerKillDeathResult.Times, playerKillDeathResult.Counts, Color.Black);
				barPlayerKillDeathCount.Bar.Fill = new Fill(Color.Red);
			}

			if (npcKillDeathResult != null)
			{
				BarItem barNpcKillDeathCount = graphPane.AddBar(StringDef.PlayerDeathByNpc, npcKillDeathResult.Times, npcKillDeathResult.Counts, Color.Black);
				barNpcKillDeathCount.Bar.Fill = new Fill(Color.Blue);
			}
		}
		else if (_chartType == ChartType.Line)
		{
			if (playerKillDeathResult != null)
			{
				graphPane.AddCurve(StringDef.PlayerDeathByPlayer, playerKillDeathResult.Times, playerKillDeathResult.Counts, Color.Red, (showSymbol ? WebConfig.GraphSymbols[0] : SymbolType.None));
			}

			if (npcKillDeathResult != null)
			{
				graphPane.AddCurve(StringDef.PlayerDeathByNpc, npcKillDeathResult.Times, npcKillDeathResult.Counts, Color.Blue, (showSymbol ? WebConfig.GraphSymbols[0] : SymbolType.None));
			}
		}		
	}

	void ClearQuestStatisticData()
	{
		for (int i = 0; i < QuestCountTypes.Length; i++)
		{
			Session.Remove(string.Format("AdvancedStatistic_QuestStatistic_{0}", i));
		}
	}

	void QuestStatistic(System.Drawing.Graphics g, ZedGraph.MasterPane masterPane)
	{
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
		switch (_viewType)
		{
			case ViewType.ByMonth:
				graphPane.XAxis.Scale.MajorUnit = DateUnit.Month;
				break;
			case ViewType.ByDay:
				graphPane.XAxis.Scale.MajorUnit = DateUnit.Day;
				break;
		}

		graphPane.YAxis.MajorGrid.IsVisible = true;
		graphPane.YAxis.MajorGrid.DashOff = 0;
		graphPane.YAxis.MajorGrid.Color = Color.Gray;
		graphPane.YAxis.MinorGrid.IsVisible = true;
		graphPane.YAxis.MinorGrid.Color = Color.LightGray;
		graphPane.YAxis.MinorGrid.DashOff = 0;

		graphPane.Title.Text = String.Format("{0} - {1}", StringDef.QuestStatistic, _statisticDesc);
		graphPane.YAxis.Title.Text = StringDef.QuestStatistic;

        int index = 0;
		for (int i = 0; i < QuestCountTypes.Length; i++)
		{
			ListItem item = CheckBoxListShowQuestCount.Items[i];
			if (item.Selected)
			{
				AnalyseResult result = null;
				if (_useCache)
				{
					result = Session[string.Format("AdvancedStatistic_QuestStatistic_{0}", i)] as AnalyseResult;
				}
				if (result == null)
				{
                    result = _analyseResultList[index];
                    ++index;
				}
				if (_chartType == ChartType.Bar)
				{
					BarItem barItem = graphPane.AddBar(QuestCountTypes[i], result.Times, result.Counts, QuestCountColors[i]);
					barItem.Bar.Fill = new Fill(QuestCountColors[i]);
				}
				else if (_chartType == ChartType.Line)
				{
					LineItem lineItem = graphPane.AddCurve(QuestCountTypes[i], result.Times, result.Counts, QuestCountColors[i]);
				}
			}
		}

		if (_chartType == ChartType.Bar)
		{
			graphPane.BarSettings.Type = BarType.SortedOverlay;
		}
		else if (_chartType == ChartType.Line)
		{
		}
	}

	void UseSkillStatistic(System.Drawing.Graphics g, ZedGraph.MasterPane masterPane)
	{
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
		switch (_viewType)
		{
			case ViewType.ByMonth:
				graphPane.XAxis.Scale.MajorUnit = DateUnit.Month;
				break;
			case ViewType.ByDay:
				graphPane.XAxis.Scale.MajorUnit = DateUnit.Day;
				break;
		}

		graphPane.YAxis.MajorGrid.IsVisible = true;
		graphPane.YAxis.MajorGrid.DashOff = 0;
		graphPane.YAxis.MajorGrid.Color = Color.Gray;
		graphPane.YAxis.MinorGrid.IsVisible = true;
		graphPane.YAxis.MinorGrid.Color = Color.LightGray;
		graphPane.YAxis.MinorGrid.DashOff = 0;

		graphPane.Title.Text = String.Format("{0} - {1}", StringDef.UseSkill, _statisticDesc);
		graphPane.YAxis.Title.Text = StringDef.UseSkill;

		AnalyseResult result = null;
		if (_useCache)
		{
			result = Session["AdvancedStatistic_UseSkillStatistic"] as AnalyseResult;
		}
		if (result == null)
		{
            result = _analyseResultList[0];
			Session["AdvancedStatistic_UseSkillStatistic"] = result;
			Session["AdvancedStatistic_CanUseCache"] = true;
		}
		if (_chartType == ChartType.Bar)
		{
			BarItem barItem = graphPane.AddBar("使用技能", result.Times, result.Counts, Color.Blue);
			barItem.Bar.Fill = new Fill(Color.Blue);
		}
		else if (_chartType == ChartType.Line)
		{
			LineItem lineItem = graphPane.AddCurve("使用技能", result.Times, result.Counts, Color.Blue);
		}
	}

	void ClearAllCacheData()
	{
		Session.Remove("AdvancedStatistic_CanUseCache");
		ClearPlayerDeathStatisticData();
		ClearItemStatisticData();
		ClearQuestStatisticData();
	}
}

public class AnalyseResult
{
    public double[] Values;
    public double[] Times;
    public double[] Counts;
}
