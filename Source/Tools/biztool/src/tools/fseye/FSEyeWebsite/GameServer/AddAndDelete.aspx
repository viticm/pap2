<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="AddAndDelete.aspx.cs" Inherits="GameServer_AddAndDelete" Title="<%$ Resources:StringDef, AddAndRemoveServer %>" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<table width="100%">
	<tr>
		<td align="center">
            <asp:HyperLink runat="server" NavigateUrl="AddServerGroupSimplified.aspx" Text="<%$ Resources:StringDef, AddServerGroupSimplified %>" />
            <asp:HyperLink runat="server" NavigateUrl="EditServerGroup.aspx" Text="<%$ Resources:StringDef, AddGameServerGroup %>" />
			<asp:HyperLink runat="server" NavigateUrl="DeleteServer.aspx" Text="<%$ Resources:StringDef, DeleteServer %>" />
			<asp:HyperLink runat="server" NavigateUrl="ServerList.aspx" Text="<%$ Resources:StringDef, Return %>" />						
		</td>
	</tr>
</table>
</asp:Content>

