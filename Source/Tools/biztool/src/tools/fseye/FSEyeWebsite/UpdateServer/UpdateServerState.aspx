<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="UpdateServerState.aspx.cs" Inherits="UpDateManagement_UpdateServerState" Title="UpdateServerState" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">
<script type="text/javascript">
function deleteConfirm(msg)
{
    return window.confirm(msg);
}
function removeConfirm()
{
    return window.confirm("<%=Resources.StringDef.RemoveFileFromUploadList %>");
}
</script>

<script runat="server">
String GetImageUrl(object type)
{
    string t = type as string;
    if (t == "д©б╪")
    {    
        return "~/Image/Folder.jpg";
    }
    else if (t == "нд╪Ч")
    {
        return "~/Image/File.jpg";
    }

    return String.Empty;
}
</script>

    <table width="100%">
        <tr class="TableRowSpacing" />
        <tr class="TableCaption">
            <td>
                <asp:Label ID="LabelTitle" runat="server" Text="<%$ Resources:StringDef,UpdateServer %>" />
            </td>                        
        </tr>
        <tr>
            <td>
            <asp:UpdatePanel runat="server" ID="UpdatePanel1" UpdateMode="Conditional">                
                <ContentTemplate>
                <table width="100%">
                    <tr>
                        <td>
                            <asp:Label ID="Message" runat="server"/>             
                        </td>
                    </tr>  
                    <tr class="TableRowSpacing" />                      
                    <tr>
                        <td>
                            <asp:Label runat="server" ID="LabelPath" Text="<%$ Resources:StringDef,Path %>" />&nbsp;<asp:Literal runat="server" ID="LiteralPath" />
                        </td>
                    </tr>                        
                    <tr>
                        <td>
                            <asp:DataGrid runat="server" ID="DataGridServerFileList" AutoGenerateColumns="false" CssClass="TN" ItemStyle-CssClass="TCN" HeaderStyle-CssClass="TCH" CellSpacing="1" Width="100%" CellPadding="0">
                                <Columns>
                                    <asp:TemplateColumn HeaderStyle-Width="5%" ItemStyle-Width="5%">
                                        <HeaderTemplate>
                                            <asp:CheckBox runat="server" ID="CheckBoxSelectAll" OnCheckedChanged="CheckBoxSelectAll_CheckedChanged" AutoPostBack="true" />
                                        </HeaderTemplate>
                                        <ItemTemplate>
                                            <asp:CheckBox runat="server" ID="CheckBoxServerFile" />
                                        </ItemTemplate>
                                    </asp:TemplateColumn>
                                    <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,Type %>" HeaderStyle-Width="5%" ItemStyle-Width="5%">
                                        <ItemTemplate>
                                            <asp:Image ID="Image1" runat="server" Height="15px" ImageUrl='<%# GetImageUrl(Eval("Type")) %>' />                                            
                                        </ItemTemplate>                                        
                                    </asp:TemplateColumn>
                                    <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,Name %>" HeaderStyle-Width="20%" ItemStyle-Width="20%">
                                        <ItemTemplate>                                            
                                            <asp:LinkButton runat="server" ID="LinkButtonViewDir" Text='<%# Eval("Name") %>' OnClick="LinkButtonViewDir_Click" SkinID="PlainText" style="text-decoration:none;" />
                                        </ItemTemplate>
                                    </asp:TemplateColumn>                                    
                                    <asp:BoundColumn DataField="Size" HeaderText="<%$ Resources:StringDef,Size %>" HeaderStyle-Width="10%" ItemStyle-Width="10%"/>
                                    <asp:BoundColumn DataField="Modified" HeaderText="<%$ Resources:StringDef,ModifiedTime %>" HeaderStyle-Width="20%" ItemStyle-Width="20%"/>
                                    <asp:BoundColumn DataField="Attribute" HeaderText="<%$ Resources:StringDef,Attribute %>" HeaderStyle-Width="10%" ItemStyle-Width="10%"/>
                                    <asp:BoundColumn DataField="Owner" HeaderText="<%$ Resources:StringDef,Owner %>" HeaderStyle-Width="10%" ItemStyle-Width="10%" />
                                    <asp:BoundColumn DataField="Type" Visible="false" />
                                </Columns>
                            </asp:DataGrid>
                        </td>
                    </tr>
                    <tr>                        
                        <td align="center">
                            <asp:LinkButton runat="server" ID="LinkButtonRefresh" Text="<%$ Resources:StringDef,Refresh %>" OnClick="LinkButtonRefresh_Click" />
                            <asp:LinkButton runat="server" ID="LinkButtonUpFolder" Text="<%$ Resources:StringDef, UpFolder %>" OnClick="LinkButtonUpFolder_Click" />
                            <asp:LinkButton ID="LinkButtonDeleteServerFile" runat="server" OnClick="LinkButtonDeleteServerFile_Click" Text="<%$ Resources:StringDef,Delete %>" />            
                            <ajax:ConfirmButtonExtender runat="server" ID="ConfirmButtonExtender2" TargetControlID="LinkButtonDeleteServerFile" ConfirmText="<%$ Resources:StringDef,DeleteUpdateServerFileConfirm %>" />
                        </td>
                    </tr>
                </table>
                </ContentTemplate>
            </asp:UpdatePanel>
            </td>
        </tr>
        <tr class="TableRowSpacing" />
        <tr class="TableCaption">
            <td>
                <asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef,UploadFile %>" />
            </td>            
        </tr>
        <tr>
            <td>               
                <asp:UpdatePanel runat="server" ID="UpdatePanel2" UpdateMode="Conditional">
                    <ContentTemplate>                                      
                    <asp:Timer ID="Timer2" runat="server" OnTick="Timer2_Tick2" Interval="4000"></asp:Timer>      
                    <asp:Label ID="MessageUploadFile" runat="server" />
                    <asp:Table ID="UploadFileList" runat="server" ></asp:Table>
                    </ContentTemplate>                    
                </asp:UpdatePanel>    
            </td>
        </tr>
    </table>
</asp:Content>