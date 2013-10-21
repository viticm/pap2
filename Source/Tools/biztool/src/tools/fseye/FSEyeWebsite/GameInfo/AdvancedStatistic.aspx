<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="AdvancedStatistic.aspx.cs" Inherits="GameInfo_AdvancedStatistic" Title="<%$ Resources:StringDef, AdvancedStatistic %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="SkillDropDownList" Src="~/Common/SkillDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td align="center">			
			<asp:Label runat="server" Text="<%$ Resources:StringDef, AdvancedStatistic %>" SkinID="Title" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<asp:Table runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell Width="20%">
						<asp:Literal ID="literal2" runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" OnSelectedServerGroupChanged="ServerGroupIndexChanged" AutoPostBack="true" />
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, RoleName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxRoleName" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, ViewContent %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxViewContent" runat="server" Rows="1" OnSelectedIndexChanged="ListBoxViewContent_SelectedIndexChanged" AutoPostBack="True">
							<asp:ListItem Text="经验" Value="0" />
							<asp:ListItem Text="金钱" Value="1" />
							<asp:ListItem Text="物品" Value="2" />
							<asp:ListItem Text="玩家死亡" Value="3" />
							<asp:ListItem Text="任务" Value="4" />
							<asp:ListItem Text="使用技能" Value="5" />
						</asp:ListBox>
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, ViewOptions %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <asp:Panel ID="PanelViewExpOptions" runat="server" >
						    <asp:Literal runat="server" Text="<%$ Resources:StringDef, None %>" />
						</asp:Panel>					    
						<asp:Panel ID="PanelViewMoneyOptions" runat="server" Visible="False">
							<asp:CheckBox ID="CheckBoxShowAddMoney" runat="server" Text="金钱增加" Checked="True" />
							<asp:CheckBox ID="CheckBoxShowRemoveMoney" runat="server" Text="金钱减少" />
							<asp:CheckBox ID="CheckBoxShowChangeMoney" runat="server" Text="金钱总量变化" />
						</asp:Panel>
						<asp:Panel ID="PanelViewItemOptions" runat="server" Visible="False">
						<table width="100%" rules="all">
							<tr>
								<td align="right">
									<asp:Label runat="server" Text="<%$ Resources:StringDef, ItemTemplateId %>" />
								</td>
								<td align="left">
									<asp:TextBox ID="TextBoxItemTemplateId" runat="server" />
								</td>
							</tr>
							<tr>
								<td colspan="2">
									<asp:CheckBoxList ID="CheckBoxListShowItemCount" runat="server" RepeatColumns="5" Width="100%" />
								</td>
							</tr>
							<tr>
								<td colspan="2" align="center">
                                    <asp:LinkButton ID="ButtonRefreshItemCount" runat="server" Text="<%$ Resources:StringDef, Refresh %>" OnClick="ButtonRefreshItemCount_Click" SkinID="SmallButton" />
									<asp:LinkButton ID="ButtonSelectAllItemCount" runat="server" Text="<%$ Resources:StringDef, SelectAll %>" OnClick="SelectAllItemCount_Click" SkinID="SmallButton" />
									<asp:LinkButton ID="ButtonSelectNoneItemCount" runat="server" Text="<%$ Resources:StringDef, SelectNone %>" OnClick="SelectNoneItemCount_Click" SkinID="SmallButton" />
								</td>
							</tr>
						</table>
						</asp:Panel>
						<asp:Panel ID="PanelViewPlayerDeathOptions" runat="server" Visible="False">
							<asp:CheckBox ID="CheckBoxShowPlayerKill" runat="server" Text="被玩家杀死" Checked="True" />
							<asp:CheckBox ID="CheckBoxShowNpcKill" runat="server" Text="被NPC杀死" Checked="True" />
						</asp:Panel>
						<asp:Panel ID="PanelViewQuestOptions" runat="server" Visible="False">				
						<table width="100%" rules="all">
							<tr>
								<td align="right">
									<asp:Label runat="server" Text="<%$ Resources:StringDef, QuestId %>" />
								</td>
								<td align="left">
									<asp:TextBox ID="TextBoxQuestId" runat="server" />
								</td>
							</tr>
							<tr>
								<td colspan="2">
									<asp:CheckBoxList ID="CheckBoxListShowQuestCount" runat="server" RepeatColumns="3" Width="100%" />
								</td>
							</tr>
							<tr>
								<td colspan="2" align="center">
									<asp:LinkButton ID="ButtonRefreshQuestCount" runat="server" Text="<%$ Resources:StringDef, Refresh %>" OnClick="ButtonRefreshQuestCount_Click" SkinID="SmallButton" />									
								</td>
							</tr>
						</table>
						</asp:Panel>
						<asp:Panel ID="PanelViewSkillOptions" runat="server" Visible="false">
							<Blaze:SkillDropDownList ID="SkillDropDownList1" runat="server" MaxSkillPerPage="100" />
						</asp:Panel>
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, ViewType %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxViewType" runat="server" Rows="1" OnSelectedIndexChanged="ListBoxViewType_OnSelectedIndexChanged" AutoPostBack="True">
							<asp:ListItem Text="<%$ Resources:StringDef, Day %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, Month %>" Value="1" />							
						</asp:ListBox>
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, StartTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ScriptCalendar ID="StartDate" runat="server" ShowCheckBox="false" Enabled="true" />						
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, EndTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ScriptCalendar ID="EndDate" runat="server" ShowCheckBox="false" Enabled="true" />						
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal5" runat="server" Text="<%$ Resources:StringDef, ChartType %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxChartType" runat="server" Rows="1" OnSelectedIndexChanged="ListBoxChartType_OnSelectedIndexChanged" AutoPostBack="True">
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
            <asp:LinkButton ID="LinkButtonStatistic" runat="server" Text="<%$ Resources:StringDef, Statistic %>" OnClick="LinkButtonStatistic_Click" />
	        <asp:HyperLink runat="server" NavigateUrl="Statistic.aspx" Text="<%$ Resources:StringDef, Return %>" />			
		</td>
	</tr>	
	<tr class="TableRowSpacing" />	
	<tr>
		<td align="center">
			<asp:Label ID="LabelOpMsg" runat="server" />
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

<table width="100%">
	<tr>
		<td align="center">
			<asp:UpdateProgress ID="UpdateProgress2" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
				<ProgressTemplate>	
					<asp:Image runat="server" SkinID="PleaseWait" />
					<asp:Label runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />
					<br />
					<br />
				</ProgressTemplate>
			</asp:UpdateProgress>
		</td>
	</tr>
</table>

<asp:UpdatePanel runat="server" ID="UpdatePanelResult">
<contenttemplate>
<table width="100%">
	<tr>
		<td align="center">			
			<ZGW:ZEDGRAPHWEB id="ZedGraphWebExpStatistic" runat="server" RenderMode="ImageTag" Width="840" Height="400" Visible="false" OnRenderGraph="ZedGraphWebExpStatistic_RenderGraph" />
		</td>
	</tr>
</table>

</contenttemplate>
</asp:UpdatePanel>

</asp:Content>