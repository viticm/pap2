<%@ page language="C#" autoeventwireup="true" masterpagefile="~/Common/Main.master" inherits="GameInfo_Mail, App_Web_d99p0fdh" theme="BlazeSky" %>

<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td align="center" colspan="4">
			<asp:Label ID="literal3" runat="server" Text="<%$ Resources:StringDef, Mail %>" SkinID="Title" />
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
						<asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef,Sender %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<table border="0" cellspacing="0" width="100%">
				            <tr>    
				                <td width="60%" align="right">
				                    <asp:TextBox ID="TextBoxSender" runat="server" />
				                </td>
				                <td align="left">
				                    <asp:CheckBox ID="CheckBoxSender" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" />
				                </td>
				            </tr>
				        </table>
					</asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef,Receiver %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<table border="0" cellspacing="0" width="100%">
				            <tr>    
				                <td width="60%" align="right">
				                    <asp:TextBox ID="TextBoxReceiver" runat="server" />
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
				        <asp:Literal ID="LiteralRemove" runat="server" />
				    </asp:TableHeaderCell>
				    <asp:TableCell>
				        <table border="0" cellspacing="0" width="100%">
				            <tr>    
				                <td width="60%" align="right">
				                    <asp:TextBox ID="TextBoxFilter" runat="server" />            
				                </td>
				                <td align="left">
				                    <asp:CheckBox ID="CheckBoxFilter" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" Checked="true" /><br />        
				                </td>
				            </tr>
				            <tr>
				                <td width="60%" align="right">
				                    <asp:TextBox ID="TextBoxFilter1" runat="server" />    
                                </td>
                                <td align="left">
                                    <asp:CheckBox ID="CheckBoxFilter1" runat="server" Text="<%$ Resources:StringDef, ExactlyMatch %>" Checked="true" />        
                                </td>
				            </tr>
				        </table>			        
				    </asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal ID="Literal30" runat="server" Text="<%$ Resources:StringDef,StartDate %>" />
				    </asp:TableHeaderCell>
                    <asp:TableCell>
	                    <Blaze:ScriptCalendar ID="StartDate" runat="server" ShowCheckBox="false"/>
	                </asp:TableCell>
				</asp:TableRow>
                <asp:TableRow>
				    <asp:TableHeaderCell>
				        <asp:Literal ID="Literal6" runat="server" Text="<%$ Resources:StringDef,EndDate %>" />
				    </asp:TableHeaderCell>
                    <asp:TableCell>
	                    <Blaze:ScriptCalendar ID="EndDate" runat="server" ShowCheckBox="false"/>
	                </asp:TableCell>
				</asp:TableRow>
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal8" runat="server" Text="<%$ Resources:StringDef, OrderBy %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
						<asp:ListBox ID="ListBoxOrderBy" runat="server" Rows="1">
                            <asp:ListItem Text="<%$ Resources:StringDef,Time %>" Value="0" Selected="true"/>
						    <asp:ListItem Text="<%$ Resources:StringDef,Sender %>" Value="1" />
							<asp:ListItem Text="<%$ Resources:StringDef,Receiver %>" Value="2" />
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
	        <asp:Label runat="server" ID="LabelOpMsg" SkinID="Failure" />
	    </td>
	</tr>
	<tr>
		<td align="center">
            <asp:LinkButton ID="LinkButtonStatistic" runat="server" Text="<%$ Resources:StringDef,Statistic %>" OnClick="ButtonStatistic_Click" />
		    <asp:HyperLink ID="HyperLink1" runat="server" NavigateUrl="Statistic.aspx" Text="<%$ Resources:StringDef, Return %>" />			
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

<table width="100%">
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:UpdateProgress ID="UpdateProgress2" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
				<ProgressTemplate>	
				    <asp:Image ID="Image1" runat="server" SkinID="PleaseWait" />
					<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />
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
	    <td align="center">
			<asp:Label ID="LabelResult" runat="server" SkinID="Title" />
	    </td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="right">
			<table cellpadding="0">
				<tr>
					<td>
						<asp:Label ID="Label2" runat="server" Text="<%$ Resources:StringDef, RecordPerPage %>" />
						<asp:ListBox ID="ListBoxRecordPerPage" runat="server" Rows="1">
							<asp:ListItem Text="20" Value="20" />
							<asp:ListItem Text="50" Value="50" Selected="true" />
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
		<asp:Table ID="TableSearchMailList" runat="server">
			<asp:TableHeaderRow>
			    <asp:TableHeaderCell><asp:Literal ID="literal2" runat="server" Text="<%$ Resources:StringDef,Sender %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal ID="literal9" runat="server" Text="<%$ Resources:StringDef,Receiver %>" /></asp:TableHeaderCell>				
				<asp:TableHeaderCell><asp:Literal ID="literal10" runat="server" Text="<%$ Resources:StringDef,SendTime %>" /></asp:TableHeaderCell>
				<asp:TableHeaderCell><asp:Literal ID="literal13" runat="server" Text="<%$ Resources:StringDef,Content %>" /></asp:TableHeaderCell>
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
                    <EnableAction AnimationTarget="LinkButtonStatistic" Enabled="false" />               
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
                    <EnableAction AnimationTarget="LinkButtonStatistic" Enabled="true" />              
                </Parallel>
                <StyleAction duration="0" Attribute="overflow" Value="visible" />
            </Sequence>
        </OnUpdated>
    </Animations>
</ajax:UpdatePanelAnimationExtender>
</asp:Content>