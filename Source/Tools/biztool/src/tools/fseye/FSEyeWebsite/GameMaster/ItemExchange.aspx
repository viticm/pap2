<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="ItemExchange.aspx.cs" Inherits="GameInfo_ItemExchange" Title="<%$ Resources:StringDef,ItemExchange %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td align="center" colspan="4">
			<asp:Label ID="literal3" runat="server" Text="<%$ Resources:StringDef,ItemExchange %>" SkinID="Title" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />	
    <tr>
        <td align="center">
			<asp:Table ID="Table1" runat="server">
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
				        <asp:Literal runat="server" Text="<%$ Resources:StringDef,ItemName %>" />
				    </asp:TableHeaderCell>
				    <asp:TableCell>
				        <table cellspacing="0" border="0" width="100%">
				            <tr>
				                <td align="right" width="60%">
				                    <asp:TextBox ID="TextBoxInputItemName" runat="server" />            
				                </td>
				                <td align="left">
				                    <asp:CheckBox ID="CheckBoxItemName" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" />    
				                </td>
				            </tr>
				        </table>
				    </asp:TableCell>
				</asp:TableRow>
            	<asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal runat="server" Text="<%$ Resources:StringDef,Giver %>" />
				    </asp:TableHeaderCell>
				    <asp:TableCell>
				        <table cellspacing="0" border="0" width="100%">
				            <tr>
				                <td align="right" width="60%">
				                    <asp:TextBox ID="TextBoxInputGiver" runat="server" />
				                </td>
				                <td align="left">
				                    <asp:CheckBox ID="CheckBoxGiver" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" />
				                </td>
				            </tr>
				        </table>
				    </asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal runat="server" Text="<%$ Resources:StringDef,Receiver %>" />
				    </asp:TableHeaderCell>
				    <asp:TableCell>
				        <table cellspacing="0" border="0" width="100%">
				            <tr>
				                <td align="right" width="60%">
				                    <asp:TextBox ID="TextBoxInputReceiver" runat="server" />
				                </td>
				                <td align="left">
				                    <asp:CheckBox ID="CheckBoxReceiver" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" />
				                </td>
				            </tr>
				        </table>				        
				    </asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal runat="server" Text="<%$ Resources:StringDef,StartTime %>" />
				    </asp:TableHeaderCell>
				    <asp:TableCell>
				        <Blaze:ScriptCalendar ID="StartDate" runat="server" ShowCheckbox="false" />
				    </asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal runat="server" Text="<%$ Resources:StringDef,EndTime %>" />
				    </asp:TableHeaderCell>
				    <asp:TableCell>
				        <Blaze:ScriptCalendar ID="EndDate" runat="server" ShowCheckbox="false" />
				    </asp:TableCell>
				</asp:TableRow>
            </asp:Table> 
        </td>
    </tr>
    <tr>
        <td align="center">
            <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
        </td>
    </tr>    
    <tr>
        <td align="center">
            <asp:LinkButton ID="LinkButtonQuery" runat="server" OnClick="LinkButtonQuery_Click" Text="<%$ Resources:StringDef,Query %>" />			
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

<table width="100%">
    <tr>
		<td align="center">
            <asp:UpdateProgress ID="UpdateProgress1" runat="server">
               <ProgressTemplate>
                   <asp:Literal runat="server" Text="<%$ Resources:StringDef,PleaseWaitWhileQuerying%>" />
               </ProgressTemplate>
            </asp:UpdateProgress>
`       </td>
    </tr>
</table>

<asp:UpdatePanel runat="server" ID="UpdatePanelResult">
<contenttemplate>
<table width="100%">
<asp:Panel ID="PanelResult" runat="server" Visible="false">
    <tr>
	    <td align="center">
			<asp:Label ID="LabelResult" runat="server" SkinID="Title" />
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td>
		    <asp:DataGrid ID="DataGridResult" runat="server" AutoGenerateColumns="false" CssClass="TN" HeaderStyle-CssClass="TCH" CellSpacing="1" >
		        <Columns>
		            <asp:BoundColumn HeaderText="<%$ Resources:StringDef,Time %>"  ReadOnly="True" DataField="LogTime" />
                    <asp:TemplateColumn>
		                <HeaderTemplate>
		                    <asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, Giver %>" />
		                </HeaderTemplate>
		                <ItemTemplate>
		                    <asp:HyperLink ID="LinkGiver" runat="server" Target="_blank" NavigateUrl='<%# "~/GameMaster/RoleDetail.aspx?roleID=" + Eval("GiverId") + "&serverId=" + Eval("ServerId")%>' Text='<%# Eval("GiverName") %>' SkinID="PlainText" />
		                </ItemTemplate>
		            </asp:TemplateColumn>
    	            <asp:BoundColumn HeaderText="<%$ Resources:StringDef,ItemName %>" ReadOnly="True" DataField="ItemName" />
                    <asp:TemplateColumn>
		                <HeaderTemplate>
		                    <asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, Receiver %>" />
		                </HeaderTemplate>
		                <ItemTemplate>
		                    <asp:HyperLink ID="LinkReceiver" runat="server" Target="_blank" NavigateUrl='<%# "~/GameMaster/RoleDetail.aspx?roleID=" + Eval("ReceiverId") + "&serverId=" + Eval("ServerId")%>' Text='<%# Eval("ReceiverName") %>' SkinID="PlainText" />
		                </ItemTemplate>
		            </asp:TemplateColumn>
		            <asp:BoundColumn ReadOnly="true" Visible="false" DataField="ServerId" />
		        </Columns>
		    </asp:DataGrid>
		</td>
	</tr>
	</asp:Panel>
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