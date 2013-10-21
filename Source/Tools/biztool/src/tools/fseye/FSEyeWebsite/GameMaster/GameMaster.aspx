<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="GameMaster.aspx.cs" Inherits="GameMaster_GameMaster" Title="<%$ Resources:StringDef, GameMaster %>" %>
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

