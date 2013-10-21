<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameMaster_GameMaster, App_Web_6lmf-asn" title="<%$ Resources:StringDef, GameMaster %>" theme="BlazeSky" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<table width="100%">
	<tr>
		<td align="center">
            <asp:HyperLink runat="server" NavigateUrl="QueryRole.aspx" Text="<%$ Resources:StringDef, QueryRole %>" />
            <asp:HyperLink runat="server" NavigateUrl="GMCommand.aspx" Text="<%$ Resources:StringDef, GMCommand %>" />
            <asp:HyperLink runat="server" NavigateUrl="MessageCenter.aspx" Text="<%$ Resources:StringDef, MessageCenter %>" />
            <asp:HyperLink runat="server" NavigateUrl="PlayerWho.aspx" Text="<%$ Resources:StringDef, OnlinePlayer %>" />
            <asp:HyperLink runat="server" NavigateUrl="ItemExchange.aspx" Text="<%$ Resources:StringDef, ItemExchange %>" />
            <asp:HyperLink runat="server" NavigateUrl="ItemFlow.aspx" Text="<%$ Resources:StringDef, ItemFlow %>" />
            <asp:HyperLink runat="server" NavigateUrl="SendMail.aspx" Text="<%$ Resources:StringDef, SendMail %>" />
            <asp:HyperLink runat="server" NavigateUrl="ChatMonitor.aspx" Text="<%$ Resources:StringDef, ChatMonitor %>" />
		</td>
	</tr>
</table>

</asp:Content>

