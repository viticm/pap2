<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameMaster_ItemQuery, App_Web_6lmf-asn" title="<%$ Resources:Web.Sitemap, RoleItem %>" theme="BlazeSky" %>

<%@ Register TagPrefix="Blaze" TagName="DropDownListGameCenter" Src="~/Common/DropDownListGameCenter.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">
    <asp:UpdatePanel id="UpdatePanel1" runat="server">
        <contenttemplate>
<table align="center" width="60%">
<tr class="TableRowSpacing" />
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="literal3" runat="server" Text="<%$ Resources:StringDef, PageItemQueryaspxLabel %>" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />	
	<tr>
		<td>
			<asp:Table runat="server">
			<asp:TableRow>
			<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, ZoneName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:DropDownListGameCenter ID="DropDownListGameCenter" runat="server" />
					</asp:TableCell>
			</asp:TableRow>
			
			<%--帐号名--%>
			<%--   <asp:TableRow>
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
					            <td>
					                <asp:LinkButton ID="ShowRole" runat="server" Text="<%$ Resources:StringDef, ShowRole %>" OnClick="ShowRole_Click"></asp:LinkButton>
					            </td>
					        </tr>
					    </table>
					</asp:TableCell>
				</asp:TableRow>--%>
				
			<%--角色名--%>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, szRoleName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>					   
					    <table border="0" cellspacing="0" width="100%">
					        <tr>
					            <td width="60%">
					                 <asp:TextBox ID="TextBoxcRoleName" runat="server" />
					                 <asp:RequiredFieldValidator ID="RequiredFieldValidator" runat="server" 							
							                                                        ControlToValidate="TextBoxcRoleName"
							                                                        Display="None"
							                                                        SetFocusOnError="true"
							                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
						                                                        <ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender" TargetControlID="RequiredFieldValidator" />
						                                                        
					                <asp:Label ID="Labelmsg" runat="server" Visible="false" Text="<%$ Resources:StringDef, Roles %>" Color="red"></asp:Label>
					                  
					                   <asp:ListBox ID="ListBoxRoleName" runat="server" Rows="1" Visible="false" >
						            <asp:ListItem Text="<%$ Resources:StringDef, Unspecified %>" Value="0" />
						            </asp:ListBox> 
					            </td>
					        </tr>
					    </table>
					</asp:TableCell>
					</asp:TableRow>
					
					<%--物品名--%>
				<asp:TableRow>
				<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, ItemName %>" />
					</asp:TableHeaderCell>
					
						<asp:TableCell>
					    <table border="0" cellspacing="0" width="100%">
					        <tr>
					            <td align="center" width="60%">
					                <asp:TextBox ID="TextBoxcItemName" runat="server" />					                
					            </td>
					         <%--    <td align="left">
					                <asp:CheckBox ID="CheckBoxcItemName" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" />        
					            </td> --%>
					        </tr>
					    </table>
					</asp:TableCell>
			</asp:TableRow>
			
					<%--状态--%>
				<%-- 等待交易摆摊详细日值
				<asp:TableRow>
			<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, ItemState %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxItemState" runat="server" Rows="1">
						<asp:ListItem Text="<%$ Resources:StringDef, Unspecified %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, Birth %>" Value="1" />
							<asp:ListItem Text="<%$ Resources:StringDef, Strongger %>" Value="2" />
							<asp:ListItem Text="<%$ Resources:StringDef, Batch %>" Value="3" />
							<asp:ListItem Text="<%$ Resources:StringDef, Lost %>" Value="4" />
							<asp:ListItem Text="<%$ Resources:StringDef, OutDate %>" Value="5" />
							<asp:ListItem Text="<%$ Resources:StringDef, Exchange %>" Value="6" />
						</asp:ListBox>
					</asp:TableCell>
			</asp:TableRow>
			 --%>
			<%--最近登陆时间--%>
			 <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, DateTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell >
				
					<table>
					        <tr>
					            <td>					          
					               <Blaze:ScriptCalendar ID="RecentTime_start" runat="server" ShowCheckbox="false" />
					            </td>
					             <td> &nbsp;-----&nbsp; </td>
					            <td>					        
					                 <Blaze:ScriptCalendar ID="RecentTime_end" runat="server" ShowCheckbox="false" />       
					            </td>
					        </tr>
					    </table>
					    </asp:TableCell >
				</asp:TableRow>
				
				
			</asp:Table>
			
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr class="TableRowSpacing" />			
</table>
</contenttemplate>
</asp:UpdatePanel>
    
<!--物品结果集-->
    <asp:UpdatePanel runat="server" ID="UpdatePanelResult" UpdateMode="Conditional">
        <ContentTemplate>
            <table align="center">
                <tr>
                    <td>
                        <asp:LinkButton ID="LinkButtonItemQuery" runat="server" Text="<%$ Resources:StringDef, QueryThisItem %>"
                            OnClick="LinkButtonItemQuery_Click" />
                    </td>
                </tr>
                <tr class="TableRowSpacing" />
                <tr>
                    <td>
                        <asp:Label ID="LabelSuccess" runat="server" SkinID="MessageSuccess" />
                    </td>
                </tr>
                <tr>
                    <td>
                        <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
                    </td>
                </tr>
            </table>
            
            <br />
            
            <table align="center">
                <tr>
                    <td>
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
                <asp:Panel ID="ItemPanelResult" runat="server" __designer:wfdid="w1" Visible="true">
                    <tr />
                    <td />
                    <asp:DataGrid ID="DataGridItemResult" runat="server" __designer:wfdid="w2" CssClass="TN"
                        HeaderStyle-CssClass="TCH" AutoGenerateColumns="false" CellSpacing="1">
                        <Columns>
                            <asp:BoundColumn HeaderText="<%$ Resources:StringDef,ItemName %>" ReadOnly="true"
                                DataField="GoodsName" />
                            <asp:TemplateColumn>
                                <HeaderTemplate>
                                    <asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, ItemGuid %>" />
                                </HeaderTemplate>
                                <ItemTemplate>
                                    <asp:HyperLink ID="LinkItemGuid" runat="server" Target="_blank" NavigateUrl='<%# "ItemFlow.aspx?itemGUID=" + Eval("GoodsID")+"&serverID="+Eval("serverID") %>'
                                        Text='<%# Eval("GoodsID") %>' ToolTip='<%$ Resources:Web.Sitemap, ItemTrack %>'
                                        SkinID="PlainText" />
                                </ItemTemplate>
                            </asp:TemplateColumn>
                        </Columns>
                    </asp:DataGrid>
                    <tr />
                    <td align="center" />
                </asp:Panel>
                <tbody>
                    <tr class="TableRowSpacing">
                    </tr>
                </tbody>
            </table>
        </ContentTemplate>
        <Triggers>
            <asp:AsyncPostBackTrigger ControlID="LinkButtonItemQuery" EventName="Click"></asp:AsyncPostBackTrigger>
        </Triggers>
    </asp:UpdatePanel>
</asp:Content>
