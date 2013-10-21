<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="StrategyList.aspx.cs" Inherits="GameServer_StrategyList" Title="<%$ Resources:StringDef, StrategyList %>" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr class="TableCaption">
		<td>
			<asp:Label ID="Label4" runat="server" Text="<%$ Resources:StringDef, StrategyList %>" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<table width="65%">
				<tr>
					<td align="right" colspan="2">
						<asp:ListBox ID="ListBoxStrategy" runat="server" Width="100%" Height="200" SelectionMode="Multiple" />
					</td>
				</tr>
				<tr>
					<td align="left">
						<asp:Label ID="LabelStrategyOn" runat="server" />
					</td>
					<td align="right">
						<asp:Label ID="LabelStrategySummary" runat="server" />					
					</td>
				</tr>
			</table>			
		</td>
	</tr>
	<tr class="TableRowSpacing" />
    <tr>
	    <td align="center">
            <asp:Label ID="LabelOpMsg" runat="server" />
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<table cellpadding="0" cellspacing="0">
				<tr>
					<td>						
						<asp:LinkButton ID="ButtonSwitchStrategyOn" runat="server" CssClass="LinkButton" OnClick="ButtonSwitchStrategyOn_Click" />
						<asp:HyperLink runat="server" NavigateUrl="AddStrategy.aspx" Text="<%$ Resources:StringDef, Add %>" />
						<asp:LinkButton ID="LinkButtonModify" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Modify %>" OnClick="ButtonModify_Click" />
						<asp:LinkButton ID="LinkButtonEnable" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Enable %>" OnClick="ButtonEnable_Click" />
						<asp:LinkButton ID="LinkButtonDisable" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Disable %>" OnClick="ButtonDisable_Click" />
						<asp:LinkButton ID="LinkButtonDel" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Delete %>" OnClick="ButtonDel_Click" />
						<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender1" runat="server" TargetControlID="LinkButtonDel" ConfirmText="<%$ Resources:StringDef, DeleteStrategyConfirm %>" />
						<asp:LinkButton ID="LinkButtonTestEvent" runat="server" CssClass="LinkButton" Text="´¥·¢²âÊÔÊÂ¼þ" OnClick="ButtonTestEvent_Click" />
						<asp:LinkButton ID="LinkButtonExport" runat="server" Text="<%$ Resources:StringDef, Export %>" OnClick="LinkButtonExport_Click" />
						<asp:LinkButton ID="LinkButtonImport" runat="server" Text="<%$ Resources:StringDef, Import %>" OnClick="LinkButtonImport_Click" />
						<asp:HyperLink runat="server" NavigateUrl="Automation.aspx" Text="<%$ Resources:StringDef, Return %>" />
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

</contenttemplate>
</asp:UpdatePanel>

<table width="100%">
	<tr>
	    <td align="center">
	        <asp:UpdateProgress ID="UpdateProgress1" runat="server" >
	            <ProgressTemplate>
	                <asp:Label ID="LabelWaiting" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileOperating %>" />
                </ProgressTemplate>
	        </asp:UpdateProgress>
	    </td>
	</tr>
</table>

</asp:Content>

