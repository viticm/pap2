//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-10
//      File_base        : WebConfig
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : Web配置相关
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Drawing;
using System.Drawing.Imaging;
using ZedGraph;

#region Web 配置类
/// <summary>
/// Web 配置类
/// </summary>
public class WebConfig
{
	/* Common */

	public const string ApplicationName = "FSEyeWebsite";//应用名称

	public const string TempGraphPath = @"Image\TempGraph\";//临时图片目录相对路径

	public const string TempGraphUrl = @"..\Image\TempGraph\";//临时图片目录相对URL

	public const string SystemConfigFile = "SystemConfig.xml";//系统配置文件

    public const string ShellScriptDir = @"ShellScripts\";//Shell脚本目录

    public const string ConfigTemplateDir = @"ShellScripts\template\"; //系统配置文件的配置模板放置路径

	public static string WebsiteRootPath = null;//网站根目录的绝对路径

	public const int RandomMax = 99999999;

	public const int RandomMin = 10000000;


	/* Parameter */
	public const string ParamOperation = "op";
    //服务器类型
    public const string ParamServerType = "serverType";

    public const string ParamAddGroup = "addGroup";
	public const string ParamObjectId = "objectId";

	public const string ParamUserId = "userId";

	public const string ParamGroupId = "groupId";

	public const string ParamPrivilege = "privilege";

	public const string ParamPath = "path";

	public const string ParamServerId = "serverId";

    public const string ParamServerGroupId = "serverGroupId";

	public const string ParamRoleId = "roleId";
   
	public const string ParamRoleName = "roleName";

    public const string ParamcRoleName = "cRoleName";

    public const string ParamcAccountName = "cAccountName"; 

	public const string ParamCommand = "cmd";

	public const string ParamIndex = "index";

	public const string ParamFMTaskIndex = "fmTaskIndex";

	public const string ParamGMCmd = "gmCmd";

	public const string ParamQuerySessionId = "querySessionId";

	public const string ParamSysCmd = "sysCmd";

	public const string ParamSysCmdInput = "sysCmdInput";

    public const string ParamItemTemplateId = "itemTemplateId";

    public const string ParamSkillTemplateId = "skillTemplateId";

    public const string ParamAccount = "account";

    public const string ParamRoleGuid = "roleGuid";

    public const string ParamFileName = "fileName";

    public const string ParamUserName = "userName";

    public const string ParamNodeGUID = "nodeGuid";

    public const string ParamScriptId = "scriptId";

    public const string ParamTaskId = "taskId";

    public const string ParamRoleGUID = "roleGUID";
    public const string ParamitemGUID = "itemGUID";
    public const string serverID = "serverID";

	/* Others */

	public const string OpTypeModify = "modify";

	public const string OpTypeAdd = "add";

    public const string OpType = "opType";


	/* Session Key */

	public const string SessionCurrentUser = "CurrentUser";//当前用户

	public const string SessionOpResult = "OpResult";//操作结果

	public const string SessionLoginForbiddenTime = "LoginForbiddenTime";//禁止登录时间

	public const string SessionLoginErrorCount = "LoginErrorCount";//登录错误计数

	public const string SessionAutomation = "Automation";//自动化作业

	public const string SessionStrategy = "Strategy";//策略

	public const string SessionScheduledTask = "ScheduledTask";//计划任务

	public const string SessionSelectedServer = "SelectedServer";//选择的服务器

	public const string SessionLastResults = "LastResults";//上次的结果

	public const string SessionShowSiteMap = "ShowSiteMap";//显示网站地图（左边的导航栏）

	public const string SessionQueryLogOffset = "QueryLogOffset";//查询日志偏移

	public const string SessionDayIndex = "DayIndex";//日期序号


	public const string ProgressBarBgColor = "#DDDDDD";

	public const string ProgressBarBorderColor = "#000000";

	public const string ProgressBarProcessedColor = "#0000FF";

	public const string ProgressBarCompletedColor = "#00FF00";


	/* Graph */
	public static ImageFormat GraphImageFormat = ImageFormat.Png;

	public static string GraphFileSuffix = "." + GraphImageFormat.ToString();

	public static Color GraphXAxisGridColor = Color.DarkGreen;

	public static Color GraphYAxisGridColor = Color.DarkGreen;

	public static Color GraphPaneBgColor = Color.FromArgb(212, 208, 200);

	public static Color GraphChartBgColor = Color.Black;

	public static Color GraphLegendFontColor = Color.White;

	public static Color GraphLegendBgColor = Color.Black;

	public static Color GraphBarFillColor = Color.Yellow;

	public static Color GraphBarBorderColor = Color.Yellow;

	public static Color GraphDiskUsageFreeColor = Color.Purple;

	public static Color GraphDiskUsageUsedColor = Color.Blue;

	public static Color GraphDiskUsageTotalColor = Color.Orange;

	public static Color[] GraphColors = new Color[] { Color.Yellow, Color.Blue, Color.Purple, Color.Red, Color.Green, Color.White, Color.Orange };

	public static SymbolType[] GraphSymbols = new SymbolType[] { SymbolType.Square, SymbolType.Triangle, SymbolType.Circle, SymbolType.Plus, SymbolType.XCross, SymbolType.Diamond, SymbolType.Star };

	/* Game Server */
	public const int GamePlayerCountGraphWidth = 700;

	public const int GamePlayerCountGraphHeight = 100;

	public const int GameServerCpuLoadGraphWidth = 700;

	public const int GameServerCpuLoadGraphHeight = 100;

	public const int GameServerMemoryUsageGraphWidth = 700;

	public const int GameServerMemoryUsageGraphHeight = 100;

	public const int GameServerNetworkLoadGraphWidth = 700;

	public const int GameServerNetworkLoadGraphHeight = 100;

	public const int GameServerDiskInfoGraphWidth = 100;

	public const int GameServerDiskInfoGraphHeight = 100;

    public const int GameServerPlayerCountGraphWidth = 800;

    public const int GameServerPlayerCountGraphHeight = 150;

	/* GameServer Database */
	public const int GetQueryResultRetryTimes = 60;
	public const int GetQueryResultDelay = 1000;
    public const int GetQueryResultDelayMid = 100;
    public const int GetQueryResultDelayShort = 10;
	public const string GameServerDatabaseName = "XXX";

	/* TempGraph */
	public const int TempGraphDeleteDelayMultiple = 4;

	/* Statistic */
	//统计各个等级的人数
	public const int StatisticRoleCountByLevelGraphWidth = 830;
	public const int StatisticRoleCountByLevelGraphHeight = 400;
    
    //活跃玩家时间参数设置 最后登录时间到给定时间点之差小于两周（设置时间）的玩家
    public const int ActivePlayerDaySpan = 14;                    //设置的时间段
    public const int ActivePlayerDayValue = 7;                    //通常为一周(7天)->流失玩家用

    /*GM指令*/
    public const string GMCommandKickPlayer = "?gm ds KickPlayer()";
    public const string GMCommandSave = "?gm ds Save()";
    public const string GMCommandSendMail = "?gm ds SendMail({0})";
    public const string GMCommandSendMailToAll = "?gm ds SendMailToAll({0})";//群发邮件
    public const string GMCommandAddExp = "?gm ds AddExp({0})";
    public const string GMCommandAddLevel = "?gm ds GM_AddLevel({0})";
    public const string GMCommandAddMoney = "?gm ds GM_AddMoney({0})";
    public const string GMCommandAddBuff = "?gm ds AddBuff({0})";
    public const string GMCommandAddItem = "?gm ds AddItem({0})";
    public const string GMCommandAddSkillExp = "?gm ds AddSkillExp({0})";
    public const string GMCommandNewWorld = "?gm ds NewWorld({0})";
    public const string GMCommandAddArmorSet = "?gm ds AddArmorSet({0})";
    public const string GMCommandAddNpcRAttr = "?gm ds AddNpcRAttr({0})";
    public const string GMCommandAddNpcCompAttr = "?gm ds AddNpcCompAttr({0})";
    public const string GMCommandAddNpcCurAttr = "?gm ds AddNpcCurAttr({0})";
    public const string GMCommandExpPercentage = "?gm ds ExpPercentage({0})";
    public const string GMCommandQuestExpPercentage = "?gm ds QuestExpPercentage({0})";
    public const string GMCommandSkillExpPercentage = "?gm ds SkillExpPercentage({0})";
    public const string GMCommandGlobalExpPercentage = "?gm ds GlobalExpPercentage({0})";
    public const string GMCommandGlobalQuestExpPercentage = "?gm ds GlobalQuestExpPercentage({0})";
    public const string GMCommandGlobalSkillExpPercentage = "?gm ds GlobalSkillExpPercentage({0})";
    public const string GMCommandShowBanner = "?gm ds ShowBanner({0})";

    /*存储用文件*/
    public const string FileTaskList = "ScheduledTaskList.xml";
    public const string FileStrategyList = "StrategyList.xml";

    /*更新服务器目录分隔符*/
    public const string UpdateServerPathDelimiter = "/";

    /*脚本文件名*/
    public const string ScriptFileNameDBAddUsr = "DBAddUsr";
    public const string ScriptFileNameDBDelUsr = "DBDelUsr";    

	/* 页面 */
	public const string PageNotEnoughPrivilege = "~/Common/NotEnoughPrivilege.aspx";//没有足够的权限
	public const string PageHome = "~/Common/Homepage.aspx";
	public const string PageLogin = "~/Common/SystemLogin.aspx";
	public const string PageLogout = "~/Common/SystemLogout.aspx";

	/* 功能对象 */
    public const string FunctionSystemSystemConfig = "/FSEye/Function/System/SystemConfig";
    public const string FunctionSystemSystemControl = "/FSEye/Function/System/SystemControl";

	public const string FunctionGameServer = "/FSEye/Function/GameServer";
    public const string FunctionGameServerServerList = "/FSEye/Function/GameServer/ServerList";
    public const string FunctionGameServerServerOverview = "/FSEye/Function/GameServer/ServerOverview";
    public const string FunctionGameServerServerDetail = "/FSEye/Function/GameServer/ServerDetail";
    public const string FunctionGameServerServerControl = "/FSEye/Function/GameServer/ServerControl";
    public const string FunctionGameServerPlayerCount = "/FSEye/Function/GameServer/PlayerCount";
    public const string FunctionGameServerAdvancedPlayerCount = "/FSEye/Function/GameServer/AdvancedPlayerCount";
    public const string FunctionGameServerEditServer = "/FSEye/Function/GameServer/EditServer";
    public const string FunctionGameServerDeleteServer = "/FSEye/Function/GameServer/DeleteServer";
    public const string FunctionGameServerUploadFile = "/FSEye/Function/GameServer/UploadFile";
    public const string FunctionGameServerListFile = "/FSEye/Function/GameServer/ListFile";
    public const string FunctionGameServerAlertCondition = "/FSEye/Function/GameServer/AlertCondition";
    public const string FunctionGameServerAlertConfig = "/FSEye/Function/GameServer/AlertConfig";
    public const string FunctionGameServerConsole = "/FSEye/Function/GameServer/Console";
    public const string FunctionGameServerServerGroupManagement = "/FSEye/Function/GameServer/ServerGroupManagement";
    public const string FunctionGameServerDatabaseInfo = "/FSEye/Function/GameServer/DatabaseInfo";
    public const string FunctionGameServerServerOverviewPlayerCount = "/FSEye/Function/GameServer/ServerOverviewPlayerCount";    

    public const string FunctionGameMaster = "/FSEye/Function/GameMaster";
    public const string FunctionGameMasterGameSetting = "/FSEye/Function/GameMaster/GameSetting";
    public const string FunctionGameMasterGMCommand = "/FSEye/Function/GameMaster/GMCommand";
    public const string FunctionGameMasterGMCommandAdvanced = "/FSEye/Function/GameMaster/GMCommandAdvanced";
    public const string FunctionGameMasterMessageCenter = "/FSEye/Function/GameMaster/MessageCenter";
    public const string FunctionGameMasterPlayerWho = "/FSEye/Function/GameMaster/PlayerWho";
    public const string FunctionGameMasterQueryItem = "/FSEye/Function/GameMaster/QueryItem";
    public const string FunctionGameMasterQueryPlayer = "/FSEye/Function/GameMaster/QueryPlayer";
    public const string FunctionGameMasterQueryRole = "/FSEye/Function/GameMaster/QueryRole";
    public const string FunctionGameMasterRoleDetail = "/FSEye/Function/GameMaster/RoleDetail";
    public const string FunctionGameMasterRoleItemInfo = "/FSEye/Function/GameMaster/RoleItemInfo";
    public const string FunctionGameMasterRoleSkillInfo = "/FSEye/Function/GameMaster/RoleSkillInfo";
    public const string FunctionGameMasterChatMonitor = "/FSEye/Function/GameMaster/ChatMonitor";
    public const string FunctionGameMasterItemExchange = "/FSEye/Function/GameMaster/ItemExchange";
    public const string FunctionGameMasterItemFlow = "/FSEye/Function/GameMaster/ItemFlow";
    public const string FunctionGameMasterOperationQuery = "/FSEye/Function/GameMaster/OperationQuery";
    public const string FunctionGameMasterSendMail = "/FSEye/Function/GameMaster/SendMail";
    public const string FunctionGameMasterPlayerMonitor = "/FSEye/Function/GameMaster/PlayerMonitor";

    public const string FunctionGameInfo = "/FSEye/Function/GameInfo";
    public const string FunctionGameInfoTopPlayer = "/FSEye/Function/GameInfo/TopPlayer";
    public const string FunctionGameInfoGameDetail = "/FSEye/Function/GameInfo/GameDetail";
    public const string FunctionGameInfoStatistic = "/FSEye/Function/GameInfo/Statistic";
    public const string FunctionGameInfoStatisticActivePlayerStatistic = "/FSEye/Function/GameInfo/Statistic/ActivePlayerStatistic";
    public const string FunctionGameInfoStatisticAdvancedStatistic = "/FSEye/Function/GameInfo/Statistic/AdvancedStatistic";
    public const string FunctionGameInfoStatisticAuctionStatistic = "/FSEye/Function/GameInfo/Statistic/AuctionStatistic";
    public const string FunctionGameInfoStatisticEasyStatistic = "/FSEye/Function/GameInfo/Statistic/EasyStatistic";
    public const string FunctionGameInfoStatisticExpStatistic = "/FSEye/Function/GameInfo/Statistic/ExpStatistic";
    public const string FunctionGameInfoStatisticItemStatistic = "/FSEye/Function/GameInfo/Statistic/ItemStatistic";
    public const string FunctionGameInfoStatisticLevelDistribution = "/FSEye/Function/GameInfo/Statistic/LevelDistribution";
    public const string FunctionGameInfoStatisticMailStatistic = "/FSEye/Function/GameInfo/Statistic/MailStatistic";
    public const string FunctionGameInfoStatisticMoneyStatistic = "/FSEye/Function/GameInfo/Statistic/MoneyStatistic";
    public const string FunctionGameInfoStatisticNpcDeathStatistic = "/FSEye/Function/GameInfo/Statistic/NpcDeathStatistic";
    public const string FunctionGameInfoStatisticNpcDropItemStatistic = "/FSEye/Function/GameInfo/Statistic/NpcDropItemStatistic";
    public const string FunctionGameInfoStatisticProfessionalSexStatistic = "/FSEye/Function/GameInfo/Statistic/ProfessionalSexStatistic";
    public const string FunctionGameInfoStatisticRoleCountStatistic = "/FSEye/Function/GameInfo/Statistic/RoleCountStatistic";
    public const string FunctionGameInfoStatisticTaiSuiStatistic = "/FSEye/Function/GameInfo/Statistic/TaiSuiStatistic";
    public const string FunctionGameInfoStatisticClanMemberStatistic = "/FSEye/Function/GameInfo/Statistic/ClanMemberStatistic";
    public const string FunctionGameInfoStatisticSocialRelationStatistic = "/FSEye/Function/GameInfo/Statistic/SocialRelationStatistic";
    
    public const string FunctionAutomation = "/FSEye/Function/Automation";
    public const string FunctionAutomationAddScheduledTask = "/FSEye/Function/Automation/AddScheduledTask";
    public const string FunctionAutomationAddStrategy = "/FSEye/Function/Automation/AddStrategy";
    public const string FunctionAutomationBatchTask = "/FSEye/Function/Automation/BatchTask";
    public const string FunctionAutomationBatchTaskDetail = "/FSEye/Function/Automation/BatchTaskDetail";
    public const string FunctionAutomationEditBatchTask = "/FSEye/Function/Automation/EditBatchTask";
    public const string FunctionAutomationScheduledTaskList = "/FSEye/Function/Automation/ScheduledTaskList";
    public const string FunctionAutomationStrategyList = "/FSEye/Function/Automation/StrategyList";

    public const string FunctionSecurity = "/FSEye/Function/Security";
    public const string FunctionSecurityAddGroup = "/FSEye/Function/Security/AddGroup";
    public const string FunctionSecurityAddUser = "/FSEye/Function/Security/AddUser";
    public const string FunctionSecurityDeleteGroup = "/FSEye/Function/Security/DeleteGroup";
    public const string FunctionSecurityDeleteUser = "/FSEye/Function/Security/DeleteUser";
    public const string FunctionSecurityEditGroup = "/FSEye/Function/Security/EditGroup";
    public const string FunctionSecurityEditUser = "/FSEye/Function/Security/EditUser";
    public const string FunctionSecurityListGroup = "/FSEye/Function/Security/ListGroup";
    public const string FunctionSecurityListUser = "/FSEye/Function/Security/ListUser";
    public const string FunctionSecurityLog = "/FSEye/Function/Security/Log";
    public const string FunctionSecurityUserGroup = "/FSEye/Function/Security/UserGroup";
    public const string FunctionSecurityWho = "/FSEye/Function/Security/Who";
    public const string FunctionSecurityEditPrivilege = "/FSEye/Function/Security/EditPrivilege";
    //public const string FunctionSecurityUserPassword = "/FSEye/Function/Security/UserPassword";

    public const string FunctionUpdateServer = "/FSEye/Function/UpdateServer";
    public const string FunctionUpdateServerAddUpdateServer = "/FSEye/Function/UpdateServer/AddUpdateServer";
    public const string FunctionUpdateServerEditUpdateServer = "/FSEye/Function/UpdateServer/EditUpdateServer";
    public const string FunctionUpdateServerUpdateManagement = "/FSEye/Function/UpdateServer/UpdateManagement";
    public const string FunctionUpdateServerUpdateServerConfig = "/FSEye/Function/UpdateServer/UpdateServerConfig";
    public const string FunctionUpdateServerUpdateServerState = "/FSEye/Function/UpdateServer/UpdateServerState";
    public const string FunctionUpdateServerUploadFiles = "/FSEye/Function/UpdateServer/UploadFiles";

    public const string FunctionPaysys = "/FSEye/Function/Paysys";
    public const string FunctionPaysysManagement = "/FSEye/Function/Paysys/PaysysManagement";
    public const string FunctionPaysysAccountInfo = "/FSEye/Function/Paysys/AccountInfo";
    public const string FunctionPaysysAccountPassword = "/FSEye/Function/Paysys/AccountPassword";
    public const string FunctionPaysysAccountState = "/FSEye/Function/Paysys/AccountState";
    public const string FunctionPaysysGatewayInfo = "/FSEye/Function/Paysys/GatewayInfo";

    //7.22权限整理任务新增
    public const string FunctionGameServerServerOverviewAndMonitor = "/FSEye/Function/GameServer/ServerOverviewAndMonitor";
    public const string FunctionGameServerServerListConfig = "/FSEye/Function/GameServer/ServerListConfig";
    public const string FunctionGameServerAddServerGroupSimplified = "/FSEye/Function/GameServer/AddServerGroupSimplified";
    public const string FunctionGameServerEditServerGroup = "/FSEye/Function/GameServer/EditServerGroup";
    public const string FunctionGameServerServerOperationHistory = "/FSEye/Function/GameServer/ServerOperationHistory";
    public const string FunctionUpdateServerGroupStatus = "/FSEye/Function/UpdateServer/GroupStatus";
    public const string FunctionUpdateServerDispenseFiles2 = "/FSEye/Function/UpdateServer/DispenseFiles2";
    public const string FunctionUpdateServerDispenseFiles = "/FSEye/Function/UpdateServer/DispenseFiles";
    public const string FunctionUpdateServerMD5 = "/FSEye/Function/UpdateServer/MD5";
    public const string FunctionUpdateServerUploadHistory = "/FSEye/Function/UpdateServer/UploadHistory";
    public const string FunctionCustomizationCabinet = "/FSEye/Function/Customization/Cabinet";
    public const string FunctionCustomizationAddNewCabinet = "/FSEye/Function/Customization/AddNewCabinet";
}
#endregion

#region CSS配置类
public class CssConfig
{
	public const string ServerConnected = "Connected";
	public const string ServerDisconnected = "Disconnected";
	public const string TableNormal = "TN";
	public const string TableSection = "TableSection";
	public const string TableRowHeader = "TRH";
	public const string TableRowNormal = "TRN";	
	public const string TableCellHeader = "TCH";
	public const string TableCellNormal = "TCN";
	public const string TableRowSectionTitle = "TableRowSectionTitle";
	public const string TableInsidePanelNoScroll = "TableInsidePanelNoScroll";
    public const string LinkDefault = "LinkTopBar";
	
	public const string LinkButton = "LB";
	public const string LinkButtonSmall = "LBSmall";
	public const string LinkButtonLogin = "LBLogin";
	public const string Unknown = "GSUnknown";
	public const string Starting = "GSStarting";
	public const string Running = "GSRunning";
	public const string Stopping = "GSStopping";
	public const string Stopped = "GSStopped";
	public const string ModuleNormal = "ModuleNormal";
	public const string ModuleError = "ModuleError";
	public const string ModuleUnknown = "ModuleUnknown";

	public const string MessageSuccess = "MessageSuccess";
	public const string MessageFailure = "MessageFailure";
	public const string MessageAttention = "MessageAttention";
	public const string MessageNormal = "MessageNormal";

	public const string ServerOverviewConnected = "SOC";
	public const string ServerOverviewDisconnected = "SOD";
    public const string ServerOverviewNoPrivilege = "SONP";

    public const string ServerGroupOverviewMonitor = "SGOM";

    public const string ChatMsgRole = "CMR";
    public const string ChatMsgDiv = "CMD";
    public const string ChatMsgSys = "CMS";
}
#endregion

#region 图表类型
public enum ChartType
{
	Bar,
	Line
}
#endregion
