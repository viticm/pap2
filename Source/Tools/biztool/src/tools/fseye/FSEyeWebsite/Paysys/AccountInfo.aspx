<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="AccountInfo.aspx.cs" Inherits="Paysys_AccountInfo" Title="<%$ Resources: StringDef, AccountInfo %>" %>
<%@ Register TagPrefix="Blaze" TagName="DropDownListPaysysZone" Src="~/Common/PaysysZoneList.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel runat="server">
<ContentTemplate>
<table width="100%">
    <tr>
        <td align="center">
            <asp:Label ID="LabelTitle" runat="server" Text="<%$ Resources: StringDef, AccountInfo %>" SkinID="Title" />
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
                        <asp:Literal runat="server" Text="<%$ Resources:StringDef,ZoneName %>" />
                    </td>
                    <td class="TCN">
                        <Blaze:DropDownListPaysysZone ID="PaysysZoneList" runat="server" />
                    </td>
                </tr>
            </table>   
        </td>
    </tr>   
    <tr>
        <td align="center">
            <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
        </td>
    </tr>   
    <tr class="TableRowSpacing" />	
    <tr>
	    <td align="center">
	        <asp:LinkButton ID="LinkButtonQuery" runat="server" OnClick="LinkButtonQuery_Click" Text="<%$ Resources:StringDef,Query %>" />
		</td>
	</tr>	
</table>
</ContentTemplate>
</asp:UpdatePanel>

<table width="100%">
    <tr>
		<td align="center">
            <asp:UpdateProgress ID="UpdateProgress1" runat="server">
               <ProgressTemplate>
                   <asp:Literal runat="server" Text="<%$ Resources:StringDef,PleaseWaitWhileQuerying%>" />
               </ProgressTemplate>
            </asp:UpdateProgress>
        </td>
    </tr>
</table>

<asp:UpdatePanel runat="server" ID="UpdatePanelResult">
<ContentTemplate>
	<asp:Panel ID="PanelResult" runat="server" Width="100%" Visible="false"> 
	<table width="100%" align="center">
	    <tr>
	        <td align="center">
	            <asp:Label ID="LiteralResult" runat="server" SkinID="Title" />
	        </td>
	    </tr>
        <tr>
            <td>                          
                <asp:Table ID="TableResult" runat="server" >
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell Width="30%">
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,Account %>" />
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextAccount" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,ZoneName %>" />
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextZoneName" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,EndDate %>" />
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextEndDate" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow> 
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,ExtPoint %>" />0
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextExtPoint0" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>                     
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,ExtPoint %>"  />1
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextExtPoint1" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>                     
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,ExtPoint %>" />2
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextExtPoint2" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>                     
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,ExtPoint %>"  />3
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextExtPoint3" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>                     
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,ExtPoint %>" />4
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextExtPoint4" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>                     
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,ExtPoint %>" />5
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextExtPoint5" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>                     
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,ExtPoint %>" />6
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextExtPoint6" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>                     
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,ExtPoint %>" />7
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextExtPoint7" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>                     
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,LeftSecond %>" />
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextLeftSecond" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,LeftCoin %>" />
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextLeftCoin" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>   
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,LastLoginTime %>" />
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextLastLoginTime" runat="server" />
                        </asp:TableCell>
                   </asp:TableHeaderRow>
                   <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,LastLogoutTime %>" />
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextLastLogoutTime" runat="server" />
                        </asp:TableCell>
                   </asp:TableHeaderRow>
                   <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,LastLoginIP %>" />
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextLastLoginIP" runat="server" />
                        </asp:TableCell>
                   </asp:TableHeaderRow>
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,ActiveIP %>" />
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextActiveIP" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,ActiveTime %>" />
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextActiveTime" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>   
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,ActiveType %>" />
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextActiveType" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,State %>" />
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextState" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>
                    <asp:TableHeaderRow>
                        <asp:TableHeaderCell>
                            <asp:Literal runat="server" Text="<%$ Resources:StringDef,GatewayInfo %>" />
                        </asp:TableHeaderCell>
                        <asp:TableCell>
                            <asp:Literal ID="TextGatewayInfo" runat="server" />
                        </asp:TableCell>
                    </asp:TableHeaderRow>
                </asp:Table>
                    <table align="center">
                        <tr>
                            <td align="center">
                                <asp:Label ID="LabelOpResult" runat="server" />
                            </td>
                        </tr>
                        <tr>
                            <td align="center">
                                <asp:LinkButton ID="LinkButtonFreeze"   runat="server" OnClick="LinkButtonFreeze_Click" Text="<%$ Resources:StringDef,Freeze %>" CommandName="Freeze" />
                                <asp:LinkButton ID="LinkButtonUnFreeze" runat="server" OnClick="LinkButtonFreeze_Click" Text="<%$ Resources:StringDef,Unfreeze %>" CommandName="UnFreeze" />
                                <asp:HyperLink  ID="HyperLinkSetPassword" runat="server" Text="<%$ Resources:StringDef,SetPassword %>" />
                                <asp:LinkButton ID="LinkButtonSavePassword" runat="server" OnClick="LinkButtonPassword_Click" Text="<%$ Resources:StringDef,SavePassword %>" CommandName="SavePass" />
                                <asp:LinkButton ID="LinkButtonResetPassword" runat="server" OnClick="LinkButtonPassword_Click" Text="<%$ Resources:StringDef,ResetPassword %>" CommandName="ResetPass" />
                            </td>
                        </tr>
                    </table>
            </td>
        </tr>
    </table>
    </asp:Panel>
</ContentTemplate>
</asp:UpdatePanel>

<ajax:UpdatePanelAnimationExtender ID="upae" BehaviorID="animation" runat="server" TargetControlID="UpdatePanelResult">
    <Animations>
        <OnUpdating>
            <Sequence>
                <%-- Disable all the controls --%>
                <Parallel duration="0">
                    <EnableAction AnimationTarget="LinkButtonQuery" Enabled="false" />               
                </Parallel>
                
                <StyleAction Attribute="overflow" Value="hidden" />
                
                <%-- Do each of the selected effects --%>
                <Parallel duration=".3" Fps="30">                    
                    <Condition ConditionScript="true">
                        <Resize Height="0" />
                    </Condition>
                </Parallel>
            </Sequence>
        </OnUpdating>
        <OnUpdated>
            <Sequence>
                <%-- Do each of the selected effects --%>
                <Parallel Duration=".3" Fps="30">
                    <Condition ConditionScript="true">
                        <%-- Get the stored height --%>
                        <Resize AnimationTarget="UpdatePanelResult" HeightScript="500" Unit="px" />
                    </Condition>
                </Parallel>
                <Parallel duration="0">
                    <EnableAction AnimationTarget="LinkButtonQuery" Enabled="true" />              
                </Parallel>
                <StyleAction duration="0" Attribute="overflow" Value="visible" />
            </Sequence>
        </OnUpdated>
    </Animations>
</ajax:UpdatePanelAnimationExtender>

</asp:Content>