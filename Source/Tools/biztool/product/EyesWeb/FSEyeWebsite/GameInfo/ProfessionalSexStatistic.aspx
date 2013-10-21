<%@ page language="C#" autoeventwireup="true" masterpagefile="~/Common/Main.master" inherits="GameInfo_ProfessionalSexStatistic, App_Web_d99p0fdh" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td align="center" colspan="4">
			<asp:Label ID="literal3" runat="server" Text="<%$ Resources:StringDef, ProfessionalSex %>" SkinID="Title" />
		</td>
	</tr>
    <tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:Table runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell Width="25%">
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" OnSelectedServerGroupChanged="ServerGroupIndexChanged" AutoPostBack="true"/>
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, Level %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxStartLevel" runat="server" Text="1" SkinID="Custom" Height="18px" />
						<ajax:NumericUpDownExtender ID="NumericUpDownExtenderStartLevel" runat="server" TargetControlID="TextBoxStartLevel" Width="60" Minimum="1" Maximum="120" Tag="1" />
						<asp:RangeValidator ID="RangeValidatorStartLevel" runat="server" ControlToValidate="TextBoxStartLevel" MinimumValue="1" MaximumValue="120" Type="Integer" ErrorMessage="<%$ Resources:StringDef,MessageLevelInvalid %>" Display="None" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtenderStartLevel" TargetControlID="RangeValidatorStartLevel"/>
						<asp:Literal runat="server" Text=" - " />
						<asp:TextBox ID="TextBoxEndLevel" runat="server" Text="120" SkinID="Custom" Height="18px" />
        				<ajax:NumericUpDownExtender ID="NumericUpDownExtenderEndLevel" runat="server" TargetControlID="TextBoxEndLevel" Width="60" Minimum="1" Maximum="120" Tag="1" />
						<asp:RangeValidator ID="RangeValidatorEndLevel" runat="server" ControlToValidate="TextBoxEndLevel" MinimumValue="1" MaximumValue="120" Type="Integer" ErrorMessage="<%$ Resources:StringDef,MessageLevelInvalid %>" Display="None" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender2" TargetControlID="RangeValidatorEndLevel"/>                             
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal runat="server" Text="<%$ Resources:StringDef,ActivePlayer %>" />
				    </asp:TableHeaderCell>
				    <asp:TableCell>				    
                        <asp:DropDownList ID="DropDownListActivePlayer" runat="server" OnSelectedIndexChanged="DropDownListActivePlayerSelectedIndexChanged"  AutoPostBack="true">
                        	<asp:ListItem Text="<%$ Resources:StringDef,AllPlayers %>" Selected="true" />
	                        <asp:ListItem Text="<%$ Resources:StringDef,ActivePlayer %>" />
	                        <asp:ListItem Text="<%$ Resources:StringDef,InActivePlayer %>" />
	                    </asp:DropDownList><br />
	                    <Blaze:ScriptCalendar ID="DateCalendar" runat="server" ShowCheckBox="false" Enabled="false" />
				    </asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal runat="server" Text="<%$ Resources:StringDef,SocialRelation %>" />
				    </asp:TableHeaderCell>
				    <asp:TableCell>
                        <asp:DropDownList ID="DropDownListSocialRelation" runat="server" >
                            <asp:ListItem Text="<%$ Resources:StringDef,AllPlayers %>" Selected="True" />
	                        <asp:ListItem Text="<%$ Resources:StringDef,PlayerInClan %>" />
	                        <asp:ListItem Text="<%$ Resources:StringDef,PlayerNotInClan %>" />
	                    </asp:DropDownList>
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
            <asp:LinkButton ID="Statistic" runat="server" OnClick="ButtonStatistic_Click" Text="<%$Resources:StringDef,Statistic %>"  />
			<asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="Statistic.aspx" Text="<%$ Resources:StringDef, Return %>" />			
        </td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

<table width="100%">
	<tr>
	    <td align="center">
        	<asp:UpdateProgress ID="UpdateProcess1" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
        	    <ProgressTemplate>
        	        <asp:Image runat="server" SkinID="PleaseWait" />
        	        <asp:Literal runat="server" Text="<%$Resources:StringDef,PleaseWaitWhileQuerying %>" />
        	    </ProgressTemplate>
        	</asp:UpdateProgress>
	    </td>
	</tr>
</table>


<asp:UpdatePanel runat="server" ID="UpdatePanelResult">
<contenttemplate>
<table width="100%">
	<asp:Panel ID="PanelResult" runat="server" Visible="false">
	<tr class="TableRowSpacing" />
	<tr>
	    <td align="center">
	        <asp:Label runat="server" ID="LabelResult" SkinID="Title" />
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td>
		<asp:Table ID="TableSearchRoleSexStatistic" runat="server">
			<asp:TableHeaderRow>
				<asp:TableHeaderCell ColumnSpan="2"><asp:Literal ID="literal5" runat="server" Text="<%$ Resources:StringDef,RoleClass %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef,Male %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef,Female %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell ColumnSpan="2"><asp:Literal ID="literal10" runat="server" Text="<%$ Resources:Stringdef,Total %>" /></asp:TableHeaderCell>
			</asp:TableHeaderRow>
		</asp:Table>
		</td>
	</tr>
    <tr>
	    <td>
	        <ZGW:ZEDGRAPHWEB id="ZedGraphWebProfsStat" runat="server" RenderMode="ImageTag" Width="840" Height="400" OnRenderGraph="ZedGraphWebProfsStat_RenderGraph"  Visible="false" />
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
                    <EnableAction AnimationTarget="Statistic" Enabled="false" />               
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
                    <EnableAction AnimationTarget="Statistic" Enabled="true" />              
                </Parallel>
                <StyleAction duration="0" Attribute="overflow" Value="visible" />
            </Sequence>
        </OnUpdated>
    </Animations>
</ajax:UpdatePanelAnimationExtender>
</asp:Content>