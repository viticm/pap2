<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_AddServerSameType, App_Web_jvyc5ygz" title="Untitled Page" theme="BlazeSky" %>
                                        
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:UpdatePanel id="UpdatePanel1" runat="server" UpdateMode="Conditional">
<ContentTemplate>

<table width="100%" align="center">
<caption class="TableCaption"><asp:Literal ID="Literal11" runat="server" Text="<%$ Resources:StringDef, AddSameTypeServer %>" /></caption>

<tr class="TableRowNormal">
<table width="50%" align="center">
    <tr>
	    <td align="center">
	        <table border="0" class="TN" cellpadding="0" cellspacing="1">
	            <tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, ServerType %>" />
					</td>
					<td class="TCN">
						<asp:DropDownList ID="ListBoxServerType"  runat="server">
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTypeUnidentified %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTypeGameServer %>" Value="1" />
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTypeDatabaseServer %>" Value="2" />
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTyperBishop %>" Value="3" />
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTyperGoddess %>" Value="4" />
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTyperGameCenter %>" Value="5" />
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTyperPaysys %>" Value="6" />
						</asp:DropDownList>
					</td>
				</tr>
				          
	            <tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef, Count %>" />
					</td>
					<td class="TCN">
						<asp:DropDownList ID="DropDownList1"  runat="server">
							<asp:ListItem Text="<%$ Resources:StringDef, ServerTypeUnidentified %>" Value="0" />
							<asp:ListItem Text="2台" Value="2" />
							<asp:ListItem Text="3台" Value="3" />
							<asp:ListItem Text="4台" Value="4" />
							<asp:ListItem Text="5台" Value="5" />
							<asp:ListItem Text="6台" Value="6" />
							<asp:ListItem Text="7台" Value="7" />
							<asp:ListItem Text="8台" Value="8" />
						</asp:DropDownList>
					</td>
				</tr>
	            
	            <tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef, FSEyeIp %>" />
					</td>
					<td class="TCN">
					    <asp:TextBox ID="TextBoxFSEyeAddress" runat="server"></asp:TextBox>
					</td>
				</tr>								
			</table>
		</td>
	</tr>
</table>
</tr>

<tr class="TableRowSpacing" />
<tr class="TableRowSpacing" />
<tr class="TableRowSpacing" />
<tr class="TableRowSpacing" />
<tr class="TableRowSpacing" />

<table width="80%" align="center">
<tr class="TableRowNormal">
<td>
<asp:Panel ID="Panel1" runat="server" Visible="true">
<table width="50%" align="left">
	<tr>
		<td align="center">
			<table class="TN">
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
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
					</td>
					<td class="TCN" >
						<asp:TextBox ID="TextBoxGameDir" runat="server" CssClass="TextBox"  onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>"  />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator4" runat="server" 							
							ControlToValidate="TextBoxGameDir"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender3" TargetControlID="RequiredFieldValidator4" />
					</td>
				</tr>							
			</table>
		</td>
	</tr>
</table>

				            <table width="50%" align="left">
				            	<tr>
		                        <td align="center">
		                        <table class="TN">
			                	<tr class="TableRowNormal">
			                		<td class="TCH" width="30%">
			                			<asp:Literal ID="Literal20" runat="server" Text="<%$ Resources:StringDef, DbHost %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBoxDbHost" runat="server" />
			                		</td>
			                	</tr>
		                		<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal21" runat="server" Text="<%$ Resources:StringDef, DbName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBoxDbName" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal22" runat="server" Text="<%$ Resources:StringDef, DbLoginName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBoxDbLoginName" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="LiteralDbBackupPath" runat="server" Text="<%$ Resources:StringDef, DbBackupPath %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBoxDbBackupPath" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal23" runat="server" Text="<%$ Resources:StringDef, DbLoginPwd %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBoxDbLoginPwd" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal27" runat="server" Text="<%$ Resources:StringDef, PasswordConfirm %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBoxDbLoginPwdConfirm" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			               </table>
			               </td>
			               </tr>
			               </table>
</asp:Panel>
</td>
</tr>

<tr class="TableRowSpacing" />
<tr class="TableRowSpacing" />

<tr class="TableRowNormal">
<td>
<asp:Panel ID="Panel2" runat="server" Visible="true">
<table width="50%" align="left">
	<tr>
		<td align="center">
			<table class="TN">
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, ID %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox1" runat="server" CssClass="TextBox" Enabled="false" />						
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal6" runat="server" Text="<%$ Resources:StringDef, Name %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox2" runat="server" CssClass="TextBox" />						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" 							
							ControlToValidate="TextBoxServerName"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender5" TargetControlID="RequiredFieldValidator5" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal12" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox3" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator6" runat="server" 							
							ControlToValidate="TextBoxServerComment"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender6" TargetControlID="RequiredFieldValidator1" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal13" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox4" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator7" runat="server" 							
							ControlToValidate="TextBoxServerIpAddress"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender7" TargetControlID="RequiredFieldValidator3" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal14" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
					</td>
					<td class="TCN" >
						<asp:TextBox ID="TextBox5" runat="server" CssClass="TextBox"  onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>"  />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator8" runat="server" 							
							ControlToValidate="TextBoxGameDir"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender8" TargetControlID="RequiredFieldValidator4" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal92" runat="server" Text="与首个服务器数据库配置相同" />
					</td>
					<td class="TCN">
						<asp:CheckBox ID="ConfigCheckBox1"	 runat="server" Checked="false" />				
					</td>
				</tr>							
			</table>
</table>

				            <table width="50%" align="left">
				            	<tr>
		                        <td align="center">
		                        <table class="TN">
			                	<tr class="TableRowNormal">
			                		<td class="TCH" width="30%">
			                			<asp:Literal ID="Literal15" runat="server" Text="<%$ Resources:StringDef, DbHost %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox6" runat="server" />
			                		</td>
			                	</tr>
		                		<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal16" runat="server" Text="<%$ Resources:StringDef, DbName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox7" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal17" runat="server" Text="<%$ Resources:StringDef, DbLoginName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox8" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal18" runat="server" Text="<%$ Resources:StringDef, DbBackupPath %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox9" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal19" runat="server" Text="<%$ Resources:StringDef, DbLoginPwd %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox10" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal24" runat="server" Text="<%$ Resources:StringDef, PasswordConfirm %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox11" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			               </table>
			               </td>
			               </tr>
			               </table>
</asp:Panel>    			       
</td>
</tr>


<tr class="TableRowSpacing" />
<tr class="TableRowSpacing" />

<tr class="TableRowNormal">
<td>
<asp:Panel ID="Panel3" runat="server" Visible="true">
<table width="50%" align="left">
	<tr>
		<td align="center">
			<table class="TN">
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal25" runat="server" Text="<%$ Resources:StringDef, ID %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox12" runat="server" CssClass="TextBox" Enabled="false" />						
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal26" runat="server" Text="<%$ Resources:StringDef, Name %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox13" runat="server" CssClass="TextBox" />						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator9" runat="server" 							
							ControlToValidate="TextBoxServerName"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender9" TargetControlID="RequiredFieldValidator5" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal28" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox14" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator10" runat="server" 							
							ControlToValidate="TextBoxServerComment"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender10" TargetControlID="RequiredFieldValidator1" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal29" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox15" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator11" runat="server" 							
							ControlToValidate="TextBoxServerIpAddress"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender11" TargetControlID="RequiredFieldValidator3" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal30" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
					</td>
					<td class="TCN" >
						<asp:TextBox ID="TextBox16" runat="server" CssClass="TextBox"  onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>"  />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator12" runat="server" 							
							ControlToValidate="TextBoxGameDir"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender12" TargetControlID="RequiredFieldValidator4" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal31" runat="server" Text="与首个服务器数据库配置相同" />
					</td>
					<td class="TCN">
						<asp:CheckBox ID="CheckBox1"	 runat="server" Checked="false" />				
					</td>
				</tr>							
			</table>
</table>

				            <table width="50%" align="left">
				            	<tr>
		                        <td align="center">
		                        <table class="TN">
			                	<tr class="TableRowNormal">
			                		<td class="TCH" width="30%">
			                			<asp:Literal ID="Literal32" runat="server" Text="<%$ Resources:StringDef, DbHost %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox17" runat="server" />
			                		</td>
			                	</tr>
		                		<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal33" runat="server" Text="<%$ Resources:StringDef, DbName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox18" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal34" runat="server" Text="<%$ Resources:StringDef, DbLoginName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox19" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal35" runat="server" Text="<%$ Resources:StringDef, DbBackupPath %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox20" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal36" runat="server" Text="<%$ Resources:StringDef, DbLoginPwd %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox21" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal37" runat="server" Text="<%$ Resources:StringDef, PasswordConfirm %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox22" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			               </table>
			               </td>
			               </tr>
			               </table>
</asp:Panel>    			       
</td>
</tr>


<tr class="TableRowSpacing" />
<tr class="TableRowSpacing" />

<tr class="TableRowNormal">
<td>
<asp:Panel ID="Panel4" runat="server" Visible="true">
<table width="50%" align="left">
	<tr>
		<td align="center">
			<table class="TN">
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal38" runat="server" Text="<%$ Resources:StringDef, ID %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox23" runat="server" CssClass="TextBox" Enabled="false" />						
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal39" runat="server" Text="<%$ Resources:StringDef, Name %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox24" runat="server" CssClass="TextBox" />						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator13" runat="server" 							
							ControlToValidate="TextBoxServerName"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender13" TargetControlID="RequiredFieldValidator5" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal40" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox25" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator14" runat="server" 							
							ControlToValidate="TextBoxServerComment"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender14" TargetControlID="RequiredFieldValidator1" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal41" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox26" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator15" runat="server" 							
							ControlToValidate="TextBoxServerIpAddress"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender15" TargetControlID="RequiredFieldValidator3" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal42" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
					</td>
					<td class="TCN" >
						<asp:TextBox ID="TextBox27" runat="server" CssClass="TextBox"  onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>"  />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator16" runat="server" 							
							ControlToValidate="TextBoxGameDir"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender16" TargetControlID="RequiredFieldValidator4" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal43" runat="server" Text="与首个服务器数据库配置相同" />
					</td>
					<td class="TCN">
						<asp:CheckBox ID="CheckBox2"	 runat="server" Checked="false" />				
					</td>
				</tr>							
			</table>
</table>

				            <table width="50%" align="left">
				            	<tr>
		                        <td align="center">
		                        <table class="TN">
			                	<tr class="TableRowNormal">
			                		<td class="TCH" width="30%">
			                			<asp:Literal ID="Literal44" runat="server" Text="<%$ Resources:StringDef, DbHost %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox28" runat="server" />
			                		</td>
			                	</tr>
		                		<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal45" runat="server" Text="<%$ Resources:StringDef, DbName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox29" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal46" runat="server" Text="<%$ Resources:StringDef, DbLoginName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox30" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal47" runat="server" Text="<%$ Resources:StringDef, DbBackupPath %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox31" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal48" runat="server" Text="<%$ Resources:StringDef, DbLoginPwd %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox32" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal49" runat="server" Text="<%$ Resources:StringDef, PasswordConfirm %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox33" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			               </table>
			               </td>
			               </tr>
			               </table>
</asp:Panel>    			       
</td>
</tr>


<tr class="TableRowSpacing" />
<tr class="TableRowSpacing" />

<tr class="TableRowNormal">
<td>
<asp:Panel ID="Panel5" runat="server" Visible="true">
<table width="50%" align="left">
	<tr>
		<td align="center">
			<table class="TN">
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal50" runat="server" Text="<%$ Resources:StringDef, ID %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox34" runat="server" CssClass="TextBox" Enabled="false" />						
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal51" runat="server" Text="<%$ Resources:StringDef, Name %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox35" runat="server" CssClass="TextBox" />						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator17" runat="server" 							
							ControlToValidate="TextBoxServerName"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender17" TargetControlID="RequiredFieldValidator5" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal52" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox36" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator18" runat="server" 							
							ControlToValidate="TextBoxServerComment"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender18" TargetControlID="RequiredFieldValidator1" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal53" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox37" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator19" runat="server" 							
							ControlToValidate="TextBoxServerIpAddress"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender19" TargetControlID="RequiredFieldValidator3" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal54" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
					</td>
					<td class="TCN" >
						<asp:TextBox ID="TextBox38" runat="server" CssClass="TextBox"  onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>"  />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator20" runat="server" 							
							ControlToValidate="TextBoxGameDir"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender20" TargetControlID="RequiredFieldValidator4" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal55" runat="server" Text="与首个服务器数据库配置相同" />
					</td>
					<td class="TCN">
						<asp:CheckBox ID="CheckBox3"	 runat="server" Checked="false" />				
					</td>
				</tr>							
			</table>
</table>

				            <table width="50%" align="left">
				            	<tr>
		                        <td align="center">
		                        <table class="TN">
			                	<tr class="TableRowNormal">
			                		<td class="TCH" width="30%">
			                			<asp:Literal ID="Literal56" runat="server" Text="<%$ Resources:StringDef, DbHost %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox39" runat="server" />
			                		</td>
			                	</tr>
		                		<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal57" runat="server" Text="<%$ Resources:StringDef, DbName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox40" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal58" runat="server" Text="<%$ Resources:StringDef, DbLoginName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox41" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal59" runat="server" Text="<%$ Resources:StringDef, DbBackupPath %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox42" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal60" runat="server" Text="<%$ Resources:StringDef, DbLoginPwd %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox43" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal61" runat="server" Text="<%$ Resources:StringDef, PasswordConfirm %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox44" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			               </table>
			               </td>
			               </tr>
			               </table>
</asp:Panel>    			       
</td>
</tr>


<tr class="TableRowSpacing" />
<tr class="TableRowSpacing" />

<tr class="TableRowNormal">
<td>
<asp:Panel ID="Panel6" runat="server" Visible="true">
<table width="50%" align="left">
	<tr>
		<td align="center">
			<table class="TN">
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal62" runat="server" Text="<%$ Resources:StringDef, ID %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox45" runat="server" CssClass="TextBox" Enabled="false" />						
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal63" runat="server" Text="<%$ Resources:StringDef, Name %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox46" runat="server" CssClass="TextBox" />						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator21" runat="server" 							
							ControlToValidate="TextBoxServerName"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender21" TargetControlID="RequiredFieldValidator5" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal64" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox47" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator22" runat="server" 							
							ControlToValidate="TextBoxServerComment"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender22" TargetControlID="RequiredFieldValidator1" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal65" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox48" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator23" runat="server" 							
							ControlToValidate="TextBoxServerIpAddress"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender23" TargetControlID="RequiredFieldValidator3" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal66" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
					</td>
					<td class="TCN" >
						<asp:TextBox ID="TextBox49" runat="server" CssClass="TextBox"  onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>"  />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator24" runat="server" 							
							ControlToValidate="TextBoxGameDir"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender24" TargetControlID="RequiredFieldValidator4" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal67" runat="server" Text="与首个服务器数据库配置相同" />
					</td>
					<td class="TCN">
						<asp:CheckBox ID="CheckBox4"	 runat="server" Checked="false" />				
					</td>
				</tr>							
			</table>
</table>

				            <table width="50%" align="left">
				            	<tr>
		                        <td align="center">
		                        <table class="TN">
			                	<tr class="TableRowNormal">
			                		<td class="TCH" width="30%">
			                			<asp:Literal ID="Literal68" runat="server" Text="<%$ Resources:StringDef, DbHost %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox50" runat="server" />
			                		</td>
			                	</tr>
		                		<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal69" runat="server" Text="<%$ Resources:StringDef, DbName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox51" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal70" runat="server" Text="<%$ Resources:StringDef, DbLoginName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox52" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal71" runat="server" Text="<%$ Resources:StringDef, DbBackupPath %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox53" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal72" runat="server" Text="<%$ Resources:StringDef, DbLoginPwd %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox54" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal73" runat="server" Text="<%$ Resources:StringDef, PasswordConfirm %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox55" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			               </table>
			               </td>
			               </tr>
			               </table>
</asp:Panel>    			       
</td>
</tr>


<tr class="TableRowSpacing" />
<tr class="TableRowSpacing" />

<tr class="TableRowNormal">
<td>
<asp:Panel ID="Panel7" runat="server" Visible="true">
<table width="50%" align="left">
	<tr>
		<td align="center">
			<table class="TN">
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal74" runat="server" Text="<%$ Resources:StringDef, ID %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox56" runat="server" CssClass="TextBox" Enabled="false" />						
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal75" runat="server" Text="<%$ Resources:StringDef, Name %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox57" runat="server" CssClass="TextBox" />						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator25" runat="server" 							
							ControlToValidate="TextBoxServerName"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender25" TargetControlID="RequiredFieldValidator5" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal76" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox58" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator26" runat="server" 							
							ControlToValidate="TextBoxServerComment"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender26" TargetControlID="RequiredFieldValidator1" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal77" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox59" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator27" runat="server" 							
							ControlToValidate="TextBoxServerIpAddress"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender27" TargetControlID="RequiredFieldValidator3" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal78" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
					</td>
					<td class="TCN" >
						<asp:TextBox ID="TextBox60" runat="server" CssClass="TextBox"  onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>"  />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator28" runat="server" 							
							ControlToValidate="TextBoxGameDir"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender28" TargetControlID="RequiredFieldValidator4" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal79" runat="server" Text="与首个服务器数据库配置相同" />
					</td>
					<td class="TCN">
						<asp:CheckBox ID="CheckBox5"	 runat="server" Checked="false" />				
					</td>
				</tr>							
			</table>
</table>

				            <table width="50%" align="left">
				            	<tr>
		                        <td align="center">
		                        <table class="TN">
			                	<tr class="TableRowNormal">
			                		<td class="TCH" width="30%">
			                			<asp:Literal ID="Literal80" runat="server" Text="<%$ Resources:StringDef, DbHost %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox61" runat="server" />
			                		</td>
			                	</tr>
		                		<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal81" runat="server" Text="<%$ Resources:StringDef, DbName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox62" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal82" runat="server" Text="<%$ Resources:StringDef, DbLoginName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox63" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal83" runat="server" Text="<%$ Resources:StringDef, DbBackupPath %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox64" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal84" runat="server" Text="<%$ Resources:StringDef, DbLoginPwd %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox65" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal85" runat="server" Text="<%$ Resources:StringDef, PasswordConfirm %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox66" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			               </table>
			               </td>
			               </tr>
			               </table>
</asp:Panel>    			       
</td>
</tr>


<tr class="TableRowSpacing" />
<tr class="TableRowSpacing" />

<tr class="TableRowNormal">
<td>
<asp:Panel ID="Panel8" runat="server" Visible="true">
<table width="50%" align="left">
	<tr>
		<td align="center">
			<table class="TN">
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal86" runat="server" Text="<%$ Resources:StringDef, ID %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox67" runat="server" CssClass="TextBox" Enabled="false" />						
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal87" runat="server" Text="<%$ Resources:StringDef, Name %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox68" runat="server" CssClass="TextBox" />						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator29" runat="server" 							
							ControlToValidate="TextBoxServerName"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender29" TargetControlID="RequiredFieldValidator5" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal88" runat="server" Text="<%$ Resources:StringDef, Comment %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox69" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator30" runat="server" 							
							ControlToValidate="TextBoxServerComment"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender30" TargetControlID="RequiredFieldValidator1" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal89" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="TextBox70" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator31" runat="server" 							
							ControlToValidate="TextBoxServerIpAddress"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender31" TargetControlID="RequiredFieldValidator3" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal90" runat="server" Text="<%$ Resources:StringDef, GameDir %>" />
					</td>
					<td class="TCN" >
						<asp:TextBox ID="TextBox71" runat="server" CssClass="TextBox"  onmouseover="<%$ Resources:StringDef, ShowToolTipGameDir %>"  />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator32" runat="server" 							
							ControlToValidate="TextBoxGameDir"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender32" TargetControlID="RequiredFieldValidator4" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal91" runat="server" Text="与首个服务器数据库配置相同" />
					</td>
					<td class="TCN">
						<asp:CheckBox ID="CheckBox6"	 runat="server" Checked="false" />				
					</td>
				</tr>							
			</table>
</table>

				            <table width="50%" align="left">
				            	<tr>
		                        <td align="center">
		                        <table class="TN">
			                	<tr class="TableRowNormal">
			                		<td class="TCH" width="30%">
			                			<asp:Literal ID="Literal93" runat="server" Text="<%$ Resources:StringDef, DbHost %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox72" runat="server" />
			                		</td>
			                	</tr>
		                		<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal94" runat="server" Text="<%$ Resources:StringDef, DbName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox73" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal95" runat="server" Text="<%$ Resources:StringDef, DbLoginName %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox74" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal96" runat="server" Text="<%$ Resources:StringDef, DbBackupPath %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox75" runat="server" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal97" runat="server" Text="<%$ Resources:StringDef, DbLoginPwd %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox76" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			                	<tr class="TableRowNormal">
			                		<td class="TCH">
			                			<asp:Literal ID="Literal98" runat="server" Text="<%$ Resources:StringDef, PasswordConfirm %>" />
			                		</td>
			                		<td class="TCN">
			                			<asp:TextBox ID="TextBox77" runat="server" TextMode="Password" EnableViewState="true" />
			                		</td>
			                	</tr>
			               </table>
			               </td>
			               </tr>
			               </table>
</asp:Panel>    			       
</td>
</tr>

<tr class="TableRowSpacing" />
<tr class="TableRowSpacing" />
<tr class="TableRowSpacing" />

	<tr>
		<td align="center">
				<asp:LinkButton ID="LinkButtonEdit" runat="server"  Text="<%$ Resources:StringDef, Save %>"/>&nbsp;
			<asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="AddAndDelete.aspx" Text="<%$ Resources:StringDef, Return %>" />
		</td>
	</tr>

</table>

</table>

            			      
</ContentTemplate>
</asp:UpdatePanel>
</asp:Content>

