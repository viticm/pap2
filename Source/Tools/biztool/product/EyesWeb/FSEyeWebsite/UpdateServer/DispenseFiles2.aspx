<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="UpdateServer_DispenseFiles2, App_Web_hm9xpbcd" title="Untitled Page" theme="BlazeSky" %>
<asp:Content ID="Content1" ContentPlaceHolderID="ContentPlaceHolderMain" runat="Server">
<script type="text/javascript">
//                        function goPostBack()
//                        {
//                            var o = window.event.srcElement;
//                            if (o.tagName == "INPUT" && o.type == "checkbox")
//                            {
//                               __doPostBack("","");
//                            } 
//                        }
                        
function deleteConfirm(msg)
{
    return window.confirm(msg);
}
function LinkButtonClick(val)
{
    document.all['ctl00_ContentPlaceHolderMain_txtHiddenFolderName'].value = val;
}

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

    <table width="100%">
        <tr class="TableRowSpacing" />                   
         <tr>            
            <td>
                <asp:UpdatePanel ID="UpdatePanelServerGroup" runat="server" UpdateMode="Conditional">
                    <ContentTemplate>
                        <asp:Label ID="LabelOpMsg" runat="server"></asp:Label>
                        <asp:TreeView ID="TreeViewServerGroup" runat="server" ShowCheckBoxes="All">
                        </asp:TreeView>
                    </ContentTemplate>
                </asp:UpdatePanel>
            </td>
        </tr>
        <tr>
            <td>                
                <asp:LinkButton ID="buttonStart" runat="server" Text="<%$ Resources:StringDef,NextStep %>"
                    Width="50" OnClick="buttonStart_Click"></asp:LinkButton>                
            </td>
        </tr>
        
         <tr>
            <td>
                <asp:UpdatePanel runat="server" ID="UpdatePanelFtpFiles" UpdateMode="Conditional">
                    <ContentTemplate>     
                     <asp:Label ID="TimeNow" runat="server" ></asp:Label>
                        <asp:Label ID="ShowCostTime" runat="server" ></asp:Label>
                        <asp:Timer ID="Timer1" runat="server" OnTick="Timer1_Tick1" Interval="1000" Enabled="false"></asp:Timer>               
                        <asp:Table ID="ServerFileList" runat="server" />

                    </ContentTemplate>
                </asp:UpdatePanel>
            </td>
        </tr>
     </table>
</asp:Content>
