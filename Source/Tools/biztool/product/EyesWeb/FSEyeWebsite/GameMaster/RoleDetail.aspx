<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameMaster_RoleDetail, App_Web_6lmf-asn" maintainscrollpositiononpostback="true" title="<%$ Resources:StringDef, RoleDetail %>" theme="BlazeSky" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderTopMessage" runat="server">
<asp:Label ID="LabelTopMessage" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileOperating %>" CssClass="MessageNormal" />
</asp:Content>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="literal14" runat="server" Text="<%$ Resources:StringDef, Role %>" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<asp:Table runat="server" Width="80%">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal1" runat="server" Text="<%$ Resources:StringDef, RoleName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxRoleName" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal2" runat="server" Text="<%$ Resources:StringDef, Account %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxAccountName" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal5" runat="server" Text="<%$ Resources:StringDef, RoleClass %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxType" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal6" runat="server" Text="<%$ Resources:StringDef, Level %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxLevel" runat="server" />
					</asp:TableCell>
				</asp:TableRow>	
            	<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal4" runat="server" Text="<%$ Resources:StringDef, Sex %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxSex" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
            	<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal3" runat="server" Text="<%$ Resources:StringDef, Clan %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxTongName" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal9" runat="server" Text="<%$ Resources:StringDef, PlayedTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxPlayedTime" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal10" runat="server" Text="<%$ Resources:StringDef, CreateDate %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxCreateDate" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal11" runat="server" Text="<%$ Resources:StringDef, LastPlayingDate %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxLastPlayingDate" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal12" runat="server" Text="<%$ Resources:StringDef, LastPlayingIP %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxLastPlayingIP" runat="server" />
					</asp:TableCell>
				</asp:TableRow>			
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal7" runat="server" Text="<%$ Resources:StringDef, Money %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxMoney" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="literal8" runat="server" Text="<%$ Resources:StringDef, MoneyInBox %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxMoneyInBox" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, NoChatInGame %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxNoChatIn" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, NoLoginGame %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxNoLoginIn" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, UseRevivePos %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxUseRevivePosition" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, EnterMapId %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxEnterMapID" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, RevivePosId %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxReviveID" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, RevivePosX %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxReviveX" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, RevivePosY %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxReviveY" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, Guid %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:Label ID="TextBoxRoleGuid" runat="server" SkinID="Long" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, SpyLevel %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:DropDownList ID="ListBoxSpyLevel" runat="server" OnSelectedIndexChanged="ListBoxSpyLevel_SelectedIndexChanged" AutoPostBack="True">
							<asp:ListItem Text="<%$ Resources:StringDef, None %>" Value="0" />
							<asp:ListItem Text="1" Value="1" />
							<asp:ListItem Text="2" Value="2" />
							<asp:ListItem Text="3" Value="3" />
							<asp:ListItem Text="4" Value="4" />
							<asp:ListItem Text="5" Value="5" />
						</asp:DropDownList>
					</asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, IsOnline %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
                        <asp:Label runat="server" ID="LabelIsOnline" />
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>				
		</td>
	</tr>
	<tr class="TableRowSpacing" />	
	<tr>
		<td align="center">
			<asp:UpdateProgress ID="UpdateProgress1" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
				<ProgressTemplate>				
					<asp:Label runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileOperating %>" />
					</ProgressTemplate>
			</asp:UpdateProgress>				
		</td>
	</tr>	
	<tr>
		<td align="center">			
			<asp:Label ID="LabelOpMsg" runat="server" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<asp:HyperLink ID="LinkExpStatistic" runat="server" Text="<%$ Resources:StringDef,ExpStatistic %>" Target="_blank" Width="80" />
			<asp:HyperLink ID="LinkRoleItemInfo" runat="server" Text="<%$ Resources:StringDef, ItemInfo %>" Width="80"/>
			<asp:HyperLink ID="LinkRoleSkillInfo" runat="server" Text="<%$ Resources:StringDef, SkillInfo %>" Width="80"/>
			<asp:HyperLink ID="LinkClanInfo"  runat="server" Width="80"/>
            <asp:HyperLink ID="LinkViewAccountInfo"  runat="server" Text="<%$ Resources:StringDef,ViewAccountInfo %>" Width="80"/>
            <asp:HyperLink ID="LinkDownloadData" runat="server" Text="<%$ Resources:StringDef, GetRoleData %>" Target="_blank" Width="80"/>
        </td>
	</tr>
    <tr>
        <td align="center">        
            <asp:HyperLink ID="LinkExecuteGMCommand" runat="server" Text="<%$ Resources: StringDef,ExecuteGMCommand %>" Width="80"/>
            <asp:LinkButton ID="LinkButtonUseRevivePos" runat="server" Text="<%$ Resources:StringDef, UseRevivePos %>" OnClick="LinkButtonUseRevivePos_Click" Width="80"/>
            <ajax:ConfirmButtonExtender ID="ConfirmButtonExtender1" runat="server" TargetControlID="LinkButtonUseRevivePos" ConfirmText="<%$ Resources:StringDef, ConfirmUseRevivePos %>" />
            <asp:LinkButton ID="LinkButtonTalk"  runat="server" OnClick="LinkButtonGMCommand_Click" Width="80"/>
            <ajax:ConfirmButtonExtender ID="ConfirmButtonExtender2" runat="server" TargetControlID="LinkButtonTalk" ConfirmText="<%$ Resources:StringDef, ConfirmForbiddenChat %>" />
			<asp:LinkButton ID="LinkButtonFreeze" runat="server" OnClick="LinkButtonGMCommand_Click" Width="80"/>
			<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender3" runat="server" TargetControlID="LinkButtonFreeze" ConfirmText="<%$ Resources:StringDef, ConfirmFreezeRole %>" />
			<asp:LinkButton ID="LinkButtonSave"  runat="server" OnClick="LinkButtonGMCommand_Click" Width="80"/>
			<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender4" runat="server" TargetControlID="LinkButtonSave" ConfirmText="<%$ Resources:StringDef, ConfirmSaveRole %>" />
            <asp:LinkButton ID="LinkButtonQuit"  runat="server" OnClick="LinkButtonGMCommand_Click" Width="80"/>
            <ajax:ConfirmButtonExtender ID="ConfirmButtonExtender5" runat="server" TargetControlID="LinkButtonQuit" ConfirmText="<%$ Resources:StringDef, ConfirmKickRole %>" />
		</td>
	</tr>	
</table>

</contenttemplate>
</asp:UpdatePanel>

</asp:Content>

