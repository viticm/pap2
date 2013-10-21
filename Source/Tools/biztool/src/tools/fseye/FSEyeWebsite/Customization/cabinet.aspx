<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true"
    CodeFile="cabinet.aspx.cs" Inherits="Customization_cabinet" Title="<%$ Resources:StringDef,Cabinet %>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">
    <asp:UpdatePanel ID="UpdatePanelCabinet" runat="server">
    <ContentTemplate>    
        <table width="100%">
            <tr>
                <td align="right"><asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef,NotInCabinet%>" /></td>
                <td align="center"></td>
                <td align="left">&nbsp;
                    <asp:DropDownList ID="DropDownListCabinet" runat="server" AutoPostBack="True" OnSelectedIndexChanged="DropDownListCabinet_SelectedIndexChanged">
                    </asp:DropDownList><asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="~/Customization/AddNewCabinet.aspx" Text="Ìí¼Ó,ÐÞ¸Ä,É¾³ý»ú¹ñ"></asp:HyperLink></td>
            </tr>
            <tr>
                <td align="right">
                    <asp:ListBox ID="ListBoxGroup" runat="server" Rows="28" Width="160px"></asp:ListBox></td>
                <td align="center">
                    <asp:Button ID="ButtonInsert" runat="server" Text=">>" OnClick="ButtonInsert_Click" /><br />
                    <asp:Button ID="ButtonRemove" runat="server" Text="<<" OnClick="ButtonRemove_Click" /></td>
                <td align="left">
                    &nbsp;<asp:ListBox ID="ListBoxCabinet" runat="server" Rows="28" Width="160px"></asp:ListBox></td>
            </tr>
            <tr>
                <td colspan="3" align="center">
                    </td>
            </tr>
        </table>
        </ContentTemplate>
    </asp:UpdatePanel>
</asp:Content>
