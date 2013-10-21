<%@ control language="C#" autoeventwireup="true" inherits="Common_GameServerFilter, App_Web_ygkqysx-" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>

<table width="100%">
	<tr>
		<td align="left">
			<asp:ListBox ID="ListBoxFilter" runat="server" Rows="1" OnSelectedIndexChanged="ListBoxFilter_SelectedIndexChanged" AutoPostBack="true"/>
		</td>
		<td>
			<asp:Panel ID="PanelNullFilter" runat="server" Visible="false">
			</asp:Panel>
			<asp:Panel ID="PanelServerTypeFilter" runat="server" Visible="false">
				<asp:DropDownList ID="DropDownListServerType" runat="server" OnSelectedIndexChanged="DropDownListServerType_SelectedIndexChanged" AutoPostBack="true">
					<asp:ListItem Text="<%$ Resources:StringDef, ServerTypeUnidentified %>" Value="0" />
					<asp:ListItem Text="<%$ Resources:StringDef, ServerTypeGameServer %>" Value="1" />
					<asp:ListItem Text="<%$ Resources:StringDef, ServerTypeDatabaseServer %>" Value="2" />
					<asp:ListItem Text="<%$ Resources:StringDef, ServerTyperBishop %>" Value="3" />
					<asp:ListItem Text="<%$ Resources:StringDef, ServerTyperGoddess %>" Value="4" />
					<asp:ListItem Text="<%$ Resources:StringDef, ServerTyperGameCenter %>" Value="5" />
					<asp:ListItem Text="<%$ Resources:StringDef, ServerTyperPaysys %>" Value="6" />
				</asp:DropDownList>
			</asp:Panel>
			<asp:Panel ID="PanelConnectionStatusFilter" runat="server" Visible="false">
				<asp:CheckBox ID="CheckBoxConnected" runat="server" Checked="true" Text="<%$ Resources:StringDef, Connected %>" OnCheckedChanged="CheckBoxConnected_CheckedChanged" AutoPostBack="true" />
			</asp:Panel>
			<asp:Panel ID="PanelGameServerGroupFilter" runat="server" Visible="false">
				<Blaze:ServerGroupDropDownList id="ServerGroupDropDownList" runat="server" OnSelectedServerGroupChanged="ServerGroupDropDownList_SelectedServerGroupChanged" AutoPostBack="true"/>
			</asp:Panel>
			<asp:Panel ID="PanelKeywordFilter" runat="server" Visible="false">
				<table>
					<tr>
						<td><asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, Keyword %>" /></td>
						<td><asp:TextBox ID="TextBoxKeyword" runat="server" /></td>
						<td><asp:LinkButton ID="ButtonFilterByKeyword" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, DoFilter %>" OnClick="ButtonFilterByKeyword_Click" /></td>
					</tr>
				</table>
			</asp:Panel>
		</td>
	</tr>
</table>
