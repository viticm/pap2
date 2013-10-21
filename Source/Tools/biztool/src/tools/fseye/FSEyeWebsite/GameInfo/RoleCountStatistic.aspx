<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true"
	CodeFile="RoleCountStatistic.aspx.cs" Inherits="GameInfo_RoleCountStatistic" MaintainScrollPositionOnPostback="true" Title="<%$ Resources:StringDef, RoleCountStatistic %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderTopMessage" runat="server">
<asp:Label ID="LabelTopMessage" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" CssClass="MessageNormal" />
</asp:Content>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

	<table width="100%">
		<tr>
			<td align=center>
				<asp:Label runat="server" Text="<%$ Resources:StringDef, RoleCountStatistic %>" SkinID="Title" />
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
							<asp:Literal ID="literal4" runat="server" Text="<%$ Resources:StringDef, GroupMode %>" />
						</asp:TableHeaderCell>
						<asp:TableCell>
							<asp:ListBox ID="ListBoxStatType" runat="server" Rows="1">
								<asp:ListItem Text="<%$ Resources:StringDef, Level %>" Value="0" />
								<asp:ListItem Text="<%$ Resources:StringDef, RoleClass %>" Value="1" />
								<asp:ListItem Text="<%$ Resources:StringDef, Sex %>" Value="2" />
								<asp:ListItem Text="<%$ Resources:StringDef, Map %>" Value="3" />
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
					<asp:TableRow>
						<asp:TableHeaderCell>
							<asp:Literal ID="literal8" runat="server" Text="<%$ Resources:StringDef, Filter %>" />
						</asp:TableHeaderCell>
						<asp:TableCell>
							<asp:Table runat="server">
								<asp:TableRow>
									<asp:TableHeaderCell>
										<asp:Literal ID="literal9" runat="server" Text="<%$ Resources:StringDef, Level %>" />
									</asp:TableHeaderCell>
									<asp:TableCell>
										<asp:ListBox ID="ListBoxLevelFrom" runat="server" Rows="1" />
										<asp:Literal ID="literal14" runat="server" Text="<%$ Resources:StringDef, To %>" />
										<asp:ListBox ID="ListBoxLevelTo" runat="server" Rows="1" />
									</asp:TableCell>
								</asp:TableRow>
								<asp:TableRow>
									<asp:TableHeaderCell>
										<asp:Literal ID="literal10" runat="server" Text="<%$ Resources:StringDef, RoleClass %>" />
									</asp:TableHeaderCell>
									<asp:TableCell>
										<asp:ListBox ID="ListBoxRoleClass" runat="server" Rows="1" />
									</asp:TableCell>
								</asp:TableRow>
								<asp:TableRow>
									<asp:TableHeaderCell>
										<asp:Literal ID="literal11" runat="server" Text="<%$ Resources:StringDef, Sex %>" />
									</asp:TableHeaderCell>
									<asp:TableCell>
										<asp:ListBox ID="ListBoxSex" runat="server" Rows="1" />
									</asp:TableCell>
								</asp:TableRow>
								<asp:TableRow>
									<asp:TableHeaderCell>
										<asp:Literal ID="literal12" runat="server" Text="<%$ Resources:StringDef, Map %>" />
									</asp:TableHeaderCell>
									<asp:TableCell>
										<asp:ListBox ID="ListBoxMap" runat="server" Rows="1" />
									</asp:TableCell>
								</asp:TableRow>
							</asp:Table>
						</asp:TableCell>
					</asp:TableRow>
				</asp:Table>
			</td>
		</tr>
		<tr>
			<td align="center">
				<table cellspacing="0" cellpadding="0">
					<tr>
						<td>
							<ul>
								<li><asp:LinkButton ID="LinkButtonStatistic" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Statistic %>" OnClick="LinkButtonStatistic_Click" OnClientClick="showAndBlinkTopMessage();" /></li>
								<li><asp:HyperLink runat="server" NavigateUrl="Statistic.aspx" Text="<%$ Resources:StringDef, Return %>" /></li>
							</ul>
						</td>
					</tr>
				</table>
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
		<tr class="TableRowSpacing" />
		<tr>
			<td>
				<asp:Table ID="TableStatistic" runat="server" CssClass="TableNormal" GridLines="Both" Visible="False">
				</asp:Table>
			</td>
		</tr>
	</table>
	
</contenttemplate>
</asp:UpdatePanel>
	
</asp:Content>
