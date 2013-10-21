<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="PlayerMonitor.aspx.cs" Inherits="GameMaster_PlayerMonitor" Title="<%$ Resources:StringDef, PlayerMonitor %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<script type="text/javascript">
var updateRoleSkillInfoTipAjax = new sack("../GameMaster/GameMasterHandler.ashx");
var updatingRoleSkillInfo = false;

function init() {
}

function updateRoleSkillInfo(skillTemplateId) {
	if (!updatingRoleSkillInfo)
	{
		updatingRoleSkillInfo = true;		
		updateRoleSkillInfoTipAjax.reset();
		updateRoleSkillInfoTipAjax.element = "DivRoleSkillInfo";
		updateRoleSkillInfoTipAjax.runAJAX("op=GetRoleSkillInfo&skillTemplateId="+skillTemplateId);
		updateRoleSkillInfoTipAjax.onCompletion = new Function("updatingRoleSkillInfo=false;TagToTip('DivRoleSkillInfo',BALLOON, true,BORDERWIDTH, 0,OPACITY,85,BORDERCOLOR ,'#000000'  );");		
	}
}
</script>

<div id="DivRoleSkillInfo" style="display:none;" ></div>

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>
<table width="100%">
	<tr class="TableRowSpacing" />
	<tr>
		<td class="TableCaption">
			<asp:Literal runat="server" Text="<%$ Resources:StringDef, PlayerMonitor %>" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:Table ID="Table1" runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" AutoPostBack="true" OnSelectedServerGroupChanged="SelectedServerGroupChanged" />
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, RoleName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <table border="0" cellspacing="0" width="100%">
					        <tr>
					            <td width="60%" align="right">
					                <asp:TextBox ID="TextBoxRole" runat="server" CssClass="TextBox" />    
					            </td>
					            <td></td>
					        </tr>
					    </table>						
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, StartTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<Blaze:ScriptCalendar ID="ScriptCalendarStartTime" runat="server" ShowCheckBox="false" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, EndTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<Blaze:ScriptCalendar ID="ScriptCalendarEndTime" runat="server" ShowCheckBox="false" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, PlayerAction %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <asp:Panel runat="server" HorizontalAlign="Left">
						<asp:CheckBoxList ID="CheckBoxListPlayerAction" runat="server" RepeatLayout="Table" RepeatColumns="4" RepeatDirection="Horizontal" TextAlign="Right" align="center" />
						</asp:Panel>
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal10" runat="server" Text="<%$ Resources:StringDef, OrderBy %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxOrderBy" runat="server" Rows="1">
							<asp:ListItem Text="<%$ Resources:StringDef, Time %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, PlayerAction %>" Value="1" />
						</asp:ListBox>
						<asp:ListBox ID="ListBoxOrderByType" runat="server" Rows="1">
							<asp:ListItem Text="<%$ Resources:StringDef, OrderASC %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, OrderDESC %>" Value="1" />
						</asp:ListBox>
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
    <tr>
        <td align="center">
            <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
        </td>
    </tr>
    <tr>
        <td align="center">
            <asp:LinkButton ID="LinkButtonSearchRole" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Query %>" OnClick="LinkButtonSearchRole_Click" />
			<asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="GameMaster.aspx" Text="<%$ Resources:StringDef, Return %>" />
        </td>
	</tr>
</table>    
</contenttemplate>
</asp:UpdatePanel>
<table width="100%">
	<tr>
		<td align="center">
			<asp:UpdateProgress ID="UpdateProgress1" runat="server" AssociatedUpdatePanelID="UpdatePanelResult">			    
				<ProgressTemplate>				
					<asp:Label runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />					
				</ProgressTemplate>
			</asp:UpdateProgress>
        </td>
    </tr>
</table>
<asp:UpdatePanel runat="server" ID="UpdatePanelResult">
<contenttemplate>
<table width="100%">	
	<asp:Panel ID="PanelResult" runat="server" Visible="false">
	<tr>
		<td align="right">
			<table cellpadding="0">
				<tr>
					<td>
						<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, RecordPerPage %>" />
						<asp:ListBox ID="ListBoxRecordPerPage" runat="server" Rows="1">
							<asp:ListItem Text="20" Value="20" />
							<asp:ListItem Text="50" Value="50" />
							<asp:ListItem Text="100" Value="100" />
						</asp:ListBox>
						<asp:Button ID="ButtonFirstPage" runat="server" Text="<%$ Resources:StringDef, FirstPage %>" OnClick="ButtonFirstPage_Click" Enabled="false" />
						<asp:Button ID="ButtonPreviousPage" runat="server" Text="<%$ Resources:StringDef, PreviousPage %>" OnClick="ButtonPreviousPage_Click" Enabled="false" />
						<asp:Button ID="ButtonNextPage" runat="server" Text="<%$ Resources:StringDef, NextPage %>" OnClick="ButtonNextPage_Click" Enabled="false" />					
					</td>
				</tr>
			</table>
		</td>
	</tr>	
	<tr>
		<td>
		<asp:Table ID="TableC2SNetMsg" runat="server">
			<asp:TableHeaderRow>				
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, ID %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Time %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="Protocol" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Desc %>" /></asp:TableHeaderCell>
			</asp:TableHeaderRow>
		</asp:Table>
		</td>
	</tr>
	</asp:Panel>
	<tr class="TableRowSpacing" />
</table>
</contenttemplate>
</asp:UpdatePanel>
<ajax:UpdatePanelAnimationExtender ID="upae" BehaviorID="animation" runat="server" TargetControlID="UpdatePanelResult">
    <Animations>
        <OnUpdating>
            <Sequence>
                <%-- Disable all the controls --%>
                <Parallel duration="0">
                    <EnableAction AnimationTarget="LinkButtonSearchRole" Enabled="false" />               
                </Parallel>
                
                <StyleAction Attribute="overflow" Value="hidden" />
                
                <%-- Do each of the selected effects --%>
                <Parallel duration=".3" Fps="30">                    
                    <Condition ConditionScript="true">
                        <Resize Height="0" />
                    </Condition>
                </Parallel>
            </Sequence>
        </OnUpdating>
        <OnUpdated>
            <Sequence>
                <%-- Do each of the selected effects --%>
                <Parallel Duration=".3" Fps="30">
                    <Condition ConditionScript="true">
                        <%-- Get the stored height --%>
                        <Resize AnimationTarget="UpdatePanelResult" HeightScript="500" Unit="px" />
                    </Condition>
                </Parallel>
                <Parallel duration="0">
                    <EnableAction AnimationTarget="LinkButtonSearchRole" Enabled="true" />              
                </Parallel>
                <StyleAction duration="0" Attribute="overflow" Value="visible" />
            </Sequence>
        </OnUpdated>
    </Animations>
</ajax:UpdatePanelAnimationExtender>
</asp:Content>