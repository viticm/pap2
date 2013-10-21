<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="TopPlayer.aspx.cs" Inherits="GameInfo_TopPlayer" Title="<%$ Resources:StringDef, TopPlayer %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx"%>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>
<table width=100%>
	<tr>
		<td>
		    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" AutoPostBack="true" OnSelectedServerGroupChanged="ServerGroupIndexChanged" />
			<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" AutoPostBack="true"/>
		</td>
		<td>
		    <asp:LinkButton ID="LinkButtonStatistic" runat="server" OnClick="LinkButtonStatistic_Click" Text="<%$ Resources:StringDef,Statistic %>" Width="90%" />
		</td>
	</tr>
	<tr>
	    <td>
	        <asp:Label runat="server" ID="LabelOpMsg" SkinID="Failure" />   
	    </td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

<table width="100%">
	<tr>
		<td>
			<asp:UpdateProgress ID="UpdateProgress1" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
				<ProgressTemplate>
					<asp:Label ID="LabelTopMessage" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />
				</ProgressTemplate>
			</asp:UpdateProgress>
		</td>
	</tr>
</table>

<asp:UpdatePanel runat="server" ID="UpdatePanelTopPlayer" UpdateMode="Conditional">
<ContentTemplate>
<table width="100%">
	<tr>	
		<td align=center>			
			<asp:Label runat="server" Text="<%$ Resources:StringDef, TopPlayer %>" SkinID="Title" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td>
		<asp:Table ID="TableTopPlayer" runat="server" GridLines="Both" CssClass="TN">
			<asp:TableHeaderRow>				
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Place %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Level %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, RoleName %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Account %>" /></asp:TableHeaderCell>
				<%--
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Tong %>" /></asp:TableHeaderCell>				
				--%>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Sex %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, RoleClass %>" /></asp:TableHeaderCell>
			</asp:TableHeaderRow>
		</asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
</table>
</ContentTemplate>
<Triggers>
    <asp:AsyncPostBackTrigger ControlID="LinkButtonStatistic" EventName="Click" />
</Triggers>
</asp:UpdatePanel>

<ajax:UpdatePanelAnimationExtender ID="upaetopplayer" BehaviorID="animation" runat="server" TargetControlID="UpdatePanelTopPlayer">
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
                        <Resize AnimationTarget="UpdatePanelTopPlayer" HeightScript="300" Unit="px" />
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

<asp:UpdatePanel runat="server" ID="UpdatePanelTopPlutocrat" UpdateMode="Conditional">
<ContentTemplate>
<table width="100%">
<tr class="TableRowSpacing" />
	<tr>
		<td align="center">			
			<asp:Label runat="server" Text="<%$ Resources:StringDef, Top10Plutocrat %>" SkinID="Title" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td>
		<asp:Table ID="TableTopPlutocrat" runat="server" GridLines="Both" CssClass="TN">
			<asp:TableHeaderRow>				
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Place %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, TotalMoney %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, RoleName %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Account %>" /></asp:TableHeaderCell>
				<%--
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Tong %>" /></asp:TableHeaderCell>
				--%>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Level %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Sex %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, RoleClass %>" /></asp:TableHeaderCell>				
			</asp:TableHeaderRow>
		</asp:Table>
		</td>
	</tr>
</table>
</ContentTemplate>
<Triggers>
    <asp:AsyncPostBackTrigger ControlID="LinkButtonStatistic" EventName="Click" />
</Triggers>
</asp:UpdatePanel>
<ajax:UpdatePanelAnimationExtender ID="upaetopplutocrat" BehaviorID="animation1" runat="server" TargetControlID="UpdatePanelTopPlutocrat">
    <Animations>
        <OnUpdating>
            <Sequence>                
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
                        <Resize AnimationTarget="UpdatePanelTopPlutocrat" HeightScript="300" Unit="px" />
                    </Condition>
                </Parallel>
                <StyleAction duration="0" Attribute="overflow" Value="visible" />
            </Sequence>
        </OnUpdated>
    </Animations>
</ajax:UpdatePanelAnimationExtender>
</asp:Content>

