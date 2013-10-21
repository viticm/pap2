<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_ServerOverviewPlayerCount, App_Web_jvyc5ygz" title="<%$ Resources:StringDef,PlayerCount %>" theme="BlazeSky" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<script type="text/javascript">

var updateGameServerOverviewPlayerCountAjax = new sack("GameServerHandler.ashx");
var updatingGameServerOverviewPlayerCount = false;

function init() {
	updateGameServerOverviewPlayerCount();	
	setInterval(updateGameServerOverviewPlayerCount, 10000);
}

function updateGameServerOverviewPlayerCount() {
	if (!updatingGameServerOverviewPlayerCount)
	{
		updatingGameServerOverviewPlayerCount = true;
		updateGameServerOverviewPlayerCountAjax.reset();
		updateGameServerOverviewPlayerCountAjax.element = "DivGameServerOverviewPlayerCount";
		updateGameServerOverviewPlayerCountAjax.runAJAX("op=GetGameServerOverviewPlayerCount");
		updateGameServerOverviewPlayerCountAjax.onCompletion = new Function("updatingGameServerOverviewPlayerCount=false;");		
	}
}
</script>

<table width="100%">
	<tr>
		<td align="center"><asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, PlayerCount %>" SkinID="Title" /></td>
	</tr>
	<tr>
		<td>
			<div id="DivGameServerOverviewPlayerCount" style="width: 100%" class="DivNormal"></div>
		</td>
	</tr>
</table>

</asp:Content>