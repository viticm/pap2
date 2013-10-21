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
using ZedGraph;
using FSEye;
using FSEye.DAL;
using FSEye.Security;
using FSEye.FS2Lib;

public partial class GameInfo_MoneyStatistic : BasePage
{
	private static Color[] Colors = new Color[] { Color.Blue, Color.Purple, Color.Red, Color.Green, Color.HotPink };

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticMoneyStatistic, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (!IsPostBack)
		{
			SetCurrentDate();
		}
	}

	private void SetCurrentDate()
	{
		DateTime now = DateTime.Now;
		TextBoxDateFrom.Text = now.ToShortDateString();
		TextBoxDateTo.Text = now.ToShortDateString();
	}

	protected void LinkButtonStatistic_Click(object sender, EventArgs e)
	{
		try
		{			
            GameServer server = ServerDropDownList.SelectedGameServer;

			try
			{
				DateTime dateFrom = DateTime.Parse(TextBoxDateFrom.Text);
			}
			catch (Exception)
			{
				ShowMessage(StringDef.Date + "(" + TextBoxDateFrom.Text + ") " + StringDef.InvalidFormat, MessageType.Failure);
			}

			try
			{
				DateTime dateTo = DateTime.Parse(TextBoxDateTo.Text);
			}
			catch (Exception)
			{
				ShowMessage(StringDef.Date + "(" + TextBoxDateTo.Text + ") " + StringDef.InvalidFormat, MessageType.Failure);
			}

			ChartType chartType = (ChartType)int.Parse(ListBoxChartType.SelectedValue);

			if (server != null)
			{
				string imageName = CreateStatisticMoneyImage(server, TextBoxDateFrom.Text, TextBoxDateTo.Text, chartType, true);
				if (imageName != null)
				{
					ImageStatistic.ImageUrl = WebConfig.TempGraphUrl + imageName;
					ImageStatistic.Visible = true;
				}
				else
				{
					ImageStatistic.Visible = false;
				}
			}
		}
		catch (Exception)
		{			
			ImageStatistic.Visible = false;
		}
	}

	private string CreateStatisticMoneyImage(GameServer server, string dateFrom, string dateTo, ChartType type, bool showSymbol)
	{
		using (IBlazeDatabase db = DbFactory.GetDatabase())
		{
			IBlazeTable gameLogTable = db.GetTable(TableString.GameLogTableName);
			DataSet data = new DataSet();			

			gameLogTable.Get(
				data,
				FilterFactory.CreateAndFilter(
					FilterFactory.CreateEqualFilter(TableString.GameLogFieldGameServerId, server.Id),
					FilterFactory.CreateAndFilter(
						FilterFactory.CreateLargerEqualFilter(TableString.GameLogFieldDate, dateFrom),
						FilterFactory.CreateLesserEqualFilter(TableString.GameLogFieldDate, dateTo)
					)
				)
			);

			DataTable table = data.Tables[0];

			int count = table.Rows.Count;
			double[] money = new double[count];
			double[] moneyInBox = new double[count];
			double[] moneyTotal = new double[count];
			double[] date = new double[count];
			for (int i = 0; i < count; i++)
			{
				DataRow row = table.Rows[i];
				money[i] = (long)row[TableString.GameLogFieldTotalMoney];
				moneyInBox[i] = (long)row[TableString.GameLogFieldTotalMoneyInBox];
				moneyTotal[i] = money[i] + moneyInBox[i];
				DateTime currentDate = (DateTime)row[TableString.GameLogFieldDate];
				date[i] = new XDate(currentDate.Year, currentDate.Month, currentDate.Day);
			}

			bool success = true;

			if (success)
			{
				//»æÖÆÍ¼±í
				GraphPane graphPane = new GraphPane();

				graphPane.Title.Text = StringDef.MoneyStatistic;
				graphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);

				graphPane.Legend.Fill.IsVisible = false;
				graphPane.Legend.Border.IsVisible = false;

				graphPane.XAxis.Title.Text = StringDef.Date;
				graphPane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;
				graphPane.XAxis.Type = AxisType.DateAsOrdinal;				
				graphPane.XAxis.MinorTic.Size = 0;
				graphPane.XAxis.Scale.MajorStep = 1;
				graphPane.XAxis.Scale.MajorUnit = DateUnit.Day;				
				graphPane.XAxis.Scale.FontSpec.Size = 12;
				graphPane.XAxis.Scale.Format = "M-d";

				graphPane.YAxis.Title.Text = StringDef.Money;
				graphPane.YAxis.MajorGrid.IsVisible = true;
				graphPane.YAxis.MajorGrid.DashOff = 0;
				graphPane.YAxis.MajorGrid.Color = Color.Gray;
				graphPane.YAxis.MinorGrid.IsVisible = true;
				graphPane.YAxis.MinorGrid.Color = Color.LightGray;
				graphPane.YAxis.MinorGrid.DashOff = 0;

				if (type == ChartType.Bar)
				{
					graphPane.BarSettings.Type = BarType.Stack;

					BarItem barItemMoney = graphPane.AddBar(StringDef.Money, date, money, Colors[0]);
					BarItem barItemMoneyInBox = graphPane.AddBar(StringDef.MoneyInBox, date, moneyInBox, Colors[1]);
					barItemMoney.Bar.Fill = new Fill(Colors[0]);
					barItemMoneyInBox.Bar.Fill = new Fill(Colors[1]);
				}
				else if (type == ChartType.Line)
				{
					LineItem lineItemMoney = graphPane.AddCurve(StringDef.Money, date, money, Colors[0], (showSymbol ? WebConfig.GraphSymbols[0] : SymbolType.None));
					LineItem lineItemMoneyInBox = graphPane.AddCurve(StringDef.MoneyInBox, date, moneyInBox, Colors[1], (showSymbol ? WebConfig.GraphSymbols[1] : SymbolType.None));
					LineItem lineItemMoneyTotal = graphPane.AddCurve(StringDef.MoneyTotal, date, moneyTotal, Colors[2], (showSymbol ? WebConfig.GraphSymbols[2] : SymbolType.None));
				}

				Bitmap bitmap = new Bitmap(1, 1);
				using (Graphics g = Graphics.FromImage(bitmap))
				{
					graphPane.AxisChange(g);
				}

				bitmap = graphPane.GetImage(WebConfig.StatisticRoleCountByLevelGraphWidth, WebConfig.StatisticRoleCountByLevelGraphHeight, 75.0f);
				string imageName = WebUtil.CreateRandomName("Statistic", WebConfig.GraphFileSuffix);
				string file = WebConfig.WebsiteRootPath + WebConfig.TempGraphPath + imageName;
				try
				{
					bitmap.Save(file, WebConfig.GraphImageFormat);
					TempFileManager.TheInstance.AddTempFile(file, 5000 * WebConfig.TempGraphDeleteDelayMultiple);

					return imageName;
				}
				catch (Exception)
				{
					//TODO ¼ÇÂ¼´íÎó
					return null;
				}
			}

			return null;
		}
	}
}
