<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameInfo_TaiSuiStatistic, App_Web_d99p0fdh" theme="BlazeSky" %>

<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td align="center" colspan="4">
			<asp:Label ID="literal3" runat="server" Text="<%$ Resources:StringDef, TaiSui %>" SkinID="Title" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />	
	<tr>
        <td align="center">
			<asp:Table ID="Table1" runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell Width="25%">
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" OnSelectedServerGroupChanged="ServerGroupIndexChanged" AutoPostBack="true"/>
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal runat="server" Text="<%$ Resources:StringDef,Type %>" />
				    </asp:TableHeaderCell>
                    <asp:TableCell>
                        <asp:RadioButtonList runat="server" ID="RadioButtonListType" RepeatDirection="Horizontal" >
                            <asp:ListItem Text="<%$ Resources:StringDef,Day %>" Selected="true" Value="Day" />
                            <asp:ListItem Text="<%$ Resources:StringDef,Month %>" Value="Month" />
                        </asp:RadioButtonList>
	                </asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal ID="Literal30" runat="server" Text="<%$ Resources:StringDef,StartDate %>" />
				    </asp:TableHeaderCell>
                    <asp:TableCell>
                        <Blaze:ScriptCalendar ID="StartDate" runat="server" ShowCheckBox="false" />
	                </asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal ID="Literal6" runat="server" Text="<%$ Resources:StringDef,EndDate %>" />
				    </asp:TableHeaderCell>
                    <asp:TableCell>
                        <Blaze:ScriptCalendar ID="EndDate" runat="server" ShowCheckBox="false" />	                    
	                </asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef,RoleName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <table border="0" cellspacing="0" width="100%">  
					        <tr>
					            <td width="60%" align="right">
					                <asp:TextBox ID="TextBoxRoleName" runat="server" CssClass="TextBox" />            
					            </td>
					            <td></td>
					        </tr>
					    </table>						
                    </asp:TableCell>
				</asp:TableRow>
			</asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
    <tr>
	    <td align="center">
	        <asp:Label runat="server" ID="LabelOpMsg" SkinID="Failure" />
	    </td>
	</tr>
	<tr>
		<td align="center">
            <asp:LinkButton ID="LinkButtonStatistic" runat="server" Text="<%$ Resources:StringDef,Statistic %>" OnClick="ButtonStatistic_Click" />
			<asp:HyperLink runat="server" NavigateUrl="Statistic.aspx" Text="<%$ Resources:StringDef, Return %>" />			
		</td>
	</tr>
	<tr class="TableRowSpacing" />
</table>
</contenttemplate>
</asp:UpdatePanel>

<table width="100%">
	<tr>
		<td align="center">
			<asp:UpdateProgress ID="UpdateProgress2" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
				<ProgressTemplate>	
				    <asp:Image ID="Image1" runat="server" SkinID="PleaseWait" />
					<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />
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
	    <td align="center">
			<asp:Label ID="LabelResult" runat="server" SkinID="Title" />
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
	    <td>
	        <ZGW:ZEDGRAPHWEB id="ZedGraphWebStatistic" runat="server" RenderMode="ImageTag" Width="840" Height="400" Visible="false" OnRenderGraph="ZedGraphWebTaiSuiStatistic_RenderGraph"  />
	    </td>
	</tr>
	<tr>
		<td>
		<asp:Table ID="TableSearchTaiSuiList" runat="server" />
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