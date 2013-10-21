<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_ServerGroupManagement, App_Web_jvyc5ygz" title="<%$ Resources:StringDef, ServerGroupManagement %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<script type="text/javascript">

function getTreeViewSelectedNode()
{
    var selectedNodeId = ctl00_ContentPlaceHolderMain_TreeViewServerGroup_Data.selectedNodeID.value;
    var selectedNode = document.getElementById(selectedNodeId);
    return selectedNode;
}

function getDropDownListSelectedGroup()
{
    var select = document.getElementById('ctl00_ContentPlaceHolderMain_ServerGroupDropDownList_DropDownListServerGroup');
    
    if (select.selectedIndex != -1)
    {
        var selectedOption = select.options[select.selectedIndex];
        var selectedOptionOuterText = selectedOption.outerText;
        var groupName = selectedOptionOuterText.substr(1, selectedOptionOuterText.length - 1);
        return groupName;
    }
}

</script>

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>
<table width="100%">
	<tr class="TableRowSpacing" />	
	<tr>
		<td>
			<asp:TreeView ID="TreeViewServerGroup" runat="server" SkinID="ServerGroup" ExpandDepth="2" OnSelectedNodeChanged="TreeViewServerGroup_SelectedNodeChanged" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
	    <td>
	        <asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef,SelObj %>" /><asp:Literal ID="Literal6" runat="server" Text="<%$ Resources:StringDef,Colon %>" /><asp:Literal runat="server" ID="LiteralSelectedNode" />
	    </td>	    
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
        <td><asp:Label ID="LabelSuccess" runat="server" SkinID="MessageSuccess" /></td>
    </tr>    
    <tr>
        <td><asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" /></td>
    </tr>	   
	<tr class="TableRowSpacing" />
	<tr>
		<td>
			<asp:Panel ID="PanelCreateSeverGroup" runat="server" Enabled="false">
				<table>
					<tr>
						<td><asp:Literal runat="server" Text="<%$ Resources:StringDef, GroupName %>" /></td>
						<td><asp:TextBox ID="TextBoxServerGroupName" runat="server" Text="" /></td>
						<td><asp:Literal ID="Literal3" runat="server" Text="组名不得为空或空格" /></td>
					</tr>
					<tr>
						<td><asp:Literal runat="server" Text="<%$ Resources:StringDef, Comment %>" /></td>
						<td><asp:TextBox ID="TextBoxServerGroupComment" runat="server" Text="" /></td>
						<td><asp:Literal ID="Literal4" runat="server" Text="添加服务器组时，若不填内容，则其备注为当时的时间" /></td>
					</tr>
					<tr>
						<td><asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, Cabinet %>" /></td>
						<td><asp:DropDownList ID="DropDownListCabinet" runat="server" /></td>
						<td><asp:Literal ID="Literal5" runat="server" Text="此信息只对服务器组有效，包含服务器组所在机房、机柜的信息" /></td>
					</tr>
					<tr>
						<td colspan="2"><asp:Button ID="ButtonCreateServerGroup" runat="server" Text="<%$ Resources:StringDef, CreateServerGroup %>" OnClick="ButtonCreateServerGroup_Click" /></td>
					</tr>
				</table>
			</asp:Panel>
		</td>
	</tr>
	<tr>
	    <td>
	        <asp:Panel ID="PanelModifyServerGroup" runat="server" Enabled="false" >
	            <asp:Button ID="ButtonModifyServerGroup" runat="server" Text="修改服务器组" OnClick="ButtonModifyServerGroup_Click" OnClientClick="var selectedNode = getTreeViewSelectedNode();return window.confirm('确认修改服务器组 ' + selectedNode.outerText + ' 吗？');" />
	        </asp:Panel>
	    </td>
	</tr>
	<tr>
		<td>
			<asp:Panel ID="PanelDeleteServerGroup" runat="server" Enabled="false">
				<asp:Button ID="ButtonDeleteServerGroup" runat="server" Text="<%$ Resources:StringDef, DeleteServerGroup %>" OnClick="ButtonDeleteServerGroup_Click" OnClientClick="var selectedNode = getTreeViewSelectedNode();return window.confirm('确认要删除服务器组 ' + selectedNode.outerText + ' 吗？');" />
			</asp:Panel>
		</td>
	</tr>
	<tr>
		<td>
			<asp:Panel ID="PanelMoveToGroup" runat="server" Enabled="false">
				<asp:Button ID="ButtonMoveToGroup" runat="server" Text="<%$ Resources:StringDef, MoveToGroup %>" OnClick="ButtonMoveToGroup_Click" OnClientClick="var selectedNode = getTreeViewSelectedNode();var group = getDropDownListSelectedGroup();return window.confirm('确认要将 ' + selectedNode.outerText + ' 移动到组 ' + group + ' 吗？');" />
				<Blaze:ServerGroupDropDownList ID="ServerGroupDropDownList" runat="server" />
				<br />
				<asp:Button ID="ButtonDeleteServer" runat="server" Text="<%$ Resources:StringDef, DeleteServer %>" OnClick="ButtonDeleteServer_Click" />
			</asp:Panel>
		</td>
	</tr>
	<tr>
	<td>
	<asp:HyperLink ID="ButtonReturntoAddServer"  runat="server" NavigateUrl="EditServerGroup.aspx" Text="<%$ Resources:StringDef, ReturnBackToAddGroupServer %>" />
	</td>
	</tr>
	
	
</table>
</contenttemplate>
</asp:UpdatePanel>
</asp:Content>