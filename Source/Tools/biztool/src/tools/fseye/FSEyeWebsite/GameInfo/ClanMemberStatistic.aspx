<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="ClanMemberStatistic.aspx.cs" Inherits="GameInfo_ClanMemberStatistic" Title="<%$ Resources:StringDef, SocialRelation %>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>
<table width="100%">
	<tr>
		<td align="center">
			<asp:Label ID="LabelTitle" runat="server" SkinID="Title" />
		</td>
	</tr>
	<tr>
	    <td align="center">
	        <asp:Label runat="server" ID="LabelOpMsg" SkinID="failure" />
	    </td>
	</tr>
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
	<tr>
	    <td>
	        <asp:DataGrid ID="DataGridResult" runat="server" AutoGenerateColumns="false" CellSpacing="1" CssClass="TN" HeaderStyle-CssClass="TCH" >
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
            <asp:DataGrid ID="DataGridRole" runat="server" AutoGenerateColumns="false" CssClass="TN" HeaderStyle-CssClass="TCH" CellSpacing="1">  
		        <Columns>		            
		            <asp:TemplateColumn>
		                <HeaderTemplate>
		                    <asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, RoleName %>" />
		                </HeaderTemplate>
		                <ItemTemplate>
		                    <asp:HyperLink ID="LinkRoleName" runat="server" Target="_blank" NavigateUrl='<%# "~/GameMaster/RoleDetail.aspx?roleID=" + Eval("RoleId") + "&serverId=" + Eval("ServerId")%>' Text='<%# Eval("RoleName") %>' SkinID="PlainText" />
		                </ItemTemplate>
		            </asp:TemplateColumn>
		            <asp:BoundColumn HeaderText="<%$ Resources:StringDef, Account %>" ReadOnly="true" DataField="RoleAccount" />
		            <asp:BoundColumn HeaderText="<%$ Resources:StringDef, Level %>" ReadOnly="true" DataField="Level" />
		            <asp:BoundColumn HeaderText="<%$ Resources:StringDef, Sex %>" ReadOnly="true" DataField="Sex" />
		            <asp:BoundColumn HeaderText="<%$ Resources:StringDef, RoleClass %>" ReadOnly="true" DataField="RoleClass" />
		            <asp:BoundColumn HeaderText="<%$ Resources:StringDef, ChatDisable %>" ReadOnly="true" DataField="NoChatInGame" />
		            <asp:BoundColumn HeaderText="<%$ Resources:StringDef, Freeze %>" ReadOnly="true" DataField="NoLoginGame" />
		            <asp:BoundColumn HeaderText="<%$ Resources:StringDef, Online %>" ReadOnly="true" DataField="Online" />
		        </Columns>
		    </asp:DataGrid>
	    </td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>
</asp:Content>