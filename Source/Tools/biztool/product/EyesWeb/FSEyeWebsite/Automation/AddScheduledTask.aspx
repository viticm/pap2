<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="Automation_AddScheduledTask, App_Web_4eiilbni" title="<%$ Resources:StringDef, EditScheduledTask %>" theme="BlazeSky" %>
<%@ Register TagPrefix="Blaze" TagName="AutomationEditor" Src="~/Common/AutomationEditor.ascx" %>

<asp:Content ID="ContentMain" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">

<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>
<table width="100%">
	<tr>
		<td align="center">
			<asp:Label ID="LabelOpType" runat="server" SkinID="Title" />
		</td>
	</tr>
	<tr>
		<td>
			<table class="TableMedium" rules="all" align="center">
				<tr>
		            <td align="left">
            			<asp:Label ID="OperateResult" runat="server" />
		            </td>
	            </tr>
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
												<asp:Label ID="Label3" runat="server" Text="<%$ Resources:StringDef,AutomationEditor %>" />
											</td>
										</tr>
										<tr class="TableRowSpacing" />
										<tr>
											<td align="center">
												<Blaze:AutomationEditor ID="AutomationEditor1" runat="server" />
											</td>
										</tr>
										<tr class="TableRowSpacing" />
									</table>
								</asp:WizardStep>
								<asp:WizardStep ID="WizardStep2" runat="server" Title="<%$ Resources:StringDef, Step2 %>">
									<table width="100%">
										<tr>
											<td class="TableCaption" colspan="2">
												<asp:Label ID="Label2" runat="server" Text="<%$ Resources:StringDef,ChooseTypeOfTask %>" />
											</td>
										</tr>
										<tr>
											<td>
												<asp:DropDownList ID="DropDownListScheduledTaskType" runat="server" AutoPostBack="true" OnSelectedIndexChanged="ListBoxScheduledTaskType_SelectedIndexChanged" />
											</td>
										</tr>
										<tr>
											<td align="center">
												<asp:Panel ID="PanelOnce" runat="server" Visible="true">
													<table class="TableNormal" rules="all">
														<tr>
															<td>
																<asp:Label ID = "LabelOnceExecuteTime" runat="server" Text="<%$ Resources:StringDef, ExecuteTime %>" />
															</td>
															<td>
																<asp:TextBox ID="TextBoxExecuteTime" runat="server" />
															</td>
														</tr>
													</table>
												</asp:Panel>
												<asp:Panel ID="PanelRepeat" runat="server" Visible="false">
													<table class="TableNormal" rules="all">
														<tr>
															<td>
																<asp:Label ID="LabelRepeatStartTime" runat="server" Text="<%$ Resources:StringDef,StartTime %>" />
															</td>
															<td>
																<asp:TextBox ID="TextBoxStartTime" runat="server" />
															</td>
														</tr>
														<tr>
															<td>
																<asp:Label ID="LabelRepeatEndTime" runat="server" Text="<%$ Resources:StringDef,EndTime %>" />
															</td>
															<td>
																<asp:TextBox ID="TextBoxEndTime" runat="server" />
															</td>
														</tr>
														<tr>
															<td>
																<asp:Label ID="LabelIntervalTime" runat="server" Text="<%$ Resources:StringDef,IntervalTime %>" />
															</td>
															<td>
																<asp:TextBox ID="TextBoxInterval" runat="server" />
															</td>
														</tr>
													</table>
												</asp:Panel>
												<asp:Panel ID="PanelDayOfWeek" runat="server" Visible="false">
													<table class="TableNormal" rules="all">
													    <tr>
													        <td>
													            <asp:Label runat="server" Text="<%$ Resources:StringDef,ExecuteDayOfWeek %>" />
													        </td>
													        <td>
													            <asp:CheckBoxList ID = "CheckBoxListDayOfWeek" runat="server" RepeatColumns="4" RepeatDirection="Horizontal" TextAlign="Right" RepeatLayout="Table" >
													                <asp:ListItem Text="<%$ Resources:StringDef,Sunday %>" Value="0" />
													                <asp:ListItem Text="<%$ Resources:StringDef,Monday %>" Value="1" />
													                <asp:ListItem Text="<%$ Resources:StringDef,Tuesday %>" Value="2" />
													                <asp:ListItem Text="<%$ Resources:StringDef,Wednesday %>" Value="3" />
													                <asp:ListItem Text="<%$ Resources:StringDef,Thursday %>" Value="4" />
													                <asp:ListItem Text="<%$ Resources:StringDef,Friday %>" Value="5" />
													                <asp:ListItem Text="<%$ Resources:StringDef,Saturday %>" Value="6" />
													            </asp:CheckBoxList>
													        </td>
													    </tr>
														<tr>
															<td>
																<asp:Label ID="LabelDayOfWeekExecuteTime" runat="server" Text="<%$ Resources:StringDef, ExecuteTime %>" />
															</td>
															<td>
																<asp:TextBox ID="TextBoxDayOfWeekExecuteTime" runat="server" />
															</td>
														</tr>
													</table>
												</asp:Panel>
											</td>
										</tr>
									</table>
								</asp:WizardStep>
								<asp:WizardStep ID="WizardStep3" runat="server" Title="<%$ Resources:StringDef, Step3 %>" >
									<table width="100%">
										<tr class="TableRowSpacing" />
										<tr>
											<td class="TableCaption" colspan="2">
												<asp:Label ID="Label4" runat="server" Text="<%$ Resources:StringDef,TaskDescription%>" />
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
															<asp:TextBox ID="TextBoxName" runat="server" />
															<asp:RequiredFieldValidator ID="ScheduledTaskNameRequiredValidator" runat="server"
															                            ControlToValidate="TextBoxName"
															                            Display="None"
                                                                                        SetFocusOnError="true"                                                        
                                                                                        ErrorMessage="<%$ Resources:StringDef, RequiredFieldValidatorErrorMsg %>" />
                                                            <ajax:ValidatorCalloutExtender runat="Server" ID="ScheduledTaskNameRequiredValidatorCalloutExtender" TargetControlID="ScheduledTaskNameRequiredValidator" />
														</td>
													</tr>
													<tr>
														<td>
															<asp:Label runat="server" Text="<%$ Resources:StringDef, Comment %>" />
														</td>
														<td>
															<asp:TextBox ID="TextBoxComment" runat="server" />
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
			</table>
		</td>
	</tr>
	<tr class="TableRowSpacing" />
	<tr>
		<td align="center">
			<table cellspacing="0" cellpadding="0">
				<tr>
					<td align="center">
                        <asp:HyperLink runat="server" NavigateUrl="ScheduledTaskList.aspx" Text="<%$ Resources:StringDef, ScheduledTaskList %>" CssClass="LinkButton" />
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
</contenttemplate>
</asp:UpdatePanel>
</asp:Content>

