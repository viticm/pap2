<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="RoleSkillInfo.aspx.cs" Inherits="GameMaster_RoleSkillInfo" Title="<%$ Resources:StringDef,SkillInfo %>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<script type="text/javascript">
var updateRoleSkillInfoTipAjax = new sack("../GameMaster/GameMasterHandler.ashx");
var updatingRoleSkillInfo = false;

function init() {
}

function updateRoleSkillInfo(skillTemplateId) {
	if (!updatingRoleSkillInfo)
	{
		updatingRoleSkillInfo = true;				
		updateRoleSkillInfoTipAjax.reset();
		updateRoleSkillInfoTipAjax.element = "DivRoleSkillInfo";
		updateRoleSkillInfoTipAjax.runAJAX("op=GetRoleSkillInfo&skillTemplateId="+skillTemplateId);
		updateRoleSkillInfoTipAjax.onCompletion = new Function("updatingRoleSkillInfo=false;TagToTip('DivRoleSkillInfo',BALLOON, true,BORDERWIDTH, 0,OPACITY,85,BORDERCOLOR ,'#000000'  );");		
	}
}

</script>
<div id="DivRoleSkillInfo" style="display:none;" ></div>

<asp:UpdatePanel ID="UpdatePanel1" runat="server">
<ContentTemplate>
<table width="100%">
	<tr>
		<td class="TableCaption" colspan="3">
			<asp:Label ID="LabelTitle" runat="server" />        
		</td>
    </tr>    
	<tr>
	    <td width="100%" valign="top">
            <div id="DivSkillInfo" runat="server"></div>            
	    </td>   
    </tr>
    <tr>
        <td>
            <asp:Label runat="server" ID="LabelOpMsg" SkinID="Failure" />
        </td>
    </tr>
</table>

</ContentTemplate>
</asp:UpdatePanel>
</asp:Content>