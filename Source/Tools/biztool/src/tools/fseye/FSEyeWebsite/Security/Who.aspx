<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="Who.aspx.cs" Inherits="Security_Who" Title="<%$ Resources:StringDef, Who %>" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<table width="100%">
	<tr class="TableRowSpacing" />
	<tr>
		<td class="TableCaption"><asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, Who %>" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:Table ID="TableLoginUser" runat="server">
				<asp:TableHeaderRow>
					<asp:TableHeaderCell><asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, User %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef, HostAddress %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef, LoginTime %>" /></asp:TableHeaderCell>
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
						<asp:HyperLink runat="server" NavigateUrl="Security.aspx" Text="<%$ Resources:StringDef, Return %>" />
                    </td>
				</tr>
			</table>
		</td>
	</tr>
</table>
</asp:Content>

