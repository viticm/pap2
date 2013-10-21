<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="DatabaseInfo.aspx.cs" Inherits="GameServer_DatabaseInfo" Title="<%$ Resources:StringDef,DatabaseInfo %>" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td align="center" colspan="4">
			<asp:Label ID="literal3" runat="server" Text="<%$ Resources:StringDef, DatabaseInfo %>" SkinID="Title" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
	    <td align="center">
	        <asp:Label runat="server" ID="LabelOpMsg" SkinID="Failure" />
	    </td>
	</tr>
    <tr>
	    <td>
            <asp:UpdateProgress ID="UpdateProgress1" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
                <ProgressTemplate>
                    <asp:Label runat="server" Text="<%$ Resources:StringDef,Refreshing %>" />
                </ProgressTemplate>
            </asp:UpdateProgress>
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
	    <td align="center" width="100%" colspan="2">
	        <fieldset>
	            <legend><asp:LinkButton runat="server" ID="LinkButtonProcess" CommandName="Process" Text="<%$ Resources:StringDef,Process %>" OnClick="LinkButton_Click" SkinID="PlainText" /></legend>
	            <table width="100%" align="center" border="0" cellpadding="10">
	                <tr>
	                    <td>
                            <asp:DataGrid runat="server" ID="DataGridProcessList" AutoGenerateColumns="false" HeaderStyle-CssClass="TCH" CellSpacing="1" CssClass="TN" OnPageIndexChanged="DataGridProcessList_PageIndexChanged" Visible="true" >
	                            <Columns>
	                                <asp:BoundColumn DataField="User" ReadOnly="true" HeaderText="<%$ Resources:StringDef,User %>" />
	                                <asp:BoundColumn DataField="Database" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Database %>" />
	                                <asp:BoundColumn DataField="Command" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Command %>" />
	                                <asp:BoundColumn DataField="Time" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Time %>" />
	                                <asp:BoundColumn DataField="State" ReadOnly="true" HeaderText="<%$ Resources:StringDef,State %>" />
	                                <asp:BoundColumn DataField="Info" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Information %>"  />
	                            </Columns>
                            </asp:DataGrid>                            
	                    </td>
	                </tr>
	            </table>
	        </fieldset>	        
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
    <tr>
	    <td align="center" width="100%" colspan="2">
	        <fieldset>
	            <legend><asp:LinkButton runat="server" ID="LinkButton1" CommandName="Variable" Text="<%$ Resources:StringDef,Variable %>" OnClick="LinkButton_Click" SkinID="PlainText" /></legend>	            
	            <table width="100%" align="center" border="0" cellpadding="10">
	                <tr>
	                    <td>
	                        <asp:DataGrid runat="server" ID="DataGridVariables" AutoGenerateColumns="false" HeaderStyle-CssClass="TCH" CellSpacing="1" CssClass="TN" OnPageIndexChanged="DataGridVariables_PageIndexChanged" Visible="false" >
	                            <Columns>
	                                <asp:BoundColumn DataField="VariableName" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Name %>" />
	                                <asp:BoundColumn DataField="VariableValue" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Value %>" />
	                            </Columns>
	                        </asp:DataGrid>	                        
	                    </td>
	                </tr>
                </table>
            </fieldset>
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
    <tr>
	    <td align="center" width="100%" colspan="2">
	        <fieldset>
    	        <legend><asp:LinkButton runat="server" ID="LinkButton2" CommandName="State" Text="<%$ Resources:StringDef,State %>" OnClick="LinkButton_Click" SkinID="PlainText" /></legend>
	            <table width="100%" align="center" border="0" cellpadding="10">
	                <tr>
	                    <td>
	                        <asp:DataGrid runat="server" ID="DataGridState" AutoGenerateColumns="false" HeaderStyle-CssClass="TCH" CellSpacing="1" CssClass="TN" OnPageIndexChanged="DataGridState_PageIndexChanged" Visible="false" >	                            
	                            <Columns>
	                                <asp:BoundColumn DataField="StateName" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Name %>" />
	                                <asp:BoundColumn DataField="StateValue" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Value %>" />
	                            </Columns>
	                        </asp:DataGrid>	                        
	                    </td>
	                </tr>
                </table>
            </fieldset>
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
	    <td align="center" width="100%" colspan="2">
	        <fieldset>
	            <legend><asp:LinkButton runat="server" ID="LinkButton3" CommandName="User" Text="<%$ Resources:StringDef,User %>" OnClick="LinkButton_Click" SkinID="PlainText" /></legend>	            
	            <table width="100%" align="center" border="0" cellpadding="10">
	                <tr>
	                    <td>
	                        <asp:DataGrid runat="server" ID="DataGridUser" AutoGenerateColumns="false" HeaderStyle-CssClass="TCH" CellSpacing="1" CssClass="TN"  OnPageIndexChanged="DataGridUser_PageIndexChanged" Visible="false" >	                            
	                            <Columns>
	                                <asp:BoundColumn DataField="UserName" ReadOnly="true" HeaderText="<%$ Resources:StringDef,Name %>" />	                                
	                            </Columns>
	                        </asp:DataGrid>	                        
	                    </td>
	                </tr>
                </table>
            </fieldset>
	    </td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>
</asp:Content>