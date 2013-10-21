<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_AddAlertCondition, App_Web_jvyc5ygz" title="<%$ Resources:StringDef, AddAlertCondition %>" theme="BlazeSky" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderTopMessage" Runat="Server">
</asp:Content>
<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<table border="1" class="TableMedium">
				<caption class='TableCaption'><asp:Literal ID="Literal11" runat="server" Text="<%$ Resources:StringDef, AddAlertCondition %>" /></caption>
				<tr class="TableRowNormal">
					<td>						
						<asp:Literal ID="Literal6" runat="server" Text="<%$ Resources:StringDef, AlertCondition %>" />
					</td>
					<td>
						<asp:ListBox ID="ListBoxAlertCondition" runat="server" SelectionMode="Single" Rows="1" OnSelectedIndexChanged="ListBoxAlertCondition_SelectIndexChanged" AutoPostBack="true" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td>						
						<asp:Literal ID="LiteralParam1" runat="server" />
					</td>
					<td>
						<asp:TextBox ID="TextBoxParam1" runat="server" CssClass="TextBox" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td>						
						<asp:Literal ID="LiteralParam2" runat="server" />
					</td>
					<td>
						<asp:TextBox ID="TextBoxParam2" runat="server" CssClass="TextBox" />
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
	    <td width="100%" align="center">
	        <asp:Label runat="server" ID="LabelOpMsg" />
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:LinkButton ID="LinkButtonAdd" runat="server" CssClass="LinkButton" OnClick="LinkButtonAdd_Click" Text="<%$ Resources:StringDef, Add %>"/>
			<asp:HyperLink runat="server" NavigateUrl="AlertConfig.aspx" Text="<%$ Resources:StringDef, Return %>" />			
		</td>
	</tr>
	<tr class="TableRowSpacing"></tr>
</table>

</contenttemplate>
</asp:UpdatePanel>

</asp:Content>

