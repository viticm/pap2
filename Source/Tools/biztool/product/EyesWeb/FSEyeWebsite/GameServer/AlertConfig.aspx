<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_AlertConfig, App_Web_jvyc5ygz" title="<%$ Resources:StringDef, AlertConfig %>" theme="BlazeSky" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:UpdatePanel runat="server" ID="UpdatePanel1" >
<ContentTemplate>
<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="Literal11" runat="server" Text="<%$ Resources:StringDef, AlertConfig %>" />
		</td>
	</tr>	
	<tr>
		<td align="center">
			<asp:Table runat="server" SkinID="Medium">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal10" runat="server" Text="<%$ Resources:StringDef, CheckPeriod %>" />						
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxCheckPeriod" runat="server" />						
						<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, Millisecond %>" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator5" runat="server" 							
							ControlToValidate="TextBoxCheckPeriod"
							Display="Dynamic"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, FieldRequired %>">
						</asp:RequiredFieldValidator>
						<asp:RangeValidator ID="RangeValidator1" runat="server"
							ControlToValidate="TextBoxCheckPeriod"
							MinimumValue="100"
							MaximumValue="3600000"
							Display="Dynamic"
							SetFocusOnError="true"							
							ErrorMessage="<%$ Resources:StringDef, InvalidCheckPeriod %>">
						</asp:RangeValidator>
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="Literal15" runat="server" Text="<%$ Resources:StringDef, AlertCondition %>" />
		</td>
	</tr>
	<tr>
		<td align="center">
			<asp:Table ID="TableAlertCondition" runat="server" SkinID="Medium">
				<asp:TableHeaderRow>
					<asp:TableHeaderCell><asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef, Operation %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef, Index %>" /></asp:TableHeaderCell>					
					<asp:TableHeaderCell><asp:Literal ID="Literal8" runat="server" Text="<%$ Resources:StringDef, Comment %>" /></asp:TableHeaderCell>
				</asp:TableHeaderRow>
			</asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
	    <td width="100%" align="center">
	        <asp:Label runat="server" ID="LabelOpMsg" />
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
            <asp:LinkButton ID="LinkButtonApply" runat="server" CssClass="LinkButton" OnClick="LinkButtonApply_Click" Text="<%$ Resources:StringDef, Apply %>"/>
			<asp:HyperLink runat="server" NavigateUrl="AddAlertCondition.aspx" Text="<%$ Resources:StringDef, AddAlertCondition %>" />
			<asp:HyperLink runat="server" NavigateUrl="ServerList.aspx" Text="<%$ Resources:StringDef, Return %>" />
		</td>
	</tr>
	<tr class="TableRowSpacing"></tr>
</table>
</ContentTemplate>
</asp:UpdatePanel>
</asp:Content>