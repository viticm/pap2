<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="EditUser.aspx.cs" Inherits="Security_EditUser" Title="<%$ Resources:StringDef, EditUser %>" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server" UpdateMode="Conditional">
<contenttemplate>

<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, EditUser %>" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<asp:Table runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, UserName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxUserName" runat="server" Enabled="false" MaxLength="16" CssClass="TextBox" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, RealName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxRealName" runat="server" MaxLength="32" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" 							
							ControlToValidate="TextBoxRealName"
							Display="Dynamic"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, FieldRequired %>">
						</asp:RequiredFieldValidator>
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
				<asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal runat="server" Text="<%$ Resources:StringDef, OldPassword %>" />
				    </asp:TableHeaderCell>
				    <asp:TableCell>
				        <asp:TextBox ID="TextBoxOldPassword" runat="server" TextMode="Password" MaxLength="16" />
				    </asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef, NewPassword %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxPassword" runat="server" TextMode="Password" MaxLength="16" CssClass="TextBox" />
						<asp:RegularExpressionValidator ID="RegularExpressionValidator2" runat="server"
							ControlToValidate="TextBoxPassword"
							Display="Dynamic"
							SetFocusOnError="true"
							ValidationExpression="[0-9a-zA-Z]*"
							ErrorMessage="<%$ Resources:StringDef, InvalidFormat %>">							
						</asp:RegularExpressionValidator>
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef, NewPasswordConfirm %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxPasswordConfirm" runat="server" TextMode="Password" MaxLength="16" CssClass="TextBox" />
						<asp:CompareValidator ID="CompareValidator1" runat="server" 
							ControlToValidate="TextBoxPasswordConfirm"
							ControlToCompare="TextBoxPassword"
							Display="Dynamic"
							Type="String"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, PasswordConfirmFail %>">
						</asp:CompareValidator>	
					</asp:TableCell>
				    </asp:TableRow>
				    <%--
				    <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal7" runat="server" Text="<%$ Resources:StringDef, Group %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <asp:CheckBoxList ID="CheckBoxListGroup" runat="server"  />
					</asp:TableCell>
				    </asp:TableRow>
				     --%>
			</asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="Literal15" runat="server" Text="<%$ Resources:StringDef, Group %>" />		
		</td>
	</tr>
	<tr>	
		<td>
			<asp:Table ID="TableUserInGroup" runat="server">
				<asp:TableHeaderRow>
					<asp:TableHeaderCell><asp:Literal ID="Literal12" runat="server" Text="<%$ Resources:StringDef, Operation %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal ID="Literal10" runat="server" Text="<%$ Resources:StringDef, Index %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal ID="Literal11" runat="server" Text="<%$ Resources:StringDef, ID %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal ID="Literal13" runat="server" Text="<%$ Resources:StringDef, GroupName %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal ID="Literal14" runat="server" Text="<%$ Resources:StringDef, Comment %>" /></asp:TableHeaderCell>
				</asp:TableHeaderRow>
			</asp:Table>
		</td>
	</tr>
	<tr>
	    <td align="center">
	        <asp:Label runat="server" ID="LabelOpMsg" SkinID="Failure" />
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">						
						<asp:LinkButton ID="LinkButtonSave" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Save %>" OnClick="LinkButtonSave_Click" />
						<asp:HyperLink ID="HyperLinkEditPrivilege" runat="server" Text="<%$ Resources:StringDef, EditPrivilege %>" />
						<asp:HyperLink ID="HyperLinkAddUserToGroup" runat="server" Text="<%$ Resources:StringDef, AddUserToGroup %>" />
						<asp:HyperLink runat="server" NavigateUrl="ListUser.aspx" Text="<%$ Resources:StringDef, Return %>" />
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>
</asp:Content>