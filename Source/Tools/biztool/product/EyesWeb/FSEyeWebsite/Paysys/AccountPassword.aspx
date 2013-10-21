<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Paysys_AccountPassword, App_Web_ipk8xfvi" title="<%$ Resources: StringDef, AccountPassword %>" theme="BlazeSky" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel runat="server">
<ContentTemplate>
<table width="100%" align="center">
    <tr>
        <td align="center">
            <asp:Label ID="LabelTitle" runat="server" Text="<%$ Resources: StringDef, AccountPassword %>" SkinID="Title" />
        </td>
    </tr>
    <tr>
        <td>
            <table width="100%" class="TN" cellspacing="1">
                <tr>
                    <td class="TCH" width="30%">
                        <asp:Literal runat="server" Text="<%$ Resources:StringDef,Account %>" />
                    </td>
                    <td class="TCN">
                        <asp:TextBox ID="TextBoxInputAccount" runat="server" />
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
                   <asp:Literal runat="server" Text="<%$ Resources:StringDef,PleaseWaitWhileQuerying%>" />
               </ProgressTemplate>
            </asp:UpdateProgress>
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">						
						<asp:LinkButton ID="LinkButtonSetPassword" runat="server" OnClick="LinkButtonSetPassword_Click" Text="<%$ Resources:StringDef,SetPassword %>" />
						<asp:LinkButton ID="LinkButtonSavePassword" runat="server" OnClick="LinkButtonPassword_Click" Text="<%$ Resources:StringDef,SavePassword %>" CommandName="SavePass" />
                        <asp:LinkButton ID="LinkButtonResetPassword" runat="server" OnClick="LinkButtonPassword_Click" Text="<%$ Resources:StringDef,ResetPassword %>" CommandName="ResetPass" />
					</td>
				</tr>
			</table>			
		</td>
	</tr>
</table>
</ContentTemplate>
</asp:UpdatePanel>
</asp:Content>