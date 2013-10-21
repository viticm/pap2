/*
*######################################
* eWebEditor v4.00 - Advanced online web based WYSIWYG HTML editor.
* Copyright (c) 2003-2007 eWebSoft.com
*
* For further information go to http://www.ewebsoft.com/
* This copyright notice MUST stay intact for use.
*######################################
*/

var sMenuHr="<tr><td align=center valign=middle height=2><TABLE border=0 cellpadding=0 cellspacing=0 width="+(lang["UIMenuWidth"]-22)+" height=2><tr><td height=1 class=HrShadow><\/td><\/tr><tr><td height=1 class=HrHighLight><\/td><\/tr><\/TABLE><\/td><\/tr>";
var sMenu1="<TABLE border=0 cellpadding=0 cellspacing=0 class=Menu width="+lang["UIMenuWidth"]+"><tr><td width=18 valign=bottom align=center style='background:url(sysimage/contextmenu.gif);background-position:bottom;'><\/td><td width="+(lang["UIMenuWidth"]-18)+" class=RightBg><TABLE border=0 cellpadding=0 cellspacing=0>";
var sMenu2="<\/TABLE><\/td><\/tr><\/TABLE>";
var oPopupMenu=null;
if(BrowserInfo.IsIE55OrMore)
{
	oPopupMenu=window.createPopup();
	
}function getMenuRow(s_Disabled,s_Event,s_Image,s_Html)
{
	var s_MenuRow="";
	s_MenuRow="<tr><td align=center valign=middle><TABLE border=0 cellpadding=0 cellspacing=0 width="+(lang["UIMenuWidth"]-18)+"><tr "+s_Disabled+"><td valign=middle height=20 class=MouseOut onMouseOver=this.className='MouseOver'; onMouseOut=this.className='MouseOut';";
	if(s_Disabled=="")
	{
		s_MenuRow+=" onclick=\"parent."+s_Event+";parent.oPopupMenu.hide();\"";
		
	}s_MenuRow+=">";
	if(s_Image!="")
	{
		s_MenuRow+="&nbsp;<img border=0 src='buttonimage/"+config.ButtonDir+"/"+s_Image+"' width=20 height=20 align=absmiddle "+s_Disabled+">&nbsp;";
		
	}else 
	{
		s_MenuRow+="&nbsp;";
		
	}s_MenuRow+=s_Html+"<\/td><\/tr><\/TABLE><\/td><\/tr>";
	return s_MenuRow;
	
}function getFormatMenuRow(menu,html,image)
{
	var s_Disabled="";
	if(!eWebEditor.document.queryCommandEnabled(menu))
	{
		s_Disabled="disabled";
		
	}var s_Event="format('"+menu+"')";
	var s_Image=menu+".gif";
	if(image)
	{
		s_Image=image;
		
	}return getMenuRow(s_Disabled,s_Event,s_Image,html);
	
}function getTableMenuRow(what)
{
	var s_Menu="";
	var s_Disabled="disabled";
	switch(what)
	{
		case "TableInsert":if(!isTableSelected())s_Disabled="";
		s_Menu+=getMenuRow(s_Disabled,"TableInsert()","tableinsert.gif",lang["TableInsert"]);
		break;
		case "TableProp":if(isTableSelected()||isCursorInTableCell())s_Disabled="";
		s_Menu+=getMenuRow(s_Disabled,"TableProp()","tableprop.gif",lang["TableProp"]);
		break;
		case "TableCell":if(isCursorInTableCell())s_Disabled="";
		s_Menu+=getMenuRow(s_Disabled,"TableCellProp()","tablecellprop.gif",lang["TableCellProp"]);
		s_Menu+=getMenuRow(s_Disabled,"TableCellSplit()","tablecellsplit.gif",lang["TableCellSplit"]);
		s_Menu+=sMenuHr;
		s_Menu+=getMenuRow(s_Disabled,"TableRowProp()","tablerowprop.gif",lang["TableRowProp"]);
		s_Menu+=getMenuRow(s_Disabled,"TableRowInsertAbove()","tablerowinsertabove.gif",lang["TableRowInsertAbove"]);
		s_Menu+=getMenuRow(s_Disabled,"TableRowInsertBelow()","tablerowinsertbelow.gif",lang["TableRowInsertBelow"]);
		s_Menu+=getMenuRow(s_Disabled,"TableRowMerge()","tablerowmerge.gif",lang["TableRowMerge"]);
		s_Menu+=getMenuRow(s_Disabled,"TableRowSplit(2)","tablerowsplit.gif",lang["TableRowSplit"]);
		s_Menu+=getMenuRow(s_Disabled,"TableRowDelete()","tablerowdelete.gif",lang["TableRowDelete"]);
		s_Menu+=sMenuHr;
		s_Menu+=getMenuRow(s_Disabled,"TableColInsertLeft()","tablecolinsertleft.gif",lang["TableColInsertLeft"]);
		s_Menu+=getMenuRow(s_Disabled,"TableColInsertRight()","tablecolinsertright.gif",lang["TableColInsertRight"]);
		s_Menu+=getMenuRow(s_Disabled,"TableColMerge()","tablecolmerge.gif",lang["TableColMerge"]);
		s_Menu+=getMenuRow(s_Disabled,"TableColSplit(2)","tablecolsplit.gif",lang["TableColSplit"]);
		s_Menu+=getMenuRow(s_Disabled,"TableColDelete()","tablecoldelete.gif",lang["TableColDelete"]);
		break;
		
	}return s_Menu;
	
}function showContextMenu(event)
{
	if(!bEditMode)return false;
	var width=lang["UIMenuWidth"];
	var height=0;
	var lefter=event.clientX;
	var topper=event.clientY;
	var oPopDocument=oPopupMenu.document;
	var oPopBody=oPopupMenu.document.body;
	var sMenu="";
	sMenu+=getFormatMenuRow("cut",lang["Cut"]);
	sMenu+=getFormatMenuRow("copy",lang["Copy"]);
	sMenu+=getFormatMenuRow("paste",lang["Paste"]);
	sMenu+=getFormatMenuRow("delete",lang["Delete"]);
	sMenu+=getFormatMenuRow("selectall",lang["SelectAll"]);
	sMenu+=sMenuHr;
	height+=102;
	if(isCursorInTableCell())
	{
		sMenu+=getTableMenuRow("TableProp");
		sMenu+=getTableMenuRow("TableCell");
		sMenu+=sMenuHr;
		height+=286;
		
	}if(isControlSelected("TABLE"))
	{
		sMenu+=getTableMenuRow("TableProp");
		sMenu+=sMenuHr;
		height+=22;
		
	}if(isControlSelected("IMG"))
	{
		sMenu+=getMenuRow("","showDialog('img.htm', true)","img.gif",lang["CMenuImg"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","zIndex('forward')","forward.gif",lang["zIndexForward"]);
		sMenu+=getMenuRow("","zIndex('backward')","backward.gif",lang["zIndexBackward"]);
		sMenu+=sMenuHr;
		height+=64;
		
	}if(tagInSelection("P"))
	{
		sMenu+=getMenuRow("","showDialog('paragraph.htm', true)","paragraph.gif",lang["CMenuParagraph"]);
		sMenu+=sMenuHr;
		height+=22;
		
	}sMenu+=getMenuRow("","findReplace()","findreplace.gif",lang["FindReplace"]);
	height+=20;
	sMenu=sMenu1+sMenu+sMenu2;
	oPopDocument.open();
	oPopDocument.write(config.StyleMenuHeader+sMenu);
	oPopDocument.close();
	height+=2;
	if(lefter+width>document.body.clientWidth)lefter=lefter-width;
	oPopupMenu.show(lefter,topper,width,height,eWebEditor.document.body);
	return false;
	
}function showToolMenu(menu)
{
	if(!bEditMode)return false;
	eWebEditor.focus();
	var sMenu="";
	var width=lang["UIMenuWidth"];
	var height=0;
	var lefter=event.clientX;
	var leftoff=event.offsetX;
	var topper=event.clientY;
	var topoff=event.offsetY;
	var oPopDocument=oPopupMenu.document;
	var oPopBody=oPopupMenu.document.body;
	switch(menu)
	{
		case "font":sMenu+=getFormatMenuRow("bold",lang["Bold"],"bold.gif");
		sMenu+=getFormatMenuRow("italic",lang["Italic"],"italic.gif");
		sMenu+=getFormatMenuRow("underline",lang["UnderLine"],"underline.gif");
		sMenu+=getFormatMenuRow("strikethrough",lang["StrikeThrough"],"strikethrough.gif");
		sMenu+=sMenuHr;
		sMenu+=getFormatMenuRow("superscript",lang["SuperScript"],"superscript.gif");
		sMenu+=getFormatMenuRow("subscript",lang["SubScript"],"subscript.gif");
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","formatText('uppercase')","ucase.gif",lang["UpperCase"]);
		sMenu+=getMenuRow("","formatText('lowercase')","lcase.gif",lang["LowerCase"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","showDialog('selcolor.htm?action=forecolor', true)","forecolor.gif",lang["ForeColor"]);
		sMenu+=getMenuRow("","showDialog('selcolor.htm?action=backcolor', true)","backcolor.gif",lang["BackColor"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","insert('big')","tobig.gif",lang["Big"]);
		sMenu+=getMenuRow("","insert('small')","tosmall.gif",lang["Small"]);
		height=248;
		break;
		case "paragraph":sMenu+=getFormatMenuRow("JustifyLeft",lang["JustifyLeft"],"justifyleft.gif");
		sMenu+=getFormatMenuRow("JustifyCenter",lang["JustifyCenter"],"justifycenter.gif");
		sMenu+=getFormatMenuRow("JustifyRight",lang["JustifyRight"],"justifyright.gif");
		sMenu+=getFormatMenuRow("JustifyFull",lang["JustifyFull"],"justifyfull.gif");
		sMenu+=sMenuHr;
		sMenu+=getFormatMenuRow("insertorderedlist",lang["OrderedList"],"insertorderedlist.gif");
		sMenu+=getFormatMenuRow("insertunorderedlist",lang["UnOrderedList"],"insertunorderedlist.gif");
		sMenu+=getFormatMenuRow("indent",lang["Indent"],"indent.gif");
		sMenu+=getFormatMenuRow("outdent",lang["Outdent"],"outdent.gif");
		sMenu+=sMenuHr;
		sMenu+=getFormatMenuRow("insertparagraph",lang["Paragraph"],"insertparagraph.gif");
		sMenu+=getMenuRow("","insert('br')","br.gif",lang["BR"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow((tagInSelection("P"))?"":"disabled","showDialog('paragraph.htm', true)","paragraph.gif",lang["CMenuParagraph"]);
		height=226;
		break;
		case "edit":var s_Disabled="";
		if(history.data.length<=1||history.position<=0)s_Disabled="disabled";
		sMenu+=getMenuRow(s_Disabled,"goHistory(-1)","undo.gif",lang["UnDo"]);
		if(history.position>=history.data.length-1||history.data.length==0)s_Disabled="disabled";
		sMenu+=getMenuRow(s_Disabled,"goHistory(1)","redo.gif",lang["ReDo"]);
		sMenu+=sMenuHr;
		sMenu+=getFormatMenuRow("Cut",lang["Cut"],"cut.gif");
		sMenu+=getFormatMenuRow("Copy",lang["Copy"],"copy.gif");
		sMenu+=getFormatMenuRow("Paste",lang["Paste"],"paste.gif");
		sMenu+=getMenuRow("","PasteText()","pastetext.gif",lang["PasteText"]);
		sMenu+=getMenuRow("","PasteWord()","pasteword.gif",lang["PasteWord"]);
		sMenu+=sMenuHr;
		sMenu+=getFormatMenuRow("delete",lang["Delete"],"delete.gif");
		sMenu+=getFormatMenuRow("RemoveFormat",lang["RemoveFormat"],"removeformat.gif");
		sMenu+=sMenuHr;
		sMenu+=getFormatMenuRow("SelectAll",lang["SelectAll"],"selectall.gif");
		sMenu+=getFormatMenuRow("Unselect",lang["UnSelect"],"unselect.gif");
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","findReplace()","findreplace.gif",lang["FindReplace"]);
		height=248;
		break;
		case "object":sMenu+=getMenuRow("","showDialog('selcolor.htm?action=bgcolor', true)","bgcolor.gif",lang["BgColor"]);
		sMenu+=getMenuRow("","showDialog('backimage.htm', true)","bgpic.gif",lang["BackImage"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","absolutePosition()","abspos.gif",lang["absolutePosition"]);
		sMenu+=getMenuRow("","zIndex('forward')","forward.gif",lang["zIndexForward"]);
		sMenu+=getMenuRow("","zIndex('backward')","backward.gif",lang["zIndexBackward"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","showBorders()","showborders.gif",lang["ShowBorders"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","insert('quote')","quote.gif",lang["Quote"]);
		sMenu+=getMenuRow("","insert('code')","code.gif",lang["Code"]);
		height=166;
		break;
		case "component":sMenu+=getMenuRow("","showDialog('img.htm', true)","img.gif",lang["Image"]);
		sMenu+=getMenuRow("","showDialog('flash.htm', true)","flash.gif",lang["Flash"]);
		sMenu+=getMenuRow("","showDialog('media.htm', true)","media.gif",lang["Media"]);
		sMenu+=getMenuRow("","showDialog('file.htm', true)","file.gif",lang["File"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","remoteUpload()","remoteupload.gif",lang["RemoteUpload"]);
		sMenu+=getMenuRow("","showDialog('localupload.htm', true)","localupload.gif",lang["LocalUpload"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","showDialog('fieldset.htm', true)","fieldset.gif",lang["Fieldset"]);
		sMenu+=getMenuRow("","showDialog('iframe.htm', true)","iframe.gif",lang["Iframe"]);
		sMenu+=getFormatMenuRow("InsertHorizontalRule",lang["HorizontalRule"],"inserthorizontalrule.gif");
		sMenu+=getMenuRow("","showDialog('marquee.htm', true)","marquee.gif",lang["Marquee"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","createLink()","createlink.gif",lang["CreateLink"]);
		sMenu+=getMenuRow("","showDialog('anchor.htm', true)","anchor.gif",lang["Anchor"]);
		sMenu+=getMenuRow("","mapEdit()","map.gif",lang["Map"]);
		sMenu+=getFormatMenuRow("UnLink",lang["Unlink"],"unlink.gif");
		height=286;
		break;
		case "tool":sMenu+=getMenuRow("","showDialog('symbol.htm', true)","symbol.gif",lang["Symbol"]);
		sMenu+=getMenuRow("","insert('excel')","excel.gif",lang["Excel"]);
		sMenu+=getMenuRow("","showDialog('emot.htm', true)","emot.gif",lang["Emot"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","showDialog('eq.htm', true)","eq.gif",lang["EQ"]);
		sMenu+=getMenuRow("","showDialog('art.htm', true)","art.gif",lang["Art"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","insert('nowdate')","date.gif",lang["NowDate"]);
		sMenu+=getMenuRow("","insert('nowtime')","time.gif",lang["NowTime"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","showDialog('importword.htm', true)","importword.gif",lang["ImportWord"]);
		sMenu+=getMenuRow("","showDialog('importexcel.htm', true)","importexcel.gif",lang["ImportExcel"]);
		height=186;
		break;
		case "file":sMenu+=getMenuRow("","format('Refresh')","refresh.gif",lang["Refresh"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","setMode('CODE')","modecodebtn.gif",lang["ModeCode"]);
		sMenu+=getMenuRow("","setMode('EDIT')","modeeditbtn.gif",lang["ModeEdit"]);
		sMenu+=getMenuRow("","setMode('TEXT')","modetextbtn.gif",lang["ModeText"]);
		sMenu+=getMenuRow("","setMode('VIEW')","modeviewbtn.gif",lang["ModeView"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","sizeChange(300)","sizeplus.gif",lang["SizePlus"]);
		sMenu+=getMenuRow("","sizeChange(-300)","sizeminus.gif",lang["SizeMinus"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","format('Print')","print.gif",lang["Print"]);
		sMenu+=sMenuHr;
		sMenu+=getMenuRow("","showDialog('about.htm')","about.gif",lang["About"]);
		sMenu+=getMenuRow("","window.open('http://www.ewebeditor.net')","site.gif",lang["Site"]);
		height=208;
		break;
		case "table":sMenu+=getTableMenuRow("TableInsert");
		sMenu+=getTableMenuRow("TableProp");
		sMenu+=sMenuHr;
		sMenu+=getTableMenuRow("TableCell");
		height=306;
		break;
		case "form":sMenu+=getFormatMenuRow("InsertInputText",lang["FormText"],"formtext.gif");
		sMenu+=getFormatMenuRow("InsertTextArea",lang["FormTextArea"],"formtextarea.gif");
		sMenu+=getFormatMenuRow("InsertInputRadio",lang["FormRadio"],"formradio.gif");
		sMenu+=getFormatMenuRow("InsertInputCheckbox",lang["FormCheckbox"],"formcheckbox.gif");
		sMenu+=getFormatMenuRow("InsertSelectDropdown",lang["FormDropdown"],"formdropdown.gif");
		sMenu+=getFormatMenuRow("InsertButton",lang["FormButton"],"formbutton.gif");
		height=120;
		break;
		case "gallery":sMenu+=getMenuRow("","showDialog('browse.htm?type=image')","galleryimage.gif",lang["GalleryImage"]);
		sMenu+=getMenuRow("","showDialog('browse.htm?type=flash')","galleryflash.gif",lang["GalleryFlash"]);
		sMenu+=getMenuRow("","showDialog('browse.htm?type=media')","gallerymedia.gif",lang["GalleryMedia"]);
		sMenu+=getMenuRow("","showDialog('browse.htm?type=file')","galleryfile.gif",lang["GalleryFile"]);
		height=80;
		break;
		case "zoom":for(var i=0;i<aZoomSize.length;i++)
		{
			if(aZoomSize[i]==nCurrZoomSize)
			{
				sMenu+=getMenuRow("","doZoom("+aZoomSize[i]+")","checked.gif",aZoomSize[i]+"%");
				
			}else 
			{
				sMenu+=getMenuRow("","doZoom("+aZoomSize[i]+")","space.gif",aZoomSize[i]+"%");
				
			}height+=20;
			
		}break;
		
	}sMenu=sMenu1+sMenu+sMenu2;
	oPopDocument.open();
	oPopDocument.write(config.StyleMenuHeader+sMenu);
	oPopDocument.close();
	height+=2;
	if(lefter+width>document.body.clientWidth)lefter=lefter-width+22;
	oPopupMenu.show(lefter-leftoff-2,topper-topoff+22,width,height,document.body);
	eWebEditor.focus();
	return false;
	
}