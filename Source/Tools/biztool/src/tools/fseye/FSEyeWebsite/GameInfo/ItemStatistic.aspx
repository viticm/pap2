<%@ Page Language="C#" MasterPageFile="~/Common/Main.master"AutoEventWireup="true" CodeFile="ItemStatistic.aspx.cs" Inherits="GameInfo_Item" Title="<%$ Resources:StringDef,Item %>" %>

<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<script type="text/javascript">
var updateRoleItemInfoTipAjax = new sack("../GameMaster/GameMasterHandler.ashx");
var updatingRoleItemInfo = false;

function init() {
}

function updateRoleItemInfo(itemTemplateId) {
	if (!updatingRoleItemInfo)
	{
		updatingRoleItemInfo = true;		
		updateRoleItemInfoTipAjax.reset();
		updateRoleItemInfoTipAjax.element = "DivRoleItemInfo";
		updateRoleItemInfoTipAjax.runAJAX("op=GetRoleItemInfo&itemTemplateId="+itemTemplateId);
		updateRoleItemInfoTipAjax.onCompletion = new Function("updatingRoleItemInfo=false;TagToTip('DivRoleItemInfo',BALLOON, true,BORDERWIDTH, 0,OPACITY,85,BORDERCOLOR ,'#000000'  );");
	}
}
</script>
<div id="DivRoleItemInfo" style="display:none;" ></div>

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td align="center" colspan="4">
			<asp:Label ID="literal3" runat="server" Text="<%$ Resources:StringDef, Item %>" SkinID="Title" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:Table ID="Table1" runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell Width="25%">
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" OnSelectedServerGroupChanged="ServerGroupIndexChanged" AutoPostBack="true"/>
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" />
					</asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal45" runat="server" Text="<%$ Resources:StringDef,ItemName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <table border="0" cellspacing="0" width="100%">
					        <tr>
					            <td width="60%" align="right">
					                <asp:TextBox ID="TextBoxItemName" runat="server" autocomplete="off" />
					            </td>
					            <td width="10%" align="left">
					                <asp:ImageButton runat="server" SkinID="SmallSearch" OnClick="ButtonSearchItemId_Click" ToolTip="<%$ Resources:StringDef,SearchItemID %>" />            
					            </td>
					            <td align="center">
					                <asp:Label ID="LabelSearchId" runat="server" Visible="false" SkinID="Failure" />
					            </td>
					        </tr>					        
					    </table> 					    
					    
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
					    
					    <asp:Panel runat="server" ID="PanelSearchItemId" HorizontalAlign="Left">
					        <asp:RadioButtonList ID="RadioButtonListItemId" runat="server" RepeatColumns="3" RepeatLayout="Table" OnSelectedIndexChanged="SelectedItemIdChanged" AutoPostBack="true" align="center"/>
					    </asp:Panel>
					</asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal44" runat="server" Text="<%$ Resources:StringDef,ItemID %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <table width="100%" border="0" cellspacing="0">
					        <tr>
					            <td align="right" width="60%">
					                <asp:TextBox ID="TextBoxItemId" runat="server" />        
					            </td>
					            <td></td>
					        </tr>
					    </table>					    
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef,StartTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
                        <Blaze:ScriptCalendar runat="server" ID = "StartDate" ShowCheckBox="false" />
					</asp:TableCell>
				</asp:TableRow>
            	<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal8" runat="server" Text="<%$ Resources:StringDef,EndTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
                        <Blaze:ScriptCalendar runat="server" ID = "EndDate" ShowCheckBox="false" />
					</asp:TableCell>
				</asp:TableRow>
            	<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal18" runat="server" Text="<%$ Resources:StringDef,Source %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
                        <asp:DropDownList ID="ListBoxWay" runat="server" >
                            <%--
                            <asp:ListItem Text="<%$ Resources:StringDef,All %>" Value="All" /> 
                            <asp:ListItem Text="<%$ Resources:StringDef,Buy %>" Value="Buy" />
                            --%>
                            <asp:ListItem Text="<%$ Resources:StringDef,Drop %>" Value="Drop" />
                        </asp:DropDownList>
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
    <tr>	
	    <td align="center">
	        <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
	    </td>
	</tr>
    <tr>
	    <td align="center">
            <asp:LinkButton ID="Statistic" runat="server" OnClick="ButtonStatistic_Click" Text="<%$Resources:StringDef,Statistic %>" />
			<asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="Statistic.aspx" Text="<%$ Resources:StringDef, Return %>" />												
        </td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

<table width="100%">
	<tr>
	    <td align="center">
        	<asp:UpdateProgress ID="UpdateProcess1" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
        	    <ProgressTemplate>
        	        <asp:Image ID="Image1" runat="server" SkinID="PleaseWait" />
        	        <asp:Literal ID="Literal7" runat="server" Text="<%$Resources:StringDef,PleaseWaitWhileQuerying %>" />
        	    </ProgressTemplate>
        	</asp:UpdateProgress>
	    </td>
	</tr>	
</table>


<asp:UpdatePanel runat="server" ID="UpdatePanelResult">
<contenttemplate>
<table width="100%">
	<asp:Panel ID="PanelResult" runat="server" Visible="false">
    <tr>
		<td align="right">
			<table cellpadding="0">
				<tr>
					<td>
						<asp:Label runat="server" Text="<%$ Resources:StringDef, RecordPerPage %>" />
						<asp:ListBox ID="ListBoxRecordPerPage" runat="server" Rows="1">
							<asp:ListItem Text="20" Value="20" />
							<asp:ListItem Text="50" Value="50" Selected="true" />
							<asp:ListItem Text="100" Value="100" />
						</asp:ListBox>
						<asp:Button ID="ButtonFirstPage" runat="server" Text="<%$ Resources:StringDef, FirstPage %>" CommandName="FirstPage" OnClick="ButtonPage_Click" Enabled="false" />
						<asp:Button ID="ButtonPreviousPage" runat="server" Text="<%$ Resources:StringDef, PreviousPage %>" CommandName="PreviousPage" OnClick="ButtonPage_Click" Enabled="false" />
						<asp:Button ID="ButtonNextPage" runat="server" Text="<%$ Resources:StringDef, NextPage %>" CommandName="NextPage" OnClick="ButtonPage_Click" Enabled="false" />
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />	
	<tr>
	    <td align="center">
	        <asp:Label runat="server" ID="LabelResult" SkinID="Title" />
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td>
		<asp:Table ID="TableSearchItem" runat="server">
			<asp:TableHeaderRow>
			    <asp:TableHeaderCell><asp:Literal ID="literal5"  runat="server" Text="<%$ Resources:StringDef,Item %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal ID="literal91" runat="server" Text="<%$ Resources:StringDef, ItemSource%>" /></asp:TableHeaderCell>				
				<asp:TableHeaderCell><asp:Literal ID="literal4"  runat="server" Text="<%$ Resources:StringDef,Source %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal ID="literal9"  runat="server" Text="<%$ Resources:StringDef,Count %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal ID="literal11" runat="server" Text="<%$ Resources:StringDef,Percentage %>" /></asp:TableHeaderCell>
			</asp:TableHeaderRow>
		</asp:Table>
		</td>
	</tr>
	</asp:Panel>
	<tr class="TableRowSpacing" />
</table>
</contenttemplate>
</asp:UpdatePanel>
<ajax:UpdatePanelAnimationExtender ID="upae" BehaviorID="animation" runat="server" TargetControlID="UpdatePanelResult">
    <Animations>
        <OnUpdating>
            <Sequence>
                <%-- Disable all the controls --%>
                <Parallel duration="0">
                    <EnableAction AnimationTarget="Statistic" Enabled="false" />               
                </Parallel>
                
                <StyleAction Attribute="overflow" Value="hidden" />
                
                <%-- Do each of the selected effects --%>
                <Parallel duration=".3" Fps="30">                    
                    <Condition ConditionScript="true">
                        <Resize Height="0" />
                    </Condition>
                </Parallel>
            </Sequence>
        </OnUpdating>
        <OnUpdated>
            <Sequence>
                <%-- Do each of the selected effects --%>
                <Parallel Duration=".3" Fps="30">
                    <Condition ConditionScript="true">
                        <%-- Get the stored height --%>
                        <Resize AnimationTarget="UpdatePanelResult" HeightScript="500" Unit="px" />
                    </Condition>
                </Parallel>
                <Parallel duration="0">
                    <EnableAction AnimationTarget="Statistic" Enabled="true" />              
                </Parallel>
                <StyleAction duration="0" Attribute="overflow" Value="visible" />
            </Sequence>
        </OnUpdated>
    </Animations>
</ajax:UpdatePanelAnimationExtender>
</asp:Content>