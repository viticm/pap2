<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" CodeFile="UpdateManagement.aspx.cs" Inherits="UpDateManagement_UpdateManagement" Title="UpdateManagement" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<table width="100%">
	<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">
                        <asp:HyperLink runat="server" NavigateUrl="UpdateServerConfig.aspx" Text="<%$ Resources:StringDef, UpdateServerConfig %>" />
						<asp:HyperLink runat="server" NavigateUrl="UploadFiles.aspx" Text="<%$ Resources:StringDef, UploadFile %>" />
					</td>
				</tr>
			</table>			
		</td>
	</tr>
</table>
</asp:Content>