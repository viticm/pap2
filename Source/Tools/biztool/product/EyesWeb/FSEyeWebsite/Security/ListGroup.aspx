<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Security_ListGroup, App_Web_jztwgtrl" title="<%$ Resources:StringDef, Group %>" theme="BlazeSky" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="Literal8" runat="server" Text="<%$ Resources:StringDef, Group %>" />		
		</td>
	</tr>
	<tr>
		<td>
			<asp:Table ID="TableGroup" runat="server" CssClass="TN" CellSpacing="1">
				<asp:TableHeaderRow CssClass="TRH">
					<asp:TableHeaderCell CssClass="TCH"><asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, Operation %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell CssClass="TCH"><asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef, Index %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell CssClass="TCH"><asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, ID %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell CssClass="TCH"><asp:Literal ID="Literal6" runat="server" Text="<%$ Resources:StringDef, GroupName %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell CssClass="TCH"><asp:Literal ID="Literal7" runat="server" Text="<%$ Resources:StringDef, Comment %>" /></asp:TableHeaderCell>
				</asp:TableHeaderRow>
			</asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">						
						<asp:HyperLink runat="server" NavigateUrl="AddGroup.aspx" Text="<%$ Resources:StringDef, AddGroup %>" />
						<asp:HyperLink runat="server" NavigateUrl="Security.aspx" Text="<%$ Resources:StringDef, Return %>" />						
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

</asp:Content>

