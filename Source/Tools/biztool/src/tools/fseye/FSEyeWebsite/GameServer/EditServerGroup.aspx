<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true"
    CodeFile="EditServerGroup.aspx.cs" Inherits="GameServer_EditServerGroup" Title="<%$ Resources:StringDef, EditServer %>" %>

<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerConfigTextBox" Src="~/Common/ServerConfigTextBox.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="FtpDropDownList" Src="~/Common/FtpServerDropdownlist.ascx" %>


<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">
    
<caption class="TableCaption">
    <asp:Literal ID="Literal11" runat="server" Text="<%$ Resources:StringDef, AddGameServerGroup %>">
    </asp:Literal>
</caption>
<asp:UpdatePanel ID="UpdatePanel3" runat="server" UpdateMode="conditional">
<ContentTemplate>
    <table border="0" cellspacing="0" cellpadding="0" width="60%" align="center">
        <tr>
            <td>
                <fieldset>
                    <legend>
                        <asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef,BasicConfig %>" />
                    </legend>
                    
                    <table width="95%" style="margin: 5px,0px,5px,0px;" class="TN" cellspacing="1">
                        <tr>
                            <td class="TCH">
                                <asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef, ServerGroup %>"></asp:Literal>
                            </td>
                            <td class="TCN">
                                <Blaze:ServerGroupDropDownList ID="ServerGroupDropDownList" runat="server" OnSelectedServerGroupChanged="ServerGroupDropDownList_Changed"
                                    AutoPostBack="true"></Blaze:ServerGroupDropDownList>
                                <asp:HyperLink ID="HyperLink2" runat="server" NavigateUrl="ServerGroupManagement.aspx?addGroup=addGroup" Text="<%$ Resources:StringDef, AddGroupFirst %>" />
                            </td>
                        </tr>
                        <tr>
                            <td class="TCH" width="30%">
                                <asp:Literal ID="Literal27" runat="server" Text="<%$ Resources:StringDef, FSEyeIp %>" />
                            </td>
                            <td class="TCN">
                                <asp:TextBox ID="TextBoxFSEyeIp" runat="server" />

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
                    </table>
                </fieldset>
            </td>
        </tr>
    </table>
    </ContentTemplate>
</asp:UpdatePanel>        

        
      
<asp:UpdatePanel ID="UpdatePanel2" runat="server" UpdateMode="conditional">
<ContentTemplate>
                            <table border="0" cellspacing="0" cellpadding="0" width="60%" align="center">
                            	   <tr>
                                    <td>
                                        <fieldset>
                                            <legend>
                                                <asp:CheckBox ID="CheckBoxBishop" runat="server" Text="<%$ Resources:StringDef,ServerTyperBishop %>"
                                                    AutoPostBack="true" OnCheckedChanged="CheckBoxBishop_Click" Checked="false"></asp:CheckBox></legend>
                                            <asp:Panel ID="PanelBishop" runat="server" Enabled="false">
                                                <table width="40%" style="margin: 5px,0px,5px,0px;" class="TN" cellspacing="1">
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal10" runat="server" Text="<%$ Resources:StringDef, ID %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxBishopId" runat="server" CssClass="TextBox" Enabled="false" />
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal9" runat="server" Text="<%$ Resources:StringDef, Name %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxBishopName" runat="server" CssClass="TextBox" />
                                                            <asp:RequiredFieldValidator ID="RequiredFieldValidator13" runat="server" 							
							                                                        ControlToValidate="TextBoxBishopName"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                       Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender25" TargetControlID="RequiredFieldValidator13" />
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal8" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxBishopComment" runat="server" CssClass="TextBox" />
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal7" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxBishopIpAddress" runat="server" CssClass="TextBox" />
                                                               <asp:RequiredFieldValidator ID="RequiredFieldValidator" runat="server" 							
							                                                        ControlToValidate="TextBoxBishopIpAddress"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                       Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender" TargetControlID="RequiredFieldValidator" />
						                                                         <asp:RegularExpressionValidator ID="ServerIpValidator" runat="server"
					                                                                 ControlToValidate="TextBoxBishopIpAddress"
							                                                         Display="None"
							                                                         SetFocusOnError="true"
							                                                         Enabled="false"
							                                                         ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
        	    	                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender11" TargetControlID="ServerIpValidator" />		            										
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxBishopDir" runat="server" CssClass="TextBox" onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>" />
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal14" runat="server" Text="<%$ Resources:StringDef, ConfigFile %>" />
                                                        </td>
                                                        <td align="center">
                                                            <asp:LinkButton ID="ButtonShowBishopConfig" runat="server" Text="<%$ Resources:StringDef, ShowConfigFile%>"
                                                                Enabled="true"></asp:LinkButton>
                                                        </td>
                                                    </tr>
                                                </table>
                                            </asp:Panel> </fieldset> </td></tr><tr><td><fieldset><legend><asp:CheckBox id="CheckBoxGoddess" runat="server" Text="<%$ Resources:StringDef,ServerTyperGoddess %>" AutoPostBack="true" OnCheckedChanged="CheckBoxGoddess_Click" Checked="false"></asp:CheckBox></legend><asp:Panel id="PanelGoddess" runat="server" Enabled="false">
                                                <table width="40%" style="margin: 5px,0px,5px,0px;" class="TN" cellspacing="1">
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef, ID %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxGoddessId" runat="server" CssClass="TextBox" Enabled="false" />
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, Name %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxGoddessName" runat="server" CssClass="TextBox" />
                                                            <asp:RequiredFieldValidator ID="RequiredFieldValidator14" runat="server" 							
							                                                        ControlToValidate="TextBoxGoddessName"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                       Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender26" TargetControlID="RequiredFieldValidator14" />
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal6" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxGoddessComment" runat="server" CssClass="TextBox" />
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal12" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxGoddessIpAddress" runat="server" CssClass="TextBox" />
                                                             <asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" 							
							                                                        ControlToValidate="TextBoxGoddessIpAddress"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                         Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender1" TargetControlID="RequiredFieldValidator1" />
						                                                          <asp:RegularExpressionValidator ID="ServerIpValidator1" runat="server"
					                                                                 ControlToValidate="TextBoxGoddessIpAddress"
							                                                         Display="None"
							                                                         SetFocusOnError="true"
							                                                            Enabled="false"
							                                                         ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
        	    	                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender12" TargetControlID="ServerIpValidator1" />	
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal13" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxGoddessDir" runat="server" CssClass="TextBox" onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>" />
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal20" runat="server" Text="<%$ Resources:StringDef, ConfigFile %>" />
                                                        </td>
                                                        <td align="center">
                                                            <asp:LinkButton ID="ButtonShowGoddessConfig" runat="server" Text="<%$ Resources:StringDef, ShowConfigFile%>"
                                                                Enabled="true"></asp:LinkButton>
                                                        </td>
                                                    </tr>
                                                </table>
                                            </asp:Panel> </fieldset> </td></tr><tr><td><fieldset><legend><asp:CheckBox id="CheckBoxGameCenter" runat="server" Text="<%$ Resources:StringDef,ServerTyperGameCenter %>" AutoPostBack="true" OnCheckedChanged="CheckBoxGameCenter_Click" Checked="false">
                                                </asp:CheckBox></legend><asp:Panel id="PanelGameCenter" runat="server" Enabled="false">
                                                <table width="40%" style="margin: 5px,0px,5px,0px;" class="TN" cellspacing="1">
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal15" runat="server" Text="<%$ Resources:StringDef, ID %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxGameCenterId" runat="server" CssClass="TextBox" Enabled="false" />
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal16" runat="server" Text="<%$ Resources:StringDef, Name %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxGameCenterName" runat="server" CssClass="TextBox" />
                                                            <asp:RequiredFieldValidator ID="RequiredFieldValidator15" runat="server" 							
							                                                        ControlToValidate="TextBoxGameCenterName"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                       Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender27" TargetControlID="RequiredFieldValidator15" />
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal17" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxGameCenterComment" runat="server" CssClass="TextBox" />
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal18" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxGameCenterIpAddress" runat="server" CssClass="TextBox" />
                                                              <asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" 							
							                                                        ControlToValidate="TextBoxGameCenterIpAddress"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                         Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender2" TargetControlID="RequiredFieldValidator2" />
						                                                        
						                                                         <asp:RegularExpressionValidator ID="ServerIpValidator2" runat="server"
					                                                                 ControlToValidate="TextBoxGameCenterIpAddress"
							                                                         Display="None"
							                                                         SetFocusOnError="true"
							                                                            Enabled="false"
							                                                         ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
        	    	                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender13" TargetControlID="ServerIpValidator2" />	
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal19" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="TextBoxGameCenterDir" runat="server" CssClass="TextBox" onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>" />
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal53" runat="server" Text="GMC地址" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="GMCIpTextBox" runat="server" CssClass="TextBox" />
                                                            <asp:RequiredFieldValidator ID="RequiredFieldValidator24" runat="server" 							
		                                                        ControlToValidate="GMCIpTextBox"
		                                                        Display="None"
		                                                        SetFocusOnError="true"
		                                                         Enabled="false"
		                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
	                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender36" TargetControlID="RequiredFieldValidator24" />
	                                                          <asp:RegularExpressionValidator ID="RegularExpressionValidator1" runat="server"
                                                                 ControlToValidate="GMCIpTextBox"
		                                                         Display="None"
		                                                         SetFocusOnError="true"
		                                                            Enabled="false"
		                                                         ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender37" TargetControlID="RegularExpressionValidator1" />	
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal54" runat="server" Text="GMC帐号" />
                                                        </td>
                                                        <td class="TCN">
                                                            <asp:TextBox ID="GMCAccountTextBox" runat="server" CssClass="TextBox" />
                                                            <asp:RequiredFieldValidator ID="RequiredFieldValidator25" runat="server" 							
		                                                        ControlToValidate="GMCAccountTextBox"
		                                                        Display="None"
		                                                        SetFocusOnError="true"
		                                                         Enabled="false"
		                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
	                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender38" TargetControlID="RequiredFieldValidator25" />
                                                        </td>
                                                    </tr>
                                                    <tr class="TableRowNormal">
                                                        <td class="TCH">
                                                            <asp:Literal ID="Literal39" runat="server" Text="<%$ Resources:StringDef, ConfigFile %>" />
                                                        </td>
                                                        <td align="center">
                                                            <asp:LinkButton ID="ButtonShowGameCenterConfig" runat="server" Text="<%$ Resources:StringDef, ShowConfigFile%>"
                                                                Enabled="true"></asp:LinkButton>
                                                        </td>
                                                    </tr>
                                                </table>
                                            </asp:Panel> </fieldset> </td></tr>
                                            
                                            <!-- 日志服务器开始 -->
                                            <tr>
                                                <td>
                                                    <fieldset>
                                                        <legend>
                                                            <asp:CheckBox ID="CheckBoxLogServer" runat="server" Text="<%$ Resources:StringDef,ServerTypeLogServer %>" AutoPostBack="true" OnCheckedChanged="CheckBoxLogServer_Click" Checked="false" />
                                                        </legend>
                                                        <asp:Panel ID="PanelLogServer" runat="server" Enabled="false">
                                                            <table style="MARGIN: 5px 0px" class="TN" cellspacing="1" width="40%">
                                                                <tbody>
                                                                    <tr class="TableRowNormal">
                                                                        <td class="TCH">
                                                                            <asp:Literal id="Literal55" runat="server" Text="<%$ Resources:StringDef, ID %>"></asp:Literal>
                                                                        </td>
                                                                        <td class="TCN">
                                                                            <asp:TextBox id="TextBoxLogServerId" runat="server" Enabled="false" CssClass="TextBox" />
                                                                        </td>
                                                                    </tr>
                                                                    <tr class="TableRowNormal">
                                                                        <td class="TCH">
                                                                            <asp:Literal id="Literal56" runat="server" Text="<%$ Resources:StringDef, Name %>" />
                                                                        </td>
                                                                        <td class="TCN">
                                                                            <asp:TextBox id="TextBoxLogServerName" runat="server" CssClass="TextBox" />
                                                                            <asp:RequiredFieldValidator ID="RequiredFieldValidator26" runat="server" ControlToValidate="TextBoxLogServerName" Display="None" SetFocusOnError="true" Enabled="false" ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
				                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender39" TargetControlID="RequiredFieldValidator26" />
                                                                        </td>
                                                                    </tr>                                                                    
                                                                    <tr class="TableRowNormal">
                                                                        <td class="TCH">
                                                                            <asp:Literal id="Literal57" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
                                                                        </td>
                                                                        <td class="TCN">
                                                                            <asp:TextBox id="TextBoxLogServerComment" runat="server" CssClass="TextBox" />
                                                                        </td>
                                                                    </tr>
                                                                    <tr class="TableRowNormal">
                                                                        <td class="TCH">
                                                                            <asp:Literal id="Literal58" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
                                                                        </td>
                                                                        <td class="TCN">
                                                                            <asp:TextBox id="TextBoxLogServerIpAddress" runat="server" CssClass="TextBox" />
                                                                            <asp:RequiredFieldValidator ID="RequiredFieldValidator27" runat="server" ControlToValidate="TextBoxLogServerIpAddress" Display="None" SetFocusOnError="true" Enabled="false" ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender40" TargetControlID="RequiredFieldValidator27" />
						                                                    <asp:RegularExpressionValidator ID="RegularExpressionValidator2" runat="server" ControlToValidate="TextBoxLogServerIpAddress" Display="None" SetFocusOnError="true" Enabled="false" ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
                                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender41" TargetControlID="RegularExpressionValidator2" />	
                                                                        </td>
                                                                    </tr>                                                                    
                                                                    <tr class="TableRowNormal">
                                                                        <td class="TCH">
                                                                            <asp:Literal id="Literal59" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
                                                                        </td>
                                                                        <td class="TCN">
                                                                            <asp:TextBox id="TextBoxLogServerDir" onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>" runat="server" CssClass="TextBox" /> 
                                                                        </td>
                                                                    </tr>
                                                                    <tr class="TableRowNormal">
                                                                        <td class="TCH">
                                                                            <asp:Literal id="Literal60" runat="server" Text="<%$ Resources:StringDef, ConfigFile %>" />
                                                                        </td>
                                                                        <td align="center">
                                                                            <asp:LinkButton id="ButtonShowLogServerConfig" runat="server" Text="<%$ Resources:StringDef, ShowConfigFile%>" Enabled="true" />
                                                                        </td>
                                                                    </tr>
                                                                </tbody>
                                                            </table>
                                                        </asp:Panel>
                                                    </fieldset>
                                                </td>
                                            </tr>
                                            <!-- 日志服务器结束 -->
                                            
                                            <tr><td><fieldset><legend><asp:CheckBox id="CheckBoxGameServer1" runat="server" Text="<%$ Resources:StringDef,ServerTypeGameServer %>" AutoPostBack="true" OnCheckedChanged="CheckBoxGameServer1_Click" Checked="false">
                                                </asp:CheckBox></legend><asp:Panel id="PanelGameServer1" runat="server" Enabled="false"><table style="MARGIN: 5px 0px" class="TN" cellspacing="1" width="40%"><tbody><tr class="TableRowNormal"><td class="TCH"><asp:Literal id="Literal21" runat="server" Text="<%$ Resources:StringDef, ID %>"></asp:Literal> </td>
                                                <td class="TCN"><asp:TextBox id="TextBoxGameServerId1" runat="server" Enabled="false" CssClass="TextBox"></asp:TextBox> </td>
                                                </tr>
                                                <tr class="TableRowNormal">
                                                <td class="TCH"><asp:Literal id="Literal22" runat="server" Text="<%$ Resources:StringDef, Name %>"></asp:Literal> </td>
                                                <td class="TCN">
                                                    <asp:TextBox id="TextBoxGameServerName1" runat="server" CssClass="TextBox"></asp:TextBox>
                                                    <asp:RequiredFieldValidator ID="RequiredFieldValidator16" runat="server" 							
					                                                        ControlToValidate="TextBoxGameServerName1"
					                                                        Display="None"
					                                                        SetFocusOnError="true"
					                                                       Enabled="false"
					                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
				                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender28" TargetControlID="RequiredFieldValidator16" />
                                                
                                                </td></tr>
                                                <tr class="TableRowNormal"><td class="TCH"><asp:Literal id="Literal23" runat="server" Text="<%$ Resources:StringDef, Comment %>"></asp:Literal> </td>
                                                <td class="TCN"><asp:TextBox id="TextBoxGameServerComment1" runat="server" CssClass="TextBox"></asp:TextBox> </td>
                                                </tr>
                                                <tr class="TableRowNormal"><td class="TCH"><asp:Literal id="Literal24" runat="server" Text="<%$ Resources:StringDef, Ip %>"></asp:Literal> </td>
                                                <td class="TCN"><asp:TextBox id="TextBoxGameServerIpAddress1" runat="server" CssClass="TextBox"></asp:TextBox> 
                                                
                                                 <asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server" 							
							                                                        ControlToValidate="TextBoxGameServerIpAddress1"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                         Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender3" TargetControlID="RequiredFieldValidator3" />
						                                                           <asp:RegularExpressionValidator ID="ServerIpValidator3" runat="server"
					                                                                 ControlToValidate="TextBoxGameServerIpAddress1"
							                                                         Display="None"
							                                                         SetFocusOnError="true"
							                                                            Enabled="false"
							                                                         ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
        	    	                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender14" TargetControlID="ServerIpValidator3" />	
                                                </td></tr>
                                                <tr class="TableRowNormal"><td class="TCH"><asp:Literal id="Literal25" runat="server" Text="<%$ Resources:StringDef, GameDir %>"></asp:Literal> </td>
                                                <td class="TCN"><asp:TextBox id="TextBoxGameServerDir1" onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>" runat="server" CssClass="TextBox"></asp:TextBox> </td></tr>
                                                <tr class="TableRowNormal"><td class="TCH"><asp:Literal id="Literal26" runat="server" Text="<%$ Resources:StringDef, AddMultiServer %>"></asp:Literal> </td>
                                                <td class="TCN"><asp:DropDownList id="GameServerCountDropDownList" runat="server" AutoPostBack="True" OnSelectedIndexChanged="GameServerCountDropDownList_SelectedIndexChanged"><asp:ListItem Value="1" Text="1台"></asp:ListItem>
                                                <asp:ListItem Text="2台" Value="2"></asp:ListItem>
                                                <asp:ListItem Text="3台" Value="3"></asp:ListItem>
                                                <asp:ListItem Text="4台" Value="4"></asp:ListItem>
                                                <asp:ListItem Text="5台" Value="5"></asp:ListItem>
                                                <asp:ListItem Text="6台" Value="6"></asp:ListItem>
                                                <asp:ListItem Text="7台" Value="7"></asp:ListItem>
                                                <asp:ListItem Text="8台" Value="8"></asp:ListItem>
                                                </asp:DropDownList> </td></tr>
                                                <tr class="TableRowNormal"><td class="TCH"><asp:Literal id="Literal40" runat="server" Text="<%$ Resources:StringDef, ConfigFile %>"></asp:Literal> </td>
                                                <td align="center"><asp:LinkButton id="ButtonShowGameServerConfig" runat="server" Text="<%$ Resources:StringDef, ShowConfigFile%>" Enabled="true"></asp:LinkButton> </td></tr>
                                                </tbody></table>
                                                </asp:Panel> </fieldset> </td></tr><!--  以下是显示多台服务器  --><!--游戏服务器2开始--><tr><td><asp:UpdatePanel id="UpdatePanelGameServer2" runat="server" UpdateMode="conditional" Visible="false"><ContentTemplate>
                                                <table border="0" cellspacing="0" cellpadding="0" width="100%" align="center">
                                                    <tr>
                                                        <td>
                                                            <fieldset>
                                                                <legend>
                                                                    <asp:CheckBox ID="CheckBoxGameServer2" runat="server" Text="<%$ Resources:StringDef,ServerTypeGameServer2 %>"
                                                                        AutoPostBack="true" OnCheckedChanged="CheckBoxGameServer2_Click" Checked="false">
                                                                    </asp:CheckBox></legend>
                                                                <asp:Panel ID="PanelGameServer2" runat="server" Enabled="false">
                                                                    <table width="40%" style="margin: 5px,0px,5px,0px;" class="TN" cellspacing="1">
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal31" runat="server" Text="<%$ Resources:StringDef, ID %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerId2" runat="server" CssClass="TextBox" Enabled="false" />
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal32" runat="server" Text="<%$ Resources:StringDef, Name %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerName2" runat="server" CssClass="TextBox" />
                                                                                <asp:RequiredFieldValidator ID="RequiredFieldValidator17" runat="server" 							
				                                                                                        ControlToValidate="TextBoxGameServerName2"
				                                                                                        Display="None"
				                                                                                        SetFocusOnError="true"
				                                                                                       Enabled="false"
				                                                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
			                                                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender29" TargetControlID="RequiredFieldValidator17" />
                               
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal33" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerComment2" runat="server" CssClass="TextBox" />
                                                                                <%--	<asp:RequiredFieldValidator ID="RequiredFieldValidator14" runat="server" 							
							                                                            ControlToValidate="TextBoxGameServerComment"
							                                                            Display="None"
							                                                            SetFocusOnError="true"
							                                                            ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender14" TargetControlID="RequiredFieldValidator1" />--%>
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal34" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerIpAddress2" runat="server" CssClass="TextBox" />
                                                                               
                                                 <asp:RequiredFieldValidator ID="RequiredFieldValidator4" runat="server" 							
							                                                        ControlToValidate="TextBoxGameServerIpAddress2"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                         Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender4" TargetControlID="RequiredFieldValidator4" />
						                                                          <asp:RegularExpressionValidator ID="ServerIpValidator4" runat="server"
					                                                                 ControlToValidate="TextBoxGameServerIpAddress2"
							                                                         Display="None"
							                                                         SetFocusOnError="true"
							                                                            Enabled="false"
							                                                         ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
        	    	                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender15" TargetControlID="ServerIpValidator4" />	
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal35" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerDir2" runat="server" CssClass="TextBox" onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>" />
                                                                                <%--<asp:RequiredFieldValidator ID="RequiredFieldValidator16" runat="server" 							
							                                                            ControlToValidate="TextBoxGameServerDir"
							                                                            Display="None"
							                                                            SetFocusOnError="true"
							                                                            ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender16" TargetControlID="RequiredFieldValidator4" />--%>
                                                                            </td>
                                                                        </tr>
                                                                        
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal id="Literal52" runat="server" Text="<%$ Resources:StringDef, ConfigFile %>" />
                                                                            </td>
                                                                            <td align="center">
                                                                                <table>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:CheckBox runat="server" ID="gameServerConfigCheckBox2" Checked="false" AutoPostBack="true" Text="与第一台游戏服务器采用相同配置" OnCheckedChanged="gameServerConfigCheckBox2_OnCheckedChanged" />
                                                                                        </td>
                                                                                    </tr>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:LinkButton id="ButtonShowGameServerConfig2" runat="server" Text="<%$ Resources:StringDef, ShowConfigFile%>" Enabled="true" />
                                                                                        </td>
                                                                                    </tr>
                                                                                </table>
                                                                            </td>
                                                                        </tr>                                                                                                                                                

                                                                    </table>
                                                                </asp:Panel>
                                                            </fieldset>
                                                        </td>
                                                    </tr>
                                                </table>
                                            
                                            </ContentTemplate>
                                            </asp:UpdatePanel> </td>
</tr><!--游戏服务器2结束--><!--游戏服务器3开始--><tr><td><asp:UpdatePanel id="UpdatePanelGameServer3" runat="server" UpdateMode="conditional" Visible="false"><ContentTemplate>
                                                <table border="0" cellspacing="0" cellpadding="0" width="100%"  align="center">
                                                    <tr>
                                                        <td>
                                                            <fieldset>
                                                                <legend>
                                                                    <asp:CheckBox ID="CheckBoxGameServer3" runat="server" Text="<%$ Resources:StringDef,ServerTypeGameServer3 %>"
                                                                        AutoPostBack="true" OnCheckedChanged="CheckBoxGameServer3_Click" Checked="false">
                                                                    </asp:CheckBox></legend>
                                                                <asp:Panel ID="PanelGameServer3" runat="server" Enabled="false">
                                                                    <table width="40%" style="margin: 5px,0px,5px,0px;" class="TN" cellspacing="1">
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Liter41" runat="server" Text="<%$ Resources:StringDef, ID %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerId3" runat="server" CssClass="TextBox" Enabled="false" />
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal42" runat="server" Text="<%$ Resources:StringDef, Name %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerName3" runat="server" CssClass="TextBox" />
                                                                                <asp:RequiredFieldValidator ID="RequiredFieldValidator18" runat="server" 							
				                                                                                        ControlToValidate="TextBoxGameServerName3"
				                                                                                        Display="None"
				                                                                                        SetFocusOnError="true"
				                                                                                       Enabled="false"
				                                                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
			                                                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender30" TargetControlID="RequiredFieldValidator18" />
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal43" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerComment3" runat="server" CssClass="TextBox" />
                                                                                <%--	<asp:RequiredFieldValidator ID="RequiredFieldValidator14" runat="server" 							
							                                                                ControlToValidate="TextBoxGameServerComment"
							                                                                Display="None"
							                                                                SetFocusOnError="true"
							                                                                ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                                <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender14" TargetControlID="RequiredFieldValidator1" />--%>
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal44" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerIpAddress3" runat="server" CssClass="TextBox" />
                                                                              <asp:RequiredFieldValidator ID="RequiredFieldValidator5" runat="server" 							
							                                                        ControlToValidate="TextBoxGameServerIpAddress3"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                         Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender5" TargetControlID="RequiredFieldValidator5" />
						                                                        
						                                                         <asp:RegularExpressionValidator ID="ServerIpValidator5" runat="server"
					                                                                 ControlToValidate="TextBoxGameServerIpAddress3"
							                                                         Display="None"
							                                                         SetFocusOnError="true"
							                                                            Enabled="false"
							                                                         ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
        	    	                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender16" TargetControlID="ServerIpValidator5" />	
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal45" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerDir3" runat="server" CssClass="TextBox" onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>" />
                                                                                <%--<asp:RequiredFieldValidator ID="RequiredFieldValidator16" runat="server" 							
							                                                            ControlToValidate="TextBoxGameServerDir"
							                                                            Display="None"
							                                                            SetFocusOnError="true"
							                                                            ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender16" TargetControlID="RequiredFieldValidator4" />--%>
                                                                            </td>
                                                                        </tr>
                                                                        
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal id="Literal41" runat="server" Text="<%$ Resources:StringDef, ConfigFile %>" />
                                                                            </td>
                                                                            <td align="center">
                                                                                <table>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:CheckBox runat="server" ID="gameServerConfigCheckBox3" Checked="false" AutoPostBack="true" Text="与第一台游戏服务器采用相同配置" OnCheckedChanged="gameServerConfigCheckBox3_OnCheckedChanged" />
                                                                                        </td>
                                                                                    </tr>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:LinkButton id="ButtonShowGameServerConfig3" runat="server" Text="<%$ Resources:StringDef, ShowConfigFile%>" Enabled="true" />
                                                                                        </td>
                                                                                    </tr>
                                                                                </table>
                                                                            </td>
                                                                        </tr>                                                                            
                                                                                                                                              
                                                                    </table>
                                                                </asp:Panel>
                                                            </fieldset>
                                                        </td>
                                                    </tr>
                                                </table>
                                            
</ContentTemplate>
</asp:UpdatePanel> </td></tr><!--游戏服务器3结束--><!--游戏服务器4开始--><tr><td><asp:UpdatePanel id="UpdatePanelGameServer4" runat="server" UpdateMode="conditional" Visible="false"><ContentTemplate>
                                                <table border="0" cellspacing="0" cellpadding="0" width="100%"  align="center">
                                                    <tr>
                                                        <td>
                                                            <fieldset>
                                                                <legend>
                                                                    <asp:CheckBox ID="CheckBoxGameServer4" runat="server" Text="<%$ Resources:StringDef,ServerTypeGameServer4 %>"
                                                                        AutoPostBack="true" OnCheckedChanged="CheckBoxGameServer4_Click" Checked="false">
                                                                    </asp:CheckBox></legend>
                                                                <asp:Panel ID="PanelGameServer4" runat="server" Enabled="false">
                                                                    <table width="40%" style="margin: 5px,0px,5px,0px;" class="TN" cellspacing="1">
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal61" runat="server" Text="<%$ Resources:StringDef, ID %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerId4" runat="server" CssClass="TextBox" Enabled="false" />
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal62" runat="server" Text="<%$ Resources:StringDef, Name %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerName4" runat="server" CssClass="TextBox" />
                                                                                <asp:RequiredFieldValidator ID="RequiredFieldValidator19" runat="server" 							
				                                                                                        ControlToValidate="TextBoxGameServerName4"
				                                                                                        Display="None"
				                                                                                        SetFocusOnError="true"
				                                                                                       Enabled="false"
				                                                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
			                                                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender31" TargetControlID="RequiredFieldValidator19" />
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal63" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerComment4" runat="server" CssClass="TextBox" />
                                                                                <%--	<asp:RequiredFieldValidator ID="RequiredFieldValidator14" runat="server" 							
							                                                            ControlToValidate="TextBoxGameServerComment"
							                                                            Display="None"
							                                                            SetFocusOnError="true"
							                                                            ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender14" TargetControlID="RequiredFieldValidator1" />--%>
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal64" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerIpAddress4" runat="server" CssClass="TextBox" />
                                                                                 <asp:RequiredFieldValidator ID="RequiredFieldValidator6" runat="server" 							
							                                                        ControlToValidate="TextBoxGameServerIpAddress4"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                         Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender6" TargetControlID="RequiredFieldValidator6" />
						                                                         <asp:RegularExpressionValidator ID="ServerIpValidator6" runat="server"
					                                                                 ControlToValidate="TextBoxGameServerIpAddress4"
							                                                         Display="None"
							                                                         SetFocusOnError="true"
							                                                            Enabled="false"
							                                                         ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
        	    	                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender17" TargetControlID="ServerIpValidator6" />	
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal65" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerDir4" runat="server" CssClass="TextBox" onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>" />
                                                                                <%--<asp:RequiredFieldValidator ID="RequiredFieldValidator16" runat="server" 							
							                                                        ControlToValidate="TextBoxGameServerDir"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender16" TargetControlID="RequiredFieldValidator4" />--%>
                                                                            </td>
                                                                        </tr>

                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal id="Literal46" runat="server" Text="<%$ Resources:StringDef, ConfigFile %>" />
                                                                            </td>
                                                                            <td align="center">
                                                                                <table>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:CheckBox runat="server" ID="gameServerConfigCheckBox4" Checked="false" AutoPostBack="true" Text="与第一台游戏服务器采用相同配置" OnCheckedChanged="gameServerConfigCheckBox4_OnCheckedChanged" />
                                                                                        </td>
                                                                                    </tr>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:LinkButton id="ButtonShowGameServerConfig4" runat="server" Text="<%$ Resources:StringDef, ShowConfigFile%>" Enabled="true" />
                                                                                        </td>
                                                                                    </tr>
                                                                                </table>
                                                                            </td>
                                                                        </tr>                                                                          
                                                                                                                                             
                                                                    </table>
                                                                </asp:Panel>
                                                            </fieldset>
                                                        </td>
                                                    </tr>
                                                </table>
                                            
</ContentTemplate>
</asp:UpdatePanel> </td></tr><!--游戏服务器4结束--><!--游戏服务器5开始--><tr><td><asp:UpdatePanel id="UpdatePanelGameServer5" runat="server" UpdateMode="conditional" Visible="false"><ContentTemplate>
                                                <table border="0" cellspacing="0" cellpadding="0" width="100%">
                                                    <tr>
                                                        <td>
                                                            <fieldset>
                                                                <legend>
                                                                    <asp:CheckBox ID="CheckBoxGameServer5" runat="server" Text="<%$ Resources:StringDef,ServerTypeGameServer5 %>"
                                                                        AutoPostBack="true" OnCheckedChanged="CheckBoxGameServer5_Click" Checked="false">
                                                                    </asp:CheckBox></legend>
                                                                <asp:Panel ID="PanelGameServer5" runat="server" Enabled="false">
                                                                    <table width="40%" style="margin: 5px,0px,5px,0px;" class="TN" cellspacing="1">
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal71" runat="server" Text="<%$ Resources:StringDef, ID %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerId5" runat="server" CssClass="TextBox" Enabled="false" />
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal72" runat="server" Text="<%$ Resources:StringDef, Name %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerName5" runat="server" CssClass="TextBox" />
                                                                                <asp:RequiredFieldValidator ID="RequiredFieldValidator20" runat="server" 							
				                                                                                        ControlToValidate="TextBoxGameServerName5"
				                                                                                        Display="None"
				                                                                                        SetFocusOnError="true"
				                                                                                       Enabled="false"
				                                                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
			                                                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender32" TargetControlID="RequiredFieldValidator20" />
                                                                                
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal73" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerComment5" runat="server" CssClass="TextBox" />
                                                                                <%--	<asp:RequiredFieldValidator ID="RequiredFieldValidator14" runat="server" 							
							                                                        ControlToValidate="TextBoxGameServerComment"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender14" TargetControlID="RequiredFieldValidator1" />--%>
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal74" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerIpAddress5" runat="server" CssClass="TextBox" />
                                                                              <asp:RequiredFieldValidator ID="RequiredFieldValidator7" runat="server" 							
							                                                        ControlToValidate="TextBoxGameServerIpAddress5"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                         Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender7" TargetControlID="RequiredFieldValidator7" />
						                                                         <asp:RegularExpressionValidator ID="ServerIpValidator7" runat="server"
					                                                                 ControlToValidate="TextBoxGameServerIpAddress5"
							                                                         Display="None"
							                                                         SetFocusOnError="true"
							                                                            Enabled="false"
							                                                         ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
        	    	                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender18" TargetControlID="ServerIpValidator7" />	
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal75" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerDir5" runat="server" CssClass="TextBox" onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>" />
                                                                                <%--<asp:RequiredFieldValidator ID="RequiredFieldValidator16" runat="server" 							
							                                                        ControlToValidate="TextBoxGameServerDir"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender16" TargetControlID="RequiredFieldValidator4" />--%>
                                                                            </td>
                                                                        </tr>
                                                                        
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal id="Literal47" runat="server" Text="<%$ Resources:StringDef, ConfigFile %>" />
                                                                            </td>
                                                                            <td align="center">
                                                                                <table>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:CheckBox runat="server" ID="gameServerConfigCheckBox5" Checked="false" AutoPostBack="true" Text="与第一台游戏服务器采用相同配置" OnCheckedChanged="gameServerConfigCheckBox5_OnCheckedChanged" />
                                                                                        </td>
                                                                                    </tr>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:LinkButton id="ButtonShowGameServerConfig5" runat="server" Text="<%$ Resources:StringDef, ShowConfigFile%>" Enabled="true" />
                                                                                        </td>
                                                                                    </tr>
                                                                                </table>
                                                                            </td>
                                                                        </tr>                                                                          
                                                                                                                                               
                                                                    </table>
                                                                </asp:Panel>
                                                            </fieldset>
                                                        </td>
                                                    </tr>
                                                </table>
                                            
</ContentTemplate>
</asp:UpdatePanel> </td></tr><!--游戏服务器5结束--><!--游戏服务器6开始--><tr><td><asp:UpdatePanel id="UpdatePanelGameServer6" runat="server" UpdateMode="conditional" Visible="false">
                                            <ContentTemplate>
                                                <table border="0" cellspacing="0" cellpadding="0" width="100%">
                                                    <tr>
                                                        <td>
                                                            <fieldset>
                                                                <legend>
                                                                    <asp:CheckBox ID="CheckBoxGameServer6" runat="server" Text="<%$ Resources:StringDef,ServerTypeGameServer6 %>"
                                                                        AutoPostBack="true" OnCheckedChanged="CheckBoxGameServer6_Click" Checked="false">
                                                                    </asp:CheckBox></legend>
                                                                <asp:Panel ID="PanelGameServer6" runat="server" Enabled="false">
                                                                    <table width="40%" style="margin: 5px,0px,5px,0px;" class="TN" cellspacing="1">
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal81" runat="server" Text="<%$ Resources:StringDef, ID %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerId6" runat="server" CssClass="TextBox" Enabled="false" />
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal82" runat="server" Text="<%$ Resources:StringDef, Name %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerName6" runat="server" CssClass="TextBox" />
                                                                                <asp:RequiredFieldValidator ID="RequiredFieldValidator21" runat="server" 							
				                                                                                        ControlToValidate="TextBoxGameServerName6"
				                                                                                        Display="None"
				                                                                                        SetFocusOnError="true"
				                                                                                       Enabled="false"
				                                                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
			                                                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender33" TargetControlID="RequiredFieldValidator21" />
                                                                                
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal83" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerComment6" runat="server" CssClass="TextBox" />
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal84" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerIpAddress6" runat="server" CssClass="TextBox" />
                                                                                 <asp:RequiredFieldValidator ID="RequiredFieldValidator8" runat="server" 							
							                                                        ControlToValidate="TextBoxGameServerIpAddress6"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                         Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender8" TargetControlID="RequiredFieldValidator8" />
						                                                         <asp:RegularExpressionValidator ID="ServerIpValidator8" runat="server"
					                                                                 ControlToValidate="TextBoxGameServerIpAddress6"
							                                                         Display="None"
							                                                         SetFocusOnError="true"
							                                                            Enabled="false"
							                                                         ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
        	    	                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender19" TargetControlID="ServerIpValidator8" />	
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal85" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerDir6" runat="server" CssClass="TextBox" onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>" />
                                                                            </td>
                                                                        </tr>
                                                                        
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal id="Literal48" runat="server" Text="<%$ Resources:StringDef, ConfigFile %>" />
                                                                            </td>
                                                                            <td align="center">
                                                                                <table>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:CheckBox runat="server" ID="gameServerConfigCheckBox6" Checked="false" AutoPostBack="true" Text="与第一台游戏服务器采用相同配置" OnCheckedChanged="gameServerConfigCheckBox6_OnCheckedChanged" />
                                                                                        </td>
                                                                                    </tr>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:LinkButton id="ButtonShowGameServerConfig6" runat="server" Text="<%$ Resources:StringDef, ShowConfigFile%>" Enabled="true" />
                                                                                        </td>
                                                                                    </tr>
                                                                                </table>
                                                                            </td>
                                                                        </tr>                                                                          
                                                                                                                                               
                                                                    </table>
                                                                </asp:Panel>
                                                            </fieldset>
                                                        </td>
                                                    </tr>
                                                </table>
                                            </ContentTemplate>
                                        </asp:UpdatePanel> </td></tr><!--游戏服务器6结束--><!--游戏服务器7开始--><tr><td><asp:UpdatePanel id="UpdatePanelGameServer7" runat="server" UpdateMode="conditional" Visible="false">
                                            <ContentTemplate>
                                                <table border="0" cellspacing="0" cellpadding="0" width="100%">
                                                    <tr>
                                                        <td>
                                                            <fieldset>
                                                                <legend>
                                                                    <asp:CheckBox ID="CheckBoxGameServer7" runat="server" Text="<%$ Resources:StringDef,ServerTypeGameServer7 %>"
                                                                        AutoPostBack="true" OnCheckedChanged="CheckBoxGameServer7_Click" Checked="false">
                                                                    </asp:CheckBox></legend>
                                                                <asp:Panel ID="PanelGameServer7" runat="server" Enabled="false">
                                                                    <table width="40%" style="margin: 5px,0px,5px,0px;" class="TN" cellspacing="1">
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal91" runat="server" Text="<%$ Resources:StringDef, ID %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerId7" runat="server" CssClass="TextBox" Enabled="false" />
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal92" runat="server" Text="<%$ Resources:StringDef, Name %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerName7" runat="server" CssClass="TextBox" />
                                                                                <asp:RequiredFieldValidator ID="RequiredFieldValidator22" runat="server" 							
				                                                                                        ControlToValidate="TextBoxGameServerName7"
				                                                                                        Display="None"
				                                                                                        SetFocusOnError="true"
				                                                                                       Enabled="false"
				                                                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
			                                                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender34" TargetControlID="RequiredFieldValidator22" />
                                                                                
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal93" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerComment7" runat="server" CssClass="TextBox" />
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal94" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerIpAddress7" runat="server" CssClass="TextBox" />
                                                                                 <asp:RequiredFieldValidator ID="RequiredFieldValidator9" runat="server" 							
							                                                        ControlToValidate="TextBoxGameServerIpAddress7"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                         Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender9" TargetControlID="RequiredFieldValidator9" />
						                                                         <asp:RegularExpressionValidator ID="ServerIpValidator9" runat="server"
					                                                                 ControlToValidate="TextBoxGameServerIpAddress7"
							                                                         Display="None"
							                                                         SetFocusOnError="true"
							                                                            Enabled="false"
							                                                         ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
        	    	                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender20" TargetControlID="ServerIpValidator9" />	
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal95" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerDir7" runat="server" CssClass="TextBox" onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>" />
                                                                            </td>
                                                                        </tr>
                                                                        
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal id="Literal49" runat="server" Text="<%$ Resources:StringDef, ConfigFile %>" />
                                                                            </td>
                                                                            <td align="center">
                                                                                <table>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:CheckBox runat="server" ID="gameServerConfigCheckBox7" Checked="false" AutoPostBack="true" Text="与第一台游戏服务器采用相同配置" OnCheckedChanged="gameServerConfigCheckBox7_OnCheckedChanged" />
                                                                                        </td>
                                                                                    </tr>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:LinkButton id="ButtonShowGameServerConfig7" runat="server" Text="<%$ Resources:StringDef, ShowConfigFile%>" Enabled="true" />
                                                                                        </td>
                                                                                    </tr>
                                                                                </table>
                                                                            </td>
                                                                        </tr>                                                                          
                                                                                                                                             
                                                                    </table>
                                                                </asp:Panel>
                                                            </fieldset>
                                                        </td>
                                                    </tr>
                                                </table>
                                            </ContentTemplate>
                                        </asp:UpdatePanel> </td></tr><!--游戏服务器7结束--><!--游戏服务器8开始--><tr><td><asp:UpdatePanel id="UpdatePanelGameServer8" runat="server" UpdateMode="conditional" Visible="false">
                                            <ContentTemplate>
                                                <table border="0" cellspacing="0" cellpadding="0" width="100%">
                                                    <tr>
                                                        <td>
                                                            <fieldset>
                                                                <legend>
                                                                    <asp:CheckBox ID="CheckBoxGameServer8" runat="server" Text="<%$ Resources:StringDef,ServerTypeGameServer8 %>"
                                                                        AutoPostBack="true" OnCheckedChanged="CheckBoxGameServer8_Click" Checked="false">
                                                                    </asp:CheckBox></legend>
                                                                <asp:Panel ID="PanelGameServer8" runat="server" Enabled="false">
                                                                    <table width="40%" style="margin: 5px,0px,5px,0px;" class="TN" cellspacing="1">
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal101" runat="server" Text="<%$ Resources:StringDef, ID %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerId8" runat="server" CssClass="TextBox" Enabled="false" />
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal102" runat="server" Text="<%$ Resources:StringDef, Name %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerName8" runat="server" CssClass="TextBox" />
                                                                                <asp:RequiredFieldValidator ID="RequiredFieldValidator23" runat="server" 							
				                                                                                        ControlToValidate="TextBoxGameServerName8"
				                                                                                        Display="None"
				                                                                                        SetFocusOnError="true"
				                                                                                       Enabled="false"
				                                                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
			                                                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender35" TargetControlID="RequiredFieldValidator23" />
                                                                                
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal103" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerComment8" runat="server" CssClass="TextBox" />
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal104" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerIpAddress8" runat="server" CssClass="TextBox" />
                                                                                 <asp:RequiredFieldValidator ID="RequiredFieldValidator10" runat="server" 							
							                                                        ControlToValidate="TextBoxGameServerIpAddress8"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                         Enabled="false"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender10" TargetControlID="RequiredFieldValidator10" />
						                                                         <asp:RegularExpressionValidator ID="ServerIpValidator10" runat="server"
					                                                                 ControlToValidate="TextBoxGameServerIpAddress8"
							                                                         Display="None"
							                                                         SetFocusOnError="true"
							                                                            Enabled="false"
							                                                         ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
        	    	                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender21" TargetControlID="ServerIpValidator10" />	
                                                                            </td>
                                                                        </tr>
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal ID="Literal105" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
                                                                            </td>
                                                                            <td class="TCN">
                                                                                <asp:TextBox ID="TextBoxGameServerDir8" runat="server" CssClass="TextBox" onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>" />
                                                                            </td>
                                                                        </tr>
                                                                        
                                                                        <tr class="TableRowNormal">
                                                                            <td class="TCH">
                                                                                <asp:Literal id="Literal50" runat="server" Text="<%$ Resources:StringDef, ConfigFile %>" />
                                                                            </td>
                                                                            <td align="center">
                                                                                <table>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:CheckBox runat="server" ID="gameServerConfigCheckBox8" Checked="false" AutoPostBack="true" Text="与第一台游戏服务器采用相同配置" OnCheckedChanged="gameServerConfigCheckBox8_OnCheckedChanged" />
                                                                                        </td>
                                                                                    </tr>
                                                                                    <tr>
                                                                                        <td>
                                                                                            <asp:LinkButton id="ButtonShowGameServerConfig8" runat="server" Text="<%$ Resources:StringDef, ShowConfigFile%>" Enabled="true" />
                                                                                        </td>
                                                                                    </tr>
                                                                                </table>
                                                                            </td>
                                                                        </tr>                                                                           
                                                                                                                                               
                                                                    </table>
                                                                </asp:Panel>
                                                            </fieldset>
                                                        </td>
                                                    </tr>
                                                </table>
                                            </ContentTemplate>
                                        </asp:UpdatePanel>
                                    </td>
                                </tr>
        <tr class="TableRowSpacing" />	
        <tr class="TableRowSpacing" />	
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
                                <!--游戏服务器8结束-->
                            </ContentTemplate>
</asp:UpdatePanel>
     
</asp:Content>
