<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_EditServer, App_Web_jvyc5ygz" title="<%$ Resources:StringDef, EditServer %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerConfigTextBox" Src="~/Common/ServerConfigTextBox.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="FtpDropDownList" Src="~/Common/FtpServerDropdownlist.ascx" %>
                                        
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:UpdatePanel id="UpdatePanelConfig" runat="server" UpdateMode="conditional">
<ContentTemplate>
<table width="80%" align="center">
	<tr>
		<td align="center">
			<table border="0" class="TN" cellpadding="0" cellspacing="1">
				<caption class="TableCaption"><asp:Literal ID="Literal11" runat="server" Text="<%$ Resources:StringDef, GameServerConfig %>" /></caption>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal10" runat="server" Text="<%$ Resources:StringDef, ID %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBoxServerId" runat="server" CssClass="TextBox" Enabled="false" />						
					</td>
					
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal9" runat="server" Text="<%$ Resources:StringDef, Name %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBoxServerName" runat="server" CssClass="TextBox" />						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator5" runat="server" 							
							ControlToValidate="TextBoxServerName"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender4" TargetControlID="RequiredFieldValidator5" />															                        				    				        					        
				    </td>
				    
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal8" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBoxServerComment" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" 							
							ControlToValidate="TextBoxServerComment"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender2" TargetControlID="RequiredFieldValidator1" />
					</td>
					
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal7" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBoxServerIpAddress" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server" 							
							ControlToValidate="TextBoxServerIpAddress"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender1" TargetControlID="RequiredFieldValidator3" />
		                <asp:RegularExpressionValidator ID="ServerIpValidator" runat="server"
					         ControlToValidate="TextBoxServerIpAddress"
							 Display="None"
							 SetFocusOnError="true"
							 ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
        	    	    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender5" TargetControlID="ServerIpValidator" />		            										
					</td>
					
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
					</td>
					<td class="TCN" >
						<asp:TextBox ID="TextBoxGameDir" runat="server" CssClass="TextBox" Enabled="true"  onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>"  />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator4" runat="server" 							
							ControlToValidate="TextBoxGameDir"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender3" TargetControlID="RequiredFieldValidator4" />
					</td>
					
				</tr>
                <tr class="TableRowNormal">
                    <td class="TCH">
                        <asp:Literal ID="Literal13" runat="server" Text="<%$ Resources:StringDef, FtpDownload %>" />
                    </td>
                    <td class="TCN">
						<Blaze:FtpDropDownList ID="FtpDropDownList" runat="server" />						
					</td>                   
                </tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, ServerType %>" />
					</td>
					<td class="TCN">
						<asp:DropDownList ID="ListBoxServerType"  runat="server" AutoPostBack="true"  OnSelectedIndexChanged="ListBoxServerType_OnSelectedIndexChanged">
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTypeUnidentified %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTypeGameServer %>" Value="1" />
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTypeDatabaseServer %>" Value="2" />
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTyperBishop %>" Value="3" />
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTyperGoddess %>" Value="4" />
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTyperGameCenter %>" Value="5" />
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTyperPaysys %>" Value="6" />
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTypeLogServer %>" Value="7" />
						</asp:DropDownList>
					</td>
					
				</tr>				
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef, ServerGroup %>" />
					</td>
					<td class="TCN">
						<Blaze:ServerGroupDropDownList ID="ServerGroupDropDownList" runat="server" />						
					</td>					
				</tr>
			</table>
		</td>
    <tr class="TableRowSpacing" />
    <tr>
        <td align="center">
            <asp:Label ID="LabelLordIsConfig" runat="server" SkinID="Failure" Visible="false" Text="<%$ Resources:StringDef,MsgHasNoLordConfig %>" />
        </td>
    </tr>
	<tr>
		<td align="center" >
			<asp:Panel ID="PanelLordConfig" runat="server" Width="100%" Visible="false">
			<table border="0" class="TN">
				<caption class='TableCaption'><asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef, LordConfig %>" /></caption>
				<tr>
                <td>
                    <fieldset>
                        <legend><asp:Literal runat="server" Text="<%$ Resources:StringDef,BasicConfig %>" /></legend>
                        <table width="95%" style="margin:5px,0px,5px,0px;" class="TN" cellspacing="1">                                
                            <tr>
			            		<td class="TCH" width="30%">
			            			<asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, ServerCount %>" />
			            		</td>
			            		<td class="TCN">
			            			<asp:TextBox ID="TextBoxLordPort" runat="server" />
			            		</td>
			            	</tr>
			            	<tr>
			            		<td class="TCH">
			            			<asp:Literal ID="Literal12" runat="server" Text="<%$ Resources:StringDef, ServerIDs %>" />
			            		</td>
			            		<td class="TCN">
			            			<asp:TextBox ID="TextBoxMaxPlayer" runat="server" />
			            		</td>
			            	</tr>	
			           
				            <tr>
		            			<td class="TCH" width="30%">
		            				<asp:Literal ID="Literal24" runat="server" Text="<%$ Resources:StringDef, FSEyeIp %>" />
		            			</td>
		            			<td class="TCN">
		            				<asp:TextBox ID="TextBoxFSEyeIp" runat="server"/>
		            			</td>							    		            			
		            		</tr>
		            		<tr>
		            			<td class="TCH">
		            				<asp:Literal ID="Literal25" runat="server" Text="<%$ Resources:StringDef, FSEyePort %>" />
		            			</td>
		            			<td class="TCN">
		            				<asp:TextBox ID="TextBoxFSEyePort" runat="server" Enabled="false"/>
		            			</td>
		            		</tr>
		            		<tr runat="server" id="GMCIpTr" visible="false">
		            		    <td class="TCH">
		            		        <asp:Literal ID="Literal53" runat="server" Text="GMC地址" />
		            		    </td>
		            		    <td class="TCN">
                                    <asp:TextBox ID="GMCIpTextBox" runat="server" CssClass="TextBox" />
                                    <asp:RequiredFieldValidator ID="RequiredFieldValidator24" runat="server" 							
                                        ControlToValidate="GMCIpTextBox"
                                        Display="None"
                                        SetFocusOnError="true"
                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender36" TargetControlID="RequiredFieldValidator24" />
                                      <asp:RegularExpressionValidator ID="RegularExpressionValidator1" runat="server"
                                         ControlToValidate="GMCIpTextBox"
                                         Display="None"
                                         SetFocusOnError="true"
                                         ErrorMessage="非法的IP地址" ValidationExpression="^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$" />
                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender37" TargetControlID="RegularExpressionValidator1" />	
		            		    </td>
		            		</tr>
		            		<tr runat="server" id="GMCAccountTr" visible="false">
		            		    <td class="TCH">
		            		        <asp:Literal ID="Literal1" runat="server" Text="GMC帐号" />
		            		    </td>
		            		    <td class="TCN">
                                    <asp:TextBox ID="GMCAccountTextBox" runat="server" CssClass="TextBox" />
                                    <asp:RequiredFieldValidator ID="RequiredFieldValidator25" runat="server" 							
                                        ControlToValidate="GMCAccountTextBox"
                                        Display="None"
                                        SetFocusOnError="true"
                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender38" TargetControlID="RequiredFieldValidator25" />
		            		    </td>
		            		</tr>
                        </table>
                    </fieldset>
				</td>
				</tr>
				<tr class="TableRowSpacing" />
				<tr>
                </tr> 
				
				<tr class="TableRowSpacing" />
				<tr>
					<td colspan="2" align="center">
						<asp:LinkButton ID="SetToDefaultLordConfig"  runat="server" Text="<%$ Resources:StringDef, SetToDefaultLordConfig %>" OnClick="SetToDefaultLordConfig_Click" />
						<asp:LinkButton ID="SaveAsDefaultLordConfig" runat="server" Text="<%$ Resources:StringDef, SaveAsDefaultConfig %>"  OnClick="SaveAsDefaultLordConfig_Click" />
						<asp:HyperLink ID="HyperLinkViewLordConfigFile" runat="server" Text="<%$ Resources:StringDef, ViewLordConfigFile %>" Target="_blank" CssClass="LinkButton" />
				<asp:LinkButton ID="LinkButtonReconfigGuard" runat="server" Text="<%$ Resources:StringDef, ReconfigreGuard %>" OnClick="LinkButtonReconfigGuard_Click" />
						<asp:LinkButton ID="LinkButtonAutoDeployLordConfig" runat="server" Text="<%$ Resources:StringDef, AutoDeployLordConfig %>" CssClass="LinkButton" OnClick="LinkButtonAutoDeployLordConfig_Click" Visible="False" />
					</td>
				</tr>
			</table>
			</asp:Panel>
		<%--服务器配置的panel开始--%>
            <tr width="100%" height="100%">
                <td align="center" width="100%" height="100%">
                    <asp:Panel ID="PanelServerConfig" runat="server" Width="100%" Height="100%" Visible="false">
                        <table border="0" class="TN" width="100%" height="100%">
                            <tr>
                                <td>
                                    <fieldset>
                                        <legend>
                                            <asp:Literal ID="Literal6" runat="server" Text="<%$ Resources:StringDef,ShowServerConfig %>" /></legend>
                                        <table width="100%" style="margin: 5px,0px,5px,0px;" class="TN" cellspacing="1">
                                            <tr>
                                                <td width="100%" height="100%">
                                                    <Blaze:ServerConfigTextBox ID="ServerConfig" runat="server"></Blaze:ServerConfigTextBox>
                                                </td>
                                            </tr>
                                            <tr>
                                                <td colspan="2" align="center" style="height: 21px">
                                                </td>
                                            </tr>
                                        </table>
                                    </fieldset>
                                    <asp:LinkButton ID="ButtonSendFile" runat="server" Text="<%$ Resources:StringDef, UploadServerConfigFile %>"
                                        OnClick="ButtonSendFile_Click" /></td>
                            </tr>
                            <tr class="TableRowSpacing" />
                        </table>
                    </asp:Panel>
                </td>
            </tr>
	    <%--服务器配置的panel结束--%>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:Label ID="LabelSuccess" runat="server" SkinID="MessageSuccess" Visible="false" EnableViewState="false" />
			<asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
            <asp:LinkButton ID="ButtonShowLordConfig" runat="server" Text="<%$ Resources:StringDef, ShowLordConfig %>" OnClick="ButtonShowLordConfig_Click" />
			<asp:LinkButton ID="ButtonShowServerConfig" runat="server" Text="<%$ Resources:StringDef, ShowConfigFile%>" OnClick="ButtonShowServerConfig_Click" />&nbsp;
				<asp:LinkButton ID="LinkButtonEdit" runat="server" OnClick="LinkButtonSave_Click" />&nbsp;
			<asp:HyperLink runat="server" NavigateUrl="AddAndDelete.aspx" Text="<%$ Resources:StringDef, Return %>" />
		</td>
	</tr>
</table>
</ContentTemplate>
</asp:UpdatePanel>
 
<table width="80%" align="center">
	<tr>
		<td align="center">
			<asp:UpdateProgress ID="UpdateProgress2" runat="server" AssociatedUpdatePanelID="UpdatePanelConfig">
				<ProgressTemplate>
					<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileOperating %>"
						SkinID="Operating" />
				</ProgressTemplate>
			</asp:UpdateProgress>
		</td>
	</tr>
</table>
</asp:Content>