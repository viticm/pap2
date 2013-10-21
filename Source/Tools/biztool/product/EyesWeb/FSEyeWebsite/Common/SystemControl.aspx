<%@ page language="C#" autoeventwireup="true" inherits="Common_SystemControl, App_Web_1ceayx6e" title="<%$ Resources:StringDef, SystemControl %>" theme="BlazeSky" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
</head>
<body>
    <form id="form1" runat="server">
    <ajax:ToolkitScriptManager runat="server" EnablePartialRendering="true" ID="ScriptManager1" />    
    <table width=100%>
		<tr class="TableRowSpacing">
		<tr>
			<td align=center>
				<asp:Label runat="server" Text="<%$ Resources:StringDef, SystemControl %>" SkinID="title" />
			</td>
		</tr>
		<tr>
		    <td>
		        <asp:Label runat="server" ID="LabelMessage" />
		    </td>
		</tr>
		<tr class="TableRowSpacing">
		<tr>
			<td align=center>
                <asp:LinkButton ID="ButtonServiceSwitch" runat="server" />
				<asp:HyperLink runat="server" NavigateUrl="Homepage.aspx" Text="<%$ Resources:StringDef, Return %>" />
			</td>
		</tr>
	</table>
    <ajax:ModalPopupExtender runat="server" ID="programmaticModalPopup"
        TargetControlID="ButtonServiceSwitch"
        PopupControlID="programmaticPopup" 
        BackgroundCssClass="modalBackground"
        DropShadow="True"
        CancelControlID="CancelButton">
    </ajax:ModalPopupExtender>
    <asp:Panel runat="server" CssClass="modalPopup" ID="programmaticPopup" style="display:none;width:350px;padding:10px;text-align:center;">
        <div style="background-color:#DDDDDD;border:solid 1px Gray;color:Black;text-align:center;vertical-align:middle;padding:10px;margin:10px;">
            <asp:Literal runat="server" Text="<%$ Resources:StringDef,SystemControl %>" />
        </div>
        <%= GetControlMessage %>
        <br />
        <br />
        <asp:LinkButton ID="OkButton" SkinID="ModalPopup" runat="server" Text="<%$ Resources:StringDef, Ok %>" OnClick="ButtonServiceSwitch_Click" />
        <asp:LinkButton ID="CancelButton" SkinID="ModalPopup" runat="server" Text="<%$ Resources:StringDef, Cancel %>" />
    </asp:Panel>
    </form>
</body>
</html>
