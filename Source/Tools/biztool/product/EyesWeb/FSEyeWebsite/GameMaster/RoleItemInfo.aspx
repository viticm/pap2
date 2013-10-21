<%@ page language="C#" autoeventwireup="true" masterpagefile="~/Common/Main.master" inherits="GameMaster_RoleItemInfo, App_Web_6lmf-asn" theme="BlazeSky" %>

<asp:Content ContentPlaceHolderID="ContentPlaceHolderMain" runat="server" ID="Content1" >
<script type="text/javascript">
var updateRoleItemInfoTipAjax = new sack("../GameMaster/GameMasterHandler.ashx");
var updatingRoleItemInfo = false;

function init() {
}

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
</script>
<div id="DivRoleItemInfo" style="display:none;" ></div>

<asp:UpdatePanel ID="UpdatePanel1" runat="server">
<ContentTemplate>
<table width="100%">
	<tr>
		<td class="TableCaption" colspan="3">
			<asp:Label ID="LabelTitle" runat="server" />        
		</td>
    </tr>    
	<tr>
	    <td width="30%" valign="top">
            <asp:Table ID="Table2" runat="server" >
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell Text="<%$ Resources:StringDef,EquipedItemInfo %>"></asp:TableHeaderCell>
                    </asp:TableHeaderRow>
                    <asp:TableRow>
                        <asp:TableCell><div id="DivRoleState" runat="server"></div></asp:TableCell>
                    </asp:TableRow>
            </asp:Table>	        
	    </td>
	    <td width="30%" valign="top">
            <asp:Table ID="Table1" runat="server" >
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell Text="<%$ Resources:StringDef,BagItemInfo %>"></asp:TableHeaderCell>
                    </asp:TableHeaderRow>
                    <asp:TableRow>
                        <asp:TableCell><div id="DivRoleItem" runat="server"></div></asp:TableCell>
                    </asp:TableRow>
                    <asp:TableRow>
                        <asp:TableCell>
                            <asp:LinkButton ID="LinkButtonBag1" runat="server" OnClick="LinkButtonBag_Click" Text="第一页" SkinID="PlainText" />&nbsp;
                            <asp:LinkButton ID="LinkButtonBag2" runat="server" OnClick="LinkButtonBag_Click" Text="第二页" SkinID="PlainText"/>&nbsp;
                            <asp:LinkButton ID="LinkButtonBag3" runat="server" OnClick="LinkButtonBag_Click" Text="第三页" SkinID="PlainText"/>&nbsp;
                            <asp:LinkButton ID="LinkButtonBag4" runat="server" OnClick="LinkButtonBag_Click" Text="第四页" SkinID="PlainText"/>
                        </asp:TableCell>
                    </asp:TableRow>                            
            </asp:Table>	        
	    </td>
	    <td width="30%" valign="top">
            <asp:Table ID="Table3" runat="server" >
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,StorageItemInfo %>" />
                        </asp:TableHeaderCell>
                    </asp:TableHeaderRow>
                    <asp:TableRow>
                        <asp:TableCell><div id="DivItemStorage" runat="server"></div></asp:TableCell>
                    </asp:TableRow>
                    <asp:TableRow>
                        <asp:TableCell>
                            <asp:LinkButton ID="LinkButtonStorage1" runat="server" OnClick="LinkButtonStorage_Click" Text="第一页" SkinID="PlainText" />&nbsp;
                            <asp:LinkButton ID="LinkButtonStorage2" runat="server" OnClick="LinkButtonStorage_Click" Text="第二页" SkinID="PlainText"/>&nbsp;
                            <asp:LinkButton ID="LinkButtonStorage3" runat="server" OnClick="LinkButtonStorage_Click" Text="第三页" SkinID="PlainText"/>&nbsp;
                            <asp:LinkButton ID="LinkButtonStorage4" runat="server" OnClick="LinkButtonStorage_Click" Text="第四页" SkinID="PlainText"/>
                        </asp:TableCell>
                    </asp:TableRow>
            </asp:Table>
	    </td>
	</tr>
	<tr>
	    <td align="center">
	        <asp:Label runat="server" ID="LabelOpMsg" SkinID="Failure" />
	    </td>
	</tr>
</table>

</ContentTemplate>
</asp:UpdatePanel>

</asp:Content>