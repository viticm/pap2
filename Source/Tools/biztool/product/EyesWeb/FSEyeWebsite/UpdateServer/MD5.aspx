<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="UpdateServer_MD5, App_Web_hm9xpbcd" title="Untitled Page" theme="BlazeSky" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
    <table>
        <tr>
            <td><asp:LinkButton ID="buttonCreatMD5"
                    runat="server" OnClick="buttonCreatMD5_Click" Visible="False">随机生成MD5</asp:LinkButton></td>            
        </tr>        
        <tr>
           <td>
                文件：<asp:TextBox ID="textboxFile" runat="server" style="width:120px"></asp:TextBox>
                <asp:LinkButton ID="buttonMD5" runat="server" Text="生成MD5"
                Width="50" OnClick="buttonMD5_Click"></asp:LinkButton>
           </td>
        </tr>        
        <tr>
            <td>
                MD5：<asp:TextBox ID="textboxRandomMD5" runat="server" style="width:294px;"></asp:TextBox>
            </td>
        </tr>
    </table>
</asp:Content>

