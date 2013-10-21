<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameMaster_PlayerWho, App_Web_6lmf-asn" title="<%$ Resources:StringDef, OnlinePlayer %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>
<table width="100%">
    <tr>
        <td class="TableCaption">
			<asp:Literal ID="literal1" runat="server" Text="<%$ Resources:StringDef, OnlinePlayer %>" />
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
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, RoleClass %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>						
						<asp:CheckBoxList ID="CheckBoxListRoleType" runat="server" RepeatDirection="Horizontal" RepeatLayout="Table" RepeatColumns="3" />
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, Level %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:TextBox ID="TextBoxStartLevel" runat="server" Text="1" SkinID="Custom" Height="18px" EnableTheming="true"/>
						<ajax:NumericUpDownExtender ID="NumericUpDownExtenderStartLevel" runat="server" TargetControlID="TextBoxStartLevel" Width="60" Minimum="1" Maximum="120" Tag="1" />
						<asp:RangeValidator ID="RangeValidatorStartLevel" runat="server" ControlToValidate="TextBoxStartLevel" MinimumValue="1" MaximumValue="120" Type="Integer" ErrorMessage="<%$ Resources:StringDef,MessageLevelInvalid %>" Display="None" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtenderStartLevel" TargetControlID="RangeValidatorStartLevel"/>
						<asp:Literal runat="server" Text=" - " />
						<asp:TextBox ID="TextBoxEndLevel" runat="server" Text="120" SkinID="Custom" Height="18px" EnableTheming="true"/>
        				<ajax:NumericUpDownExtender ID="NumericUpDownExtenderEndLevel" runat="server" TargetControlID="TextBoxEndLevel" Width="60" Minimum="1" Maximum="120" Tag="1" />
						<asp:RangeValidator ID="RangeValidatorEndLevel" runat="server" ControlToValidate="TextBoxEndLevel" MinimumValue="1" MaximumValue="120" Type="Integer" ErrorMessage="<%$ Resources:StringDef,MessageLevelInvalid %>" Display="None" />
						<ajax:ValidatorCalloutExtender runat="Server" ID="ValidatorCalloutExtender2" TargetControlID="RangeValidatorEndLevel"/>                             
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, Sex %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxSex" runat="server" Rows="1">
							<asp:ListItem Text="<%$ Resources:StringDef, All %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, Male %>" Value="1" />
							<asp:ListItem Text="<%$ Resources:StringDef, Female %>" Value="2" />
						</asp:ListBox>
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal runat="server" Text="<%$ Resources:StringDef, OrderBy %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxOrderBy" runat="server" Rows="1">
							<asp:ListItem Text="<%$ Resources:StringDef, ID %>" Value="0" />
							<asp:ListItem Text="<%$ Resources:StringDef, Level %>" Value="1" />
							<asp:ListItem Text="<%$ Resources:StringDef, RoleName %>" Value="2" />
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
            <asp:LinkButton ID="ButtonRefresh" runat="server" OnClick="ButtonRefresh_Click" Text="<%$Resources:StringDef,Refresh %>"  />
			<asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="GameMaster.aspx" Text="<%$ Resources:StringDef, Return %>" />			
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
	<asp:Panel ID="PanelResult" runat="server" Visible="false">
	<tr>
		<td align="right">
			<table cellpadding="0">
				<tr>
					<td>
						<asp:Label runat="server" Text="<%$ Resources:StringDef, RecordPerPage %>" />
						<asp:ListBox ID="ListBoxRecordPerPage" runat="server" Rows="1">
							<asp:ListItem Text="20" Value="20" />
							<asp:ListItem Text="50" Value="50" />
							<asp:ListItem Text="100" Value="100" />
						</asp:ListBox>
						<asp:Button ID="ButtonFirstPage" runat="server" Text="<%$ Resources:StringDef, FirstPage %>" OnClick="ButtonFirstPage_Click" Enabled="false" />
						<asp:Button ID="ButtonPreviousPage" runat="server" Text="<%$ Resources:StringDef, PreviousPage %>" OnClick="ButtonPreviousPage_Click" Enabled="false" />
						<asp:Button ID="ButtonNextPage" runat="server" Text="<%$ Resources:StringDef, NextPage %>" OnClick="ButtonNextPage_Click" Enabled="false" />					
					</td>
				</tr>
			</table>
		</td>
	</tr>	
	<tr class="TableRowSpacing" />
	<tr>
		<td>
		<asp:Table ID="TableSearchRoleList" runat="server">
			<asp:TableHeaderRow>				
				<asp:TableHeaderCell><asp:Literal ID="literal12" runat="server" Text="<%$ Resources:StringDef, ID %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal ID="literal5" runat="server" Text="<%$ Resources:StringDef, RoleName %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal ID="literal9" runat="server" Text="<%$ Resources:StringDef, Account %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal ID="literal6" runat="server" Text="<%$ Resources:StringDef, Tong %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal ID="literal7" runat="server" Text="<%$ Resources:StringDef, Level %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal ID="literal8" runat="server" Text="<%$ Resources:StringDef, Sex %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal ID="literal10" runat="server" Text="<%$ Resources:StringDef, RoleClass %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell Width="20%"><asp:Literal ID="literal11" runat="server" Text="<%$ Resources:StringDef, Operation %>" /></asp:TableHeaderCell>
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
                    <EnableAction AnimationTarget="ButtonRefresh" Enabled="false" />               
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
                    <EnableAction AnimationTarget="ButtonRefresh" Enabled="true" />              
                </Parallel>
                <StyleAction duration="0" Attribute="overflow" Value="visible" />
            </Sequence>
        </OnUpdated>
    </Animations>
</ajax:UpdatePanelAnimationExtender>
</asp:Content>