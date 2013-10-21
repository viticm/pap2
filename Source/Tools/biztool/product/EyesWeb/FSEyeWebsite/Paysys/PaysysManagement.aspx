<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Paysys_PaysysManagement, App_Web_ipk8xfvi" title="<%$ Resources:StringDef, PaysysManagement %>" theme="BlazeSky" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<table width="100%">
	<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">						
						<asp:HyperLink runat="server" NavigateUrl="AccountInfo.aspx" Text="<%$ Resources:StringDef, AccountInfo %>" />
						<asp:HyperLink runat="server" NavigateUrl="AccountPassword.aspx" Text="<%$ Resources:StringDef, GatewayInfo %>" />
						<asp:HyperLink runat="server" NavigateUrl="AccountState.aspx" Text="<%$ Resources:StringDef, AccountState %>" />
						<asp:HyperLink runat="server" NavigateUrl="GatewayInfo.aspx" Text="<%$ Resources:StringDef, GatewayInfo %>" />						
					</td>
				</tr>
			</table>			
		</td>
	</tr>
</table>
</asp:Content>

