<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="UserGroup.aspx.cs" Inherits="Security_UserGroup" Title="<%$ Resources:StringDef, UserInGroup %>" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, User %>" /> <--> <asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef, Group %>" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<asp:Table runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, User %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:DropDownList ID="DropDownListUser" runat="server">
						</asp:DropDownList>
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, Group %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:DropDownList ID="DropDownListGroup" runat="server">
						</asp:DropDownList>
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align=center>
			<asp:Label ID="LabelMessage" runat="server" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">						
						<asp:LinkButton ID="LinkButtonAdd" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Add %>" OnClick="LinkButtonAdd_Click" />
						<asp:LinkButton ID="LinkButtonRemove" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Remove %>" OnClick="LinkButtonRemove_Click" />
						<asp:LinkButton ID="LinkButtonReturn" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Return %>" OnClick="LinkButtonReturn_Click" />						
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

</contenttemplate>
</asp:UpdatePanel>

</asp:Content>

