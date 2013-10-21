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
using Resources;
using FSEye;
using ZedGraph;
using FSEye.DAL;
using FSEye.Security;
using FSEye.FS2Lib;

public partial class GameInfo_EasyStatistic : BasePage
{
	private const int RoleSexCount = 2;

	private static bool showSymbol = true;

	private static Color[] Colors = new Color[] { Color.Blue, Color.Purple, Color.Red, Color.Green, Color.HotPink };

	GameServer _server;

	int _statisticType;

	ChartType _chartType;

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticEasyStatistic, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}
        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
        }
	}

	protected void ZedGraphWebStatistic_RenderGraph(ZedGraph.Web.ZedGraphWeb webObject, Graphics g, MasterPane masterPane)
	{
		if (_server != null)
		{
			GraphPane graphPane = masterPane[0];

			if (_statisticType == 1)
			{
				FS2RoleType[] roleClasses = FS2GameDataManager.RoleClasses;
				SqlCommand[] cmds = new SqlCommand[roleClasses.Length];
				SqlResult[] results = new SqlResult[roleClasses.Length];
				for (int i = 0; i < roleClasses.Length; i++)
				{
					cmds[i] = new SqlCommand("SELECT RoleLevel,COUNT(*) FROM rolesfirst WHERE RoleType=? GROUP BY RoleLevel", (int)roleClasses[i]);
                    if (!_server.IsConnected)
                    {
                        //LabelMessage.Visible = true;
                        //LabelMessage.Text = StringDef.NoConnectionAlert;
                        return;
                    }
                    results[i] = WebUtil.QueryGameServerDb(CurrentUser.Id, _server, cmds[i]);
				}

				bool success = true;
				for (int i = 0; i < roleClasses.Length; i++)
				{
					if (results[i] == null || !results[i].Success)
					{
						success = false;
						break;
					}
				}

				if (success)
				{
					//整理数据			
					SqlDataType[] dataTypes = new SqlDataType[] { SqlDataType.Int16, SqlDataType.UInt32 };
					for (int i = 0; i < roleClasses.Length; i++)
					{
						results[i].SetFieldType(dataTypes);
					}
					int minLevel = 1;
					int maxLevel = FS2GameDataManager.MaxLevel;
					object[] record = null;
					RoleLevelStatisticInfo[] infos = new RoleLevelStatisticInfo[maxLevel - minLevel + 1];
					for (int i = minLevel; i <= maxLevel; i++)
					{
						RoleLevelStatisticInfo info = new RoleLevelStatisticInfo((short)i);
						info.Counts = new UInt32[roleClasses.Length];
						infos[i - minLevel] = info;
					}

					for (int i = 0; i < roleClasses.Length; i++)
					{
						while ((record = results[i].ReadRecord()) != null)
						{
							Int16 level = (Int16)record[0];
							UInt32 count = (UInt32)record[1];
							infos[level - minLevel].Counts[i] = count;
						}
					}

					//绘制图表
					string[] roleClassNames = new string[] { StringDef.Jiashi, StringDef.Daoshi, StringDef.Yiren };

					graphPane.Title.Text = StringDef.RoleCountByLevel;
					graphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);
					graphPane.Legend.FontSpec.Fill.IsVisible = false;

					graphPane.Legend.Fill.IsVisible = false;
					graphPane.Legend.Border.IsVisible = false;

					graphPane.XAxis.Title.Text = StringDef.Level;
					graphPane.XAxis.Scale.Max = maxLevel + 1;
					graphPane.XAxis.Scale.Min = minLevel - 1;
					graphPane.XAxis.Scale.MajorStep = 5;
					graphPane.XAxis.Scale.MinorStep = 1;

					graphPane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;

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
						double[] levels = new double[infos.Length];

						for (int i = 0; i < roleClasses.Length; i++)
						{
							for (int j = 0; j < infos.Length; j++)
							{
								RoleLevelStatisticInfo info = infos[j] as RoleLevelStatisticInfo;
								counts[j] = info.Counts[i];
								levels[j] = info.Level;
							}

							BarItem barItem = graphPane.AddBar(roleClassNames[i], levels, counts, Colors[i]);
							barItem.Bar.Fill = new Fill(Colors[i]);
						}
					}
					else if (_chartType == ChartType.Line)
					{
						double[] counts = new double[infos.Length];
						double[] levels = new double[infos.Length];

						for (int i = 0; i < roleClasses.Length; i++)
						{
							for (int j = 0; j < infos.Length; j++)
							{
								RoleLevelStatisticInfo info = infos[j] as RoleLevelStatisticInfo;
								counts[j] = info.Counts[i];
								levels[j] = info.Level;
							}

							LineItem lineItem = graphPane.AddCurve(roleClassNames[i], levels, counts, Colors[i], (showSymbol ? WebConfig.GraphSymbols[i] : SymbolType.None));
							lineItem.Line.Width = 1;
						}
					}
					graphPane.AxisChange(g);
				}
			}
			else if (_statisticType == 2)
			{
				SqlCommand[] cmds = new SqlCommand[RoleSexCount];
				SqlResult[] results = new SqlResult[RoleSexCount];
				for (int i = 0; i < RoleSexCount; i++)
				{
					cmds[i] = new SqlCommand("SELECT RoleLevel,COUNT(*) FROM rolesfirst WHERE RoleSex=? GROUP BY RoleLevel", i);
                    if (!_server.IsConnected)
                    {
                        //LabelMessage.Visible = true;
                        //LabelMessage.Text = StringDef.NoConnectionAlert;
                        return;
                    }
                    results[i] = WebUtil.QueryGameServerDb(CurrentUser.Id, _server, cmds[i]);
				}

				bool success = true;
				for (int i = 0; i < RoleSexCount; i++)
				{
					if (results[i] == null || !results[i].Success)
					{
						success = false;
						break;
					}
				}

				if (success)
				{
					int minLevel = 1;
					int maxLevel = FS2GameDataManager.MaxLevel;

					//整理数据			
					SqlDataType[] dataTypes = new SqlDataType[] { SqlDataType.Int16, SqlDataType.UInt32 };
					for (int i = 0; i < RoleSexCount; i++)
					{
						results[i].SetFieldType(dataTypes);
					}

					object[] record = null;
					RoleLevelStatisticInfo[] infos = new RoleLevelStatisticInfo[maxLevel - minLevel + 1];
					for (int i = minLevel; i <= maxLevel; i++)
					{
						RoleLevelStatisticInfo info = new RoleLevelStatisticInfo((short)i);
						info.Counts = new UInt32[RoleSexCount];
						infos[i - minLevel] = info;
					}

					for (int i = 0; i < RoleSexCount; i++)
					{
						while ((record = results[i].ReadRecord()) != null)
						{
							Int16 level = (Int16)record[0];
							UInt32 count = (UInt32)record[1];
							infos[level - minLevel].Counts[i] = count;
						}
					}

					//绘制图表
					string[] roleSexNames = new string[] { StringDef.Male, StringDef.Female };

					graphPane.Title.Text = StringDef.RoleCountByLevel;
					graphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);

					graphPane.Legend.Fill.IsVisible = false;
					graphPane.Legend.Border.IsVisible = false;

					graphPane.XAxis.Title.Text = StringDef.Level;
					graphPane.XAxis.Scale.Max = maxLevel + 1;
					graphPane.XAxis.Scale.Min = minLevel - 1;
					graphPane.XAxis.Scale.MajorStep = 5;
					graphPane.XAxis.Scale.MinorStep = 1;
					graphPane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;

					graphPane.YAxis.Title.Text = StringDef.Count;
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
						double[] levels = new double[infos.Length];

						for (int i = 0; i < RoleSexCount; i++)
						{
							for (int j = 0; j < infos.Length; j++)
							{
								RoleLevelStatisticInfo info = infos[j] as RoleLevelStatisticInfo;
								counts[j] = info.Counts[i];
								levels[j] = info.Level;
							}

							BarItem barItem = graphPane.AddBar(roleSexNames[i], levels, counts, Colors[i]);
							barItem.Bar.Fill = new Fill(Colors[i]);
						}
					}
					else if (_chartType == ChartType.Line)
					{
						double[] counts = new double[infos.Length];
						double[] levels = new double[infos.Length];

						for (int i = 0; i < RoleSexCount; i++)
						{
							for (int j = 0; j < infos.Length; j++)
							{
								RoleLevelStatisticInfo info = infos[j] as RoleLevelStatisticInfo;
								counts[j] = info.Counts[i];
								levels[j] = info.Level;
							}

							LineItem lineItem = graphPane.AddCurve(roleSexNames[i], levels, counts, Colors[i], (showSymbol ? WebConfig.GraphSymbols[i] : SymbolType.None));
						}
					}

					graphPane.AxisChange(g);
				}
			}
		}
	}

	protected void LinkButtonStatistic_Click(object sender, EventArgs e)
	{
		try
		{			
            _server = ServerDropDownList.SelectedGameServer;
            //加入权限检查
            if (!WebUtil.CheckPrivilege(_server.SecurityObject, OpType.READ, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }
			_statisticType = int.Parse(ListBoxStatType.SelectedValue);
			_chartType = (ChartType)int.Parse(ListBoxChartType.SelectedValue);

			if (_server != null && _server.IsConnected && _statisticType >= 1)
			{
				ZedGraphWebStatistic.Visible = true;
			}
		}
		catch (Exception)
		{
		}
	}

	private class RoleLevelStatisticInfo
	{
		public Int16 Level;

		public UInt32[] Counts;

		public RoleLevelStatisticInfo(Int16 level)
		{
			Level = level;
			Counts = new UInt32[FS2GameDataManager.RoleClasses.Length];
		}
	}

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }
}