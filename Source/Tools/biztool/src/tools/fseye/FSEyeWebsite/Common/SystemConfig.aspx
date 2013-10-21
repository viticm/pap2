<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="SystemConfig.aspx.cs" Inherits="Common_SystemConfig" Title="<%$ Resources:StringDef, SystemConfig %>" %>
<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">
	<table width="100%">
		<tr class="TableRowSpacing" />
		<tr>
			<td align="center">
				<asp:Label ID="Label1" runat="server" CssClass="TableCaption" Text="<%$ Resources:StringDef, SystemConfig %>" />
			</td>
		</tr>
		<tr class="TableRowSpacing" />
		<tr>
			<td align="center">
				<table border="1" class="TableMedium">
					<tr>
						<td><asp:Literal runat="server" Text="<%$ Resources:StringDef, SystemListenPort %>" />
						</td>
						<td><asp:Label ID="LabelListenPort" runat="server" />
						</td>
					</tr>
				</table>
			</td>
		</tr>
		<tr class="TableRowSpacing" />
		<tr>
			<td align="center" colspan="2">
				<table cellspacing="0" cellpadding="0">
					<tr>
						<td>
							<ul>
								<li><asp:HyperLink runat="server" NavigateUrl="Homepage.aspx" Text="<%$ Resources:StringDef, Return %>" /></li>								
							</ul>
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>
</asp:Content>

