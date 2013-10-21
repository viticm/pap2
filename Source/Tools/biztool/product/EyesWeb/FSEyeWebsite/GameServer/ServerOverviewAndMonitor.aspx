<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_ServerOverviewAndMonitor, App_Web_jvyc5ygz" title="服务器总览与监控" theme="BlazeSky" %>


<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderTopMessage" Runat="Server">
	<div style="width: 13px; height: 10px; background-color: #00FF00; border: solid 1px #FFFFFF;">
	</div>
</asp:Content>



<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<script type="text/javascript">

var updateServerGroupOverviewMonitorAjax = new sack("GameServerHandler.ashx");
var updateServerGroupSummaryAjax = new sack("GameServerHandler.ashx");
var updateServerGroupSummaryId = 0;
var updatingServerGroupOverviewMonitor = false;
var updatingServerGroupSummary = false;
var intervalID = "";    //存储setInterval()所返回的ID，在调用clearInterval()时会用到
var enableToolTip = 1;  //用来指示是否打开tooltip功能

function init() {
	updateServerGroupOverviewMonitor();
}

intervalID = setInterval(updateServerGroupOverviewMonitor, 3000);
//setInterval(updateCabinetOverviewMonitor, 3000);
setInterval(updateServerGroupSummary, 1000);	

function getDisplayOptionValue() {    
    var optionValue = ""
    var options = document.getElementsByName('displayOption');
    for (var i = 0; i < options.length; i++)
    {
        if (options[i].checked)
            optionValue += options[i].value;
    }
    
    return optionValue;

}

function updateServerGroupOverviewMonitor() {	
	var displayOption = getDisplayOptionValue();
	
	if (!updatingServerGroupOverviewMonitor)
	{
		updatingServerGroupOverviewMonitor = true;
		updateServerGroupOverviewMonitorAjax.reset();
		updateServerGroupOverviewMonitorAjax.element = "DivServerGroupOverviewMonitor";
		updateServerGroupOverviewMonitorAjax.runAJAX("op=GetServerGroupOverviewMonitor&DisplayOption=" + displayOption);
		updateServerGroupOverviewMonitorAjax.onCompletion = new Function("hideTopMessage(5);showPlayerCount();updatingServerGroupOverviewMonitor=false;");
		showTopMessage(10);
	}
}

function updateCabinetOverviewMonitor() {
	var displayOption = getDisplayOptionValue();
	
	if (!updatingServerGroupOverviewMonitor)
	{
		updatingServerGroupOverviewMonitor = true;
		updateServerGroupOverviewMonitorAjax.reset();
		updateServerGroupOverviewMonitorAjax.element = "DivServerGroupOverviewMonitor";
		updateServerGroupOverviewMonitorAjax.runAJAX("op=GetCabinetMonitor&DisplayOption=" + displayOption);
		updateServerGroupOverviewMonitorAjax.onCompletion = new Function("hideTopMessage(5);showPlayerCount();updatingServerGroupOverviewMonitor=false;");
		showTopMessage(10);
	}
}

function setEnableTooltip()
{
    var checkBox = document.getElementById("enableTipCheckBox");        

    checkBox.checked ? enableToolTip = 1 : enableToolTip = 0;
}

function updateServerGroupSummary() {
    if (!enableToolTip)
    {
        return;
    }
    	
	if (updateServerGroupSummaryId && !updatingServerGroupSummary)
	{
		updatingServerGroupSummary = true;
		updateServerGroupSummaryAjax.reset();
		updateServerGroupSummaryAjax.element = "tooltip";
		updateServerGroupSummaryAjax.runAJAX("op=GetServerGroupSummary&serverGroupId=" + updateServerGroupSummaryId);
		updateServerGroupSummaryAjax.onCompletion = new Function("updatingServerGroupSummary=false");	
	}
}

function setNavigatingServerGroup(serverGroupId) {
	updateServerGroupSummaryId = serverGroupId;
}

function showPlayerCount()
{
    var totalPlayerCount = document.getElementById("totalPlayerCountHiddenInput").value;
    
    document.getElementById("ctl00_ContentPlaceHolderMain_TotalPlayerCountLabel").innerText = totalPlayerCount;
}

//根据鼠标位置设置tooltip的位置，防止遮挡鼠标无法点击超链接
function posTooltip() {
    var curMouseX = window.event.clientX;
    var curMouseY = window.event.clientY;
    
    var curWindowX = document.body.clientWidth;
    var curWindowY = document.body.clientHeight;    
   
    var toolTip = document.getElementById("tooltip");        
    toolTip.style.position = "absolute";
    
    var curTooltipX = toolTip.clientWidth;
    var curTooltipY = toolTip.clientHeight;
    
    if ((curWindowX - curMouseX) < (curTooltipX + 50))
    {
        toolTip.style.left = (curMouseX - curTooltipX -  50) + "px";
    }
    else
    {
        toolTip.style.left = (curMouseX + 25) + "px";
    }
    
    if ((curWindowY - curMouseY) < (curTooltipY + 200))
    {
        toolTip.style.top = (curMouseY - curTooltipY - 50) + "px";
    }
    else
    {
        toolTip.style.top = (curMouseY - 50) + "px";
    }           
}

//鼠标移出时重设tooltip的位置
function restoreTooltip() {
        var toolTip = document.getElementById("tooltip");
        toolTip.style.position = "";
        toolTip.style.left = "";
        toolTip.style.top = "";
}

//过滤
function filterDropDownListChanged(filterString) {
    if (intervalID > 0){
        clearInterval(intervalID);
     }    
    
    updatingServerGroupOverviewMonitor=false;
    
    if ( "1" == filterString){ 
        updateServerGroupOverviewMonitor();       
        intervalID = setInterval(updateServerGroupOverviewMonitor, 3000);
    }
    else if ( "2" == filterString){
        updateCabinetOverviewMonitor();
        intervalID = setInterval(updateCabinetOverviewMonitor, 3000);
    }
}

</script>

<table width="100%">
	<tr>
		<td align="center"><asp:Label ID="Label1" runat="server" Text="服务器总览与监控" SkinID="Title" />&nbsp(<asp:Label ID="TotalPlayerCountLabel" runat="server" SkinID="Title" />)</td>		
	</tr>
	<tr class="TableRowSpacing" />
                <tr align="left">
                    <td>
		                     显示模式：<select id="filterDropDownList" onchange="filterDropDownListChanged(this.options[this.selectedIndex].value);" >  
		                    <option value="1">区</option>
		                    <option value="2">机柜</option>		                    
		                    </select>
		                    &nbsp;&nbsp;
		                    <input type="checkbox" id="enableTipCheckBox" checked="checked" onclick="setEnableTooltip();" />启用Tips		          		                
		            </td>
                </tr>
	<tr class="TableRowSpacing" />
	<tr>
	    <td>
	        <input type="checkbox" name="displayOption" value="01" checked="checked" />当前在线人数&nbsp;&nbsp;
	        <input type="checkbox" name="displayOption" value="02" />当前离线托管人数&nbsp;&nbsp;
	        <input type="checkbox" name="displayOption" value="03" />历史最高人数&nbsp;&nbsp;
	        <input type="checkbox" name="displayOption" value="04" />区服编号&nbsp;&nbsp;
	        <input type="checkbox" name="displayOption" value="05" />备注&nbsp;&nbsp;
	        <input type="checkbox" name="displayOption" value="06" />机房机柜信息&nbsp;&nbsp;
	        <input type="checkbox" name="displayOption" value="07" />当前版本号&nbsp;&nbsp;
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td>
			<div id="DivServerGroupOverviewMonitor" style="width: 100%" class="DivNormal"></div>
		</td>
	</tr>

</table>

<br /><br /><br />

<table width="100%">
	<tr>
	    <td align="center">如果出现小方块则说明组内的服务器出现告警信息。</td>
	</tr>
	<tr>
	    <td align="center">服务器组背景颜色优先级别从高到低依次表示：</td>
	</tr>
</table>

<br /><br />

<table width="400px" align="center">
    <tr>
	    <td align="center" width="20%" height="20px" style="background-color:#996633"></td>
	    <td align="center">网络断开</td>
	</tr>
	<tr>
	    <td align="center" width="20%" height="20px" style="background-color:Gold"></td>
	    <td align="center">启动中</td>
	</tr>
	<tr>
	    <td align="center" width="20%" height="20px" style="background-color:DeepSkyBlue"></td>
	    <td align="center">关闭中</td>
	</tr>
	<tr>
	    <td align="center" width="20%" height="20px" style="background-color:MediumOrchid"></td>
	    <td align="center">更新中，表示guard解压更新包覆盖本地游戏服务器文件</td>
	</tr>
	<tr>
	    <td align="center" width="20%" height="20px" style="background-color:Red"></td>
	    <td align="center">断开，指服务器程序和WEB断开</td>
	</tr>
	<tr>
	    <td align="center" width="20%" height="20px" style="background-color:DarkGray"></td>
	    <td align="center">停止，指web主动关闭服务器程序</td>
	</tr>
	<tr>
	    <td align="center" width="20%" height="20px" style="background-color:#00CC00"></td>
	    <td align="center">正常运行中</td>
	</tr>
</table>
</asp:Content>

