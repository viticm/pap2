<%@ control language="C#" autoeventwireup="true" inherits="Common_AutomationEditor, App_Web_1ceayx6e" %>

<table class="TableNormal" rules="all" width="100%">
	<tr>
		<td colspan="2" align="center" class="TableCaption">
			<asp:Label ID="Label4" runat="server" Text="<%$ Resources:StringDef, AutomationEditor %>" />
		</td>		
	</tr>
	<tr>
		<td valign="top">
			<asp:Label ID="Label3" runat="server" Text="<%$ Resources:StringDef, TaskStructure %>" />
		</td>
		<td align="left">
			<asp:TreeView ID="TreeViewAutomation" runat="server" OnSelectedNodeChanged="TreeViewAutomation_SelectedNodeChanged" ImageSet="Arrows"> 
				<ParentNodeStyle Font-Bold="False" />
				<HoverNodeStyle Font-Underline="True" ForeColor="#5555DD" />
				<SelectedNodeStyle Font-Underline="True" ForeColor="#5555DD" HorizontalPadding="0px"
					VerticalPadding="0px" />
				<NodeStyle Font-Names="Verdana" Font-Size="8pt" ForeColor="Black" HorizontalPadding="5px"
					NodeSpacing="0px" VerticalPadding="0px" />
			</asp:TreeView>
		</td>		
	</tr>
	<tr>
		<td>
			<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, AutomationTask %>" />
		</td>
		<td>
			<asp:DropDownList ID="DropDownListAutomation" runat="server" AutoPostBack="true" OnSelectedIndexChanged="DropDownListAutomation_SelectedIndexChanged" /><br />
			<table width="70%" align="center">
			    <tr>
			        <td align="center">
			            <asp:Literal ID="LitearlAutomationDes" runat="server" />
			        </td>
			    </tr>
			</table>
		</td>
	</tr>
	<asp:Panel ID="PanelParameterOut" runat="server" Visible="false" >
	<tr>
		<td>
			<asp:Label ID="Label2" runat="server" Text="<%$ Resources:StringDef, TaskParameter %>" />
		</td>
		<td>
    		<asp:Panel runat="server" ID="PanelParameter" >	    		
	    		<table border="0">
	    		    <tr>
	    		        <td>
	    		            <asp:Label ID="LabelParameter1" runat="server" Visible="false" />            
	    		        </td>
	    		        <td>
	    		            <asp:TextBox ID="TextBoxParameter1" runat="server" Visible="false" />   	    		            
	    		        </td>
	    		        <td>
	    		            <asp:Button ID="ButtonServerIdSelect" runat="server" Visible="false" OnClick="ButtonServerIdSelect_Click" Text="<%$ Resources:StringDef,Select %>" />
	    		        </td>
	    		        <td>
	    		            <asp:Button ID="ButtonFtpServerIdSelect" runat="server" Visible="false" OnClick="ButtonFtpServerIdSelect_Click" Text="<%$ Resources:StringDef,Select %>" />
	    		        </td>
	    		    </tr>
                    <tr>
	    		        <td colspan="3" align="left">
	    		            <asp:Panel ID="PanelServer" runat="server" Visible="false">
	    		                <ContentTemplate>
	    		                    <fieldset>
	    		                        <legend><asp:Literal runat="server" ID="LiteralServerListLegend" Text="<%$ Resources:StringDef,GameServerList %>" /></legend>
	    		                        <asp:CheckBoxList ID="CheckBoxListServer" runat="server" AutoPostBack="true" RepeatLayout="Table" RepeatDirection="Horizontal" OnSelectedIndexChanged="CheckBoxListServer_SelectedIndexChanged" RepeatColumns="4" />
	    		                    </fieldset>
	    		                </ContentTemplate>
	    		            </asp:Panel>
	    		        </td>
	    		     </tr>
	    		     <tr>
	    		     	<td colspan="3" align="left">
	    		            <asp:Panel ID="PanelFtpServer" runat="server" Visible="false">
	    		                <ContentTemplate>
	    		                    <fieldset>
	    		                        <legend><asp:Literal runat="server" ID="Literal2" Text="<%$ Resources:StringDef,FtpServerList %>" /></legend>
	    		                        <asp:CheckBoxList ID="CheckBoxListFtpServer" runat="server" AutoPostBack="true" RepeatLayout="Table" RepeatDirection="Horizontal" OnSelectedIndexChanged="CheckBoxListFtpServer_SelectedIndexChanged" RepeatColumns="4" />
	    		                    </fieldset>
	    		                </ContentTemplate>
	    		            </asp:Panel>
	    		        </td>  
	    		     </tr>
	    		    <tr>
	    		        <td>
	    		            <asp:Label ID="LabelParameter2" runat="server" Visible="false"/>            
	    		        </td>
	    		        <td>
	    		            <asp:TextBox ID="TextBoxParameter2" runat="server" Visible="false"/>            
	    		        </td>
	    		        <td>
	    		            <asp:Button ID="ButtonServerGroupIdSelect" runat="server" Visible="false" OnClick="ButtonServerGroupIdSelect_Click" Text="<%$ Resources:StringDef,Select %>" />
	    		        </td>
	    		    </tr>
                    <tr>
	    		        <td colspan="3" align="left">
	    		            <asp:Panel ID="PanelServerGroup" runat="server" Visible="false">
	    		                <ContentTemplate>
	    		                    <fieldset>
	    		                        <legend><asp:Literal runat="server" ID="Literal1" Text="<%$ Resources:StringDef,GameServerGroupList %>" /></legend>
	    		                        <asp:CheckBoxList ID="CheckBoxListServerGroup" runat="server" AutoPostBack="true" RepeatLayout="Table" RepeatDirection="Horizontal" OnSelectedIndexChanged="CheckBoxListServerGroup_SelectedIndexChanged" RepeatColumns="4" />
	    		                    </fieldset>
	    		                </ContentTemplate>
	    		            </asp:Panel>
	    		        </td>   
	    		     </tr>
	    		    <tr>
	    		        <td>
	    		            <asp:Label ID="LabelParameter3" runat="server" Visible="false"/>            
	    		        </td>
	    		        <td>
	    		            <asp:CheckBoxList ID="MailContentTypeCheckBoxList" runat="server" Visible="false" >
                                <asp:ListItem Text="各区服24小时内在线人数统计(文字)" Value="PlayerCountOneDayStyleText" />
                                <asp:ListItem Text="各区服24小时内在线人数统计(图表)" Value="PlayerCountOneDayStylePic" />
                                <asp:ListItem Text="各区服一月内在线人数统计(文字)" Value="PlayerCountOneMonthStyleText" />	                                 
                                <asp:ListItem Text="各区服一月内在线人数统计(图表)" Value="PlayerCountOneMonthStylePic" />	    		            
                                <asp:ListItem Text="各服务器24小时内事件记录" Value="ServerEventOneDay" />
	    		            </asp:CheckBoxList>
	    		        </td>
	    		        <td>
	    		            <asp:TextBox ID="TextBoxParameter3" runat="server" Visible="false"/>
	    		        </td>
	    		        
	    		</table>
			</asp:Panel>
		</td>
	</tr>
	</asp:Panel>
	<tr>
	    <td colspan="2">
	        <asp:Label ID="LabelParameterInfo" runat="server" Visible="false" SkinID="Failure" />
	    </td>
	</tr>
	<tr>
		<td colspan="2" align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">
                        <asp:LinkButton ID="LinkButtonApply" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Apply %>" OnClick="ButtonApply_Click" />
						<asp:LinkButton ID="LinkButtonRemove" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Remove %>" OnClick="ButtonRemove_Click" />
					</td>
				</tr>
			</table>			
		</td>		
	</tr>
</table>