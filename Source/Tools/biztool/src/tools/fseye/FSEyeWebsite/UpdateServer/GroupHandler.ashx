<%@ WebHandler Language="C#" Class="GroupHandler" %>

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2008-5-21
//      File_base        : copy from GameServerHandler
//      File_ext         : ashx
//      Author           : 
//      Description      : 专用于启动、关闭、下载、更新
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Web.SessionState;
using System.Drawing;
using System.Drawing.Imaging;
using System.Text;
using Resources;
using ZedGraph;
using FSEye;
using FSEye.Security;
using FSEye.WebControls;
using FSEye.PlugIn;

/// <summary>
/// 游戏服务器AJAX请求处理器
/// </summary>
/// <author>徐晓刚</author>
public class GroupHandler : IHttpHandler, IReadOnlySessionState 
{
	/* AJAX请求的操作 */

    //总览与监控(ServerOverviewAndMonitor.aspx)
    private const string OperationGetServerGroupOverviewMonitor = "GetServerGroupOverviewMonitor";

    //总览与监控(ServerOverviewAndMonitor.aspx) tooltip(悬浮窗)
    private const string OperationGetServerGroupSummary = "GetServerGroupSummary";

	public void ProcessRequest(HttpContext context)
	{        
		try
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
  
                //对Request进行分割
                //Request的形式为： op / selectGroupId / filter
                
                string fullOp = context.Request.Params[WebConfig.ParamOperation];
                String[] tempString = fullOp.Split('_');
                string op = tempString[0];
                string selectGroupId = string.Empty;
                string filter = string.Empty;
                if (tempString.Length == 3)
                {
                    selectGroupId = tempString[1];
                    filter = tempString[2];
                }
                
                
				switch (op)
				{
					
                    //总览与监控(ServerOverviewAndMonitor.aspx)                            
                    case OperationGetServerGroupOverviewMonitor:
                        ProcessGetServerGroupOverviewMonitor(context, selectGroupId, filter);
                        break;

                    //总览与监控(ServerOverviewAndMonitor.aspx)
                    //tooltip(悬浮窗)
                    case OperationGetServerGroupSummary:
                        ProcessGetServerGroupSummary(context);
                        break;
                                                                  
					default:
						//TODO 记录错误，测试时抛出异常
						throw new Exception("收到未知的GameServerHandler请求");
				}
			}
			else
			{
				//TODO 记录错误
			}
		}
		catch (Exception ex)
		{
			Util.DebugLog(ex.ToString());
		}
	}

	public bool IsReusable
	{
		get
		{
			return true;
		}
	}






    private void ProcessGetServerGroupOverviewMonitor(HttpContext context, string selectGroup, string filter)
    {
        IList netGroupList = AdminServer.TheInstance.GameServerManager.TopServerGroups;

        ServerGroup tempServerGroup = new ServerGroup();
        tempServerGroup.GroupType = ServerGroup.Type.Group;
        
        foreach (ServerGroup netGroup in netGroupList)
        {
            if (netGroup.GroupType == ServerGroup.Type.Server)
            {
                if (filter == "noFilter")
                {
                    tempServerGroup.List.Add(netGroup);
                }
                else if (filter == "filterOpenGroup" && netGroup.group_enable == ServerGroup.GroupEnable.Open)
                {
                    tempServerGroup.List.Add(netGroup);
                }
                else if (filter == "filterCloseGroup" && netGroup.group_enable == ServerGroup.GroupEnable.Close)
                {
                    tempServerGroup.List.Add(netGroup);
                }               
            }
            else if (netGroup.GroupType == ServerGroup.Type.Group)
            {
                GetActualServerGroup(context, netGroup, netGroup.Name, selectGroup, filter);
            }
        }

        if (tempServerGroup.List.Count > 0)
        {
            CreateServerGroupOverviewMonitor(context, tempServerGroup, "/", selectGroup);
        }
    }

    private void GetActualServerGroup(HttpContext context, ServerGroup netGroup, String groupName, String selectGroup, String filter)
    {
        if (context == null)
            throw new ArgumentNullException("context");
        if (netGroup == null)
            return;

        //实际上，传进来的netGroup一定类型是Group。因为已经在上一个函数处理过了
        if (netGroup.GroupType == ServerGroup.Type.Group)
        {
            IList childGroupIList = netGroup.List;
            ServerGroup tempServerGroup = new ServerGroup();
            tempServerGroup.GroupType = ServerGroup.Type.Group;
            
            if ((childGroupIList != null) && (childGroupIList.Count > 0))
            {                
                foreach (ServerGroup childGroup in childGroupIList)                
                {                    
                    if (childGroup.GroupType == ServerGroup.Type.Group)
                    {
                        string childGroupName = groupName + "/" + childGroup.Name;
                        GetActualServerGroup(context, childGroup, childGroupName, selectGroup, filter);
                    }
                    else if (childGroup.GroupType == ServerGroup.Type.Server)
                    {                        
                        if (filter == "noFilter")
                        {
                            tempServerGroup.List.Add(childGroup);
                        }
                        else if (filter == "filterOpenGroup" && childGroup.group_enable == ServerGroup.GroupEnable.Open)
                        {
                            tempServerGroup.List.Add(childGroup);
                        }
                        else if (filter == "filterCloseGroup" && childGroup.group_enable == ServerGroup.GroupEnable.Close)
                        {
                            tempServerGroup.List.Add(childGroup);
                        }   
                    }
                }
            }
            else
            {
                return;   
            }
            
            if (tempServerGroup.List.Count > 0)
            {
                IComparer myComparer = new ByServerGroupIdComparer();
                ArrayList tempArrayList = ArrayList.Adapter(tempServerGroup.List);
                tempArrayList.Sort(myComparer);
                tempServerGroup.List = (IList)tempArrayList;
                
                CreateServerGroupOverviewMonitor(context, tempServerGroup, groupName, selectGroup);
            }
        }
    }

    private void CreateServerGroupOverviewMonitor(HttpContext context, ServerGroup netGroup, String groupName, string selectGroup)
    {
        //=============================
        //    【进度条说明－－按优先级从高到低】
        //启动          按组的服务器程序数量为分母
        //关闭          按组的服务器程序数量为分母        
        //Updating      获取每个guard的更新情况，显示最小的
        //Downloading   获取每个guard的下载情况，显示最小的
        //=============================
                    
        using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
        {
            writer.Write("<table cellspacing='1' class='{0}' style='width:1000;'>", CssConfig.TableNormal);
            writer.Write("<tr><td  align='center' style='font-size:16px;color:#FF9900;weight:2000' class='{0}'>{1}</td></tr>", CssConfig.TableCellNormal, groupName);
            writer.Write("<tr>");
            writer.Write("<td>");
            IList childGroupIList = netGroup.List;

            //设置checkbox状态
            bool bCheckboxEnable;
            if (context.Session["bCheckboxEnable"] == null)
                bCheckboxEnable = true;
            else
                bCheckboxEnable = (bool)context.Session["bCheckboxEnable"];
            
            int count = 0;
            int groupCount = childGroupIList.Count;
            int groupPerRow = 10;
            string columnWidth = "width='" + (100 / groupPerRow) + "%'";
            int rowCount = groupCount / groupPerRow;
            int remainder = groupCount % groupPerRow;            
            if (remainder > 0)
                rowCount++;

            writer.Write("<table width=100% cellspacing=1 cellpadding=0>");

            for (int i = 0; i < rowCount; i++)
            {
                writer.Write("<tr>");

                for (int j = 0; j < groupPerRow; j++)
                {
                    if (count < groupCount)
                    {
                        //统计数据
                        double groupServerCount = 0;    //本组服务器程序数量
                        double startedCount = 0;    //本组已启动服务器程序数量
                        double stopedCount = 0;    //本组已关闭服务器程序数量

                        bool showProgessBar = false;
                        double progressBarValue = 0;
                        bool isDownloading = false;
                        bool isSelectGroup = false;
                        bool bNoConnected = false; //判断该组是否有网络连接
                       
                        
                        
                        ServerGroup groupToWrite = childGroupIList[i * groupPerRow + j] as ServerGroup;
                        IList serverIList = groupToWrite.List;
                        groupServerCount = groupToWrite.List.Count;

                        int enumLen = Enum.GetValues(typeof(GameServer.ServiceState)).GetLength(0) - 1 ; 
                        GameServer.ServiceState serverGroupState = GameServer.ParseServiceState(Enum.GetName(typeof(GameServer.ServiceState), enumLen));  //这个状态的颜色显示等级最低
                        //bool noWarning = true;
                        IList checkResults = AdminServer.TheInstance.GameServerMonitor.LastCheckResults;

                        Dictionary<string, double> mapIp = new Dictionary<string ,double>();
                        bool bUpdating = false;
                        //foreach (GameServer server in serverIList)
                        for (int iServer = 0; iServer < serverIList.Count; iServer++) 
                        {
                            GameServer server = (GameServer)serverIList[iServer];
                            if (server.GameServerState == GameServer.ServerState.Unknown)
                            {
                                bNoConnected = true;
                                break;
                            }                            
                            
                            if (!mapIp.ContainsKey(server.IpAddress))
                                mapIp.Add(server.IpAddress, 0);
                            
                            if (server.GameServiceState == GameServer.ServiceState.Running)
                            {
                                startedCount++;
                            }
                            else if (server.GameServiceState == GameServer.ServiceState.OKStopped)
                            {
                                stopedCount++;
                            } 
                            else if (server.GameServiceState == GameServer.ServiceState.Starting)
                            {
                               // showProgessBar = true;
                                progressBarValue = startedCount / groupServerCount;
                            }
                            else if (server.GameServiceState == GameServer.ServiceState.Stopping)
                            {
                                //showProgessBar = true;
                                progressBarValue = stopedCount / groupServerCount;
                            }

                            switch (server.UpdateOrDownState)
                            {
                                case GameServer.UpdateState.Downloaded:
                                case GameServer.UpdateState.Updated: 
                                    {
                                        mapIp[server.IpAddress] = 100;
                                        showProgessBar = true;
                                    } break;
                                case GameServer.UpdateState.Updating:
                                    {
                                        showProgessBar = true;
                                        isDownloading = false;
                                        if (server.UpdateMsg.Count > 0)
                                        {
                                            string[] str = server.UpdateMsg[server.UpdateMsg.Count - 1] as string[];
                                            double bar = (double.Parse(str[0]) / double.Parse(str[1]));
                                            mapIp[server.IpAddress] = bar * 100;
                                            if (bar == 0)
                                            {
                                                //只有有一个下载状态为0％，进度条为0％
                                                progressBarValue = 0;
                                                break;
                                            }
                                            else if (progressBarValue == 0)
                                            {
                                                //如果是循环到第一个值
                                                progressBarValue = bar;
                                            }
                                            else if (bar < progressBarValue)
                                            {
                                                //获取所有server.DownloadProgress中最小的值
                                                progressBarValue = bar;
                                            }
                                        }
                                        bUpdating = true;
                                    } break;
                                case GameServer.UpdateState.Downloading:
                                    {

                                        showProgessBar = true;
                                        isDownloading = true;
                                        mapIp[server.IpAddress] = server.DownloadProgress * 100;
                                        if (server.DownloadProgress == 0)
                                        {
                                            //只有有一个下载状态为0％，进度条为0％
                                            progressBarValue = 0;
                                            break;
                                        }
                                        else if (progressBarValue == 0)
                                        {
                                            //如果是循环到第一个值
                                            progressBarValue = server.DownloadProgress;
                                        }
                                        else if (server.DownloadProgress < progressBarValue)
                                        {
                                            //获取所有server.DownloadProgress中最小的值
                                            progressBarValue = server.DownloadProgress;
                                        }                                        
                                    } break;
                            }
                            
                            
                            if (server.GameServiceState < serverGroupState)
                            {
                                serverGroupState = server.GameServiceState;
                            }
                            
                            //if (noWarning == true)
                            //{
                            //    int indexOfServer = AdminServer.TheInstance.GameServerManager.Servers.IndexOf(server);
                            //    if (checkResults != null && indexOfServer < checkResults.Count && indexOfServer >= 0)
                            //    {
                            //        CheckResult result = checkResults[indexOfServer] as CheckResult;
                            //        noWarning = result.NoProblem;
                            //    }
                            //}
                        }

                        string color = String.Empty;
                        ////===================
                        //checkbox
                        
                        if (context.Session["SelectGroup"] != null && selectGroup == "")
                        {
                            selectGroup = context.Session["SelectGroup"].ToString();
                        }
                        if (selectGroup.IndexOf(";" + groupToWrite.Id.ToString() + ";") != -1)
                        {
                            isSelectGroup = true;
                        }
                        if (context.Session["JobType"] != null && isSelectGroup)
                        {
                            string jobtype = context.Session["JobType"].ToString();
                            if (jobtype != null)
                            {
                                switch (jobtype)
                                {
                                    case "2":
                                        serverGroupState = GameServer.ServiceState.Starting;
                                        break;
                                    case "3":
                                        serverGroupState = GameServer.ServiceState.Stopping;
                                        break;
                                }
                            }
                        }

                        if (bNoConnected)
                        {
                            color = "#996633";
                        }
                        else
                        {
                            switch (serverGroupState)
                            {
                                case GameServer.ServiceState.Running:
                                    color = "#00CC00"; //绿色
                                    break;
                                case GameServer.ServiceState.OKStopped:
                                    color = "DarkGray";
                                    break;
                                case GameServer.ServiceState.ErrorStopped:
                                    color = "Red";
                                    break;
                                //                            case GameServer.ServiceState.Updating:
                                //                              color = "MediumOrchid";
                                //                            break;
                                case GameServer.ServiceState.Stopping:
                                    color = "DeepSkyBlue";
                                    break;
                                case GameServer.ServiceState.Starting:
                                    color = "Gold";
                                    break;

                                default:
                                    color = "DarkGray";
                                    break;
                            }

                            if (serverGroupState != GameServer.ServiceState.Stopping && serverGroupState != GameServer.ServiceState.Starting && bUpdating)
                            {
                                color = "MediumOrchid";
                            }
                        }
                        
                        writer.Write("<td " + columnWidth + ">");
                        writer.Write("<table width='100%' cellpadding='0' cellspacing='1' class='" + CssConfig.ServerGroupOverviewMonitor + "' style='background-color:" + color + "' onmouseover='setNavigatingServerGroup(" + groupToWrite.Id + ");if (enableToolTip) {{ posTooltip(); showtip(); }}' onmouseout='hidetip();restoreTooltip();setNavigatingServerGroup(0);'>");
                        writer.Write("<tr height='10'>");

                        writer.Write("<td align='center'>");
                        CheckBox chk = new CheckBox();
                        chk.Attributes.Add("onclick", "checkboxClick('" + groupToWrite.Id + "')");
                        if (selectGroup.IndexOf(";"+groupToWrite.Id.ToString()+";") != -1) 
                        {
                            chk.Checked = true;                            
                        }
                        chk.Enabled = bCheckboxEnable;
                        chk.RenderControl(writer);
                        writer.Write("</td>");
                        ////===================
                        writer.Write("<td align='center'>");
                        writer.Write("<a style='color:blue' href='../gameserver/ServerList.aspx?serverGroupId=");
                        writer.Write(groupToWrite.Id);
                        writer.Write("'>");
                        if (groupToWrite.Name.Length <= 10)
                        {
                            writer.Write(groupToWrite.Name);
                        }
                        else
                        {
                            writer.Write(groupToWrite.Name.Substring(0, 8) + "..");
                        }
                        if (groupToWrite.group_enable == ServerGroup.GroupEnable.Close)
                        {
                            writer.Write("(" + StringDef.Disable + ")");
                        }
                        writer.Write("</a></td>");
                        
                        //2008-7-10 huangbiao 暂时取消读取告警信息
                        writer.Write("<td align='center'>");
                            //ProgressBar warningProgressBar = new ProgressBar();
                            //warningProgressBar.Width = new Unit(10, UnitType.Pixel);
                            //warningProgressBar.Height = new Unit(16, UnitType.Pixel);
                            ////ColorConverter myColorConverter = new ColorConverter();
                            ////warningProgressBar.BackColor = (Color)myColorConverter.ConvertFromString(color);
                            //if (noWarning)
                            //{
                            //    ColorConverter myColorConverter = new ColorConverter();
                            //    warningProgressBar.BackColor = (Color)myColorConverter.ConvertFromString(color);
                            //    warningProgressBar.BorderColor = (Color)myColorConverter.ConvertFromString(color);
                            //}
                            //else
                            //{
                            //    warningProgressBar.BackColor = Color.Black;
                            //    warningProgressBar.BorderColor = Color.Black;
                            //}
                            //warningProgressBar.BorderStyle = BorderStyle.Solid;
                            //warningProgressBar.BorderWidth = new Unit(1, UnitType.Pixel);
                            //warningProgressBar.ShowIndicator = false;                            
                            //warningProgressBar.RenderControl(writer);

                        writer.Write("</td>");             
                        writer.Write("</tr>");
                        writer.Write("<tr><td colspan='3' align='center'>" + groupToWrite.GroupVer + "</td></tr>");                        
                        writer.Write("<tr>");

                       

                        ////===================
                        //进度条

                        if (showProgessBar)
                        {

                            //细节进度条
                            writer.Write("<td width='102' colspan='3' style='border: outset 1px #FFFFFF;'>");

                            ProgressBar taskProgress1 = new ProgressBar();
                            taskProgress1.Progress = progressBarValue;
                            taskProgress1.Width = new Unit(100, UnitType.Pixel);
                            taskProgress1.Height = new Unit(6, UnitType.Pixel);
                            taskProgress1.CssClass = "LowLoad";
                            taskProgress1.ProcessedCssClass = "ProgressBarProcessed";
                            taskProgress1.IndicatorCssClass = "ProgressBarIndicator";
                            taskProgress1.CompletedCssClass = "ProgressBarCompleted";

                            if (isDownloading == false)
                            {
                                taskProgress1.BackColor = Color.DarkGray;
                            }
                            else
                            {
                                taskProgress1.ForeColor = Color.Cyan;
                            }
                            taskProgress1.RenderControl(writer);
                            writer.Write("</tr>");
                            writer.Write("<tr>");
                            
                            
                            //总进度条
                            writer.Write("<td width='102' colspan='3' style='border: outset 1px #FFFFFF;'>");
                            
                            ProgressBar taskProgress = new ProgressBar();
                            double nCurTotal = 0;
                            foreach(double nCur in mapIp.Values) 
                            {
                                nCurTotal += nCur;
                            }
                            taskProgress.Progress = nCurTotal*1.0 / (mapIp.Count * 100.0);
                            taskProgress.Width = new Unit(100, UnitType.Pixel);
                            taskProgress.Height = new Unit(9, UnitType.Pixel);
                            taskProgress.CssClass = "HighLoad";// "ProgressBarBorder";
                            taskProgress.ProcessedCssClass = "ProgressBarProcessed";
                            taskProgress.IndicatorCssClass = "ProgressBarIndicator";
                            taskProgress.CompletedCssClass = "ProgressBarCompleted";
                            
                            if (isDownloading == false)
                            {
                                taskProgress.BackColor = Color.DarkGray;
                            }
                            else
                            {
                                taskProgress.ForeColor = Color.Cyan;
                            }
                            taskProgress.RenderControl(writer);
                            writer.Write("</td>");
                            writer.Write("</td>");                          
                        }
                        else
                        {
                            writer.Write("<td></td>");
                        }

                        ////===================

                        writer.Write("</tr>");
                        writer.Write("</table>");
                        writer.Write("</td>");
                    }
                    else
                    {
                        writer.Write("<td " + columnWidth + ">");
                        writer.Write("</td>");
                    }

                    count++;
                }

                writer.Write("</tr>");
            }

            writer.Write("</table>");
            
            writer.Write("</td>");
            writer.Write("</tr>");
            writer.Write("</table>");
        }
    }

    
    private void ProcessGetServerGroupSummary(HttpContext context)
    {
        using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
        {
            if (context.Request.Params[WebConfig.ParamServerGroupId] != null)
            {
                int serverGroupId = int.Parse(context.Request.Params[WebConfig.ParamServerGroupId]);
                ServerGroup serverGroup = AdminServer.TheInstance.GameServerManager.GetGameServerGroup(serverGroupId);

                if (serverGroup != null)
                {
                    writer.Write("<table width='100%'>");
                    writer.Write("<tr><td style='font-weight:bold; text-align: center; color: Yellow;'>{0}</td></tr>", serverGroup.Name);
                    writer.Write("</table>");

                    writer.Write("<br />");
                    writer.Write("<table width='100%'>");
                    writer.Write("<tr><td style='text-align: center; color: White;'>{0}：{1}</td></tr>", "离线托管人数", serverGroup.OfflinePlayer.ToString());
                    writer.Write("<tr><td style='text-align: center; color: White;'>{0}：{1}</td></tr>", "历史最高人数", serverGroup.MaxPlayer.ToString());
                    writer.Write("<tr><td style='text-align: center; color: White;'>{0}：{1}</td></tr>", "版本号", serverGroup.GroupVer.ToString());
                    writer.Write("</table>");                    
                    
                    writer.Write("<table width='100%'>");
                    writer.Write("<tr style='color:white'><td align='center'>IP</td><td align='center'>服务器名</td><td align='center'>状态</td><td align='center'>说明</td></tr>");

                    writer.Write("<tr class='TableRowSpacing' />");

                    IList serverIList = serverGroup.List;
                    
                    //对servers进行排序，以.Name属性进行排序                          
                    IComparer myComparer = new ByServerNameComparer();
                    ArrayList tempArrayList = ArrayList.Adapter(serverIList);
                    tempArrayList.Sort(myComparer);
                    serverIList = (IList)tempArrayList;

                    foreach (GameServer server in serverIList)
                    {
                        string serverState = String.Empty;
                        string color = "Black";

                        if (server.GameServerState == GameServer.ServerState.Unknown)
                        {
                            serverState = "网络断开";
                            color = "#996633";
                        }
                        else
                        {
                            switch (server.GameServiceState)
                            {
                                case GameServer.ServiceState.Running:
                                    serverState = "运行";
                                    color = "#00CC00"; //绿色
                                    break;
                                case GameServer.ServiceState.OKStopped:
                                    serverState = "正常关闭";
                                    color = "DarkGray";
                                    break;
                                case GameServer.ServiceState.ErrorStopped:
                                    serverState = "故障停止";
                                    color = "Red";
                                    break;
                                //                            case GameServer.ServiceState.Updating:
                                //                              serverState = "更新中";
                                //                            color = "MediumOrchid";
                                //                          break;
                                case GameServer.ServiceState.Stopping:
                                    serverState = "关闭中";
                                    color = "DeepSkyBlue";
                                    break;
                                case GameServer.ServiceState.Starting:
                                    serverState = "启动中";
                                    color = "Gold";
                                    break;

                                default:
                                    break;
                            }
                        }

                        writer.Write("<tr style='color:{0}'><td align='center'>{1}</td><td align='center'>{2}</td><td align='center'>{3}</td><td align='center'>{4}</td></tr>", color, server.IpAddress, server.Name, serverState, String.Empty);
                    }
                    
                    writer.Write("</table>");
                }
            }
        }
    }
    
	

    //按NAME排序，供函数CreateServerDecribeTable使用
    public class ByServerNameComparer : IComparer
    {
        int IComparer.Compare(Object x, Object y)
        {
            string xName = ((GameServer)x).Name;
            string yName = ((GameServer)y).Name;
            return ((new CaseInsensitiveComparer()).Compare(xName, yName));
        }
    }

    //按ID排序
    public class ByServerGroupIdComparer : IComparer
    {
        int IComparer.Compare(Object x, Object y)
        {
            int xId = ((ServerGroup)x).Id;
            int yId = ((ServerGroup)y).Id;
            return ((new CaseInsensitiveComparer()).Compare(xId, yId));
        }
    }
}
