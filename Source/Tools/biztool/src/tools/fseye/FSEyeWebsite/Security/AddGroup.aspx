<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="AddGroup.aspx.cs" Inherits="Security_AddGroup" Title="<%$ Resources:StringDef, AddGroup %>" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<table width="100%">
	<tr class="TableCaption">
		<td>
			<asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, AddGroup %>" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<asp:Table runat="server" SkinID="Medium">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, GroupName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxGroupName" runat="server" MaxLength="16" CssClass="TextBox" onmouseover="<%$ Resources:StringDef, ShowToolTipUserName %>" />
						
						<ajax:FilteredTextBoxExtender ID="FilteredTextBoxExtender1" runat="server" TargetControlID="TextBoxGroupName" FilterType="LowercaseLetters, UppercaseLetters" />
						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server"
							ControlToValidate="TextBoxGroupName"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender7" TargetControlID="RequiredFieldValidator1" />
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
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender1" TargetControlID="RequiredFieldValidator5" />
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
						<asp:LinkButton ID="LinkButtonAdd" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Add %>" OnClick="LinkButtonAdd_Click" />
						<asp:HyperLink runat="server" NavigateUrl="ListGroup.aspx" Text="<%$ Resources:StringDef, Return %>" />
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

</asp:Content>

