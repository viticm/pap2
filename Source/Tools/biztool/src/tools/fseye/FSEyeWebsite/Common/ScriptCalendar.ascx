<%@ Control Language="C#" AutoEventWireup="true" CodeFile="ScriptCalendar.ascx.cs" Inherits="Common_ScriptCalendar" %>

<table border="0" cellspacing="0" width="100%">
    <tr>
        <td align="right" width="60%">
            <asp:CheckBox ID="CheckBoxSelect" runat="server" AutoPostBack="true" OnCheckedChanged="CheckBoxSelect_CheckedChanged" /><asp:TextBox ID="TextBoxDate" runat="server" Enabled="false" />
        </td>
        <td align="left">
            <asp:ImageButton runat="server" ID="ButtonDate" ImageUrl="~/Image/ShowCalendar.gif" Enabled="false" />
        </td>
    </tr>
</table>
<ajax:CalendarExtender ID="calendarButtonExtender" runat="server" TargetControlID="TextBoxDate" PopupButtonID="ButtonDate" Format="yyyy-MM-dd HH:mm:ss" EnableViewState="false" />

<%--
<img id="ButtonDate" runat="server" src="../Image/ShowCalendar.gif" onclick="return showCalendar('<% =TextBoxClientId %>', '%Y-%m-%d %H:%M', '24', true);" disabled="disabled" style="cursor: hand;" />

<input type="button" id="ButtonDate" runat="server" value="<%$ Resources:StringDef,Select %>" onclick="return showCalendar('<% =TextBoxClientId %>', '%Y-%m-%d %H:%M', '24', true);" disabled="disabled" />
<asp:ImageButton runat="Server" ID="Image1" ImageUrl="~/images/Calendar_scheduleHS.png" AlternateText="Click to show calendar" /><br />        
--%>
