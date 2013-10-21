<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="AddUser.aspx.cs" Inherits="Security_AddUser" Title="<%$ Resources:StringDef, AddUser %>" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<table width="100%">
	<tr class="TableCaption">
		<td>
			<asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, AddUser %>" />
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
						<asp:TextBox ID="TextBoxUserName" runat="server" MaxLength="16" CssClass="TextBox" onmouseover="<%$ Resources:StringDef, ShowToolTipUserName %>" />
						
						<ajax:FilteredTextBoxExtender ID="FilteredTextBoxExtender1" runat="server" TargetControlID="TextBoxUserName" FilterType="LowercaseLetters, UppercaseLetters" />

						<asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server"
							ControlToValidate="TextBoxUserName"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender7" TargetControlID="RequiredFieldValidator1" />
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
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender5" TargetControlID="RequiredFieldValidator2" />
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
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender4" TargetControlID="RequiredFieldValidator5" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef, Password %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxPassword" runat="server" TextMode="Password" MaxLength="16" CssClass="TextBox" onmouseover="<%$ Resources:StringDef, ShowToolTipPassword %>" />

						<ajax:PasswordStrength ID="PasswordStrength1" runat="server" TargetControlID="TextBoxPassword"
							DisplayPosition="RightSide" 
							StrengthIndicatorType="BarIndicator"
							PreferredPasswordLength="12"
							StrengthStyles="PasswordStrengthBarIndicator_weak;PasswordStrengthBarIndicator_average;PasswordStrengthBarIndicator_good"
							BarBorderCssClass="PasswordStrengthBarBorder"
							MinimumNumericCharacters="1"
							MinimumSymbolCharacters="1"
							TextStrengthDescriptions="Very Poor;Weak;Average;Strong;Excellent"
							RequiresUpperAndLowerCaseCharacters="true" />
							
						<ajax:FilteredTextBoxExtender ID="FilteredTextBoxExtender2" runat="server" TargetControlID="TextBoxPassword" FilterType="LowercaseLetters, UppercaseLetters, Custom, Numbers" ValidChars="!@#$%_-+" />
						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator4" runat="server" 							
							ControlToValidate="TextBoxPassword"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender1" TargetControlID="RequiredFieldValidator4" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef, PasswordConfirm %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxPasswordConfirm" runat="server" TextMode="Password" MaxLength="16" CssClass="TextBox" />

						<asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server"
							ControlToValidate="TextBoxPasswordConfirm"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender2" TargetControlID="RequiredFieldValidator3" />

						<asp:CompareValidator ID="CompareValidator1" runat="server" 
							ControlToValidate="TextBoxPasswordConfirm"
							ControlToCompare="TextBoxPassword"
							Display="None"
							Type="String"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RetypePasswordValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCallout1" TargetControlID="CompareValidator1" />
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
						<asp:HyperLink runat="server" NavigateUrl="ListUser.aspx" Text="<%$ Resources:StringDef, Return %>" />						
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

</asp:Content>

