<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_AddAndDelete, App_Web_jvyc5ygz" title="<%$ Resources:StringDef, AddAndRemoveServer %>" theme="BlazeSky" %>

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

