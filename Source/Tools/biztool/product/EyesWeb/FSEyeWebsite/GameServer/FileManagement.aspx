<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_FileManagement, App_Web_jvyc5ygz" title="<%$ Resources:StringDef, FileManagement %>" theme="BlazeSky" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<table width="100%">
	<tr>
		<td align="center">
            <asp:HyperLink runat="server" NavigateUrl="UploadFile.aspx" Text="<%$ Resources:StringDef, UploadFile %>" />
			<asp:HyperLink runat="server" NavigateUrl="FileList.aspx" Text="<%$ Resources:StringDef, FileList %>" />
			<asp:HyperLink runat="server" NavigateUrl="ServerList.aspx" Text="<%$ Resources:StringDef, Return %>" />
		</td>
	</tr>
</table>
</asp:Content>

