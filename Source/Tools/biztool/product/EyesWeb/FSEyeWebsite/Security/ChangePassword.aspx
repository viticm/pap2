<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Security_ChangePassword, App_Web_jztwgtrl" title="<%$ Resources:StringDef,ChangePassword %>" theme="BlazeSky" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel runat="server" ID="UpdatePanel1">
<ContentTemplate>
<table width="100%">
	<tr>
		<td align="center">
			<asp:Table ID="Table1" runat="server" SkinID="Medium">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="LabelOldPass" runat="server" Text="<%$ Resources:StringDef,OldPassword %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxOldPassword" runat="server" TextMode="Password" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Label1" runat="server" Text="<%$ Resources:StringDef,NewPassword %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxNewPassword" runat="server" TextMode="Password" />
						<ajax:PasswordStrength ID="PasswordStrength2" runat="server" TargetControlID="TextBoxNewPassword"
							DisplayPosition="RightSide" 
							StrengthIndicatorType="BarIndicator"
							PreferredPasswordLength="12"
							StrengthStyles="PasswordStrengthBarIndicator_weak;PasswordStrengthBarIndicator_average;PasswordStrengthBarIndicator_good"
							BarBorderCssClass="PasswordStrengthBarBorder"
							MinimumNumericCharacters="1"
							MinimumSymbolCharacters="1"
							TextStrengthDescriptions="Very Poor;Weak;Average;Strong;Excellent"
							RequiresUpperAndLowerCaseCharacters="true" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Label2" runat="server" Text="<%$ Resources:StringDef,NewPasswordConfirm %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxNewPasswordConfirm" runat="server" TextMode="Password" />
						<asp:CompareValidator ID="CompareValidator1" ControlToValidate="TextBoxNewPasswordConfirm" ControlToCompare = "TextBoxNewPassword" Type="String" Display="None" runat="server" ErrorMessage="<%$ Resources:StringDef, RetypePasswordValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCallout1" TargetControlID="CompareValidator1" />						
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>
		</td>
	</tr>
	<tr>
        <td align="center">
            <asp:Label ID="LabelMsg" runat="server" />
        </td>
    </tr>
    <tr>
		<td>
			<table align="center" cellpadding="0">
				<tr>
					<td align="center">
                        <asp:LinkButton ID="LinkButtonModify" runat="server" Text="<%$ Resources:StringDef, Modify %>" OnClick="LinkButtonModify_Click" />
					    <asp:HyperLink ID="HyperLinkReturn" runat="server" Text="<%$ Resources:StringDef, Cancel %>" NavigateUrl="~/Common/HomePage.aspx" />
					</td>
				</tr>
			</table>
        </td>
    </tr>
</table>
</ContentTemplate>
</asp:UpdatePanel>
</asp:Content>