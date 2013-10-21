<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="GameSetting.aspx.cs" Inherits="GameMaster_GameSetting" Title="<%$ Resources:StringDef, GameSetting %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>
<table width="100%">
	<tr class="TableRowSpacing" />
	<tr>
		<td align="left">
		    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" AutoPostBack="true" OnSelectedServerGroupChanged="SelectedServerGroupChanged" />
			<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" AutoPostBack="true" />
		</td>
	</tr>
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, GameSetting %>" />
		</td>
	</tr>
	<tr>
		<td align="center">			
			<asp:Table runat="server" SkinID="Medium">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="1" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxGlobalVariable1" runat="server" CssClass="TextBox" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="2" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxGlobalVariable2" runat="server" CssClass="TextBox" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="3" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxGlobalVariable3" runat="server" CssClass="TextBox" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="4" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxGlobalVariable4" runat="server" CssClass="TextBox" />
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
            <asp:UpdateProgress ID="UpdateProgress1" runat="server">
               <ProgressTemplate>
                   <asp:Literal runat="server" Text="<%$ Resources:StringDef,PleaseWaitWhileQuerying%>" />
               </ProgressTemplate>
            </asp:UpdateProgress>
        </td>
    </tr>
    <tr>
        <td align="center">
			<asp:LinkButton ID="LinkButtonShow" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Show %>" OnClick="LinkButtonShow_Click" />
			<asp:LinkButton ID="LinkButtonApply" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Apply %>" OnClick="LinkButtonApply_Click" />
			<asp:HyperLink runat="server" NavigateUrl="GameMaster.aspx" Text="<%$ Resources:StringDef, Return %>" />
		</td>
	</tr>	
	<tr class="TableRowSpacing"></tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

</asp:Content>