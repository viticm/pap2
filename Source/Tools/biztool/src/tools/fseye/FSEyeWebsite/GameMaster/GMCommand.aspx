<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="GMCommand.aspx.cs" Inherits="GameMaster_GMCommand" Title="<%$ Resources:StringDef, GMCommand %>" ValidateRequest="false" %>
<%@ Register TagPrefix="Blaze" TagName="GameServerCheckBoxList" Src="~/Common/GameServerCheckBoxList.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ScriptCalendar" Src="~/Common/ScriptCalendar.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="ItemIdSearch" Src="~/Common/ItemIdSearch.ascx" %>
<%@ Register TagPrefix="Blaze" TagName="DropDownListGameCenter" Src="~/Common/DropDownListGameCenter.ascx" %>


<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="Server">
<script type="text/javascript" >
    
//TreeViewServerGroup在勾选node的checkbox时postback
//    function goPostBack()
//    {
//        var o = window.event.srcElement;
//        if (o.tagName == "INPUT" && o.type == "checkbox")
//        {
//           __doPostBack("","");
//        } 
//    }

   function OnTreeClick(evt)
   {
        var src = window.event != window.undefined ? window.event.srcElement : evt.target;
        var isChkBoxClick = (src.tagName.toLowerCase() == "input" && src.type == "checkbox");
        if(isChkBoxClick)
        {
            var parentTable = GetParentByTagName("table", src);
            var nxtSibling = parentTable.nextSibling;
            if(nxtSibling && nxtSibling.nodeType == 1)//check if nxt sibling is not null & is an element node
            {
                if(nxtSibling.tagName.toLowerCase() == "div") //if node has children
                {
                    //check or uncheck children at all levels
                    CheckUncheckChildren(parentTable.nextSibling, src.checked);
                }
            }
            //check or uncheck parents at all levels
            CheckUncheckParents(src, src.checked);
        }
   } 
   function CheckUncheckChildren(childContainer, check)
   {
      var childChkBoxes = childContainer.getElementsByTagName("input");
      var childChkBoxCount = childChkBoxes.length;
      for(var i = 0; i<childChkBoxCount; i++)
      {
        childChkBoxes[i].checked = check;
      }
   }
   function CheckUncheckParents(srcChild, check)
   {
       var parentDiv = GetParentByTagName("div", srcChild);
       var parentNodeTable = parentDiv.previousSibling;
       
       if(parentNodeTable)
        {
            var checkUncheckSwitch;
            
            if(check) //checkbox checked
            {
                var isAllSiblingsChecked = AreAllSiblingsChecked(srcChild);
                if(isAllSiblingsChecked)
                    checkUncheckSwitch = true;
                else    
                    return; //do not need to check parent if any(one or more) child not checked
            }
            else //checkbox unchecked
            {
                checkUncheckSwitch = false;
            }
            
            var inpElemsInParentTable = parentNodeTable.getElementsByTagName("input");
            if(inpElemsInParentTable.length > 0)
            {
                var parentNodeChkBox = inpElemsInParentTable[0]; 
                parentNodeChkBox.checked = checkUncheckSwitch; 
                //do the same recursively
                CheckUncheckParents(parentNodeChkBox, checkUncheckSwitch);
            }
        }
   }
   function AreAllSiblingsChecked(chkBox)
   {
     var parentDiv = GetParentByTagName("div", chkBox);
     var childCount = parentDiv.childNodes.length;
     for(var i=0; i<childCount; i++)
     {
        if(parentDiv.childNodes[i].nodeType == 1) //check if the child node is an element node
        {
            if(parentDiv.childNodes[i].tagName.toLowerCase() == "table")
            {
               var prevChkBox = parentDiv.childNodes[i].getElementsByTagName("input")[0];
              //if any of sibling nodes are not checked, return false
              if(!prevChkBox.checked) 
              {
                return false;
              } 
            }
        }
     }
     return true;
   }
   //utility function to get the container of an element by tagname
   function GetParentByTagName(parentTagName, childElementObj)
   {
      var parent = childElementObj.parentNode;
      while(parent.tagName.toLowerCase() != parentTagName.toLowerCase())
      {
         parent = parent.parentNode;
      }
    return parent;    
   }
   
   
   //对multiline textbox执行最大输入长度的校验
   function validateMaxLength(control, maxLength)
   {
        if (control.value.length >= maxLength)
        {
            return false;
        }
        
        return true;
   }
   //取消掉刚才的粘贴
   function doBeforePaste(control, maxLength)
   {
        if(maxLength)
        {
          event.returnValue = false;
        }
    }    
    //根据长度截断输入，只粘贴允许的长度
    function doPaste(control, maxLength)
    {
        value = control.value;
        if(maxLength)
        {
            event.returnValue = false;
            maxLength = parseInt(maxLength);
            var oTR = control.document.selection.createRange();
            var iInsertLength = maxLength - value.length + oTR.text.length;
            var sData = window.clipboardData.getData("Text").substr(0,iInsertLength);
            oTR.text = sData;
        }
    }

</script>
<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="80%" align="center">
    <tr>
        <td>
            <asp:Table runat="server">
                <asp:TableRow>
                    <asp:TableHeaderCell Width="20%">
                        <asp:Literal ID="Literal11" runat="server" Text="目标服务器组选择方式" />
                    </asp:TableHeaderCell>
                    <asp:TableCell>
                        <asp:RadioButtonList runat="server" ID="serverGroupSelectionOptionRadioButtonList" OnSelectedIndexChanged="serverGroupSelectionOptionRadioButtonList_OnSelectedIndexChanged" AutoPostBack="true" RepeatDirection="Horizontal" RepeatLayout="Table" RepeatColumns="2">                            
                            <asp:ListItem Text="树形显示" Value="TreeView" Selected="True" />
                            <asp:ListItem Text="下拉列表" Value="DropDownList"/>
                        </asp:RadioButtonList>
                    </asp:TableCell>
                </asp:TableRow>
                
                <asp:TableRow>
                    <asp:TableHeaderCell Width="20%">
                        <asp:Literal ID="Literal7" runat="server" Text="目标服务器组" />
                    </asp:TableHeaderCell>
                    <asp:TableCell>
                    <table>
                        <tr>
                            <td></td>
                            <td align="left">                        
                                <Blaze:DropDownListGameCenter ID="serverGroupDropDownList" runat="server" Visible="false" />
                                <asp:TreeView ID="TreeViewServerGroup" runat="server" Visible="true" ShowCheckBoxes="All" />
                            </td>
                            <td></td>
                        </tr>
                    </table>

                    </asp:TableCell>                    
                </asp:TableRow>                
                
                <asp:TableRow>
                    <asp:TableHeaderCell>
                        <asp:Literal ID="Literal8" runat="server" Text="GM指令类型" />
                    </asp:TableHeaderCell>
                    <asp:TableCell>
                        <asp:ListBox ID="ListBoxOperation" runat="server" SelectionMode="Single" Rows="1" AutoPostBack="true" OnSelectedIndexChanged="ListBoxOperation_IndexChanged" />
                    </asp:TableCell>
                </asp:TableRow>
            </asp:Table>
        </td>
    </tr>
</table>

<br />

<asp:Table runat="server" ID="TemplateTable" Width="80%" HorizontalAlign="Center" BorderStyle="None">    
    <asp:TableRow runat="server">
        <asp:TableCell runat="server" ID="TemplateTableCell" BorderStyle="None" />
    </asp:TableRow>
</asp:Table>

<table width="80%">
    <tr>
	    <td align="center">
	        <asp:Label ID="LabelOpMsg" runat="server" />	        
	    </td>
	</tr>
	<tr>
	    <td align="center">
	        <asp:Label ID="LabelResultReceivedMsg" runat="server" />
		</td>
	</tr>
</table>

<br />

<asp:Panel ID="resultMessagePanel" runat="server" HorizontalAlign="Center">
    <table>
        <tr>
            <td align="left">
                <asp:Literal ID="Literal9" runat="server">GM指令执行结果</asp:Literal>
            </td>
        </tr>
        <tr>
            <td align="left">
                <asp:Timer ID="ShowGMCommandResultTimer" runat="server" OnTick="ShowGMCommandResult" Interval="1000" Enabled="false" />
                <textarea id="resultMessageTextArea" runat="server" rows="18" cols="120" readonly="readonly" />
            </td>
        </tr>
    </table>
</asp:Panel>

</contenttemplate>
</asp:UpdatePanel>
</asp:Content>