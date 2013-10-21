<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="AddServerGroupSimplified.aspx.cs" Inherits="GameServer_AddServerGroupSimplified" Title="Untitled Page" %>

<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerConfigTextBox" Src="~/Common/ServerConfigTextBox.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="FtpDropDownList" Src="~/Common/FtpServerDropdownlist.ascx" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:UpdatePanel ID="UpdatePanel1" runat="server" UpdateMode="Conditional">
<ContentTemplate>
    <table width="60%" align="center">
	    <tr>
	        <td align="center">
	            <asp:Label ID="Label1" runat="server" SkinID="Title" Text="添加一组服务器(单台单组的简化模式)" />
	        </td>
	    </tr>
    </table>

    <table border="0" cellspacing="0" cellpadding="0" width="60%" align="center">
        <tr>
            <td>
                <fieldset>
                    <legend>
                        <asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef,BasicConfig %>" />
                    </legend>
                    
                    <table width="96%" style="margin: 5px,0px,5px,0px;" class="TN" cellspacing="1">
                        <tr>
                            <td class="TCH">
                                <asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef, ServerGroup %>"></asp:Literal>
                            </td>
                            <td class="TCN">
                                <Blaze:ServerGroupDropDownList ID="ServerGroupDropDownList" runat="server" />
                                <asp:HyperLink ID="HyperLink2" runat="server" NavigateUrl="ServerGroupManagement.aspx?addGroup=addGroup" Text="<%$ Resources:StringDef, AddGroupFirst %>" />
                            </td>
                        </tr>
                        <tr>
                            <td class="TCH" width="30%">
                                <asp:Literal ID="Literal27" runat="server" Text="<%$ Resources:StringDef, FSEyeIp %>" />
                            </td>
                            <td class="TCN">
                                <asp:TextBox ID="TextBoxFSEyeIp" runat="server" />
                                <asp:RequiredFieldValidator ID="RequiredFieldValidator4" runat="server" 							
                                                        ControlToValidate="TextBoxFSEyeIp"
                                                        Display="None"
                                                        SetFocusOnError="true"                                                        
                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
                                <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender6" TargetControlID="RequiredFieldValidator4" />
                                <asp:RegularExpressionValidator ID="RegularExpressionValidator3" runat="server"
                                     ControlToValidate="TextBoxFSEyeIp"
                                     Display="None"
                                     SetFocusOnError="true"                                     
                                     ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
                                <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender7" TargetControlID="RegularExpressionValidator3" />                                
                            </td>
                        </tr>
                        <tr>
                            <td class="TCH">
                                <asp:Literal ID="Literal28" runat="server" Text="<%$ Resources:StringDef, FSEyePort %>" />
                            </td>
                            <td class="TCN">
                                <asp:TextBox ID="TextBoxFSEyePort" runat="server" Enabled="false" />
                            </td>
                        </tr>
                        <tr>
                            <td class="TCH">
                                <asp:Literal ID="Literal51" runat="server" Text="<%$ Resources:StringDef, FtpDownload %>" />
                            </td>
                            <td class="TCN">
                                <Blaze:FtpDropDownList ID="FtpDropDownList" runat="server" />
                            </td>
                        </tr>
                        <tr class="TableRowNormal">
                            <td class="TCH">
                                <asp:Literal ID="Literal12" runat="server" Text="服务器组IP" />
                            </td>
                            <td class="TCN">
                                <asp:TextBox ID="TextBoxGroupIpAddress" runat="server" CssClass="TextBox" />
                                <asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" 							
                                                        ControlToValidate="TextBoxGroupIpAddress"
                                                        Display="None"
                                                        SetFocusOnError="true"                                                        
                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
                                <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender1" TargetControlID="RequiredFieldValidator1" />
                                <asp:RegularExpressionValidator ID="RegularExpressionValidator1" runat="server"
                                     ControlToValidate="TextBoxGroupIpAddress"
                                     Display="None"
                                     SetFocusOnError="true"                                     
                                     ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
                                <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender2" TargetControlID="RegularExpressionValidator1" />	
                            </td>
                        </tr>
                        <tr class="TableRowNormal">
                            <td class="TCH">
                                <asp:Literal ID="Literal53" runat="server" Text="GMC地址" />
                            </td>
                            <td class="TCN">
                                <asp:TextBox ID="GMCIpTextBox" runat="server" CssClass="TextBox" />
                                <asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" 							
                                    ControlToValidate="GMCIpTextBox"
                                    Display="None"
                                    SetFocusOnError="true"                                    
                                    ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
                                <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender3" TargetControlID="RequiredFieldValidator2" />
                                <asp:RegularExpressionValidator ID="RegularExpressionValidator2" runat="server"
                                     ControlToValidate="GMCIpTextBox"
                                     Display="None"
                                     SetFocusOnError="true"                                     
                                     ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
                                <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender4" TargetControlID="RegularExpressionValidator2" />	
                            </td>
                        </tr>
                        <tr class="TableRowNormal">
                            <td class="TCH">
                                <asp:Literal ID="Literal54" runat="server" Text="GMC帐号" />
                            </td>
                            <td class="TCN">
                                <asp:TextBox ID="GMCAccountTextBox" runat="server" CssClass="TextBox" />
                                <asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server" 							
                                    ControlToValidate="GMCAccountTextBox"
                                    Display="None"
                                    SetFocusOnError="true"                                   
                                    ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
                                <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender5" TargetControlID="RequiredFieldValidator3" />
                            </td>
                        </tr>                        
                    </table>
                </fieldset>
            </td>
        </tr>        
    </table>
    
    <br />
    
    <table border="0" cellspacing="0" cellpadding="0" width="90%" align="center">
        <tr>
            <td>
                <fieldset>
                    <legend>
                        <asp:Literal ID="Literal7" runat="server" Text="服务器配置文件参数" />
                    </legend>
                    <asp:Table ID="ConfigParametersTable" runat="server" />
                </fieldset>
            </td>
        </tr>
    </table>
    
    <br />
    
    <table align="center">
        <tr>
            <td align="center">
                <asp:Label ID="LabelSuccess" runat="server" SkinID="MessageSuccess" EnableViewState="false"
                    Visible="false"></asp:Label>
                <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure"></asp:Label>
            </td>
        </tr>
        <tr class="TableRowSpacing" />	
        <tr class="TableRowSpacing" />	
        <tr>
            <td align="center">
                <asp:LinkButton ID="LinkButtonEdit" runat="server" Text="<%$ Resources:StringDef, Add %>" OnClick="LinkButtonSave_Click" />&nbsp;
                <asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="AddAndDelete.aspx" Text="<%$ Resources:StringDef, Return %>" />
            </td>
        </tr>	
    </table>
    
    <Blaze:ServerConfigTextBox ID="ServerConfigTextBox" runat="server" Visible="false" />
</ContentTemplate>
</asp:UpdatePanel>  

</asp:Content>

