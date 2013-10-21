<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true"
    CodeFile="UploadFiles.aspx.cs" Inherits="UpDateManagement_UpLoadFiles" Title="UploadFiles" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">
    <asp:UpdatePanel id="UpdatePanel1" runat="server">
        <ContentTemplate>    
        <table width="100%">
            <tr class="TableRowSpacing" />
            <tr class="TableCaption">
                <td>
                    <asp:Label ID="LabelTitle" runat="server" Text="<%$ Resources:StringDef, UploadFile %>" />
                </td>
            </tr>
            <tr>
                <td>
                    <table border="1" class="TN">
                        <tr>
                            <td class="TableCellList">
                                <asp:Panel ID="UploadedFilePanel" runat="server" Height="150" ScrollBars="Vertical">
                                    <asp:Label ID="LabelUploadedNoFile" runat="server" Text="<%$ Resources:StringDef,NoUploadFiles %>" />
                                    <asp:CheckBoxList ID="CheckBoxListUploadedFile" runat="server" RepeatColumns="3"
                                        RepeatLayout="Table" />                                 
                                </asp:Panel>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <asp:RadioButtonList ID="OverWriteChoice" runat="server" RepeatDirection="Horizontal">
                                   <asp:ListItem Text = "<%$ Resources: StringDef,Append%>" />
                                   <asp:ListItem Text = "<%$ Resources: StringDef,OverWrite%>" Selected="true" />
                                </asp:RadioButtonList>
                            </td>
                        </tr>
                        <tr class="TableRowNormal">
                            <td align="left">
                                <asp:Label ID="LabelUploadedFilesInfo" runat="server" />
                            </td>
                        </tr>
                        <tr>
                            <td align="center">
                                <asp:LinkButton ID="LinkButtonSelectAllFiles" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef,SelectAll %>" OnClick="ButtonSelectAllFiles_Click" />
                                <asp:LinkButton ID="LinkButtonSelectNoFile" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef,SelectNone %>" OnClick="ButtonSelectNoFile_Click" />
                            </td>
                        </tr>
                    </table>
                </td>
            </tr>
            <tr class="TableRowSpacing" />
            <tr class="TableCaption">
                <td>
                    <asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, UpdateServer %>" />
                </td>
            </tr>
            <tr>
                <td align="center">
                    <asp:CheckBoxList ID="CheckBoxListFTPServerType" runat="server" RepeatLayout="Table" RepeatDirection="Horizontal" RepeatColumns="3" OnSelectedIndexChanged="CheckBoxListFTPServerType_OnSelectedIndexChanged" AutoPostBack="true" />
                </td>
            </tr>
            <tr>
                <td>
                    <table border="1" class="TN">
                        <tr>
                            <td class="TableCellList">
                                <asp:Panel ID="Panel1" runat="server" Height="150" ScrollBars="Vertical">
                                    <asp:Label ID="NoUpdateServer" runat="server" Text="<%$ Resources:StringDef,NoUpdateServer %>" />
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
                                <asp:LinkButton ID="LinkButton1" runat="server" Text="<%$ Resources:StringDef,SelectAll %>" OnClick="ButtonSelectAllServers_Click" />
                                <asp:LinkButton ID="LinkButton2" runat="server" Text="<%$ Resources:StringDef,SelectNone %>" OnClick="ButtonSelectNoServer_Click" />
                            </td>
                        </tr>
                    </table>
                </td>
            </tr>
        </table>
        </ContentTemplate>
    </asp:UpdatePanel>    
    
    <asp:UpdatePanel runat="server" ID="UpdatePanel2" UpdateMode="Conditional">
        <ContentTemplate>
            <table width="100%">
                <tr class="TableRowSpacing" />
                <tr>
                    <td align="center">
                        <table cellspacing="0" cellpadding="0">
                            <tr>
                                <td align="center">
                                    <asp:LinkButton ID="ButtonUploadToUpdateServer" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, UploadToUpdateServer %>" OnClick="ButtonUploadToUpdateServer_Click" />
                                    <asp:HyperLink runat="server" NavigateUrl="UpdateManagement.aspx" Text="<%$ Resources:StringDef, Return %>" />
                                </td>
                            </tr>
                        </table>
                    </td>
                </tr>
                <tr class="TableRowSpacing" />
                
                <tr>
                    <td align="center">
                        <asp:Label ID="LabelOpResultMessage" runat="server" />
                    </td>
                </tr>
                
                <tr class="TableRowSpacing" />
                <tr>
                    <td>                                                   
                        <asp:Timer ID="ShowUploadingStatusTimer" runat="server" OnTick="ShowUploadingStatusTimer_OnTick" Interval="1500" Enabled="false"></asp:Timer>      
                        <asp:Table ID="ShowUploadingStatusTable" runat="server" ></asp:Table>
                    </td>
                </tr>
            </table>
    </ContentTemplate>
    </asp:UpdatePanel>
    
</asp:Content>
