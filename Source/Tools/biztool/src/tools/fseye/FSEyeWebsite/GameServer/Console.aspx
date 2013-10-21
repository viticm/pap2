<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="Console.aspx.cs" Inherits="GameServer_Console" Title="<%$ Resources:StringDef, Console %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGropDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<script type="text/javascript">
function init() {	
	setInterval(updateSysCmdOutput, 1000);
}

var updateSysCmdOutputServerId=<%=(ServerDropDownList1.SelectedGameServer != null ? ServerDropDownList1.SelectedGameServer.Id : 0) %>
var updateSysCmdOutputAjax = new sack("GameServerHandler.ashx");
var updatingSysCmdOutput = false;
var exeSysCmdAjax = new sack("GameServerHandler.ashx");
var autoScroll = true;

function updateSysCmdOutput() {		
	if (updateSysCmdOutputServerId)
	{
	    if(!updatingSysCmdOutput)
	    {
	        updatingSysCmdOutput = true;
	        updateSysCmdOutputAjax.reset();
		    updateSysCmdOutputAjax.setVar("op", "GetExeSysCmdOutput");
		    updateSysCmdOutputAjax.setVar("serverId", updateSysCmdOutputServerId);
		    updateSysCmdOutputAjax.element = "DivSysCmdOutput";
		    updateSysCmdOutputAjax.runAJAX();
		    updateSysCmdOutputAjax.onCompletion = new Function("updatingSysCmdOutput=false;scrollOutputToBottom();");
		}
	}
}

function switchAutoScroll()
{
	autoScroll = !autoScroll;
}

function scrollOutputToBottom() {
	if (autoScroll)
	{
		var outputDiv = document.getElementById("DivSysCmdOutput");		
		outputDiv.scrollTop = 100000;
	}
}

function checkCmdFinished()
{
	var cmd = document.getElementById("InputSysCmd").getAttribute("value");
	var index = cmd.indexOf("\r\n");
	if (index > 0 && index == cmd.length - 2)
	{	
		document.getElementById("InputSysCmd").setAttribute("value", cmd.substring(0, cmd.length-2));
		exeSysCmd();
	}
}

function checkCmdInputFinished()
{
	var input = document.getElementById("InputSysCmdInput").getAttribute("value");
	var index = input.indexOf("\r\n");
	if (index > 0 && index == input.length - 2)
	{	
		document.getElementById("InputSysCmdInput").setAttribute("value", input.substring(0, input.length - 2));
		exeSysCmd();
	}
}

function exeSysCmd() {
	var cmd = document.getElementById("InputSysCmd").getAttribute("value");
	var input = document.getElementById("InputSysCmdInput").getAttribute("value");
	
	exeSysCmdAjax.reset();
	exeSysCmdAjax.setVar("sysCmd", cmd);
	exeSysCmdAjax.setVar("sysCmdInput", input);
	exeSysCmdAjax.setVar("serverId", updateSysCmdOutputServerId);
	exeSysCmdAjax.setVar("op", "ExeSysCmd");
	exeSysCmdAjax.element = "DivExeSysCmdResult";
	exeSysCmdAjax.runAJAX();
	exeSysCmdAjax.onCompletion = new Function("updateSysCmdOutput()");
	
	document.getElementById("InputSysCmd").setAttribute("value", "");
	document.getElementById("InputSysCmdInput").setAttribute("value", "");
}

</script>

<table width="100%">
	<tr class="TableRowSpacing" />	
	<tr>
		<td>
			<table width="100%">
				<tr>
					<td align="left">
					    <Blaze:ServerGropDropDownList ID="ServerGroupDropDownList" runat="server" AutoPostBack="true" OnSelectedServerGroupChanged="ServerGroupIndexChanged" />
						<Blaze:ServerDropDownList ID="ServerDropDownList1" runat="server" AutoPostBack="true" />
					</td>
					<td align="right">
					    <asp:Literal runat="server" Text="<%$ Resources:StringDef,AutoScroll %>" />
						<input type="checkbox" checked="checked" onclick="switchAutoScroll();" />
					</td>
				</tr>
			</table>			
		</td>
	</tr>
	<tr>
		<td align="left">
			<div id="DivSysCmdOutput" style="width: 100%; height: 250px; border: inset 2px #FFFFFF; background-color: #000000; color: #00AA00; overflow-y:scroll;" />
		</td>
	</tr>
	<tr>
		<td align="left">
			<div id="DivExeSysCmdResult" style="width: 100%; height: 20px;" />
		</td>
	</tr>
	<tr>
		<td align="left">
			<textarea id="InputSysCmd" rows="3" cols="45" onkeyup="checkCmdFinished();"></textarea>
			<textarea id="InputSysCmdInput" rows="3" cols="45" onkeyup="checkCmdInputFinished();"></textarea>
		</td>
	</tr>	
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
            <asp:HyperLink runat="server" NavigateUrl="ServerList.aspx" Text="<%$ Resources:StringDef, Return %>" />
		</td>
	</tr>
	<tr class="TableRowSpacing"></tr>
</table>

</asp:Content>

