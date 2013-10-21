<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="AddNewCabinet.aspx.cs" Inherits="Customization_AddNewCabinet" Title="Untitled Page" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderTopMessage" Runat="Server">
</asp:Content>
<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:UpdatePanel ID="UpdatePanel1" runat="server">
<ContentTemplate>
    <table>
        <tr><td><asp:Literal ID="Literal1" runat="server" Text="已有的机柜" /></td></tr>
        <tr>
            <td>
                <asp:ListBox ID="ListBoxCabinet" runat="server" Rows="20" Width="160px" />
                <asp:LinkButton ID="LinkButtonDeleteCabinet" runat="server" Text="删除选中的机柜" OnClick="LinkButtonDeleteCabinet_Click" />
            </td>       
        </tr>
        <tr class="TableRowSpacing" />
        <tr>            
            <td>
                <asp:Literal runat="server" ID="Literal2" Text="要添加的机柜名：" /><asp:TextBox ID="TextBoxAddCabinet" runat="server" />
                <asp:LinkButton ID="LinkButtonAddCabinet" runat="server" Text="添加机柜" OnClick="LinkButtonAddCabinet_Click" />
            </td>
        </tr>      
        <tr class="TableRowSpacing" />
        <tr>
            <td>
                <asp:Literal runat="server" ID="Literal3" Text="将机柜重命名为：" /><asp:TextBox ID="TextBoxRenameCabinet" runat="server" />
                <asp:LinkButton ID="LinkButtonRenameCabinet" runat="server" Text="重命名选中的机柜" OnClick="LinkButtonRenameCabinet_Click" />
            </td>
        </tr>	
    </table>
    
    <br />
    
    <table>
        <tr><td>注：</td></tr>
        <tr><td>删除机柜将会把该机柜下服务器组的机柜信息清空</td></tr>
        <tr><td>重命名机柜将会同时更新该机柜下服务器组的机柜信息</td></tr>
        <tr class="TableRowSpacing" />
        <tr>
            <td><asp:Label ID="LabelSuccess" runat="server" SkinID="MessageSuccess" /></td>
        </tr>    
        <tr>
            <td><asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" /></td>
        </tr>
    </table>
</ContentTemplate>
</asp:UpdatePanel>    
</asp:Content>

