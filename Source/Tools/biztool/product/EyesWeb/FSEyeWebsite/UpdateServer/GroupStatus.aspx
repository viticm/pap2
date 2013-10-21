<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameServer_GroupStatus, App_Web_hm9xpbcd" title="<%$ Resources:StringDef,GroupStatus %>" theme="BlazeSky" %>

<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderTopMessage" runat="Server">
    <div style="width: 13px; height: 10px; background-color: #00FF00; border: solid 1px #FFFFFF;">
    </div>
</asp:Content>
<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">

    <script type="text/javascript">

var updateServerGroupOverviewMonitorAjax = new sack("GroupHandler.ashx");
var updateServerGroupSummaryAjax = new sack("GroupHandler.ashx");
var updateServerGroupSummaryId = 0;
var updatingServerGroupOverviewMonitor = false;
var updatingServerGroupSummary = false;
var selectGroupId = "";
var strOP = "";
var strFilter = "noFilter";
var enableToolTip = 1;  //用来指示是否打开tooltip功能

function init() {        
	updateServerGroupOverviewMonitor();	
	setInterval(updateServerGroupOverviewMonitor, 3000);
    setInterval(updateServerGroupSummary, 1000);
}

		

selectGroupId = getQueryString("groupId");

function updateServerGroupOverviewMonitor() {
	if (!updatingServerGroupOverviewMonitor)
	{
		updatingServerGroupOverviewMonitor = true;
		updateServerGroupOverviewMonitorAjax.reset();
		updateServerGroupOverviewMonitorAjax.element = "DivServerGroupOverviewMonitor";
		updateServerGroupOverviewMonitorAjax.runAJAX("op=GetServerGroupOverviewMonitor" + "_"+selectGroupId+"_"+strFilter);		
		updateServerGroupOverviewMonitorAjax.onCompletion = new Function("updatingServerGroupOverviewMonitor=false;hideTopMessage(5);");
		showTopMessage(10);
	}
}

function setEnableTooltip()
{
    var checkBox = document.getElementById("enableTipCheckBox");        

    checkBox.checked ? enableToolTip = 1 : enableToolTip = 0;
}

function updateServerGroupSummary() {	
    if (!enableToolTip)
    {
        return;
    }

	if (updateServerGroupSummaryId && !updatingServerGroupSummary)
	{
		updatingServerGroupSummary = true;
		updateServerGroupSummaryAjax.reset();
		updateServerGroupSummaryAjax.element = "tooltip";
		updateServerGroupSummaryAjax.runAJAX("op=GetServerGroupSummary&serverGroupId=" + updateServerGroupSummaryId);
		updateServerGroupSummaryAjax.onCompletion = new Function("updatingServerGroupSummary=false");	
	}
}

function setNavigatingServerGroup(serverGroupId) {
	updateServerGroupSummaryId = serverGroupId;
}

//记录用户选择的group
function checkboxClick(groupCheck)
{
    var iPos = selectGroupId.indexOf(groupCheck);
    if (iPos >=0 )
    {
        selectGroupId = selectGroupId.replace(";"+groupCheck + ";","");
    }
    else
    {
        selectGroupId = selectGroupId + ";" +groupCheck + ";";
    }
    document.all['ctl00_ContentPlaceHolderMain_textboxServerGroupId'].value = selectGroupId;
}

//根据鼠标位置设置tooltip的位置，防止遮挡鼠标无法点击超链接
function posTooltip() {
    var curMouseX = window.event.clientX;
    var curMouseY = window.event.clientY;
    
    var curWindowX = document.body.clientWidth;
    var curWindowY = document.body.clientHeight;    
   
    var toolTip = document.getElementById("tooltip");        
    toolTip.style.position = "absolute";
    
    var curTooltipX = toolTip.clientWidth;
    var curTooltipY = toolTip.clientHeight;
    
    if ((curWindowX - curMouseX) < (curTooltipX + 50))
    {
        toolTip.style.left = (curMouseX - curTooltipX -  50) + "px";
    }
    else
    {
        toolTip.style.left = (curMouseX + 25) + "px";
    }
    
    if ((curWindowY - curMouseY) < (curTooltipY + 200))
    {
        toolTip.style.top = (curMouseY - curTooltipY - 50) + "px";
    }
    else
    {
        toolTip.style.top = (curMouseY - 50) + "px";
    }  
}

//鼠标移出时重设tooltip的位置
function restoreTooltip() {
        var toolTip = document.getElementById("tooltip");
        toolTip.style.position = "";
        toolTip.style.left = "";
        toolTip.style.top = "";
}

//javascript读取request.params
function getQueryString( name )
{  
    name = name.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");  
    var regexS = "[\\?&]"+name+"=([^&#]*)";  
    var regex = new RegExp( regexS );  
    var results = regex.exec( window.location.href );  
    if( results == null )    
        return "";  
    else    
        return results[1];
}

//过滤
function filterDropDownListChanged(filterString) {
    strFilter = filterString;
    updateServerGroupOverviewMonitor();
}

    </script>
<table width="100%">
                <tr>
                    <td align="center">
                        <asp:Label ID="LabelGroupStatus" runat="server" Text="<%$ Resources:StringDef,GroupStatus %>"
                            SkinID="Title" /></td>
                </tr>
                <tr class="TableRowSpacing" />
                <tr>
                    <td>
		                    组过滤：<select id="filterDropDownList" onchange="filterDropDownListChanged(this.options[this.selectedIndex].value);" >  
		                    <option value="noFilter">无过滤</option>
		                    <option value="filterOpenGroup">已开放</option>
		                    <option value="filterCloseGroup">未开放</option>
		                    </select>
		                    &nbsp;&nbsp;
		                    <input type="checkbox" id="enableTipCheckBox" checked="checked" onclick="setEnableTooltip();" />启用Tips		          		                		          		                
		            </td>	
                </tr>
 </table>
    <asp:UpdatePanel runat="server" ID="UpdatePanelFileList" UpdateMode="Conditional">
        <ContentTemplate>
            <div id="divFileList">
            <table width="100%">
                <tr>
                    <td>
                        <fieldset>
                            <legend>
                                <asp:CheckBox runat="server" ID="CheckBoxShowFileList" Text="<%$ Resources:StringDef,FileList %>"
                                    Checked="false" OnCheckedChanged="CheckBoxShowFileList_Click" AutoPostBack="true" /></legend>
                            <asp:Panel ID="PanelSelectFiles" runat="server" Visible="false">
                                <table border="1" class="TN">
                                    <tr>
                                        <td class="TableCellList">
                                            <asp:Panel ID="UploadedFilePanel" runat="server" Height="150" ScrollBars="Vertical">
                                                <asp:Label ID="LabelUploadedNoFile" runat="server" Text="<%$ Resources:StringDef,NoUploadFiles %>" />
                                                <asp:RadioButtonList ID="CheckBoxListUploadedFile" runat="server" RepeatColumns="4"
                                                    RepeatLayout="Table">
                                                </asp:RadioButtonList>
                                            </asp:Panel>
                                        </td>
                                    </tr>
                                    <tr class="TableRowNormal">
                                        <td align="left">
                                            <asp:Label ID="LabelUploadedFilesInfo" runat="server" />
                                        </td>
                                    </tr>
                                    <tr>
                                        <td align="center">
                                            <asp:LinkButton ID="LinkButtonSelectAllFiles" runat="server" CssClass="LinkButton"
                                                Text="<%$ Resources:StringDef,SelectAll %>" OnClick="ButtonSelectAllFiles_Click"
                                                Visible="false" />
                                            <asp:LinkButton ID="LinkButtonSelectNoFile" runat="server" CssClass="LinkButton"
                                                Text="<%$ Resources:StringDef,SelectNone %>" OnClick="ButtonSelectNoFile_Click"
                                                Visible="false" />
                                            <asp:CheckBox runat="server" ID="checkboxMD5" Text="<%$ Resources:StringDef,MD5Check %>"
                                                Checked="true" AutoPostBack="false" />
                                        </td>
                                    </tr>
                                </table>
                            </asp:Panel>
                        </fieldset>
                    </td>
                </tr>
            </table>
            </div>
        </ContentTemplate>
    </asp:UpdatePanel>
    <asp:UpdatePanel runat="server" ID="UpdatePanelDivServerGroup" UpdateMode="Conditional">
        <ContentTemplate>
            <table width="100%">
                <tr class="TableRowSpacing" />
                <tr>
                    <td>
                        <div id="DivServerGroupOverviewMonitor" style="width: 100%" class="DivNormal">
                        </div>
                    </td>
                </tr>
            </table>
        </ContentTemplate>
    </asp:UpdatePanel>
    
    <table width="100%">
        <tr class="TableRowSpacing" />
        <tr>
            <td align="left">
                
            </td>
        </tr>
        <tr>
            <td>
                <asp:TextBox ID="textboxServerGroupId" runat="server" Style="border-color: White;
                    width: 0px; height: 0px; background-color: White;"></asp:TextBox>
            </td>
        </tr>
        <tr>
            <td>
                <asp:UpdatePanel runat="server" ID="UpdatePanelDivServerGroupResult" UpdateMode="Conditional">
                    <ContentTemplate>
                        <asp:ListBox ID="ListBoxOperation" runat="server" SelectionMode="Single" Rows="1" />
                <asp:LinkButton ID="LinkButtonExecute" runat="server" Text="<%$ Resources:StringDef,Execute %>"
                    Width="50" OnClick="buttonOperation_Click"></asp:LinkButton>
                        <asp:Timer ID="Timer_GroupStatus" runat="server" OnTick="Timer_GroupStatus_Tick1"
                            Interval="3000" Enabled="false">
                        </asp:Timer>
                        <div id="DivSerialNum">
                            <asp:Label ID="Label1" runat="server" Text="<%$ Resources:StringDef, SerialNum %>"></asp:Label>
                            <asp:Label ID="Label2" runat="server" Text="<%$ Resources:StringDef, Colon %>"></asp:Label>
                            <asp:Label ID="LabelSerialNum" runat="server" Text=""></asp:Label></div>
                        <div id="DivShowTime">
                            <asp:Label ID="lblStartTime" runat="server" Text=""></asp:Label>
                            <br />
                            <asp:Label ID="lblCostTime" runat="server" Text=""></asp:Label></div>
                        <div id="DivShowResult">
                            <asp:Table ID="ResultList" runat="server" />
                        </div>
                        <div>
                            <asp:Label ID="LabelSuccess" runat="server" SkinID="MessageSuccess" />
                            <br />
                            <asp:Label ID="LabelOpMsg" runat="server" SkinID="Failure" />
                        </div>
                    </ContentTemplate>
                </asp:UpdatePanel>
            </td>
        </tr>
        <tr>
            <td>
            </td>
        </tr>
        <tr>
            <td>
            </td>
        </tr>
    </table>
</asp:Content>
