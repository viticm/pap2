<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="EditPrivilege.aspx.cs" Inherits="Security_EditPrivilege" Title="<%$ Resources:StringDef, EditPrivilege %>" %>
<%@ Register TagPrefix="Blaze" TagName="SystemObjectTree" Src="~/Common/SystemObjectTree.ascx" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server" UpdateMode="Conditional">
<contenttemplate>

<table width="100%">
	<tr>
		<td align="center">
			<asp:Label ID="LabelTitle" runat="server" Text="<%$ Resources:StringDef, EditPrivilege %>" SkinID="Title" />
		</td>
	</tr>
    <tr>
	    <td>
	        <asp:UpdateProgress ID="UpdateProcess1" AssociatedUpdatePanelID="UpdatePanel1" runat="server" >
	            <ProgressTemplate> 
	                <asp:Label ID="UpdateProcessMeg" runat="server" Text="<%$ Resources:StringDef,PleaseWaitWhileOperating %>" />
	            </ProgressTemplate>
	        </asp:UpdateProgress>
	    </td>
	</tr>
	<tr>
		<td align="left">
			<asp:Label ID="LabelEditObject" runat="server" />
		</td>
	</tr>
	<tr>
		<td>
			<Blaze:SystemObjectTree ID="SystemObjectTree1" runat="server" OnSelectedObjectChanged="SystemObjectTree1_SelectedObjectChanged" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td>
		    <asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef,SelObj %>" /><asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef,Colon %>" /><asp:Literal runat="server" ID="LiteralSelectedNode" />
			<asp:Panel runat="server" ID="PanelAce" Enabled="False">
			<table>
				<tr>
					<td><asp:Label runat="server" Text="<%$ Resources:StringDef, Operation %>" /></td>
					<td><asp:CheckBox ID="CheckBoxAllowRead" runat="server" Text="<%$ Resources:StringDef, Read %>" /></td>
					<td><asp:CheckBox ID="CheckBoxAllowWrite" runat="server" Text="<%$ Resources:StringDef, Write %>" /></td>
					<td><asp:CheckBox ID="CheckBoxAllowExecute" runat="server" Text="<%$ Resources:StringDef, Execute %>" /></td>
				</tr>
				<tr>
				    <td colspan="4">
				        <asp:RadioButtonList ID="RadioButtonListApplyAll" runat="server" Enabled="false" >
				            <asp:ListItem Text="<%$ Resources:StringDef,ApplyAllChildNode %>" Selected="True" />
				            <asp:ListItem Text="<%$ Resources:StringDef,ApplyJustCurrNode %>" />
				        </asp:RadioButtonList>
				    </td>
				</tr>
			</table>
			</asp:Panel>
		</td>
	</tr>
	<tr>
		<td>
			<table cellpadding="0" cellspacing="0">
				<tr>
					<td align="center">
						<asp:LinkButton ID="ButtonModify" runat="server" Text="<%$ Resources:StringDef, Modify %>" OnClick="ButtonModify_Click" Enabled="True" />
						<asp:LinkButton ID="ButtonSave" runat="server" Text="<%$ Resources:StringDef, Save %>" OnClick="ButtonSave_Click" Enabled="False" />
						<asp:LinkButton ID="ButtonCancel" runat="server" Text="<%$ Resources:StringDef, Cancel %>" OnClick="ButtonCancel_Click" Enabled="False" />						
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

</contenttemplate>
</asp:UpdatePanel>

</asp:Content>