<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameMaster_QueryRole, App_Web_6lmf-asn" title="<%$ Resources:StringDef, QueryRole %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="DropDownListGameCenter" Src="~/Common/DropDownListGameCenter.ascx" %>


<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>
<table width="100%">
	<tr class="TableRowSpacing" />
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="literal3" runat="server" Text="<%$ Resources:StringDef, QueryRole %>" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:Table runat="server" Width="60%">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, ZoneName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:DropDownListGameCenter ID="serverGroupDropDownList" runat="server" />					    					    
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, szRoleName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <table border="0" cellspacing="0" width="100%">
					        <tr>
					            <td align="right" width="60%">
					                <asp:TextBox ID="TextBoxcRoleName" runat="server" />
					            </td>
					            <td align="left">
					                <asp:CheckBox ID="CheckBoxcRoleName" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" />        
					            </td>
					        </tr>
					    </table>
					</asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, szAccountName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <table border="0" cellspacing="0" width="100%">
					        <tr>
					            <td align="right" width="60%">
					                <asp:TextBox ID="TextBoxcAccountName" runat="server" CssClass="TextBox" />            
					            </td>
					            <td align="left">
					                <asp:CheckBox ID="CheckBoxcAccountName" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" />            
					            </td>
					        </tr>
					    </table>
					</asp:TableCell>
				</asp:TableRow>				
		    </asp:Table>			
		</td>
	</tr>
	<tr>
	    <td>
	    <%--剑侠世界的 --%>
								 <fieldset>
				        <legend><asp:CheckBox runat="server" ID="CheckBoxPanelAdvancedQuery" Text="<%$ Resources:StringDef,AdvancedQuery %>" Checked="false" OnCheckedChanged="CheckBoxPanelAdvancedQuery_Click" AutoPostBack="true" /></legend>
				        <asp:Panel ID="PanelAdvancedQuery" runat="server" Visible="false">
				        
				        
		    <asp:Table ID="Table1" runat="server">

                    	<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, Menpailuxian %>" />
					</asp:TableHeaderCell>					
					<asp:TableCell>
						<asp:CheckBoxList ID="CheckBoxListRoleType" runat="server" RepeatDirection="Horizontal" RepeatLayout="Table" RepeatColumns="3" />
						<asp:LinkButton ID="LinkButtonChooseAll" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, ChooseAll %>" OnClick="LinkButtonChooseAll_Click" />
						<asp:LinkButton ID="LinkButtonNotChoose" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, notChoose %>" OnClick="LinkButtonNotChoose_Click" />
					</asp:TableCell>    
					         
				</asp:TableRow>	
								 <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, dwGameTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <table border="0" cellspacing="0" width="100%">
					        <tr>
					            <td align="right" >
					               <asp:TextBox ID="TextBoxdwGameTime_start" runat="server" CssClass="TextBox" />  
					               <asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef, Hours %>" />
					            </td>
					             <td>-</td>
					            <td align="left">
					                 <asp:TextBox ID="TextBoxdwGameTime_end" runat="server" CssClass="TextBox" />   
					                 <asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef, Hours %>" />    
					            </td>
					        </tr>
					    </table>		   
					</asp:TableCell>
				</asp:TableRow>
							 <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal6" runat="server" Text="<%$ Resources:StringDef, dwMoney %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					   <table border="0" cellspacing="0" width="100%">
					        <tr>
					            <td align="right" >
					               <asp:TextBox ID="TextBoxMoney_start" runat="server" CssClass="TextBox" />  
					            <asp:Literal ID="Literal7" runat="server" Text="<%$ Resources:StringDef, KingsoftCoin %>" />   
					            </td>
					            <td>-</td>
					            <td align="left">
					                 <asp:TextBox ID="TextBoxMoney_end" runat="server" CssClass="TextBox" />   
					                   <asp:Literal ID="Literal8" runat="server" Text="<%$ Resources:StringDef, KingsoftCoin %>" />     
					            </td>
					        </tr>
					    </table>
					</asp:TableCell>
				</asp:TableRow>
				
				 <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal9" runat="server" Text="<%$ Resources:StringDef, dwCreateTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell >
				
					<table border="0" cellspacing="0" width="100%" >
					        <tr>
					            <td align="right" width="60%">
					               <Blaze:ScriptCalendar ID="CreateTime_start" runat="server" ShowCheckbox="false" />
					            </td>
					             <td width="2%">-</td>
					            <td align="left" width="50%">
					                 <Blaze:ScriptCalendar ID="CreateTime_end" runat="server" ShowCheckbox="false" />       
					            </td>
					        </tr>
					    </table>
					    </asp:TableCell >
				</asp:TableRow>
						 <asp:TableRow>
						 	
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal10" runat="server" Text="<%$ Resources:StringDef, dwLastLoginTime %>" />
					</asp:TableHeaderCell>
						<asp:TableCell >
				
					<table border="0" cellspacing="0" width="100%">
					        <tr>
					            <td align="right" width="60%">
					               <Blaze:ScriptCalendar ID="LastLoginTime_start" runat="server" ShowCheckbox="false" />
					            </td>
					             <td width="2%">-</td>
					            <td align="left"  width="50%">
					                 <Blaze:ScriptCalendar ID="LastLoginTime_end" runat="server" ShowCheckbox="false" />       
					            </td>
					        </tr>
					    </table>
					    </asp:TableCell >					
				</asp:TableRow>
				 <%--<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, bySeries %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:CheckBoxList ID="CheckBoxListbySeries" runat="server" RepeatDirection="Horizontal" RepeatLayout="Table" RepeatColumns="5" />
					</asp:TableCell>
				</asp:TableRow>
					 --%>
			
			<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal11" runat="server" Text="<%$ Resources:StringDef, bySex %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxSex" runat="server" Rows="1">
							<asp:ListItem Text="<%$ Resources:StringDef, All %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, Male %>" Value="1" />
							<asp:ListItem Text="<%$ Resources:StringDef, Female %>" Value="2" />
						</asp:ListBox>
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal12" runat="server" Text="<%$ Resources:StringDef, wLevel %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxStartLevel" runat="server" Rows="1" />
						<asp:Literal ID="Literal13" runat="server" Text=" - " />
						<asp:ListBox ID="ListBoxEndLevel" runat="server" Rows="1" />
					</asp:TableCell>
				</asp:TableRow>
            	
              
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal14" runat="server" Text="<%$ Resources:StringDef, OrderBy %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxOrderBy" runat="server" Rows="1">
							<asp:ListItem Text="<%$ Resources:StringDef, wLevel %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, szRoleName %>" Value="1" />
						</asp:ListBox>
						<asp:ListBox ID="ListBoxOrderByType" runat="server" Rows="1">
							<asp:ListItem Text="<%$ Resources:StringDef, OrderASC %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, OrderDESC %>" Value="1" />
						</asp:ListBox>
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>
		        </asp:Panel>
		        </fieldset>	
		    </td>
	</tr>
</table>

<table align="center">
    <tr>
	    <td align="center">
		    <asp:LinkButton ID="LinkButtonSearchRole" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Query %>" OnClick="LinkButtonSearchRole_Click" />
            <asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="GameMaster.aspx" Text="<%$ Resources:StringDef, Return %>" />
		</td>
	</tr>
</table>	

<br />
	
<table align="center">	
    <tr>
	    <td>
            <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
	    </td>
    </tr>
</table>

<br />

<table align="center">
	<tr>
	    <td align="center">
	        <asp:UpdateProgress ID="UpdateProgress1" runat="server" >
	            <ProgressTemplate>
	                <asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />
	            </ProgressTemplate>
	        </asp:UpdateProgress> 
	    </td>
	</tr>
</table>

</contenttemplate>
</asp:UpdatePanel>

<br />

<asp:UpdatePanel runat="server" ID="UpdatePanelResult" UpdateMode="Conditional">
<contenttemplate>

<table width="100%">
	<asp:Panel ID="PanelResult" runat="server" Visible="false">
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
						<asp:Button ID="ButtonFirstPage" runat="server" Text="<%$ Resources:StringDef, FirstPage %>" OnClick="ButtonFirstPage_Click" Enabled="false" />
						<asp:Button ID="ButtonPreviousPage" runat="server" Text="<%$ Resources:StringDef, PreviousPage %>" OnClick="ButtonPreviousPage_Click" Enabled="false" />
						<asp:Button ID="ButtonNextPage" runat="server" Text="<%$ Resources:StringDef, NextPage %>" OnClick="ButtonNextPage_Click" Enabled="false" />					
					</td>
				</tr>
			</table>
		</td>
	</tr>	
	<tr>
		<td>
		<asp:DataGrid ID="DataGridResult" runat="server" AutoGenerateColumns="false" CssClass="TN" HeaderStyle-CssClass="TCH" CellSpacing="1">  
		    <Columns>
		        <asp:TemplateColumn>
		            <HeaderTemplate>
		                <asp:Literal runat="server" Text="<%$ Resources:StringDef, szRoleName %>" />
		            </HeaderTemplate>
		            <ItemTemplate>
		                <asp:HyperLink ID="LinkRoleName" runat="server" Target="_blank"  Text='<%# Eval("cRoleName") %>' SkinID="PlainText" />
		            </ItemTemplate>
		        </asp:TemplateColumn>		         
		        <asp:BoundColumn HeaderText="<%$ Resources:StringDef, szAccountName %>" ReadOnly="true" DataField="cAccountName" />
		        <asp:BoundColumn HeaderText="<%$ Resources:StringDef, wLevel %>" ReadOnly="true" DataField="iLevel" />
		        <asp:BoundColumn HeaderText="<%$ Resources:StringDef, bySex %>" ReadOnly="true" DataField="iSex" />
		        <asp:BoundColumn HeaderText="<%$ Resources:StringDef, Menpailuxian %>" ReadOnly="true" DataField="RoleClass" />
		         <asp:BoundColumn HeaderText="<%$ Resources:StringDef, CreateTime %>" ReadOnly="true" DataField="dCreateTime" />
		         <asp:BoundColumn HeaderText="<%$ Resources:StringDef, LastLoginTime %>" ReadOnly="true" DataField="dLastLoginTime" />
		        <asp:BoundColumn HeaderText="<%$ Resources:StringDef, GameTimeHours%>" ReadOnly="true" DataField="iGameTime" />
		        <asp:BoundColumn HeaderText="<%$ Resources:StringDef, dwMoney %>" ReadOnly="true" DataField="iMoney" />		        
		        <asp:BoundColumn HeaderText="<%$ Resources:StringDef, RoleGUID1 %>" ReadOnly="true" DataField="cRoleID" />
		        <asp:BoundColumn HeaderText="<%$ Resources:StringDef, RoleGUID1 %>" ReadOnly="true" DataField="cRoleNameUrlEncode" Visible="false" />
		        
		        <%-- 将该列改为了普通列。不再具有超链接功能。另外原来的列也存在错误："itemGuid"应该是"roleGUID" --%>
		        <%-- <asp:TemplateColumn>
		            <HeaderTemplate>
		                <asp:Literal ID="Literal15" runat="server" Text="<%$ Resources:StringDef, RoleGUID1 %>" />
		            </HeaderTemplate>
		            <ItemTemplate>
		                <asp:HyperLink ID="LinkItemGuid" runat="server"  Target="_self" NavigateUrl='<%# "RolePath.aspx?itemGuid=" + Eval("cRoleID") %>'  Text='<%# Eval("cRoleID") %>' ToolTip='<%$ Resources:StringDef, RolePathQuery %>' SkinID="PlainText" />
		            </ItemTemplate>
		        </asp:TemplateColumn> --%>
		        
		        <asp:TemplateColumn>
		            <HeaderTemplate>
		                <asp:Literal ID="Literal15" runat="server" Text="<%$ Resources:StringDef, Operation %>" />
		            </HeaderTemplate>
		            <ItemTemplate>
		                <asp:HyperLink ID="LinkRolePath" runat="server"  Target="_self" NavigateUrl='<%# "RolePath.aspx?roleGUID=" + Eval("cRoleID")+"&serverID="+Eval("serverID") %>'  Text='<%$ Resources:Web.Sitemap, RolePath %>' ToolTip='<%$ Resources:StringDef, RolePathQuery %>' SkinID="PlainText" />
		                <asp:HyperLink ID="LinkItemQuery" runat="server"  Target="_self" NavigateUrl='<%# "ItemQuery.aspx?cRoleName="+Eval("cRoleNameUrlEncode")+"&roleGUID=" + Eval("cRoleID")+"&serverID="+Eval("serverID") %>'  Text='<%$ Resources:Web.Sitemap, ItemTrack %>' ToolTip='<%$ Resources:StringDef, ItemTrackQuery %>' SkinID="PlainText" />
		            </ItemTemplate>
		        </asp:TemplateColumn>
		    </Columns>
		</asp:DataGrid>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
    <tr>
	    <td align="center">
            <asp:LinkButton ID="LinkButtonRoleFreeze" runat="server" OnClick="LinkButtonFreeze_Click" />
	        <asp:LinkButton ID="LinkButtonRoleUnFreeze" runat="server" OnClick="LinkButtonUnFreeze_Click" />
	        <asp:LinkButton ID="LinkButtonRoleAccountFreeze" runat="server" OnClick="LinkButtonRoleAccountFreeze_Click" />
	        <asp:LinkButton ID="LinkButtonRoleAccountUnFreeze" runat="server" OnClick="LinkButtonRoleAccountUnFreeze_Click" />
	    </td>
	</tr>
	</asp:Panel>
	<tr class="TableRowSpacing" />
</table>
</contenttemplate>
<Triggers>
    <asp:AsyncPostBackTrigger ControlID="LinkButtonSearchRole" EventName="Click" />
</Triggers>
</asp:UpdatePanel>





</asp:Content>