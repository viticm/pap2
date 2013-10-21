<%@ page language="C#" autoeventwireup="true" inherits="GameServer_ServerConfig, App_Web_jvyc5ygz" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ServerConfigTextBox" Src="~/Common/ServerConfigTextBox.ascx" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title></title>
</head>

<body>
    <form id="form1" runat="server">
       
			<table border="0" class="TN"  Width="100%" Height="100%">
			<tr>
			<td>
			<caption class="TableCaption"><asp:Literal ID="Literal11" runat="server" Text="<%$ Resources:StringDef,ShowServerConfig %>" /></caption>
			</td>
			</tr>
				<tr>
                <td align="center">
			           <Blaze:ServerConfigTextBox ID="ServerConfig" runat="server"    ></Blaze:ServerConfigTextBox>
					</td>
				</tr>		            	
                         <tr>
            <td align="center">
                <asp:Label ID="LabelSuccess" runat="server" SkinID="MessageSuccess" EnableViewState="false"
                    Visible="false"></asp:Label>
                <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure"></asp:Label>
            </td>
        </tr>
                  
                    <tr >
                    <td  align="center">
                    <asp:LinkButton ID="ButtonSaveFile" runat="server" Text="<%$ Resources:StringDef, Save %>" OnClick="ButtonSaveFile_Click"  />
                       <asp:LinkButton ID="closewin" runat="server" Text="<%$ Resources:StringDef, Return %>"  /> 
                    </td>
                    </tr>
                    
				
                    </table>   
    </form>
</body>
</html>
