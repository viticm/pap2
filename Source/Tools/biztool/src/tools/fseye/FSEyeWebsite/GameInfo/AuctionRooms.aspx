<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="AuctionRooms.aspx.cs" Inherits="GameInfo_AuctionRooms" Title="<%$ Resources:StringDef,AuctionRooms %>" %>
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
			<asp:Label ID="literal3" runat="server" Text="<%$ Resources:StringDef, AuctionRooms %>" SkinID="Title" />
		</td>
	</tr>
    <tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:Table runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell Width="25%">
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" OnSelectedServerGroupChanged="ServerGroupIndexChanged" AutoPostBack="true"/>
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" />
					</asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef,Seller %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <asp:TextBox ID="TextBoxSeller" runat="server" />
					    <asp:CheckBox ID="CheckBoxSeller" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" />
    				</asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef,Buyer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <asp:TextBox ID="TextBoxBuyer" runat="server" />
					    <asp:CheckBox ID="CheckBoxBuyer" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" />
    				</asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef,ItemName %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <asp:TextBox ID="TextBoxItemName" runat="server" />
					    <asp:CheckBox ID="CheckBoxItemName" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" />
    				</asp:TableCell>
				</asp:TableRow>
                <%--
                <asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef,StartTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
                        <Blaze:ScriptCalendar runat="server" ID = "StartDate" ShowCheckBox="true" />
					</asp:TableCell>
				</asp:TableRow>
            	<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal8" runat="server" Text="<%$ Resources:StringDef,EndTime %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
                        <Blaze:ScriptCalendar runat="server" ID = "EndDate" ShowCheckBox="true"  />
					</asp:TableCell>
				</asp:TableRow>
                --%>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, OrderBy %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxOrderBy" runat="server" Rows="1">
							<asp:ListItem Text="<%$ Resources:StringDef, CurrentPrice %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, PassPrice %>" Value="1" />							
						</asp:ListBox>
						<asp:ListBox ID="ListBoxOrderByType" runat="server" Rows="1">
							<asp:ListItem Text="<%$ Resources:StringDef, OrderASC %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, OrderDESC %>" Value="1" />
						</asp:ListBox>
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
            <asp:LinkButton ID="LinkButtonQuery" runat="server" OnClick="LinkButtonQuery_Click" Text="<%$Resources:StringDef,Query %>"  />
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
        	        <asp:Image runat="server" SkinID="PleaseWait" />
        	        <asp:Literal runat="server" Text="<%$Resources:StringDef,PleaseWaitWhileQuerying %>" />
        	    </ProgressTemplate>
        	</asp:UpdateProgress>
	    </td>
	</tr>
</table>

<asp:UpdatePanel runat="server" ID="UpdatePanelResult">
<contenttemplate>
<table width="100%">
	<tr class="TableRowSpacing" />
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
	<tr>
	    <td align="center">	           
	        <asp:DataGrid ID="DataGridAuctionItem" runat="server" AutoGenerateColumns="false" CssClass="TN" HeaderStyle-CssClass="TCH" CellSpacing="1">
	            <Columns>
	                <asp:BoundColumn DataField="Goods" HeaderText="<%$ Resources:StringDef,Item %>" ReadOnly="true" />
	                <asp:BoundColumn DataField="Seller" HeaderText="<%$ Resources:StringDef,Seller %>" ReadOnly="true" />	                
	                <asp:BoundColumn DataField="CurrentPrice" HeaderText="<%$ Resources:StringDef,CurrentPrice %>" ReadOnly="true" />
	                <asp:BoundColumn DataField="PassPrice" HeaderText="<%$ Resources:StringDef,PassPrice %>" ReadOnly="true" />	     
	                <asp:BoundColumn DataField="Buyer" HeaderText="<%$ Resources:StringDef,Buyer %>" ReadOnly="true" />
	                <asp:BoundColumn DataField="StartTime" HeaderText="<%$ Resources:StringDef,StartTime %>" ReadOnly="true" />
	                <asp:BoundColumn DataField="FinishTime" HeaderText="<%$ Resources:StringDef,EndTime %>" ReadOnly="true" />	                           
	            </Columns>
	        </asp:DataGrid>
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
                    <EnableAction AnimationTarget="LinkButtonQuery" Enabled="false" />               
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
                    <EnableAction AnimationTarget="LinkButtonQuery" Enabled="true" />              
                </Parallel>
                <StyleAction duration="0" Attribute="overflow" Value="visible" />
            </Sequence>
        </OnUpdated>
    </Animations>
</ajax:UpdatePanelAnimationExtender>
</asp:Content>