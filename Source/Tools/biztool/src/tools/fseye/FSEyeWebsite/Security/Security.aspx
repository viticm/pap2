<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="Security.aspx.cs" Inherits="Security_Security" Title="<%$ Resources:StringDef, Security %>" %>
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

