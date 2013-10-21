<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="ServerOperationHistory.aspx.cs" Inherits="GameServer_ServerOperationHistory" Title="Untitled Page" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="Literal1" runat="server" Text="服务器（组）操作查询" />
		</td>
	</tr>
	<tr>
		<td align="center">
		    <asp:Table ID="Table1" runat="server">

		        <asp:TableRow>
		            <asp:TableHeaderCell> 
		                <asp:Literal ID="Literal3" runat="server" Text="按照什么查询" />
		            </asp:TableHeaderCell>
		            <asp:TableCell>
		               	<asp:RadioButtonList ID="QueryByWhatRadioButtonList" AutoPostBack="true" runat="server" RepeatDirection="Horizontal" OnSelectedIndexChanged="QueryByWhatRadioButtonList_OnSelectedIndexChanged">
						    <asp:ListItem Text="按操作对象" Value="target" Selected="true" />
						    <asp:ListItem Text="按操作流水号" Value="operationId" />
						    <asp:ListItem Text="按操作时间" Value="time" />
						</asp:RadioButtonList>
		            </asp:TableCell>
		        </asp:TableRow>
		        
	            <asp:TableHeaderRow ID="OperationTargetTableHeaderRow">
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal6" runat="server" Text="操作对象" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" AutoPostBack="true" OnSelectedServerGroupChanged="SelectedServerGroupChanged" Enabled="true" />
						<asp:DropDownList runat="server" ID="ServerDropDownList" Enabled="true" />
					</asp:TableCell>
				</asp:TableHeaderRow>

				<asp:TableHeaderRow ID="OperationIdTableHeaderRow" Visible="false">
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal8" runat="server" Text="操作流水号" />
					</asp:TableHeaderCell>
					<asp:TableCell>
                        <asp:TextBox ID="OperationIdTextBox" runat="server" />
                        <asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" 							
							ControlToValidate="OperationIdTextBox"
							Display="None"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender1" TargetControlID="RequiredFieldValidator1" />
		                <asp:RegularExpressionValidator ID="DigitOnlyRegularExpressionValidator1" runat="server"
					         ControlToValidate="OperationIdTextBox"
							 Display="None"
							 SetFocusOnError="true"
							 ErrorMessage="只能填写数字" ValidationExpression="[0-9]+" />
        	    	    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender5" TargetControlID="DigitOnlyRegularExpressionValidator1" />		            										
					</asp:TableCell>
				</asp:TableHeaderRow>
				
                <asp:TableHeaderRow ID="OperationTypeTableHeaderRow">
		            <asp:TableHeaderCell>
		                <asp:Literal ID="Literal4" runat="server" Text="操作类型" />
		            </asp:TableHeaderCell>
		            <asp:TableCell>
		                <asp:Panel runat="server" ID="PanelCheckBoxListOperation" Width="80%" HorizontalAlign="Center" >
		                    <asp:CheckBoxList ID="CheckBoxListOperation" runat="server" RepeatLayout="Table" RepeatDirection="Horizontal" RepeatColumns="3"  />
		                </asp:Panel>
		            </asp:TableCell>
		        </asp:TableHeaderRow>
		        
		        <asp:TableHeaderRow ID="OperationResultTableHeaderRow">
		            <asp:TableHeaderCell>
		                <asp:Literal ID="Literal9" runat="server" Text="操作结果" />
		            </asp:TableHeaderCell>
		            <asp:TableCell>
		                <asp:Panel runat="server" ID="Panel1" Width="80%" HorizontalAlign="Center" >
		                    <asp:CheckBoxList ID="resultSucessCheckBoxList" runat="server" RepeatLayout="Table" RepeatDirection="Horizontal" RepeatColumns="2">
		                        <asp:ListItem Text="成功" Value="1" Selected="true" />
		                        <asp:ListItem Text="失败" Value="0" Selected="true" />
		                    </asp:CheckBoxList>
		                </asp:Panel>
		            </asp:TableCell>
		        </asp:TableHeaderRow>
		        
		        <asp:TableHeaderRow ID="UserTableHeaderRow">
		            <asp:TableHeaderCell>
		                <asp:Literal ID="Literal2" runat="server" Text="平台用户" />
		            </asp:TableHeaderCell>
		            <asp:TableCell>
		                <asp:RadioButtonList ID="UserRadioButtonList" runat="server" AutoPostBack="true" RepeatDirection="Horizontal" OnSelectedIndexChanged="UserRadioButtonList_OnSelectedIndexChanged">
						    <asp:ListItem Text="<%$ Resources:StringDef,Name %>" Value="name" Selected="true" />
						    <asp:ListItem Text="<%$ Resources:StringDef,ID %>" Value="id" />
						</asp:RadioButtonList>
						<table width="100%">
						    <tr>
						        <td width="60%" align="right">
						            <asp:TextBox ID="UserNameTextBox" Visible="true" runat="server" />        
						        </td>
						        <td></td>
						    </tr>
						    
						    <tr>
						        <td width="60%" align="right">
						            <asp:TextBox ID="UserIdTextBox" Visible="false" runat="server" />
						            <asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" 							
							            ControlToValidate="UserIdTextBox"
							            Display="None"
							            SetFocusOnError="true"
							            ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						            <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender2" TargetControlID="RequiredFieldValidator2" />
		                            <asp:RegularExpressionValidator ID="DigitOnlyRegularExpressionValidator2" runat="server"
					                     ControlToValidate="UserIdTextBox"
							             Display="None"
							             SetFocusOnError="true"
							             ErrorMessage="只能填写数字" ValidationExpression="[0-9]+" />
        	    	                <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender3" TargetControlID="DigitOnlyRegularExpressionValidator2" />		            										
						        </td>
						        <td></td>
						    </tr>
						</table>						
		            </asp:TableCell>
		        </asp:TableHeaderRow>
		        
		        
		        <asp:TableHeaderRow ID="StartTimeTableHeaderRow">
		            <asp:TableHeaderCell>
		                <asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, StartTime %>" />
		            </asp:TableHeaderCell>
		            <asp:TableCell>
		                <Blaze:ScriptCalendar ID="StartDate" runat="server" ShowCheckBox="true" />
		            </asp:TableCell>
		        </asp:TableHeaderRow>
		        
                <asp:TableHeaderRow ID="EndTimeTableHeaderRow">
		            <asp:TableHeaderCell>
		                <asp:Literal ID="Literal7" runat="server" Text="<%$ Resources:StringDef, EndTime %>" />
		            </asp:TableHeaderCell>
		            <asp:TableCell>
		                <Blaze:ScriptCalendar ID="EndDate" runat="server" ShowCheckBox="true" />
		            </asp:TableCell>
		        </asp:TableHeaderRow>
		       
		        
		    </asp:Table>
		</td>
	</tr>
</table>

<br />

<table width="100%">
<tr>
	<td align="center">
        <asp:LinkButton ID="ButtonQuery" Text="<%$ Resources:StringDef, Query %>" runat="server" OnClick="ButtonQuery_Click" />
	</td>
</tr>
<tr class="TableRowSpacing" />
<tr>
	<td align="center">
		<asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
	</td>
</tr>
</table>

<br />

<table width="100%">
	<tr>
		<td align="center">
			<asp:UpdateProgress ID="UpdateProgress1" runat="server">
				<ProgressTemplate>
					<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />
				</ProgressTemplate>
			</asp:UpdateProgress>
		</td>
	</tr>
</table>

<br />

<table width="100%">    
	<tr>
		<td>
		    <asp:DataGrid ID="ResultDataGrid" runat="server" AutoGenerateColumns="false" CssClass="TN" HeaderStyle-CssClass="TCH" CellSpacing="1">
		        <Columns>
		            <asp:BoundColumn DataField="operation_id" ReadOnly="true" HeaderText="操作流水号" />
		            <asp:BoundColumn DataField="user_name" ReadOnly="true" HeaderText="平台用户名" />
		            <asp:BoundColumn DataField="target_type" ReadOnly="true" HeaderText="操作对象类型" />
		            <asp:BoundColumn DataField="target_name" ReadOnly="true" HeaderText="操作对象名称" />
		            <asp:BoundColumn DataField="operation_type" ReadOnly="true" HeaderText="操作类型" />
		            <asp:BoundColumn DataField="operation_result" ReadOnly="true" HeaderText="操作结果" />
		            <asp:BoundColumn DataField="description" ReadOnly="true" HeaderText="操作描述" />
		            <asp:BoundColumn DataField="log_time" ReadOnly="true" HeaderText="日志记录时间" />
		        </Columns>		        
		    </asp:DataGrid>
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>


</asp:Content>

