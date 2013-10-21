<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="DeleteUser.aspx.cs" Inherits="Security_DeleteUser" Title="<%$ Resources:StringDef, DeleteUser %>" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, DeleteUser %>" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<asp:Table runat="server" SkinID="Medium">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, UserName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxUserName" runat="server" MaxLength="16" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server"
							ControlToValidate="TextBoxUserName"
							Display="Dynamic"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, FieldRequired %>">
						</asp:RequiredFieldValidator>
						<asp:RegularExpressionValidator ID="RegularExpressionValidator1" runat="server"
							ControlToValidate="TextBoxUserName"
							Display="Dynamic"
							SetFocusOnError="true"							
							ValidationExpression="[a-zA-Z]*"
							ErrorMessage="<%$ Resources:StringDef, UserNameLegalFormat %>">							
						</asp:RegularExpressionValidator>
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">						
						<asp:LinkButton ID="LinkButtonDelete" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Delete %>" OnClick="LinkButtonDelete_Click" />
						<asp:HyperLink runat="server" NavigateUrl="ListUser.aspx" Text="<%$ Resources:StringDef, Return %>" />						
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

</asp:Content>

