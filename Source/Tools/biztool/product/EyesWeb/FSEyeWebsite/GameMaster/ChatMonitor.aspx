<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameMaster_ChatMonitor, App_Web_6lmf-asn" title="<%$ Resources:StringDef,ChatMonitor %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ServerGroupDropDownList" Src="~/Common/ServerGroupDropDownList.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<script type="text/javascript">
var updateRoleItemInfoTipAjax = new sack("../GameMaster/GameMasterHandler.ashx");
var updatingRoleItemInfo = false;
var updateChatMsgServerId=<%=(ServerDropDownList.SelectedGameServer != null ? ServerDropDownList.SelectedServerId : 0) %>
var updateChatMsgAjax = new sack("../GameMaster/GameMasterHandler.ashx");
var updatingChatMsg = false;

var autoScroll = true;
var original = false;
var showTime = true;

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

function updateChatMsg() {    
	if (updateChatMsgServerId)
	{
	    if(!updatingChatMsg)
	    {
	        updatingChatMsg = true;
            updateChatMsgAjax.reset();
	    	updateChatMsgAjax.element = "DivChatContent";
	    	updateChatMsgAjax.runAJAX("op=UpdateChatMsg&serverId="+updateChatMsgServerId+"&original="+original+"&showTime="+showTime);
	    	updateChatMsgAjax.onCompletion = updateChatMsgComplete;
		}
	}
}

function init() {	
	setInterval(updateChatMsg, 3000);
}

function switchAutoScroll() {
    autoScroll = !autoScroll;
}

function switchOriginal()
{
    original = !original;
}

function switchShowTime()
{
    showTime = !showTime;
}

function updateChatMsgComplete()
{
    updatingChatMsg=false;
    scrollOutputToBottom();
}

function scrollOutputToBottom() {
	if (autoScroll)
	{
		var outputDiv = document.getElementById("DivChatContent");
		outputDiv.scrollTop = 100000;
	}
}

function clearMsg()
{
    var outputDiv = document.getElementById("DivChatContent");
	outputDiv.innerHTML = "";
}
</script>

<div id="DivRoleItemInfo" style="display:none;" ></div>
<table width="700px" align="center">
	<tr class="TableRowSpacing" />
	<tr>
		<td class="TableCaption">
			<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef,ChatMonitor %>" />
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>		
		<td align="center">
			<asp:Table ID="Table1" runat="server">
				<asp:TableRow>
					<asp:TableHeaderCell>
						<asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef, GameServer %>" />
					</asp:TableHeaderCell>
					<asp:TableCell>
					    <Blaze:ServerGroupDropDownList runat="server" ID="ServerGroupDropDownList" AutoPostBack="true" OnSelectedServerGroupChanged="SelectedServerGroupChanged" />
						<Blaze:ServerDropDownList runat="server" ID="ServerDropDownList" AutoPostBack="true" />
					</asp:TableCell>
				</asp:TableRow>
			</asp:Table>			
		</td>
	</tr>
    <%-- 
    <tr>
		<td align="center">
            <asp:UpdateProgress ID="UpdateProgress1" runat="server">
               <ProgressTemplate>
                   <asp:Literal runat="server" Text="<%$ Resources:StringDef,PleaseWaitWhileOperating%>" />
               </ProgressTemplate>
            </asp:UpdateProgress>					
		</td>
	</tr>

    <tr>
		<td align="center">
		    <table>
		        <tr>
		            <td>
		                <ul>
		                    <li><asp:LinkButton ID="LinkButtonStartMonitor" runat="server" Text="<%$ Resources:StringDef,StartMonitor %>" CommandName="Start" OnClick="LinkButton_Click" /></li>
		                    <li><asp:LinkButton ID="LinkButtonStopMonitor" runat="server" Text="<%$ Resources:StringDef,StopMonitor %>" CommandName="Stop" OnClick="LinkButton_Click"/></li>		                    
		                    <li><asp:LinkButton ID="LinkButtonChatMsg" runat="server" Text="<%$ Resources:StringDef,ClearChatMsg %>" OnClientClick="clearMsg();" /></li>
		                </ul>            
		            </td>
		        </tr>
		    </table>
		</td>
	</tr>
	--%>
	<tr class="TableRowSpacing" />
    <tr>
		<td align="right">
			<input type="checkbox" checked="checked" onclick="switchShowTime();" />
		    <asp:Literal runat="server" Text="<%$ Resources:StringDef,Time %>" />
		    <input type="checkbox" onclick="switchOriginal();" />
		    <asp:Literal runat="server" Text="<%$ Resources:StringDef,OriginalData %>" />
            <input type="checkbox" checked="checked" onclick="switchAutoScroll();" />
		    <asp:Literal runat="server" Text="<%$ Resources:StringDef,AutoScroll %>" />            
		    <%--
		    <asp:CheckBox ID="CheckBoxOriginalData" runat="server" Text="<%$ Resources:StringDef,OriginalData %>" />
		    <asp:CheckBox ID="CheckBoxAutoScroll" runat="server" Text="<%$ Resources:StringDef,AutoScroll %>" Checked="true" />
		    --%>
		</td>
	</tr>
	<tr>
		<td align="center">
            <div id="DivChatContent" style="width:694px;height:400px;overflow:auto;color:#FFD39C;background-color:#293842;border:3px double #738A8C;padding:5px,12px,5px,5px;text-align:left;" enableviewstate="false" />
        </td>
	</tr>
</table>
<asp:UpdatePanel runat="server" ID="UpdatePanel1" >
    <ContentTemplate>   
        <table width="700px" align="center" border="0" cellpadding="0" cellspacing="0">
            <tr>
                <td colspan="2">
                    <asp:Label runat="server" ID="LabelOpMsg" />
                </td>
            </tr>
            <tr>
                <td width="600px" align="left">
                    <asp:TextBox runat="server" ID="TextBoxMessage" SkinID="Custom" Width="98%" Height="20px" />
                </td>
                <td align="center" width="100px">
                    <%--
                    <asp:Button runat="server" ID="ButtonSendMsg" Text="<%$ Resources:StringDef,Send %>" TabIndex="1" OnClick="LinkButtonSendMsg_Click" />
                    --%>
                    <asp:LinkButton runat="server" ID="LinkButtonSendMsg" Text="<%$ Resources:StringDef,Send %>" OnClick="LinkButtonSendMsg_Click" TabIndex="0" Width="90" />
                </td>
            </tr>
        </table>
    </ContentTemplate>	            
</asp:UpdatePanel>
</asp:Content>