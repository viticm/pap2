<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_ControlGame, App_Web_jvyc5ygz" title="<%$ Resources:StringDef, ServerControl %>" theme="BlazeSky" %>

<%@ Register TagPrefix="Blaze" TagName="GameServerCheckBoxList" Src="~/Common/GameServerCheckBoxList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>
<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">

<asp:UpdatePanel runat="server" ID="UpdatePanelListBox">
<contenttemplate>

<table width="100%">
	<tr class="TableRowSpacing" />
	<tr>
		<td align="left">
			<asp:ListBox ID="ListBoxOperation" runat="server" SelectionMode="Single" Rows="1" AutoPostBack="true" />			
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

<table width="100%">
	<tr>
		<td align="center">
			<asp:UpdateProgress ID="UpdateProgress2" runat="server">
				<ProgressTemplate>	
					<asp:Label runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileOperating %>" SkinID="Operating" />
				</ProgressTemplate>
			</asp:UpdateProgress>
		</td>
	</tr>
</table>

<asp:UpdatePanel runat="server" ID="UpdatePanel1" >
<contenttemplate>
<TABLE width="100%">
<TBODY>
<TR>
<TD align=center>
<asp:Label id="LabelSuccess" runat="server" SkinID="MessageSuccess" EnableViewState="false" Visible="false"></asp:Label> 
<asp:Label id="LabelFailure" runat="server" SkinID="MessageFailure" EnableViewState="false" Visible="false"></asp:Label><BR />
<asp:Label id="LabelOpMsg" runat="server">
</asp:Label> 
</TD>
</TR>
<TR>
<TD align=center>
<asp:Panel id="PanelStartGame" runat="server">
				<table>
					<tr>
						<td>
							<asp:LinkButton ID="LinkButtonStartGame" runat="server" CssClass="LinkButton" OnClick="LinkButton_Click" Text="<%$ Resources:StringDef, StartGame %>" CommandName="StartGame" />
							<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender1" runat="server" TargetControlID="LinkButtonStartGame" ConfirmText="<%$ Resources:StringDef, ConfirmStartGame %>" />
						</td>
					</tr>
				</table>
			</asp:Panel> <asp:Panel id="PanelStopGame" runat="server">
				<table>
					<tr>
						<td>
							<asp:LinkButton ID="LinkButtonStopGame" runat="server" CssClass="LinkButton" OnClick="LinkButton_Click"	Text="<%$ Resources:StringDef, StopGame %>" CommandName="StopGame" />
							<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender2" runat="server" TargetControlID="LinkButtonStopGame" ConfirmText="<%$ Resources:StringDef, ConfirmStopGame %>" />
						</td>
					</tr>
				</table>
			</asp:Panel> <asp:Panel id="PanelUpdateGame" runat="server">
				<table>
					<tr>
						<td>
							<asp:Label ID="Label8" runat="server" Text="<%$ Resources:StringDef, PatchName %>" />
							<asp:TextBox ID="TextBoxPatchName" runat="server" Text="server.tar.gz" />
						</td>
						<td>
							<asp:LinkButton ID="LinkButtonUpdateGame" runat="server" OnClick="LinkButton_Click" Text="<%$ Resources:StringDef, UpdateGame %>" CommandName="UpdateGame" />
							<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender3" runat="server" TargetControlID="LinkButtonUpdateGame" ConfirmText="<%$ Resources:StringDef, ConfirmUpdateGame %>" />
						</td>
					</tr>
				</table>
			</asp:Panel> <asp:Panel id="PanelUpdateGuard" runat="server">
				<table>
					<tr>
						<td>
							<asp:LinkButton ID="LinkButtonUpdateGuard" runat="server" OnClick="LinkButton_Click" Text="<%$ Resources:StringDef, UpdateGuard %>" CommandName="UpdateGuard" />
							<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender4" runat="server" TargetControlID="LinkButtonUpdateGuard" ConfirmText="<%$ Resources:StringDef, ConfirmUpdateGuard %>" />
						</td>
					</tr>
				</table>
			</asp:Panel> <asp:Panel id="PanelSetServerTime" runat="server">
				<table>
				    <tr>
				        <td>
				            <Blaze:ScriptCalendar runat="server" ID="DateTimeToSet" CheckBoxToolTip="<%$ Resources:StringDef, UseSelectedTime %>" />
				        </td>
						<td>
							<asp:LinkButton ID="LinkButtonSetServerTime" runat="server" OnClick="LinkButton_Click" Text="<%$ Resources:StringDef, SetServerTime %>" CommandName="SetServerTime" />
							<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender5" runat="server" TargetControlID="LinkButtonSetServerTime" ConfirmText="<%$ Resources:StringDef, ConfirmSetServerTime %>" />
						</td>
					</tr>
				</table>
			</asp:Panel> <asp:Panel id="PanelDelete" runat="server">
				<table>
					<tr>
						<td>
							<asp:LinkButton ID="LinkButtonDelete" runat="server" OnClick="LinkButton_Click" Text="<%$ Resources:StringDef, DeleteServer %>" CommandName="DeleteServer" />
							<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender6" runat="server" TargetControlID="LinkButtonDelete" ConfirmText="<%$ Resources:StringDef, ConfirmDeleteServer %>" />
						</td>
					</tr>
				</table>
			</asp:Panel> <asp:Panel id="PanelCreateGameServerListFile" runat="server">
				<table>
					<tr>
						<td>
							<asp:LinkButton ID="LinkButtonCreateGameServerListFile" runat="server" OnClick="LinkButton_Click" Text="<%$ Resources:StringDef, CreateGameServerListFile %>" CommandName="CreateGameServerListFile" />
						</td>
					</tr>
				</table>
			</asp:Panel> <asp:Panel id="PanelAddDbUser" runat="server"><TABLE class="TN" cellSpacing=1><TBODY><TR><TD class="TCH" width="20%"><asp:Literal runat="server" Text="<%$ Resources:StringDef,UserName %>"></asp:Literal> </TD><TD class="TCN"><asp:TextBox id="TextBoxAddDbUserUserName" runat="server"></asp:TextBox> </TD></TR><TR><TD class="TCH"><asp:Literal runat="server" Text="<%$ Resources:StringDef,Password %>"></asp:Literal> </TD><TD class="TCN"><asp:TextBox id="TextBoxAddDbUserUserPassword" runat="server" TextMode="Password"></asp:TextBox> </TD></TR><TR><TD class="TCH"><asp:Literal id="Literal1" runat="server" Text="<%$ Resources:StringDef,New %>"></asp:Literal><asp:Literal id="Literal2" runat="server" Text="<%$ Resources:StringDef,UserName %>"></asp:Literal> </TD><TD class="TCN"><asp:TextBox id="TextBoxAddDbUserNewUserName" runat="server"></asp:TextBox> </TD></TR><TR><TD class="TCH"><asp:Literal id="Literal3" runat="server" Text="<%$ Resources:StringDef,Password %>"></asp:Literal> </TD><TD class="TCN"><asp:TextBox id="TextBoxAddDbUserNewUserPassword" runat="server" TextMode="Password"></asp:TextBox> </TD></TR><TR><TD class="TCH"><asp:Literal runat="server" Text="<%$ Resources:StringDef,DatabaseName %>"></asp:Literal> </TD><TD class="TCN"><asp:TextBox id="TextBoxAddDbUserDatabaseName" runat="server"></asp:TextBox> </TD></TR><TR><TD class="TCH"><asp:Literal id="Literal4" runat="server" Text="<%$ Resources:StringDef,Host %>"></asp:Literal> </TD><TD class="TCN"><asp:TextBox id="TextBoxAddDbUserHost" runat="server"></asp:TextBox> </TD></TR><TR><TD class="TCH"><asp:Literal runat="server" Text="<%$ Resources:StringDef,Privilege %>"></asp:Literal> </TD><TD class="TCN"><asp:Panel id="PanelPrivilege" runat="server" HorizontalAlign="Left"><asp:CheckBoxList id="CheckBoxListPrivilege" runat="server" RepeatColumns="5" RepeatLayout="Table" RepeatDirection="Horizontal" >
				                    <asp:ListItem Text="ALL" />
				                    <asp:ListItem Text="ALTER" />
				                    <asp:ListItem Text="ALTER ROUTINE" />
				                    <asp:ListItem Text="CREATE" />
				                    <asp:ListItem Text="CREATE ROUTINE" />
				                    <asp:ListItem Text="CREATE TEMPORARY TABLES" />
				                    <asp:ListItem Text="CREATE USER" />
				                    <asp:ListItem Text="CREATE VIEW" />
				                    <asp:ListItem Text="DELETE" />
				                    <asp:ListItem Text="DROP" />
				                    <asp:ListItem Text="EXECUTE" />
				                    <asp:ListItem Text="FILE" />
				                    <asp:ListItem Text="INDEX" />
				                    <asp:ListItem Text="INSERT" />
				                    <asp:ListItem Text="LOCK TABLES" />
				                    <asp:ListItem Text="PROCESS" />
				                    <asp:ListItem Text="RELOAD" />
				                    <asp:ListItem Text="REPLICATION CLIENT" />
				                    <asp:ListItem Text="REPLICATION SLAVE" />
				                    <asp:ListItem Text="SELECT" />
				                    <asp:ListItem Text="SHOW DATABASES" />
				                    <asp:ListItem Text="SHOW VIEW" />
				                    <asp:ListItem Text="SHUTDOWN" />
				                    <asp:ListItem Text="SUPER" />
				                    <asp:ListItem Text="UPDATE" />
				                    <asp:ListItem Text="GRANT OPTION" />
				                </asp:CheckBoxList> </asp:Panel> </TD></TR><TR><TD class="TCN" align=center colSpan=2><asp:LinkButton id="LinkButtonAddDbUser" onclick="LinkButtonAddDbUser_Click" runat="server" Text="<%$ Resources:StringDef, AddGameDbUser %>" CommandName="AddDbUser"></asp:LinkButton> <ajax:ConfirmButtonExtender id="ConfirmButtonExtender7" runat="server" ConfirmText="<%$ Resources:StringDef, ConfirmAddGameDbUser %>" TargetControlID="LinkButtonAddDbUser"></ajax:ConfirmButtonExtender> </TD></TR></TBODY></TABLE></asp:Panel> <asp:Panel id="PanelDeleteDbUser" runat="server">
				<table cellspacing="1" class="TN">
                    <tr>
				        <td class="TCH" width="20%">
				            <asp:Literal runat="server" Text="<%$ Resources:StringDef,UserName %>" />
				        </td>
				        <td class="TCN"> 
				            <asp:TextBox ID="TextBoxDelDbUserUserName" runat="server" />
				        </td>
				    </tr>
				    <tr>
				        <td class="TCH">
				            <asp:Literal runat="server" Text="<%$ Resources:StringDef,Password %>" />
				        </td>
				        <td class="TCN"> 
				            <asp:TextBox ID="TextBoxDelDbUserUserPassword" runat="server" TextMode="Password" />
				        </td>
				    </tr>
				    <tr>
				        <td class="TCH">
				            <asp:Literal runat="server" Text="<%$ Resources:StringDef,Delete %>" /><asp:Literal runat="server" Text="<%$ Resources:StringDef,UserName %>" />
				        </td>
				        <td class="TCN"> 
				            <asp:TextBox ID="TextBoxDelDbUserDelUserName" runat="server" />
				        </td>
				    </tr>
                    <tr>
				        <td class="TCH">
				            <asp:Literal runat="server" Text="<%$ Resources:StringDef,Host %>" />
				        </td>
				        <td class="TCN"> 
				            <asp:TextBox ID="TextBoxDelDbUserHost" runat="server" />
				        </td>
				    </tr>
					<tr>
						<td colspan="2" align="center" class="TCN">
						    <asp:LinkButton ID="LinkButtonDeleteDbUser" runat="server" OnClick="LinkButtonDeleteDbUser_Click" Text="<%$ Resources:StringDef, DeleteGameDbUser %>" CommandName="DeleteDbUser" />
							<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender8" runat="server" TargetControlID="LinkButtonDeleteDbUser" ConfirmText="<%$ Resources:StringDef, ConfirmDeleteGameDbUser %>" />							
						</td>
					</tr>
				</table>
			</asp:Panel> <asp:Panel id="PanelBackupDb" runat="server">
				<table>
					<tr>
					    <td>
					        <asp:RadioButtonList runat="server" ID="RadioButtonListBackupMode" RepeatDirection="Horizontal">
					            <asp:ListItem Text="<%$ Resources:StringDef, GameRoleDb %>" Selected="True" />
					            <asp:ListItem Text="<%$ Resources:StringDef, GameLogDb %>" />
					        </asp:RadioButtonList>
					    </td>
						<td>
							<asp:LinkButton ID="LinkButtonBackupDb" runat="server" OnClick="LinkButton_Click" Text="<%$ Resources:StringDef, BackupGameDb %>" CommandName="BackupDb" />
							<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender9" runat="server" TargetControlID="LinkButtonBackupDb" ConfirmText="<%$ Resources:StringDef, ConfirmBackupGameDb %>" />
						</td>
					</tr>
				</table>
			</asp:Panel> <asp:Panel id="PanelRollbackDb" runat="server">
				<table>
					<tr>
						<td>
							<asp:Label runat="server" Text="<%$ Resources:StringDef, GameDbBackupFile %>" />
							<asp:TextBox ID="TextBoxDumpFileName" runat="server" Text="" />
						</td>
						<td>
							<asp:LinkButton ID="LinkButtonRollbackDb" runat="server" OnClick="LinkButton_Click" Text="<%$ Resources:StringDef, RollbackGameDb %>" CommandName="RollbackDb" />
							<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender10" runat="server" TargetControlID="LinkButtonRollbackDb" ConfirmText="<%$ Resources:StringDef, ConfirmRollbackGameDb %>" />
						</td>
					</tr>
				</table>
			</asp:Panel> <asp:Panel id="PanelRestartDb" runat="server">
				<table>
					<tr>
						<td>
							<asp:LinkButton ID="LinkButtonRestartDb" runat="server" OnClick="LinkButton_Click" Text="<%$ Resources:StringDef, RestartGameDb %>" CommandName="RestartDb" />
							<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender11" runat="server" TargetControlID="LinkButtonRestartDb" ConfirmText="<%$ Resources:StringDef, ConfirmRestartGameDb %>" />
						</td>
					</tr>
				</table>
			</asp:Panel> <asp:Panel id="PanelRestartServer" runat="server">
				<table>
					<tr>
						<td>
							<asp:LinkButton ID="LinkButtonRestartServer" runat="server" OnClick="LinkButton_Click" Text="<%$ Resources:StringDef, RestartServer %>" />
							<ajax:ConfirmButtonExtender ID="ConfirmButtonExtender12" runat="server" TargetControlID="LinkButtonRestartServer" ConfirmText="<%$ Resources:StringDef, ConfirmRestartServer %>" />
						</td>
					</tr>
				</table>
			</asp:Panel> </TD></TR><TR class="TableRowSpacing"></TR><TR><TD align=center><Blaze:GameServerCheckBoxList id="GameServerCheckBoxList" runat="server" CssClass="TableNormal" GameServerPanelHeight="400" RepeatColumns="1"></Blaze:GameServerCheckBoxList> </TD></TR><TR class="TableRowSpacing"></TR><TR><TD align=center><asp:HyperLink runat="server" Text="<%$ Resources:StringDef, Return %>" NavigateUrl="ServerList.aspx"></asp:HyperLink> </TD></TR><TR class="TableRowSpacing"></TR></TBODY></TABLE>
</contenttemplate>
</asp:UpdatePanel>

</asp:Content>
