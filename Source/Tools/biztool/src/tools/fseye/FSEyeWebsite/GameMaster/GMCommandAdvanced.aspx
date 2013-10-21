<%@ Page Language="C#" MasterPageFile="~/Common/Main.master" AutoEventWireup="true" CodeFile="GMCommandAdvanced.aspx.cs" Inherits="GameMaster_GMCommandAdvanced" Title="高级GM指令" ValidateRequest="false" %>
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

</script>
<asp:UpdatePanel id="UpdatePanel1" runat="server">
<contenttemplate>

<table width="80%" align="center">
    <tr>
        <td>
            <asp:Table ID="Table1" runat="server">
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
                                <Blaze:DropDownListGameCenter ID="serverGroupDropDownList" runat="server" Visible="false" AutoPostBack="true" />
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

<table width="80%" align="center">
    <tr>
		<td align="center">	
			<asp:Panel ID="PanelCustom" runat="server" HorizontalAlign="Center">
			    <asp:Table ID="Table2" runat="server">
			        
			        <asp:TableRow>
			            <asp:TableHeaderCell>
			                <asp:Literal ID="Literal1" runat="server" Text="GM指令执行对象" />
			            </asp:TableHeaderCell>
			            
			            <asp:TableCell>
			                <asp:DropDownList runat="server" ID="GMTargetDropDownList" OnSelectedIndexChanged="GMTargetDropDownList_SelectedIndexChanged" AutoPostBack="true">
			                    <asp:ListItem Text="<%$ Resources:StringDef,ServerTyperGameCenter %>" Value="GameCenter" />			                    
			                    <asp:ListItem Text="玩家" Value="Player" />
			                </asp:DropDownList>			                    
			            </asp:TableCell>
			        </asp:TableRow>
			        
			        <asp:TableRow ID="roleOrAccountTableRow" Visible="false">
			            <asp:TableHeaderCell Width="20%">
			            
			                <asp:DropDownList runat="server" ID="roleOrAccountDropDownList">
			                    <asp:ListItem Text="<%$ Resources:StringDef,RoleName %>" Value="RoleName" />
			                    <asp:ListItem Text="<%$ Resources:StringDef,szAccountName %>" Value="AccountName" />
			                </asp:DropDownList>
			            
			                <%-- <asp:Literal runat="server" Text="<%$ Resources:StringDef,RoleName %>" /> --%>
			                
			            </asp:TableHeaderCell>
			            <asp:TableCell>
			                <asp:TextBox ID="TextBoxCustomRoleName" runat="server" />
			            </asp:TableCell>
			        </asp:TableRow>
			        
			        <asp:TableRow ID="gameServersTableRow" Visible="false">
			            <asp:TableHeaderCell Width="20%">
			                <asp:Literal ID="Literal10" runat="server" Text="目标游戏服务器" />
			            </asp:TableHeaderCell>
			            
			            <asp:TableCell>			            
                            <asp:CheckBoxList runat="server" ID="gameServersCheckBoxList" RepeatLayout="Table">
                            </asp:CheckBoxList>
			            </asp:TableCell>
			            			            			            
			        </asp:TableRow>
			        
                    <asp:TableRow Height="280px">
			            <asp:TableHeaderCell Width="20%">
			                <asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef,GMCommand %>" />
			            </asp:TableHeaderCell>
			            <asp:TableCell>
			                <asp:TextBox ID="TextBoxCustomCommand" runat="server" TextMode="MultiLine" SkinID="Multiline" />
			            </asp:TableCell>
			        </asp:TableRow>
				</asp:Table>
				<br />
				<asp:LinkButton ID="LinkButtonCustom" runat="server" OnClick="LinkButton_Click" OnClientClick="return window.confirm('确认执行GM指令吗?');" CommandName="Custom" Text="<%$ Resources:StringDef,Send %>" />
			</asp:Panel>
	    </td>
	</tr>
	<tr class="TableRowSpace" />
	<tr class="TableRowSpace" />
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