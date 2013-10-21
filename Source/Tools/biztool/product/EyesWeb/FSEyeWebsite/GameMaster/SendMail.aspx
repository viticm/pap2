<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameMaster_SendMail, App_Web_6lmf-asn" title="<%$ Resources:StringDef,SendMail %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>
<table width="80%" align="center">
	<tr class="TableRowSpacing" />
	<tr>
		<td class="TableCaption">
			<asp:Label ID="Label2" runat="server" Text="<%$ Resources:StringDef,SendMail %>" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:Table ID="Table1" runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell Width="30%">
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" OnSelectedServerGroupChanged="ServerGroupIndexChanged" AutoPostBack="true" />
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="LiteralReceiver" runat="server" Text="<%$ Resources:StringDef, Receiver %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxInputReceiver" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
            	<asp:TableRow>            	
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef, Subject %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxInputSubject" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal4" runat="server" Text="Cost Money" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxInputCostMoney" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal5" runat="server" Text="Post Money" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxInputPostMoney" runat="server" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef, Content %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxInputContent" runat="server" TextMode="MultiLine" SkinID="Multiline" />
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>
			<asp:UpdatePanel runat="server" ID="UpdatePanelItem">
			    <ContentTemplate>
			        <fieldset>
			            <legend>
			                <asp:CheckBox ID="CheckBoxItem" runat="server" Text="<%$ Resources:StringDef,Attachment %>" AutoPostBack="true" OnCheckedChanged="CheckBoxItem_CheckedChanged" />
			            </legend>
			            <asp:Table ID="TableItem" runat="server" Visible="false">
                            <asp:TableRow>
		                		<asp:TableHeaderCell Width="30%">
		                			<asp:Literal ID="Literal45" runat="server" Text="<%$ Resources:StringDef,ItemName %>" />
		                		</asp:TableHeaderCell>
		                		<asp:TableCell>
		                		    <table border="0" cellspacing="0" width="100%">
		                		        <tr>
		                		            <td width="60%" align="right">
		                		                <asp:TextBox ID="TextBoxItemName" runat="server" />            
		                		            </td>
		                		            <td width="5%" align="left">
		                		                <asp:ImageButton ID="ButtonSearchItemId" runat="server" SkinID="SmallSearch" OnClick="ButtonSearchItemId_Click" ToolTip="<%$ Resources:StringDef,SearchItemID %>" />            
		                		            </td>
		                		            <td align="center">
		                		                <asp:Label ID="LabelSearchId" runat="server" Visible="false" SkinID="Failure" />            
		                		            </td>
		                		        </tr>
		                		    </table>
		                		    <asp:Panel runat="server" ID="PanelSearchItemId" HorizontalAlign = "Left">
		                		        <asp:RadioButtonList ID="RadioButtonListItemId" runat="server" RepeatColumns="3" RepeatLayout="Table" AutoPostBack="true" align="center" OnSelectedIndexChanged="SelectedItemIdChanged" />
		                		    </asp:Panel>
		                		</asp:TableCell>
		                	</asp:TableRow>		
		                	<asp:TableRow>
		                		<asp:TableHeaderCell>
		                			<asp:Literal runat="server" Text="<%$ Resources:StringDef,ItemId %>" />
		                		</asp:TableHeaderCell>
		                		<asp:TableCell>
		                		    <table border="0" cellspacing="0" width="100%">
		                		        <tr>
		                		            <td width="60%" align="right">
		                		                <asp:TextBox ID="TextBoxItemId" runat="server" />            
		                		            </td>
		                		            <td></td>
		                		        </tr>
		                		    </table>		                		    
		                		</asp:TableCell>
		                	</asp:TableRow>		
                            <asp:TableRow>
		                		<asp:TableHeaderCell>
		                			<asp:Literal runat="server" Text="<%$ Resources:StringDef,Count %>" />
		                		</asp:TableHeaderCell>
		                		<asp:TableCell>
		                		    <table border="0" cellspacing="0" width="100%">
		                		        <tr>
		                		            <td width="60%" align="right">
		                		                <asp:TextBox ID="TextBoxInputItemCount" runat="server" />
		                		            </td>
		                		            <td></td>
		                		        </tr>
		                		    </table>
		                		</asp:TableCell>
		                	</asp:TableRow>
		                </asp:Table>
			        </fieldset>
			        
                    <ajax:AutoCompleteExtender
                        runat="server" 
                        BehaviorID="AutoCompleteEx"
                        ID="autoComplete1" 
                        TargetControlID="TextBoxItemName"
                        ServicePath="../WebService/FS2GameData.asmx"
                        ServiceMethod="GetItemNameList"
                        MinimumPrefixLength="1" 
                        CompletionInterval="100"
                        EnableCaching="true"
                        CompletionSetCount="20"
                        CompletionListCssClass="autocomplete_completionListElement" 
                        CompletionListItemCssClass="autocomplete_listItem" 
                        CompletionListHighlightedItemCssClass="autocomplete_highlightedListItem">
                        <Animations>
                            <OnShow>
                                <Sequence>
                                    <%-- Make the completion list transparent and then show it --%>
                                    <OpacityAction Opacity="0" />
                                    <HideAction Visible="true" />
                                    
                                    <%--Cache the original size of the completion list the first time
                                        the animation is played and then set it to zero --%>
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
                    
			    </ContentTemplate>
			</asp:UpdatePanel>	
		</td>
	</tr>
	<tr>
	    <td align="center">
	        <asp:Label ID="LabelOpMsg" runat="server" />
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
            <asp:LinkButton ID="LinkButtonSendMail" runat="server" Text="<%$ Resources:StringDef, Send %>" OnClick="LinkButtonSendMail_Click" />
			<asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="GameMaster.aspx" Text="<%$ Resources:StringDef, Return %>" />			
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>
</asp:Content>