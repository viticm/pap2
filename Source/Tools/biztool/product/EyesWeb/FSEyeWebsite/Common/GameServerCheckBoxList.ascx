<%@ control language="C#" autoeventwireup="true" inherits="Common_GameServerCheckBoxList, App_Web_s7ebi-lz" %>
<%@ Register TagPrefix="Blaze" TagName="ServerFilter" Src="~/Common/GameServerFilter.ascx" %>

<asp:Panel ID="PanelBorder" runat="server" CssClass="TableMedium">
<table border="1" width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, GameServerList %>" />
		</td>
	</tr>
	<tr class="TableRowNormal">
		<td>
			<Blaze:ServerFilter ID="ServerFilter1" runat="server" OnFilterChanged="ServerFilter1_FilterChanged" />
		</td>
	</tr>
	<tr align="left">
		<td class="TableCellList">
			<asp:Panel ID="PanelGameServer" runat="server" ScrollBars="Vertical">
				<asp:Label ID="LabelNoGameServer" runat="server" Text="<%$ Resources:StringDef, None %>" />
				<asp:CheckBoxList ID="CheckBoxListGameServer" runat="server" RepeatColumns="1" RepeatLayout="Table" />
			</asp:Panel>
		</td>
	</tr>
	<tr class="TableRowNormal">
		<td align="left">
			<asp:Label ID="LabelSummary" runat="server" />
		</td>
		
	</tr>	
		<tr class="TableRowNormal">
		<td align="left">
			<asp:Label ID="ServerSelectedSummary" runat="server" />
		</td>
		
	</tr>	
	
	<tr class="TableRowNormal">
		<td align="center">
			<asp:LinkButton ID="LinkButtonSelectAllServers" runat="server" Text="<%$ Resources:StringDef, SelectAll %>" OnClick="ButtonSelectAllServers_Click" />
            <asp:LinkButton ID="LinkButtonSelectNoServer" runat="server" Text="<%$ Resources:StringDef, SelectNone %>" OnClick="ButtonSelectNoServer_Click" />
		</td>
	</tr>
</table>
</asp:Panel>
