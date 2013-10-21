<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="GameInfo.aspx.cs" Inherits="GameInfo_GameInfo" Title="<%$ Resources:StringDef, GameInfo %>" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<table width="100%">
	<tr>
		<td align="center">
            <asp:HyperLink runat="server" NavigateUrl="TopPlayer.aspx" Text="<%$ Resources:StringDef, TopPlayer %>" />
			<asp:HyperLink runat="server" NavigateUrl="Statistic.aspx" Text="<%$ Resources:StringDef, Statistic %>" />			
		</td>
	</tr>
</table>

</asp:Content>