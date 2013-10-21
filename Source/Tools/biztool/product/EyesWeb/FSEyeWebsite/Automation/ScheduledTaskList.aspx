<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Automation_ScheduledTaskList, App_Web_4eiilbni" title="<%$ Resources:StringDef, ScheduledTaskList %>" theme="BlazeSky" %>
<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td align="center">
			<asp:Label runat="server" Text="<%$ Resources:StringDef, ScheduledTaskList %>" SkinID="Title" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<table width="65%">
				<tr>
					<td align="right" colspan="2">
						<asp:ListBox ID="ListBoxScheduledTask" runat="server" Width="100%" Height="200" SelectionMode="Multiple" AutoPostBack="true" OnSelectedIndexChanged="RefreshTaskDescription" />
					</td>
				</tr>
				<tr>
        		<td colspan="2" width="100%">
        		<asp:Panel ID="PannelTaskDes" runat="server" Visible="false" BorderWidth="1" BorderColor="Lavender">
        			<table width="100%" border="1">
        				<tr>
        					<td align="center" width="20%">
        						<asp:Literal ID="LiteralTaskState" runat="server" Text="<%$ Resources:StringDef,TaskState %>" />
        					</td>
        					<td align="center" width="20%">
        						<asp:Literal ID="LiteralTask" runat="server" Text="<%$ Resources:StringDef,TaskType %>" />
        					</td>
        					<td align="center" width="30%">
        						<asp:Literal ID="LiteralAutomation" runat="server" Text="<%$ Resources:StringDef,AutomationTask %>" />
        					</td>        					        					
        					<td align="center" width="30%">
        						<asp:Literal ID="LiteralTaskNextExecuteTime" runat="server" Text="<%$ Resources:StringDef,NextExecuteTime %>" />
        					</td>
        				</tr>
        				<tr>
        					<td align="center">
        						<asp:Label ID="LabelTaskState1" runat="server" />
        					</td>
        					<td align="center">
        						<asp:Literal ID="LiteralTask1" runat="server" />
        					</td>
                            <td align="center">
        						<asp:Literal ID="LiteralAutomation1" runat="server" />
        					</td>        					        					
        					<td align="center">
        						<asp:Literal ID="LiteralTaskNextExecuteTime1" runat="server" />
        					</td>
        				</tr>
        			</table>
                </asp:Panel>
        		</td>
        	    </tr>
        	    <tr class="TableRowSpacing" />
				<tr>
					<td align="left">
						<asp:Label ID="LabelSchedulerOn" runat="server" />
					</td>
					<td align="right">
						<asp:Label ID="LabelSchedulerSummary" runat="server" />					
					</td>
				</tr>
			</table>			
		</td>
	</tr>	
	<tr class="TableRowSpacing" />
    <tr>
	    <td align="center">
	        <asp:UpdateProgress ID="UpdateProgress1" runat="server" >
	            <ProgressTemplate>
	                <asp:Label ID="LabelWaiting" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileOperating %>" />
	            </ProgressTemplate>
	        </asp:UpdateProgress>
	    </td>
	</tr>
    <tr>
	    <td align="center">
            <asp:Label ID="LabelOpMsg" runat="server" />
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
		    <asp:LinkButton ID="ButtonSwitchSchedulerOn" runat="server" OnClick="ButtonSwitchSchedulerOn_Click" />
		    <asp:HyperLink runat="server" NavigateUrl="AddScheduledTask.aspx" Text="<%$ Resources:StringDef, Add %>" />
		    <asp:LinkButton ID="LinkButtonModify" runat="server" Text="<%$ Resources:StringDef, Modify %>" OnClick="LinkButtonModify_Click" />
		    <asp:LinkButton ID="LinkButtonEnable" runat="server" Text="<%$ Resources:StringDef, Enable %>" OnClick="LinkButtonEnable_Click" />
		    <asp:LinkButton ID="LinkButtonDisable" runat="server" Text="<%$ Resources:StringDef, Disable %>" OnClick="LinkButtonDisable_Click" />
		    <asp:LinkButton ID="LinkButtonDel" runat="server" Text="<%$ Resources:StringDef, Delete %>" OnClick="LinkButtonDel_Click" />
		    <ajax:ConfirmButtonExtender ID="ConfirmButtonExtender1" runat="server" TargetControlID="LinkButtonDel" ConfirmText="<%$ Resources:StringDef, ConfirmDeleteScheduledTask %>" />
		    <asp:LinkButton ID="LinkButtonExport" runat="server" Text="<%$ Resources:StringDef, Export %>" OnClick="LinkButtonExport_Click" />
		    <asp:LinkButton ID="LinkButtonImport" runat="server" Text="<%$ Resources:StringDef, Import %>" OnClick="LinkButtonImport_Click" />
		    <asp:HyperLink runat="server" NavigateUrl="Automation.aspx" Text="<%$ Resources:StringDef, Return %>" />
		</td>
	</tr>
</table>

</contenttemplate>
</asp:UpdatePanel>
</asp:Content>

