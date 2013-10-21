<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="ServerListConfig.aspx.cs" Inherits="GameServer_ServerListConfig" Title="<%$ Resources:StringDef,ServerListConfig %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:UpdatePanel runat="server" ID="UpdatePanel1">
<ContentTemplate>
<table width="100%">
    <tr>
        <td align="center">
            <asp:Label ID="LabelTitle" runat="server" Text="<%$ Resources:StringDef,ServerListConfig %>" SkinID="title" />
        </td>
    </tr>
    <tr>
        <td align="left">
            <asp:Label ID="LabelTotalReport" runat="server" />
        </td>
    </tr>
    <tr>
        <td align="center" width="100%" >
            <asp:DataGrid runat="server" AutoGenerateColumns="false" ID="DataGridServerMap" OnCancelCommand="DataGridServerMap_CancelCommand"
                OnUpdateCommand="DataGridServerMap_UpdateCommand" OnDeleteCommand="DataGridServerMap_DeleteCommand"
                OnEditCommand="DataGridServerMap_EditCommand" OnItemCommand="DataGridServerMap_ItemCommand"
                OnItemDataBound="DataGridServerMap_ItemDataBound" Width="100%">
                <Columns>
                    <asp:BoundColumn DataField="Id" ReadOnly="true" Visible="false" />                    
                    <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,ServerListNet %>">
                        <ItemTemplate>
                            <asp:Literal runat="server" ID="LiteralNet" Text='<%# Eval("Net") %>' />
                        </ItemTemplate>
                        <EditItemTemplate>
                            <asp:DropDownList runat="server" ID="DropDownListNetEdit" OnSelectedIndexChanged="DropDownListNetEdit_SelectedIndexChanged" AutoPostBack="true" />
                        </EditItemTemplate>
                    </asp:TemplateColumn>
                    <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,ServerListRegion %>">
                        <ItemTemplate>
                            <asp:Literal runat="server" ID="LiteralRegion" Text='<%# Eval("Region") %>' />
                        </ItemTemplate>
                        <EditItemTemplate>
                            <asp:DropDownList runat="server" ID="DropDownListRegionEdit" />
                        </EditItemTemplate>
                    </asp:TemplateColumn>
                    <asp:BoundColumn DataField="Name" HeaderText="<%$ Resources:StringDef, Server %>" />
                    <asp:BoundColumn DataField="Flag" ReadOnly="true" HeaderText="<%$ Resources:StringDef, Flag %>" Visible="false" />
                    <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,Flag %>">
                        <ItemTemplate>
                            <asp:Literal runat="server" Text='<%# Eval("FlagDesc") %>' />
                        </ItemTemplate>
                        <EditItemTemplate>
                            <asp:CheckBoxList runat="server" ID="CheckBoxListServerFlag" RepeatLayout="Table" RepeatDirection="Horizontal" RepeatColumns="3" />
                        </EditItemTemplate>
                    </asp:TemplateColumn>
                    <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,RealMachine %>">
                        <ItemTemplate>
                            <asp:Literal runat="server" ID="LiteralServerName" Text='<%# Eval("ServerName") %>' />
                        </ItemTemplate>
                        <EditItemTemplate>
                            <Blaze:ServerGroupDropDownList runat="server" ID="DropDownListServerGroup" AutoPostBack="true" OnSelectedServerGroupChanged="ServerGroupIndexChanged" />
                            <Blaze:ServerDropDownList runat="server" ID="DropDownListServer" />
                        </EditItemTemplate>
                    </asp:TemplateColumn>
                    <asp:BoundColumn DataField="ServerIp" ReadOnly="true" HeaderText="<%$ Resources:StringDef,IPAddress %>" />
                    <asp:ButtonColumn CommandName="Delete" Text="<%$ Resources:StringDef,Delete %>" />
                    <asp:EditCommandColumn CancelText="<%$ Resources:StringDef, Cancel %>" EditText="<%$ Resources:StringDef,Modify %>" UpdateText="<%$ Resources:StringDef,Save %>" />
                    <asp:ButtonColumn ButtonType="LinkButton" Text="<%$ Resources:StringDef,MoveUp %>" CommandName="MoveUp" />
                    <asp:ButtonColumn ButtonType="LinkButton" Text="<%$ Resources:StringDef,MoveDown %>" CommandName="MoveDown" />
                    <asp:ButtonColumn ButtonType="LinkButton" Text="¶¥ÒÆ" CommandName="MoveTop" />
                    <asp:ButtonColumn ButtonType="LinkButton" Text="µ×ÒÆ" CommandName="MoveBottom" />
                </Columns>
            </asp:DataGrid>        
        </td>
    </tr>
    <tr>
        <td align="center">
            <asp:LinkButton runat="server" ID="LinkButtonShowNet" OnClick="LinkButtonShowNet_Click" Text="<%$ Resources:StringDef,ShowNet %>" />
            <asp:LinkButton runat="server" ID="LinkButtonShowRegion" OnClick="LinkButtonShowRegion_Click" Text="<%$ Resources:StringDef,ShowRegion %>" />
            <asp:LinkButton runat="server" ID="LinkButtonAddServer" OnClick="LinkButtonAddServer_Click" Text="<%$ Resources:StringDef,AddServer %>" />
            <asp:LinkButton runat="server" ID="LinkButtonSave" OnClick="LinkButtonSave_Click" Text="<%$ Resources:StringDef,SaveWholeConfiguration %>" />
            <ajax:ConfirmButtonExtender runat="server" TargetControlID="LinkButtonSave" ConfirmText="<%$ Resources:StringDef,MsgOperationConfirm %>" />
            <asp:LinkButton runat="server" ID="LinkButtonCreateServerListFile" OnClick="LinkButtonCreateServerListFile_Click" Text="<%$ Resources:StringDef,CreateGameServerListFile %>" />
            <ajax:ConfirmButtonExtender runat="server" TargetControlID="LinkButtonCreateServerListFile" ConfirmText="<%$ Resources:StringDef,MsgOperationConfirm %>" />
            <asp:HyperLink runat="server" ID="HyperLinkViewSystemServerList" Text="<%$ Resources:StringDef,ViewSysServerListFile %>" NavigateUrl="GameServerHandler.ashx?op=CreateServerListFile" Target="_blank" />
            <asp:HyperLink runat="server" ID="HyperLinkViewCurrServerList" Text="<%$ Resources:StringDef,ViewCurrServerListFile %>" NavigateUrl="GameServerHandler.ashx?op=CreateCurrentServerListFile" Target="_blank" />
        </td>
    </tr>
</table>
</ContentTemplate>
</asp:UpdatePanel>

<table width="100%" align="center">
	<tr>
		<td align="center">
		    <asp:UpdateProgress ID="UpdateProgress1" runat="server">
				<ProgressTemplate>
					<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileOperating %>" />
				</ProgressTemplate>
			</asp:UpdateProgress>
	    </td>
    </tr>
</table>

<asp:UpdatePanel runat="server" ID="UpdatePanelNet">
<ContentTemplate>
<asp:Panel runat="server" ID="PanelNet" Visible="false">
<table width="100%">
    <tr>
        <td align="center">
            <asp:DataGrid runat="server" ID="DataGridNet" AutoGenerateColumns="false" OnUpdateCommand="DataGridNet_UpdateCommand" OnCancelCommand="DataGridNet_CancelCommand"
                OnEditCommand="DataGridNet_EditCommand" OnItemCommand="DataGridNet_ItemCommand" OnItemDataBound="DataGridNet_ItemDataBound" Width="100%">
                <Columns>
                    <asp:BoundColumn DataField="Id" ReadOnly="true" Visible="false" />
                    <asp:BoundColumn DataField="Name" HeaderText="<%$ Resources:StringDef,Name %>" />
                    <asp:BoundColumn DataField="Flag" HeaderText="<%$ Resources:StringDef,Flag %>" ReadOnly="true" Visible="false" />
                    <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,Flag %>">
                        <ItemTemplate>
                            <asp:Literal runat="server" Text='<%# Eval("FlagDesc") %>' />
                        </ItemTemplate>
                        <EditItemTemplate>
                            <asp:CheckBoxList runat="server" ID="CheckBoxListNetFlag" RepeatLayout="Table" RepeatDirection="Horizontal" RepeatColumns="3" />
                        </EditItemTemplate>
                    </asp:TemplateColumn>
                    <asp:ButtonColumn Text="<%$ Resources:StringDef,Delete %>" CommandName="Delete" />
                    <asp:EditCommandColumn CancelText="<%$ Resources:StringDef, Cancel %>" EditText="<%$ Resources:StringDef,Modify %>" UpdateText="<%$ Resources:StringDef,Save %>" />
                    <asp:ButtonColumn ButtonType="LinkButton" Text="<%$ Resources:StringDef,MoveUp %>" CommandName="MoveUp" />
                    <asp:ButtonColumn ButtonType="LinkButton" Text="<%$ Resources:StringDef,MoveDown %>" CommandName="MoveDown" />
                    <asp:ButtonColumn ButtonType="LinkButton" Text="¶¥ÒÆ" CommandName="MoveTop" />
                    <asp:ButtonColumn ButtonType="LinkButton" Text="µ×ÒÆ" CommandName="MoveBottom" />
                </Columns>
            </asp:DataGrid>
        </td>
    </tr>
    <tr>
        <td align="center">
            <asp:LinkButton runat="server" ID="LinkButtonAdd" OnClick="LinkButtonAddNet_Click" Text="<%$ Resources:StringDef,AddNet %>" />
        </td>
    </tr>
</table>
</asp:Panel>
</ContentTemplate>
</asp:UpdatePanel>
<asp:UpdatePanel runat="server" ID="UpdatePanelRegion">
<ContentTemplate>
<asp:Panel runat="server" ID="PanelRegion" Visible="false">
<table width="100%">
    <tr>
        <td align="center">
            <asp:DataGrid runat="server" ID="DataGridRegion" AutoGenerateColumns="false" OnCancelCommand="DataGridRegion_CancelCommand"
                OnUpdateCommand="DataGridRegion_UpdateCommand" OnDeleteCommand="DataGridRegion_DeleteCommand"
                OnItemDataBound="DataGridRegion_ItemDataBound" OnEditCommand="DataGridRegion_EditCommand"
                OnItemCommand="DataGridRegion_ItemCommand" Width="100%">
                <Columns>
                    <asp:BoundColumn DataField="Id" ReadOnly="true" Visible="false" />
                    <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,ServerListNet %>">
                        <ItemTemplate>
                            <asp:Literal runat="server" ID="LiteralNet" Text='<%# Eval("Net") %>' />
                        </ItemTemplate>
                        <EditItemTemplate>
                            <asp:DropDownList runat="server" ID="DropDownListNet" />
                        </EditItemTemplate>
                    </asp:TemplateColumn>
                    <asp:BoundColumn DataField="Name" HeaderText="<%$ Resources:StringDef,Name %>" />
                    <asp:BoundColumn DataField="Flag" HeaderText="<%$ Resources:StringDef,Flag %>" ReadOnly="true" Visible="false" />
                    <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,Flag %>">
                        <ItemTemplate>
                            <asp:Literal runat="server" Text='<%# Eval("FlagDesc") %>' />
                        </ItemTemplate>
                        <EditItemTemplate>
                            <asp:CheckBoxList runat="server" ID="CheckBoxListRegionFlag" RepeatLayout="Table" RepeatDirection="Horizontal" RepeatColumns="3" />
                        </EditItemTemplate>
                    </asp:TemplateColumn>
                    <asp:ButtonColumn Text="<%$ Resources:StringDef,Delete %>" CommandName="Delete" />
                    <asp:EditCommandColumn CancelText="<%$ Resources:StringDef, Cancel %>" EditText="<%$ Resources:StringDef,Modify %>" UpdateText="<%$ Resources:StringDef,Save %>" />
                    <asp:ButtonColumn ButtonType="LinkButton" Text="<%$ Resources:StringDef,MoveUp %>" CommandName="MoveUp" />
                    <asp:ButtonColumn ButtonType="LinkButton" Text="<%$ Resources:StringDef,MoveDown %>" CommandName="MoveDown" />
                </Columns>
            </asp:DataGrid>
        </td>
    </tr>
    <tr>
        <td align="center">
            <asp:LinkButton runat="server" ID="LinkButtonAddRegion" OnClick="LinkButtonAddRegion_Click" Text="<%$ Resources:StringDef,AddRegion %>" />
        </td>
    </tr>
</table>
</asp:Panel>
</ContentTemplate>
</asp:UpdatePanel>

<asp:UpdatePanel runat="server" ID="LableOpResultMessageUpdatePanel">
<ContentTemplate>
    <table width="100%">
        <tr align="center">
            <td>
                <asp:Label runat="server" ID="LabelOpResultMessage" Visible="false" />
            </td>
        </tr>
    </table>
</ContentTemplate>
</asp:UpdatePanel>
</asp:Content>

<%--
<asp:UpdatePanel runat="server" ID="UpdatePanel1">
<ContentTemplate>
<table width="100%">    
    <tr>
        <td>
            <asp:Panel runat="server" ID="PanelAttributes" BorderWidth="1" BorderStyle="Outset">
                <table width="100%">
                    <tr>
                        <td width="30%">
                            <asp:TreeView runat="server" ID="TreeViewServerList" OnSelectedNodeChanged="TreeViewServerList_SelectedNodeChanged"></asp:TreeView>
                        </td>
                        <td width="70%">                            
                            <table width="100%">
                                <tr>
                                    <td>
                                        <asp:Literal runat="server" ID="LiteralFlag" Text="<%$ Resources:StringDef,Flag %>" />
                                    </td>
                                    <td>
                                        <asp:CheckBoxList runat="server" ID="CheckBoxListFlag" RepeatDirection="Horizontal" RepeatColumns="3" />
                                    </td>
                                </tr>
                                <tr>
                                    <td>
                                        <asp:Label runat="server" ID="LabelName" Text="<%$ Resources:StringDef,Name %>" />
                                    </td>
                                    <td>
                                        <asp:TextBox runat="server" ID="TextBoxName" />
                                    </td>
                                </tr>
                                <tr>
                                    <td>
                                        <asp:Label runat="server" ID="LabelType" Text="<%$ Resources:StringDef,Type %>" />
                                    </td>
                                    <td>
                                        <asp:DropDownList runat="server" ID="DropDownListType" AutoPostBack="true" OnSelectedIndexChanged="DropDownListType_SelectedIndexChanged" />
                                    </td>
                                </tr>
                                <tr>
                                    <td>
                                        <asp:Label runat="server" ID="LabelGameServer" Text="<%$ Resources:StringDef,GameServer %>" />
                                    </td>
                                    <td>
                                        <asp:Panel runat="server" ID="PanelServerList">
                                            <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" AutoPostBack="true" OnSelectedServerGroupChanged="ServerGroupIndexChanged" />
						                    <Blaze:ServerDropDownList runat="server" ID="ServerDropDownList"  ServerType="GameServer"/>
                                        </asp:Panel>
                                    </td>
                                </tr>
                                <tr>
                                    <td>
                                        <asp:Button runat="server" ID="ButtonCreate" OnClick="ButtonCreate_Click" Text="<%$ Resources:StringDef,Create %>" />
                                        <asp:Button runat="server" ID="ButtonModify" OnClick="ButtonModify_Click" Text="<%$ Resources:StringDef,Modify %>" />
                                        <asp:Button runat="server" ID="ButtonDelete" OnClick="ButtonDelete_Click" Text="<%$ Resources:StringDef,Delete %>" />
                                        <asp:Button runat="server" ID="ButtonMoveUp" OnClick="ButtonMove_Click" Text="ÉÏÒÆ" CommandName="Up" />
                                        <asp:Button runat="server" ID="ButtonMoveDown" OnClick="ButtonMove_Click" Text="ÏÂÒÆ" CommandName="Down" />
                                        <asp:Button runat="server" ID="ButtonLeft" OnClick="ButtonMove_Click" Text="×óÒÆ" CommandName="Left" />
                                        <asp:Button runat="server" ID="ButtonRight" OnClick="ButtonMove_Click" Text="ÓÒÒÆ" CommandName="Right" />
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>
                </table>
            </asp:Panel>
        </td>
    </tr>    
    <tr>
        <td align="center">
            <asp:LinkButton runat="server" ID="LinkButtonSave" OnClick="LinkButtonSave_Click" Text="<%$ Resources:StringDef,Save %>" />
            <asp:LinkButton runat="server" ID="LinkButtonCreateServerListFile" OnClick="LinkButtonCreateServerListFile_Click" Text="<%$ Resources:StringDef,CreateGameServerListFile %>" />
            <asp:HyperLink runat="server" ID="HyperLinkViewServerListFile" Text="<%$ Resources:StringDef,View %>" NavigateUrl="GameServerHandler.ashx?op=CreateServerListFile" Target="_blank" />
        </td>
    </tr>
</table>
</ContentTemplate>
</asp:UpdatePanel>
--%>