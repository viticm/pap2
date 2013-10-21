<%@ page language="C#" autoeventwireup="true" inherits="Common_SystemLogout, App_Web_1ceayx6e" theme="BlazeSky" %>
 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head id="Head1" runat="server">
    <title><%=Resources.StringDef.SystemFullName %></title>
    <link href="../App_Themes/BlazeSky/Main.css" rel="stylesheet" type="text/css" />
</head>
<body>
<form id="form1" runat="server">
	<table width="100%" border="0" cellpadding="0" cellspacing="0">
		<tr height="100">		
			<td>
				<asp:Panel runat="server" SkinID="LoginTop" />
			</td>
		</tr>
		<tr height="150">
		</tr>
		<tr>
			<td align="center">				
					<table>						
						<tr class="TableRowSpacing" />
						<tr>
							<td align="center">							    
								<asp:Label ID="LabelInfo" runat="server" ></asp:Label>
							</td>
						</tr>
						<tr>
						    <td><br/></td>
						</tr>
						<tr>
							<td colspan="2" align="center">
								<asp:Button ID="ButtonLogout" runat="server" OnClick="ButtonLogout_Click" TabIndex="1"
									Width="80" Text="<%$ Resources:StringDef, Logout %>" />
							</td>
						</tr>
					</table>
			</td>
		</tr>
		<tr height="150">
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
