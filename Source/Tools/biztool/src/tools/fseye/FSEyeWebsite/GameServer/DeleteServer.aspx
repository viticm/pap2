<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="DeleteServer.aspx.cs" Inherits="GameServer_DeleteServer" Title="<%$ Resources:StringDef, RemoveServer %>" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<script type="text/javascript">
function init() {
	updateGameServerDesc();
}

function updateGameServerDesc() {
	var serverIdText = document.getElementById("GameServerId").getAttribute("value");	
	document.all.ButtonDeleteGameServer.setAttribute("href", "DeleteServer.aspx?noPromot=true&serverId=" + serverIdText);
	if (serverIdText) {		
		ajax = new sack("GameServerHandler.ashx");
		ajax.element = "GameServerDesc";
		ajax.runAJAX("op=GetGameServerDesc&serverId=" + serverIdText);
	}
}

function deleteConfirm()
{
	if (document.all.CanDelete && document.all.CanDelete.getAttribute("value") == "true")	{
		return window.confirm("<%=Resources.StringDef.DeleteGameServerConfirm %>");
	}
	else {
		window.alert("<%=Resources.StringDef.SpecifyGameServer %>");
		return false;
	}
}

</script>

<table width="100%">
	<tr>
		<td align="center">
			<table border="1" class="TableMedium">
				<caption class="TableCaption"><asp:Literal ID="literal1" runat="server" Text="<%$ Resources:StringDef, DeleteGameServer %>" /></caption>
				<tr class="TableRowNormal">
					<td width="100"><asp:Literal ID="literal10" runat="server" Text="<%$ Resources:StringDef, ID %>" /></td>
					<td><input type="text" id="GameServerId" class="TextBox" onkeyup="updateGameServerDesc();" value='<%=GameServerId %>' /></td>
				</tr>
				<tr class="TableRowNormal">
					<td><asp:Literal ID="literal2" runat="server" Text="<%$ Resources:StringDef, DeleteGameServer %>" /></td>
					<td id="GameServerDesc"></td>
				</tr>
			</table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />	
	<tr>
	    <td width="100%" align="center">
	        <asp:Label runat="server" ID="LabelOpMsg" />
	    </td>
	</tr>
	<tr class="TableRowSpacing" />	
	<tr>
		<td align="center">
            <a href="#" id="ButtonDeleteGameServer" class="LB" onclick="return deleteConfirm();"><asp:Literal ID="literal3" runat="server" Text="<%$ Resources:StringDef, Delete %>" /></a>
            <asp:HyperLink runat="server" NavigateUrl="AddAndDelete.aspx" Text="<%$ Resources:StringDef, Return %>" />            
		</td>		
	</tr>
</table>
</asp:Content>

