<%@ WebHandler Language="C#" Class="GameInfoHandler" %>

using System;
using System.Web;
using System.Text;
using System.Collections;
using System.Drawing;
using System.IO;
using System.Threading;
using FSEye;
using ZedGraph;
using Resources;

public class GameInfoHandler : IHttpHandler {

	/* AJAX请求的操作 */
	private const string OperationGetGameBasicInfo = "GetGameBasicInfo";
		
    public void ProcessRequest (HttpContext context)
	{
		if (!AdminServer.TheInstance.IsRunning)
		{
			context.Response.ContentType = "text/plain";
			context.Response.Output.Write(StringDef.ServiceUnavaiable);
			return;
		}
		
		//进行权限检查
        if (context.Session[WebConfig.SessionCurrentUser] == null)
        {
            //Response.Redirect(WebConfig.PageLogin, true);
            return;
        }
        UserInfo userInfo = context.Session[WebConfig.SessionCurrentUser] as UserInfo;
        if (userInfo == null) return;
        if (!AdminServer.TheInstance.SecurityManager.ValidateLoginUser(userInfo.Id, userInfo.LoginId))
        {
            context.Session.Remove(WebConfig.SessionCurrentUser);
            return;
        }
		
		if (context.Request.Params[WebConfig.ParamOperation] != null)
		{
			context.Response.ContentType = "text/plain";
			string op = context.Request.Params[WebConfig.ParamOperation];
			switch (op)
			{
				//游戏操作
				case OperationGetGameBasicInfo:
					ProcessGetGameBasicInfo(context);
					break;
				default:
					//TODO 记录错误
					break;
			}
		}
		else
		{
			//TODO 记录错误
		}
    }
 
    public bool IsReusable {
        get {
            return true;
        }
    }

	/// <summary>
	/// 获得游戏基本信息
	/// </summary>
	private void ProcessGetGameBasicInfo(HttpContext context)
	{
		if (context.Request.Params[WebConfig.ParamServerId] != null)
		{
			int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
			GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);
			//GameInfo[] infos = server.GameInfos;

			StringBuilder outputHtml = new StringBuilder();
			//outputHtml.Append("<table class='TableSection'><tr><td class='TableRowSectionTitle'>");
			//outputHtml.Append(StringDef.GameServerBasicInfo);
			//outputHtml.Append("</td></tr><tr><td>");
			//outputHtml.Append("<table border='1' class='TableNormal'><tr><td>");
			//outputHtml.Append(StringDef.ID);
			//outputHtml.Append("</td><td>");
			//outputHtml.Append(server.Id);
			//outputHtml.Append("</td></tr><tr><td>");
			//outputHtml.Append(StringDef.Name);
			//outputHtml.Append("</td><td>");
			//outputHtml.Append(server.Name);

			//GameInfo basicInfo = null;
			//if (infos != null && infos.Length > 0)
			//{
			//    basicInfo = infos[infos.Length - 1];
			//}

			//outputHtml.Append("</td></tr><tr><td>");
			//outputHtml.Append(StringDef.PlayerCount);
			//outputHtml.Append("</td><td>");
			//outputHtml.Append((basicInfo != null ? basicInfo.PlayerCount.ToString() : StringDef.Unknown));
			//outputHtml.Append("</td></tr><tr><td>");
			//outputHtml.Append(StringDef.ServiceTime);
			//outputHtml.Append("</td><td>");

			//outputHtml.Append("</td></tr></table>");
			//outputHtml.Append("</td></tr></table>");

			context.Response.Output.Write(outputHtml.ToString());
		}
	}

	private string CreateGetPlayerCountByLevelResult(int[] counts)
	{
		StringBuilder outputHtml = new StringBuilder();
		
		outputHtml.Append("<table border='1' width='200'>");
		
		outputHtml.Append("<tr><th>");
		outputHtml.Append("等级");
		outputHtml.Append("</th><th>");
		outputHtml.Append("数量");
		outputHtml.Append("</th></tr>");
		for (int i = 0; i < counts.Length; i++)
		{
			outputHtml.Append("<tr><td>");
			outputHtml.Append(i + 1);
			outputHtml.Append("</td><td>");
			outputHtml.Append(counts[i].ToString());
			outputHtml.Append("</td></tr>");
		}

		outputHtml.Append("</table>");

		return outputHtml.ToString();
	}

	///// <summary>
	///// 获得游戏服务器CPU负载
	///// </summary>
	//private void ProcessGetGameServerCpuLoad(HttpContext context)
	//{
	//    StringBuilder outputHtml = new StringBuilder();
	//    outputHtml.Append("<table class='TableSection'><tr><td class='TableRowSectionTitle'>");
	//    outputHtml.Append(StringDef.CpuLoad);
	//    outputHtml.Append("</td></tr><tr><td>");

	//    if (context.Request.Params[ParamServerId] != null)
	//    {
	//        int serverId = int.Parse(context.Request.Params[ParamServerId]);
	//        GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

	//        if (!CheckPrivilege(server.SecurityObject.GetChild(SecurityObject.ServerInfo).GetChild(SecurityObject.Cpu), OpType.READ, context.Session))
	//        {
	//            outputHtml.Append(StringDef.NotEnoughPrivilege);
	//        }
	//        else
	//        {
	//            if (server.CpuInfos != null && server.CpuInfos.Count > 0)
	//            {
	//                string imageFileName = null;

	//                DateTime last = DateTime.MinValue;
	//                if (g_lastGameServerCpuStateGraphFileTimeMap.ContainsKey(serverId))
	//                {
	//                    last = (DateTime)g_lastGameServerCpuStateGraphFileTimeMap[serverId];
	//                }
	//                double escaped = ((TimeSpan)(DateTime.Now - last)).TotalMilliseconds;
	//                if (escaped > g_gameServerCpuStateGraphPeriod)
	//                {
	//                    DeleteGameServerCpuLoadGraph(serverId);

	//                    imageFileName = Util.CreateRandomName("GameServerCpuLoad_", WebConfig.GraphFileSuffix);
	//                    if (CreateGameServerCpuLoadGraph(server, imageFileName))
	//                    {
	//                        outputHtml.Append(HtmlBuilder.CreateImage(
	//                            WebConfig.TempGraphUrl + imageFileName,
	//                            WebConfig.GameServerCpuLoadGraphWidth,
	//                            WebConfig.GameServerCpuLoadGraphHeight));
	//                    }
	//                    else
	//                    {
	//                        outputHtml.Append(StringDef.CreateGraphFail);
	//                    }
	//                }
	//                else
	//                {
	//                    string fileName = g_lastGameServerCpuStateGraphFile[serverId] as string;
	//                    if (fileName != null)
	//                    {
	//                        int index = fileName.LastIndexOf("\\");
	//                        imageFileName = fileName.Substring(index + 1);
	//                        outputHtml.Append(HtmlBuilder.CreateImage(
	//                                WebConfig.TempGraphUrl + imageFileName,
	//                                WebConfig.GameServerCpuLoadGraphWidth,
	//                                WebConfig.GameServerCpuLoadGraphHeight));
	//                    }
	//                }
	//            }
	//            else
	//            {
	//                outputHtml.Append(StringDef.NoInfo);
	//            }
	//        }
	//    }

	//    outputHtml.Append("</td></tr></table>");
	//    context.Response.Output.Write(outputHtml.ToString());
	//}

	///// <summary>
	///// 创建游戏服务器CPU负载图表
	///// </summary>
	//private bool CreateGameServerCpuLoadGraph(GameServer server, string imageFileName)
	//{
	//    try
	//    {
	//        IList infos = server.CpuInfos;

	//        if (infos != null && infos.Count > 0)
	//        {
	//            MasterPane cpuLoadMasterPane = new MasterPane();

	//            GraphPane currentCpuLoadPane = new GraphPane();

	//            currentCpuLoadPane.Margin.Top = 120;
	//            currentCpuLoadPane.Margin.Bottom = 100;
	//            currentCpuLoadPane.Margin.Right = 50;

	//            currentCpuLoadPane.Fill = new Fill(WebConfig.GraphPaneBgColor);
	//            currentCpuLoadPane.BarSettings.ClusterScaleWidth = 1;
	//            currentCpuLoadPane.Chart.Fill = new Fill(WebConfig.GraphChartBgColor);
	//            currentCpuLoadPane.Legend.IsVisible = false;

	//            currentCpuLoadPane.XAxis.IsVisible = false;

	//            currentCpuLoadPane.YAxis.Scale.Max = 110;
	//            currentCpuLoadPane.YAxis.Scale.Min = -10;
	//            currentCpuLoadPane.YAxis.Scale.FontSpec.Size = 60;
	//            currentCpuLoadPane.YAxis.Scale.Format = "0'%'";
	//            currentCpuLoadPane.YAxis.Scale.MajorStep = 25;
	//            currentCpuLoadPane.YAxis.MajorGrid.IsVisible = true;
	//            currentCpuLoadPane.YAxis.MajorGrid.Color = WebConfig.GraphYAxisGridColor;
	//            currentCpuLoadPane.YAxis.MajorGrid.DashOn = 0;

	//            cpuLoadMasterPane.Add(currentCpuLoadPane);
	//            double[] value = new double[1];
	//            double[] pos = new double[1];
	//            pos[0] = 1;
	//            for (int i = 0; i < infos.Count; i++)
	//            {
	//                CpuInfo info = infos[i] as CpuInfo;
	//                value[0] += info.Loads[info.Loads.Length - 1];
	//            }
	//            value[0] /= infos.Count;
	//            BarItem bar = currentCpuLoadPane.AddBar("Load", null, value, Color.Blue);
	//            bar.Bar.Fill = new Fill(WebConfig.GraphBarFillColor);
	//            bar.Bar.Border = new Border(WebConfig.GraphBarBorderColor, 1);
	//            TextObj cpuLoadText = new TextObj(value[0] + "%", 0.64F, 0.1F, CoordType.PaneFraction);
	//            cpuLoadText.FontSpec.Size = 60;
	//            currentCpuLoadPane.GraphObjList.Add(cpuLoadText);

	//            GraphPane cpuStatePane = new GraphPane();
	//            cpuLoadMasterPane.Add(cpuStatePane);

	//            cpuStatePane.Title.Text = "CPU Load";
	//            cpuStatePane.Title.FontSpec.Size = 40;
	//            cpuStatePane.Fill = new Fill(WebConfig.GraphPaneBgColor);
	//            cpuStatePane.Chart.Fill = new Fill(WebConfig.GraphChartBgColor);

	//            cpuStatePane.Legend.FontSpec.Size = 40;
	//            cpuStatePane.Legend.FontSpec.FontColor = WebConfig.GraphLegendFontColor;
	//            cpuStatePane.Legend.Position = LegendPos.Right;
	//            cpuStatePane.Legend.Fill = new Fill(WebConfig.GraphLegendBgColor);

	//            cpuStatePane.XAxis.Title.IsVisible = false;
	//            cpuStatePane.XAxis.Scale.IsVisible = true;
	//            cpuStatePane.XAxis.Scale.FontSpec.Size = 40;
	//            cpuStatePane.XAxis.Scale.Max = 100;
	//            cpuStatePane.XAxis.Scale.Min = 0;
	//            cpuStatePane.XAxis.Scale.MajorStep = 10;
	//            cpuStatePane.XAxis.MajorGrid.IsVisible = true;
	//            cpuStatePane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;

	//            cpuStatePane.YAxis.Title.Text = "Load";
	//            cpuStatePane.YAxis.Title.FontSpec.Size = 40;
	//            cpuStatePane.YAxis.Scale.FontSpec.Size = 40;
	//            cpuStatePane.YAxis.Scale.Max = 110;
	//            cpuStatePane.YAxis.Scale.Min = -10;
	//            cpuStatePane.YAxis.Scale.MajorStep = 25;
	//            cpuStatePane.YAxis.Scale.Format = "0'%'";
	//            cpuStatePane.YAxis.MajorGrid.IsVisible = true;
	//            cpuStatePane.YAxis.MajorGrid.DashOff = 0;
	//            cpuStatePane.YAxis.MajorGrid.Color = WebConfig.GraphYAxisGridColor;

	//            for (int i = 0; i < infos.Count; i++)
	//            {
	//                CpuInfo info = infos[i] as CpuInfo;
	//                ushort[] loads = info.Loads;
	//                double[] counts = new double[loads.Length];
	//                double[] time = new double[loads.Length];

	//                //如果记录队列未满，曲线向右对齐
	//                if (loads.Length < CpuInfo.LoadQueueMaxLength)
	//                {
	//                    int offset = CpuInfo.LoadQueueMaxLength - loads.Length;
	//                    for (int j = 0; j < loads.Length; j++)
	//                    {
	//                        time[j] = j + offset;
	//                        counts[j] = (int)loads[j];
	//                    }
	//                }
	//                else
	//                {
	//                    for (int j = 0; j < loads.Length; j++)
	//                    {
	//                        time[j] = j;
	//                        counts[j] = (int)loads[j];
	//                    }
	//                }
	//                Color color = Color.Yellow;
	//                if (i < WebConfig.GraphColors.Length)
	//                {
	//                    color = WebConfig.GraphColors[i];
	//                }
	//                cpuStatePane.AddCurve("CPU_" + i, time, counts, color, SymbolType.None);
	//            }

	//            Bitmap bitmap = new Bitmap(1, 1);
	//            using (Graphics g = Graphics.FromImage(bitmap))
	//            {
	//                cpuLoadMasterPane.AxisChange(g);
	//                cpuLoadMasterPane.SetLayout(g, false, new int[] { 1, 1 }, new float[] { 1, 6 });
	//            }

	//            bitmap = cpuLoadMasterPane.GetImage(WebConfig.GameServerCpuLoadGraphWidth, WebConfig.GameServerCpuLoadGraphHeight, 75.0f);
	//            string file = WebConfig.WebsiteRootPath + WebConfig.TempGraphPath + imageFileName;
	//            bitmap.Save(file, WebConfig.GraphImageFormat);
	//            g_lastGameServerCpuStateGraphFile[server.Id] = file;
	//            g_lastGameServerCpuStateGraphFileTimeMap[server.Id] = DateTime.Now;

	//            return true;
	//        }

	//        return false;
	//    }
	//    catch (Exception)
	//    {
	//        return false;
	//    }
	//}

	///// <summary>
	///// 删除游戏服务器CPU负载图表
	///// </summary>
	//private void DeleteGameServerCpuLoadGraph(int serverId)
	//{
	//    if (g_lastGameServerCpuStateGraphFile.ContainsKey(serverId))
	//    {
	//        FileInfo info = new FileInfo(g_lastGameServerCpuStateGraphFile[serverId] as string);
	//        if (info.Exists)
	//        {
	//            info.Delete();
	//        }
	//    }
	//}
}