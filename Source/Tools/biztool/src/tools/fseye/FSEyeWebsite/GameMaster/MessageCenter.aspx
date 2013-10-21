<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="MessageCenter.aspx.cs" Inherits="GameMaster_MessageCenter" Title="<%$ Resources:StringDef, MessageCenter %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="GameServerCheckBoxList" Src="~/Common/GameServerCheckBoxList.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>
<table width="100%">
	<tr class="TableRowSpacing" />
	<tr>
		<td class="TableCaption">
			<asp:Label ID="Label2" runat="server" Text="<%$ Resources:StringDef, MessageCenter %>" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:Table runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:GameServerCheckBoxList ID="GameServerCheckBoxList" runat="server" RepeatColumns="1" GameServerPanelHeight="200" CssClass="TableNormal" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, Message %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxMessage" runat="server" TextMode="MultiLine" SkinID="Multiline" />
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>		
		</td>
	</tr>
	<tr class="TableRowSpacing" />
    <tr>
        <td align="center">
            <asp:Label ID="LabelOpMsg" runat="server" />
        </td>
    </tr>
	<tr>
		<td align="center">
            <asp:UpdateProgress ID="UpdateProgress1" runat="server">
               <ProgressTemplate>
                   <asp:Literal runat="server" Text="<%$ Resources:StringDef,PleaseWaitWhileQuerying%>" />
               </ProgressTemplate>
            </asp:UpdateProgress>            			
		</td>
	</tr>
	<tr>
	    <td align="center">
			<asp:LinkButton ID="ButtonSendMessage" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Send %>" OnClick="ButtonSendMessage_Click" />
			<asp:HyperLink runat="server" NavigateUrl="GameMaster.aspx" Text="<%$ Resources:StringDef, Return %>" />
	    </td>
	</tr>
	</table>
</contenttemplate>
</asp:UpdatePanel>
</asp:Content>