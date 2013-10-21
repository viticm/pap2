<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Security_DeleteGroup, App_Web_jztwgtrl" title="<%$ Resources:StringDef, DeleteGroup %>" theme="BlazeSky" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, DeleteGroup %>" />
		</td>		
	</tr>
	<tr>
		<td align="center">
			<asp:Table ID="Table1" runat="server" SkinID="Medium">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, GroupName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxGroupName" runat="server" MaxLength="16" CssClass="TextBox" />
						<asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server"
							ControlToValidate="TextBoxGroupName"
							Display="Dynamic"
							SetFocusOnError="true"
							ErrorMessage="<%$ Resources:StringDef, FieldRequired %>">
						</asp:RequiredFieldValidator>						
						<ajax:autocompleteextender runat="server" id="autoComplete1" targetcontrolid="TextBoxGroupName" BehaviorID="AutoCompleteEx"
							ServicePath="../WebService/FSEyeService.asmx" servicemethod="GetUserGroupNames" minimumprefixlength="1"
							completioninterval="100" enablecaching="true" completionsetcount="10" completionlistcssclass="autocomplete_completionListElement"
							completionlistitemcssclass="autocomplete_listItem" completionlisthighlighteditemcssclass="autocomplete_highlightedListItem"
							delimitercharacters=";, :">
							<Animations>
								<OnShow>
									<Sequence>
										<%-- Make the completion list transparent and then show it --%>
										<OpacityAction Opacity="0" />
										<HideAction Visible="true" />
										
										<%--Cache the original size of the completion list the first time the animation is played and then set it to zero --%>
										<ScriptAction Script="
										// Cache the size and setup the initial size
										var behavior = $find('AutoCompleteEx');
										if (!behavior._height) {
										var target = behavior.get_completionList();
										behavior._height = target.offsetHeight - 2;
										target.style.height = '0px';
										}" />
										
										<%-- Expand from 0px to the appropriate size while fading in --%>
										<Parallel Duration=".4">
											<FadeIn />
											<Length PropertyKey="height" StartValue="0" EndValueScript="$find('AutoCompleteEx')._height" />
										</Parallel>
									</Sequence>
								</OnShow>
								<OnHide>
								<%-- Collapse down to 0px and fade out --%>
									<Parallel Duration=".4">
										<FadeOut />
										<Length PropertyKey="height" StartValueScript="$find('AutoCompleteEx')._height" EndValue="0" />
									</Parallel>
								</OnHide>
							</Animations>
						</ajax:AutoCompleteExtender>
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">						
						<asp:LinkButton ID="LinkButtonDelete" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Delete %>" OnClick="LinkButtonDelete_Click" />
						<asp:HyperLink runat="server" NavigateUrl="ListGroup.aspx" Text="<%$ Resources:StringDef, Return %>" />						
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

</asp:Content>

