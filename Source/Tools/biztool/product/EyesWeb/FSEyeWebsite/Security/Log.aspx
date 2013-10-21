<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Security_Log, App_Web_jztwgtrl" title="<%$ Resources:StringDef, Log %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>
<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Literal runat="server" Text="<%$ Resources:StringDef, Log %>" />		
		</td>
	</tr>
	<tr>
		<td align="center">
			<table border="0" class="TN" cellspacing="1">
				<tr>
					<td class="TCH"><asp:Literal runat="server" Text="<%$ Resources:StringDef, Operator %>" /></td>
					<td class="TCN">
					    <table width="100%">
					        <tr>
					            <td align="right" width="60%">
					                <asp:TextBox ID="TextBoxOperator" runat="server" />            
					            </td>
					            <td align="left">
                                    <asp:RadioButtonList ID="RadioButtonListOperator" runat="server" RepeatDirection="Horizontal">
					            	    <asp:ListItem Text="<%$ Resources:StringDef,ID %>" Selected="true" />
					            	    <asp:ListItem Text="<%$ Resources:StringDef,Name %>" />
					            	</asp:RadioButtonList>					                
					            </td>
					        </tr>
					    </table>						
					</td>
				</tr>			
				<tr>
					<td class="TCH"><asp:Literal runat="server" Text="<%$ Resources:StringDef, Target %>" /></td>
					<td class="TCN">
					    <table width="100%">
					        <tr>
					            <td width="60%" align="right">
					                <asp:TextBox ID="TextBoxTarget" runat="server" />            
					            </td>
					            <td align="left">
					                <asp:RadioButtonList ID="RadioButtonListTarget" runat="server" RepeatDirection="Horizontal">
					            	    <asp:ListItem Text="<%$ Resources:StringDef,ID %>" Selected="true" />
					            	    <asp:ListItem Text="<%$ Resources:StringDef,Name %>" />
					            	</asp:RadioButtonList>            
					            </td>
					        </tr>
					    </table>
					</td>
				</tr>
				<tr>
					<td class="TCH">
					    <asp:Literal runat="server" Text="<%$ Resources:StringDef, Description %>" />
                    </td>
					<td class="TCN">
					    <table width="100%">
					        <tr>
					            <td width="60%" align="right">
					                <asp:TextBox ID="TextBoxDescription" runat="server" />
					            </td>
					            <td></td>
					        </tr>
					    </table>						
					</td>
				</tr>
                <tr>
					<td class="TCH">
					    <asp:Literal runat="server" Text="<%$ Resources:StringDef, Operation %>" />
                    </td>
					<td class="TCN">
						<asp:CheckBox ID="CheckBoxRead" runat="server" Text="<%$ Resources:StringDef, Read %>" Checked="true" />
						<asp:CheckBox ID="CheckBoxWrite" runat="server" Text="<%$ Resources:StringDef, Write %>" Checked="true" />
						<asp:CheckBox ID="CheckBoxExecute" runat="server" Text="<%$ Resources:StringDef, Execute %>" Checked="true" />
					</td>
				</tr>
				<tr>
					<td class="TCH">
					    <asp:Literal runat="server" Text="<%$ Resources:StringDef, StartTime %>" />
                    </td>
					<td class="TCN">
					    <Blaze:ScriptCalendar runat="server" ID="StartTime" ShowCheckBox="false" />
					</td>
				</tr>				
				<tr>
					<td class="TCH">
					    <asp:Literal runat="server" Text="<%$ Resources:StringDef, EndTime %>" />
                    </td>
					<td class="TCN">
					    <Blaze:ScriptCalendar runat="server" ID="EndTime" ShowCheckBox="false" />						
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr>
	    <td align="center">
	        <asp:Label runat="server" ID="LabelOpMsg" SkinID="Failure" />
	    </td>
	</tr>
	<tr>
		<td align="center">
            <asp:LinkButton ID="ButtonQuery" Text="<%$ Resources:StringDef, Query %>" runat="server" OnClick="ButtonQuery_Click" />
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>

<table width="100%">
	<tr>
		<td align="center">
			<asp:UpdateProgress runat="server" AssociatedUpdatePanelID="UpdatePanel1">
				<ProgressTemplate>
					<asp:Label runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileQuerying %>" />
				</ProgressTemplate>
			</asp:UpdateProgress>
		</td>
	</tr>
</table>

<asp:UpdatePanel runat="server" ID="UpdatePanelResult">
<contenttemplate>
<table width="100%">	
	<asp:Panel runat="server" ID="PanelResult">
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
		<td>
			<asp:Table ID="TableLog" runat="server" Width="100%">
				<asp:TableHeaderRow>
				    <%--
				    <asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, ID %>" /></asp:TableHeaderCell>
					 --%>
					<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Operator %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Operation %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Target %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, Desc %>" /></asp:TableHeaderCell>
					<asp:TableHeaderCell><asp:Literal runat="server" Text="<%$ Resources:StringDef, DateTime %>" /></asp:TableHeaderCell>
				</asp:TableHeaderRow>
			</asp:Table>
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
                    <EnableAction AnimationTarget="ButtonQuery" Enabled="false" />               
                </Parallel>
                
                <StyleAction Attribute="overflow" Value="hidden" />
                
                <%-- Do each of the selected effects --%>
                <Parallel duration=".3" Fps="30">                    
                    <Condition ConditionScript="true" >
                        <Resize Height="0" AnimationTarget="UpdatePanelResult" />
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
                    <EnableAction AnimationTarget="ButtonQuery" Enabled="true" />              
                </Parallel>
                <StyleAction duration="0" Attribute="overflow" Value="visible" />
            </Sequence>
        </OnUpdated>
    </Animations>
</ajax:UpdatePanelAnimationExtender>	
</asp:Content>

