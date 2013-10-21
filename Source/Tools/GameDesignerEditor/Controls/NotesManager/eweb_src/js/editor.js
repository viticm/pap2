/*
*######################################
* eWebEditor v4.00 - Advanced online web based WYSIWYG HTML editor.
* Copyright (c) 2003-2007 eWebSoft.com
*
* For further information go to http://www.ewebsoft.com/
* This copyright notice MUST stay intact for use.
*######################################
*/

var sCurrMode = null;var bEditMode = null;var oLinkField = null;var BrowserInfo = new Object() ;BrowserInfo.MajorVer = navigator.appVersion.match(/MSIE (.)/)[1] ;BrowserInfo.MinorVer = navigator.appVersion.match(/MSIE .\.(.)/)[1] ;BrowserInfo.IsIE55OrMore = BrowserInfo.MajorVer >= 6 || ( BrowserInfo.MajorVer >= 5 && BrowserInfo.MinorVer >= 5 ) ;config.IsSP2 = (navigator.userAgent.indexOf("SV1") != -1);config.IsIE7 = (navigator.userAgent.toLowerCase().indexOf("msie 7") != -1);window.onresize = initWidths;var yToolbars = new Array();var bInitialized = false;function document.onreadystatechange(){if (document.readyState!="complete") {return;}if (bInitialized) {return;}bInitialized = true;initHistory();initWidths();oLinkField = parent.document.getElementsByName(sLinkFieldName)[0];if ((!oLinkField)&&(config.ServerExt=="aspx")){oLinkField = getSpecialLinkFieldAspx("input");if (!oLinkField){oLinkField = getSpecialLinkFieldAspx("textarea");}}if (!BrowserInfo.IsIE55OrMore){config.InitMode = "TEXT";}if (ContentFlag.value=="0") { ContentEdit.value = oLinkField.value;ContentLoad.value = oLinkField.value;ModeEdit.value = config.InitMode;ContentFlag.value = "1";}setMode(ModeEdit.value);setLinkedField() ;}function getSpecialLinkFieldAspx(s_Tag){var els = parent.document.getElementsByTagName(s_Tag);for(var i=0;i<els.length;i++){var s_Name = els[i].getAttribute("name");var n = s_Name.lastIndexOf("$");if (n<0){n = s_Name.lastIndexOf(":");}if (n >= 0){s_Name = s_Name.substr(n+1);if (s_Name==sLinkFieldName){return els[i];}}}return null;}function initWidths(){var i, curr;for (i=0; i<document.body.all.length;i++){curr=document.body.all[i];if (curr.className == "yToolbar"){InitTB(curr);yToolbars[yToolbars.length] = curr;}}}function InitBtn(btn) {btn.onmouseover = BtnMouseOver;btn.onmouseout = BtnMouseOut;btn.onmousedown = BtnMouseDown;btn.onmouseup = BtnMouseUp;btn.ondragstart = YCancelEvent;btn.onselectstart = YCancelEvent;btn.onselect = YCancelEvent;btn.YUSERONCLICK = btn.onclick;btn.onclick = YCancelEvent;btn.YINITIALIZED = true;return true;}function InitTB(y) {y.TBWidth = 0;if (! PopulateTB(y)) {return false;}y.style.posWidth = y.TBWidth;return true;}function YCancelEvent() {event.returnValue=false;event.cancelBubble=true;return false;}function BtnMouseOver() {if (event.srcElement.tagName != "IMG") {return false;}var image = event.srcElement;var element = image.parentElement;if (image.className == "Ico") {element.className = "BtnMouseOverUp";}else if (image.className == "IcoDown") {element.className = "BtnMouseOverDown";}event.cancelBubble = true;}function BtnMouseOut() {if (event.srcElement.tagName != "IMG") {event.cancelBubble = true;return false;}var image = event.srcElement;var element = image.parentElement;yRaisedElement = null;element.className = "Btn";image.className = "Ico";event.cancelBubble = true;}function BtnMouseDown() {if (event.srcElement.tagName != "IMG") {event.cancelBubble = true;event.returnValue=false;return false;}var image = event.srcElement;var element = image.parentElement;element.className = "BtnMouseOverDown";image.className = "IcoDown";event.cancelBubble = true;event.returnValue=false;return false;}function BtnMouseUp() {if (event.srcElement.tagName != "IMG") {event.cancelBubble = true;return false;}var image = event.srcElement;var element = image.parentElement;if (element.YUSERONCLICK) {eval(element.YUSERONCLICK + "anonymous()");}element.className = "BtnMouseOverUp";image.className = "Ico";event.cancelBubble = true;return false;}function PopulateTB(y) {var i, elements, element;elements = y.children;for (i=0; i<elements.length; i++) {element = elements[i];if (element.tagName == "SCRIPT" || element.tagName == "!") {continue;}switch (element.className) {case "Btn":if (element.YINITIALIZED == null) {if (! InitBtn(element)) {alert("Problem initializing:" + element.id);return false;}}element.style.posLeft = y.TBWidth;y.TBWidth += element.offsetWidth + 1;break;case "TBGen":element.style.posLeft = y.TBWidth;y.TBWidth += element.offsetWidth + 1;break;case "TBSep":element.style.posLeft = y.TBWidth + 2;y.TBWidth += 5;break;case "TBHandle":element.style.posLeft = 2;y.TBWidth += element.offsetWidth + 7;break;default:alert("Invalid class: " + element.className + " on Element: " + element.id + " <" + element.tagName + ">");return false;}}y.TBWidth += 1;return true;}function setLinkedField() {if (! oLinkField) {return ;}var oForm = oLinkField.form ;if (!oForm) {return ;}oForm.attachEvent("onsubmit", AttachSubmit) ;if (! oForm.submitEditor) oForm.submitEditor = new Array() ;oForm.submitEditor[oForm.submitEditor.length] = AttachSubmit ;if (! oForm.originalSubmit) {oForm.originalSubmit = oForm.submit ;oForm.submit = function() {if (this.submitEditor) {for (var i = 0 ; i < this.submitEditor.length ; i++) {this.submitEditor[i]() ;}}this.originalSubmit() ;}}oForm.attachEvent("onreset", AttachReset) ;if (! oForm.resetEditor) oForm.resetEditor = new Array() ;oForm.resetEditor[oForm.resetEditor.length] = AttachReset ;if (! oForm.originalReset) {oForm.originalReset = oForm.reset ;oForm.reset = function() {if (this.resetEditor) {for (var i = 0 ; i < this.resetEditor.length ; i++) {this.resetEditor[i]() ;}}this.originalReset() ;}}}function AttachSubmit() { var oForm = oLinkField.form ;if (!oForm) {return;}var html = getHTML();ContentEdit.value = html;if (sCurrMode=="TEXT"){html = HTMLEncode(html);}splitTextField(oLinkField, html);} function doSubmit(){var oForm = oLinkField.form ;if (!oForm) {return ;}oForm.submit();}function AttachReset() {if(bEditMode){eWebEditor.document.body.innerHTML = ContentLoad.value;}else{eWebEditor.document.body.innerText = ContentLoad.value;}}function onHelp(){showDialog('about.htm');return false;}function onPaste() {if (sCurrMode=="VIEW") {return false;}if (!history.saved){saveHistory();}if (sCurrMode=="EDIT"){var sHTML = GetClipboardHTML() ;if ((config.AutoDetectPasteFromWord=="1") && BrowserInfo.IsIE55OrMore) {var re = /<\w[^>]* class="?MsoNormal"?/gi ;if ( re.test(sHTML)){if ( confirm( lang["MsgPasteWordConfirm"] ) ){cleanAndPaste( sHTML ) ;saveHistory();return false ;}else{var arr = showModalDialog("dialog/localupload.htm?action=paste", window, "dialogWidth:0px;dialogHeight:0px;help:no;scroll:no;status:no");if (arr){saveHistory();return false;}}}}if(config.BaseHref!=""){eWebEditor.document.selection.createRange().pasteHTML(sHTML) ;return false;}else{return true;}}else{eWebEditor.document.selection.createRange().pasteHTML(HTMLEncode( clipboardData.getData("Text"))) ;saveHistory();return false;}}

function onKeyDown(event)
{
	var n_KeyCode=event.keyCode;
	var s_Key=String.fromCharCode(n_KeyCode).toUpperCase();
	if(n_KeyCode==113)
	{
		showBorders();
		return false;
	}

	if(event.ctrlKey)
	{
		if(n_KeyCode==10)
		{
			doSubmit();
			return false;
			
		}
		if(s_Key=="+")
		{
			sizeChange(300);
			return false;
			
		}
		if(s_Key=="-")
		{
			sizeChange(-300);
			return false;
			
		}
		if(s_Key=="1")
		{
			setMode("CODE");
			return false;
			
		}
		if(s_Key=="2")
		{
			setMode("EDIT");
			return false;
			
		}
		if(s_Key=="3")
		{
			setMode("TEXT");
			return false;
			
		}
		if(s_Key=="4")
		{
			setMode("VIEW");
			return false;
			
		}
	}
	switch(sCurrMode)
	{
		case "VIEW":
			return true;
			break;
		case "EDIT":
			//TAB°´ÏÂ
			if(n_KeyCode==9)
			{
				var sel=eWebEditor.document.selection.createRange();
				sel.pasteHTML("&nbsp;&nbsp;&nbsp;&nbsp;");
				return false;
			}
			if(n_KeyCode==13)
			{
				var sel=eWebEditor.document.selection.createRange();
				//sel.pasteHTML("<div><BR></div>");
				sel.select();
				sel.moveEnd("character",1);
				sel.moveStart("character",1);
				sel.collapse(false);
				//return false;
			}
			if(event.ctrlKey)
			{
				if(s_Key=="D")
				{
					PasteWord();
					return false;
					
				}if(s_Key=="R")
				{
					findReplace();
					return false;
					
				}if(s_Key=="Z")
				{
					goHistory(-1);
					return false;
					
				}if(s_Key=="Y")
				{
					goHistory(1);
					return false;
				}
			}
			break;
		default:
			if(n_KeyCode==13)
			{
				if(!history.saved)
				{
					saveHistory();
					
				}
				history.saved=false;
				var sel=eWebEditor.document.selection.createRange();
				sel.pasteHTML("<BR>");
				event.cancelBubble=true;
				event.returnValue=false;
				sel.select();
				sel.moveEnd("character",1);
				sel.moveStart("character",1);
				sel.collapse(false);
				return false;
			}
			if(event.ctrlKey)
			{
				if((s_Key=="B")||(s_Key=="I")||(s_Key=="U"))
				{
					return false;
					
				}
				if(s_Key=="Z")
				{
					goHistory(-1);
					return false;
					
				}
				if(s_Key=="Y")
				{
					goHistory(1);
					return false;
					
				}
			}
			break;
	}
	if((n_KeyCode==13)||(n_KeyCode==8)||(n_KeyCode==46))
	{
		if(!history.saved)
		{
			saveHistory();
			
		}history.saved=false;
		
	}
	else if((n_KeyCode>=33)&&(n_KeyCode<=40))
	{
		if(!history.saved)
		{
			saveHistory();
			
		}
	}else if(!event.ctrlKey&&s_Key!="A"&&s_Key!="F")
	{
		history.saved=false;
		
	}
	return true;
	
}
var oResizing=new Object;


function onMouseDown(){oResizing.El = null;if (eWebEditor.document.selection.type == "Control") {var oControlRange = eWebEditor.document.selection.createRange();oResizing.El = oControlRange(0);oResizing.W = oResizing.El.style.width;oResizing.H = oResizing.El.style.height;}if (!history.saved){saveHistory();}}function onMouseUp(){if (oResizing.El){if ((oResizing.El.style.width!=oResizing.W)||(oResizing.El.style.height!=oResizing.H)){saveHistory();}}}function doDragEnd(){if (!history.saved){saveHistory();}var oSelection = eWebEditor.document.selection.createRange();var sRangeType = eWebEditor.document.selection.type;if (sRangeType == "Control") {var oControl = oSelection.item(0);if (oControl.tagName == "IMG"){oControl.src = FullPath2SetPath(oControl.src);}}if (sRangeType == "Text") {var els = eWebEditor.document.body.getElementsByTagName("IMG");var oRngTemp = eWebEditor.document.body.createTextRange();for(var i=0;i<els.length;i++){oRngTemp.moveToElementText(els(i));if (oSelection.inRange(oRngTemp)){els(i).src = FullPath2SetPath(els(i).src);}}}saveHistory();return true;}function FullPath2SetPath(url){if (url.indexOf("://")<0){return url;}var s_SitePath = getSitePath();if (url.indexOf(s_SitePath) < 0){return url;}switch(config.BaseUrl){case "0":var s_BaseHref = s_SitePath + config.BaseHref;return url.substr(s_BaseHref.length);break;case "1":return url.substr(s_SitePath.length);break;case "2":return url;break;}}function getSitePath(){var sSitePath = document.location.protocol + "//" + document.location.host;if (sSitePath.substr(sSitePath.length-3) == ":80"){sSitePath = sSitePath.substring(0,sSitePath.length-3);}return sSitePath;}function GetClipboardHTML() {var oDiv = document.getElementById("eWebEditor_Temp_HTML");oDiv.innerHTML = "" ;var oTextRange = document.body.createTextRange() ;oTextRange.moveToElementText(oDiv) ;oTextRange.execCommand("Paste") ;var sData = oDiv.innerHTML ;oDiv.innerHTML = "" ;return sData ;}function cleanAndPaste( html ) {html = html.replace(/<\/?SPAN[^>]*>/gi, "" );html = html.replace(/<(\w[^>]*) class=([^ |>]*)([^>]*)/gi, "<$1$3") ;html = html.replace(/<(\w[^>]*) style="([^"]*)"([^>]*)/gi, "<$1$3") ;html = html.replace(/<(\w[^>]*) lang=([^ |>]*)([^>]*)/gi, "<$1$3") ;html = html.replace(/<\\?\?xml[^>]*>/gi, "") ;html = html.replace(/<\/?\w+:[^>]*>/gi, "") ;html = html.replace(/&nbsp;/, " " );insertHTML( html ) ;}function insertHTML(html) {if (isModeView()) {return false;}eWebEditor.focus();if (eWebEditor.document.selection.type.toLowerCase() != "none"){eWebEditor.document.selection.clear() ;}if (sCurrMode!="EDIT"){html=HTMLEncode(html);}eWebEditor.document.selection.createRange().pasteHTML(html) ; }function setHTML(html, b_NotSaveHistory) {ContentEdit.value = html;switch (sCurrMode){case "CODE":eWebEditor.document.designMode="On";eWebEditor.document.open();eWebEditor.document.write(getStyleEditorHeader());eWebEditor.document.body.innerText=html;eWebEditor.document.body.contentEditable="true";eWebEditor.document.close();bEditMode=false;break;case "EDIT":eWebEditor.document.designMode="On";eWebEditor.document.open();eWebEditor.document.write(getStyleEditorHeader()+html);eWebEditor.document.body.contentEditable="true";eWebEditor.document.execCommand("2D-Position",true,true);eWebEditor.document.execCommand("MultipleSelection", true, true);eWebEditor.document.execCommand("LiveResize", true, true);eWebEditor.document.close();doZoom(nCurrZoomSize);bEditMode=true;break;case "TEXT":eWebEditor.document.designMode="On";eWebEditor.document.open();eWebEditor.document.write(getStyleEditorHeader());eWebEditor.document.body.innerText=html;eWebEditor.document.body.contentEditable="true";eWebEditor.document.close();bEditMode=false;break;case "VIEW":eWebEditor.document.designMode="off";eWebEditor.document.open();eWebEditor.document.write(getStyleEditorHeader()+html);eWebEditor.document.body.contentEditable="false";eWebEditor.document.close();bEditMode=false;break;}eWebEditor.document.body.onpaste = onPaste ;eWebEditor.document.body.onhelp = onHelp ;eWebEditor.document.body.ondragend = new Function("return doDragEnd();");eWebEditor.document.onkeydown = new Function("return onKeyDown(eWebEditor.event);");eWebEditor.document.oncontextmenu=new Function("return showContextMenu(eWebEditor.event);");eWebEditor.document.onmousedown = new Function("return onMouseDown();");eWebEditor.document.onmouseup = new Function("return onMouseUp();");if ((borderShown != "0")&&bEditMode) {borderShown = "0";showBorders();}if (!b_NotSaveHistory){saveHistory();}}function getHTML() {var html;if((sCurrMode=="EDIT")||(sCurrMode=="VIEW")){html = eWebEditor.document.body.innerHTML;}else{html = eWebEditor.document.body.innerText;}if (sCurrMode!="TEXT"){if ((html.toLowerCase()=="<p>&nbsp;</p>")||(html.toLowerCase()=="<p></p>")){html = "";}}return html;}function appendHTML(html) {if(isModeView()){return false;}if(sCurrMode=="EDIT"){eWebEditor.document.body.innerHTML += html;}else{eWebEditor.document.body.innerText += html;}}function PasteWord(){if(!validateMode()){return;}eWebEditor.focus();if (!history.saved){saveHistory();}if(BrowserInfo.IsIE55OrMore){cleanAndPaste( GetClipboardHTML() ) ;saveHistory();}else if(confirm(lang["MsgNotCompatiblePaste"])){format("paste") ;}eWebEditor.focus();}function PasteText(){if(!validateMode()){return;}eWebEditor.focus();if (!history.saved){saveHistory();}var sText = HTMLEncode( clipboardData.getData("Text") ) ;insertHTML(sText);saveHistory();eWebEditor.focus();}function validateMode() {if(sCurrMode=="EDIT"){return true;}alert(lang["MsgOnlyInEditMode"]);eWebEditor.focus();return false;}function isModeView(){if (sCurrMode=="VIEW"){alert(lang["MsgCanotSetInViewMode"]);return true;}return false;}function format(what,opt) {if(!validateMode()){return;}eWebEditor.focus();if (!history.saved){saveHistory();}if(opt=="RemoveFormat"){what=opt;opt=null;}if(opt==null) {var s = "";switch(what.toLowerCase()){case "justifyleft":s = "left";break;case "justifycenter":s = "center";break;case "justifyright":s = "right";break;}var b = false;if (s){var sel = eWebEditor.document.selection.createRange();sel.type = eWebEditor.document.selection.type;if (sel.type=="Control"){var oControl = sel.item(0);try{oControl.align = s;b = true;}catch(e){}}}if (!b){eWebEditor.document.execCommand(what);}}else{eWebEditor.document.execCommand(what,"",opt);}saveHistory();eWebEditor.focus();}function formatText(what){eWebEditor.focus();var s_Type = eWebEditor.document.selection.type.toLowerCase();if (s_Type!="text"){return;}if (!history.saved){saveHistory();}var r1 = eWebEditor.document.selection.createRange().duplicate();var t1 = r1.text.toLowerCase();var l1 = r1.text.replace(/\r\n/g," ").length;var r2 = eWebEditor.document.body.createTextRange();r2.setEndPoint("EndToStart",r1);eWebEditor.document.execCommand("fontname","","eWebEditor_Temp_FontName");var a_Font = eWebEditor.document.body.getElementsByTagName("FONT");var arr = new Array();for (var i=0; i<a_Font.length; i++){var o_Font = a_Font[i];if (o_Font.getAttribute("face") == "eWebEditor_Temp_FontName"){arr[arr.length] = a_Font[i];}}for (var i=0; i<arr.length; i++){var o_Font = arr[i];childNode2Case(o_Font, what);o_Font.removeNode();}r2.setEndPoint("StartToEnd",r2);r2.moveEnd("character",l1);while ((r2.text.toLowerCase()!=t1)&&(r2.text.length<t1.length)){r2.moveEnd("character",1);}r2.select();saveHistory();}function childNode2Case(obj, what){var o_Nodes = obj.childNodes;for (var j=0; j<o_Nodes.length; j++){var o_Node = o_Nodes[j];if (o_Node.nodeType==1){if (o_Node.hasChildNodes()){childNode2Case(o_Node, what);}else{switch(what){case "uppercase":o_Node.innerHTML = o_Node.innerHTML.toUpperCase();break;case "lowercase":o_Node.innerHTML = o_Node.innerHTML.toLowerCase();break;}}}else if (o_Node.nodeType==3){switch(what){case "uppercase":o_Node.nodeValue = o_Node.nodeValue.toUpperCase();break;case "lowercase":o_Node.nodeValue = o_Node.nodeValue.toLowerCase();break;}}}}function formatFont(what, v){eWebEditor.focus();var s_Type = eWebEditor.document.selection.type.toLowerCase();if (s_Type!="text"){return;}if (!history.saved){saveHistory();}var r = eWebEditor.document.selection.createRange();var bk = r.getBookmark();eWebEditor.document.execCommand("fontname","","eWebEditor_Temp_FontName");var a_Font = eWebEditor.document.body.getElementsByTagName("FONT");var arr = new Array();for (var i=0; i<a_Font.length; i++){var o_Font = a_Font[i];if (o_Font.getAttribute("face") == "eWebEditor_Temp_FontName"){arr[arr.length] = a_Font[i];}}for (var i=0; i<arr.length; i++){var o_Font = arr[i];delStyleInFont(o_Font, what);delEmptyNodeInFont(o_Font);setStyleInFont(o_Font, what, v);o_Font.removeAttribute("face");var o_Parent = o_Font.parentElement;if (o_Parent.tagName=="FONT"){fontAttribute2Style(o_Parent);}if ((o_Parent.tagName=="FONT")||(o_Parent.tagName=="SPAN")){if (o_Parent.innerText==o_Font.innerText){o_Parent.style.cssText = o_Parent.style.cssText + ";" + o_Font.style.cssText;o_Parent.innerHTML = o_Font.innerHTML;continue;}}o_Font.outerHTML = "<span style=\""+o_Font.style.cssText+"\">"+o_Font.innerHTML+"</span>";}r.moveToBookmark(bk);r.select();saveHistory();}function setFontStyleValue(obj, what, v){try{switch(what){case "face":obj.style.fontFamily = v;break;case "size":obj.style.fontSize = v;break;case "color":obj.style.color = v;break;default:break;}}catch(e){}}function delStyleInFont(obj, what){setFontStyleValue(obj, what, "");var o_Children = obj.children;for (var j=0; j<o_Children.length; j++){delStyleInFont(o_Children[j], what);if (o_Children[j].tagName=="FONT"){fontAttribute2Style(o_Children[j]);}}}function setStyleInFont(obj, what, v){setFontStyleValue(obj, what, v);var o_Children = obj.children;for (var j=0; j<o_Children.length; j++){if ((o_Children[j].tagName=="SPAN")||(o_Children[j].tagName=="FONT")){setStyleInFont(o_Children[j], what, v);}}}function delEmptyNodeInFont(obj){var o_Children = obj.children;for (var j=0; j<o_Children.length; j++){delEmptyNodeInFont(o_Children[j]);if ((o_Children[j].tagName=="FONT") || (o_Children[j].tagName=="SPAN")){if ((o_Children[j].style.cssText=="")||(o_Children[j].innerHTML=="")){o_Children[j].removeNode(false);delEmptyNodeInFont(obj);return;}}}}function fontAttribute2Style(el){if (el.style.fontFamily==""){var s = el.getAttribute("face");if (s){el.style.fontFamily = s;}}el.removeAttribute("face");if (el.style.fontSize==""){var s = el.getAttribute("size");if (s){switch(s){case "1":s = "8pt";break;case "2":s = "10pt";break;case "3":s = "12pt";break;case "4":s = "14pt";break;case "5":s = "18pt";break;case "6":s = "24pt";break;case "7":s = "36pt";break;default:s = "";break;}}if (s){el.style.fontSize = s;}}el.removeAttribute("size");if (el.style.color==""){var s = el.getAttribute("color");if (s){el.style.color = s;}}el.removeAttribute("color");}function VerifyFocus() {if (eWebEditor){eWebEditor.focus();}}function setMode(NewMode){if (NewMode==sCurrMode){return;}if (!BrowserInfo.IsIE55OrMore){if ((NewMode=="CODE") || (NewMode=="EDIT") || (NewMode=="VIEW")){alert(lang["MsgNotCompatibleHtml"]);return false;}}if (NewMode=="TEXT"){if (sCurrMode==ModeEdit.value){if (!confirm(lang["MsgHtmlToText"])){return false;}}}var sBody = "";switch(sCurrMode){case "CODE":if (NewMode=="TEXT"){eWebEditor_Temp_HTML.innerHTML = eWebEditor.document.body.innerText;sBody = eWebEditor_Temp_HTML.innerText;}else{sBody = eWebEditor.document.body.innerText;}break;case "TEXT":sBody = eWebEditor.document.body.innerText;sBody = HTMLEncode(sBody);break;case "EDIT":case "VIEW":if (NewMode=="TEXT"){sBody = eWebEditor.document.body.innerText;}else{sBody = eWebEditor.document.body.innerHTML;}break;default:sBody = ContentEdit.value;break;}try{document.all["eWebEditor_CODE"].className = "StatusBarBtnOff";document.all["eWebEditor_EDIT"].className = "StatusBarBtnOff";document.all["eWebEditor_TEXT"].className = "StatusBarBtnOff";document.all["eWebEditor_VIEW"].className = "StatusBarBtnOff";document.all["eWebEditor_"+NewMode].className = "StatusBarBtnOn";}catch(e){}sCurrMode = NewMode;ModeEdit.value = NewMode;setHTML(sBody);disableChildren(eWebEditor_Toolbar);}function disableChildren(obj){if (obj){obj.disabled=(!bEditMode);for (var i=0; i<obj.children.length; i++){disableChildren(obj.children[i]);}}}function showDialog(url, optValidate){var sName;var nIndex = url.indexOf(".");if (nIndex<0){sName = url;url = url + ".htm";}else{sName = url.substring(0, nIndex);}url = "dialog/" + url;sName = sName.toLowerCase();url = url.toLowerCase();if (optValidate) {if (!validateMode()) {return;}}eWebEditor.focus();if (!history.saved){saveHistory();}var arr = showModalDialog(url, window, "dialogWidth:0px;dialogHeight:0px;help:no;scroll:no;status:no");saveHistory();eWebEditor.focus();}function Maximize() {if (!validateMode()) {return;}saveHistory();window.open("dialog/fullscreen.htm?style="+config.StyleName, 'FullScreen'+sLinkFieldName, 'toolbar=no,location=no,directories=no,status=yes,menubar=no,scrollbars=yes,resizable=yes,fullscreen=yes');}function createLink(){if (!validateMode()) {return;}if (eWebEditor.document.selection.type == "Control") {var oControlRange = eWebEditor.document.selection.createRange();if (oControlRange(0).tagName.toUpperCase() != "IMG") {alert(lang["MsgHylnkLimit"]);return;}}showDialog("hyperlink.htm", true);}function HTMLEncode(text){if (text==null){return "";}text = text.replace(/&/g, "&amp;") ;text = text.replace(/"/g, "&quot;") ;text = text.replace(/</g, "&lt;") ;text = text.replace(/>/g, "&gt;") ;text = text.replace(/\ /g,"&nbsp;");text = text.replace(/\n/g,"<br>");text = text.replace(/\t/g,"&nbsp;&nbsp;&nbsp;&nbsp;");return text;}function insert(what) {if (!validateMode()) {return;}eWebEditor.focus();saveHistory();var sel = eWebEditor.document.selection.createRange();switch(what){case "nowdate":var d = new Date();insertHTML(d.toLocaleDateString());break;case "nowtime":var d = new Date();insertHTML(d.toLocaleTimeString());break;case "br":insertHTML("<br>");break;case "code":insertHTML('<table width=95% border="0" align="Center" cellpadding="6" cellspacing="0" style="border: 1px Dotted #CCCCCC; TABLE-LAYOUT: fixed"><tr><td bgcolor=#FDFDDF style="WORD-WRAP: break-word"><font style="color: #990000;font-weight:bold">'+lang["HtmlCode"]+'</font><br>'+HTMLEncode(sel.text)+'</td></tr></table>');break;case "quote":insertHTML('<table width=95% border="0" align="Center" cellpadding="6" cellspacing="0" style="border: 1px Dotted #CCCCCC; TABLE-LAYOUT: fixed"><tr><td bgcolor=#F3F3F3 style="WORD-WRAP: break-word"><font style="color: #990000;font-weight:bold">'+lang["HtmlQuote"]+'</font><br>'+HTMLEncode(sel.text)+'</td></tr></table>');break;case "big":insertHTML("<big>" + sel.text + "</big>");break;case "small":insertHTML("<small>" + sel.text + "</small>");break;case "printbreak":insertHTML("<div style=\"FONT-SIZE: 1px; PAGE-BREAK-BEFORE: always; VERTICAL-ALIGN: middle; HEIGHT: 1px; BACKGROUND-COLOR: #c0c0c0\">&nbsp; </div>");break;default:alert(lang["ErrParam"]);break;}sel=null;}var borderShown = config.ShowBorder;function showBorders() {if (!validateMode()) {return;}var allForms = eWebEditor.document.body.getElementsByTagName("FORM");var allInputs = eWebEditor.document.body.getElementsByTagName("INPUT");var allTables = eWebEditor.document.body.getElementsByTagName("TABLE");var allLinks = eWebEditor.document.body.getElementsByTagName("A");for (a=0; a < allForms.length; a++) {if (borderShown == "0") {allForms[a].runtimeStyle.border = "1px dotted #FF0000";} else {allForms[a].runtimeStyle.cssText = "";}}for (b=0; b < allInputs.length; b++) {if (borderShown == "0") {if (allInputs[b].type.toUpperCase() == "HIDDEN") {allInputs[b].runtimeStyle.border = "1px dashed #000000";allInputs[b].runtimeStyle.width = "15px";allInputs[b].runtimeStyle.height = "15px";allInputs[b].runtimeStyle.backgroundColor = "#FDADAD";allInputs[b].runtimeStyle.color = "#FDADAD";}} else {if (allInputs[b].type.toUpperCase() == "HIDDEN"){allInputs[b].runtimeStyle.cssText = "";}}}for (i=0; i < allTables.length; i++) {if (borderShown == "0") {allTables[i].runtimeStyle.border = "1px dotted #BFBFBF";} else {allTables[i].runtimeStyle.cssText = "";}allRows = allTables[i].rows;for (y=0; y < allRows.length; y++) { 	allCellsInRow = allRows[y].cells;for (x=0; x < allCellsInRow.length; x++) {if (borderShown == "0") {allCellsInRow[x].runtimeStyle.border = "1px dotted #BFBFBF";} else {allCellsInRow[x].runtimeStyle.cssText = "";}}}}for (a=0; a < allLinks.length; a++) {if (borderShown == "0") {if (allLinks[a].href.toUpperCase() == "") {allLinks[a].runtimeStyle.borderBottom = "1px dashed #000000";}} else {allLinks[a].runtimeStyle.cssText = "";}}if (borderShown == "0") {borderShown = "1";} else {borderShown = "0";}scrollUp();}function scrollUp() {eWebEditor.scrollBy(0,0);}var nCurrZoomSize = 100;var aZoomSize = new Array(10, 25, 50, 75, 100, 150, 200, 500);function doZoom(size) {eWebEditor.document.body.runtimeStyle.zoom = size + "%";nCurrZoomSize = size;}function findReplace(){showDialog('findreplace.htm', true);}function absolutePosition(){var objReference = null;var RangeType = eWebEditor.document.selection.type;if (RangeType != "Control") {return;}var selectedRange = eWebEditor.document.selection.createRange();for (var i=0; i<selectedRange.length; i++){objReference = selectedRange.item(i);if (objReference.style.position != 'relative') {objReference.style.position='relative';}else{objReference.style.position='static';}}}function zIndex(action){var objReference = null;var RangeType = eWebEditor.document.selection.type;if (RangeType != "Control") {return;}var selectedRange = eWebEditor.document.selection.createRange();for (var i=0; i<selectedRange.length; i++){objReference = selectedRange.item(i);if (action=='forward'){objReference.style.zIndex  +=1;}else{objReference.style.zIndex  -=1;}objReference.style.position='relative';}}function isControlSelected(tag){if (tag){if (eWebEditor.document.selection.type == "Control") {var oControlRange = eWebEditor.document.selection.createRange();if (oControlRange(0).tagName.toUpperCase() == tag) {return true;}}}return false;}function findParentElement(tag) {var el = null;if (eWebEditor.document.selection.type != "Control") {el = eWebEditor.document.selection.createRange().parentElement();while (el.tagName.toUpperCase() != tag){el = el.parentElement;if (el == null){                break;}}}return el;}function sizeChange(size){if (!BrowserInfo.IsIE55OrMore){alert(lang["MsgNotCompatibleFunc"]);return false;}for (var i=0; i<parent.frames.length; i++){if (parent.frames[i].document==self.document){var obj=parent.frames[i].frameElement;var height = parseInt(obj.offsetHeight);if (height+size>=300){obj.height=height+size;}break;}}}function mapEdit(){if (!validateMode()) {return;}saveHistory();var b = false;if (eWebEditor.document.selection.type == "Control") {var oControlRange = eWebEditor.document.selection.createRange();if (oControlRange(0).tagName.toUpperCase() == "IMG") {b = true;}}if (!b){alert(lang["MsgMapLimit"]);return;}window.open("dialog/map.htm", "mapEdit"+sLinkFieldName, "toolbar=no,location=no,directories=no,status=not,menubar=no,scrollbars=no,resizable=yes,width=450,height=300");}function paragraphAttr(){if (!validateMode()) {return;}eWebEditor.focus();if (!tagInSelection("P")){alert(lang["MsgNotParagraph"]);return;}showDialog('paragraph.htm', true);}function tagInSelection(tag){var sel = eWebEditor.document.selection.createRange();sel.type = eWebEditor.document.selection.type;if (sel.type!="Control"){var oBody = eWebEditor.document.body;var aAllEl = oBody.getElementsByTagName(tag);var aSelEl = new Array();var oRngTemp = oBody.createTextRange();for(var i=0;i<aAllEl.length;i++){oRngTemp.moveToElementText(aAllEl(i));if (sel.inRange(oRngTemp)){aSelEl[aSelEl.length] = aAllEl[i];}else{if ( ((sel.compareEndPoints("StartToEnd",oRngTemp)<0)&&(sel.compareEndPoints("StartToStart",oRngTemp)>0)) || ((sel.compareEndPoints("EndToStart",oRngTemp)>0)&&(sel.compareEndPoints("EndToEnd",oRngTemp)<0)) ){aSelEl[aSelEl.length] = aAllEl[i];}}}if (aSelEl.length>0){return true;}}return false;}function addUploadFile(originalFileName, saveFileName, savePathFileName){doInterfaceUpload(sLinkOriginalFileName, originalFileName);doInterfaceUpload(sLinkSaveFileName, saveFileName);doInterfaceUpload(sLinkSavePathFileName, savePathFileName);}function doInterfaceUpload(strLinkName, strValue){if (strValue=="") {return;}if (strLinkName){var objLinkUpload = parent.document.getElementsByName(strLinkName)[0];if (objLinkUpload){if (objLinkUpload.value!=""){objLinkUpload.value = objLinkUpload.value + "|";}objLinkUpload.value = objLinkUpload.value + strValue;objLinkUpload.fireEvent("onchange");}}}function splitTextField(objField, html) { objField.value = html;if (config.ServerExt != "asp") {return;}var strFieldName = objField.name;var objForm = objField.form;var objDocument = objField.document;var FormLimit = 50000 ;for (var i=1;i<objDocument.getElementsByName(strFieldName).length;i++) {objDocument.getElementsByName(strFieldName)[i].value = "";}if (html.length > FormLimit) { objField.value = html.substr(0, FormLimit) ;html = html.substr(FormLimit) ;while (html.length > 0) { var objTEXTAREA = objDocument.createElement("<TEXTAREA NAME='"+strFieldName+"'></TEXTAREA>") ;objTEXTAREA.style.display = "none" ;objTEXTAREA.value = html.substr(0, FormLimit) ;objForm.appendChild(objTEXTAREA) ;html = html.substr(FormLimit) ;} }} var sEventUploadAfter;function remoteUpload(strEventUploadAfter) { if (config.AutoRemote!="1") {return;}if (sCurrMode=="TEXT") {return;}sEventUploadAfter = strEventUploadAfter;var objField = document.getElementsByName("eWebEditor_UploadText")[0];splitTextField(objField, getHTML());divProcessing.style.top = (document.body.clientHeight-parseFloat(divProcessing.style.height))/2;divProcessing.style.left = (document.body.clientWidth-parseFloat(divProcessing.style.width))/2;divProcessing.style.display = "";eWebEditor_UploadForm.submit();} function remoteUploadOK() {divProcessing.style.display = "none";if (oLinkField){if (sEventUploadAfter){eval("parent."+sEventUploadAfter);}}}var history = new Object;history.data = [];history.position = 0;history.bookmark = [];history.saved = false;function saveHistory() {history.saved = true;var html = getHTML();if (history.data[history.position] != html){var nBeginLen = history.data.length;var nPopLen = history.data.length - history.position;for (var i=1; i<nPopLen; i++){history.data.pop();history.bookmark.pop();}history.data[history.data.length] = html;if (eWebEditor.document.selection.type != "Control"){try{history.bookmark[history.bookmark.length] = eWebEditor.document.selection.createRange().getBookmark();}catch(e){history.bookmark[history.bookmark.length] = "";}} else {var oRng = eWebEditor.document.selection.createRange();var el = oRng.item(0);history.bookmark[history.bookmark.length] = "[object]|" + el.tagName + "|" + getElementTagIndex(el);}if (nBeginLen!=0){history.position++;}}}function getElementTagIndex(el){var els = eWebEditor.document.body.getElementsByTagName(el.tagName);for (var i=0; i<els.length; i++){if (els[i]==el){return i;}}return null;}function initHistory() {history.data.length = 0;history.bookmark.length = 0;history.position = 0;history.saved = false;}function goHistory(value) {if (!history.saved){saveHistory();}if (value == -1){if (history.position > 0){history.position = history.position - 1;setHTML(history.data[history.position], true);setHistoryCursor();}} else {if (history.position < history.data.length -1){history.position = history.position + 1;setHTML(history.data[history.position], true);setHistoryCursor();}}eWebEditor.focus();}function setHistoryCursor() {var s_Bookmark = history.bookmark[history.position];if (s_Bookmark){eWebEditor_Layout.focus();if (s_Bookmark.substring(0,8) != "[object]"){r = eWebEditor.document.body.createTextRange();if (r.moveToBookmark(history.bookmark[history.position])){r.select();}}else{if (sCurrMode=="EDIT"){r = eWebEditor.document.body.createControlRange();var a = s_Bookmark.split("|");var els = eWebEditor.document.body.getElementsByTagName(a[1]);var el = els[a[2]];r.addElement(el);r.select();}}}}function getStyleEditorHeader(){if ((sCurrMode=="EDIT") || (sCurrMode=="VIEW")){return config.StyleEditorHeader1 + sExtCSS + config.StyleEditorHeader2;}else{return config.StyleEditorHeader1 + config.StyleEditorHeader2;}}function getCount(n_Type){var str=getText();str = str.replace(/\n/g,"");str = str.replace(/\r/g,"");var l=str.length;var n=0;    for (var i=0;i<l;i++){        if (str.charCodeAt(i)<0||str.charCodeAt(i)>255){if (n_Type!=0){n++;if (n_Type==3){n++;}}}else{if (n_Type!=1){n++;}}    }    return n;}function getText(){eWebEditor_Temp_HTML.innerHTML = getHTML();var s = eWebEditor_Temp_HTML.innerText;return s;}function spellCheck(){try {var tmpis = new ActiveXObject("ieSpell.ieSpellExtension");tmpis.CheckAllLinkedDocuments(eWebEditor.document);} catch(exception) {if (confirm(lang["MsgIeSpellDownload"])){window.open("http://www.iespell.com/download.php","IeSpellDownload");}}}