<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_AddStrategy, App_Web_4eiilbni" title="<%$ Resources:StringDef, AddStrategy %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="AutomationEditor" Src="~/Common/AutomationEditor.ascx" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="100%">
	<tr>
		<td class="TableCaption">
			<asp:Label ID="LabelOpType" runat="server" />
		</td>
	</tr>
	<tr>
		<td>
			<table class="TableMedium" rules="all" align="center">
				<tr>
					<td>
						<asp:Wizard ID="Wizard1" runat="server" ActiveStepIndex="0" OnFinishButtonClick="FinishButton_Click"
							Width="100%" StartNextButtonText="<%$ Resources:StringDef, NextStep %>" StepNextButtonText="<%$ Resources:StringDef, NextStep %>"
							StepPreviousButtonText="<%$ Resources:StringDef, PreviousStep %>" FinishPreviousButtonText="<%$ Resources:StringDef, PreviousStep %>"
							FinishCompleteButtonText="<%$ Resources:StringDef, Finish %>">
							<WizardSteps>
								<asp:WizardStep ID="WizardStep1" runat="server" Title="<%$ Resources:StringDef, Step1 %>">
									<table width="100%">
										<tr class="TableRowSpacing" />
										<tr>
											<td class="TableCaption" colspan="2">
												<asp:Label ID="Label3" runat="server" Text="编辑自动化作业" />
											</td>
										</tr>
										<tr class="TableRowSpacing" />
										<tr>
											<td align="center">
												<Blaze:AutomationEditor ID="AutomationEditorStrategy" runat="server" />
											</td>
										</tr>
										<tr class="TableRowSpacing" />
									</table>
								</asp:WizardStep>
								<asp:WizardStep ID="WizardStep2" runat="server" Title="<%$ Resources:StringDef, Step2 %>">
									<table width="100%">
										<tr class="TableRowSpacing" />
										<tr>
											<td class="TableCaption" colspan="2">
												<asp:Label ID="Label2" runat="server" Text="选择触发事件" />
											</td>
										</tr>
										<tr class="TableRowSpacing" />
										<tr>
											<td align="center">
												<table class="TableNormal" rules="all">
													<tr>
														<td>
															<asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, Event %>" />
														</td>
														<td>
															<asp:DropDownList ID="DropDownListEvent" runat="server" />
														</td>
													</tr>
												</table>
											</td>
										</tr>
										<tr class="TableRowSpacing" />
									</table>
								</asp:WizardStep>								
								<asp:WizardStep ID="WizardStep3" runat="server" Title="<%$ Resources:StringDef, Step3 %>">
									<table width="100%">
										<tr class="TableRowSpacing" />
										<tr>
											<td class="TableCaption" colspan="2">
												<asp:Label ID="Label4" runat="server" Text="编辑策略描述" />
											</td>
										</tr>
										<tr class="TableRowSpacing" />
										<tr>
											<td align="center">
												<table class="TableNormal" rules="all">
													<tr>
														<td>
															<asp:Label runat="server" Text="<%$ Resources:StringDef, Name %>" />
														</td>
														<td>
															<asp:TextBox ID="TextBoxName" runat="server" Width="300" MaxLength="25" />
															<asp:RequiredFieldValidator ID="StrategyNameRequiredValidator" runat="server"
															                            ControlToValidate="TextBoxName"
															                            Display="None"
                                                                                        SetFocusOnError="true"                                                        
                                                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="StrategyNameRequiredValidatorCalloutExtender" TargetControlID="StrategyNameRequiredValidator" />
														</td>
													</tr>
													<tr>
														<td>
															<asp:Label runat="server" Text="<%$ Resources:StringDef, Comment %>" />
														</td>
														<td>
															<asp:TextBox ID="TextBoxComment" runat="server" Width="300" MaxLength="25" />
														</td>
													</tr>
												</table>
											</td>
										</tr>
										<tr class="TableRowSpacing" />
									</table>
								</asp:WizardStep>
							</WizardSteps>
						</asp:Wizard>
					</td>
				</tr>
				<tr>
				<tr>
		<td align="center">
			<asp:Label ID="LabelSuccess" runat="server" SkinID="MessageSuccess" Visible="false" EnableViewState="false" />
			<asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
		</td>
		</tr>
	</tr>
			</table>
            
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">
					    <asp:HyperLink runat="server" NavigateUrl="StrategyList.aspx" Text="<%$ Resources:StringDef, Return %>" />
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

</contenttemplate>
</asp:UpdatePanel>
	
</asp:Content>

