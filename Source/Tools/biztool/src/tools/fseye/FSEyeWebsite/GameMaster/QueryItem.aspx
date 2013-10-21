<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="QueryItem.aspx.cs" Inherits="GameMaster_QueryItem" Title="Untitled Page" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<table width="100%">	
	<tr>
		<td></td>
	</tr>
	<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td>
						<ul>
							<li><asp:LinkButton ID="LinkButtonSearchRole" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Search %>" OnClientClick="showTopMessage();" /></li>
							<li><a href="GameMaster.aspx" class="LinkButton"><asp:Literal ID="Literal14" runat="server" Text="<%$ Resources:StringDef, Return %>" /></a></li>
						</ul>
					</td>
				</tr>
			</table>			
		</td>
	</tr>
</table>

</asp:Content>

