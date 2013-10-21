<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="ServerOverview.aspx.cs" Inherits="GameServer_ServerOverview" Title="<%$ Resources:StringDef, ServerOverview %>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderTopMessage" runat="server">
	<div style="width: 13px; height: 10px; background-color: #00FF00; border: solid 1px #FFFFFF;">
	</div>
</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<script type="text/javascript">

var updateGameServerListAjax = new sack("GameServerHandler.ashx");
var updateGameServerSummaryAjax = new sack("GameServerHandler.ashx");
var updateGameServerSummaryId = 0;
var updatingGameServerList = false;
var updatingGameSummary = false;

function init() {
	updateGameServerList();
	setInterval(updateGameServerList, 3000);
	setInterval(updateGameServerSummary, 1000);	
}

function updateGameServerList() {
	if (!updatingGameServerList)
	{
		updatingGameServerList = true;
		updateGameServerListAjax.reset();
		updateGameServerListAjax.element = "DivGameServerOverview";
		updateGameServerListAjax.runAJAX("op=GetGameServerOverview");
		updateGameServerListAjax.onCompletion = new Function("updatingGameServerList=false;hideTopMessage(5);");
		showTopMessage(10);
	}
}

function updateGameServerSummary() {	
	if (updateGameServerSummaryId && !updatingGameSummary)
	{
		updatingGameSummary = true;
		updateGameServerSummaryAjax.reset();
		updateGameServerSummaryAjax.element = "tooltip";
		updateGameServerSummaryAjax.runAJAX("op=GetGameServerSummary&serverId=" + updateGameServerSummaryId);
		updateGameServerSummaryAjax.onCompletion = new Function("updatingGameSummary=false");	
	}
}

function setNavigatingGameServer(serverId) {
	updateGameServerSummaryId = serverId;
}
</script>

<table width="100%">
	<tr>
		<td align="center"><asp:Label runat="server" Text="<%$ Resources:StringDef, GameServerList %>" SkinID="Title" /></td>
	</tr>
	<tr>
		<td>
			<div id="DivGameServerOverview" style="width: 100%" class="DivNormal"></div>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:HyperLink runat="server" NavigateUrl="ControlGame.aspx" Text="<%$ Resources:StringDef, ControlGame %>" />
			<asp:HyperLink runat="server" NavigateUrl="AddAndDelete.aspx" Text="<%$ Resources:StringDef, AddAndDelete %>" />
			<asp:HyperLink runat="server" NavigateUrl="FileManagement.aspx" Text="<%$ Resources:StringDef, FileManagement %>" />
			<asp:HyperLink runat="server" NavigateUrl="AlertConfig.aspx" Text="<%$ Resources:StringDef, AlertConfig %>" />
		</td>
	</tr>
</table>

</asp:Content>

