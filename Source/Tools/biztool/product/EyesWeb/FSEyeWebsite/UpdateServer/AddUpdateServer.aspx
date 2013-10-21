<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="UpDateManagement_AddUpdateServer, App_Web_hm9xpbcd" title="AddUpdateServer" theme="BlazeSky" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">
<asp:UpdatePanel id="UpdatePanel1" runat="server">
<ContentTemplate>  
<table width="100%">
    	<tr>
		<td align="center">
			<table border="1" class="TableMedium">
				<caption class='TableCaption'><asp:Literal ID="Literal11" runat="server" Text="<%$ Resources:StringDef, UpdateServer %>" /></caption>
                <tr class="TableRowNormal">
					<td>
						<asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, UpdateServerType %>" />
					</td>
					<td>
                        <asp:DropDownList ID="UpdateServerTypeDropDownList" runat="server" />
					</td>
				</tr>	                
                <tr class="TableRowNormal">
					<td>
						<asp:Literal ID="Literal8" runat="server" Text="<%$ Resources:StringDef, UpdateServerName %>" />
					</td>
					<td>
						<asp:TextBox ID="TextBoxServerComment" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" 							
							ControlToValidate="TextBoxServerComment"
							Display="Dynamic"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, FieldRequired %>">
						</asp:RequiredFieldValidator>
					</td>
				</tr>	
                <tr class="TableRowNormal">
					<td>
						<asp:Literal ID="Literal7" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
					</td>
					<td>
						<asp:TextBox ID="TextBoxServerIpAddress" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server" 							
							ControlToValidate="TextBoxServerIpAddress"
							Display="Dynamic"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, FieldRequired %>">
						</asp:RequiredFieldValidator>
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td>
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, Port %>" />
					</td>
					<td>
						<asp:TextBox ID="TextBoxServerPort" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator4" runat="server" 							
							ControlToValidate="TextBoxServerPort"
							Display="Dynamic"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, FieldRequired %>">
						</asp:RequiredFieldValidator>
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td>
						<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, UserName %>" />
					</td>
					<td>
						<asp:TextBox ID="TextBoxServerUserName" runat="server" CssClass="TextBox" />						
					</td>
				</tr>	
				<tr class="TableRowNormal">
					<td>
						<asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef, Password %>" />
					</td>
					<td>
						<asp:TextBox ID="TextBoxServerPassword" runat="server" CssClass="TextBox" TextMode="Password" MaxLength="16" />
						<ajax:PasswordStrength ID="PasswordStrength1" runat="server" TargetControlID="TextBoxServerPassword"
							DisplayPosition="RightSide" 
							StrengthIndicatorType="BarIndicator"
							PreferredPasswordLength="12"
							StrengthStyles="PasswordStrengthBarIndicator_weak;PasswordStrengthBarIndicator_average;PasswordStrengthBarIndicator_good"
							BarBorderCssClass="PasswordStrengthBarBorder"
							MinimumNumericCharacters="1"
							MinimumSymbolCharacters="1"
							TextStrengthDescriptions="Very Poor;Weak;Average;Strong;Excellent"
							RequiresUpperAndLowerCaseCharacters="true" />											
						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" 							
							ControlToValidate="TextBoxServerPassword"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender1" TargetControlID="RequiredFieldValidator2" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td>
						<asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef, PasswordConfirm %>" />
					</td>
					<td>
						<asp:TextBox ID="TextBoxPasswordConfirm" runat="server" TextMode="Password" MaxLength="16" CssClass="TextBox" />

						<asp:RequiredFieldValidator ID="RequiredFieldValidator5" runat="server"
							ControlToValidate="TextBoxPasswordConfirm"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender2" TargetControlID="RequiredFieldValidator5" />

						<asp:CompareValidator ID="CompareValidator1" runat="server" 
							ControlToValidate="TextBoxPasswordConfirm"
							ControlToCompare="TextBoxServerPassword"
							Display="None"
							Type="String"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RetypePasswordValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCallout1" TargetControlID="CompareValidator1" />
					</td>
				</tr>			
			</table>
		</td>
	</tr>
	<tr class="TableRowNormal" />
	<tr>
	    <td align="center">
		    <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
	    </td>
    </tr>
	<tr>	
	    <td align="center">
	    <table cellpadding="0" cellspacing="0">
	    <tr>
			<td align="center">
                <asp:LinkButton ID="AddUpdateServerLinkButton" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef,Add %>" OnClick="Add_Click" />
				<asp:HyperLink runat="server" NavigateUrl="UpdateServerConfig.aspx" Text="<%$ Resources:StringDef, Return %>" />				
	        </td>
	    </tr>
	    </table>
	    </td>	    
	</tr>
</table>
</ContentTemplate>
</asp:UpdatePanel>
</asp:Content>