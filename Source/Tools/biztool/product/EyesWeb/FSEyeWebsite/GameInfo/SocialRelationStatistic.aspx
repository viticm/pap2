<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameInfo_SocialRelationStatistic, App_Web_d99p0fdh" title="<%$ Resources:StringDef, SocialRelation %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>
<table width="100%">
	<tr>
		<td align="center" colspan="4">
			<asp:Label ID="literal3" runat="server" Text="<%$ Resources:StringDef, SocialRelation %>" SkinID="Title" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:Table ID="Table1" runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell Width="25%">
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" OnSelectedServerGroupChanged="ServerGroupIndexChanged" AutoPostBack="true"/>
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, Name %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxName" runat="server" />
						<asp:CheckBox ID="CheckBoxName" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, Leader %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxLeaderName" runat="server" />
						<asp:CheckBox ID="CheckBoxLeaderName" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, Type %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:DropDownList ID="DropDownListType" runat="server" >
						    <asp:ListItem Text="<%$ Resources:StringDef,All %>" Value="1" Selected="True"/>
						    <asp:ListItem Text="<%$ Resources:StringDef,Clan %>" Value="2" />
						    <asp:ListItem Text="<%$ Resources:StringDef,Seigneur %>" Value="3" />
						    <asp:ListItem Text="<%$ Resources:StringDef,Nation %>" Value="4" />
						</asp:DropDownList>
					</asp:TableCell>
				</asp:TableRow>
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
            <asp:LinkButton ID="LinkButtonQuery" runat="server" OnClick="LinkButtonQuery_Click" Text="<%$Resources:StringDef,Statistic %>"  />
			<asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="Statistic.aspx" Text="<%$ Resources:StringDef, Return %>" />			
        </td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

<table width="100%">
	<tr>
	    <td align="center">
        	<asp:UpdateProgress ID="UpdateProcess1" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
        	    <ProgressTemplate>
        	        <asp:Image ID="Image1" runat="server" SkinID="PleaseWait" />
        	        <asp:Literal ID="Literal8" runat="server" Text="<%$Resources:StringDef,PleaseWaitWhileQuerying %>" />
        	    </ProgressTemplate>
        	</asp:UpdateProgress>
	    </td>
	</tr>
</table>


<asp:UpdatePanel runat="server" ID="UpdatePanelResult">
<contenttemplate>
<table width="100%">
    <tr class="TableRowSpacing" />	
    <asp:Panel runat="server" ID="PanelResult">
	<tr>
		<td align="right">
			<table cellpadding="0">
				<tr>
					<td>
						<asp:Label runat="server" Text="<%$ Resources:StringDef, RecordPerPage %>" />
						<asp:ListBox ID="ListBoxRecordPerPage" runat="server" Rows="1">
							<asp:ListItem Text="20" Value="20" />
							<asp:ListItem Text="50" Value="50" Selected="true" />
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
	<tr class="TableRowSpacing" />	
	<tr>
	    <td>
	        <asp:DataGrid ID="DataGridResult" runat="server" AutoGenerateColumns="false" CellSpacing="1" CssClass="TN" HeaderStyle-CssClass="TCH">
	            <Columns>
	                <asp:BoundColumn DataField="Name" HeaderText="<%$ Resources:StringDef,Name %>" ReadOnly="true" ></asp:BoundColumn>
	                <%--
	                <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,Name %>">
    	                <ItemTemplate>
	                        <a href='../GameInfo/SocialRelation.aspx?serverId=<%# Eval("ServerId") %>&nodeGuid=<%# Eval("NodeGUID") %>' ><%# Eval("Name") %></a>
	                    </ItemTemplate>
	                </asp:TemplateColumn>
	                 --%>
	                <asp:BoundColumn DataField="Type" HeaderText="<%$ Resources:StringDef,Type %>" ReadOnly="true"/>
	                <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,Leader %>">
    	                <ItemTemplate>
	                        <a href='../GameMaster/RoleDetail.aspx?serverId=<%# Eval("ServerId") %>&roleName=<%# Eval("RoleNameEncoded") %>' target="_blank" ><%# Eval("Leader") %></a>
	                    </ItemTemplate>
	                </asp:TemplateColumn>
	                <asp:BoundColumn DataField="ViewParent" HeaderText="<%$ Resources:StringDef,InvolvedOrganization %>" ReadOnly="true"/>
                    <asp:BoundColumn DataField="ChildCount" HeaderText="<%$ Resources:StringDef,MemberCount %>" ReadOnly="true"/>
	                <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,MemberInfo %>">
    	                <ItemTemplate>
	                        <a href='../GameInfo/ClanMemberStatistic.aspx?serverId=<%# Eval("ServerId") %>&nodeGuid=<%# Eval("NodeGUID") %>' target="_blank" ><asp:Literal runat="server" Text="<%$ Resources:StringDef,MemberInfo %>" /></a>
	                    </ItemTemplate>
	                </asp:TemplateColumn>
	                <%-- 
                    <asp:ButtonColumn ButtonType="LinkButton" CommandName="MemberInfo" HeaderText="<%$ Resources:StringDef,Member %>"¡¡Text="<%$ Resources:StringDef,MemberInfo %>" />
                    --%>
                    <asp:BoundColumn DataField="NodeGUID" ReadOnly="true" Visible="false" />
	            </Columns>
	        </asp:DataGrid>
	    </td>
	</tr>
	</asp:Panel>
</table>
</contenttemplate>
</asp:UpdatePanel>
<ajax:UpdatePanelAnimationExtender ID="upae" BehaviorID="animation" runat="server" TargetControlID="UpdatePanelResult">
    <Animations>
        <OnUpdating>
            <Sequence>
                <%-- Disable all the controls --%>
                <Parallel duration="0">
                    <EnableAction AnimationTarget="LinkButtonQuery" Enabled="false" />               
                </Parallel>
                
                <StyleAction Attribute="overflow" Value="hidden" />
                
                <%-- Do each of the selected effects --%>
                <Parallel duration=".3" Fps="30">                    
                    <Condition ConditionScript="true">
                        <Resize Height="0" />
                    </Condition>
                </Parallel>
            </Sequence>
        </OnUpdating>
        <OnUpdated>
            <Sequence>
                <%-- Do each of the selected effects --%>
                <Parallel Duration=".3" Fps="30">
                    <Condition ConditionScript="true">
                        <%-- Get the stored height --%>
                        <Resize AnimationTarget="UpdatePanelResult" HeightScript="500" Unit="px" />
                    </Condition>
                </Parallel>
                <Parallel duration="0">
                    <EnableAction AnimationTarget="LinkButtonQuery" Enabled="true" />              
                </Parallel>
                <StyleAction duration="0" Attribute="overflow" Value="visible" />
            </Sequence>
        </OnUpdated>
    </Animations>
</ajax:UpdatePanelAnimationExtender>
</asp:Content>