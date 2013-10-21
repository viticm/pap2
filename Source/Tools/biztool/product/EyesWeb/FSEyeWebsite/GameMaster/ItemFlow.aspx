<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameInfo_ItemFlow, App_Web_6lmf-asn" title="<%$ Resources:Web.Sitemap, ItemTrack %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="DropDownListGameCenter" Src="~/Common/DropDownListGameCenter.ascx" %>


<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:Panel ID="Panel1" runat="server" DefaultButton="LinkButtonQuery">

<asp:UpdatePanel id="UpdatePanel1" runat="server" UpdateMode="Conditional">
<contenttemplate>

<table width="60%"  align="center">
	<tr>
		<td align="center" colspan="4">
			<asp:Label ID="literal3" runat="server" Text="<%$ Resources:StringDef, TrackItemFlow %>" SkinID="Title" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />	
    <tr>
        <td align="center">
			<asp:Table ID="Table1" runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell Width="25%">
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, ZoneName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:DropDownListGameCenter ID="serverGroupDropDownList" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
            	<asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal runat="server" Text="<%$ Resources:StringDef,ItemGUID %>" />
				    </asp:TableHeaderCell>
				    <asp:TableCell>
				        <table border="0" cellspacing="0" width="100%">
				            <tr>
				                <td>
				                    <asp:TextBox ID="TextBoxInputItemGuid" runat="server" />
				                    <asp:RequiredFieldValidator ID="RequiredFieldValidator5" runat="server" 							
							            ControlToValidate="TextBoxInputItemGuid"
							            Display="None"
							            SetFocusOnError="true"
							            ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
                                    <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender4" TargetControlID="RequiredFieldValidator5" />															                        				    				        					        
           
				                </td>
				                <td></td>
				            </tr>
				        </table>				        
				    </asp:TableCell>
				</asp:TableRow>
				
				<asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal runat="server" Text="事件类型" />
				    </asp:TableHeaderCell>
				    <asp:TableCell>
				        <asp:CheckBoxList runat="server" ID="eventTypeCheckBoxList"  RepeatDirection="Horizontal" RepeatLayout="Table" RepeatColumns="6" >
				            <asp:ListItem Text="生成" Value="log_4_1" Selected="true" />
				            <asp:ListItem Text="销毁" Value="log_4_2" Selected="true" />
				            <asp:ListItem Text="丢弃" Value="log_4_3" Selected="true" />
				            <asp:ListItem Text="获得" Value="log_4_4" Selected="true" />
				            <asp:ListItem Text="所有者变更" Value="log_4_5" Selected="true" />
				            <asp:ListItem Text="属性改变" Value="log_4_6" Selected="true" />
				        </asp:CheckBoxList>
				    </asp:TableCell>
				</asp:TableRow>
				
				
				 <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, Time %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>				
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
					</asp:TableCell >
				</asp:TableRow>
            </asp:Table> 
        </td>
    </tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

<br />

<asp:UpdatePanel runat="server" ID="UpdatePanelResult" UpdateMode="Conditional">
<contenttemplate>

<table align="center">
    <tr>
	    <td align="center">
	        <asp:LinkButton ID="LinkButtonQuery" runat="server"  Text="<%$ Resources:StringDef,Query %>" OnClick="LinkButtonQuery_Click" />
	    </td>
	</tr>    
</table>

<br />

<table align="center">
    <tr>
        <td>
            <asp:Label ID="LabelSuccess" runat="server" SkinID="MessageSuccess" />
        </td>
    </tr>
    <tr>
        <td align="center">
            <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
        </td>
    </tr>
</table>

<br />

<table align="center">
    <tr>
		<td align="center">
            <asp:UpdateProgress ID="UpdateProgress1" runat="server">
               <ProgressTemplate>
                   <asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef,PleaseWaitWhileQuerying%>" />
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

                    <asp:BoundColumn HeaderText="时间" ReadOnly="true" DataField="time" />
                    <asp:BoundColumn HeaderText="事件" ReadOnly="true" DataField="event" />
                    <asp:BoundColumn HeaderText="物品名" ReadOnly="true" DataField="itemName" />
                                        
		            <asp:TemplateColumn>
		                <HeaderTemplate>
		                    <asp:Literal ID="Literal4" runat="server" Text="所有者ID" />
		                </HeaderTemplate>
		                <ItemTemplate>
		                    <asp:HyperLink ID="roleGUIDHyperLink" runat="server"  Target="_blank"  Text='<%# Eval("ownerID") %>'
		                        NavigateUrl='<%# "RolePath.aspx?roleGUID=" + Eval("ownerID")+"&serverID="+Eval("gameCenterID") %>'
		                        ToolTip='<%$ Resources:StringDef, RolePathQuery %>' SkinID="PlainText" />
		                </ItemTemplate>
		            </asp:TemplateColumn>                                        
                    
                    <asp:BoundColumn HeaderText="所有者角色名" ReadOnly="true" DataField="ownerRoleName" />
                    <asp:BoundColumn HeaderText="物品详细说明" ReadOnly="true" DataField="itemDetails" />
                                                        
                </Columns>
            </asp:DataGrid>
		</td>
	</tr>
</table>


</contenttemplate>
</asp:UpdatePanel>

</asp:Panel>
</asp:Content>

