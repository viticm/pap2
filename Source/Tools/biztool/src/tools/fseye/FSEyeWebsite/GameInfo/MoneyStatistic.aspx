<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="MoneyStatistic.aspx.cs" Inherits="GameInfo_MoneyStatistic" MaintainScrollPositionOnPostback="true" Title="<%$ Resources:StringDef, MoneyStatistic %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderTopMessage" runat="server">
<asp:Label ID="LabelTopMessage" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" CssClass="MessageNormal" />
</asp:Content>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<table width="100%">
	<tr>
		<td align="center">
			<table class="TableNormal" border="1">
				<caption class="TableCaption"><asp:Literal ID="literal3" runat="server" Text="<%$ Resources:StringDef, MoneyStatistic %>" /></caption>
				<tr>
					<th><asp:Literal ID="literal2" runat="server" Text="<%$ Resources:StringDef, GameServer %>" /></th>
					<td align="left"><Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" /></td>
				</tr>
				<tr>
					<th><asp:Literal ID="literal4" runat="server" Text="<%$ Resources:StringDef, DateFrom %>" /></th>
					<td align="left">
						<asp:TextBox ID="TextBoxDateFrom" runat="server" CssClass="TextBox" MaxLength="10" />						
					</td>
				</tr>
				<tr>
					<th><asp:Literal ID="literal1" runat="server" Text="<%$ Resources:StringDef, DateTo %>" /></th>
					<td align="left">				
						<asp:TextBox ID="TextBoxDateTo" runat="server" CssClass="TextBox" MaxLength="10" />
					</td>
				</tr>
				<tr>
					<th><asp:Literal ID="literal5" runat="server" Text="<%$ Resources:StringDef, ChartType %>" /></th>
					<td align="left">
						<asp:ListBox ID="ListBoxChartType" runat="server" Rows="1">
							<asp:ListItem Text="<%$ Resources:StringDef, BarChart %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, LineChart %>" Value="1" />
						</asp:ListBox>
					</td>
				</tr>
			</table>
		</td>
	</tr>
		<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td>
						<ul>
							<li id="StatisticButton"><asp:LinkButton ID="LinkButtonStatistic" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Statistic %>" OnClick="LinkButtonStatistic_Click" OnClientClick="showAndBlinkTopMessage();" /></li>
							<li><a href="Statistic.aspx" class="LinkButton"><asp:Literal ID="Literal7" runat="server" Text="<%$ Resources:StringDef, Return %>" /></a></li>
						</ul>
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr class="TableRowSpacing"	 />
	<tr>
		<td>
			<asp:Image ID="ImageStatistic" runat="server" Visible="False" />
		</td>
	</tr>
</table>

</asp:Content>

