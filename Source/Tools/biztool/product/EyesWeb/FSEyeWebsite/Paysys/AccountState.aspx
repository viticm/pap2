<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Paysys_AccountState, App_Web_ipk8xfvi" title="<%$ Resources: StringDef, AccountState %>" theme="BlazeSky" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel runat="server">
<ContentTemplate>
<table width="100%" align="center">
    <tr>
        <td align="center">
            <asp:Label ID="LabelTitle" runat="server" Text="<%$ Resources: StringDef, AccountState %>" SkinID="Title" />
        </td>
    </tr>
    <tr>
        <td>
            <table width="100%" class="TN" cellspacing="1">
                <tr>
                    <td class="TCH" width="30%">
                        <asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef,Account %>" />
                    </td>
                    <td class="TCN">
                        <asp:TextBox ID="TextBoxInputAccount" runat="server" />
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
<table width="100%">
	<tr class="TableRowSpacing" />
	<tr>
	    <td align="center">
	        <asp:Label ID="LabelResult" runat="server" SkinID="Title" />
	    </td>
	</tr>
	<tr>
	    <td>
	        <asp:Table ID="TableResult" runat="server" Visible="false" />
	    </td>
	</tr>
</table>
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