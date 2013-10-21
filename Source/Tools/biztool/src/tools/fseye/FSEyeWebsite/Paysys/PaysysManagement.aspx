<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="PaysysManagement.aspx.cs" Inherits="Paysys_PaysysManagement" Title="<%$ Resources:StringDef, PaysysManagement %>" %>

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

