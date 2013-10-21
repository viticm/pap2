<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Automation_EditBatchTask, App_Web_4eiilbni" title="<%$ Resources:StringDef, EditBatchTask %>" theme="BlazeSky" %>

<%@ Register TagPrefix="Blaze" TagName="GameServerCheckBoxList" Src="~/Common/GameServerCheckBoxList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="AutomationEditor" Src="~/Common/AutomationEditor.ascx" %>
<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">

	<asp:UpdatePanel id="UpdatePanel1" runat="server">
		<contenttemplate>
			<table width="100%" align="center" cellpadding="0" cellspacing="0">
				<tr>
					<td>
						<table width="100%" align="center" border="1">
						    <tr>
								<td align="right" class="TCH">
									<asp:Literal runat="server" Text="<%$ Resources:StringDef, Name %>" />
								</td>
								<td align="left">
									<asp:TextBox runat="server" ID="TextBoxName" />
								</td>
							</tr>
							<tr>
								<td align="right" class="TCH">
									<asp:Literal runat="server" Text="<%$ Resources:StringDef, Desc %>" />
								</td>
								<td align="left">
									<asp:TextBox runat="server" ID="TextBoxDesc" />
								</td>
							</tr>
							<tr>
								<td align="right" class="TCH">
									<asp:Literal runat="server" Text="<%$ Resources:StringDef, StepSize %>" />
								</td>
								<td align="left">
									<asp:DropDownList runat="server" ID="DropDownListStepSize">
										<asp:ListItem Text="1" Value="1" />
										<asp:ListItem Text="2" Value="2" />
										<asp:ListItem Text="3" Value="3" />
										<asp:ListItem Text="4" Value="4" />
										<asp:ListItem Text="5" Value="5" />
										<asp:ListItem Text="10" Value="10" />
										<asp:ListItem Text="20" Value="20" />
										<asp:ListItem Text="50" Value="50" />
										<asp:ListItem Text="100" Value="100" />
										<asp:ListItem Text="200" Value="200" />
										<asp:ListItem Text="500" Value="500" />
										<asp:ListItem Text="1000" Value="1000" />
									</asp:DropDownList>
								</td>
							</tr>
						</table>
					</td>
				</tr>
				<tr class="TableRowSpacing" />
				<tr>
					<td>
						<table width="100%" cellpadding="0" cellspacing="0">
							<tr>
								<td>
									<Blaze:GameServerCheckBoxList ID="GameServerCheckBoxList1" runat="server" RepeatColumns="2" GameServerPanelHeight="200" CssClass="TableNormal" />
								</td>
							</tr>
							<tr class="TableRowSpacing" />
							<tr>
								<td>
									<Blaze:AutomationEditor ID="AutomationEditor1" runat="server" />
								</td>
							</tr>
							<tr class="TableRowSpacing" />
							<tr>
								<td align="center">
									<asp:LinkButton runat="server" ID="LinkButtonApply" OnClick="LinkButtonApply_Click" />
									<asp:HyperLink runat="server" NavigateUrl="BatchTask.aspx" Text="<%$ Resources:StringDef, Return %>" />
								</td>
							</tr>
							<tr class="TableRowSpacing" />
							<tr>
							    <td align="center">
							        <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
							    </td>
							</tr>
						</table>
					</td>
				</tr>
			</table>
		</contenttemplate>
	</asp:UpdatePanel>
</asp:Content>
