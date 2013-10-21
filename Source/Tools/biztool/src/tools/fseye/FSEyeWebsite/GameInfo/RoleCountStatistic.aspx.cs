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
using System.Text;
using Resources;
using FSEye;
using ZedGraph;
using FSEye.Security;
using FSEye.FS2Lib;

public partial class GameInfo_RoleCountStatistic : BasePage
{
	private static string[] StatisticTypes = new string[] { "RoleLevel", "RoleType", "RoleSex", "EnterMapID" };

	GameServer _server;

	int _statisticType;

	ChartType _chartType;

	string _whereStatement;

	private class StatisticInfo
	{
		public int Type;

		public int Count;

		public StatisticInfo(int type, int count)
		{
			Type = type;
			Count = count;
		}
	}

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticRoleCountStatistic, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (!IsPostBack)
		{
			CreateRoleLevelList();
			CreateRoleClassList();
			CreateMapList();
            CreateSexList();
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
		}
	}

	private void CreateRoleLevelList()
	{		
		for (int i = 1; i <= FS2GameDataManager.MaxLevel; i++)
		{
			ListBoxLevelFrom.Items.Add(new ListItem(i.ToString(), i.ToString()));
			ListBoxLevelTo.Items.Add(new ListItem(i.ToString(), i.ToString()));
		}
		ListBoxLevelFrom.SelectedIndex = 0;
		ListBoxLevelTo.SelectedIndex = ListBoxLevelTo.Items.Count - 1;
	}

	private void CreateRoleClassList()
	{
		ListBoxRoleClass.Items.Add(new ListItem(StringDef.All, "-1"));

		FS2RoleType[] roleClasses = FS2GameDataManager.RoleClasses;
		for (int i = 0; i<roleClasses.Length; i++)
		{
			ListBoxRoleClass.Items.Add(new ListItem(roleClasses[i].ToString(), ((int)roleClasses[i]).ToString()));
		}
	}

	private void CreateMapList()
	{
		ListBoxMap.Items.Add(new ListItem(StringDef.All, "-1"));

		for (int i = 0; i < 5; i++)
		{
			ListBoxMap.Items.Add(new ListItem(i.ToString(), i.ToString()));
		}
	}

	private void CreateSexList()
	{
		ListBoxSex.Items.Add(new ListItem(StringDef.All, "-1"));

		FS2RoleSex [] sexes = FS2GameDataManager.RoleSexes;
		for (int i = 0; i < sexes.Length; i++)
		{
			ListBoxSex.Items.Add(new ListItem(sexes[i].ToString(), ((int)sexes[i]).ToString()));
		}
	}

	protected void LinkButtonStatistic_Click(object sender, EventArgs e)
	{
		try
		{
			_server = ServerDropDownList.SelectedGameServer;
			if (_server != null)
			{
                //加入权限检查
				if (!WebUtil.CheckPrivilege(_server.SecurityObject, OpType.READ, Session))
                {
                    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
                }

				_statisticType = ListBoxStatType.SelectedIndex;
				_chartType = (ChartType)int.Parse(ListBoxChartType.SelectedValue);
				int levelFrom = int.Parse(ListBoxLevelFrom.SelectedValue);
				int levelTo = int.Parse(ListBoxLevelTo.SelectedValue);
				int roleClass = int.Parse(ListBoxRoleClass.SelectedValue);
		    	int sex = int.Parse(ListBoxSex.SelectedValue);
				int map = int.Parse(ListBoxMap.SelectedValue);

				StringBuilder whereStatement = new StringBuilder();
				whereStatement.Append("WHERE RoleLevel>=");
				whereStatement.Append(levelFrom);
				whereStatement.Append(" AND RoleLevel<=");
				whereStatement.Append(levelTo);
				if (roleClass >= 0)
				{
					whereStatement.Append(" AND RoleType=");
					whereStatement.Append(roleClass);
				}
				if (sex >= 0)
				{
					whereStatement.Append(" AND RoleSex=");
					whereStatement.Append(sex);
				}
				if (map >= 0)
				{
					whereStatement.Append(" AND EnterMapID=");
					whereStatement.Append(map);
				}

				_whereStatement = whereStatement.ToString();

				ZedGraphWebStatistic.Visible = true;
			}
		}
		catch (Exception)
		{
		}
	}

	//private bool CreateStatisticTable(StatisticInfo[] infos, int statisticTypeIndex, object[] statistic)
	//{
	//    string[] StatisticTypeNames = new string[] { StringDef.Level, StringDef.RoleClass, StringDef.Sex, StringDef.Map };

	//    TableHeaderRow headerRow = new TableHeaderRow();
	//    TableHeaderCell headerCell = new TableHeaderCell();
	//    headerCell.Text = StatisticTypeNames[statisticTypeIndex];
	//    headerRow.Cells.Add(headerCell);

	//    headerCell = new TableHeaderCell();
	//    headerCell.Text = StringDef.Count;
	//    headerRow.Cells.Add(headerCell);

	//    TableStatistic.Rows.Add(headerRow);

	//    for (int i = 0; i < infos.Length; i++)
	//    {
	//        StatisticInfo info = infos[i];
	//        TableRow row = new TableRow();

	//        TableCell cell = new TableCell();
	//        for (int j = 0; j < statistic.Length; j++)
	//        {
	//            if (statistic[j].Id == info.Type)
	//            {
	//                cell.Text = statistic[j].Name;
	//                row.Cells.Add(cell);
	//                break;
	//            }
	//        }

	//        cell = new TableCell();
	//        cell.Text = info.Count.ToString();
	//        row.Cells.Add(cell);

	//        TableStatistic.Rows.Add(row);
	//    }

	//    return true;
	//}

	protected void ZedGraphWebStatistic_RenderGraph(ZedGraph.Web.ZedGraphWeb webObject, Graphics g, MasterPane masterPane)
	{
		if (_server != null)
		{
			GraphPane graphPane = masterPane[0];

			string[] StatisticTypeNames = new string[] { StringDef.Level, StringDef.RoleClass, StringDef.Sex, StringDef.Map };
			string staticType = StatisticTypes[_statisticType];
			string staticTypeName = StatisticTypeNames[_statisticType];
			SqlCommand cmd = new SqlCommand("SELECT " + staticType + ",COUNT(*) FROM rolesfirst " + _whereStatement + " GROUP BY " + staticType);
            if (!_server.IsConnected)
            {
                //LabelMessage.Visible = true;
                //LabelMessage.Text = StringDef.NoConnectionAlert;
                return;
            }
			SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id,_server, cmd);

			if (result != null && result.Success)
			{
				//整理数据
				SqlDataType[] dataTypes = new SqlDataType[] { SqlDataType.Int32, SqlDataType.Int32 };
				result.SetFieldType(dataTypes);

				object[] statistic = null;
				switch (_statisticType)
				{
					case 0:
						{
							statistic = new object[FS2GameDataManager.MaxLevel];
							for (int level = 1; level <= FS2GameDataManager.MaxLevel; level++)
							{
								statistic[level - 1] = level;
							}
						}
						break;
					case 1:
						{
							statistic = new object[FS2GameDataManager.RoleClasses.Length];
							for (int i = 0; i < FS2GameDataManager.RoleClasses.Length; i++)
							{
								statistic[i] = FS2GameDataManager.RoleClasses[i];
							}
						}
						break;
					case 2:
						{
							statistic = new object[FS2GameDataManager.RoleSexes.Length];
							for (int i = 0; i < FS2GameDataManager.RoleSexes.Length; i++)
							{
								statistic[i] = FS2GameDataManager.RoleSexes[i];
							}
						}
						break;
					case 3:
						statistic = new object[] { 0, 1, 2, 3, 4 };
						break;
				}
				
				StatisticInfo[] infos = new StatisticInfo[statistic.Length];
				for (int i = 0; i < infos.Length; i++)
				{
					infos[i] = new StatisticInfo(int.Parse(statistic[i].ToString()), 0);
				}

				object[] record = null;
				while ((record = result.ReadRecord()) != null)
				{
					int type = (int)record[0];
					int count = (int)record[1];

					for (int i = 0; i < infos.Length; i++)
					{
						if (infos[i].Type == type)
						{
							infos[i].Count = count;
						}
					}
				}

				////构造数据表格
				//bool success = CreateStatisticTable(infos, _statisticType, statistic);
				//if (success)
				//{
				//    TableStatistic.Visible = true;
				//}

				graphPane.Title.Text = staticTypeName;
				graphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);

				graphPane.Legend.IsVisible = false;

				string[] textLabels = new string[statistic.Length];
				for (int i = 0; i < statistic.Length; i++)
				{
					textLabels[i] = statistic[i].ToString();
				}

				graphPane.XAxis.Title.Text = staticTypeName;
				graphPane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;

				if (_statisticType > 0)
				{
					graphPane.XAxis.MajorTic.IsBetweenLabels = true;
					graphPane.XAxis.Type = AxisType.Text;
					graphPane.XAxis.Scale.TextLabels = textLabels;
				}

				graphPane.YAxis.Title.Text = StringDef.Count;
				graphPane.YAxis.Scale.Min = 0;
				graphPane.YAxis.MajorGrid.IsVisible = true;
				graphPane.YAxis.MajorGrid.DashOff = 0;
				graphPane.YAxis.MajorGrid.Color = Color.Gray;
				graphPane.YAxis.MinorGrid.IsVisible = true;
				graphPane.YAxis.MinorGrid.Color = Color.LightGray;
				graphPane.YAxis.MinorGrid.DashOff = 0;

				if (_chartType == ChartType.Bar)
				{
					graphPane.BarSettings.Type = BarType.Stack;

					double[] counts = new double[infos.Length];
					double[] types = new double[infos.Length];

					for (int i = 0; i < infos.Length; i++)
					{
						StatisticInfo info = infos[i] as StatisticInfo;
						counts[i] = info.Count;
						types[i] = info.Type;

						//添加数值标签
						string lab = info.Count.ToString();
						TextObj text = new TextObj(lab, i + 1, (float)(info.Count)); ;
						text.Location.CoordinateFrame = CoordType.AxisXYScale;
						text.FontSpec.Border.IsVisible = false;
						text.FontSpec.Fill.IsVisible = false;
						if (_statisticType == 0)
						{
							text.Location.AlignH = AlignH.Left;
							text.Location.AlignV = AlignV.Center;
							text.FontSpec.Angle = 90f;
						}
						else
						{
							text.Location.AlignH = AlignH.Center;
							text.Location.AlignV = AlignV.Bottom;
						}
						graphPane.GraphObjList.Add(text);
					}

					//绘制柱子
					BarItem barItem = graphPane.AddBar(StringDef.Count, types, counts, WebConfig.GraphColors[0]);
					barItem.Bar.Fill = new Fill(WebConfig.GraphColors[0]);
				}
				else if (_chartType == ChartType.Line)
				{
					double[] counts = new double[infos.Length];
					double[] types = new double[infos.Length];

					for (int i = 0; i < infos.Length; i++)
					{
						StatisticInfo info = infos[i] as StatisticInfo;
						counts[i] = info.Count;
						types[i] = info.Type;

						//添加数值标签
						string lab = info.Count.ToString();
						TextObj text = new TextObj(lab, i + 1, (float)(info.Count)); ;
						text.Location.CoordinateFrame = CoordType.AxisXYScale;
						text.FontSpec.Border.IsVisible = false;
						text.FontSpec.Fill.IsVisible = false;
						if (_statisticType == 0)
						{
							text.Location.AlignH = AlignH.Left;
							text.Location.AlignV = AlignV.Center;
							text.FontSpec.Angle = 90f;
						}
						else
						{
							text.Location.AlignH = AlignH.Center;
							text.Location.AlignV = AlignV.Bottom;
						}
						graphPane.GraphObjList.Add(text);
					}

					//绘制线条
					LineItem lineItem = graphPane.AddCurve("xxx", types, counts, WebConfig.GraphColors[1], SymbolType.None);
				}

				graphPane.AxisChange(g);
			}
		}
	}

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }

}
