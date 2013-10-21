<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="AdvancedPlayerCount.aspx.cs" Inherits="GameServer_AdvancedPlayerCount" Title="<%$ Resources:StringDef, AdvancedPlayerCount %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">

				

<table width="100%" align="center">

<tr>
<td align="center">
<asp:UpdatePanel id="UpdatePanel1" runat="server" UpdateMode="Conditional">
<contenttemplate>
<table width="100%">
	<tr>
		<td align="center">
			<asp:Label runat="server" Text="<%$ Resources:StringDef, AdvancedPlayerCount %>" SkinID="title" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<asp:Table runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell Width="40%">
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell Width="60%">
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" AutoPostBack="true" OnSelectedServerGroupChanged="ServerGroupIndexChanged" />
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" />
					</asp:TableCell>
				</asp:TableRow>			
			</asp:Table>
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>
</td>
</tr>

<tr>
<td align="center">
<asp:UpdatePanel id="UpdatePanel4" runat="server" UpdateMode="Conditional">
<contenttemplate>
<table width="100%">
<tr>
    <td>
        <asp:Table runat="server">
            <asp:TableRow>
	            <asp:TableHeaderCell Width="40%">
		            <asp:Literal runat="server" Text="<%$ Resources:StringDef, StartTime %>" />
	            </asp:TableHeaderCell>
	            <asp:TableCell Width="60%">
	                <Blaze:ScriptCalendar runat="server" ID="CalendarStartTime" ShowCheckBox="false" />						
	            </asp:TableCell>
            </asp:TableRow>
            <asp:TableRow>
	            <asp:TableHeaderCell Width="40%">
		            <asp:Literal runat="server" Text="<%$ Resources:StringDef, EndTime %>" />
	            </asp:TableHeaderCell>
	            <asp:TableCell Width="60%">
	                <Blaze:ScriptCalendar runat="server" ID="CalendarEndTime" ShowCheckBox="false" />
	            </asp:TableCell>
            </asp:TableRow>
        </asp:Table>
    </td>
</tr>
</table>	
</contenttemplate>
</asp:UpdatePanel>
</td>
</tr>

</table>


<asp:UpdatePanel id="UpdatePanel3" runat="server" UpdateMode="Conditional">
<contenttemplate>
<asp:Table ID="Table1" runat="server">

				</asp:Table>
				</contenttemplate>
				</asp:UpdatePanel>

<asp:UpdatePanel runat="server" ID="UpdatePanel2" UpdateMode="Conditional">
<contenttemplate>
<table width="100%">			
<tr>
	<td align="center">
	    <asp:LinkButton ID="ButtonShow" runat="server" Text="<%$ Resources:StringDef, Show %>" OnClick="ButtonShow_Click" />
		<asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="Serverlist.aspx" Text="<%$ Resources:StringDef, Return %>" />			
	</td>
</tr>
<tr>
    <td align="center">
        <asp:Label runat="server" ID="LabelOpMsg" SkinID="Failure" />
    </td>
</tr>
</table>

<table width="100%">	
	<tr>
		<td align="center">
		    <asp:UpdateProgress ID="UpdateProgress2" runat="server">
		    	<ProgressTemplate>	
		    		<asp:Image ID="Image1" runat="server" SkinID="PleaseWait" />
		    		<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />
		    	</ProgressTemplate>
		    </asp:UpdateProgress>
        </td>
    </tr>
</table>
</contenttemplate>
</asp:UpdatePanel>


<asp:UpdatePanel runat="server" ID="UpdatePanelResult">
<contenttemplate>
<table width="100%">
    <tr class="TableRowSpacing" />
	<tr>
		<td align="center">			
			<ZGW:ZEDGRAPHWEB id="ZedGraphWebPlayerCount" runat="server" RenderMode="ImageTag" Width="840" Height="450" OnRenderGraph="ZedGraphWebPlayerCount_RenderGraph" Visible="false" />
		</td>
	</tr>	
</table>
</contenttemplate>
</asp:UpdatePanel>

<ajax:UpdatePanelAnimationExtender ID="upae" BehaviorID="animation" runat="server" TargetControlID="UpdatePanelResult">
    <Animations>
        <OnUpdating>
            <Sequence>
                <%-- Disable all the controls 
                <Parallel duration="0">
                    <EnableAction AnimationTarget="ButtonShow" Enabled="false" />               
                </Parallel>--%>
                
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
                <%--<Parallel duration="0">
                    <EnableAction AnimationTarget="ButtonShow" Enabled="true" />              
                </Parallel>--%>
                <StyleAction duration="0" Attribute="overflow" Value="visible" />
            </Sequence>
        </OnUpdated>
    </Animations>
</ajax:UpdatePanelAnimationExtender>  


</asp:Content>