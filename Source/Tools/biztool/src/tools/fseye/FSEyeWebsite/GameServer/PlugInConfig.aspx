<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" 
         CodeFile="PlugInConfig.aspx.cs" Inherits="GameServer_PlugInConfig" Title="<%$ Resources:StringDef, PlugInConfig %>" %>
<%@ Register TagPrefix="Blaze" TagName="ServerDropDownList" Src="~/Common/ServerDropDownList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="GameServerCheckBoxList" Src="~/Common/GameServerCheckBoxList.ascx" %>

<asp:Content ID="WholeContent" runat="server" ContentPlaceHolderID="ContentPlaceHolderMain">

    <asp:UpdatePanel id="UpdatePanel1" runat="server">
    <ContentTemplate>
    <table width="100%">
    <tr>
		<td align="center">
			<Blaze:GameServerCheckBoxList ID="GameServerCheckBoxList" runat="server" RepeatColumns="3" GameServerPanelHeight="120" CssClass="TableNormal" />
		</td>
	</tr>	    
    <tr class="TableRowSpacing" />
    <tr>
        <td>
            <table width="100%" border="1">
               <tr>
	            <td class="TableCaption"><asp:Label ID="Label2" runat="server" Text="<%$ Resources:StringDef,PlugInList %>" /></td>
	           </tr>
	           <tr>
	            <td>
	                <asp:Literal ID="serverText" runat="server" />
	            </td>
	           </tr>
               <tr>
	            <td colspan=2 class="TableCellList">
	        	    <table border="1" width="100%" >
	        	    	<tr >
	        		    	<td class="TableCellList">
	        					 <asp:Table ID="TablePlugIns" runat="server"> 
	        					 </asp:Table>
	        			    </td>
	        		    </tr>
	        		</table>
	            </td>
               </tr>
               <tr>
                   <td align="center">
                    <table align="center">
                    <tr><td>
                       <ul>
                        <li><asp:LinkButton ID="RefreshLinkButton" runat="server" Width="50" CssClass="LinkButton" OnClick="Refresh" Text="<%$ Resources:StringDef,Refresh %>" /></li>
                        <li><asp:LinkButton ID="Update" runat="server" Width="60" CssClass="LinkButton" OnClick="UpdatePlugIn_Click" Text="<%$ Resources:StringDef,UpdatePlugIn %>" /></li>
                        <li><asp:LinkButton ID="LinkButtonSelectAllPlugIns" runat="server" Width="50" CssClass="LinkButton" Text="<%$ Resources:StringDef,SelectAll %>" OnClick="ButtonSelectAllPlugIns_Click" /></li>
                        <li><asp:LinkButton ID="LinkButtonSelectNoPlugIns" runat="server" Width="50" CssClass="LinkButton" Text="<%$ Resources:StringDef,SelectNone %>" OnClick="ButtonSelectNoPlugIns_Click" /></li>
                        </ul>
                     </td></tr>
                     </table>
                   </td>
               </tr>
            </table>
        </td>
    </tr>
    </table>
    </ContentTemplate>
    </asp:UpdatePanel>
</asp:Content>