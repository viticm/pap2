<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameMaster_OperationQuery, App_Web_6lmf-asn" title="<%$ Resources:StringDef,GMOperationQuery %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Literal runat="server" Text="<%$ Resources:StringDef,GMOperationQuery %>" />
		</td>
	</tr>
	<tr>
		<td align="center">
		    <asp:Table runat="server">
		        <asp:TableHeaderRow>
		            <asp:TableHeaderCell>
		                <asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, Operator %>" />
		            </asp:TableHeaderCell>
		            <asp:TableCell>
		                <asp:RadioButtonList ID="RadioButtonListOperator" runat="server" RepeatDirection="Horizontal">
						    <asp:ListItem Text="<%$ Resources:StringDef,ID %>" Selected="true" />
						    <asp:ListItem Text="<%$ Resources:StringDef,Name %>" />
						</asp:RadioButtonList>
						<table width="100%">
						    <tr>
						        <td width="60%" align="right">
						            <asp:TextBox ID="TextBoxOperator" runat="server" />        
						        </td>
						        <td></td>
						    </tr>
						</table>						
		            </asp:TableCell>
		        </asp:TableHeaderRow>
		        <asp:TableHeaderRow>
		            <asp:TableHeaderCell>
		                <asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef, Target %>" />
		            </asp:TableHeaderCell>
		            <asp:TableCell>
                        <asp:RadioButtonList ID="RadioButtonListTarget" runat="server" RepeatDirection="Horizontal" AutoPostBack="true" OnSelectedIndexChanged="RadioButtonListTarget_SelectedIndexChanged">
                            <asp:ListItem Text="<%$ Resources:StringDef,All %>" Value="-1" Selected="true" />
						    <asp:ListItem Text="<%$ Resources:StringDef,Account %>" Value="0" />
						    <asp:ListItem Text="<%$ Resources:StringDef,Role %>" Value="1"/>
						</asp:RadioButtonList>
						<table width="100%">
						    <tr>
						        <td align="right" width="60%">  
						            <asp:TextBox ID="TextBoxTarget" runat="server" />            
						        </td>
						        <td align="left">
						            <asp:CheckBox ID="CheckBoxExactlyMatch" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" />            
						        </td>
						    </tr>
						</table>
		            </asp:TableCell>
		        </asp:TableHeaderRow>
	            <asp:TableHeaderRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal6" runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" AutoPostBack="true" OnSelectedServerGroupChanged="SelectedServerGroupChanged" Enabled="false" />
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" Enabled="false" />
					</asp:TableCell>
				</asp:TableHeaderRow>
                <asp:TableHeaderRow>
		            <asp:TableHeaderCell>
		                <asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, Operation %>" />
		            </asp:TableHeaderCell>
		            <asp:TableCell>
		                <asp:Panel runat="server" ID="PanelCheckBoxListOperation" Width="80%" HorizontalAlign="left" >
		                    <asp:CheckBoxList ID="CheckBoxListOperation" runat="server" RepeatLayout="Table" RepeatDirection="Horizontal" RepeatColumns="5" align="center" />
		                </asp:Panel>
		            </asp:TableCell>
		        </asp:TableHeaderRow>
		        <asp:TableHeaderRow>
		            <asp:TableHeaderCell>
		                <asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef, StartTime %>" />
		            </asp:TableHeaderCell>
		            <asp:TableCell>
		                <Blaze:ScriptCalendar ID="StartDate" runat="server" ShowCheckBox="true" />
		            </asp:TableCell>
		        </asp:TableHeaderRow>
                <asp:TableHeaderRow>
		            <asp:TableHeaderCell>
		                <asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, EndTime %>" />
		            </asp:TableHeaderCell>
		            <asp:TableCell>
		                <Blaze:ScriptCalendar ID="EndDate" runat="server" ShowCheckBox="true" />
		            </asp:TableCell>
		        </asp:TableHeaderRow>
		    </asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
		</td>
	</tr>
	<tr>
		<td align="center">
            <asp:LinkButton ID="ButtonQuery" Text="<%$ Resources:StringDef, Query %>" runat="server" OnClick="ButtonQuery_Click" />
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

<table width="100%">
	<tr>
		<td align="center">
			<asp:UpdateProgress runat="server" AssociatedUpdatePanelID="UpdatePanel1">
				<ProgressTemplate>
					<asp:Label runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />
				</ProgressTemplate>
			</asp:UpdateProgress>
		</td>
	</tr>
</table>

<asp:UpdatePanel runat="server" ID="UpdatePanelResult">
<contenttemplate>
<asp:Panel runat="server" ID="PanelResult" >
<table width="100%">
    <tr>
		<td align="right">
			<table cellpadding="0">
				<tr>
					<td>
						<asp:Label runat="server" Text="<%$ Resources:StringDef, RecordPerPage %>" />
						<asp:ListBox ID="ListBoxRecordPerPage" runat="server" Rows="1">
							<asp:ListItem Text="20" Value="20" />
							<asp:ListItem Text="50" Value="50" />
							<asp:ListItem Text="100" Value="100" />
						</asp:ListBox>
						<asp:Button ID="ButtonFirstPage" runat="server" Text="<%$ Resources:StringDef, FirstPage %>" CommandName="FirstPage" OnClick="ButtonPage_Click" Enabled="false" />
						<asp:Button ID="ButtonPreviousPage" runat="server" Text="<%$ Resources:StringDef, PreviousPage %>" CommandName="PreviousPage" OnClick="ButtonPage_Click" Enabled="false" />
						<asp:Button ID="ButtonNextPage" runat="server" Text="<%$ Resources:StringDef, NextPage %>" CommandName="NextPage" OnClick="ButtonPage_Click" Enabled="false" />
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr>
		<td>
		    <asp:DataGrid ID="DataGridResult" runat="server" AutoGenerateColumns="false" CssClass="TN" HeaderStyle-CssClass="TCH" CellSpacing="1">
		        <Columns>
		            <asp:BoundColumn DataField="User" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Operator %>" />
		            <asp:BoundColumn DataField="Operation" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Operation %>" />
		            <asp:BoundColumn DataField="Target" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Target %>" />
		            <asp:BoundColumn DataField="ServerName" ReadOnly="true" HeaderText="<%$ Resources:StringDef,GameServer %>" />
		            <asp:BoundColumn DataField="Description" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Description %>" />
		            <asp:BoundColumn DataField="LogTime" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Time %>" />
		        </Columns>		        
		    </asp:DataGrid>
		</td>
	</tr>
</table>
</asp:Panel>
</contenttemplate>
</asp:UpdatePanel>	

</asp:Content>