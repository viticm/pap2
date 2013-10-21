<%@ Page Language="C#" AutoEventWireup="true" CodeFile="SystemLogin.aspx.cs" Inherits="Common_SystemLogin" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title><%=Resources.StringDef.SystemFullName %></title>
    <link href="../App_Themes/BlazeSky/Main.css" rel="stylesheet" type="text/css" />
    <%--
    <script type="text/javascript" >
        self.resizeTo(window.screen.availWidth,window.screen.availHeight);
        self.moveTo(0,0);
    </script>
     --%>
</head>
<body>
<form id="form1" runat="server">

	<ajax:ToolkitScriptManager runat="server" EnablePartialRendering="true" ID="ScriptManager1" />

	<table width="100%" border="0" cellpadding="0" cellspacing="0" bgcolor="#E9F5F7">
		<tr height="100">		
			<td>
				<asp:Panel runat="server" SkinID="LoginTop" />
			</td>
		</tr>
		<tr height="350">
			<td>
				<table width="100%" height="100%" border="0">
					<tr>
						<td align="right" valign="middle" width="50%">
							<asp:Panel runat="server" SkinID="LoginWelcome">
								<table width="100%" height="100%" border="0">
									<tr>
										<td align="right">
											<asp:Label runat="server" Text="<%$ Resources:StringDef, SystemFullName %>" SkinID="LoginSystemName" />
										</td>
									</tr>
									<tr>
										<td align="right">
											<asp:Label runat="server" Text="<%$ Resources:StringDef, WelcomeLogin %>" SkinID="WelcomeLogin" />
										</td>
									</tr>
								</table>
							</asp:Panel>
						</td>
						<td align="center">
							<asp:Image runat="server" SkinID="LoginSeparator" />
						</td>
						<td>
							<table>
								<tr>
									<td align="center">
										<asp:Label ID="LabelLoginError" runat="server" ForeColor="#FF0000" Text="<%$ Resources:StringDef, UserNameOrPasswordError %>" Visible="false" />
									</td>
								</tr>
								<tr>
									<td>
										<table>
											<tr class="TableRowSpacing" />
											<tr>
												<td align="right">
													<asp:Label ID="Label3" runat="server" Font-Bold="true" Text="<%$ Resources:StringDef, UserName %>" />
												</td>
												<td align="left">
													<asp:TextBox ID="TextBoxUserName" runat="server" TabIndex="1" MaxLength="16" Width="130"
														CssClass="TextBox"></asp:TextBox>
													<asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" ControlToValidate="TextBoxUserName"
														Display="None" SetFocusOnError="true" ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
													<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender1" TargetControlID="RequiredFieldValidator1" />
												</td>
											</tr>
											<tr>
												<td align="right">
													<asp:Label ID="Label4" runat="server" Font-Bold="true" Text="<%$ Resources:StringDef, Password %>" />
												</td>
												<td align="left">
													<asp:TextBox ID="TextBoxPassword" runat="server" TabIndex="2" MaxLength="16" Width="130"
														CssClass="TextBox" TextMode="Password"></asp:TextBox>
													<asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" ControlToValidate="TextBoxPassword"
														Display="None" SetFocusOnError="true" ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
													<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender2" TargetControlID="RequiredFieldValidator2" />
												</td>
											</tr>
											<tr>
												<td colspan="2" align="center">
													<asp:Button ID="ButtonLogin" runat="server" OnClick="ButtonLogin_Click" TabIndex="3"
														Width="80" Text="<%$ Resources:StringDef, Login %>" />
												</td>
											</tr>
										</table>
									</td>
								</tr>
							</table>							
						</td>
					</tr>
				</table>
			</td>		
		</tr>
		<tr>
			<td valign="middle" class="Copyright">
				<asp:Panel runat="server" SkinID="Copyright">
					<table height="100%">
						<tr>
							<td>								
								<asp:Label ID="Label1" runat="server" Text='<%$ Resources:StringDef, CopyrightInfo %>' SkinID="Copyright" />
								<asp:Image runat="server" SkinID="BlazeLogo" />
								<asp:Label runat="server" Text='<%$ Resources:StringDef, BlazeProduction %>' SkinID="Copyright" />
							</td>
						</tr>
					</table>
				</asp:Panel>
			</td>
		</tr>
	</table>
</form>	
</body>
</html>
