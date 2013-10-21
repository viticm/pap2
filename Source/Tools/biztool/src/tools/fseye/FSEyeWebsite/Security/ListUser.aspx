<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="ListUser.aspx.cs" Inherits="Security_ListUser" Title="<%$ Resources:StringDef, User %>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<ContentTemplate>
<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="Literal10" runat="server" Text="<%$ Resources:StringDef, User %>" />		
		</td>
	</tr>	
	<tr>
		<td>
		    <asp:DataGrid ID="DataGridUser" runat="server" CssClass="TN" HeaderStyle-CssClass="TCH" AutoGenerateColumns="false" 
		    OnItemDataBound="DataGridUser_ItemDataBound" CellSpacing="1">
		        <Columns>
		            <%--
		            <asp:ButtonColumn CommandName="Delete" ButtonType="LinkButton" Text="<%$ Resources:StringDef,Delete %>" />
		            <asp:EditCommandColumn ButtonType="LinkButton" EditText="<%$ Resources:StringDef,Modify %>" CancelText="<%$ Resources:StringDef,Cancel %>" UpdateText="<%$ Resources:StringDef,Update %>" />
		            <asp:TemplateColumn>
		                <ItemTemplate>
		                    <a href='../Security/EditPrivilege.aspx?objectId=<%# Eval("ObjectId")%>' ><asp:Literal runat="server" Text="<%$ Resources:Stringdef,Privilege %>" /></a>
		                </ItemTemplate>
		            </asp:TemplateColumn>
                    <asp:TemplateColumn>
		                <ItemTemplate>
		                    <a href='../Security/UserPassword.aspx?userName=<%# Eval("UserName")%>' ><asp:Literal runat="server" Text="<%$ Resources:Stringdef,ModifyPassword %>" /></a>
		                </ItemTemplate>
		            </asp:TemplateColumn>
		            <asp:BoundColumn DataField="UserName" HeaderText="<%$ Resources:StringDef, UserName %>" ReadOnly="true" />
		            <asp:BoundColumn DataField="RealName" HeaderText="<%$ Resources:StringDef, RealName %>" />
		            <asp:BoundColumn DataField="Comment" HeaderText="<%$ Resources:StringDef, Comment %>" />
		            <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,Group %>" >
		                <ItemTemplate>		                
		                    <asp:Label ID="LabelUserGroup" runat="server" Text='<%# Eval("Group")%>' />
		                </ItemTemplate>
		                <EditItemTemplate>
		                    <asp:CheckBoxList ID="CheckBoxListGroup" runat="server"  />
		                </EditItemTemplate>
		            </asp:TemplateColumn>
		            --%>		            
                    <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,Operation %>">
		                <ItemTemplate>
		                    <asp:LinkButton ID="LinkButtonDelete" runat="server" SkinID="PlainText" CommandName="Delete" Text="<%$ Resources:StringDef,Delete %>" OnClick="LinkButton_Click"/>
		                    <a href='../Security/EditPrivilege.aspx?objectId=<%# Eval("ObjectId")%>' ><asp:Literal runat="server" Text="<%$ Resources:Stringdef,Privilege %>" /></a>
                            <a href='../Security/EditUser.aspx?userId=<%# Eval("ObjectId")%>' ><asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:Stringdef,Modify %>" /></a>
		                </ItemTemplate>
		            </asp:TemplateColumn>
		            <asp:BoundColumn DataField="UserName" HeaderText="<%$ Resources:StringDef, UserName %>" ReadOnly="true" />
		            <asp:BoundColumn DataField="RealName" HeaderText="<%$ Resources:StringDef, RealName %>" />
		            <asp:BoundColumn DataField="Comment" HeaderText="<%$ Resources:StringDef, Comment %>" />
		            <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,Group %>" >
		                <ItemTemplate>		                
		                    <asp:Label ID="LabelUserGroup" runat="server" Text='<%# Eval("Group")%>' />
		                </ItemTemplate>
		            </asp:TemplateColumn>
		        </Columns>
		    </asp:DataGrid>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
		    <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
            <asp:UpdateProgress ID="UpdateProgress2" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
				<ProgressTemplate>	
					<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileOperating %>" />
				</ProgressTemplate>
			</asp:UpdateProgress>
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">						
						<asp:HyperLink runat="server" NavigateUrl="AddUser.aspx" Text="<%$ Resources:StringDef, AddUser %>" />
						<asp:HyperLink runat="server" NavigateUrl="Security.aspx" Text="<%$ Resources:StringDef, Return %>" />						
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
</ContentTemplate>
</asp:UpdatePanel>
</asp:Content>