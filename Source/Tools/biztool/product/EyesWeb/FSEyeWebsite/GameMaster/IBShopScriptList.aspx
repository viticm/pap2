<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameMaster_IBShopScriptList, App_Web_6lmf-asn" title="<%$ Resources:StringDef,IBShopScriptList %>" theme="BlazeSky" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<ContentTemplate>
<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Literal runat="server" Text="<%$ Resources:StringDef, IBShopScriptList %>" />
		</td>
	</tr>
	<tr>
		<td>
		    <asp:DataGrid ID="DataGridScript" runat="server" CssClass="TN" HeaderStyle-CssClass="TCH" AutoGenerateColumns="false" CellSpacing="1">
		        <Columns>
		            <asp:BoundColumn DataField="ID" HeaderText="<%$ Resources:StringDef,ID %>" ReadOnly="true" />
		            <asp:BoundColumn DataField="TimeStamp" HeaderText="<%$ Resources:StringDef,DateTime %>" ReadOnly="true" />
                    <asp:TemplateColumn HeaderText="<%$ Resources:StringDef,Operation %>">
		                <ItemTemplate>
		                    <asp:LinkButton ID="LinkButtonDelete" runat="server" SkinID="PlainText" CommandName="Delete" Text="<%$ Resources:StringDef,Delete %>" OnClick="LinkButton_Click"/>
		                    <asp:LinkButton ID="LinkButtonView" runat="server" SkinID="PlainText" CommandName="View" Text="<%$ Resources:StringDef,View %>" OnClick="LinkButton_Click"/>
		                    <asp:LinkButton ID="LinkButtonCreateFile" runat="server" SkinID="PlainText" CommandName="Create" Text="<%$ Resources:StringDef,CreateScriptFile %>" OnClick="LinkButton_Click"/>
		                </ItemTemplate>
		            </asp:TemplateColumn>
		        </Columns>
		    </asp:DataGrid>
		</td>
	</tr>
	<tr>
	    <td align="center">
	        <asp:Label ID="LabelOpMsg" runat="server" />
	    </td>
	</tr>
</table>
</ContentTemplate>
</asp:UpdatePanel>
<table width="100%">
	<tr>
		<td align="center">
            <asp:UpdateProgress ID="UpdateProgress2" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
				<ProgressTemplate>	
					<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileOperating %>" />
				</ProgressTemplate>
			</asp:UpdateProgress>
		</td>
	</tr>
</table>
<table width="100%">
    <tr>
        <td align="center">
            <asp:HyperLink runat="server" Text="<%$ Resources:StringDef,Add %>" NavigateUrl="~/GameMaster/IBShop.aspx" />
        </td>
    </tr>
</table>
</asp:Content>
