<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Security_Security, App_Web_jztwgtrl" title="<%$ Resources:StringDef, Security %>" theme="BlazeSky" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<table width="100%">
	<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">						
						<asp:HyperLink runat="server" NavigateUrl="Who.aspx" Text="<%$ Resources:StringDef, Who %>" />
						<asp:HyperLink runat="server" NavigateUrl="ListUser.aspx" Text="<%$ Resources:StringDef, UserManagement %>" />
						<asp:HyperLink runat="server" NavigateUrl="ListGroup.aspx" Text="<%$ Resources:StringDef, GroupManagement %>" />
						<asp:HyperLink runat="server" NavigateUrl="Log.aspx" Text="<%$ Resources:StringDef, Log %>" />
					</td>
				</tr>
			</table>			
		</td>
	</tr>
</table>

</asp:Content>

