<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_PlayerCount, App_Web_jvyc5ygz" title="<%$ Resources:StringDef, PlayerCount %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">

	<asp:UpdatePanel id="UpdatePanel1" runat="server">
		<contenttemplate>

<table width="100%">
	<tr>
		<td align=center>
			<asp:Label runat="server" Text="<%$ Resources:StringDef, PlayerCount %>" SkinID="title" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align=center>
			<table cellpadding=0 cellspacing=0>
				<tr>
					<td>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" AutoPostBack="true" OnSelectedServerGroupChanged="ServerGroupIndexChanged" />
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" />
					</td>
					<td align="center">
                        <asp:LinkButton runat="server" ID="ButtonRefresh" Text="<%$ Resources:StringDef, Refresh %>" OnClick="ButtonRefresh_Click" />
						<asp:HyperLink runat="server" NavigateUrl="AdvancedPlayerCount.aspx" Text="<%$ Resources:StringDef, AdvancedPlayerCount %>" />
					</td>
					<td>
						<asp:UpdateProgress ID="UpdateProgress2" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
							<ProgressTemplate>	
								<asp:Image runat="server" SkinID="PleaseWait" />
								<asp:Label runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />
							</ProgressTemplate>
						</asp:UpdateProgress>
					</td>
				</tr>
			</table>			
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">			
			<ZGW:ZEDGRAPHWEB id="ZedGraphWebPlayerCount" runat="server" RenderMode="ImageTag" Width="840" Height="450" OnRenderGraph="ZedGraphWebPlayerCount_RenderGraph" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align=center>
			<table>
				<tr>
					<td>
						<asp:Label ID="LabelDate" runat="server"></asp:Label>
					</td>
					<td align="center">
						<asp:LinkButton ID="ButtonPreviousDay" runat="server" Text="<%$ Resources:StringDef,PreviousDay %>" OnClick="ButtonPreviousDay_Click" />
						<asp:LinkButton ID="ButtonNextDay" runat="server" Text="<%$ Resources:StringDef,NextDay %>" OnClick="ButtonNextDay_Click" />
						<asp:LinkButton ID="ButtonNow" runat="server" Text="<%$ Resources:StringDef,Today %>" OnClick="ButtonNow_Click" />
					</td>
				</tr>
			</table>			
		</td>
	</tr>
	<tr>
		<td align="center">			
			<ZGW:ZEDGRAPHWEB id="ZedGraphWebPlayerCountStatistic" runat="server" RenderMode="ImageTag" Width="840" Height="450" OnRenderGraph="ZedGraphWebPlayerCountStatistic_RenderGraph" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:HyperLink runat="server" NavigateUrl="Serverlist.aspx" Text="<%$ Resources:StringDef, Return %>" />
		</td>
	</tr>	
</table>

</contenttemplate>
	</asp:UpdatePanel>
</asp:Content>
