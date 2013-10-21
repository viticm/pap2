<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="UpdateServerConfig.aspx.cs" Inherits="UpDateManagement_UpdateServerConfig" Title="UpdateServerConfig" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">
<script type="text/javascript">
function deleteConfirm(msg)
{
    return window.confirm(msg);//"<%=Resources.StringDef.DeleteUpdateServerConfirm %>");
}
</script>

<asp:UpdatePanel id="UpdatePanel1" runat="server">
    <ContentTemplate>    
    <table width="100%">
        <tr class="TableCaption">
            <td>
                <asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, UpdateServer %>" />
            </td>
        </tr>
        <tr>
            <td>
                <asp:Label ID="NoUpdateServer" runat="server" Text="<%$ Resources:StringDef,NoUpdateServer %>" Visible="false" />
                <asp:DataGrid runat="server" ID="DataGridUpdateServer" AutoGenerateColumns="false" CellSpacing="1" CssClass="TN" HeaderStyle-CssClass="TCH"
                 OnDeleteCommand="DataGridUpdateServer_Delete" OnItemDataBound="DataGridUpdateServer_ItemDataBound">
                    <Columns>
                        <asp:ButtonColumn ButtonType="LinkButton" CommandName="Delete" Text="<%$ Resources:StringDef,Delete %>" />
                        <asp:TemplateColumn>
                            <ItemTemplate>
                                <a href='EditUpdateServer.aspx?serverId=<%# Eval("ServerId") %>'><asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef,Edit %>" /></a>
                            </ItemTemplate>
                        </asp:TemplateColumn>
                        <asp:TemplateColumn>
                            <ItemTemplate>
                                <a href='UpdateServerState.aspx?serverId=<%# Eval("ServerId") %>'><asp:Literal runat="server" Text="<%$ Resources:StringDef,Watch %>" /></a>
                            </ItemTemplate>
                        </asp:TemplateColumn>
                        <asp:BoundColumn DataField="Name" HeaderText="<%$ Resources:StringDef,Name %>" />
                        <asp:BoundColumn DataField="IP" HeaderText="<%$ Resources:StringDef,Ip %>" />
                        <asp:BoundColumn DataField="ServerType" HeaderText="<%$ Resources:StringDef,UpdateServerType %>" />
                        <asp:BoundColumn DataField="UserName" HeaderText="<%$ Resources:StringDef,UserName %>" />
                        <asp:BoundColumn DataField="Password" HeaderText="<%$ Resources:StringDef,Password %>" />
                        <asp:BoundColumn DataField="Port" HeaderText="<%$ Resources:StringDef,Port %>" />
                        <asp:BoundColumn DataField="ServerId" Visible="false" />
                    </Columns>
                </asp:DataGrid>
                <%--
                <table border="1" class="TN">
                    <tr>
                        <td class="TableCellList">
                            <asp:Panel ID="Panel1" runat="server" Height="150" ScrollBars="Vertical">
                                <asp:Label ID="NoUpdateServer" runat="server" Text="<% Resource:StringDef,NoUpdateServer%>" />
                                <asp:CheckBoxList ID="FTPServerCheckBoxList" runat="server" RepeatColumns="5" RepeatLayout="Table" />
                            </asp:Panel>
                        </td>
                    </tr>
                    <tr class="TableRowNormal">
                        <td align="left">
                            <asp:Label ID="Label2" runat="server" />
                        </td>
                    </tr>
                    <tr>
                        <td align="center">
                            <table cellspacing="0" cellpadding="0">
                                <tr>
                                    <td>
                                        <ul>
                                            <li><asp:LinkButton ID="LinkButton1" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef,SelectAll %>" OnClick="ButtonSelectAllServers_Click" /></li><li><asp:LinkButton ID="LinkButton2" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef,SelectNone %>" OnClick="ButtonSelectNoServer_Click" /></li></ul>
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>
                </table>
                --%>
            </td>
        </tr>
        <tr class="TableRowNormal" />
	    <tr>
		    <td align="center">
			    <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
		    </td>
	    </tr>
    </table>
    </ContentTemplate>
</asp:UpdatePanel>

<table width="100%">
    <tr class="TableRowSpacing" />
    <tr>
        <td align="center">
            <table cellspacing="0" cellpadding="0">
                <tr>
                    <td align="center">
                        <asp:HyperLink runat="server" NavigateUrl="AddUpdateServer.aspx" Text="<%$ Resources:StringDef, Add %>" />
                        <asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="UpdateManagement.aspx" Text="<%$ Resources:StringDef, Return %>" />
                    </td>
                </tr>
            </table>
        </td>
    </tr>
    <tr class="TableRowSpacing" />
</table>
</asp:Content>