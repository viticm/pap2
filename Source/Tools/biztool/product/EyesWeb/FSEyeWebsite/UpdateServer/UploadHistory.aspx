<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="UpdateServer_UploadHistory, App_Web_hm9xpbcd" title="Untitled Page" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
    <table width="100%">
        <tr class="TableRowSpacing" />
        <tr>
            <td>
                <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" AutoPostBack="true" OnSelectedServerGroupChanged="ServerGroupIndexChanged" />
				<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" />
				<asp:Button ID="buttonDispenseHistory" runat="server" Text="<%$ Resources:StringDef, View %>" OnClick="buttonDispenseHistory_Click">
                </asp:Button>
            </td>
        </tr>
         <tr class="TableRowSpacing" />
         <tr>
            <td>
                <asp:UpdatePanel runat="server" ID="UpdatePanelFtpFiles" UpdateMode="Conditional">
                    <ContentTemplate>     
                        <asp:Timer ID="Timer1" runat="server" OnTick="Timer1_Tick1" Interval="1000" Enabled="false"></asp:Timer>               
                        <asp:Table ID="ServerFileList" runat="server" />
                    </ContentTemplate>
                </asp:UpdatePanel>
            </td>
        </tr>
    </table>

</asp:Content>

