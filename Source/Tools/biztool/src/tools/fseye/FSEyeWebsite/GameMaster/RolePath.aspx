<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="RolePath.aspx.cs" Inherits="GameMaster_RolePath" Title="角色历程查询" %>
<%@ Register TagPrefix="Blaze" TagName="DropDownListGameCenter" Src="~/Common/DropDownListGameCenter.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>


<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:Panel ID="Panel1" runat="server" DefaultButton="submitButton">

<asp:UpdatePanel runat="server" ID="inputUpdatePanel" UpdateMode="Conditional" Visible="true">
<contenttemplate>

<table width="60%" align="center">
	<tr>
	    <td align="center">
	        <asp:Label ID="Literal11" runat="server" SkinID="Title" Text="<%$ Resources:StringDef, RolePathQuery %>" />
	    </td>
	</tr>
	<tr class="TableRowSpacing" />	
	<tr>
		<td align="center">
			<table border="0" class="TN" cellpadding="0" cellspacing="1">
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal10" runat="server" Text="<%$ Resources:StringDef, ServerGroup %>" />
					</td>
					<td class="TCN">
					    <Blaze:DropDownListGameCenter ID="serverGroupDropDownList" runat="server" />
					</td>
				</tr>
				<tr class="TableRowNormal">
					<td class="TCH">
						<asp:Literal ID="Literal9" runat="server" Text="<%$ Resources:StringDef, RoleGUID1 %>" />
					</td>
					<td class="TCN">
						<asp:TextBox ID="roleGUIDTextBox" runat="server" CssClass="TextBox" />						
						<asp:RequiredFieldValidator ID="RequiredFieldValidator5" runat="server" 							
							ControlToValidate="roleGUIDTextBox"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender4" TargetControlID="RequiredFieldValidator5" />															                        				    				        					        
				    </td>
				</tr>
				
				<tr>
				    <td class="TCH">
				        <asp:Literal runat="server" Text="事件类型" />
				    </td>
				    <td  class="TCN">
				        <asp:CheckBoxList runat="server" ID="eventTypeCheckBoxList"  RepeatDirection="Horizontal" RepeatLayout="Table" RepeatColumns="5" >
				            <asp:ListItem Text="升级" Value="log_3_1" Selected="true" />
				            <asp:ListItem Text="门派路线" Value="log_3_2,log_3_3,log_3_8" Selected="true" />
				            <asp:ListItem Text="家族" Value="log_3_4,log_3_5" Selected="true" />
				            <asp:ListItem Text="完成任务" Value="log_3_6" Selected="true" />
				            <asp:ListItem Text="玩家上下线" Value="log_3_7" Selected="true" />
				        </asp:CheckBoxList>
				    </td>
				</tr>
				
				<tr class="TableRowNormal">
				    <td class="TCH">
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, Time %>" />
					</td>
					<td  class="TCN">
					    <table>
					        <tr>
					            <td>
	                               <Blaze:ScriptCalendar ID="RecentLoginTime_start" runat="server" ShowCheckbox="false" TextBoxReadOnly="false" />
	                            </td>
	                             <td> &nbsp;-----&nbsp; </td>
	                            <td>
	                                <Blaze:ScriptCalendar ID="RecentLoginTime_end" runat="server" ShowCheckbox="false" TextBoxReadOnly="false" />       
	                            </td>
	                        </tr>
	                    </table>
	                </td>
				</tr>			
			</table>
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

<br />

<asp:UpdatePanel runat="server" ID="resultUpdatePanel" UpdateMode="Conditional" Visible="true">
<contenttemplate>

<table align="center">
    <tr>
        <td><asp:LinkButton ID="submitButton" runat="server" Text="<%$ Resources:StringDef, Query %>" OnClick="submitButton_Click" /></td>
    </tr>
</table>

<br />

<table align="center">
    <tr>
        <td><asp:Label ID="LabelSuccess" runat="server" SkinID="MessageSuccess" /></td>
    </tr>    
    <tr>
        <td><asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" /></td>
    </tr>
</table>

<br />

<table align="center">
    <tr>
        <td>
            <asp:UpdateProgress runat="server" id="updateProgress1">
                <ProgressTemplate>
                        <asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />                                                
                </ProgressTemplate>
            </asp:UpdateProgress>
        </td>
    </tr>
</table>

<br />

<table width="96%" align="center">  
    
	<tr>
		<td>
			<asp:Panel ID="resultPagingInfoPanel" runat="server" Visible="false">
			    <table align="center" cellpadding="0">
				    <tr>
					    <td>
						    <asp:Label ID="resultInfoLabel" runat="server" />
						    <asp:Button ID="ButtonFirstPage" runat="server" Text="<%$ Resources:StringDef, FirstPage %>" Enabled="false" OnClick="ButtonFirstPage_Click" />
						    <asp:Button ID="ButtonPreviousPage" runat="server" Text="<%$ Resources:StringDef, PreviousPage %>" Enabled="false" OnClick="ButtonPreviousPage_Click" />
						    <asp:Button ID="ButtonNextPage" runat="server" Text="<%$ Resources:StringDef, NextPage %>" Enabled="false" OnClick="ButtonNextPage_Click" />					
					    </td>
				    </tr>
			    </table>
			</asp:Panel>
		</td>
	</tr>	
	
    <tr>
        <td>
            <asp:DataGrid ID="resultDataGrid" runat="server" AutoGenerateColumns="false" CssClass="TN" HeaderStyle-CssClass="TCH" CellSpacing="1">  
                <Columns>

                    <asp:BoundColumn HeaderText="<%$ Resources:StringDef, Time %>" ReadOnly="true" DataField="time" />
                    <asp:BoundColumn HeaderText="<%$ Resources:StringDef, Event %>" ReadOnly="true" DataField="event" />
                    <asp:BoundColumn HeaderText="<%$ Resources:StringDef, Comment %>" ReadOnly="true" DataField="remark" />
                                
                                
                </Columns>
            </asp:DataGrid>
        </td>
    </tr>
</table>    
  
</contenttemplate>
</asp:UpdatePanel>    

</asp:Panel>
</asp:Content>

