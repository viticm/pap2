<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="UserPassword.aspx.cs" Inherits="Security_UserPassword" Title="<%$ Resources:StringDef,UserPassword %>" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<asp:UpdatePanel runat="server">
<ContentTemplate>
<table width="100%" align="center">
    <tr>
        <td align="center">
            <asp:Label ID="LabelTitle" runat="server" Text="<%$ Resources: StringDef, UserPassword %>" SkinID="Title" />
        </td>
    </tr>
    <tr>
        <td>
            <table width="100%" class="TN" cellspacing="1">
                <tr>
                    <td class="TCH" width="30%">
                        <asp:Literal runat="server" Text="<%$ Resources:StringDef,UserName %>" />
                    </td>
                    <td class="TCN">
                        <asp:TextBox ID="TextBoxInputUserName" runat="server" />
                    </td>
                </tr>
                <tr>
                    <td class="TCH" width="30%">
                        <asp:Literal runat="server" Text="<%$ Resources:StringDef,Password %>" />
                    </td>
                    <td class="TCN">
                        <asp:TextBox ID="TextBoxInputPassword" runat="server" TextMode="Password" />
                    </td>
                </tr>
                <tr>
                    <td class="TCH" width="30%">
                        <asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef,PasswordConfirm %>" />
                    </td>
                    <td class="TCN">
                        <asp:TextBox ID="TextBoxInputPasswordConfirm" runat="server" TextMode="Password" />
                    </td>
                </tr>
            </table>   
        </td>
    </tr> 
    <tr>
        <td align="center">
            <asp:Label ID="LabelOpMsg" runat="server" />
        </td>
    </tr>
    <tr>
		<td align="center">
            <asp:UpdateProgress ID="UpdateProgress1" runat="server">
               <ProgressTemplate>
                   <asp:Literal runat="server" Text="<%$ Resources:StringDef,PleaseWaitWhileOperating %>" />
               </ProgressTemplate>
            </asp:UpdateProgress>
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">
						<asp:LinkButton ID="LinkButtonSetPassword" runat="server" OnClick="LinkButtonSetPassword_Click" Text="<%$ Resources:StringDef,SetPassword %>" />
					</td>
				</tr>
			</table>			
		</td>
	</tr>
</table>
</ContentTemplate>
</asp:UpdatePanel>
</asp:Content>