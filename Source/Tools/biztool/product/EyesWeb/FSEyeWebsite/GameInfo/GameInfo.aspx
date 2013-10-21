<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameInfo_GameInfo, App_Web_d99p0fdh" title="<%$ Resources:StringDef, GameInfo %>" theme="BlazeSky" %>
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