<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Automation_BatchTaskDetail, App_Web_4eiilbni" title="<%$ Resources:StringDef, BatchTaskDetail %>" theme="BlazeSky" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:UpdatePanel id="UpdatePanel1" runat="server">
	<contenttemplate>
		<asp:Timer runat="server" Interval="2000" />		
		<table width="100%">
			<tr>
				<td>
					<asp:Table runat="server">
					    <asp:TableRow>
							<asp:TableHeaderCell>
								<asp:Literal runat="server" Text="<%$ Resources:StringDef, Name %>" />
							</asp:TableHeaderCell>
							<asp:TableCell>
								<asp:Literal runat="server" ID="LiteralName" />
							</asp:TableCell>
						</asp:TableRow>		
						<asp:TableRow>
							<asp:TableHeaderCell>
								<asp:Literal runat="server" Text="<%$ Resources:StringDef, Desc %>" />
							</asp:TableHeaderCell>
							<asp:TableCell>
								<asp:Literal runat="server" ID="LiteralDesc" />
							</asp:TableCell>
						</asp:TableRow>		
						<asp:TableRow>					
							<asp:TableHeaderCell>
								<asp:Literal runat="server" Text="<%$ Resources:StringDef, StartTime %>" />
							</asp:TableHeaderCell>
							<asp:TableCell>
								<asp:Literal runat="server" ID="LiteralBeginTime" />
							</asp:TableCell>
						</asp:TableRow>
						<asp:TableRow>
							<asp:TableHeaderCell>
								<asp:Literal runat="server" Text="<%$ Resources:StringDef, EscapedTime %>" />
							</asp:TableHeaderCell>
							<asp:TableCell>
								<asp:Literal runat="server" ID="LiteralEscapedTime" />
							</asp:TableCell>
						</asp:TableRow>
						<asp:TableRow>
							<asp:TableHeaderCell>
								<asp:Literal runat="server" Text="<%$ Resources:StringDef, EndTime %>" />
							</asp:TableHeaderCell>
							<asp:TableCell>
								<asp:Literal runat="server" ID="LiteralEndTime" />
							</asp:TableCell>
						</asp:TableRow>
					</asp:Table>
				</td>
			</tr>
			<tr>
				<td>
					<asp:Table runat="server" ID="TableServerList" />
				</td>
			</tr>
			<tr>
				<td align="center">
					<table cellpadding="0">
						<tr>
							<td align="center"> 
                                <asp:HyperLink runat="server" NavigateUrl="BatchTask.aspx" Text="<%$ Resources:StringDef, Return %>" />
							</td>
						</tr>
					</table>
				</td>
			</tr>
		</table>
	</contenttemplate>
</asp:UpdatePanel>
</asp:Content>

