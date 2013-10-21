<%@ control language="C#" autoeventwireup="true" inherits="Common_ItemIdSearch, App_Web_1ceayx6e" %>

<%--<table cellspacing="1px" cellpadding="0px" border="0px" width="100%" style="margin:0px;padding:0px;" >--%>
    <tr>
        <td class="TCH">
            <asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef,ItemName %>" />
        </td>
        <td class="TCN">
            <table border="0" cellspacing="0" width="100%">
	            <tr>
	                <td width="60%" align="right">
	                    <asp:TextBox runat="server" ID="ItemName" EnableTheming="true"/>
	                </td>
	                <td width="10%" align="left">
	                    <asp:ImageButton ID="ButtonSearchItemId" runat="server" EnableTheming="true" SkinID="SmallSearch" OnClick="ButtonSearchItemId_Click" ToolTip="<%$ Resources:StringDef,SearchItemID %>" />
	                </td>
	                <td align="left">
	                    <asp:Label ID="LabelSearchId" runat="server" EnableTheming="true" Visible="false" SkinID="Failure" />            
	                </td>
	            </tr>
	        </table>
	        <asp:Panel runat="server" ID="PanelSearchItemId" HorizontalAlign="Left" EnableTheming="true">
	            <asp:RadioButtonList ID="ListItemId" runat="server" RepeatColumns="3" RepeatLayout="Table" OnSelectedIndexChanged="SelectedItemIdChanged" AutoPostBack="true" align="center"/>
	        </asp:Panel>
        </td>
    </tr>
    <tr>
        <td class="TCH">
            <asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef,ItemID %>" />
        </td>
        <td class="TCN">
            <table border="0" cellspacing="0" width="100%">
                <tr>
                    <td width="60%" align="right">
                        <asp:TextBox ID="TextBoxItemId" EnableTheming="true" runat="server" />
                    </td>
                    <td></td>
                </tr>
            </table>
        </td>
    </tr>
<%--</table>--%>

<ajax:AutoCompleteExtender
    runat="server" 
    BehaviorID="AutoCompleteEx"
    ID="autoComplete1" 
    TargetControlID="ItemName"
    ServicePath="../WebService/FS2GameData.asmx"
    ServiceMethod="GetItemNameList"
    MinimumPrefixLength="1" 
    CompletionInterval="100"
    EnableCaching="true"
    CompletionSetCount="20"
    CompletionListCssClass="autocomplete_completionListElement" 
    CompletionListItemCssClass="autocomplete_listItem" 
    CompletionListHighlightedItemCssClass="autocomplete_highlightedListItem"
    DelimiterCharacters=" ">
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