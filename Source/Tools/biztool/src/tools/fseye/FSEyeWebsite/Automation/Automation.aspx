<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="Automation.aspx.cs" Inherits="GameServer_Automation" Title="<%$ Resources:StringDef, Automation %>" %>
<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<table width="100%">
	<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">
						<asp:HyperLink runat="server" NavigateUrl="ScheduledTaskList.aspx" Text="<%$ Resources:StringDef, Scheduler %>" />
						<asp:HyperLink runat="server" NavigateUrl="StrategyList.aspx" Text="<%$ Resources:StringDef, StrategyConfig %>" />
						<asp:HyperLink runat="server" NavigateUrl="BatchTask.aspx" Text="<%$ Resources:StringDef, BatchTask %>" />
					</td>
				</tr>
			</table>			
		</td>
	</tr>
</table>
</asp:Content>

