<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="Statistic.aspx.cs" Inherits="GameInfo_Statistic" Title="<%$ Resources:StringDef, Statistic %>" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<table width="100%">
	<tr>
		<td align="center">
	        <asp:HyperLink runat="server" NavigateUrl="ActivePlayerStatistic.aspx" Text="<%$ Resources:StringDef, ActivePlayer %>" />
			<asp:HyperLink runat="server" NavigateUrl="ProfessionalSexStatistic.aspx" Text="<%$ Resources:StringDef, ProfessionalSex %>" />
			<asp:HyperLink runat="server" NavigateUrl="LevelDistribution.aspx" Text="<%$ Resources:StringDef, LevelDistribution %>" />
			<asp:HyperLink runat="server" NavigateUrl="ItemStatistic.aspx" Text="<%$ Resources:StringDef, Item %>" />
			<asp:HyperLink runat="server" NavigateUrl="AuctionStatistic.aspx" Text="<%$ Resources:StringDef, Auction %>" />
			<asp:HyperLink runat="server" NavigateUrl="MailStatistic.aspx" Text="<%$ Resources:StringDef, Mail %>" />
			<asp:HyperLink runat="server" NavigateUrl="TaiSuiStatistic.aspx" Text="<%$ Resources:StringDef, TaiSui %>" />
			<asp:HyperLink runat="server" NavigateUrl="NpcDropItemStatistic.aspx" Text="<%$ Resources:StringDef, NpcDropItem %>" />			
		</td>
	</tr>
	<tr>
	    <td align="center">
	        <asp:HyperLink runat="server" NavigateUrl="EasyStatistic.aspx" Text="<%$ Resources:StringDef, EasyStatistic %>" />
			<asp:HyperLink runat="server" NavigateUrl="RoleCountStatistic.aspx" Text="<%$ Resources:StringDef, RoleCountStatistic %>" />
			<asp:HyperLink runat="server" NavigateUrl="AdvancedStatistic.aspx" Text="<%$ Resources:StringDef, AdvancedStatistic %>" />
			<asp:HyperLink runat="server" NavigateUrl="NpcDeathStatistic.aspx" Text="<%$ Resources:StringDef, NpcDeath %>" />
			<asp:HyperLink runat="server" NavigateUrl="ExpStatistic.aspx" Text="<%$ Resources:StringDef, ExpStatistic %>" />
	    </td>
	</tr>
</table>

</asp:Content>

