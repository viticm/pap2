<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="ServiceUnavaiable.aspx.cs" Inherits="Common_ServiceUnavaiable" Title="<%$ Resources:StringDef, ServiceUnavaiable %>" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<table width="100%">
<tr>
<td align="center">
<asp:Label ID="LabelServiceUnavaiable" runat="server" Text="<%$ Resources:StringDef, ServiceUnavaiable %>" SkinID="Failure" />
</td>
</tr>
</table>
</asp:Content>

