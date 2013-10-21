<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Security_EditGroup, App_Web_jztwgtrl" title="<%$ Resources:StringDef, EditGroup %>" theme="BlazeSky" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, EditGroup %>" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<asp:Table runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, GroupName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxGroupName" runat="server" Enabled="false" MaxLength="16" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server"
							ControlToValidate="TextBoxGroupName"
							Display="Dynamic"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, FieldRequired %>">
						</asp:RequiredFieldValidator>
						<asp:RegularExpressionValidator ID="RegularExpressionValidator1" runat="server"
							ControlToValidate="TextBoxGroupName"
							Display="Dynamic"
							SetFocusOnError="true"							
							ValidationExpression="[a-zA-Z]*"
							ErrorMessage="<%$ Resources:StringDef, UserNameLegalFormat %>">							
						</asp:RegularExpressionValidator>
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal6" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxComment" runat="server" MaxLength="32" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator5" runat="server" 							
							ControlToValidate="TextBoxComment"
							Display="Dynamic"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, FieldRequired %>">
						</asp:RequiredFieldValidator>
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="Literal15" runat="server" Text="<%$ Resources:StringDef, User %>" />
		</td>
	</tr>
	<tr>	
		<td>
			<asp:Table ID="TableUserInGroup" runat="server">
				<asp:TableHeaderRow>
					<asp:TableHeaderCell><asp:Literal ID="Literal12" runat="server" Text="<%$ Resources:StringDef, Operation %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef, Index %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal ID="Literal8" runat="server" Text="<%$ Resources:StringDef, ID %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal ID="Literal9" runat="server" Text="<%$ Resources:StringDef, UserName %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal ID="Literal11" runat="server" Text="<%$ Resources:StringDef, Comment %>" /></asp:TableHeaderCell>
				</asp:TableHeaderRow>
			</asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">
						<asp:LinkButton ID="LinkButtonModify" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Modify %>" OnClick="LinkButtonModify_Click" />
						<asp:HyperLink ID="HyperLinkEditPrivilege" runat="server" NavigateUrl="ListGroup.aspx" Text="<%$ Resources:StringDef, EditPrivilege %>" />
						<asp:HyperLink ID="HyperLinkAddUserToGroup" runat="server" NavigateUrl="ListGroup.aspx" Text="<%$ Resources:StringDef, AddUserToGroup %>" />
						<asp:HyperLink ID="HyperLink3" runat="server" NavigateUrl="ListGroup.aspx" Text="<%$ Resources:StringDef, Return %>" />
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

</asp:Content>

