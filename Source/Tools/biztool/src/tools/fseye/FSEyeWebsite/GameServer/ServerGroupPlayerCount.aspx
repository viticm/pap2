<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="ServerGroupPlayerCount.aspx.cs" Inherits="GameServer_ServerGroupPlayerCount" Title="<%$ Resources:StringDef,PlayerCount %>" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<script type="text/javascript">

var updateServerGroupPlayerCountAjax = new sack("GameServerHandler.ashx");
var updatingServerGroupPlayerCount = false;

function init() {
    updateServerGroupPlayerCount();
    setInterval(updateServerGroupPlayerCount, 15000);
}

function updateServerGroupPlayerCount() {
	if (!updatingServerGroupPlayerCount)
	{
		updatingServerGroupPlayerCount = true;
		updateServerGroupPlayerCountAjax.reset();
		updateServerGroupPlayerCountAjax.element = "DivServerGroupPlayerCount";
		updateServerGroupPlayerCountAjax.runAJAX("op=GetServerGroupPlayerCount&groupId=" + "<%= GetGroupId %>");
		updateServerGroupPlayerCountAjax.onCompletion = new Function("updatingServerGroupPlayerCount=false;");		
	}
}
</script>

<table width="100%">
	<tr>
		<td align="center"><asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, PlayerCount %>" SkinID="Title" /></td>
	</tr>
	<tr>
		<td>
			<div id="DivServerGroupPlayerCount" style="width: 100%" class="DivNormal"></div>
		</td>
	</tr>
</table>
</asp:Content>

