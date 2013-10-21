<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="UpDateManagement_EditUpdateServer, App_Web_hm9xpbcd" title="EditUpdateServer" theme="BlazeSky" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">
<asp:UpdatePanel ID="UpdatePanel2" runat="server" UpdateMode="Conditional">
<ContentTemplate>
  <table width="100%">
    	<tr>
		<td align="center" >
			<table border="1" class="TableMedium">
				<caption class='TableCaption'><asp:Literal ID="LabelTitle" runat="server" Text="<%$ Resources:StringDef, UpdateServer %>" /></caption>
                <tr class="TableRowLeft">
					<td>
						<asp:Literal ID="Literal8" runat="server" Text="<%$ Resources:StringDef, Name %>" />
					</td>
					
					<td align="left">
						<asp:TextBox ID="TextBoxServerComment" runat="server" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" 							
							ControlToValidate="TextBoxServerComment"
							Display="Dynamic"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, FieldRequired %>">
						</asp:RequiredFieldValidator>
					</td>					
				</tr>	
                <tr class="TableRowLeft">
					<td>
						<asp:Literal ID="Literal7" runat="server" Text="<%$ Resources:StringDef, Ip %>" />
					</td>                    
					<td align="left">					   
					        <asp:TextBox ID="TextBoxServerIpAddress" runat="server" CssClass="TextBox" />
						    <asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server" 							
							    ControlToValidate="TextBoxServerIpAddress"
							    Display="Dynamic"
							    SetFocusOnError="true"
							    ErrorMessage="<%$ Resources:StringDef, FieldRequired %>">
						    </asp:RequiredFieldValidator>
						    					
					</td>					
				</tr>
				<tr class="TableRowLeft">
					<td>
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, Port %>" />
					</td>
					
					<td align="left">
						<asp:TextBox ID="TextBoxServerPort" runat="server" CssClass="TextBox" >21</asp:TextBox>
						<asp:RequiredFieldValidator ID="RequiredFieldValidator4" runat="server" 							
							ControlToValidate="TextBoxServerPort"
							Display="Dynamic"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, FieldRequired %>">
						</asp:RequiredFieldValidator>
					</td>					
				</tr>
				<tr class="TableRowLeft">
					<td>
						<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, UserName %>" />
					</td>
					
					<td align="left">                      
					     <asp:UpdatePanel ID="UpdatePanel1" runat="server" UpdateMode="Conditional">
					            <ContentTemplate>
						        <asp:TextBox ID="TextBoxServerUserName" runat="server" CssClass="TextBox" />						                                            
                                <asp:CheckBox ID="CheckBoxAnonymous" runat="server" Text="<%$ Resources:StringDef, Anonymous %>" OnCheckedChanged="CheckBoxAnonymous_CheckedChanged"  AutoPostBack="true"/>
                            </ContentTemplate>    
                            <Triggers>
                                <asp:AsyncPostBackTrigger ControlID="CheckBoxAnonymous" />
                            </Triggers>                        
                            </asp:UpdatePanel>	
                    </td>
				</tr>	
				<tr class="TableRowLeft">
					<td>
						<asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef, NewPassword %>" />
					</td>					
					<td align="left">
						<asp:TextBox ID="TextBoxServerPassword" runat="server" CssClass="TextBox" TextMode="Password" MaxLength="16" Enabled="false" />
						<asp:CheckBox ID="CheckBoxChangePassword" runat="server" Text="ÐÞ¸ÄÃÜÂë" OnCheckedChanged="CheckBoxChangePassword_CheckedChanged"  AutoPostBack="true"/>
						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" 							
							ControlToValidate="TextBoxServerPassword"
							Display="None"
							SetFocusOnError="true"
						    Enabled="false"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender1" TargetControlID="RequiredFieldValidator2" />						
					</td>
				</tr>
				<tr class="TableRowLeft">
					<td>
						<asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef, PasswordConfirm %>" />
					</td>
					<td align="left">
						<asp:TextBox ID="TextBoxPasswordConfirm" runat="server" TextMode="Password" MaxLength="16" CssClass="TextBox" Enabled="false" />
						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator5" runat="server"
							ControlToValidate="TextBoxPasswordConfirm"
							Display="None"
							SetFocusOnError="true"
							Enabled="false"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender2" TargetControlID="RequiredFieldValidator5" />
                        
						<asp:CompareValidator ID="CompareValidator1" runat="server" 
							ControlToValidate="TextBoxPasswordConfirm"
							ControlToCompare="TextBoxServerPassword"
							Display="None"
							Type="String"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RetypePasswordValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCallout1" TargetControlID="CompareValidator1" />
					</td>
				</tr>				
			</table>
		</td>
	</tr>	
	<tr class="TableRowNormal" />
	<tr>
	    <td align="center">
		    <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
	    </td>
    </tr>		
	<tr>
	    <td align="center">	       
                    <asp:LinkButton ID="SaveUpdateServerLinkButton" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef,Save %>" OnClick="Save_Click" />
	                <asp:HyperLink runat="server" NavigateUrl="UpdateServerConfig.aspx" Text="<%$ Resources:StringDef, Return %>" />	           
	    </td>
	</tr>
</table>
</ContentTemplate>
</asp:UpdatePanel>
</asp:Content>