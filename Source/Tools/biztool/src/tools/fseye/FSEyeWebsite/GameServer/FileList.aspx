<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="FileList.aspx.cs" Inherits="GameServer_FileList" Title="<%$ Resources:StringDef, FileList %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>

<asp:Content ID="WholeContent" runat="server" ContentPlaceHolderID="ContentPlaceHolderMain">

<script type="text/ecmascript">
function IsRealFileName(file)
{   
//用正则表达式会好一些
    for(var i = 0;i<file.length;++i)
    {
        //怎么'*','\'等这些没有用
        if(file.charCodeAt(i)==47||file.charCodeAt(i)==92||file.charCodeAt(i)==34||
        file.charCodeAt(i)==60||file.charCodeAt(i)==62||file.charCodeAt(i)==63||
        file.charCodeAt(i)==124||file.charCodeAt(i)==42)return false;
    }
    return true;
}
</script>

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<ContentTemplate>
<table width="100%">
    <tr class="TableRowSpacing"></tr>
    <tr>
        <td runat="server" align="left">
            <Blaze:ServerGroupDropDownList ID="ServerGroupDropDownList" runat="server" AutoPostBack="true" OnSelectedServerGroupChanged="ServerGroupIndexChanged" />
            <Blaze:ServerDropDownList ID="ServerDropDownList" runat="server" AutoPostBack="true"  OnServerIndexChanged="ServerIndexChanged"/>
            <asp:Label ID="LabelPath" runat="server" Text="<%$ Resources:StringDef,Path %>" />
            <asp:TextBox ID="TextBoxPath" runat="server" />
        </td>
        <td align="right">
            <asp:LinkButton ID="Refresh" runat="server" Width="50" OnClick="ListFile_Click" Text="<%$ Resources:StringDef,Refresh %>" />
        </td>
    </tr>
    <tr>
        <td>
            <asp:UpdateProgress ID="UpdateProcess1" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
                <ProgressTemplate>
                    <asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef,Refreshing %>" Visible="false" />
                </ProgressTemplate>
            </asp:UpdateProgress>
            <asp:Label ID="LabelOpMsg" runat="server" />
        </td>
    </tr>
    <tr>
        <td>
            <asp:Label ID="Message" runat="server" />
        </td>
    </tr>
</table> 
</ContentTemplate>
</asp:UpdatePanel>
<asp:UpdatePanel runat="server" ID="UpdatePanelResult">
    <ContentTemplate>
        <table width="100%">
            <tr>
                <td align="center" width="100%">
                        <asp:DataGrid ID="DataGridResult" runat="server" AutoGenerateColumns="false" CssClass="TN" HeaderStyle-CssClass="TCH" Width="100%"  
                            OnCancelCommand="DataGridResutl_Cancel" OnDeleteCommand="DataGridResult_Delete" OnEditCommand="DataGridResutl_Rename" 
                            OnUpdateCommand="DataGridResult_Update" OnItemCommand="DataGridResult_ItemCommand" CellSpacing="1" OnItemDataBound="DataGridResult_ItemDataBound" >
            			<Columns>
            			    <asp:ButtonColumn Text="<%$ Resources:StringDef,Delete %>" ButtonType="LinkButton" CommandName="Delete" />
            			    <asp:EditCommandColumn ButtonType="LinkButton" EditText="<%$ Resources:StringDef,Rename %>" CancelText="<%$ Resources:StringDef,Cancel %>" UpdateText="<%$ Resources:StringDef,Update %>" />
            			    <asp:ButtonColumn Text="<%$ Resources:StringDef,Download %>" ButtonType="LinkButton" CommandName="Download" />
            			    <asp:TemplateColumn>
            			        <ItemTemplate>				            
            		                <asp:CheckBox ID="CheckBoxOverWrite" runat="server" Text="<%$ Resources:StringDef,OverWrite %>" />
            			        </ItemTemplate>
            			    </asp:TemplateColumn>					    
            			    <asp:BoundColumn DataField="FileName" HeaderText="<%$ Resources:StringDef,FileName %>" />
            			    <asp:BoundColumn DataField="FileAttribute" HeaderText="<%$ Resources:StringDef,FileAttribute %>" ReadOnly="true" />
            			    <asp:BoundColumn DataField="FileSize" HeaderText="<%$ Resources:StringDef,FileSize %>" ReadOnly="true" />
            			    <asp:BoundColumn DataField="ModifiedTime" HeaderText="<%$ Resources:StringDef,ModifiedTime %>" ReadOnly="true" />
            			</Columns>
            		</asp:DataGrid>
                </td>
            </tr>
        </table>
    </ContentTemplate>
</asp:UpdatePanel>
<ajax:UpdatePanelAnimationExtender ID="upae" BehaviorID="animation" runat="server" TargetControlID="UpdatePanelResult">
    <Animations>
        <OnUpdating>
            <Sequence>
                <%-- Disable all the controls --%>
                <Parallel duration="0">
                    <EnableAction AnimationTarget="Refresh" Enabled="false" />   
                    <EnableAction AnimationTarget="TextBoxPath" Enabled="false" />   
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
                    <EnableAction AnimationTarget="Refresh" Enabled="true" />
                    <EnableAction AnimationTarget="TextBoxPath" Enabled="true" />
                </Parallel>
                <StyleAction duration="0" Attribute="overflow" Value="visible" />
            </Sequence>
        </OnUpdated>
    </Animations>
</ajax:UpdatePanelAnimationExtender>
</asp:Content>