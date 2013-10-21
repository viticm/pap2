<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="BatchTask_BatchTask, App_Web_4eiilbni" title="<%$ Resources:StringDef, BatchTask %>" theme="BlazeSky" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>
<asp:Timer runat="server" Interval="2000" />
<asp:Table runat="server" ID="TableTask" />
<table width="100%">
	<tr>
		<td align="center">
			<asp:Label runat="server" ID="LabelNoTask" Visible="false" Text="<%$ Resources:StringDef, NoBatchTask %>" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<table cellpadding="0">
				<tr>
					<td align="center">												
						<asp:HyperLink runat="server" NavigateUrl="EditBatchTask.aspx" Text="<%$ Resources:StringDef, Add %>" />
						<asp:LinkButton runat="server" ID="ButtonDelete" Text="<%$ Resources:StringDef, Delete %>" OnClick="ButtonDelete_Click" />
						<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender1" runat="server" TargetControlID="ButtonDelete" ConfirmText="<%$ Resources:StringDef, ConfirmDeleteBatchTask %>" />
						<asp:LinkButton runat="server" ID="ButtonExecute" Text="<%$ Resources:StringDef, Execute %>" OnClick="ButtonExecute_Click" />
						<asp:LinkButton runat="server" ID="ButtonPause" Text="<%$ Resources:StringDef, Pause %>" OnClick="ButtonPause_Click" />
						<asp:LinkButton runat="server" ID="ButtonContinue" Text="<%$ Resources:StringDef, Continue %>" OnClick="ButtonContinue_Click" />
						<asp:HyperLink runat="server" NavigateUrl="Automation.aspx" Text="<%$ Resources:StringDef, Return %>" />
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>
</asp:Content>

