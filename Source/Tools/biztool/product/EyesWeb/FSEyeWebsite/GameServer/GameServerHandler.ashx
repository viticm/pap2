<%@ WebHandler Language="C#" Class="GameServerHandler" %>

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-8-24
//      File_base        : GameServerHandler
//      File_ext         : ashx
//      Author           : 徐晓刚
//      Description      : 游戏服务器AJAX请求处理器
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
public class GameServerHandler : IHttpHandler, IReadOnlySessionState 
{
	/* AJAX请求的操作 */

	private const string OperationGetGameServerBasicInfo = "GetGameServerBasicInfo";

	private const string OperationGetGameServerCpuBasicInfo = "GetGameServerCpuBasicInfo";

	private const string OperationGetGameServerDiskUsage = "GetGameServerDiskUsage";

	private const string OperationGetGameServerFMTaskList = "GetGameServerFMTaskList";

	private const string OperationGetGameServerCpuLoad = "GetGameServerCpuLoad";

	private const string OperationGetGameServerNetworkCardInfo = "GetGameServerNetworkCardInfo";

	private const string OperationGetGameServerProcessList = "GetGameServerProcessList";

	private const string OperationGetGameServerMemUsage = "GetGameServerMemUsage";

	private const string OperationGetGameServerNetworkLoad = "GetGameServerNetworkLoad";

	private const string OperationGetGameServerDesc = "GetGameServerDesc";

	private const string OperationGetGameServerPlugInInfo = "GetGameServerPlugInInfo";

	private const string OperationGetGameServerSummary = "GetGameServerSummary";

	private const string OperationCancelFMTask = "CancelFMTask";
    private const string OperationStartGameServer = "StartGameServer";

    private const string OperationStopGameServer = "StopGameServer";

	private const string OperationGetGameServerOverview = "GetGameServerOverview";

    //总览与监控(ServerOverviewAndMonitor.aspx)
    private const string OperationGetServerGroupOverviewMonitor = "GetServerGroupOverviewMonitor";
    private const string OperationGetCabinetMonitor = "GetCabinetMonitor";

    //总览与监控(ServerOverviewAndMonitor.aspx)
    //tooltip(悬浮窗)
    private const string OperationGetServerGroupSummary = "GetServerGroupSummary";

	private const string OperationGetExeSysCmdOutput = "GetExeSysCmdOutput";

	private const string OperationExeSysCmd = "ExeSysCmd";

    private const string OperationCreateGuardConfigFile = "CreateGuardConfigFile";
    
	private const string OperationCreateLordConfigFile = "CreateLordConfigFile";

	private const string OperationGetPlayerCount = "GetPlayerCount";

    private const string OperationFSEyeFileDownload = "GetFSEyeFile";
    
    private const string OperationGetGameServerOverviewPlayerCount = "GetGameServerOverviewPlayerCount";

    private const string OperationGetServerGroupPlayerCount = "GetServerGroupPlayerCount";

    private const string OperationGetGameServerList = "GetGameServerList";

    private const string OperationCreateServerListFile = "CreateServerListFile";

    private const string OperationCreateCurrentServerListFile = "CreateCurrentServerListFile";
    
    private const string OperationGetGameServerListShowConnectedOnly = "GetGameServerListShowConnectedOnly";

    private const string OperationGetGameServerListByType = "GetGameServerListByType";    
    
    private const string OperationGetGameServerListByTypeShowConnectedOnly = "GetGameServerListByTypeShowConnectedOnly";

    private const string OperationGetGameServerListByGroup = "GetGameServerListByGroup";

    private const string OperationGetGameServerListByGroupShowConnectedOnly = "GetGameServerListByGroupShowConnectedOnly";


    private const string OperationGetGameServerListShowDisconnectedOnly = "GetGameServerListShowDisconnectedOnly";

    private const string OperationGetGameServerListByTypeShowDisconnectedOnly = "GetGameServerListByTypeShowDisconnectedOnly";

    private const string OperationGetGameServerListByGroupShowDisconnectedOnly = "GetGameServerListByGroupShowDisconnectedOnly";
    
    
    
        
     
    
	
	/* 生成的图片 */
	static Hashtable g_lastGameServerCpuStateGraphFile = new Hashtable();

	static Hashtable g_lastGameServerDiskInfoGraphFile = new Hashtable();

	static Hashtable g_lastGameServerMemoryUsageGraphFile = new Hashtable();

	static Hashtable g_lastGameServerNetworkLoadGraphFile = new Hashtable();

	static Hashtable g_lastGameServerCpuStateGraphFileTimeMap = new Hashtable();

	static Hashtable g_lastGameServerDiskInfoGraphFileTimeMap = new Hashtable();

	static Hashtable g_lastGameServerMemoryUsageGraphFileTimeMap = new Hashtable();

	static Hashtable g_lastGameServerNetworkLoadGraphFileTimeMap = new Hashtable();

	static long g_gameServerCpuStateGraphPeriod = 3000;

	static long g_gameServerDiskInfoGraphPeriod = 5000;

	static long g_gameServerMemoryUsageGraphPeriod = 3000;

	static long g_gameServerNetworkLoadGraphPeriod = 3000;

	const int GameServerOverviewColumnPerRow = 7;
    
    const int GameServerOverviewPlayerCountColumnPerRow = 7;

    const int DefaultGameServerMaxPlayerCount = 500;

    const double LowLimit = 0.3;

    const double MidLimit = 0.8;


    enum Operation
    {
        start,
        stop
    }
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
                
                //my code//
                //++
                
                //对Request进行分割
                //Request的形式为：
                //op
                //或者
                //op_serverType
                string fullOp = context.Request.Params[WebConfig.ParamOperation];
                String[] tempString = fullOp.Split(new char[] { '_' }, 2);
                string op = tempString[0];
                string extraMessage = null;                
                if (tempString.Length == 2)              
                    extraMessage = tempString[1];
                
                //++
                //my code//
               
                
				switch (op)
				{
					case OperationGetGameServerList:
						ProcessGetGameServerList(context);
						break;
					case OperationGetGameServerBasicInfo:
						ProcessGetGameServerBasicInfo(context);
						break;
					case OperationGetGameServerCpuBasicInfo:
						ProcessGetGameServerCpuBasicInfo(context);
						break;
					case OperationGetGameServerDiskUsage:
						ProcessGetGameServerDiskUsage(context);
						break;
					case OperationGetGameServerFMTaskList:
						ProcessGetGameServerFMTaskList(context);
						break;
					case OperationGetGameServerCpuLoad:
						ProcessGetGameServerCpuLoad(context);
						break;
					case OperationGetGameServerNetworkCardInfo:
						ProcessGetGameServerNetworkCardInfo(context);
						break;
					case OperationGetGameServerProcessList:
						ProcessGetGameServerProcessList(context);
						break;
					case OperationGetGameServerMemUsage:
						ProcessGetGameServerMemoryUsage(context);
						break;
					case OperationGetGameServerNetworkLoad:
						ProcessGetGameServerNetworkLoad(context);
						break;
					case OperationGetGameServerDesc:
						ProcessGetGameServerDesc(context);
						break;
					case OperationGetGameServerPlugInInfo:
						ProcessGetGameServerPlugInInfo(context);
						break;
					case OperationGetGameServerSummary:
						ProcessGetGameServerSummary(context);
						break;
					case OperationGetGameServerOverview:
						ProcessGetGameServerOverview(context);
						break;

                    //总览与监控(ServerOverviewAndMonitor.aspx)                            
                    case OperationGetServerGroupOverviewMonitor:
                        ProcessGetServerGroupOverviewMonitor(context);
                        break;
                        
                    //按机柜监控
                    case OperationGetCabinetMonitor:
                        ProcessGetCabinetOverviewMonitor(context);
                        break;

                    //总览与监控(ServerOverviewAndMonitor.aspx)
                    //tooltip(悬浮窗)
                    case OperationGetServerGroupSummary:
                        ProcessGetServerGroupSummary(context);
                        break;
                    //my code//
                    //++

                    //无过滤返回列表，但只返回已连接的服务器
                    case OperationGetGameServerListShowConnectedOnly:
                        ProcessGetGameServerListShowConnectedOnly(context);
                        break;                    
                    
                    //按服务器类型来返回列表(已连接和无连接都显示)
                    case OperationGetGameServerListByType:
                        ProcessGetGameServerListByType(context, extraMessage);
                        break;

                    //按服务器类型来返回列表(只显示已连接的)
                    case OperationGetGameServerListByTypeShowConnectedOnly:
                        ProcessGetGameServerListByTypeShowConnectedOnly(context, extraMessage);
                        break;                                                                             
                        
                    //按服务器组来返回列表(已连接和无连接都显示)
                    case OperationGetGameServerListByGroup:
                        ProcessOperationGetGameServerListByGroup(context, extraMessage);
                        break;

                    //按服务器组来返回列表(只显示已连接的)
                    case OperationGetGameServerListByGroupShowConnectedOnly:
                        ProcessOperationGetGameServerListByGroupShowConnectedOnly(context, extraMessage);
                        break;





                    //无过滤返回列表，但只返回未连接的服务器    
                    case OperationGetGameServerListShowDisconnectedOnly:
                        ProcessOperationGetGameServerListShowDisconnectedOnly(context);
                        break;
                    
                    //按服务器类型来返回列表(只显示未连接的)   
                    case OperationGetGameServerListByTypeShowDisconnectedOnly:
                        ProcessOperationGetGameServerListByTypeShowDisconnectedOnly(context, extraMessage);
                        break;

                    //按服务器组来返回列表(只显示未连接的)        
                    case OperationGetGameServerListByGroupShowDisconnectedOnly:
                        ProcessOperationGetGameServerListByGroupShowDisconnectedOnly(context, extraMessage);
                        break;                                   
                        
                    //++
                    //my code//
                    
                    case OperationCreateServerListFile:
                        ProcessCreateServerListFile(context);
                        break;
                    case OperationCreateCurrentServerListFile:
                        ProcessCreateCurrentServerListFile(context);
                        break;
                                                

					//下面是服务器操作
					case OperationCancelFMTask:
						ProcessCancelFMTask(context);
						break;						
					case OperationGetExeSysCmdOutput:
						ProcessGetExeSysCmdOutput(context);
						break;
					case OperationExeSysCmd:
						ProcessExeSysCmd(context);
						break;
                    case OperationCreateGuardConfigFile:
                        ProcessCreateGuardConfigFile(context);
                        break;
					case OperationCreateLordConfigFile:
						ProcessCreateLordConfigFile(context);
						break;
					case OperationGetPlayerCount:
						ProcessGetPlayerCount(context);
						break;					
                    case OperationFSEyeFileDownload:
                        ProcessGetFSEyeFile(context);
                        break;
                    case OperationGetGameServerOverviewPlayerCount:
                        ProcessGetGameServerOverviewPlayerCount(context);
                        break;
                    case OperationGetServerGroupPlayerCount:
                        ProcessGetServerGroupPlayerCount(context);
                        break;
                    case OperationStartGameServer:
                        ProcessStartGameServer(context);
                        break;
                    case OperationStopGameServer:
                        ProcessStopGameServer(context);
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

	/// <summary>
	/// 获得游戏服务器列表
	/// </summary>
	private void ProcessGetGameServerList(HttpContext context)
	{
        UserInfo userInfo = (UserInfo)context.Session["CurrentUser"];
        IList netGroupList = AdminServer.TheInstance.GameServerManager.TopServerGroups;
       
        foreach (ServerGroup netGroup in netGroupList)
        {
            GetServerListTable(context, netGroup,"");          
        }      
	}
    
    
    //my code//
    //++

    //递归地遍历所有的服务器，将targetGroup中符合条件的服务器加入到tempServerGroup中。
    //只取未连接的服务器。其他的和函数recursiveVisitAllServers相同
    private void recursiveVisitAllServersDisconnectedOnly(ServerGroup tempServerGroup, ServerGroup targetGroup, string compareMessage, string byWhat)
    {
        if (targetGroup.GroupType == ServerGroup.Type.Group)
        {
            IList childGroupList = targetGroup.List;
            foreach (ServerGroup childGroup in childGroupList)
            {
                if (childGroup.GroupType == ServerGroup.Type.Server)
                {
                    //无过滤返回服务器列表
                    if (byWhat == "fullList")
                    {
                        foreach (GameServer server in childGroup.List)
                        {
                            if (server.IsConnected == false)
                            {
                                tempServerGroup.List.Add(server);
                            }
                        }
                    }
                    //按服务器类型
                    else if (byWhat == "byType")
                    {
                        foreach (GameServer server in childGroup.List)
                        {
                            if (server.Type.ToString() == compareMessage)
                            {
                                if (server.IsConnected == false)
                                {
                                    tempServerGroup.List.Add(server);
                                }
                            }
                        }
                    }
                    //按服务器组
                    else if (byWhat == "byGroup")
                    {
                        if (childGroup.Id.ToString() == compareMessage)
                        {
                            foreach (GameServer server in childGroup.List)
                            {
                                if (server.IsConnected == false)
                                {
                                    tempServerGroup.List.Add(server);
                                }
                            }
                            FSEyeObject tempObject = new FSEyeObject();
                            tempObject.Name = childGroup.Name; ;
                            tempServerGroup.SecurityObject = tempObject;
                        }
                    }                    
                }
                else if (childGroup.GroupType == ServerGroup.Type.Group)                    
                {
                    recursiveVisitAllServersDisconnectedOnly(tempServerGroup, childGroup, compareMessage, byWhat);
                }

            }
        }
        else if (targetGroup.GroupType == ServerGroup.Type.Server)
        {
            //无过滤返回服务器列表
            if (byWhat == "fullList")
            {
                foreach (GameServer server in targetGroup.List)
                {
                    if (server.IsConnected == false)
                    {
                        tempServerGroup.List.Add(server);
                    }
                }            
            }            
            //按服务器类型
            else if (byWhat == "byType")
            {
                foreach (GameServer server in targetGroup.List)
                {
                    if (server.Type.ToString() == compareMessage)
                    {
                        if (server.IsConnected == false)
                        {
                            tempServerGroup.List.Add(server);
                        }
                    }
                }
            }
            //按服务器组
            else if (byWhat == "byGroup")
            {
                if (targetGroup.Id.ToString() == compareMessage)
                {
                    foreach (GameServer server in targetGroup.List)
                    {
                        if (server.IsConnected == false)
                        {
                            tempServerGroup.List.Add(server);
                        }
                    }

                    FSEyeObject tempObject = new FSEyeObject();
                    tempObject.Name = targetGroup.Name; ;
                    tempServerGroup.SecurityObject = tempObject;
                }                                           
            } 
        }        
    }        
            
    //递归地遍历所有的服务器，将targetGroup中符合条件的服务器加入到tempServerGroup中。connected为true表示只取已连接的服务器，为false表示取所有的服务器。
    private void recursiveVisitAllServers(ServerGroup tempServerGroup, ServerGroup targetGroup, string compareMessage, string byWhat, bool connected)
    {
        if (targetGroup.GroupType == ServerGroup.Type.Group)
        {
            IList childGroupList = targetGroup.List;
            foreach (ServerGroup childGroup in childGroupList)
            {
                if (childGroup.GroupType == ServerGroup.Type.Server)
                {
                    //无过滤返回服务器列表
                    if (byWhat == "fullList")
                    {
                        foreach (GameServer server in childGroup.List)
                        {
                            //若以"fullList"调用此函数，则一定是"bool connected=true"的
                            if (server.IsConnected == connected)
                            {
                                tempServerGroup.List.Add(server);
                            }
                        }
                    }
                    //按服务器类型
                    else if (byWhat == "byType")
                    {
                        foreach (GameServer server in childGroup.List)
                        {
                            if (server.Type.ToString() == compareMessage)
                            {
                                if (connected == false)
                                {
                                    tempServerGroup.List.Add(server);
                                }
                                else
                                {
                                    if (server.IsConnected)
                                    {
                                        tempServerGroup.List.Add(server);
                                    }
                                }
                            }
                        }
                    }
                    //按服务器组
                    else if (byWhat == "byGroup")
                    {
                        if (childGroup.Id.ToString() == compareMessage)
                        {
                            if (connected == false)
                            {
                                foreach (GameServer server in childGroup.List)
                                {
                                    tempServerGroup.List.Add(server);
                                }
                            }
                            else
                            {
                                foreach (GameServer server in childGroup.List)
                                {
                                    if (server.IsConnected)
                                    {
                                        tempServerGroup.List.Add(server);
                                    }
                                }
                            }
                            
                            FSEyeObject tempObject = new FSEyeObject();
                            tempObject.Name = childGroup.Name; ;
                            tempServerGroup.SecurityObject = tempObject;
                        }
                    }                    
                }
                else if (childGroup.GroupType == ServerGroup.Type.Group)                    
                {
                    recursiveVisitAllServers(tempServerGroup, childGroup, compareMessage, byWhat, connected);
                }

            }
        }
        else if (targetGroup.GroupType == ServerGroup.Type.Server)
        {
            //无过滤返回服务器列表
            if (byWhat == "fullList")
            {
                foreach (GameServer server in targetGroup.List)
                {
                    //若以"fullList"调用此函数，则一定是"bool connected=true"的
                    if (server.IsConnected == connected)
                    {
                        tempServerGroup.List.Add(server);
                    }
                }            
            }            
            //按服务器类型
            else if (byWhat == "byType")
            {
                foreach (GameServer server in targetGroup.List)
                {
                    if (server.Type.ToString() == compareMessage)
                    {
                        if (connected == false)
                        {
                            tempServerGroup.List.Add(server);
                        }
                        else
                        {
                            if (server.IsConnected)
                            {
                                tempServerGroup.List.Add(server);
                            }
                        }
                    }
                }
            }
            //按服务器组
            else if (byWhat == "byGroup")
            {
                if (targetGroup.Id.ToString() == compareMessage)
                {
                    if (connected == false)
                    {
                        foreach (GameServer server in targetGroup.List)
                        {
                            tempServerGroup.List.Add(server);
                        }
                    }
                    else
                    {
                        foreach (GameServer server in targetGroup.List)
                        {
                            if (server.IsConnected)
                            {
                                tempServerGroup.List.Add(server);
                            }
                        }
                    }
                    FSEyeObject tempObject = new FSEyeObject();
                    tempObject.Name = targetGroup.Name; ;
                    tempServerGroup.SecurityObject = tempObject;
                }                                           
            } 
        }        
    }
    
    
    //按服务器类型来返回列表的处理函数
    private void ProcessGetGameServerListByType(HttpContext context, string serverType)
    {
        UserInfo userInfo = (UserInfo)context.Session["CurrentUser"];
        IList netGroupList = AdminServer.TheInstance.GameServerManager.TopServerGroups;

        ServerGroup tempQualifiedServerGroup = new ServerGroup();
        
        foreach (ServerGroup netGroup in netGroupList)
        {

            recursiveVisitAllServers(tempQualifiedServerGroup, netGroup, serverType, "byType", false);                        
            
            /*考虑递归之前的代码
            if (netGroup.GroupType == ServerGroup.Type.Server)
            {
                foreach (GameServer server in netGroup.List)
                {
                    if (server.Type.ToString() == serverType)
                    {
                        tempQualifiedServerGroup.List.Add(server);
                    }
                }
            }
            */ 
        }

        FSEyeObject tempObject = new FSEyeObject();
        tempObject.Name = serverType;
        tempQualifiedServerGroup.SecurityObject = tempObject;
        GetServerListTable(context, tempQualifiedServerGroup, "");                               
    }

    //无过滤返回列表，但只返回已连接的服务器的处理函数
    private void ProcessGetGameServerListShowConnectedOnly(HttpContext context)
    {
        UserInfo userInfo = (UserInfo)context.Session["CurrentUser"];
        IList netGroupList = AdminServer.TheInstance.GameServerManager.TopServerGroups;
        ServerGroup tempQualifiedServerGroup = new ServerGroup();

        foreach (ServerGroup netGroup in netGroupList)
        {
            recursiveVisitAllServers(tempQualifiedServerGroup, netGroup, "", "fullList", true);            
        }
        
        FSEyeObject tempObject = new FSEyeObject();
        tempObject.Name = "已连接的服务器";
        tempQualifiedServerGroup.SecurityObject = tempObject;

        GetServerListTable(context, tempQualifiedServerGroup, "");
        tempQualifiedServerGroup = new ServerGroup();
                        
    }
    
    //按服务器类型来返回列表(只显示已连接的)的处理函数
    private void ProcessGetGameServerListByTypeShowConnectedOnly(HttpContext context, string serverType)
    {
        UserInfo userInfo = (UserInfo)context.Session["CurrentUser"];
        IList netGroupList = AdminServer.TheInstance.GameServerManager.TopServerGroups;

        ServerGroup tempQualifiedServerGroup = new ServerGroup();

        foreach (ServerGroup netGroup in netGroupList)
        {
            recursiveVisitAllServers(tempQualifiedServerGroup, netGroup, serverType, "byType", true);            
            
            
            
            /*考虑递归之前的代码
            if (netGroup.GroupType == ServerGroup.Type.Server)
            {
                foreach (GameServer server in netGroup.List)
                {
                    if ((server.Type.ToString() == serverType) && server.IsConnected)
                    {
                        tempQualifiedServerGroup.List.Add(server);
                    }
                }
            }
            */
        }

        FSEyeObject tempObject = new FSEyeObject();
        tempObject.Name = serverType;
        tempQualifiedServerGroup.SecurityObject = tempObject;
        GetServerListTable(context, tempQualifiedServerGroup, "");                                       
    }
    
    //按服务器组来返回列表(已连接和无连接都显示)的处理函数
    private void ProcessOperationGetGameServerListByGroup(HttpContext context, string serverGroupId)
    {
        UserInfo userInfo = (UserInfo)context.Session["CurrentUser"];
        IList netGroupList = AdminServer.TheInstance.GameServerManager.TopServerGroups;

        ServerGroup tempQualifiedServerGroup = new ServerGroup();
        ServerGroup resultServerGroup = AdminServer.TheInstance.GameServerManager.GetGameServerGroup(int.Parse(serverGroupId));
        FSEyeObject tempObject = new FSEyeObject();
        tempObject.Name = resultServerGroup.Name;
        tempQualifiedServerGroup.SecurityObject = tempObject;

        foreach (ServerGroup netGroup in netGroupList)
        {
            recursiveVisitAllServers(tempQualifiedServerGroup, netGroup, serverGroupId, "byGroup", false);
        }

        GetServerListTable(context, tempQualifiedServerGroup, "");                                                
    }
    
    //按服务器组来返回列表(只显示已连接的)的处理函数
    private void ProcessOperationGetGameServerListByGroupShowConnectedOnly(HttpContext context, string serverGroupId)
    {
        UserInfo userInfo = (UserInfo)context.Session["CurrentUser"];
        IList netGroupList = AdminServer.TheInstance.GameServerManager.TopServerGroups;

        ServerGroup tempQualifiedServerGroup = new ServerGroup();
        ServerGroup resultServerGroup = AdminServer.TheInstance.GameServerManager.GetGameServerGroup(int.Parse(serverGroupId));
        FSEyeObject tempObject = new FSEyeObject();
        tempObject.Name = resultServerGroup.Name;
        tempQualifiedServerGroup.SecurityObject = tempObject;
                
        foreach (ServerGroup netGroup in netGroupList)
        {
            recursiveVisitAllServers(tempQualifiedServerGroup, netGroup, serverGroupId, "byGroup", true);                         
        }
        
        GetServerListTable(context, tempQualifiedServerGroup, "");                                                
    }
    
    //无过滤返回列表，但只返回未连接的服务器的处理函数
    private void ProcessOperationGetGameServerListShowDisconnectedOnly(HttpContext context)
    {
        UserInfo userInfo = (UserInfo)context.Session["CurrentUser"];
        IList netGroupList = AdminServer.TheInstance.GameServerManager.TopServerGroups;
        ServerGroup tempQualifiedServerGroup = new ServerGroup();

        foreach (ServerGroup netGroup in netGroupList)
        {
            recursiveVisitAllServersDisconnectedOnly(tempQualifiedServerGroup, netGroup, "", "fullList");
        }

        FSEyeObject tempObject = new FSEyeObject();
        tempObject.Name = "未连接的服务器";
        tempQualifiedServerGroup.SecurityObject = tempObject;

        GetServerListTable(context, tempQualifiedServerGroup, "");
        //tempQualifiedServerGroup = new ServerGroup();
    }

    //按服务器类型来返回列表(只显示未连接的)的处理函数
    private void ProcessOperationGetGameServerListByTypeShowDisconnectedOnly(HttpContext context, string serverType)
    {
        UserInfo userInfo = (UserInfo)context.Session["CurrentUser"];
        IList netGroupList = AdminServer.TheInstance.GameServerManager.TopServerGroups;

        ServerGroup tempQualifiedServerGroup = new ServerGroup();

        foreach (ServerGroup netGroup in netGroupList)
        {
            recursiveVisitAllServersDisconnectedOnly(tempQualifiedServerGroup, netGroup, serverType, "byType");
        }

        FSEyeObject tempObject = new FSEyeObject();
        tempObject.Name = serverType;
        tempQualifiedServerGroup.SecurityObject = tempObject;
        GetServerListTable(context, tempQualifiedServerGroup, "");
    }

    //按服务器组来返回列表(只显示未连接的)的处理函数
    private void ProcessOperationGetGameServerListByGroupShowDisconnectedOnly(HttpContext context, string serverGroupId)
    {
        UserInfo userInfo = (UserInfo)context.Session["CurrentUser"];
        IList netGroupList = AdminServer.TheInstance.GameServerManager.TopServerGroups;

        ServerGroup tempQualifiedServerGroup = new ServerGroup();
        ServerGroup resultServerGroup = AdminServer.TheInstance.GameServerManager.GetGameServerGroup(int.Parse(serverGroupId));
        FSEyeObject tempObject = new FSEyeObject();
        tempObject.Name = resultServerGroup.Name;
        tempQualifiedServerGroup.SecurityObject = tempObject;

        foreach (ServerGroup netGroup in netGroupList)
        {
            recursiveVisitAllServersDisconnectedOnly(tempQualifiedServerGroup, netGroup, serverGroupId, "byGroup");
        }

        GetServerListTable(context, tempQualifiedServerGroup, "");
    }        
    
    //++
    //my code//
    
    
    

    private void CreateServerDescribeTable(HttpContext context, ServerGroup childGroup, string strShowName)
    {
                    UserInfo userInfo = (UserInfo)context.Session["CurrentUser"];
                    using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
                    {
                        {
                            string groupName = strShowName;
                            writer.Write("<table cellspacing=1 class={0} style='width:1000;'>", CssConfig.TableNormal);
                            writer.Write("<tr><td  align='center' style='font-size:16px;color:#FF9900;weight:2000' class={0}>{1}</td>", CssConfig.TableCellNormal, groupName);
                            writer.Write("</tr>");
                            writer.Write("<tr>");
                            writer.Write("<td>");
                           // IList servers = AdminServer.TheInstance.GameServerManager.Servers;
                            IList servers = childGroup.List;


                            //对servers进行排序，以.Name属性进行排序                          
                            IComparer myComparer = new ByServerNameComparer();
                            ArrayList tempArrayList = ArrayList.Adapter(servers);
                            tempArrayList.Sort(myComparer);
                            servers = (IList)tempArrayList;
                            
                        
                            //IList checkResults = AdminServer.TheInstance.GameServerMonitor.LastCheckResults;                            
                            
                            if (servers != null && servers.Count > 0)
                            {
                                writer.Write("<table cellspacing=1 class={0} style='width:1000;'>", CssConfig.TableNormal);
                                writer.Write("<tr class={0}>", CssConfig.TableRowHeader);
                                writer.Write("<td class={0} >{1}</td>", CssConfig.TableCellHeader, StringDef.ID);

                                writer.Write("<td class={0} >{1}</td>", CssConfig.TableCellHeader, "IP");
                                
                                writer.Write("<td class={0} >{1}</td>", CssConfig.TableCellHeader, StringDef.State);
                                writer.Write("<td class={0} >{1}</td>", CssConfig.TableCellHeader, StringDef.Game);
                                writer.Write("<td class={0} >{1}</td>", CssConfig.TableCellHeader, StringDef.ServerType);
                                writer.Write("<td class={0} >{1}</td>", CssConfig.TableCellHeader, StringDef.PlayerCount);                                    
                                writer.Write("<td class={0} >{1}</td>", CssConfig.TableCellHeader, StringDef.Task);
                                writer.Write("<td class={0} >{1}</td>", CssConfig.TableCellHeader, StringDef.Alert);
                                writer.Write("<td class={0} >{1}</td>", CssConfig.TableCellHeader, StringDef.Name);
                                writer.Write("<td class={0} width='102' >{1}</td>", CssConfig.TableCellHeader, StringDef.Progress);
                                writer.Write("<td class={0} >{1}</td>", CssConfig.TableCellHeader, StringDef.Operation);
                                writer.Write("</tr>");
                                       
                                for (int i = 0; i < servers.Count; i++)
                                {
                                    GameServer server = servers[i] as GameServer;

                                    //获得这个server在AdminServer.TheInstance.GameServerManager.Servers中的index
                                    //注意，这个index与该server的预警记录在AdminServer.TheInstance.GameServerMonitor.LastCheckResults中的index匹配。
                                    //int indexOfServer = AdminServer.TheInstance.GameServerManager.Servers.IndexOf(server);
                                   
                                        //权限控制
                                    if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
                                        {
                                            //没GameServerManager上的读权限则不显示信息
                                            continue;
                                        }
                                        

                                    writer.Write("<tr>");
                                    writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, server.Id);
                                    writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, server.IpAddress);
                                    writer.Write("<td class={0} onmouseover='setNavigatingGameServer({2});showtip();' onmouseout='hidetip();setNavigatingGameServer({2});'>{1}</td>", (server.IsConnected ? CssConfig.ServerConnected : CssConfig.ServerDisconnected), (server.IsConnected ? StringDef.Connected : StringDef.Disconnected), server.Id);

                                    string cellStyle;
                                    string cellText;
                                    switch (server.GameServiceState)
                                    {
                                        case GameServer.ServiceState.OKStopped:
                                            {
                                                cellStyle = CssConfig.Unknown;
                                                cellText = StringDef.Stop;
                                            }
                                            break;
                                        case GameServer.ServiceState.Starting:
                                            {
                                                cellStyle = CssConfig.Starting;
                                                cellText = StringDef.Starting;
                                            }
                                            break;
                                        case GameServer.ServiceState.Running:
                                            {
                                                cellStyle = CssConfig.Running;
                                                cellText = StringDef.Running;
                                            }
                                            break;
                                        case GameServer.ServiceState.Stopping:
                                            {
                                                cellStyle = CssConfig.Stopping;
                                                cellText = StringDef.Stopping;
                                            }
                                            break;
                                        case GameServer.ServiceState.ErrorStopped:
                                            {
                                                cellStyle = CssConfig.Stopped;
                                                cellText = StringDef.Stopped;
                                            }
                                            break;
                                        default:
                                            {
                                                cellStyle = CssConfig.Unknown;
                                                cellText = StringDef.Stop;
                                            }
                                            break;
                                    }
                                    if (server.ConfigState == GameServer.ConfigGuardState.Configuring)
                                        cellText = "CONFIGING...";
                                    writer.Write("<td class={0} onmouseover='setNavigatingGameServerWithShorShow({1});showtip();' onmouseout='hidetip();setNavigatingGameServerWithShorShow({1});'>{2}</td>", cellStyle, server.Id, cellText);

                                    string serverTypeText = string.Empty;
                                    switch (server.Type)
                                    {
                                        case GameServer.ServerType.unidentified:
                                            serverTypeText = StringDef.ServerTypeUnidentified;
                                            break;
                                        case GameServer.ServerType.gameserver:
                                            serverTypeText = StringDef.ServerTypeGameServer;
                                            break;
                                        case GameServer.ServerType.databaseserver:
                                            serverTypeText = StringDef.ServerTypeDatabaseServer;
                                            break;
                                        case GameServer.ServerType.gamecenter:
                                            serverTypeText = StringDef.ServerTyperGameCenter;
                                            break;
                                        case GameServer.ServerType.bishop:
                                            serverTypeText = StringDef.ServerTyperBishop;
                                            break;
                                        case GameServer.ServerType.paysys:
                                            serverTypeText = StringDef.ServerTyperPaysys;
                                            break;
                                        case GameServer.ServerType.goddess:
                                            serverTypeText = StringDef.ServerTyperGoddess;
                                            break;
                                        case GameServer.ServerType.logserver:
                                            serverTypeText = StringDef.ServerTypeLogServer;
                                            break;
                                        default:
                                            serverTypeText = StringDef.ServerTypeUnidentified;
                                            break;
                                    }
                                    writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, serverTypeText);


                                    if (server.GameServiceState == GameServer.ServiceState.Running)
                                    {
                                        int playerCount = 0;
                                        try
                                        {
                                            playerCount = (int)server.GetPlugInData(userInfo.Id, LordControl.PlugInGuid, LordControl.DataKeyCurrentPlayerCount);
                                        }
                                        catch (Exception)
                                        {
                                        }
                                        writer.Write("<td class={0}><a href='PlayerCount.aspx?serverId={2}'>{1}</a></td>", CssConfig.TableCellNormal, (playerCount >= 0 ? playerCount.ToString() : StringDef.Unknown), server.Id);
                                    }
                                    else
                                    {
                                        writer.Write("<td class={0}><a href='PlayerCount.aspx?serverId={2}'>{1}</a></td>", CssConfig.TableCellNormal, StringDef.Unknown, server.Id);
                                    }
                                
                                    string guardTask = StringDef.Unknown;
                                    try
                                    {
                                        guardTask = WebUtil.GetGuardTaskName((GameServerControl.GuardTask)server.GetPlugInData(userInfo.Id, GameServerControl.PlugInGuid, GameServerControl.DataKeyGuardTask));
                                    }
                                    catch (Exception)
                                    {
                                    }
                                    writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, guardTask);

                                    writer.Write("<td class={0}>", CssConfig.TableCellNormal);

                                    //CheckResult result;
                                    //if (checkResults != null && indexOfServer < checkResults.Count && indexOfServer >= 0)
                                    //{
                                    //    result = checkResults[indexOfServer] as CheckResult;
                                    //    if (!result.NoProblem)
                                    //    {
                                    //        writer.Write("<font color=red>{0}</font>", result.ToString());
                                    //    }
                                    //}
                                    //else
                                    //{
                                    //    writer.Write(StringDef.None);
                                    //}
                                    
                                    //该用新机制-各GameServer的告警信息检测结果为GameServer的成员
                                    //这样可以避免监控页面的数组越界异常
                                    CheckResult result = server.LastCheckResult;
                                    if (result.NoProblem == false)
                                    {
                                        writer.Write("<font color=red>{0}</font>", result.ToString());
                                    }
                                    else
                                    {
                                        writer.Write(StringDef.None);
                                    }                                                                                                 
                                
                                    writer.Write("</td>");

                                    writer.Write("<td class={0}><a href='ServerDetail.aspx?serverId={1}'>{2}</a></td>", CssConfig.TableCellNormal, server.Id, server.Name);

                                  


                                    if (server.IsIdle)
                                    {
                                        writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, StringDef.None);
                                    }
                                    else
                                    {
                                        writer.Write("<td width='102' style='border: outset 1px #FFFFFF;'>");
                                        ProgressBar taskProgress = new ProgressBar();
                                        taskProgress.Progress = server.CurrentTaskProgress;
                                        taskProgress.Width = new Unit(100, UnitType.Pixel);
                                        taskProgress.Height = new Unit(15, UnitType.Pixel);
                                        taskProgress.CssClass = "ProgressBarBorder";
                                        taskProgress.ProcessedCssClass = "ProgressBarProcessed";
                                        taskProgress.IndicatorCssClass = "ProgressBarIndicator";
                                        taskProgress.CompletedCssClass = "ProgressBarCompleted";
                                        taskProgress.RenderControl(writer);
                                        writer.Write("</td>");
                                    }

                                    writer.Write("<td class={0}>", CssConfig.TableCellNormal);
                                    //if (server.GameServiceState != GameServer.ServiceState.Running)
                                        writer.Write("<a href='#' onclick='startGameServer({1});return false;'>{2}</a> ", CssConfig.LinkDefault, server.Id, StringDef.Start);
                                    //else
                                      //  writer.Write("<a href='#' onclick='startGameServer({1});return false;'>{2}</a> ", CssConfig.LinkDefault, server.Id, StringDef.Start);

                                   writer.Write("<a href='#' onclick='stopGameServer({1});return false;'>{2}</a> ", CssConfig.LinkDefault, server.Id, StringDef.Stop);
                                
                                   writer.Write("{0}"
                                        + "<a class={1} href='ServerDetail.aspx?serverId={2}'>{3}</a>"
                                        + "<a class={4} href='EditServer.aspx?op=modify&serverId={5}'>{6}</a>"
                                        + "<a class={7} href='DeleteServer.aspx?serverId={8}'>{9}</a>"
                                        + "</td>",
                                        "",
                                        CssConfig.LinkButtonSmall,
                                        server.Id,
                                        StringDef.Watch,
                                        CssConfig.LinkButtonSmall,
                                        server.Id,
                                        StringDef.Config,
                                        CssConfig.LinkButtonSmall,
                                        server.Id,
                                        StringDef.Delete,
                                        CssConfig.LinkButtonSmall
                                        );
                                
                                
                                    writer.Write("</tr>");                                    
                                }

                                writer.Write("</table>");
                            }
                            else
                            {
                                writer.Write(StringDef.NoInfo);
                            }
                            writer.Write("</td>");
                            writer.Write("</tr>");
                            writer.Write("</table>");
                           
                        }
                    }

        
        
    }
    /// <summary>
    /// 获得游戏服务器列表信息
    /// </summary>
    private void GetServerListTable(HttpContext context, ServerGroup netGroup,String groupName)
    {
        if (context == null)
            throw new ArgumentNullException("context");
        if (netGroup == null)
            return;
        
            String tempGroupName = groupName;
            UserInfo userInfo = (UserInfo)context.Session["CurrentUser"];

            if (netGroup.GroupType == ServerGroup.Type.Group)
            {
                tempGroupName += netGroup.Name + "/";

                IList childGroupList = netGroup.List;
                foreach (ServerGroup childGroup in childGroupList)
                {
                    if (childGroup.GroupType == ServerGroup.Type.Server)
                    {
                        string strName = tempGroupName;
                        strName = tempGroupName + childGroup.Name + "/";
                        CreateServerDescribeTable(context, childGroup, strName);
                    }
                    else if (childGroup.GroupType == ServerGroup.Type.Group)
                    {
                        GetServerListTable(context, childGroup, tempGroupName);
                    }

                }
            }
            else if (netGroup.GroupType == ServerGroup.Type.Server)
            {
                string strName = netGroup.Name + "/";                
                CreateServerDescribeTable(context, netGroup, strName);
            }
  
}
    
	/// <summary>
	/// 获得游戏服务器基本信息
	/// </summary>
	private void ProcessGetGameServerBasicInfo(HttpContext context)
	{
		using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
		{	
			writer.Write("<table class={0}>", CssConfig.TableSection);
			writer.Write("<tr><td class={0}>{1}</td></tr>", CssConfig.TableRowSectionTitle, StringDef.GameServerBasicInfo);
			writer.Write("<tr><td>");
			
			if (context.Request.Params[WebConfig.ParamServerId] != null)
			{
				int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
				GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

                if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
				{
					writer.Write(StringDef.NotEnoughPrivilege);
				}
				else
				{
					if (server != null)
					{	
						writer.Write("<table class={0} border='1'>", CssConfig.TableNormal);
						writer.Write("<tr><td>{0}</td><td>{1}</td></tr>", StringDef.ID, server.Id);
						writer.Write("<tr><td>{0}</td><td>{1}</td></tr>", StringDef.Name, server.Name);
						writer.Write("<tr><td>{0}</td><td>{1}</td></tr>", StringDef.Comment, server.Comment);
						writer.Write("<tr><td>{0}</td><td>{1}</td></tr>", StringDef.HostName, (server.HostName != null ? server.HostName : StringDef.Unknown));
						writer.Write("<tr><td>{0}</td><td>{1}</td></tr>", StringDef.SystemName, (server.SystemName != null ? server.SystemName : StringDef.Unknown));
						writer.Write("<tr><td>{0}</td><td>{1}</td></tr>", StringDef.State, (server.IsConnected ? StringDef.Connected : StringDef.Disconnected));
						writer.Write("</table>");
					}
				}
			}
			writer.Write("</td></tr>");
			writer.Write("</table>");
		}
	}

	/// <summary>
	/// 获得游戏服务器CPU基本信息
	/// </summary>
	private void ProcessGetGameServerCpuBasicInfo(HttpContext context)
	{
		using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
		{
			writer.Write("<table class={0}>", CssConfig.TableSection);
			writer.Write("<tr><td class={0}>{1}</td></tr>", CssConfig.TableRowSectionTitle, StringDef.CpuBasicInfo);
			writer.Write("<tr><td>");
			if (context.Request.Params[WebConfig.ParamServerId] != null)
			{
				int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
				GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

                if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
				{
					writer.Write(StringDef.NotEnoughPrivilege);
				}
				else
				{
					IList infos = server.CpuInfos;

					if (infos != null && infos.Count > 0)
					{
						writer.Write("<table cellspacing=1 class={0}>", CssConfig.TableNormal);
						
						writer.Write("<tr class={0}>", CssConfig.TableRowHeader);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.Cpu);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.Name);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.Desc);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.Manufacturer);
						writer.Write("</tr>");
						
						for (int i = 0; i < infos.Count; i++)
						{
							CpuInfo info = infos[i] as CpuInfo;                            
							writer.Write("<tr class={0}>", CssConfig.TableRowNormal);
							writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, i);
                            if (info.Name.IndexOf("烫烫") >= 0)
                            {
                                writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, StringDef.SameCPU);
                                writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal,"");
                                writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, "");
                            }
                            else
                            {
                                writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, info.Name);
                                writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, info.Desc);
                                writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, info.Vendor);
                            }
							writer.Write("</tr>");
						}
						
						writer.Write("</table>");
					}
					else
					{
						writer.Write(StringDef.NoInfo);
					}
				}
			}
			writer.Write("</td></tr>");
			writer.Write("</table>");
		}
	}

	/// <summary>
	/// 获得游戏服务器网卡基本信息
	/// </summary>
	private void ProcessGetGameServerNetworkCardInfo(HttpContext context)
	{
		using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
		{
			writer.Write("<table class={0}>", CssConfig.TableSection);
			writer.Write("<tr><td class={0}>{1}</td></tr><tr><td>", CssConfig.TableRowSectionTitle, StringDef.NetworkCardBasicInfo);
			if (context.Request.Params[WebConfig.ParamServerId] != null)
			{
				int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
				GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

                if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
				{
					writer.Write(StringDef.NotEnoughPrivilege);
				}
				else
				{
					IList infos = server.NetworkCards;
					if (infos != null && infos.Count > 0)
					{
						writer.Write("<table cellspacing=1 class={0}>", CssConfig.TableNormal);

						writer.Write("<tr class={0}>", CssConfig.TableRowHeader);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.ID);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.Name);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.Ip);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.IpMask);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.Mac);
						writer.Write("</tr>");
																		
						NetworkCardInfo info = null;
						for (int i = 0; i < infos.Count; i++)
						{
							info = infos[i] as NetworkCardInfo;

							writer.Write("<tr class={0}>", CssConfig.TableRowNormal);
							writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, i);
							writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, info.Desc);
							writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, info.Ip);
							writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, info.Mask);
							writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, info.Mac);
							writer.Write("</tr>");							
						}
						
						writer.Write("</table>");
					}
					else
					{
						writer.Write(StringDef.NoInfo);
					}
				}
			}
			writer.Write("</td></tr>");
			writer.Write("</table>");
		}
	}

	/// <summary>
	/// 获得游戏服务器文件管理任务列表
	/// </summary>
	private void ProcessGetGameServerFMTaskList(HttpContext context)
	{
		using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
		{
			writer.Write("<table class=");
			writer.Write(CssConfig.TableSection);
			writer.Write("><tr><td class=");
			writer.Write(CssConfig.TableRowSectionTitle);
			writer.Write(">");
			writer.Write(StringDef.FmTask);
			writer.Write("</td></tr><tr><td>");

			if (context.Request.Params[WebConfig.ParamServerId] != null)
			{
				int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
				GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);
				if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
				{
					writer.Write(StringDef.NotEnoughPrivilege);
				}
				else
				{
					IList tasks = server.FMTask;
					if (tasks != null && tasks.Count > 0)
					{
						writer.Write("<table class=");
						writer.Write(CssConfig.TableInsidePanelNoScroll);
						writer.Write(" border='1'><tr class=");
						writer.Write(CssConfig.TableRowHeader);
						writer.Write("><td>");
						writer.Write(StringDef.Operation);
						writer.Write("</td><td>");
						writer.Write(StringDef.Comment);
						writer.Write("</td><td>");
						writer.Write(StringDef.CompletedPercentage);
						writer.Write("</td></tr>");

						for (int taskIndex = 0; taskIndex < tasks.Count; taskIndex++)
						{
							FileManageTask task = tasks[taskIndex] as FileManageTask;

							writer.Write("<tr><td>");

							writer.Write("<div class='TextButton_Small' onclick='cancelFMTask(");
							writer.Write(taskIndex);
							writer.Write(");'>");
							writer.Write(StringDef.Cancel);
							writer.Write("</div>");

							writer.Write("</td><td>");
							writer.Write(task.ToString());
                            writer.Write("</td><td width='105'>");
                            //writer.Write((int)(task.CompletePercentage * 100));
                            //writer.Write("%</td><td width='105'>");
    
							ProgressBar taskProgress = new ProgressBar();
							taskProgress.Progress = task.CompletePercentage;
							taskProgress.Width = new Unit(100, UnitType.Pixel);
							taskProgress.Height = new Unit(15, UnitType.Pixel);
							taskProgress.CssClass = "ProgressBarBorder";
							taskProgress.ProcessedCssClass = "ProgressBarProcessed";
							taskProgress.IndicatorCssClass = "ProgressBarIndicator";
							taskProgress.CompletedCssClass = "ProgressBarCompleted";
							taskProgress.RenderControl(writer);

							writer.Write("</td></tr>");
						}
						writer.Write("</table>");
					}
					else
					{
						writer.Write(StringDef.NoFMTask);
					}
				}
			}

			writer.Write("</td></tr></table>");
		}
	}

	/// <summary>
	/// 获得游戏服务器进程列表
	/// </summary>
	private void ProcessGetGameServerProcessList(HttpContext context)
	{
		using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
		{
			writer.Write("<table class={0}><tr><td class={1}>{2}</td></tr><tr><td>",
				CssConfig.TableSection,
				CssConfig.TableRowSectionTitle,
				StringDef.ProcessList);
			if (context.Request.Params[WebConfig.ParamServerId] != null)
			{
				int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
				GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

                if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
				{
					writer.Write(StringDef.NotEnoughPrivilege);
				}
				else
				{
					IList infos = server.ProcessInfos;
					if (infos != null && infos.Count > 0)
					{
						writer.Write("<table cellspacing=1 class={0}>", CssConfig.TableNormal);
						
						writer.Write("<tr class={0}>", CssConfig.TableRowHeader);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.Pid);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.Name);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.CpuTime);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.MemUsage);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.VmSize);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.ThreadCount);
						writer.Write("</tr>");
												
						foreach (FSEye.ProcessInfo info in infos)
						{
							writer.Write("<tr class={0}>", CssConfig.TableRowNormal);
							writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, info.Pid);
							writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, info.ProcName);
							writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, info.CpuTime);
							writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, info.MemUse);
							writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, info.VmSize);
							writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellNormal, info.ThreadCount);
							writer.Write("</tr>");
						}
						
						writer.Write("</table>");
					}
					else
					{
						writer.Write(StringDef.NoInfo);
					}
				}
			}
			writer.Write("</td></tr>");
			writer.Write("</table>");
		}
	}

	/// <summary>
	/// 获得游戏服务器CPU负载
	/// </summary>
	private void ProcessGetGameServerCpuLoad(HttpContext context)
	{
		StringBuilder outputHtml = new StringBuilder();
		outputHtml.Append("<table class=");
		outputHtml.Append(CssConfig.TableSection);
		outputHtml.Append("><tr><td class=");
		outputHtml.Append(CssConfig.TableRowSectionTitle);
		outputHtml.Append(">");
		outputHtml.Append(StringDef.CpuLoad);
		outputHtml.Append("</td></tr><tr><td>");

		if (context.Request.Params[WebConfig.ParamServerId] != null)
		{
			int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
			GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

            if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
			{
				outputHtml.Append(StringDef.NotEnoughPrivilege);
			}
			else
			{
				if (server.CpuInfos != null && server.CpuInfos.Count > 0)
				{
					string imageFileName = null;

					DateTime last = DateTime.MinValue;
					if (g_lastGameServerCpuStateGraphFileTimeMap.ContainsKey(serverId))
					{
						last = (DateTime)g_lastGameServerCpuStateGraphFileTimeMap[serverId];
					}
					double escaped = ((TimeSpan)(DateTime.Now - last)).TotalMilliseconds;
					if (escaped > g_gameServerCpuStateGraphPeriod)
					{
						//DeleteGameServerCpuLoadGraph(serverId);

						imageFileName = WebUtil.CreateRandomName("GameServerCpuLoad_", WebConfig.GraphFileSuffix);
						if (CreateGameServerCpuLoadGraph(server, imageFileName))
						{
							outputHtml.Append(HtmlBuilder.CreateImage(
								WebConfig.TempGraphUrl + imageFileName,
								WebConfig.GameServerCpuLoadGraphWidth,
								WebConfig.GameServerCpuLoadGraphHeight));
						}
						else
						{
							outputHtml.Append(StringDef.CreateGraphFail);
						}
					}
					else
					{
						string fileName = g_lastGameServerCpuStateGraphFile[serverId] as string;
						if (fileName != null)
						{
							int index = fileName.LastIndexOf("\\");
							imageFileName = fileName.Substring(index + 1);
							outputHtml.Append(HtmlBuilder.CreateImage(
									WebConfig.TempGraphUrl + imageFileName,
									WebConfig.GameServerCpuLoadGraphWidth,
									WebConfig.GameServerCpuLoadGraphHeight));
						}
					}
				}
				else
				{
					outputHtml.Append(StringDef.NoInfo);
				}
			}
		}

		outputHtml.Append("</td></tr></table>");
		context.Response.Output.Write(outputHtml.ToString());
	}

	/// <summary>
	/// 创建游戏服务器CPU负载图表
	/// </summary>
	private bool CreateGameServerCpuLoadGraph(GameServer server, string imageFileName)
	{
		try
		{
			IList infos = server.CpuInfos;

			if (infos != null && infos.Count > 0)
			{
				MasterPane cpuLoadMasterPane = new MasterPane();

				GraphPane currentCpuLoadPane = new GraphPane();

				currentCpuLoadPane.Margin.Top = 120;
				currentCpuLoadPane.Margin.Bottom = 100;
				currentCpuLoadPane.Margin.Right = 50;

				currentCpuLoadPane.Fill = new Fill(WebConfig.GraphPaneBgColor);
				currentCpuLoadPane.BarSettings.ClusterScaleWidth = 1;
				currentCpuLoadPane.Chart.Fill = new Fill(WebConfig.GraphChartBgColor);
				currentCpuLoadPane.Legend.IsVisible = false;

				currentCpuLoadPane.XAxis.IsVisible = false;

				currentCpuLoadPane.YAxis.Scale.Max = 110;
				currentCpuLoadPane.YAxis.Scale.Min = -10;
				currentCpuLoadPane.YAxis.Scale.FontSpec.Size = 60;
				currentCpuLoadPane.YAxis.Scale.Format = "0'%'";
				currentCpuLoadPane.YAxis.Scale.MajorStep = 25;
				currentCpuLoadPane.YAxis.MajorGrid.IsVisible = true;
				currentCpuLoadPane.YAxis.MajorGrid.Color = WebConfig.GraphYAxisGridColor;
				currentCpuLoadPane.YAxis.MajorGrid.DashOn = 0;

				cpuLoadMasterPane.Add(currentCpuLoadPane);
				double[] value = new double[1];
				double[] pos = new double[1];
				pos[0] = 1;
				for (int i = 0; i < infos.Count; i++)
				{
					CpuInfo info = infos[i] as CpuInfo;
					value[0] += info.Loads[info.Loads.Length - 1];
				}
				value[0] /= infos.Count;
				BarItem bar = currentCpuLoadPane.AddBar("Load", null, value, Color.Blue);
				bar.Bar.Fill = new Fill(WebConfig.GraphBarFillColor);
				bar.Bar.Border = new Border(WebConfig.GraphBarBorderColor, 1);
				TextObj cpuLoadText = new TextObj(value[0] + "%", 0.64F, 0.1F, CoordType.PaneFraction);
				cpuLoadText.FontSpec.Size = 60;
				currentCpuLoadPane.GraphObjList.Add(cpuLoadText);

				GraphPane cpuStatePane = new GraphPane();
				cpuLoadMasterPane.Add(cpuStatePane);

				cpuStatePane.Title.Text = "CPU Load";
				cpuStatePane.Title.FontSpec.Size = 40;
				cpuStatePane.Fill = new Fill(WebConfig.GraphPaneBgColor);
				cpuStatePane.Chart.Fill = new Fill(WebConfig.GraphChartBgColor);

				cpuStatePane.Legend.FontSpec.Size = 40;
				cpuStatePane.Legend.FontSpec.FontColor = WebConfig.GraphLegendFontColor;
				cpuStatePane.Legend.Position = LegendPos.Right;
				cpuStatePane.Legend.Fill = new Fill(WebConfig.GraphLegendBgColor);

				cpuStatePane.XAxis.Title.IsVisible = false;
				cpuStatePane.XAxis.Scale.IsVisible = true;
				cpuStatePane.XAxis.Scale.FontSpec.Size = 40;
				cpuStatePane.XAxis.Scale.Max = 100;
				cpuStatePane.XAxis.Scale.Min = 0;
				cpuStatePane.XAxis.Scale.MajorStep = 10;
				cpuStatePane.XAxis.MajorGrid.IsVisible = true;
				cpuStatePane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;

				cpuStatePane.YAxis.Title.Text = "Load";
				cpuStatePane.YAxis.Title.FontSpec.Size = 40;
				cpuStatePane.YAxis.Scale.FontSpec.Size = 40;
				cpuStatePane.YAxis.Scale.Max = 110;
				cpuStatePane.YAxis.Scale.Min = -10;
				cpuStatePane.YAxis.Scale.MajorStep = 25;
				cpuStatePane.YAxis.Scale.Format = "0'%'";
				cpuStatePane.YAxis.MajorGrid.IsVisible = true;
				cpuStatePane.YAxis.MajorGrid.DashOff = 0;
				cpuStatePane.YAxis.MajorGrid.Color = WebConfig.GraphYAxisGridColor;

				for (int i = 0; i < infos.Count; i++)
				{
					CpuInfo info = infos[i] as CpuInfo;
					ushort[] loads = info.Loads;
					double[] counts = new double[loads.Length];
					double[] time = new double[loads.Length];

					//如果记录队列未满，曲线向右对齐
					if (loads.Length < CpuInfo.LoadQueueMaxLength)
					{
						int offset = CpuInfo.LoadQueueMaxLength - loads.Length;
						for (int j = 0; j < loads.Length; j++)
						{
							time[j] = j + offset;
							counts[j] = (int)loads[j];
						}
					}
					else
					{
						for (int j = 0; j < loads.Length; j++)
						{
							time[j] = j;
							counts[j] = (int)loads[j];
						}
					}
					Color color = Color.Yellow;
					if (i < WebConfig.GraphColors.Length)
					{
						color = WebConfig.GraphColors[i];
					}
					cpuStatePane.AddCurve("CPU_" + i, time, counts, color, SymbolType.None);
				}

				Bitmap bitmap = new Bitmap(1, 1);
				using (Graphics g = Graphics.FromImage(bitmap))
				{
					cpuLoadMasterPane.AxisChange(g);
					cpuLoadMasterPane.SetLayout(g, false, new int[] { 1, 1 }, new float[] { 1, 6 });
				}

				bitmap = cpuLoadMasterPane.GetImage(WebConfig.GameServerCpuLoadGraphWidth, WebConfig.GameServerCpuLoadGraphHeight, 75.0f);
				string file = WebConfig.WebsiteRootPath + WebConfig.TempGraphPath + imageFileName;				
				bitmap.Save(file, WebConfig.GraphImageFormat);
				g_lastGameServerCpuStateGraphFile[server.Id] = file;
				g_lastGameServerCpuStateGraphFileTimeMap[server.Id] = DateTime.Now;
				TempFileManager.TheInstance.AddTempFile(file, g_gameServerCpuStateGraphPeriod * WebConfig.TempGraphDeleteDelayMultiple);

				return true;
			}

			return false;
		}
		catch (Exception)
		{
			return false;
		}
	}

	/// <summary>
	/// 删除游戏服务器CPU负载图表
	/// </summary>
	private void DeleteGameServerCpuLoadGraph(int serverId)
	{
		if (g_lastGameServerCpuStateGraphFile.ContainsKey(serverId))
		{
			FileInfo info = new FileInfo(g_lastGameServerCpuStateGraphFile[serverId] as string);
			if (info.Exists)
			{
				info.Delete();
			}
		}
	}

	/// <summary>
	/// 获得游戏服务器磁盘使用量
	/// </summary>
	private void ProcessGetGameServerDiskUsage(HttpContext context)
	{
		StringBuilder outputHtml = new StringBuilder();
		outputHtml.Append("<table class=");
		outputHtml.Append(CssConfig.TableSection);
		outputHtml.Append("><tr><td class=");
		outputHtml.Append(CssConfig.TableRowSectionTitle);
		outputHtml.Append(">");
		outputHtml.Append(StringDef.DiskUsage);
		outputHtml.Append("</td></tr><tr><td>");

		if (context.Request.Params[WebConfig.ParamServerId] != null)
		{
			int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
			GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

            if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
			{
				outputHtml.Append(StringDef.NotEnoughPrivilege);
			}
			else
			{
				if (server.DiskInfos != null && server.DiskInfos.Count > 0)
				{
					string imageFileName = null;

					DateTime last = DateTime.MinValue;
					if (g_lastGameServerDiskInfoGraphFileTimeMap.ContainsKey(serverId))
					{
						last = (DateTime)g_lastGameServerDiskInfoGraphFileTimeMap[serverId];
					}
					double escaped = ((TimeSpan)(DateTime.Now - last)).TotalMilliseconds;
					if (escaped > g_gameServerDiskInfoGraphPeriod)
					{
						//DeleteGameServerDiskUsageGraph(serverId);

						imageFileName = WebUtil.CreateRandomName("GameServerDiskUsage_", WebConfig.GraphFileSuffix);
						if (CreateGameServerDiskUsageGraph(server, imageFileName))
						{
							outputHtml.Append(HtmlBuilder.CreateImage(
								WebConfig.TempGraphUrl + imageFileName,
								WebConfig.GameServerDiskInfoGraphWidth * server.DiskInfos.Count,
								WebConfig.GameServerDiskInfoGraphHeight));
						}
						else
						{
							outputHtml.Append(StringDef.CreateGraphFail);
						}
					}
					else
					{
						string fileName = g_lastGameServerDiskInfoGraphFile[serverId] as string;
						if (fileName != null)
						{
							int index = fileName.LastIndexOf("\\");
							imageFileName = fileName.Substring(index + 1);
							outputHtml.Append(HtmlBuilder.CreateImage(
									WebConfig.TempGraphUrl + imageFileName,
									WebConfig.GameServerDiskInfoGraphWidth * server.DiskInfos.Count,
									WebConfig.GameServerDiskInfoGraphHeight));
						}
					}
				}
				else
				{
					outputHtml.Append(StringDef.NoInfo);
				}
			}	
		}

		outputHtml.Append("</td></tr></table>");
		context.Response.Output.Write(outputHtml.ToString());
	}

	/// <summary>
	/// 创建游戏服务器磁盘使用量图表
	/// </summary>
	private bool CreateGameServerDiskUsageGraph(GameServer server, string imageFileName)
	{
		try
		{
			IList disks = server.DiskInfos;
			if (disks != null && disks.Count > 0)
			{
				MasterPane allDiskPane = new MasterPane(string.Empty, new RectangleF(0, 0, WebConfig.GameServerDiskInfoGraphWidth * disks.Count, WebConfig.GameServerDiskInfoGraphHeight));

				for (int i = 0; i < disks.Count; i++)
				{
					DiskInfo diskInfo = disks[i] as DiskInfo;
					GraphPane diskPane = new GraphPane();
					diskPane.Title.Text = diskInfo.Name;
					diskPane.Title.FontSpec.Size = 70;
					diskPane.Title.IsVisible = true;
					diskPane.Legend.IsVisible = false;

					float free = ((float)diskInfo.FreeSpace) / 1024;
					float used = ((float)diskInfo.TotalSpace - diskInfo.FreeSpace) / 1024;
					PieItem item = diskPane.AddPieSlice(free, WebConfig.GraphDiskUsageFreeColor, 0, string.Empty);
					item.LabelType = PieLabelType.None;
					item = diskPane.AddPieSlice(used, WebConfig.GraphDiskUsageUsedColor, 0, string.Empty);
					item.LabelType = PieLabelType.None;
					TextObj diskFreeText = new TextObj("Free: " + free + " G", 0.0F, 0.2F, CoordType.PaneFraction);
					TextObj diskTotalText = new TextObj("Total: " + (free + used) + " G", 0.99F, 0.99F, CoordType.PaneFraction);
					diskFreeText.Location.AlignH = AlignH.Left;
					diskFreeText.Location.AlignV = AlignV.Top;
					diskTotalText.Location.AlignH = AlignH.Right;
					diskTotalText.Location.AlignV = AlignV.Bottom;

					diskFreeText.FontSpec.Size = 60;
					diskFreeText.FontSpec.FontColor = Color.White;
					diskFreeText.FontSpec.Fill = new Fill(WebConfig.GraphDiskUsageFreeColor);
					diskTotalText.FontSpec.Size = 60;
					diskTotalText.FontSpec.FontColor = Color.Black;
					diskTotalText.FontSpec.Fill = new Fill(WebConfig.GraphDiskUsageTotalColor);
					diskPane.GraphObjList.Add(diskFreeText);
					diskPane.GraphObjList.Add(diskTotalText);

					allDiskPane.Add(diskPane);
				}

				Bitmap bitmap = new Bitmap(1, 1);
				using (Graphics g = Graphics.FromImage(bitmap))
				{
					allDiskPane.AxisChange(g);
					allDiskPane.SetLayout(g, PaneLayout.SingleRow);
				}

				bitmap = allDiskPane.GetImage();
				string file = WebConfig.WebsiteRootPath + WebConfig.TempGraphPath + imageFileName;				
				bitmap.Save(file, WebConfig.GraphImageFormat);
				g_lastGameServerDiskInfoGraphFile[server.Id] = file;
				g_lastGameServerDiskInfoGraphFileTimeMap[server.Id] = DateTime.Now;
				TempFileManager.TheInstance.AddTempFile(file, g_gameServerDiskInfoGraphPeriod * WebConfig.TempGraphDeleteDelayMultiple);
				
				return true;
			}
			else
			{
				return false;
			}
		}
		catch (Exception)
		{
			return false;
		}
	}

	/// <summary>
	/// 删除游戏服务器磁盘是使用量图表
	/// </summary>
	private void DeleteGameServerDiskUsageGraph(int serverId)
	{
		if (g_lastGameServerDiskInfoGraphFile.ContainsKey(serverId))
		{
			FileInfo info = new FileInfo(g_lastGameServerDiskInfoGraphFile[serverId] as string);
			if (info.Exists)
			{
				info.Delete();
			}
		}
	}

	/// <summary>
	/// 获得游戏服务器内存使用量
	/// </summary>
	private void ProcessGetGameServerMemoryUsage(HttpContext context)
	{
		StringBuilder outputHtml = new StringBuilder();
		outputHtml.Append("<table class=");
		outputHtml.Append(CssConfig.TableSection);
		outputHtml.Append("><tr><td class=");
		outputHtml.Append(CssConfig.TableRowSectionTitle);
		outputHtml.Append(">");
		outputHtml.Append(StringDef.MemUsage);
		outputHtml.Append("</td></tr><tr><td>");

		if (context.Request.Params[WebConfig.ParamServerId] != null)
		{
			int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
			GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

            if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
			{
				outputHtml.Append(StringDef.NotEnoughPrivilege);
			}
			else
			{
				if (server.MemoryInfos != null && server.MemoryInfos.Length > 0)
				{
					string imageFileName = null;
					DateTime last = DateTime.MinValue;
					if (g_lastGameServerMemoryUsageGraphFileTimeMap.ContainsKey(serverId))
					{
						last = (DateTime)g_lastGameServerMemoryUsageGraphFileTimeMap[serverId];
					}
					double escaped = ((TimeSpan)(DateTime.Now - last)).TotalMilliseconds;
					if (escaped > g_gameServerMemoryUsageGraphPeriod)
					{
						//DeleteGameServerMemoryUsageGraph(serverId);

						imageFileName = WebUtil.CreateRandomName("GameServerMemoryUsage_", WebConfig.GraphFileSuffix);
						if (CreateGameServerMemoryUsageGraph(server, imageFileName))
						{
							outputHtml.Append(HtmlBuilder.CreateImage(WebConfig.TempGraphUrl + imageFileName, WebConfig.GameServerMemoryUsageGraphWidth, WebConfig.GameServerMemoryUsageGraphHeight));
						}
						else
						{
							outputHtml.Append(StringDef.CreateGraphFail);
						}
					}
					else
					{
						string fileName = g_lastGameServerMemoryUsageGraphFile[serverId] as string;
						if (fileName != null)
						{
							int index = fileName.LastIndexOf("\\");
							imageFileName = fileName.Substring(index + 1);
							outputHtml.Append(HtmlBuilder.CreateImage(
								WebConfig.TempGraphUrl + imageFileName,
								WebConfig.GameServerMemoryUsageGraphWidth,
								WebConfig.GameServerMemoryUsageGraphHeight));
						}
					}
				}
				else
				{
					outputHtml.Append(StringDef.NoInfo);
				}
			}
		}

		outputHtml.Append("</td></tr></table>");
		context.Response.Output.Write(outputHtml.ToString());
	}

	/// <summary>
	/// 创建游戏服务器内存使用量图表
	/// </summary>
	private bool CreateGameServerMemoryUsageGraph(GameServer server, string imageFileName)
	{
		MemoryInfo[] infos = server.MemoryInfos;
		if (infos != null)
		{
			MemoryInfo currentInfo = infos[infos.Length - 1];

			MasterPane memUsageMasterPane = new MasterPane();

			GraphPane currentMemUsagePane = new GraphPane();

			currentMemUsagePane.Margin.Top = 120;
			currentMemUsagePane.Margin.Bottom = 110;
			currentMemUsagePane.Margin.Right = 50;
			currentMemUsagePane.Margin.Left = 80;
			currentMemUsagePane.Fill = new Fill(WebConfig.GraphPaneBgColor);
			currentMemUsagePane.Chart.Fill = new Fill(WebConfig.GraphChartBgColor);
			currentMemUsagePane.Legend.IsVisible = false;
			currentMemUsagePane.BarSettings.ClusterScaleWidth = 1;

			currentMemUsagePane.XAxis.IsVisible = false;

			currentMemUsagePane.YAxis.Title.IsVisible = false;
			currentMemUsagePane.YAxis.Title.FontSpec.Size = 50;
			currentMemUsagePane.YAxis.IsVisible = true;
			currentMemUsagePane.YAxis.Scale.Max = currentInfo.TotalSize * 1.1;
			currentMemUsagePane.YAxis.Scale.Min = 0;
			currentMemUsagePane.YAxis.Scale.FontSpec.Size = 50;
			currentMemUsagePane.YAxis.MajorGrid.IsVisible = true;
			currentMemUsagePane.YAxis.MajorGrid.Color = WebConfig.GraphYAxisGridColor;
			currentMemUsagePane.YAxis.MajorGrid.DashOn = 0;

			memUsageMasterPane.Add(currentMemUsagePane);

			double[] value = new double[1];
			double[] pos = new double[1];

			pos[0] = 1;
			value[0] = currentInfo.UsedSize;
			BarItem bar = currentMemUsagePane.AddBar("Used Mem", null, value, Color.Blue);
			bar.Bar.Fill = new Fill(WebConfig.GraphBarFillColor);
			bar.Bar.Border = new Border(WebConfig.GraphBarBorderColor, 1);
			TextObj memUsedText = new TextObj((int)(value[0] / 1024) + "/" + (int)(currentInfo.TotalSize / 1024) + "M", 0.64F, 0.1F, CoordType.PaneFraction);
			memUsedText.FontSpec.Size = 60;
			currentMemUsagePane.GraphObjList.Add(memUsedText);

			GraphPane memUsagePane = new GraphPane();
			memUsageMasterPane.Add(memUsagePane);

			memUsagePane.Title.Text = "Memory Usage";
			memUsagePane.Title.FontSpec.Size = 40;
			memUsagePane.Fill = new Fill(WebConfig.GraphPaneBgColor);
			memUsagePane.Chart.Fill = new Fill(WebConfig.GraphChartBgColor);

			memUsagePane.Legend.FontSpec.Size = 40;
			memUsagePane.Legend.Position = LegendPos.Right;
			memUsagePane.Legend.Fill = new Fill(WebConfig.GraphLegendBgColor);
			memUsagePane.Legend.FontSpec.FontColor = WebConfig.GraphLegendFontColor;

			memUsagePane.XAxis.Title.IsVisible = false;
			memUsagePane.XAxis.Scale.IsVisible = true;
			memUsagePane.XAxis.Scale.FontSpec.Size = 40;
			memUsagePane.XAxis.Scale.Max = GameServer.MAX_MEMORY_INFO_HISTORY_RECORD_COUNT;
			memUsagePane.XAxis.Scale.Min = 0;
			memUsagePane.XAxis.Scale.MajorStep = 10;
			memUsagePane.XAxis.MajorGrid.IsVisible = true;
			memUsagePane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;

			memUsagePane.YAxis.Title.Text = "Usage";
			memUsagePane.YAxis.Title.FontSpec.Size = 40;
			memUsagePane.YAxis.Scale.FontSpec.Size = 40;
			memUsagePane.YAxis.Scale.Min = 0;
			memUsagePane.YAxis.Scale.Max = currentInfo.TotalSize * 1.1;
			memUsagePane.YAxis.MajorGrid.IsVisible = true;
			memUsagePane.YAxis.MajorGrid.DashOff = 0;
			memUsagePane.YAxis.MajorGrid.Color = WebConfig.GraphYAxisGridColor;

			double[] usedSizes = new double[infos.Length];
			double[] time = new double[infos.Length];

			int offset = GameServer.MAX_MEMORY_INFO_HISTORY_RECORD_COUNT - infos.Length;//如果记录队列未满，曲线向右对齐，这个值就是向右的偏移量
			for (int i = 0; i < infos.Length; i++)
			{
				MemoryInfo info = infos[i];
				time[i] = i + offset;
				usedSizes[i] = info.TotalSize - info.FreeSize;
			}
			memUsagePane.AddCurve("Used", time, usedSizes, Color.Yellow, SymbolType.None);//TODO

			Bitmap bitmap = new Bitmap(1, 1);
			using (Graphics g = Graphics.FromImage(bitmap))
			{
				memUsageMasterPane.AxisChange(g);
				memUsageMasterPane.SetLayout(g, false, new int[] { 1, 1 }, new float[] { 1, 6 });
			}

			bitmap = memUsageMasterPane.GetImage(WebConfig.GameServerMemoryUsageGraphWidth, WebConfig.GameServerMemoryUsageGraphHeight, 75.0f);
			string file = WebConfig.WebsiteRootPath + WebConfig.TempGraphPath + imageFileName;			

			try
			{
				bitmap.Save(file, WebConfig.GraphImageFormat);
				g_lastGameServerMemoryUsageGraphFile[server.Id] = file;
				g_lastGameServerMemoryUsageGraphFileTimeMap[server.Id] = DateTime.Now;
				TempFileManager.TheInstance.AddTempFile(file, g_gameServerMemoryUsageGraphPeriod * WebConfig.TempGraphDeleteDelayMultiple);
				
				return true;
			}
			catch (Exception)
			{
				//TODO 记录错误
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	/// <summary>
	/// 删除游戏服务器内存使用量图表
	/// </summary>
	private void DeleteGameServerMemoryUsageGraph(int serverId)
	{
		if (g_lastGameServerMemoryUsageGraphFile.ContainsKey(serverId))
		{
			FileInfo info = new FileInfo(g_lastGameServerMemoryUsageGraphFile[serverId] as string);
			if (info.Exists)
			{
				info.Delete();
			}
		}
	}

	/// <summary>
	/// 获得游戏服务器网络负载
	/// </summary>
	private void ProcessGetGameServerNetworkLoad(HttpContext context)
	{
		StringBuilder outputHtml = new StringBuilder();
		outputHtml.Append("<table class=");
		outputHtml.Append(CssConfig.TableSection);
		outputHtml.Append("><tr><td class=");
		outputHtml.Append(CssConfig.TableRowSectionTitle);
		outputHtml.Append(">");
		outputHtml.Append(StringDef.NetworkLoad);
		outputHtml.Append("</td></tr><tr><td>");

		if (context.Request.Params[WebConfig.ParamServerId] != null)
		{
			int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
			GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

            if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
			{
				outputHtml.Append(StringDef.NotEnoughPrivilege);
			}
			else
			{
				if (server.NetworkCards != null && server.NetworkCards.Count > 0)
				{
					string imageFileName = null;
					DateTime last = DateTime.MinValue;
					if (g_lastGameServerNetworkLoadGraphFileTimeMap.ContainsKey(serverId))
					{
						last = (DateTime)g_lastGameServerNetworkLoadGraphFileTimeMap[serverId];
					}
					double escaped = ((TimeSpan)(DateTime.Now - last)).TotalMilliseconds;
					if (escaped > g_gameServerNetworkLoadGraphPeriod)
					{
						//DeleteGameServerNetworkLoadGraph(serverId);
						
						imageFileName = WebUtil.CreateRandomName("GameServerNetworkLoad_", WebConfig.GraphFileSuffix);
						if (CreateGameServerNetworkLoadGraph(server, imageFileName))
						{
							outputHtml.Append(HtmlBuilder.CreateImage(
								WebConfig.TempGraphUrl + imageFileName,
								WebConfig.GameServerNetworkLoadGraphWidth,
								WebConfig.GameServerNetworkLoadGraphHeight * server.NetworkCards.Count));
						}
						else
						{
							outputHtml.Append(StringDef.CreateGraphFail);
						}
					}
					else
					{
						string fileName = g_lastGameServerNetworkLoadGraphFile[serverId] as string;
						if (fileName != null)
						{
							int index = fileName.LastIndexOf("\\");
							imageFileName = fileName.Substring(index + 1);
							outputHtml.Append(HtmlBuilder.CreateImage(
								WebConfig.TempGraphUrl + imageFileName,
								WebConfig.GameServerNetworkLoadGraphWidth,
								WebConfig.GameServerNetworkLoadGraphHeight * server.NetworkCards.Count));
						}
					}
				}
				else
				{
					outputHtml.Append(StringDef.NoInfo);
				}
			}
		}

		outputHtml.Append("</td></tr></table>");
		context.Response.Output.Write(outputHtml.ToString());
	}

	/// <summary>
	/// 创建游戏服务器网络负载图表
	/// </summary>
	private bool CreateGameServerNetworkLoadGraph(GameServer server, string imageFileName)
	{
		IList cards = server.NetworkCards;
		if (cards != null && cards.Count > 0)
		{
			MasterPane networkGraphMasterPane = new MasterPane();

			for (int i = 0; i < cards.Count; i++)
			{
				NetworkCardInfo card = cards[i] as NetworkCardInfo;

				GraphPane networkGraphPane = new GraphPane();

				networkGraphPane.Title.Text = "Network Card " + i + " [" + card.Ip + "]";
				networkGraphPane.Title.FontSpec.Size = 40;
				networkGraphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);

				networkGraphPane.Chart.Fill = new Fill(WebConfig.GraphChartBgColor);

				networkGraphPane.Legend.FontSpec.Size = 40;
				networkGraphPane.Legend.Position = LegendPos.Right;
				networkGraphPane.Legend.Fill = new Fill(WebConfig.GraphLegendBgColor);
				networkGraphPane.Legend.FontSpec.FontColor = WebConfig.GraphLegendFontColor;

				networkGraphPane.XAxis.Title.IsVisible = false;
				networkGraphPane.XAxis.Scale.IsVisible = true;
				networkGraphPane.XAxis.Scale.FontSpec.Size = 40;
				networkGraphPane.XAxis.Scale.Max = NetworkCardInfo.LoadQueueMaxLength;
				networkGraphPane.XAxis.Scale.Min = 0;
				networkGraphPane.XAxis.Scale.MajorStep = 10;
				networkGraphPane.XAxis.MajorGrid.IsVisible = true;
				networkGraphPane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;

				networkGraphPane.YAxis.MajorGrid.IsVisible = true;
				networkGraphPane.YAxis.MajorGrid.DashOff = 0;
				networkGraphPane.YAxis.MajorGrid.Color = WebConfig.GraphYAxisGridColor;
				networkGraphPane.YAxis.Title.Text = "Load";
				networkGraphPane.YAxis.Title.FontSpec.Size = 40;
				networkGraphPane.YAxis.Scale.FontSpec.Size = 40;

				netcardload[] loads = card.Loads;
				double[] rxs = new double[loads.Length];
				double[] txs = new double[loads.Length];
				double[] times = new double[loads.Length];

				int offset = NetworkCardInfo.LoadQueueMaxLength - loads.Length;
				for (int j = 0; j < loads.Length; j++)
				{
					rxs[j] = loads[j].nRXRate;
					txs[j] = loads[j].nTXRate;
					times[j] = j + offset;
				}

				networkGraphPane.AddCurve("RX", times, rxs, WebConfig.GraphColors[0], SymbolType.None);
				networkGraphPane.AddCurve("TX", times, txs, WebConfig.GraphColors[1], SymbolType.None);

				if (rxs.Length > 0)
				{
					TextObj currentRxText = new TextObj("RX: " + (uint)(rxs[rxs.Length - 1] / 1024) + "K", 0.92f, 0.55f, CoordType.PaneFraction);
					currentRxText.FontSpec.Size = 40;
					currentRxText.Location.AlignH = AlignH.Left;
					currentRxText.Location.AlignV = AlignV.Top;
					networkGraphPane.GraphObjList.Add(currentRxText);

					TextObj currentTxText = new TextObj("TX: " + (uint)(txs[txs.Length - 1] / 1024) + "K", 0.92f, 0.75f, CoordType.PaneFraction);
					currentTxText.FontSpec.Size = 40;
					currentTxText.Location.AlignH = AlignH.Left;
					currentTxText.Location.AlignV = AlignV.Top;
					networkGraphPane.GraphObjList.Add(currentTxText);
				}

				networkGraphMasterPane.Add(networkGraphPane);
			}

			Bitmap bitmap = new Bitmap(1, 1);
			using (Graphics g = Graphics.FromImage(bitmap))
			{
				networkGraphMasterPane.AxisChange(g);
				networkGraphMasterPane.SetLayout(g, PaneLayout.SingleColumn);
			}

			bitmap = networkGraphMasterPane.GetImage(WebConfig.GameServerNetworkLoadGraphWidth, WebConfig.GameServerNetworkLoadGraphHeight * cards.Count, 75.0f);
			string file = WebConfig.WebsiteRootPath + WebConfig.TempGraphPath + imageFileName;
			try
			{
				bitmap.Save(file, WebConfig.GraphImageFormat);
				g_lastGameServerNetworkLoadGraphFile[server.Id] = file;
				g_lastGameServerNetworkLoadGraphFileTimeMap[server.Id] = DateTime.Now;
				TempFileManager.TheInstance.AddTempFile(file, g_gameServerNetworkLoadGraphPeriod * WebConfig.TempGraphDeleteDelayMultiple);
				
				return true;
			}
			catch (Exception)
			{
				//TODO 记录错误
				return false;
			}
		}

		return false;
	}

	/// <summary>
	/// 删除游戏服务器网络负载图表
	/// </summary>
	private void DeleteGameServerNetworkLoadGraph(int serverId)
	{
		if (g_lastGameServerNetworkLoadGraphFile.ContainsKey(serverId))
		{
			FileInfo info = new FileInfo(g_lastGameServerNetworkLoadGraphFile[serverId] as string);
			if (info.Exists)
			{
				info.Delete();
			}
		}
	}

	/// <summary>
	/// 获得游戏服务器描述
	/// </summary>
	private void ProcessGetGameServerDesc(HttpContext context)
	{
		if (context.Request.Params[WebConfig.ParamServerId] != null)
		{
			StringBuilder ouputHtml = new StringBuilder();
			int serverId = 0;
			try
			{
				serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
			}
			catch (Exception)
			{
			}

			if (serverId > 0)
			{
				GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);
				if (server != null)
				{
					ouputHtml.Append(HtmlBuilder.CreateHidden("CanDelete", "true"));
					ouputHtml.Append(StringDef.Name);
					ouputHtml.Append(": ");
					ouputHtml.Append(server.Name);
					ouputHtml.Append(", ");
					ouputHtml.Append(StringDef.Comment);
					ouputHtml.Append(": ");
					ouputHtml.Append(server.Comment);
				}
				else
				{
					ouputHtml.Append(HtmlBuilder.CreateHidden("CanDelete", "false"));
					ouputHtml.Append("<font color='#FF0000'>指定编号的游戏服务器不存在</font>");
				}
			}
			else
			{
				ouputHtml.Append(HtmlBuilder.CreateHidden("CanDelete", "false"));
				ouputHtml.Append("<font color='#FF0000'>游戏服务器编号格式不正确</font>");
			}

			context.Response.Output.Write(ouputHtml.ToString());
		}
	}
	
	private void ProcessGetGameServerPlugInInfo(HttpContext context)
	{
		using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
		{
			writer.Write("<table class={0}>", CssConfig.TableSection);
			writer.Write("<tr><td class={0}>{1}</td></tr>", CssConfig.TableRowSectionTitle, StringDef.PlugIn);			
			writer.Write("<tr><td>");

			if (context.Request.Params[WebConfig.ParamServerId] != null)
			{
				int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
				GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

                if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
				{
					writer.Write(StringDef.NotEnoughPrivilege);
				}
				else
				{
					IList infos = server.PlugInInfos;
					if (infos != null && infos.Count > 0)
					{
						writer.Write("<table cellspacing=1 class={0}>", CssConfig.TableNormal);

						writer.Write("<tr class={0}>", CssConfig.TableRowHeader);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.Index);
						writer.Write("<td class={0}>{1}</td>", CssConfig.TableCellHeader, StringDef.PlugIn);
						writer.Write("</tr>");						
						
						for (int i = 0; i < infos.Count; i++)
						{
							PlugInInfo info = infos[i] as PlugInInfo;

							writer.Write("<tr><td class={0}>{1}</td><td>", CssConfig.TableCellNormal, i);

							writer.Write("<table width='100%' cellspacing=1 class={0}>", CssConfig.TableNormal);
							writer.Write("<tr><td class={0} width=20%>{1}</td><td class={2}>{3}</td></tr>", CssConfig.TableCellNormal, StringDef.Desc, CssConfig.TableCellNormal, info.Desc);
							writer.Write("<tr><td class={0}>{1}</td><td class={2}>{3}</td></tr>", CssConfig.TableCellNormal, StringDef.Author, CssConfig.TableCellNormal, info.Author);
							writer.Write("<tr><td class={0}>{1}</td><td class={2}>{3}</td></tr>", CssConfig.TableCellNormal, StringDef.Version, CssConfig.TableCellNormal, info.Version);
							writer.Write("<tr><td class={0}>{1}</td><td class={2}>{3}</td></tr>", CssConfig.TableCellNormal, StringDef.Guid, CssConfig.TableCellNormal, info.Guid);
							writer.Write("<tr><td class={0}>{1}</td><td class={2}>{3}</td></tr>", CssConfig.TableCellNormal, StringDef.Path, CssConfig.TableCellNormal, info.Path);
							writer.Write("<tr><td class={0}>{1}</td><td class={2}>{3}</td></tr>", CssConfig.TableCellNormal, StringDef.ModuleInfo, CssConfig.TableCellNormal, info.ModuleInfo);
							writer.Write("</table>");

							writer.Write("</td></tr>");
						}

						writer.Write("</table>");
					}
					else
					{
						writer.Write(StringDef.NoInfo);
					}
				}
			}

			writer.Write("</td></tr>");
			writer.Write("</table>");
		}
	}

	private void ProcessGetGameServerSummary(HttpContext context)
	{
		using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
		{
			if (context.Request.Params[WebConfig.ParamServerId] != null)
			{
				int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
				GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

				if (server != null)
				{
                    //加入权限检查
                    if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
                    {
                        writer.Write(StringDef.NotEnoughPrivilege);
                        return;
                    }
					//else
					//{
						if (server != null)
						{
							writer.Write("<table width='100%'>");

							writer.Write("<tr><td style='font-weight:bold; text-align: center; color: Green;'>{0}</td></tr>", server.Name);
                            writer.Write("<tr><td style='text-align: center; color: White;'>{0}</td></tr>", server.Comment);
							writer.Write("<tr><td style='text-align: center; color: White;'>[ IP:{0} ]</td></tr>", server.IpAddress);

							GameServer.MessageInfo[] msgArray = server.RecentMessages;
							if (msgArray.Length > 0)
							{
								writer.Write("<tr class=TableRowSpacing />");
								for (int i = 0; i < msgArray.Length; i++)
								{
									string color = "White";
									switch (msgArray[i].MessageType)
									{
										case GameServer.MessageInfo.Type.Normal:
											color = "#FFFFFF";
											break;
										case GameServer.MessageInfo.Type.Success:
											color = "#00FF00";
											break;
										case GameServer.MessageInfo.Type.Failure:
											color = "#FF0000";
											break;
										case GameServer.MessageInfo.Type.Alert:
											color = "#FFFF00";
											break;
									}
									writer.Write("<tr><td style='color: {0};'>{1}</td></tr>", color, msgArray[i].ToString());
								}
							}

							writer.Write("<tr class=TableRowSpacing />");

							if (server.IsConnected)
							{
								writer.Write("<tr><td>");
								writer.Write("<table>");
                                
                                //sunyong

                                bool bOnlyShowShortTip = false;
                                bOnlyShowShortTip = context.Request.Params["short"] == "TRUE";
                                if (!bOnlyShowShortTip)
                                {

                                    TimeSpan gameRunningTime = TimeSpan.MinValue;
                                    try
                                    {
                                        gameRunningTime = (TimeSpan)server.GetPlugInData(((UserInfo)context.Session["CurrentUser"]).Id, LordControl.PlugInGuid, LordControl.DataKeyGameRunningTime);
                                    }
                                    catch (Exception)
                                    {
                                    }
                                    if (gameRunningTime != TimeSpan.MinValue)
                                    {
                                        int hour = (int)gameRunningTime.TotalHours;
                                        int minute = (int)gameRunningTime.Minutes;
                                        int second = (int)gameRunningTime.Seconds;
                                        writer.Write("<tr><td>{0}:</td><td>{1}:{2:d2}:{3:d2}</td></tr>", StringDef.GameRunningTime, hour, minute, second);
                                    }
                                    else
                                    {
                                        writer.Write("<tr><td>{0}:</td><td>{1}</td></tr>", StringDef.GameRunningTime, StringDef.Unknown);
                                    }

                                    DateTime serverTime = DateTime.MinValue;
                                    try
                                    {
                                        serverTime = (DateTime)server.GetPlugInData(((UserInfo)context.Session["CurrentUser"]).Id, GameServerControl.PlugInGuid, GameServerControl.DataKeyGetServerTime);
                                    }
                                    catch (Exception)
                                    {
                                    }
                                    if (serverTime != DateTime.MinValue)
                                    {
                                        writer.Write("<tr><td>{0}:</td><td>{1}:{2:d2}:{3:d2}</td></tr>", StringDef.Server + StringDef.Time, serverTime.Hour, serverTime.Minute, serverTime.Second);
                                    }
                                    else
                                    {
                                        writer.Write("<tr><td>{0}:</td><td>{1}</td></tr>", StringDef.Server + StringDef.Time, StringDef.Unknown);
                                    }

                                    IList infos = server.CpuInfos;
                                    for (int i = 0; i < infos.Count; i++)
                                    {
                                        int load = -1;
                                        CpuInfo info = infos[i] as CpuInfo;
                                        if (info.Loads.Length > 0)
                                        {
                                            load = info.Loads[info.Loads.Length - 1];
                                        }

                                        if (load >= 0)
                                        {
                                            writer.Write("<tr><td>{0}({1}):</td><td>{2} %</td></tr>", StringDef.CpuLoad, i, load);
                                        }
                                        else
                                        {
                                            writer.Write("<tr><td>{0}({1}):</td><td>{2}</td></tr>", StringDef.CpuLoad, i, StringDef.Unknown);
                                        }
                                    }

                                    //int cpuLoad = server.CurrentCpuLoad;
                                    //if (cpuLoad >= 0)
                                    //{
                                    //    writer.Write("<tr><td>{0}:</td><td>{1} %</td></tr>", StringDef.CpuLoad, server.CurrentCpuLoad);
                                    //}
                                    //else
                                    //{
                                    //    writer.Write("<tr><td>{0}:</td><td>{1}</td></tr>", StringDef.CpuLoad, StringDef.Unknown);
                                    //}

                                    writer.Write("<tr><td>");
                                    writer.Write(StringDef.MemUsage);
                                    writer.Write(":</td><td>");
                                    MemoryInfo memInfo = server.CurrentMemInfo;
                                    if (memInfo != null)
                                    {
                                        writer.Write(((float)memInfo.UsedSize) / 1024);
                                        writer.Write("/");
                                        writer.Write(((float)memInfo.TotalSize) / 1024);
                                        writer.Write(" M");
                                    }
                                    else
                                    {
                                        writer.Write(StringDef.Unknown);
                                    }
                                    writer.Write("</td></tr>");

                                    IList netInfoList = server.NetworkCards;
                                    for (int i = 0; i < netInfoList.Count; i++)
                                    {
                                        NetworkCardInfo info = netInfoList[i] as NetworkCardInfo;
                                        writer.Write("<tr><td>");
                                        writer.Write(StringDef.NetworkLoad + "(" + i + ")");
                                        writer.Write(":</td><td>");
                                        if (memInfo != null)
                                        {
                                            writer.Write("RX:");
                                            writer.Write(info.RXRate / 1024);
                                            writer.Write(" KByte/s&nbsp;&nbsp;");
                                            writer.Write("TX:");
                                            writer.Write(info.TXRate / 1024);
                                            writer.Write(" KByte/s");
                                        }
                                        else
                                        {
                                            writer.Write(StringDef.Unknown);
                                        }
                                        writer.Write("</td></tr>");
                                    }

                                    writer.Write("<tr><td>");
                                    writer.Write(StringDef.ProcessCount);
                                    writer.Write(":</td><td>");
                                    writer.Write(server.ProcessInfos.Count);
                                    writer.Write("</td></tr>");

                                    writer.Write("<tr><td>");
                                    writer.Write(StringDef.PlugInCount);
                                    writer.Write(":</td><td>");
                                    writer.Write(server.PlugInInfos.Count);
                                    writer.Write("</td></tr>");

                                    writer.Write("</table>");
                                    writer.Write("</td></tr>");
                                }//if (!bOnlyShowShortTip)
                                else
                                {
                                    // kuangsihao custom信息的添加
                                    writer.Write("<tr><td>{0}</td></tr>", "\r\n");
                                    int userid = ((UserInfo)context.Session["CurrentUser"]).Id;
                                    string strpluginid = LordControl.PlugInGuid;
                                    try
                                    {
                                        string[] customInfoStrs = (string[])server.GetPlugInData(userid, strpluginid, LordControl.DataKeyCustomInfo);
                                        if (customInfoStrs != null)
                                            foreach (string custominfo in customInfoStrs)
                                            {
                                                writer.Write("<tr><td style='text-align: left; color: Yellow;'>{0}</td></tr>", custominfo);
                                            }
                                        else
                                            writer.Write("<tr><td style='text-align: left; color: Yellow;'>{0}</td></tr>", "无任何custom信息。");
                                    }
                                    catch (Exception ex)
                                    {
                                        writer.Write("<tr><td style='text-align: left; color: Yellow;'>{0}</td></tr>", ex.Message);
                                    }
                                }
							}
							else
							{
								writer.Write("<tr><td align='center' style='font-weight:bold;'>");
								writer.Write("<font color='#FF0000'>");
								writer.Write(StringDef.Disconnected);
								writer.Write("</font>");
								writer.Write("</td></tr>");
							}


							writer.Write("</table>");
						}
						else
						{
							writer.Write(StringDef.NoInfo);
						}
					//}
				}
			}
		}		
	}

	private void ProcessCancelFMTask(HttpContext context)
	{
		if (context.Request.Params[WebConfig.ParamServerId] != null)
		{
			int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
			int taskIndex = int.Parse(context.Request.Params[WebConfig.ParamFMTaskIndex]);
			GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

			StringBuilder outputHtml = new StringBuilder();

			if (server != null)
			{
				FileManageTask cancelTask = server.FMTask[taskIndex] as FileManageTask;
				server.RemoveFMTask(taskIndex);

				outputHtml.Append("<div class='");
				outputHtml.Append(CssConfig.MessageAttention);
				outputHtml.Append("'>文件管理任务“");
				outputHtml.Append(cancelTask.ToString());
				outputHtml.Append("”已经取消</div>");
			}

			context.Response.Output.Write(outputHtml.ToString());
		}
	}

	private void CreateTable(HttpContext context,ServerGroup group)
	{
		if (context == null)
            throw new ArgumentNullException("context");
		if (group == null)
			return;

        HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output);
        
		writer.Write("<tr><td width='75' align='center' class={0}>{1}</td><td>", CssConfig.TableCellHeader, group.Name);

		if (group.GroupType == ServerGroup.Type.Group)
		{
			writer.Write("<table cellspacing=1 class={0}>", CssConfig.TableNormal);

			IList childGroupList = group.List;
			foreach (ServerGroup childGroup in childGroupList)
			{
				CreateTable(context, childGroup);
			}

			writer.Write("</table>");
		}
		else if (group.GroupType == ServerGroup.Type.Server)
		{
			IList gameServerList = group.List;
			
			string columnWidth = "width='" + (100 / GameServerOverviewColumnPerRow) + "%'";
			int totalCount = gameServerList.Count;
			int groupRow = gameServerList.Count / GameServerOverviewColumnPerRow;
			int count = 0;
			for (int row = 0; row <= groupRow; row++)
			{
				writer.Write("<table width=100% cellspacing=1 cellpadding=0>");
				writer.Write("<tr>");

				for (int col = 0; col < GameServerOverviewColumnPerRow; col++)
				{
					if (count < totalCount)
					{
						GameServer server = gameServerList[count] as GameServer;
                        if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
                        {
                            //没有GameServerManager上读权限
                            writer.Write("<td " + columnWidth + ">");
                            writer.Write("<table width='100%' cellpadding='0' cellspacing='1' class='{0}'>", CssConfig.ServerOverviewNoPrivilege);
                            writer.Write("<tr height='20'>");
                            writer.Write("<td align='center' title={0}>{1}</td></tr></table>",StringDef.NotEnoughPrivilegeShort,server.Name);
                            writer.Write("</td>");
                        }
                        else
                        {
                            string style = (server.IsConnected) ? CssConfig.ServerOverviewConnected : CssConfig.ServerOverviewDisconnected;
                            writer.Write("<td " + columnWidth + ">");

                            writer.Write("<table width='100%' cellpadding='0' cellspacing='1' class='" + style + "' onmouseover='setNavigatingGameServer(" + server.Id + ");showtip();' onmouseout='hidetip();setNavigatingGameServer(0);'>");
                            writer.Write("<tr height='10'>");
                            writer.Write("<td align='center'>");
                            writer.Write("<a href='ServerDetail.aspx?serverId=");
                            writer.Write(server.Id);
                            writer.Write("'>");
                            if (server.Name.Length <= 8)
                            {
                                writer.Write(server.Name);
                            }
                            else
                            {
                                writer.Write(server.Name.Substring(0, 6) + "..");
                            }
                            writer.Write("</a></td>");

                            writer.Write("<td width='10'>");

                            //double serverNetworkLoad = 0;
                            //if (server.IsConnected && server.NetworkCards.Count > 0)
                            //{
                            //    NetworkCardInfo networkInfo = server.NetworkCards[0] as NetworkCardInfo;
                            //    serverNetworkLoad = networkInfo.RXRate / 1024 / 1024 / 12.5;
                            //}

                            //ProgressBar networkLoadBar = new ProgressBar();
                            //networkLoadBar.Progress = serverNetworkLoad;
                            //networkLoadBar.Width = new Unit(4, UnitType.Pixel);
                            //networkLoadBar.Height = new Unit(12, UnitType.Pixel);
                            //networkLoadBar.Orientation = Orientation.Vertical;
                            //networkLoadBar.ShowIndicator = false;
                            //networkLoadBar.CssClass = "CpuLoadBarBorder";
                            //if (serverNetworkLoad < 0.33)
                            //{
                            //    networkLoadBar.ProcessedCssClass = "CpuLoadBarLowLoad";
                            //}
                            //else if (serverNetworkLoad < 0.66)
                            //{
                            //    networkLoadBar.ProcessedCssClass = "CpuLoadBarMidLoad";
                            //}
                            //else
                            //{
                            //    networkLoadBar.ProcessedCssClass = "CpuLoadBarHighLoad";
                            //    networkLoadBar.CompletedCssClass = "CpuLoadBarHighLoad";
                            //}

                            //networkLoadBar.RenderControl(writer);

                            //writer.Write("</td><td width='8'>");

                            ProgressBar taskProgress = new ProgressBar();

                            double serverCpuLoad = 0;
                            if (server.IsConnected && server.CurrentCpuLoad >= 0)
                            {
                                serverCpuLoad = (double)server.CurrentCpuLoad / 100;
                                taskProgress.ToolTip = StringDef.CpuUsage + StringDef.Colon + decimal.Round((decimal)server.CurrentCpuLoad, 2) + "%";
                            }
                            taskProgress.Progress = serverCpuLoad;
                            taskProgress.Width = new Unit(8, UnitType.Pixel);
                            taskProgress.Height = new Unit(18, UnitType.Pixel);
                            taskProgress.Orientation = Orientation.Vertical;
                            taskProgress.ShowIndicator = false;
                            taskProgress.CssClass = "CpuLoadBarBorder";
                            //根据负载不同颜色不同
                            if (serverCpuLoad < 0.3)
                            {
                                taskProgress.ProcessedCssClass = "CpuLoadBarLowLoad";
                            }
                            else if (serverCpuLoad < 0.8)
                            {
                                taskProgress.ProcessedCssClass = "CpuLoadBarMidLoad";
                            }
                            else
                            {
                                taskProgress.ProcessedCssClass = "CpuLoadBarHighLoad";
                            }
                            taskProgress.CompletedCssClass = "CpuLoadBarHighLoad";

                            taskProgress.RenderControl(writer);

                            writer.Write("</td>");

                            writer.Write("<td width='10'>");

                            taskProgress = new ProgressBar();

                            double serverNetCardLoad = 0;
                            if (server.IsConnected && server.CurrentNetCardsInfo >= 0)
                            {
                                serverNetCardLoad = server.CurrentNetCardsInfo;
                                taskProgress.ToolTip = StringDef.NetUsage + StringDef.Colon + decimal.Round((decimal)serverNetCardLoad * 100, 2) + "%";
                            }
                            taskProgress.Progress = serverNetCardLoad;
                            taskProgress.Width = new Unit(8, UnitType.Pixel);
                            taskProgress.Height = new Unit(18, UnitType.Pixel);
                            taskProgress.Orientation = Orientation.Vertical;
                            taskProgress.ShowIndicator = false;
                            taskProgress.CssClass = "CpuLoadBarBorder";
                            //根据负载不同颜色不同
                            if (serverNetCardLoad < 0.3)
                            {
                                taskProgress.ProcessedCssClass = "CpuLoadBarLowLoad";
                            }
                            else if (serverNetCardLoad < 0.8)
                            {
                                taskProgress.ProcessedCssClass = "CpuLoadBarMidLoad";
                            }
                            else
                            {
                                taskProgress.ProcessedCssClass = "CpuLoadBarHighLoad";
                            }

                            taskProgress.CompletedCssClass = "CpuLoadBarHighLoad";

                            taskProgress.RenderControl(writer);

                            writer.Write("</td>");

                            writer.Write("</tr>");
                            writer.Write("</table>");
                            writer.Write("</td>");
                        }
					}
					else
					{
						writer.Write("<td " + columnWidth + ">");
						writer.Write("</td>");
					}

					count++;
				}

				writer.Write("</tr>");
				writer.Write("</table>");
			}
		}

		writer.Write("</td></tr>");
	}
	
	private void ProcessGetGameServerOverview(HttpContext context)
	{
		using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
		{
			writer.Write("<table cellspacing=1 class={0}>", CssConfig.TableNormal);

			IList netGroupList = AdminServer.TheInstance.GameServerManager.TopServerGroups;

			foreach (ServerGroup netGroup in netGroupList)
			{
                CreateTable(context, netGroup);
			}

			writer.Write("</table>");
		}
	}

    private void ProcessGetCabinetOverviewMonitor(HttpContext context)
    {
        if (context == null)
            throw new ArgumentNullException("context");

        using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
        {
            writer.Write("<input type='hidden' id='totalPlayerCountHiddenInput' value='{0}' />", "总在线人数：" + AdminServer.TheInstance.GameServerManager.RootRegion.NowPlayers.ToString() + "   离线托管人数：" + AdminServer.TheInstance.GameServerManager.RootRegion.OfflinePlayer.ToString() + "   历史最高人数：" + AdminServer.TheInstance.GameServerManager.RootRegion.MaxPlayer.ToString());
        }

        foreach (string cabinet in AdminServer.TheInstance.GameServerManager.CabinetList)
        {
            if (cabinet == string.Empty) continue;
            CreateCabinetOverviewMonitor(context,cabinet);
        }
    }
    
    private void ProcessGetServerGroupOverviewMonitor(HttpContext context)
    {
        if (context == null)
            throw new ArgumentNullException("context");
                
        IList netGroupList = AdminServer.TheInstance.GameServerManager.TopServerGroups;

        ServerGroup tempServerGroup = new ServerGroup();
        tempServerGroup.GroupType = ServerGroup.Type.Group;
        
        foreach (ServerGroup netGroup in netGroupList)
        {
            if (netGroup.GroupType == ServerGroup.Type.Server)
            {
                tempServerGroup.List.Add(netGroup);
            }
            else if (netGroup.GroupType == ServerGroup.Type.Group)
            {
                GetActualServerGroup(context, netGroup, netGroup.Name);
            }
        }

        if (tempServerGroup.List.Count > 0)
        {            
            //TODO; 用-1代表directUnder,用-2代表机柜   
            CreateServerGroupOverviewMonitor(context, tempServerGroup, "/",-1);
        }

        //得到总人数，将其存储在隐藏控件中，传回Browser供Javascript读取并显示
        using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
        {
            writer.Write("<input type='hidden' id='totalPlayerCountHiddenInput' value='{0}' />", "总在线人数：" + AdminServer.TheInstance.GameServerManager.RootRegion.NowPlayers.ToString() + "   离线托管人数：" + AdminServer.TheInstance.GameServerManager.RootRegion.OfflinePlayer.ToString() + "   历史最高人数：" + AdminServer.TheInstance.GameServerManager.RootRegion.MaxPlayer.ToString());
        }
    }

    private void GetActualServerGroup(HttpContext context, ServerGroup netGroup, String groupName)
    {
        if (context == null)
            throw new ArgumentNullException("context");
        if (netGroup == null)
            return;
       
        
        int regionID;
        
        //实际上，传进来的netGroup一定类型是Group。因为已经在上一个函数处理过了
        if (netGroup.GroupType == ServerGroup.Type.Group)
        {
            IList childGroupIList = netGroup.List;
            ServerGroup tempServerGroup = new ServerGroup();
            tempServerGroup.GroupType = ServerGroup.Type.Group;

            regionID = netGroup.Id;
            
            if ((childGroupIList != null) && (childGroupIList.Count > 0))
            {
                foreach (ServerGroup childGroup in childGroupIList)
                {
                    if (childGroup.GroupType == ServerGroup.Type.Group)
                    {
                        string childGroupName = groupName + "/" + childGroup.Name;
                        GetActualServerGroup(context, childGroup, childGroupName);
                    }
                    else if (childGroup.GroupType == ServerGroup.Type.Server)
                    {
                        tempServerGroup.List.Add(childGroup);
                    }
                }
            }
            else
            {
                return;   
            }

            CreateServerGroupOverviewMonitor(context, tempServerGroup, groupName, regionID);
        }
    }

    private void CreateServerGroupOverviewMonitor(HttpContext context, ServerGroup netGroup, String groupName, int regionID)
    {
        using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
        {

            string groupContentString = string.Empty;

            int regionTotalPlayerCount = 0;
            int regionTotalPlayerOfflineCount = 0;
            
  
            groupContentString += string.Format("<tr>");
            groupContentString += string.Format("<td>");
            
            IList childGroupIList = netGroup.List;

            int count = 0;
            int groupCount = childGroupIList.Count;
            int groupPerRow = 10;
            int regionMaxPlayers = 0;
            string columnWidth = "width='" + (100 / groupPerRow) + "%'";
            int rowCount = groupCount / groupPerRow;
            int remainder = groupCount % groupPerRow;            
            if (remainder > 0)
                rowCount++;

            groupContentString += string.Format("<table width=100% cellspacing=1 cellpadding=0>");
            //writer.Write("<table width=100% cellspacing=1 cellpadding=0>");

            for (int i = 0; i < rowCount; i++)
            {
                groupContentString += string.Format("<tr>");
                //writer.Write("<tr>");

                for (int j = 0; j < groupPerRow; j++)
                {
                    if (count < groupCount)
                    {
                        ServerGroup groupToWrite = childGroupIList[i * groupPerRow + j] as ServerGroup;                        
                        
                        IList serverIList = groupToWrite.List;

                        //GameServer.ServiceState serverGroupState = GameServer.ServiceState.Running;
                        int enumLen = Enum.GetValues(typeof(GameServer.ServiceState)).GetLength(0) - 1;
                        GameServer.ServiceState serverGroupState = GameServer.ParseServiceState(Enum.GetName(typeof(GameServer.ServiceState), enumLen));  //这个状态的颜色显示等级最低
                        
                        
                        bool noWarning = true;
                        //IList checkResults = AdminServer.TheInstance.GameServerMonitor.LastCheckResults;

                        bool bUpdating = false;
                        
                        bool bNoConnected = false; //判断该组是否有网络连接
                        foreach (GameServer server in serverIList)
                        {
                            if (server.GameServerState == GameServer.ServerState.Unknown)
                            {
                                bNoConnected = true;
                                break;
                            }
                            
                            
                            if ((int)server.GameServiceState < (int)serverGroupState)
                            {
                                serverGroupState = server.GameServiceState;
                            }

                            if (noWarning == true)
                            {
                                //int indexOfServer = AdminServer.TheInstance.GameServerManager.Servers.IndexOf(server);
                                //if (checkResults != null && indexOfServer < checkResults.Count && indexOfServer >= 0)
                                //{
                                //    CheckResult result = checkResults[indexOfServer] as CheckResult;
                                //    noWarning = result.NoProblem;
                                //}

                                noWarning = server.LastCheckResult.NoProblem;
                            }

                            if (server.UpdateOrDownState == GameServer.UpdateState.Updating)
                            {
                                bUpdating = true;
                            }
                        }

                        string color = String.Empty;

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
                                //case GameServer.ServiceState.Updating:
                                //  color = "MediumOrchid";
                                //break;
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
                        }

                        if (serverGroupState != GameServer.ServiceState.Stopping && serverGroupState != GameServer.ServiceState.Starting && bUpdating)
                        {
                            color = "MediumOrchid";
                        }


                        groupContentString += string.Format("<td " + columnWidth + ">");
                        groupContentString += string.Format("<table width='100%' cellpadding='0' cellspacing='1' class='" + CssConfig.ServerGroupOverviewMonitor + "' style='background-color:" + color + "' onmouseover='setNavigatingServerGroup(" + groupToWrite.Id + ");if (enableToolTip) {{ posTooltip(); showtip(); }}' onmouseout='hidetip();restoreTooltip();setNavigatingServerGroup(0);'>");
                        groupContentString += string.Format("<tr height='10'>");
                        groupContentString += string.Format("<td align='center'>");
                        groupContentString += string.Format("<a style='color:blue' href='ServerList.aspx?serverGroupId=");
                        groupContentString += string.Format(groupToWrite.Id.ToString());
                        groupContentString += string.Format("'>");
                        //writer.Write("<td " + columnWidth + ">");
                        //writer.Write("<table width='100%' cellpadding='0' cellspacing='1' class='" + CssConfig.ServerGroupOverviewMonitor + "' style='background-color:" + color + "' onmouseover='setNavigatingServerGroup(" + groupToWrite.Id + ");posTooltip();showtip();' onmouseout='hidetip();restoreTooltip();setNavigatingServerGroup(0);'>");
                        //writer.Write("<tr height='10'>");
                        //writer.Write("<td align='center'>");
                        //writer.Write("<a style='color:blue' href='ServerList.aspx?serverGroupId=");
                        //writer.Write(groupToWrite.Id);
                        //writer.Write("'>");
                        
                        if (groupToWrite.Name.Length <= 10)
                        {
                            groupContentString += string.Format(groupToWrite.Name);                            
                            //writer.Write(groupToWrite.Name);
                        }
                        else
                        {
                            groupContentString += string.Format(groupToWrite.Name.Substring(0, 8) + "..");
                            //writer.Write(groupToWrite.Name.Substring(0, 8) + "..");
                        }
                        
                        if (groupToWrite.group_enable == ServerGroup.GroupEnable.Close)
                        {
                            groupContentString +="(" + StringDef.Disable + ")";
                        }

                        groupContentString += string.Format("</a></td>");
                        //writer.Write("</a></td>");


                        //为了避免闪屏幕，无论有没有Warning都先占位
                        groupContentString += string.Format("<td align='center'>");
                        //writer.Write("<td align='center'>");
                        
                        ProgressBar warningProgressBar = new ProgressBar();
                        warningProgressBar.Width = new Unit(10, UnitType.Pixel);
                        warningProgressBar.Height = new Unit(16, UnitType.Pixel);
                        
                        if (noWarning)
                        {
                            ColorConverter myColorConverter = new ColorConverter();
                            warningProgressBar.BackColor = (Color)myColorConverter.ConvertFromString(color);                                
                            warningProgressBar.BorderColor = (Color)myColorConverter.ConvertFromString(color);
                            
                        }
                        else
                        {
                            warningProgressBar.BackColor = Color.Black;
                            warningProgressBar.BorderColor = Color.Black;
                        }
                        warningProgressBar.BorderStyle = BorderStyle.Solid;
                        warningProgressBar.BorderWidth = new Unit(1, UnitType.Pixel);
                        warningProgressBar.ShowIndicator = false;
                        

                        StringBuilder sb = new StringBuilder();
                        StringWriter tw = new StringWriter(sb);
                        HtmlTextWriter hw = new HtmlTextWriter(tw);
                        warningProgressBar.RenderControl(hw);
                        groupContentString += string.Format(sb.ToString());


                        groupContentString += string.Format("</td>");
                        //writer.Write("</td>");

                        groupContentString += string.Format("</tr>");
                        writer.Write("</tr>");

                        groupContentString += string.Format("<tr height='5'>");
                        groupContentString += string.Format("<td align='center' colspan='2'>");
                        
                        //这是以前的版本（包含离线托管人数和历史总人数）
                        //groupContentString += string.Format("<label>" + groupToWrite.NowPlayers.ToString()  + "<br />" + groupToWrite.OfflinePlayer.ToString() + "<br />" + groupToWrite.MaxPlayer.ToString() + "</label>");

                        //现在的版本，不再显示离线托管人数，而是改在tip中显示
                        //groupContentString += string.Format("<label>" + groupToWrite.NowPlayers.ToString() + "</label>");

                        //自定义内容的显示，显示内容的选项从context.Request.Params中获得
                        string displayContent = String.Empty;
                        string displayOption = context.Request.Params["DisplayOption"];
                        if ((displayOption != null) && (displayOption != String.Empty))
                        {
                            string singleOption;
                            for (int k = 0; k < displayOption.Length; k += 2)
                            {
                                singleOption = displayOption.Substring(k, 2);
                                switch (singleOption)
                                {
                                    case "01":
                                        displayContent += groupToWrite.NowPlayers.ToString();
                                        break;
                                    case "02":
                                        displayContent += groupToWrite.OfflinePlayer.ToString();
                                        break;
                                    case "03":
                                        displayContent += groupToWrite.MaxPlayer.ToString();
                                        break;
                                    case "04":
                                        displayContent += groupToWrite.GatewayName;
                                        break;
                                    case "05":
                                        displayContent += groupToWrite.Comment;
                                        break;
                                    case "06":
                                        displayContent += groupToWrite.Cabinet;
                                        break;
                                    case "07":
                                        displayContent += groupToWrite.GroupVer;
                                        break;
                                }

                                if (k != displayOption.Length - 2)
                                    displayContent += "<br />";
                            }
                        }

                        groupContentString += string.Format("<label>" + displayContent + "</label>");
                        
                        groupContentString += string.Format("</td>");
                        groupContentString += string.Format("</tr>");


                        groupContentString += string.Format("</table>");
                        groupContentString += string.Format("</td>");              
                    }
                    else
                    {
                        groupContentString += string.Format("<td " + columnWidth + ">");
                        groupContentString += string.Format("</td>");
                    }

                    count++;
                }

                groupContentString += string.Format("</tr>");
            }

            groupContentString += string.Format("</table>");
            writer.Write("</table>");

            groupContentString += string.Format("</td>");
            groupContentString += string.Format("</tr>");
            groupContentString += string.Format("</table>");
            

            if (regionID > 0)
            {
                regionTotalPlayerCount = AdminServer.TheInstance.GameServerManager.GetGameServerGroup(regionID).NowPlayers;
                regionTotalPlayerOfflineCount = AdminServer.TheInstance.GameServerManager.GetGameServerGroup(regionID).OfflinePlayer;
                regionMaxPlayers = AdminServer.TheInstance.GameServerManager.GetGameServerGroup(regionID).MaxPlayer;
            }
            else if (regionID == -1)
            {
                regionTotalPlayerCount = AdminServer.TheInstance.GameServerManager.DirectUnderRegion.NowPlayers;
                regionTotalPlayerOfflineCount = AdminServer.TheInstance.GameServerManager.DirectUnderRegion.OfflinePlayer;
                regionMaxPlayers = AdminServer.TheInstance.GameServerManager.DirectUnderRegion.MaxPlayer;
            }

            groupContentString = string.Format("<tr><td><table width='100%'><tr><td  align='center' style='font-size:16px;color:#FF9900;weight:2000' class='{0}'>{1}</td><td  align='right' width='8%' class='{2}'>{3}</td></tr></table></td></tr>", CssConfig.TableCellNormal, groupName, CssConfig.TableCellNormal, regionTotalPlayerCount .ToString() + "/" + regionTotalPlayerOfflineCount.ToString() + "/" + regionMaxPlayers.ToString()) + groupContentString;
            groupContentString = string.Format("<table cellspacing='0' class='{0}' style='width:1000;'>", CssConfig.TableNormal) + groupContentString;


            writer.Write(groupContentString);            
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
                    
                    //监控页面不再显示各服务器组的离线托管人数和历史最高人数，改在tip中显示
                    writer.Write("<table width='100%'>");
                    writer.Write("<tr><td style='text-align: center; color: White;'>{0}：{1}</td></tr>", "离线托管人数",serverGroup.OfflinePlayer.ToString());
                    writer.Write("<tr><td style='text-align: center; color: White;'>{0}：{1}</td></tr>", "历史最高人数", serverGroup.MaxPlayer.ToString());
                    writer.Write("<tr><td style='text-align: center; color: White;'>{0}：{1}</td></tr>", "版本号", serverGroup.GroupVer.ToString());
                    writer.Write("</table>");                    
                    
                    writer.Write("<br />");
                    
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
                                //case GameServer.ServiceState.Updating:
                                //  serverState = "更新中";
                                //color = "MediumOrchid";
                                //break;
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
    
	private void ProcessGetExeSysCmdOutput(HttpContext context)
	{
		if (context.Request.Params[WebConfig.ParamServerId] != null)
		{
			StringBuilder outputHtml = new StringBuilder();
			
			int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
			GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

			if (server != null)
			{
				outputHtml.Append("<table cellpadding='5'><tr><td>");

                if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
				{
					outputHtml.Append(StringDef.NotEnoughPrivilege);
				}
				else
				{
					if (server.IsConnected)
					{
						outputHtml.Append(">> ");

                        string lastSysCmd = server.GetPlugInData(((UserInfo)context.Session["CurrentUser"]).Id,GameServerControl.PlugInGuid, GameServerControl.DataKeyLastSystemCommand) as string;
						if (lastSysCmd != null)
						{							
							outputHtml.Append(lastSysCmd);
							outputHtml.Append("<br>");

                            string lastSysCmdInput = server.GetPlugInData(((UserInfo)context.Session["CurrentUser"]).Id,GameServerControl.PlugInGuid, GameServerControl.DataKeyLastSystemCommandInput) as string;
							if (lastSysCmdInput.Length > 0)
							{
								outputHtml.Append(lastSysCmdInput.Replace("\n", "<br>"));
							}
							outputHtml.Append("<br>");

                            string lastSysCmdOutput = server.GetPlugInData(((UserInfo)context.Session["CurrentUser"]).Id,GameServerControl.PlugInGuid, GameServerControl.DataKeyLastSystemCommandOutput) as string;
							outputHtml.Append(lastSysCmdOutput.Replace("\n", "<br>"));

                            GameServerControl.GuardTask guardTask = (GameServerControl.GuardTask)server.GetPlugInData(((UserInfo)context.Session["CurrentUser"]).Id,GameServerControl.PlugInGuid, GameServerControl.DataKeyGuardTask);
							if (guardTask == GameServerControl.GuardTask.Idle)
							{
								outputHtml.Append("<br>");
								outputHtml.Append(">> ");
							}
						}
					}
					else
					{
						outputHtml.Append(StringDef.Disconnected);
					}
				}
				
				outputHtml.Append("</td></tr></table>");
			}
			
			context.Response.Output.Write(outputHtml.ToString());
		}
	}

	private void ProcessExeSysCmd(HttpContext context)
	{
		if (context.Request.Params[WebConfig.ParamServerId] != null)
		{
			StringBuilder outputHtml = new StringBuilder();

			int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
			GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

			if (server != null)
			{
                if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.EXECUTE, context.Session))
				{
					outputHtml.Append(StringDef.NotEnoughPrivilege);
				}
				else
				{
					if (server.IsConnected)
					{
						string cmd = context.Request.Params[WebConfig.ParamSysCmd];
						string input = context.Request.Params[WebConfig.ParamSysCmdInput];
						if (cmd != null && input != null)
						{
							input = input.Replace(";", "\r\n");

							int userId = ((UserInfo)context.Session["CurrentUser"]).Id;
                            if (server.DoPlugInAction(
								userId, 
								GameServerControl.PlugInGuid, 
								GameServerControl.ActionKeyExeSysCmd, 
								cmd,
								input,
								GameServerControl.GuardTask.DoNormalSysCmd))
							{
								outputHtml.Append("OK");
								context.Response.Output.Write(outputHtml.ToString());
								return;
							}
						}						
					}

					outputHtml.Append("Error");
				}				
			}
			
			context.Response.Output.Write(outputHtml.ToString());
		}
	}

    private void ProcessCreateGuardConfigFile(HttpContext context)
    {
        if (context.Request.Params[WebConfig.ParamServerId] != null)
        {
            int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
            GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

            if (server != null)
            {
                try
                {
                    server.LordConfig.WriteGuardConfig(context.Response.OutputStream);
                }
                catch (Exception e)
                {
                    context.Response.ClearContent();
                    context.Response.Output.Write(e.Message);
                }
            }
        }
    }
    
	private void ProcessCreateLordConfigFile(HttpContext context)
	{
		if (context.Request.Params[WebConfig.ParamServerId] != null)
		{
			int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
			GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

			if (server != null)
			{
                //权限设置?!
                //if (false)//WebUtil.CheckPrivilege(server.SecurityObject.GetChild(PredefinedSecurityObjects.Console), OpType.EXECUTE, context.Session))
                //{
                //    context.Response.Output.Write(StringDef.NotEnoughPrivilege);
                //}
                //else
				{
					try
					{
						server.LordConfig.WriteLordConfig(context.Response.OutputStream);
					}
					catch (Exception e)
					{
						context.Response.ClearContent();
						context.Response.Output.Write(e.Message);
					}					
				}
			}
		}
	}

	void ProcessGetPlayerCount(HttpContext context)
	{
		//StringBuilder outputHtml = new StringBuilder();
		//outputHtml.Append("<table class=");
		//outputHtml.Append(CssConfig.TableSection);
		//outputHtml.Append("><tr><td class=");
		//outputHtml.Append(CssConfig.TableRowSectionTitle);
		//outputHtml.Append(">");
		//outputHtml.Append(StringDef.MemUsage);
		//outputHtml.Append("</td></tr><tr><td>");

		//if (context.Request.Params[WebConfig.ParamServerId] != null)
		//{
		//    int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
		//    GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

		//    if (!WebUtil.CheckPrivilege(server.SecurityObject.GetChild(PredefinedSecurityObjects.ServerInfo).GetChild(PredefinedSecurityObjects.Memory), OpType.READ, context.Session))
		//    {
		//        outputHtml.Append(StringDef.NotEnoughPrivilege);
		//    }
		//    else
		//    {
		//        if (server.MemoryInfos != null && server.MemoryInfos.Length > 0)
		//        {
		//            string imageFileName = null;
		//            DateTime last = DateTime.MinValue;
		//            if (g_lastGameServerMemoryUsageGraphFileTimeMap.ContainsKey(serverId))
		//            {
		//                last = (DateTime)g_lastGameServerMemoryUsageGraphFileTimeMap[serverId];
		//            }
		//            double escaped = ((TimeSpan)(DateTime.Now - last)).TotalMilliseconds;
		//            if (escaped > g_gameServerMemoryUsageGraphPeriod)
		//            {
		//                //DeleteGameServerMemoryUsageGraph(serverId);

		//                imageFileName = WebUtil.CreateRandomName("GameServerMemoryUsage_", WebConfig.GraphFileSuffix);
		//                if (CreateGameServerMemoryUsageGraph(server, imageFileName))
		//                {
		//                    outputHtml.Append(HtmlBuilder.CreateImage(WebConfig.TempGraphUrl + imageFileName, WebConfig.GameServerMemoryUsageGraphWidth, WebConfig.GameServerMemoryUsageGraphHeight));
		//                }
		//                else
		//                {
		//                    outputHtml.Append(StringDef.CreateGraphFail);
		//                }
		//            }
		//            else
		//            {
		//                string fileName = g_lastGameServerMemoryUsageGraphFile[serverId] as string;
		//                if (fileName != null)
		//                {
		//                    int index = fileName.LastIndexOf("\\");
		//                    imageFileName = fileName.Substring(index + 1);
		//                    outputHtml.Append(HtmlBuilder.CreateImage(
		//                        WebConfig.TempGraphUrl + imageFileName,
		//                        WebConfig.GameServerMemoryUsageGraphWidth,
		//                        WebConfig.GameServerMemoryUsageGraphHeight));
		//                }
		//            }
		//        }
		//        else
		//        {
		//            outputHtml.Append(StringDef.NoInfo);
		//        }
		//    }
		//}

		//outputHtml.Append("</td></tr></table>");
		//context.Response.Output.Write(outputHtml.ToString());
	}

	///// <summary>
	///// 创建游戏服务器内存使用量图表
	///// </summary>
	//private bool CreateGameServerMemoryUsageGraph(GameServer server, string imageFileName)
	//{
	//    MemoryInfo[] infos = server.MemoryInfos;
	//    if (infos != null)
	//    {
	//        MemoryInfo currentInfo = infos[infos.Length - 1];

	//        MasterPane memUsageMasterPane = new MasterPane();

	//        GraphPane currentMemUsagePane = new GraphPane();

	//        currentMemUsagePane.Margin.Top = 120;
	//        currentMemUsagePane.Margin.Bottom = 110;
	//        currentMemUsagePane.Margin.Right = 50;
	//        currentMemUsagePane.Margin.Left = 80;
	//        currentMemUsagePane.Fill = new Fill(WebConfig.GraphPaneBgColor);
	//        currentMemUsagePane.Chart.Fill = new Fill(WebConfig.GraphChartBgColor);
	//        currentMemUsagePane.Legend.IsVisible = false;
	//        currentMemUsagePane.BarSettings.ClusterScaleWidth = 1;

	//        currentMemUsagePane.XAxis.IsVisible = false;

	//        currentMemUsagePane.YAxis.Title.IsVisible = false;
	//        currentMemUsagePane.YAxis.Title.FontSpec.Size = 50;
	//        currentMemUsagePane.YAxis.IsVisible = true;
	//        currentMemUsagePane.YAxis.Scale.Max = currentInfo.TotalSize * 1.1;
	//        currentMemUsagePane.YAxis.Scale.Min = 0;
	//        currentMemUsagePane.YAxis.Scale.FontSpec.Size = 50;
	//        currentMemUsagePane.YAxis.MajorGrid.IsVisible = true;
	//        currentMemUsagePane.YAxis.MajorGrid.Color = WebConfig.GraphYAxisGridColor;
	//        currentMemUsagePane.YAxis.MajorGrid.DashOn = 0;

	//        memUsageMasterPane.Add(currentMemUsagePane);

	//        double[] value = new double[1];
	//        double[] pos = new double[1];

	//        pos[0] = 1;
	//        value[0] = currentInfo.UsedSize;
	//        BarItem bar = currentMemUsagePane.AddBar("Used Mem", null, value, Color.Blue);
	//        bar.Bar.Fill = new Fill(WebConfig.GraphBarFillColor);
	//        bar.Bar.Border = new Border(WebConfig.GraphBarBorderColor, 1);
	//        TextObj memUsedText = new TextObj((int)(value[0] / 1024) + "/" + (int)(currentInfo.TotalSize / 1024) + "M", 0.64F, 0.1F, CoordType.PaneFraction);
	//        memUsedText.FontSpec.Size = 60;
	//        currentMemUsagePane.GraphObjList.Add(memUsedText);

	//        GraphPane memUsagePane = new GraphPane();
	//        memUsageMasterPane.Add(memUsagePane);

	//        memUsagePane.Title.Text = "Memory Usage";
	//        memUsagePane.Title.FontSpec.Size = 40;
	//        memUsagePane.Fill = new Fill(WebConfig.GraphPaneBgColor);
	//        memUsagePane.Chart.Fill = new Fill(WebConfig.GraphChartBgColor);

	//        memUsagePane.Legend.FontSpec.Size = 40;
	//        memUsagePane.Legend.Position = LegendPos.Right;
	//        memUsagePane.Legend.Fill = new Fill(WebConfig.GraphLegendBgColor);
	//        memUsagePane.Legend.FontSpec.FontColor = WebConfig.GraphLegendFontColor;

	//        memUsagePane.XAxis.Title.IsVisible = false;
	//        memUsagePane.XAxis.Scale.IsVisible = true;
	//        memUsagePane.XAxis.Scale.FontSpec.Size = 40;
	//        memUsagePane.XAxis.Scale.Max = GameServer.MAX_MEMORY_INFO_HISTORY_RECORD_COUNT;
	//        memUsagePane.XAxis.Scale.Min = 0;
	//        memUsagePane.XAxis.Scale.MajorStep = 10;
	//        memUsagePane.XAxis.MajorGrid.IsVisible = true;
	//        memUsagePane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;

	//        memUsagePane.YAxis.Title.Text = "Usage";
	//        memUsagePane.YAxis.Title.FontSpec.Size = 40;
	//        memUsagePane.YAxis.Scale.FontSpec.Size = 40;
	//        memUsagePane.YAxis.Scale.Min = 0;
	//        memUsagePane.YAxis.Scale.Max = currentInfo.TotalSize * 1.1;
	//        memUsagePane.YAxis.MajorGrid.IsVisible = true;
	//        memUsagePane.YAxis.MajorGrid.DashOff = 0;
	//        memUsagePane.YAxis.MajorGrid.Color = WebConfig.GraphYAxisGridColor;

	//        double[] usedSizes = new double[infos.Length];
	//        double[] time = new double[infos.Length];

	//        int offset = GameServer.MAX_MEMORY_INFO_HISTORY_RECORD_COUNT - infos.Length;//如果记录队列未满，曲线向右对齐，这个值就是向右的偏移量
	//        for (int i = 0; i < infos.Length; i++)
	//        {
	//            MemoryInfo info = infos[i];
	//            time[i] = i + offset;
	//            usedSizes[i] = info.TotalSize - info.FreeSize;
	//        }
	//        memUsagePane.AddCurve("Used", time, usedSizes, Color.Yellow, SymbolType.None);//TODO

	//        Bitmap bitmap = new Bitmap(1, 1);
	//        using (Graphics g = Graphics.FromImage(bitmap))
	//        {
	//            memUsageMasterPane.AxisChange(g);
	//            memUsageMasterPane.SetLayout(g, false, new int[] { 1, 1 }, new float[] { 1, 6 });
	//        }

	//        bitmap = memUsageMasterPane.GetImage(WebConfig.GameServerMemoryUsageGraphWidth, WebConfig.GameServerMemoryUsageGraphHeight, 75.0f);
	//        string file = WebConfig.WebsiteRootPath + WebConfig.TempGraphPath + imageFileName;

	//        try
	//        {
	//            bitmap.Save(file, WebConfig.GraphImageFormat);
	//            g_lastGameServerMemoryUsageGraphFile[server.Id] = file;
	//            g_lastGameServerMemoryUsageGraphFileTimeMap[server.Id] = DateTime.Now;
	//            TempFileManager.TheInstance.AddTempFile(file, g_gameServerMemoryUsageGraphPeriod * WebConfig.TempGraphDeleteDelayMultiple);

	//            return true;
	//        }
	//        catch (Exception)
	//        {
	//            //TODO 记录错误
	//            return false;
	//        }
	//    }
	//    else
	//    {
	//        return false;
	//    }
	//}

    void ProcessGetFSEyeFile(HttpContext context)
    {
        //context.Request.ContentEncoding = Encoding.Default;
        string fileNameParam = context.Request.Params[WebConfig.ParamFileName];
        byte[] fileNameBytes = Convert.FromBase64String(fileNameParam.Replace(' ', '+'));
        string fileName = Encoding.Default.GetString(fileNameBytes);
        //文件名判断
        if (fileName == null || fileName.Length==0 || fileName[0] == '.' || fileName.IndexOfAny(new char[] { '*', ':', '\\', '/', '<', '>', '?', '"', '|' }) != -1)
        {
            return;
        }
        if (!File.Exists(SystemConfig.Current.AdminServerUploadFileRootPath + fileName))
        {
            //文件不存在
            context.Response.Write(string.Format(StringDef.MsgNotExist,StringDef.File));
            return;
        }
        context.Response.Clear();
        context.Response.ClearHeaders();
        context.Response.ClearContent();
        context.Response.HeaderEncoding = SystemConfig.Current.DefaultEncoding;
        context.Response.ContentEncoding = SystemConfig.Current.DefaultEncoding;
        context.Response.AppendHeader("Content-disposition", string.Format("attachment; filename={0}", HttpUtility.UrlEncode(fileName)));
        context.Response.ContentType = "application/octet-stream";
        context.Response.TransmitFile(SystemConfig.Current.AdminServerUploadFileRootPath + fileName);
    }

    /// <summary>
    /// 服务器玩家数量总览
    /// </summary>
    /// <param name="context"></param>
    private void ProcessGetGameServerOverviewPlayerCount(HttpContext context)
    {
        using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
        {
            writer.Write("<table cellspacing=1 class={0}>", CssConfig.TableNormal);

            IList topServerGroup = AdminServer.TheInstance.GameServerManager.TopServerGroups;

            foreach (ServerGroup serverGroup in topServerGroup)
            {
                CreateTablePlayerCount(context, serverGroup);
            }
            writer.Write("</table>");
        }
    }

    private void CreateTablePlayerCount(HttpContext context, ServerGroup group)
    {
        if (context == null)
            throw new ArgumentNullException("writer");
        if (group == null)
            return;

        HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output);
        
        if (group.GroupType == ServerGroup.Type.Group)
        {
            writer.Write("<tr><td width='75' align='center' class={0}>{1}</td><td>", CssConfig.TableCellHeader, group.Name);
            
            context.Response.Output.Write("<table cellspacing=1 class={0}>", CssConfig.TableNormal);

            IList childGroupList = group.List;
            foreach (ServerGroup childGroup in childGroupList)
            {
                CreateTablePlayerCount(context, childGroup);
            }

            writer.Write("</table>");
        }
        else if (group.GroupType == ServerGroup.Type.Server)
        {
            writer.Write("<tr><td width='75' align='center' class={0}><a href='ServerGroupPlayerCount.aspx?groupId={1}' style='text-decoration:blink;color:White;'>{2}</a></td><td>", CssConfig.TableCellHeader, group.Id, group.Name);
            
            IList gameServerList = group.List;

            string columnWidth = "width='" + (100 / GameServerOverviewPlayerCountColumnPerRow) + "%'";
            int totalGameServerCount = 0;
            foreach (GameServer server in gameServerList)
            {
                if (server.Type == GameServer.ServerType.gameserver) ++totalGameServerCount;
            }
            int groupRow = totalGameServerCount / GameServerOverviewPlayerCountColumnPerRow;
            int count = 0;
            for (int row = 0; row <= groupRow; row++)
            {
                writer.Write("<table width=100% cellspacing=1 cellpadding=0>");
                writer.Write("<tr>");

                //int totalServerCount = 0;
                //int totalPlayerCount = 0;
                
                for (int col = 0; col < GameServerOverviewPlayerCountColumnPerRow; col++)
                {
                    if (count < gameServerList.Count)
                    {
                        GameServer server = gameServerList[count] as GameServer;
                        
                        if (server.Type == GameServer.ServerType.gameserver)
                        {
                            if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.READ, context.Session))
                            {
                                //无权限
                                writer.Write("<td " + columnWidth + ">");
                                writer.Write("<table width='100%' cellpadding='0' cellspacing='1' class='{0}'>", CssConfig.ServerOverviewNoPrivilege);
                                writer.Write("<tr height='20'>");
                                writer.Write("<td align='center' title={0}>{1}</td></tr></table>", StringDef.NotEnoughPrivilegeShort, server.Name);
                                writer.Write("</td>");
                            }
                            else
                            {
                                string style = (server.IsConnected) ? CssConfig.ServerOverviewConnected : CssConfig.ServerOverviewDisconnected;
                                writer.Write("<td " + columnWidth + ">");

                                writer.Write("<table width='100%' cellpadding='0' cellspacing='1' class='" + style + "'>");
                                writer.Write("<tr height='20'>");
                                writer.Write("<td align='center'>");
                                writer.Write("<a href='PlayerCount.aspx?serverId=");
                                writer.Write(server.Id);
                                writer.Write("'>");
                                if (server.Name.Length <= 8)
                                {
                                    writer.Write(server.Name);
                                }
                                else
                                {
                                    writer.Write(server.Name.Substring(0, 6) + "..");
                                }
                                writer.Write("</a></td>");

                                writer.Write("<td width='10'>");

                                ProgressBar taskProgress = new ProgressBar();

                                int playerCount = 0;
                                int lastPlayerCount = 0;
                                try
                                {
                                    UserInfo userInfo = context.Session[WebConfig.SessionCurrentUser] as UserInfo;
                                    if (userInfo != null && server.IsConnected)
                                    {
                                        playerCount = (int)server.GetPlugInData(userInfo.Id, LordControl.PlugInGuid, LordControl.DataKeyCurrentPlayerCount);
                                        Queue<LordControl.PlayerCountInfo> playerCountInfoQueue = server.GetPlugInData(userInfo.Id, LordControl.PlugInGuid, LordControl.DataKeyPlayerCountQueue) as Queue<LordControl.PlayerCountInfo>;
                                        if (playerCountInfoQueue != null && playerCountInfoQueue.Count > 1)
                                        {
                                            LordControl.PlayerCountInfo[] playCountInfoList = playerCountInfoQueue.ToArray();
                                            lastPlayerCount = playCountInfoList[playCountInfoList.Length - 2].Count;
                                        }
                                    }
                                }
                                catch (Exception)
                                {
                                }

                                double playerCountLoad = playerCount / DefaultGameServerMaxPlayerCount;
                                if (playerCountLoad > 1) playerCountLoad = 1;
                                if (playerCount >= 0 && server.IsConnected)
                                    taskProgress.ToolTip = StringDef.PlayerCount + StringDef.Colon + string.Format("{0}/{1}", playerCount, DefaultGameServerMaxPlayerCount);
                                else
                                    taskProgress.ToolTip = StringDef.PlayerCount + StringDef.Colon + StringDef.Unknown;

                                taskProgress.Progress = playerCountLoad;
                                taskProgress.Width = new Unit(25, UnitType.Pixel);
                                taskProgress.Height = new Unit(12, UnitType.Pixel);
                                taskProgress.Orientation = Orientation.Horizontal;
                                taskProgress.ShowIndicator = false;
                                taskProgress.CssClass = "PleyerCountBarBorder";
                                //根据负载不同颜色不同
                                if (playerCountLoad < LowLimit)
                                {
                                    taskProgress.ProcessedCssClass = "PleyerCountBarLowLoad";
                                }
                                else if (playerCountLoad < MidLimit)
                                {
                                    taskProgress.ProcessedCssClass = "PleyerCountBarMidLoad";
                                }
                                else
                                {
                                    taskProgress.ProcessedCssClass = "PleyerCountBarHighLoad";
                                }
                                taskProgress.CompletedCssClass = "PleyerCountBarHighLoad";

                                taskProgress.RenderControl(writer);
                                //writer.Write(taskProgress.ToString());
                                writer.Write("</td>");

                                if (server.IsConnected && playerCount != -1)
                                {
                                    writer.Write("<td width='10'>");
                                    if (playerCount > lastPlayerCount)
                                    {
                                        writer.Write("<img src='../Image/ArrowUp.gif' alt='↑'></img>");
                                    }
                                    else if (playerCount == lastPlayerCount)
                                    {
                                        writer.Write("<img src='../Image/ArrowRight.gif' alt='→'></img>");
                                    }
                                    else
                                    {
                                        writer.Write("<img src='../Image/ArrowDown.gif' alt='↓'></img>");
                                    }

                                    writer.Write("</td>");
                                }
                                writer.Write("</tr>");
                                writer.Write("</table>");
                                writer.Write("</td>");
                            }
                        }
                    }
                    else
                    {
                        writer.Write("<td " + columnWidth + ">");
                        writer.Write("</td>");
                    }

                    count++;
                }

                writer.Write("</tr>");
                writer.Write("</table>");
            }
        }

        writer.Write("</td></tr>");
    }
    
    private void ProcessGetServerGroupPlayerCount(HttpContext context)
    {
        if (context.Request.Params["groupId"] != null)
        {
            try
            {
                int groupId = int.Parse(context.Request.Params["groupId"]);
                ServerGroup group = AdminServer.TheInstance.GameServerManager.GetGameServerGroup(groupId);
                if (group != null && group.GroupType == ServerGroup.Type.Server)
                {
                    using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
                    {
                        IList topServerGroup = AdminServer.TheInstance.GameServerManager.TopServerGroups;

                        CreateGameServerGroupPlayerCount(context, group);
                        //foreach (ServerGroup serverGroup in topServerGroup)
                        //{
                        //    context.Response.Output.Write("<fieldset style='padding:10px,5px,5px,5px;'>");
                        //    context.Response.Output.Write(string.Format("<legend>{0}</legend>", serverGroup.Name));
                        //    CreateGameServerGroupPlayerCount(context, serverGroup);
                        //    context.Response.Output.Write("</fieldset>");
                        //    context.Response.Output.Write("<br />");
                        //}
                    }
                }
                else
                {
                    context.Response.Output.Write(StringDef.ParameterInputError + StringDef.Colon + StringDef.ServerGroupId);
                }
            }
            catch (Exception)
            {
                context.Response.Output.Write(StringDef.ParameterInputError + StringDef.Colon + StringDef.ServerGroupId);
            }
        }
    }

    private void CreateGameServerGroupPlayerCount(HttpContext context, ServerGroup group)
    {
        if (context == null)
            throw new ArgumentNullException("context");
        if (group == null)
            return;

        if (group.GroupType == ServerGroup.Type.Group)
        {
            IList childGroupList = group.List;
            foreach (ServerGroup childGroup in childGroupList)
            {
                context.Response.Output.Write("<table cellspacing=1 class={0}>", CssConfig.TableNormal);

                context.Response.Output.Write("<tr><td align='center' class={0}>{1}</td></tr>", CssConfig.TableCellHeader, childGroup.Name);
                context.Response.Output.Write("<tr><td>");
                CreateGameServerGroupPlayerCount(context, childGroup);
                context.Response.Output.Write("</td></tr>");

                context.Response.Output.Write("</table>");
            }
        }
        else if (group.GroupType == ServerGroup.Type.Server)
        {
            IList gameServerList = group.List;

            foreach (GameServer server in gameServerList)
            {
                //当server是GameServer时才显示在线人数
                if (server.Type == GameServer.ServerType.gameserver)
                    CreateGameServerPlayerCount(context, server);
            }
        }

        context.Response.Output.Write("</td></tr>");
    }

    private void CreateGameServerPlayerCount(HttpContext context, GameServer server)
    {
        if (server == null)
        {
            return;
        }

        StringBuilder outputHtml = new StringBuilder();
        outputHtml.Append("<table class=");
        outputHtml.Append(CssConfig.TableSection);
        outputHtml.Append("><tr><td class=");
        outputHtml.Append(CssConfig.TableRowSectionTitle);
        outputHtml.Append(">");
        outputHtml.Append(string.Format("{0}[{1}]{2}{3}", StringDef.GameServer, server.Name, StringDef.Last1Hour, StringDef.PlayerCount));
        outputHtml.Append("</td></tr><tr><td>");

        string imageFileName = imageFileName = WebUtil.CreateRandomName("GameServerPlayerCount_", WebConfig.GraphFileSuffix);

        if (CreateGameServerPlayerCountGraph(context, server, imageFileName))
        {
            outputHtml.Append(HtmlBuilder.CreateImage(
                WebConfig.TempGraphUrl + imageFileName,
                WebConfig.GameServerPlayerCountGraphWidth,
                WebConfig.GameServerPlayerCountGraphHeight));
        }
        else
        {
            outputHtml.Append(StringDef.CreateGraphFail);
        }

        outputHtml.Append("</td></tr></table>");
        context.Response.Output.Write(outputHtml.ToString());
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="server"></param>
    /// <param name="imageFileName"></param>
    /// <returns></returns>
    private bool CreateGameServerPlayerCountGraph(HttpContext context, GameServer server, string imageFileName)
    {
        if (server == null)
            return false;

        UserInfo user = null;
        if (context.Session[WebConfig.SessionCurrentUser] != null)
        {
            user = (UserInfo)context.Session[WebConfig.SessionCurrentUser];
        }
        Queue<LordControl.PlayerCountInfo> playerCountInfoQueue = server.GetPlugInData(user.Id, LordControl.PlugInGuid, LordControl.DataKeyPlayerCountQueue) as Queue<LordControl.PlayerCountInfo>;
        if (playerCountInfoQueue == null)
        {
            return false;
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

        MasterPane playerCountGraphMasterPane = new MasterPane();
        GraphPane graphPane = new GraphPane();

        graphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);

        graphPane.Legend.Fill.IsVisible = false;
        graphPane.Legend.Border.IsVisible = false;
        graphPane.Legend.FontSpec.Fill.IsVisible = false;
        graphPane.Legend.FontSpec.Size = 30;

        graphPane.XAxis.Title.IsVisible = true;
        graphPane.XAxis.Title.Text = StringDef.Time;
        graphPane.XAxis.Title.FontSpec.IsBold = false;
        graphPane.XAxis.Title.FontSpec.Size = 30;
        graphPane.XAxis.Scale.FontSpec.Size = 30;
        graphPane.XAxis.MajorGrid.Color = WebConfig.GraphXAxisGridColor;
        graphPane.XAxis.Type = AxisType.DateAsOrdinal;


        //graphPane.YAxis.Title.FontSpec.Size = 30;
        //graphPane.YAxis.Title.Text = StringDef.PlayerCount;
        //graphPane.YAxis.Title.FontSpec.IsBold = false;
        graphPane.YAxis.Scale.FontSpec.Size = 30;
        graphPane.YAxis.Scale.Min = 0;
        graphPane.YAxis.MajorGrid.IsVisible = true;
        graphPane.YAxis.MajorGrid.DashOff = 0;
        graphPane.YAxis.MajorGrid.Color = Color.Gray;
        graphPane.YAxis.MinorGrid.IsVisible = true;
        graphPane.YAxis.MinorGrid.Color = Color.LightGray;
        graphPane.YAxis.MinorGrid.DashOff = 0;
        //graphPane.YAxis.Scale.TextLabels = 

        //graphPane.Title.FontSpec.Size = 40;
        //graphPane.Title.IsVisible = false;
        //graphPane.Title.Text = string.Format("{0} - {1}", StringDef.PlayerCount, StringDef.Last1Hour);
        graphPane.AddCurve(StringDef.PlayerCount, timeArray, countArray, Color.Red, SymbolType.None);

        playerCountGraphMasterPane.Add(graphPane);

        Bitmap bitmap = new Bitmap(1, 1);
        using (Graphics g = Graphics.FromImage(bitmap))
        {
            playerCountGraphMasterPane.AxisChange(g);
            playerCountGraphMasterPane.SetLayout(g, PaneLayout.SingleColumn);
        }

        bitmap = playerCountGraphMasterPane.GetImage(WebConfig.GameServerPlayerCountGraphWidth, WebConfig.GameServerPlayerCountGraphHeight, 75.0f);
        string file = WebConfig.WebsiteRootPath + WebConfig.TempGraphPath + imageFileName;
        try
        {
            bitmap.Save(file, WebConfig.GraphImageFormat);
            return true;
        }
        catch (Exception)
        {
            //TODO 记录错误
            return false;
        }
    }

    //排序器，供函数CreateServerDecribeTable使用
    public class ByServerNameComparer : IComparer
    {
        int IComparer.Compare(Object x, Object y)
        {
            string xName = ((GameServer)x).Name;
            string yName = ((GameServer)y).Name;
            return ((new CaseInsensitiveComparer()).Compare(xName, yName));
        }
    }

    //排序器，按照服务器组的id排序，供函数GetActualServerGroup使用
    //public class ByServerGroupIdComparer : IComparer
    //{
    //    int IComparer.Compare(Object x, Object y)
    //    {
    //        int xId = ((ServerGroup)x).Id;
    //        int yId = ((ServerGroup)y).Id;
    //        return ((new CaseInsensitiveComparer()).Compare(xId, yId));
    //    }
    //}

    /// <summary>
    /// 查看服务器列表文件
    /// </summary>
    private void ProcessCreateServerListFile(HttpContext context)
    {
        //权限设置?!
        //if (!WebUtil.CheckPrivilege(server.SecurityObject, OpType.READ, context.Session))
        //{
        //    context.Response.Output.Write(StringDef.NotEnoughPrivilege);
        //}
        //else
        //{
        try
        {
            context.Response.Clear();
            context.Response.ClearHeaders();
            context.Response.ClearContent();
            context.Response.HeaderEncoding = SystemConfig.Current.DefaultEncoding;
            context.Response.ContentEncoding = SystemConfig.Current.DefaultEncoding;
            context.Response.AppendHeader("Content-disposition", string.Format("attachment; filename={0}", HttpUtility.UrlEncode(SystemConfig.Current.ServerListFileName)));
            context.Response.ContentType = "application/octet-stream";
            AdminServer.TheInstance.GameServerManager.CreateServerListFile(AdminServer.TheInstance.GameServerManager.ServerMap.NetList, context.Response.OutputStream, SystemConfig.Current.DefaultEncoding);
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());

            context.Response.ClearContent();
            context.Response.Output.Write(ex.Message);
        }
        //}
    }

    private void ProcessCreateCurrentServerListFile(HttpContext context)
    {
        try
        {
            if (context.Session["Store"] != null)
            {
                List<MapNode> netList = context.Session["Store"] as List<MapNode>;
                if (netList != null)
                {
                    context.Response.Clear();
                    context.Response.ClearHeaders();
                    context.Response.ClearContent();
                    context.Response.HeaderEncoding = SystemConfig.Current.DefaultEncoding;
                    context.Response.ContentEncoding = SystemConfig.Current.DefaultEncoding;
                    context.Response.AppendHeader("Content-disposition", string.Format("attachment; filename={0}", HttpUtility.UrlEncode(SystemConfig.Current.ServerListFileName)));
                    context.Response.ContentType = "application/octet-stream";
                    AdminServer.TheInstance.GameServerManager.CreateServerListFile(netList, context.Response.OutputStream, SystemConfig.Current.DefaultEncoding);
                }
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());

            context.Response.ClearContent();
            context.Response.Output.Write(ex.Message);
        }
    }
    


    private void ProcessStartGameServer(HttpContext context)
    {
        try
        {
            UserInfo userInfo = null;
            if (context.Session[WebConfig.SessionCurrentUser] != null)
            {
                userInfo = (UserInfo)context.Session[WebConfig.SessionCurrentUser];
            }
            if (userInfo != null)
            {
                if (context.Request.Params[WebConfig.ParamServerId] != null)
                {
                    int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
                    GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);
                    if (server != null)
                    {
                        if (WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.EXECUTE, context.Session))
                        {
                            if (server.DoPlugInAction(userInfo.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyStartGame))
                            {
                                //context.Response.Output.Write(StringDef.OperationSucceed);
                                AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(userInfo.Id, ServerGroup.Type.Server, server.Id, GameServer.ServerOperationType.Start, true, "【" + server.IpAddress + "】" + server.Name,server.Id.ToString()  );
                            }
                            else
                            {
                                context.Response.Output.Write(StringDef.OperationFail);
                                AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(userInfo.Id, ServerGroup.Type.Server, server.Id, GameServer.ServerOperationType.Start, false, "【" + server.IpAddress + "】" + server.Name + StringDef.Failure + StringDef.Colon + StringDef.SendMessage, server.Id.ToString());
                            }
                        }
                        else
                        {
                            context.Response.Output.Write(StringDef.NotEnoughPrivilege);
                            AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(userInfo.Id, ServerGroup.Type.Server, server.Id, GameServer.ServerOperationType.Start, false, "【" + server.IpAddress + "】" + server.Name + StringDef.Failure + StringDef.Colon + StringDef.NotEnoughPrivilege, server.Id.ToString());
                        }                        
                    }
                }
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    private void ProcessStopGameServer(HttpContext context)
    {
        try
        {
            UserInfo userInfo = null;
            if (context.Session[WebConfig.SessionCurrentUser] != null)
            {
                userInfo = (UserInfo)context.Session[WebConfig.SessionCurrentUser];
            }
            if (userInfo != null)
            {
                if (context.Request.Params[WebConfig.ParamServerId] != null)
                {
                    int serverId = int.Parse(context.Request.Params[WebConfig.ParamServerId]);
                    GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);
                    if (server != null)
                    {
                        if (WebUtil.CheckPrivilege(AdminServer.TheInstance.GameServerManager.SecurityObject, OpType.EXECUTE, context.Session))
                        {
                            if (server.DoPlugInAction(userInfo.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyStopGame))
                            {
                                //context.Response.Output.Write(StringDef.OperationSucceed);
                                AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(userInfo.Id, ServerGroup.Type.Server, server.Id, GameServer.ServerOperationType.Close, true, "【" + server.IpAddress + "】" + server.Name, server.Id.ToString() );
                            }
                            else
                            {
                                context.Response.Output.Write(StringDef.OperationFail);
                                AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(userInfo.Id, ServerGroup.Type.Server, server.Id, GameServer.ServerOperationType.Close, false, "【" + server.IpAddress + "】" + server.Name + StringDef.Failure + StringDef.Colon + StringDef.SendMessage, server.Id.ToString() );
                            }
                        }
                        else
                        {
                            context.Response.Output.Write(StringDef.NotEnoughPrivilege);
                            AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(userInfo.Id, ServerGroup.Type.Server, server.Id, GameServer.ServerOperationType.Close, false, "【" + server.IpAddress + "】" + server.Name + StringDef.Failure + StringDef.Colon + StringDef.NotEnoughPrivilege,server.Id.ToString() );
                        }
                    }
                }
            }
        }
        catch (Exception ex)
        {
            FSEye.Util.DebugLog(ex.ToString());
        }
    }

    private void CreateCabinetOverviewMonitor(HttpContext context, string cabinetName)
    {
        using (HtmlTextWriter writer = new HtmlTextWriter(context.Response.Output))
        {

            string groupContentString = string.Empty;

            groupContentString += string.Format("<tr>");
            groupContentString += string.Format("<td>");

            IList childGroupIList = AdminServer.TheInstance.GameServerManager.GetGroupInCabinet(cabinetName) ;

            int count = 0;
            int groupCount = childGroupIList.Count;
            int groupPerRow = 10;
            string columnWidth = "width='" + (100 / groupPerRow) + "%'";
            int rowCount = groupCount / groupPerRow;
            int remainder = groupCount % groupPerRow;
            if (remainder > 0)
                rowCount++;

            groupContentString += string.Format("<table width=100% cellspacing=1 cellpadding=0>");
            //writer.Write("<table width=100% cellspacing=1 cellpadding=0>");

            for (int i = 0; i < rowCount; i++)
            {
                groupContentString += string.Format("<tr>");
                //writer.Write("<tr>");

                for (int j = 0; j < groupPerRow; j++)
                {
                    if (count < groupCount)
                    {
                        ServerGroup groupToWrite = childGroupIList[i * groupPerRow + j] as ServerGroup;

                        IList serverIList = groupToWrite.List;

                        //GameServer.ServiceState serverGroupState = GameServer.ServiceState.Running;
                        int enumLen = Enum.GetValues(typeof(GameServer.ServiceState)).GetLength(0) - 1;
                        GameServer.ServiceState serverGroupState = GameServer.ParseServiceState(Enum.GetName(typeof(GameServer.ServiceState), enumLen));  //这个状态的颜色显示等级最低


                        bool noWarning = true;
                        //IList checkResults = AdminServer.TheInstance.GameServerMonitor.LastCheckResults;

                        bool bUpdating = false;

                        bool bNoConnected = false; //判断该组是否有网络连接
                        foreach (GameServer server in serverIList)
                        {
                            if (server.GameServerState == GameServer.ServerState.Unknown)
                            {
                                bNoConnected = true;
                                break;
                            }


                            if ((int)server.GameServiceState < (int)serverGroupState)
                            {
                                serverGroupState = server.GameServiceState;
                            }

                            if (noWarning == true)
                            {
                                //int indexOfServer = AdminServer.TheInstance.GameServerManager.Servers.IndexOf(server);
                                //if (checkResults != null && indexOfServer < checkResults.Count && indexOfServer >= 0)
                                //{
                                //    CheckResult result = checkResults[indexOfServer] as CheckResult;
                                //    noWarning = result.NoProblem;
                                //}

                                noWarning = server.LastCheckResult.NoProblem;
                            }

                            if (server.UpdateOrDownState == GameServer.UpdateState.Updating)
                            {
                                bUpdating = true;
                            }
                        }

                        string color = String.Empty;

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
                                //case GameServer.ServiceState.Updating:
                                //  color = "MediumOrchid";
                                //break;
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
                        }

                        if (serverGroupState != GameServer.ServiceState.Stopping && serverGroupState != GameServer.ServiceState.Starting && bUpdating)
                        {
                            color = "MediumOrchid";
                        }


                        groupContentString += string.Format("<td " + columnWidth + ">");
                        groupContentString += string.Format("<table width='100%' cellpadding='0' cellspacing='1' class='" + CssConfig.ServerGroupOverviewMonitor + "' style='background-color:" + color + "' onmouseover='setNavigatingServerGroup(" + groupToWrite.Id + ");if (enableToolTip) {{ posTooltip(); showtip(); }}' onmouseout='hidetip();restoreTooltip();setNavigatingServerGroup(0);'>");
                        groupContentString += string.Format("<tr height='10'>");
                        groupContentString += string.Format("<td align='center'>");
                        groupContentString += string.Format("<a style='color:blue' href='ServerList.aspx?serverGroupId=");
                        groupContentString += string.Format(groupToWrite.Id.ToString());
                        groupContentString += string.Format("'>");
   
                        if (groupToWrite.Name.Length <= 10)
                        {
                            groupContentString += string.Format(groupToWrite.Name);
                        }
                        else
                        {
                            groupContentString += string.Format(groupToWrite.Name.Substring(0, 8) + "..");
                        }

                        if (groupToWrite.group_enable == ServerGroup.GroupEnable.Close)
                        {
                            groupContentString += "(" + StringDef.Disable + ")";
                        }

                        groupContentString += string.Format("</a></td>");
            

                        //为了避免闪屏幕，无论有没有Warning都先占位
                        groupContentString += string.Format("<td align='center'>");


                        ProgressBar warningProgressBar = new ProgressBar();
                        warningProgressBar.Width = new Unit(10, UnitType.Pixel);
                        warningProgressBar.Height = new Unit(16, UnitType.Pixel);

                        if (noWarning)
                        {
                            ColorConverter myColorConverter = new ColorConverter();
                            warningProgressBar.BackColor = (Color)myColorConverter.ConvertFromString(color);
                            warningProgressBar.BorderColor = (Color)myColorConverter.ConvertFromString(color);

                        }
                        else
                        {
                            warningProgressBar.BackColor = Color.Black;
                            warningProgressBar.BorderColor = Color.Black;
                        }
                        warningProgressBar.BorderStyle = BorderStyle.Solid;
                        warningProgressBar.BorderWidth = new Unit(1, UnitType.Pixel);
                        warningProgressBar.ShowIndicator = false;


                        StringBuilder sb = new StringBuilder();
                        StringWriter tw = new StringWriter(sb);
                        HtmlTextWriter hw = new HtmlTextWriter(tw);
                        warningProgressBar.RenderControl(hw);
                        groupContentString += string.Format(sb.ToString());


                        groupContentString += string.Format("</td>");

                        groupContentString += string.Format("</tr>");
                        writer.Write("</tr>");

                        groupContentString += string.Format("<tr height='5'>");
                        groupContentString += string.Format("<td align='center' colspan='2'>");
                        
                        //groupContentString += string.Format("<label>" + groupToWrite.NowPlayers.ToString() + "<br />" + groupToWrite.OfflinePlayer.ToString() + "<br />" + groupToWrite.MaxPlayer.ToString() + "</label>");

                        //自定义内容的显示，显示内容的选项从context.Request.Params中获得
                        string displayContent = String.Empty;
                        string displayOption = context.Request.Params["DisplayOption"];
                        if ((displayOption != null) && (displayOption != String.Empty))
                        {
                            string singleOption;
                            for (int k = 0; k < displayOption.Length; k += 2)
                            {
                                singleOption = displayOption.Substring(k, 2);
                                switch (singleOption)
                                {
                                    case "01":
                                        displayContent += groupToWrite.NowPlayers.ToString();
                                        break;
                                    case "02":
                                        displayContent += groupToWrite.OfflinePlayer.ToString();
                                        break;
                                    case "03":
                                        displayContent += groupToWrite.MaxPlayer.ToString();
                                        break;
                                    case "04":
                                        displayContent += groupToWrite.GatewayName;
                                        break;
                                    case "05":
                                        displayContent += groupToWrite.Comment;
                                        break;
                                    case "06":
                                        displayContent += groupToWrite.Cabinet;
                                        break;
                                    case "07":
                                        displayContent += groupToWrite.GroupVer;
                                        break;
                                }

                                if (k != displayOption.Length - 2)
                                    displayContent += "<br />";
                            }
                        }                       

                        groupContentString += string.Format("<label>" + displayContent + "</label>");
                        
                        groupContentString += string.Format("</td>");
                        groupContentString += string.Format("</tr>");


                        groupContentString += string.Format("</table>");
                        groupContentString += string.Format("</td>");
                    }
                    else
                    {
                        groupContentString += string.Format("<td " + columnWidth + ">");
                        groupContentString += string.Format("</td>");
                    }

                    count++;
                }

                groupContentString += string.Format("</tr>");
            }

            groupContentString += string.Format("</table>");
            writer.Write("</table>");

            groupContentString += string.Format("</td>");
            groupContentString += string.Format("</tr>");
            groupContentString += string.Format("</table>");


            groupContentString = string.Format("<tr><td><table width='100%'><tr><td  align='center' style='font-size:16px;color:#FF9900;weight:2000' class='{0}'>{1}</td><td  align='right' width='8%' class='{2}'>{3}</td></tr></table></td></tr>", CssConfig.TableCellNormal, cabinetName, CssConfig.TableCellNormal, "-/-/-") + groupContentString;
            groupContentString = string.Format("<table cellspacing='0' class='{0}' style='width:1000;'>", CssConfig.TableNormal) + groupContentString;


            writer.Write(groupContentString);
        }
    }   
}


