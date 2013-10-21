<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="ExpStatistic.aspx.cs" Inherits="GameInfo_ExpStatistic" Title="<%$ Resources:StringDef, ExpStatistic %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>

<asp:Content ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td align="center">			
			<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, ExpStatistic %>" SkinID="Title" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<asp:Table runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" AutoPostBack="true" OnSelectedServerGroupChanged="ServerGroupIndexChanged" />
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, RoleName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <table width="100%" cellspacing="0">
					        <tr>
					            <td align="right" width="60%">
					                <asp:TextBox ID="TextBoxRoleName" runat="server" CausesValidation="false" />            
					            </td>
					            <td></td>
					        </tr>
					    </table>						
						<%--
						<asp:RequiredFieldValidator ID="RequiredFieldValidator5" runat="server"
							ControlToValidate="TextBoxRoleName"
							Display="Dynamic"							 
							SetFocusOnError="true"					        
							ErrorMessage="<%$ Resources:StringDef, FieldRequired %>">
						</asp:RequiredFieldValidator>
						--%>
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, StartTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ScriptCalendar runat="server" ID="StartDate" ShowCheckBox="false" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, EndTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ScriptCalendar runat="server" ID="EndDate" ShowCheckBox="false" />						
					</asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, ViewType %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxViewContent" runat="server" Rows="1">
							<asp:ListItem Text="<%$ Resources:StringDef, ExperienceCurve %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, LevelCurve %>" Value="1" />
						</asp:ListBox>
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>
		</td>
	</tr>
    <tr>
		<td align="center">
			<asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
		</td>
	</tr>
	<tr>
		<td align="center">
            <asp:LinkButton ID="LinkButtonStatistic" runat="server" Text="<%$ Resources:StringDef, Statistic %>" OnClick="ButtonStatistic_Click" />
			<asp:HyperLink runat="server" NavigateUrl="Statistic.aspx" Text="<%$ Resources:StringDef, Return %>" />			
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

<table width="100%">
	<tr class="TableRowSpacing" />	
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
			<ZGW:ZEDGRAPHWEB id="ZedGraphWebExpStatistic" runat="server" RenderMode="ImageTag" Width="840" Height="400" Visible="false" OnRenderGraph="ZedGraphWebExpStatistic_RenderGraph"  />
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>
<ajax:UpdatePanelAnimationExtender ID="upae" BehaviorID="animation" runat="server" TargetControlID="UpdatePanelResult">
    <Animations>
        <OnUpdating>
            <Sequence>
                <%-- Disable all the controls --%>
                <Parallel duration="0">
                    <EnableAction AnimationTarget="LinkButtonStatistic" Enabled="false" />               
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
                    <EnableAction AnimationTarget="LinkButtonStatistic" Enabled="true" />              
                </Parallel>
                <StyleAction duration="0" Attribute="overflow" Value="visible" />
            </Sequence>
        </OnUpdated>
    </Animations>
</ajax:UpdatePanelAnimationExtender>
</asp:Content>