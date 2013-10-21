<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameInfo_EasyStatistic, App_Web_d99p0fdh" title="<%$ Resources:StringDef, EasyStatistic %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td align="center">
			<asp:Label ID="literal3" runat="server" Text="<%$ Resources:StringDef, EasyStatistic %>" SkinID="Title" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<asp:Table runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal2" runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" AutoPostBack="true" OnSelectedServerGroupChanged="ServerGroupIndexChanged" />
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal4" runat="server" Text="<%$ Resources:StringDef, Type %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxStatType" runat="server" Rows="1">
							<asp:ListItem Text="<%$ Resources:StringDef, RoleClass %>" Value="1" />
							<asp:ListItem Text="<%$ Resources:StringDef, Sex %>" Value="2" />
						</asp:ListBox>
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal5" runat="server" Text="<%$ Resources:StringDef, ChartType %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxChartType" runat="server" Rows="1">
							<asp:ListItem Text="<%$ Resources:StringDef, BarChart %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, LineChart %>" Value="1" />
						</asp:ListBox>
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>
		</td>
	</tr>
		<tr>
		<td align="center">
            <asp:LinkButton ID="LinkButtonStatistic" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Statistic %>" OnClick="LinkButtonStatistic_Click" />
			<asp:HyperLink runat="server" NavigateUrl="Statistic.aspx" Text="<%$ Resources:StringDef, Return %>" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />	
	<tr>
		<td align="center">
			<asp:UpdateProgress ID="UpdateProgress2" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
				<ProgressTemplate>	
					<asp:Label runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />
					<br />
					<br />
				</ProgressTemplate>
			</asp:UpdateProgress>
		</td>
	</tr>
	<tr>
		<td align="center">			
			<ZGW:ZEDGRAPHWEB id="ZedGraphWebStatistic" runat="server" RenderMode="ImageTag" Width="840" Height="400" Visible="false" OnRenderGraph="ZedGraphWebStatistic_RenderGraph" />
		</td>
	</tr>
</table>

</contenttemplate>
</asp:UpdatePanel>

</asp:Content>