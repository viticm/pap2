<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Common_HomePage, App_Web_ygkqysx-" title="<%$ Resources:StringDef, SystemFullName %>" theme="BlazeSky" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
	<table width="100%">
		<tr>
			<td align="center">
				<table cellspacing="0" cellpadding="0">
					<tr>
						<td align="center">
							<asp:HyperLink runat="server" NavigateUrl="~/GameServer/ServerList.aspx" Text="<%$ Resources:StringDef, GameServer %>" />
							<asp:HyperLink runat="server" NavigateUrl="~/GameMaster/GameMaster.aspx" Text="<%$ Resources:StringDef, MasterGame %>" />
							<asp:HyperLink runat="server" NavigateUrl="~/GameInfo/GameInfo.aspx" Text="<%$ Resources:StringDef, GameInfo %>" />
							<asp:HyperLink runat="server" NavigateUrl="~/Automation/Automation.aspx" Text="<%$ Resources:StringDef, Automation %>" />
							<asp:HyperLink runat="server" NavigateUrl="~/Security/Security.aspx" Text="<%$ Resources:StringDef, Security %>" />
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>
</asp:Content>