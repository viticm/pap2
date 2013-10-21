<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_ServerDetail, App_Web_jvyc5ygz" title="<%$ Resources:StringDef, ServerDetail %>" maintainscrollpositiononpostback="true" theme="BlazeSky" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderTopMessage" runat="server">
<div id='DivMessagePanel'></div>
</asp:Content>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<script type="text/javascript">

var serverId;
var ajax = new sack("GameServerHandler.ashx")
var serverControlAjax = new sack("GameServerHandler.ashx");
var currentTaskIndex = 0;
var currentTaskDone = true;

function init() {
	setGameServerId();	
	setInterval(tick, 300);	
}

/* 设置服务器ID */
function setGameServerId() {
	serverId = parseInt(document.getElementById("GameServerId").getAttribute("value"), 10);
}

function tick()
{
	if (currentTaskDone == true)
	{
	switch (currentTaskIndex)
	{
		case 0:	updateGameServerBasicInfo();
		break;			
		case 1: updateGameServerFMTask();
		break;
		case 2: updateServerCpuStateGraph();
		break;
		case 3: flipServerCpuStateGraph();
		break;
		case 4: updateGameServerMemInfo();
		break;		
		case 5: flipGameServerMemInfoGraph();
		break;		
		case 6: updateGameServerNetworkLoad();
		break;		
		case 7: flipGameServerNetworkLoadGraph();
		break;		
		case 8: updateGameServerDiskInfo();
		break;		
		case 9: flipGameServerDiskInfoGraph();
		break;		
		case 10: updateGameServerCpuBasicInfo();
		break;
		case 11: updateGameServerNetworkCardInfo();
		break;
		case 12: updateGameServerProcessInfo();
		break;
		case 13: updateGameServerPlugInInfo();
		break;
		case 30: currentTaskIndex = 0;
		break;
	}
	
	currentTaskIndex++;
	}
}

function taskDone()
{	
	currentTaskDone = true;
//	layoutDo();
}

/* 服务器基本信息 */
function updateGameServerBasicInfo() {	
	currentTaskDone = false;
	ajax.reset();
	ajax.element = "DivGameServerBasicInfo";
	ajax.runAJAX("op=GetGameServerBasicInfo&serverId=" + serverId);	
	ajax.onCompletion = new Function("taskDone();");
}

/* 文件管理任务 */
function updateGameServerFMTask() {	
	currentTaskDone = false;
	ajax.reset();
	ajax.element = "DivGameServerFMTask";
	ajax.runAJAX("op=GetGameServerFMTaskList&serverId=" + serverId);
	ajax.onCompletion = new Function("taskDone();");
}

/* CPU负载图表 */
var updateGameServerCpuStateGraphFlipped = false;
function updateServerCpuStateGraph() {
	currentTaskDone = false;
	ajax.reset();
	if (updateGameServerCpuStateGraphFlipped === false) {
		ajax.element = 'DivImageServerCpuStateBack';
	}
	else {
		ajax.element = 'DivImageServerCpuStateFront';
	}
	ajax.runAJAX("op=GetGameServerCpuLoad&serverId=" + serverId);
	ajax.onCompletion = new Function("taskDone();");
}
function flipServerCpuStateGraph() {	
	if (updateGameServerCpuStateGraphFlipped === false) {
		updateGameServerCpuStateGraphFlipped = true;		
		document.all.DivImageServerCpuStateFront.style.visibility = 'hidden';
		document.all.DivImageServerCpuStateBack.style.visibility = 'visible';
	}
	else {
		updateGameServerCpuStateGraphFlipped = false;
		document.all.DivImageServerCpuStateFront.style.visibility = 'visible';
		document.all.DivImageServerCpuStateBack.style.visibility = 'hidden';		
	}
}

/* 内存图表 */
var updateGameServerMemInfoGraphFlipped = false;
function updateGameServerMemInfo() {
	currentTaskDone = false;
	ajax.reset();
	if (updateGameServerMemInfoGraphFlipped === false) {
		ajax.element = 'DivImageServerMemoryStateBack';
	}
	else {
		ajax.element = 'DivImageServerMemoryStateFront';
	}
	ajax.runAJAX("op=GetGameServerMemUsage&serverId=" + serverId);
	ajax.onCompletion = new Function("taskDone();");
}
function flipGameServerMemInfoGraph() {	
	if (updateGameServerMemInfoGraphFlipped === false) {
		updateGameServerMemInfoGraphFlipped = true;		
		document.all.DivImageServerMemoryStateFront.style.visibility = 'hidden';
		document.all.DivImageServerMemoryStateBack.style.visibility = 'visible';
	}
	else {
		updateGameServerMemInfoGraphFlipped = false;
		document.all.DivImageServerMemoryStateFront.style.visibility = 'visible';
		document.all.DivImageServerMemoryStateBack.style.visibility = 'hidden';		
	}
}

/* 网络负载图表 */
var updateGameServerNetworkGraphFlipped = false;
function updateGameServerNetworkLoad() {	
	currentTaskDone = false;
	ajax.reset();
	if (updateGameServerNetworkGraphFlipped === false) {
		ajax.element = 'DivGameServerNetworkLoadBack';
	}
	else {
		ajax.element = 'DivGameServerNetworkLoadFront';
	}
	ajax.runAJAX("op=GetGameServerNetworkLoad&serverId=" + serverId);
	ajax.onCompletion = new Function("taskDone();");
}
function flipGameServerNetworkLoadGraph() {	
	if (updateGameServerNetworkGraphFlipped === false) {
		updateGameServerNetworkGraphFlipped = true;		
		document.all.DivGameServerNetworkLoadFront.style.visibility = 'hidden';
		document.all.DivGameServerNetworkLoadBack.style.visibility = 'visible';
	}
	else {
		updateGameServerNetworkGraphFlipped = false;
		document.all.DivGameServerNetworkLoadFront.style.visibility = 'visible';
		document.all.DivGameServerNetworkLoadBack.style.visibility = 'hidden';		
	}
}

/* 磁盘 */
var updateGameServerDiskInfoGraphFlipped = false;
function updateGameServerDiskInfo() {
	currentTaskDone = false;
	ajax.reset();
	if (updateGameServerDiskInfoGraphFlipped === false) {
		ajax.element = 'DivImageGameServerDiskInfoBack';
	}
	else {
		ajax.element = 'DivImageGameServerDiskInfoFront';
	}
	ajax.runAJAX("op=GetGameServerDiskUsage&serverId=" + serverId);
	ajax.onCompletion = new Function("taskDone();");
}
function flipGameServerDiskInfoGraph() {	
	if (updateGameServerDiskInfoGraphFlipped === false) {
		updateGameServerDiskInfoGraphFlipped = true;		
		document.all.DivImageGameServerDiskInfoFront.style.visibility = 'hidden';
		document.all.DivImageGameServerDiskInfoBack.style.visibility = 'visible';
	}
	else {
		updateGameServerDiskInfoGraphFlipped = false;
		document.all.DivImageGameServerDiskInfoFront.style.visibility = 'visible';
		document.all.DivImageGameServerDiskInfoBack.style.visibility = 'hidden';		
	}
}

/* 自动更新游戏服务器CPU基本信息 */
function updateGameServerCpuBasicInfo() {
	currentTaskDone = false;
	ajax.reset();
	ajax.element = "DivGameServerCpuBasicInfo";
	ajax.runAJAX("op=GetGameServerCpuBasicInfo&serverId=" + serverId);
	ajax.onCompletion = new Function("taskDone();");
}

/* 自动更新游戏服务器网卡基本信息 */
function updateGameServerNetworkCardInfo() {	
	currentTaskDone = false;
	ajax.reset();
	ajax.element = "DivGameServerNetworkCardInfo";
	ajax.runAJAX("op=GetGameServerNetworkCardInfo&serverId=" + serverId);
	ajax.onCompletion = new Function("taskDone();");
}

/* 进程信息 */
function updateGameServerProcessInfo() {	
	currentTaskDone = false;
	ajax.reset();
	ajax.element = "DivGameServerProcessInfo";
	ajax.runAJAX("op=GetGameServerProcessList&serverId=" + serverId);
	ajax.onCompletion = new Function("taskDone();");
}

/* 插件信息 */
function updateGameServerPlugInInfo() {
	currentTaskDone = false;
	ajax.reset();
	ajax.element = "DivGameServerPlugInInfo";
	ajax.runAJAX("op=GetGameServerPlugInInfo&serverId=" + serverId);
	ajax.onCompletion = new Function("taskDone();");
}

/* 取消文件管理任务 */
function cancelFMTask(taskIndex) {
	serverControlAjax.reset();
	serverControlAjax.element = "DivMessagePanel";
	serverControlAjax.runAJAX("op=CancelFMTask&serverId=" + serverId + "&fmTaskIndex=" + taskIndex);	
	showAndBlinkTopMessage();
}

</script>

<input id="GameServerId" type="hidden" value="<%=GetGameServerId() %>" />

<table width="100%">
	<tr>
		<td align="center">
			<table>
				<tr>
					<td>
						<ul>
							<li><asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="Serverlist.aspx" Text="<%$ Resources:StringDef, Return %>" /></li>
						</ul>
					</td>
				</tr>
			</table>			
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center"><asp:Label runat="server" Text="<%$ Resources:StringDef,GameServerInfo %>" SkinID="Title" /></td>
	</tr>
	<tr>
		<td>
			<div id="DivGameServerBasicInfo"></div>
			
			<div id="DivGameServerFMTask"></div>
			
			<div id="DivServerCpuStateTitle"></div>			
			<div style="position: relative; height: 150px;">
				<div id="DivImageServerCpuStateFront" style="position: absolute;"></div>
				<div id="DivImageServerCpuStateBack" style="position: absolute;"></div>
			</div>
			
			<div id="DivServerMemoryStateTitle"></div>			
			<div style="position: relative; height: 150px;">
				<div id="DivImageServerMemoryStateFront" style="position: absolute;"></div>
				<div id="DivImageServerMemoryStateBack" style="position: absolute;"></div>
			</div>
			
			<div id="DivGameServerNetworkLoadTitle">
			</div>
			<div style="position: relative; height: 300px;">
				<div id="DivGameServerNetworkLoadFront" style="position: absolute;"></div>
				<div id="DivGameServerNetworkLoadBack" style="position: absolute;"></div>
			</div>
			
			<div id="DivGameServerDiskInfoTitle"></div>
			<div style="position: relative; height: 150px;">
				<div id="DivImageGameServerDiskInfoFront" style="position: absolute;"></div>
				<div id="DivImageGameServerDiskInfoBack" style="position: absolute;"></div>
			</div>
						
			<div id="DivGameServerCpuBasicInfo""></div>
			
			<div id="DivGameServerNetworkCardInfo"></div>
			
			<div id="DivGameServerProcessInfo"></div>
			
			<div id="DivGameServerPlugInInfo"></div>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
	        <asp:HyperLink runat="server" NavigateUrl="Serverlist.aspx" Text="<%$ Resources:StringDef, Return %>" />
		</td>
	</tr>	
</table>
</asp:Content>