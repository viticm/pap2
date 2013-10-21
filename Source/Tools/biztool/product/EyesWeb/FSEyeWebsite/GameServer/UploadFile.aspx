<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_UploadFile, App_Web_jvyc5ygz" title="<%$ Resources:StringDef, UploadFile %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="GameServerCheckBoxList" Src="~/Common/GameServerCheckBoxList.ascx" %>
<%@ Register TagPrefix="Upload" Namespace="Brettle.Web.NeatUpload" Assembly="Brettle.Web.NeatUpload" %>
<%@ Register TagPrefix="HashedUpload" Namespace="Brettle.Web.NeatUpload" Assembly="Brettle.Web.NeatUpload.HashedInputFile" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">

<script type="text/javascript">
function ShowProgressBar()
{
    document.getElementById("divProgressBar").style.display = "inline";
}
</script>
<table width="100%">
	<tr>
		<td>
			<table width="100%">
				<tr>
					<td align="left" width="30%">
					    <HashedUpload:HashedInputFile ID="UploadFile1" runat="server" />
					    <%--<Upload:InputFile id="UploadFile1" runat="server" />--%>
					    <asp:LinkButton ID="ButtonUpload" runat="server" Text="<%$ Resources:StringDef, UploadToAdminServer %>" OnClick="ButtonUpload_Click" OnClientClick="ShowProgressBar();" />
                    </td>
				</tr>
				<tr>
				    <td>
				        <asp:Literal runat="server" ID="LiteralFileName" />
				    </td>
				</tr>
				<tr>
				    <td>
				        <asp:Literal runat="server" ID="LiteralMD5Hash" />
				    </td>
				</tr>
				<tr>
				    <td width="100%">
				        <div id="divProgressBar" style="display:none;">
                            <Upload:ProgressBar id="inlineProgressBar" runat="server" inline="true" Triggers="ButtonUpload" Width="600px" />
                        </div>
				    </td>
				</tr>
			</table>
		</td>
	</tr>
</table>
<asp:UpdatePanel ID="UpdatePanel1" runat="server">
<Contenttemplate>
<table width="100%">
	<tr>
		<td align="center">
			<asp:Label runat="server" Text="<%$ Resources:StringDef, UploadFileDir %>" SkinID="Title"/>
		</td>
	</tr>
	<tr>
		<td>
			<table border="1" class="TN">					
				<tr>
					<td class="TableCellList">
						<asp:Panel ID="UploadedFilePanel" runat="server" Height="300" ScrollBars="Vertical">
							<asp:Label ID="LabelUploadedNoFile" runat="server" Text="<%$ Resources:StringDef, None %>" />
							<asp:CheckBoxList ID="CheckBoxListUploadedFile" runat="server" RepeatColumns="1" RepeatLayout="Table" />
						</asp:Panel>
					</td>
				</tr>
				<tr>
					<td align="left">
						<asp:Label ID="LabelUploadedFilesInfo" runat="server" />
					</td>
				</tr>
				<tr>
					<td align="center">
					    <asp:LinkButton ID="LinkButtonRefresh" runat="server" Text="<%$ Resources:StringDef,Refresh %>" OnClick="LinkButtonRefresh_Click" />
						<asp:LinkButton ID="LinkButtonSelectAllFiles" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, SelectAll %>" OnClick="ButtonSelectAllFiles_Click" />
						<asp:LinkButton ID="LinkButtonSelectNoFile" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, SelectNone %>" OnClick="ButtonSelectNoFile_Click" />
						<asp:LinkButton ID="LinkButtonDeleteSelectedFiles" runat="server" CssClass="LinkButton" Text="<%$ Resources:StringDef, Delete %>" OnClick="ButtonDeleteSelectedFiles_Click" />
						<asp:LinkButton ID="LinkButtonMD5Files" runat="server" CssClass="LinkButton" Text="MD5" OnClick="ButtonMD5Files_Click" />
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<asp:Label ID="LabelOpMsg" runat="server" />
		</td>
	</tr>
	<tr>
	    <td><div id="DivShowResult"><asp:Table ID="ResultList" runat="server" Visible="false" /></div> </td>
	</tr>	
</table>	
</contenttemplate>
</asp:UpdatePanel>
</asp:Content>
