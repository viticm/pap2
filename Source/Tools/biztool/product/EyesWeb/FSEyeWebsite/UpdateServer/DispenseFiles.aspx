<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="UpdateServer_DispenseFiles, App_Web_hm9xpbcd" title="DispenseFiles" theme="BlazeSky" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">

    <script type="text/javascript">
                        function goPostBack()
                        {
                            var o = window.event.srcElement;
                            if (o.tagName == "INPUT" && o.type == "checkbox")
                            {
                               __doPostBack("","");
                            } 
                        }
                        
function deleteConfirm(msg)
{
    return window.confirm(msg);
}
function LinkButtonClick(val)
{
   document.all['ctl00_ContentPlaceHolderMain_txtHiddenFolderName'].value = document.all['ctl00_ContentPlaceHolderMain_txtHiddenFolderName'].value + val;
}
    </script>

    <table width="100%">
        <tr class="TableRowSpacing" />
        <tr>
            <td>
                <asp:DropDownList ID="DropDownListFTPServer" runat="server">
                </asp:DropDownList>
                <asp:TextBox ID="txtHiddenFolderName" runat="server" Style="color: Black; background-color: #CCFFCC;
                    border-color: #0D456E; border-width: 1px; border-style: Solid; height: 17px;
                    width: 250px;" OnTextChanged="txtHiddenFolderName_TextChanged"></asp:TextBox>
                <asp:LinkButton ID="ButtonRefresh" runat="server" Text="<%$ Resources:StringDef,Refresh %>"
                    Width="50" OnClick="ButtonRefresh_Click"></asp:LinkButton>
                <asp:LinkButton ID="ButtonUpFolder" runat="server" Text="<%$ Resources:StringDef,UpFolder %>"
                    Width="100" OnClick="ButtonUpFolder_Click"></asp:LinkButton>
            </td>
        </tr>
        <tr>
            <td>
                <asp:UpdatePanel runat="server" ID="UpdatePanelFtpFiles" UpdateMode="Conditional">
                    <ContentTemplate>                  
                                <asp:Label ID="LabelOpMsg" runat="server"></asp:Label>
                                <asp:Timer ID="Timer1" runat="server" OnTick="Timer1_Tick1" Interval="3000" Enabled="False">
                                </asp:Timer>
                                <asp:Table ID="ServerFileList" runat="server" />
                                <asp:GridView ID="GridViewServerFile" runat="server" AutoGenerateColumns="False">
                                </asp:GridView>    
                    </ContentTemplate>
                </asp:UpdatePanel>
            </td>
        </tr>
    </table>
    
    
                
    <table width="100%">
        <tr class="TableRowSpacing" />
        <tr>
            <td align="center">
                <asp:UpdateProgress ID="UpdateProgress2" runat="server" AssociatedUpdatePanelID="UpdatePanelFtpFiles">
                    <ProgressTemplate>
                        <asp:Image ID="Image1" runat="server" SkinID="PleaseWait" />
                        <asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />
                    </ProgressTemplate>
                </asp:UpdateProgress>
            </td>
        </tr>
    </table>
</asp:Content>
