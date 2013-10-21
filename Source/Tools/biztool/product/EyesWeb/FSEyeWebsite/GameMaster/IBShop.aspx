<%@ page language="C#" masterpagefile="~/Common/Main.master" autoeventwireup="true" inherits="GameMaster_IBShop, App_Web_6lmf-asn" validaterequest="false" enableviewstate="false" title="<%$ Resources:StringDef,IBShop %>" theme="BlazeSky" %>

<asp:Content ID="Content2" ContentPlaceHolderID="ContentPlaceHolderMain" Runat="server" EnableViewState="false">

<head>
<script type="text/javascript" src="../Script/base64.js"></script>

<script type="text/javascript">
var DefaultPanelWidth = 400;
var DefaultPanelHeight = 200;
var DefaultPanelMargin = 10;
var DefaultPanelCellWidth = 200;
var DefaultPanelCellHeight = 100;
var DefaultPanelTitleWidth = 300;
var DefaultPanelTitleHeight = 50;
var DefaultPanelTitleBody = "<Layout w=500><Seg t-a=center f=wrap><Obj t=text wc=255,0,0 f-f=stzhongs-11>标题1</Obj></Seg></Layout>";
var DefaultPanelTheme = 0;
var SelectStyleBorder = "solid 2px #0E55C7";
var SelectStyleColor = "#0E55C7";
var OriginalStyleColor = "black";
var OriginalStyleBorder = "solid 2px #7A7A7A";
var SelectPageTitleBackgroundColor = "#0E55C7";
var MouseOverBorderStyle = "solid 2px #C79F0E";
var MouseOverBackGroundColor = "#D1D1D1";
var MouseOverColor = "#FF7508";
var StyleElementBorder = "border";
var StyleElementColor = "color";
var AttributeBoardTop = 40;
var AttributeBoardLeft = 10;
var DirectionUp = "up";
var DirectionDown = "down";
var DirectionLeft = "left";
var DirectionRight = "right";

//margin-left:102px; 在pageInitCssText里加入这个为什么效果为什么会变形
var pageInitCssText = "position:relative; border: solid 1px black; width:800px; height:400px; overflow-y:auto;"
var panelInitCssText = "position:absolute; overflow:hidden; text-align:center; border:" + OriginalStyleBorder + ";";
var cellInitCssText = "position:relative; float:left; padding:2px; cursor:hand; overflow:hidden; border:" + OriginalStyleBorder + ";";
var itemInitCssText = "position:relative; z-index:10; margin:3px; cursor:hand;color:black;";
var titleInitCssText = "float:left;border:solid 2px gray;overflow:hidden;";
var pageTitleCssText = "float:left;margin:0px;text-align:center; padding:2px;width:50px; height:10px;border-top:solid 1px black; border-right:solid 1px black;background:white;cursor:hand;";

var isResize = false;                                       //改变大小标记位
var dragObject = null;
var dragHelper  = null;
var selectObject = null;
var lastSelectObject = null;
var mouseOffset = null;
var xmlhttp = false;
var pageTitleIndex = 0;
var selectPage = null;
var lastSelectPage = null;
var ibItemArray = new Array();
var ibItemIdMax = 0;                                        //记录IbItem中Id中最大的(用作新增IbItem)
var ibItemStyleArray = new Array();
var ibItemStyleIdMax = 0;
var itemArray = new Array();
var keyWords = null;
var itemSearching = false;
var mouseOverBorderCache = null;
var mouseOverColorCache = null;
var selectObjectBorderCache = null;

function init()
{
    //loadSettingInit();
}

function mousePoint(x,y)
{
    this.x = x;
    this.y = y;
}

function ibItem(id,genre,detail,particular,level,price,discount,state,label)
{
    this.id = id;
    this.genre = genre;
    this.detail = detail;
    this.particular = particular;
    this.level = level;
    this.price = price;
    this.discount = discount;
    this.state = state;
    this.label = label;
}

function ibItemStyle(id,template)
{
    this.id = id;
    this.template = template;
}

function getIbItemStyle(id)
{
    if(id)
    {
        for(var index = 0; index != ibItemStyleArray.length; ++index)
        {
            if(ibItemStyleArray[index].id == id)
            {
                return ibItemStyleArray[index];
            }
        }
    }
    return false;
}

function getIbItemStyleByTemplate(template)
{
    if(template)
    {
        for(var index = 0; index != ibItemStyleArray.length; ++index)
        {
            if(ibItemStyleArray[index].template == template)
            {
                return ibItemStyleArray[index];
            }
        }
    }
    return false;
}

function getIbItemById(id)
{
    if(id)
    {
        for(var index = 0; index != ibItemArray.length; ++index)
        {
            if(ibItemArray[index].id == id)
            {
                return ibItemArray[index];
            }
        }
    }
    return false;
}

function getIbItemByTemplateId(templateId)
{
    if(templateId)
    {
        var ids = templateId.split(',');
        for(var index = 0; index != ibItemArray.length; ++index)
        {
            if( ibItemArray[index].genre == ids[0] &&
                ibItemArray[index].detail == ids[1] &&
                ibItemArray[index].particular == ids[2] &&
                ibItemArray[index].level == ids[3] )
            {
                return ibItemArray[index];
            }
        }
    }
    return false;
}

function item(templateId,name)
{
    this.templateId = templateId;
    this.name = name;
}

function getItem(templateId)
{
    if(templateId)
    {
        for(var index = 0; index != itemArray.length; ++index)
        {
            if(itemArray[index].templateId == templateId)
            {
                return itemArray[index];
            }
        }
    }
    return false;
}

function loadSettingInit()
{
    loadItem();
    loadIbItem();
    loadItemStyle();
    loadSetting();
    createDragHelper();
    
    if(document.addEventListener)
	{
	    document.addEventListener("mousemove",mouseMove,false);
	    document.addEventListener("mouseup",mouseUp,false);	    
	    window.addEventListener("scroll",mouseUp,false);	    
	    //document.addEventListener("keydown",keyDown,false);
	    //document.getElementById("itemSearchText").addEventListener("change",itemSearch,false);
	}
	else
	{
	    document.attachEvent("onmousemove",mouseMove);
	    document.attachEvent("onmouseup",mouseUp);
        //window.attachEvent("onscroll",scroll);
        window.attachEvent("onscroll",scroll);
	    //document.getElementById("itemSearchText").attachEvent("onchange",itemSearch);
	}
}

function createDragHelper()
{
    dragHelper = document.createElement('DIV');
	dragHelper.style.cssText = "position:absolute;display:none;padding:0px";

	document.body.appendChild(dragHelper);
}

function loadItem()
{
    if(window.ActiveXObject)
    {
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    var url = "GameMasterHandler.ashx?op=LoadItem";
    //第三个参数为false说明为同步而非异步
    xmlhttp.open("GET",url,false);
    xmlhttp.onreadystatechange = createItem;
    xmlhttp.send(null);
}

function loadSetting()
{
    if(window.ActiveXObject)
    {
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    var url = "GameMasterHandler.ashx?op=LoadSetting" + wrapUrl();
    //第三个参数为false说明为同步而非异步
    xmlhttp.open("GET",url,false);
    xmlhttp.onreadystatechange = createDom;
    xmlhttp.send(null);
    //createDom();
}

function loadIbItem()
{
    if(window.ActiveXObject)
    {
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    var url = "GameMasterHandler.ashx?op=LoadIbItem" + wrapUrl();
    xmlhttp.open("GET",url,false);
    xmlhttp.onreadystatechange = createIbItem;
    xmlhttp.send(null);
    //createIbItem();
}

function loadItemStyle()
{
    if(window.ActiveXObject)
    {
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    var url = "GameMasterHandler.ashx?op=LoadItemStyle" + wrapUrl();
    xmlhttp.open("GET",url,false);
    xmlhttp.onreadystatechange = createItemStyle;
    xmlhttp.send(null);
    //createItemStyle();
}

function wrapUrl()
{
    return "&timeStamp=" + new Date().getTime();
}

function createItem()
{
    if(xmlhttp && xmlhttp.readyState == 4 && xmlhttp.status == 200)
    {
        try
        {
            var ibItemContainer = document.getElementById("itemContainer");
            var itemNodes = xmlhttp.responseXML.getElementsByTagName("itemList")[0].childNodes;
            for(var index = 0; index != itemNodes.length; ++index)
            {
                var newItem = new item(
                    itemNodes[index].getAttribute("templateId"),
                    utf8to16(base64decode(itemNodes[index].getAttribute("name")))
                );
                itemArray.push(newItem);
                
                if(itemArray.length <= 20)
                {
                    addItemToContainer(newItem);
                }
            }
            
            xmlhttp = null;
        }
        catch(e)
        {
            alert(e.name + "\n" + e.message);
        }
    }
}

function createIbItem()
{
    if(xmlhttp && xmlhttp.readyState == 4 && xmlhttp.status == 200)
    {
        try
        {
            var itemNodes = xmlhttp.responseXML.getElementsByTagName("itemList")[0].childNodes;
            for(var index = 0; index != itemNodes.length; ++index)
            {
                var newIbItem = new ibItem(
                    itemNodes[index].getAttribute("id"),
                    itemNodes[index].getAttribute("genre"),
                    itemNodes[index].getAttribute("detail"),
                    itemNodes[index].getAttribute("particular"),
                    itemNodes[index].getAttribute("level"),
                    itemNodes[index].getAttribute("price"),
                    itemNodes[index].getAttribute("discount"),
                    itemNodes[index].getAttribute("state"),
                    itemNodes[index].getAttribute("label")
                );
                ibItemArray.push(newIbItem);
                
                //记录Id中最大的用作生成新的IbItem
                if(newIbItem.id > ibItemIdMax)
                    ibItemIdMax = newIbItem.id;
            }
            
            xmlhttp = null;
        }
        catch(e)
        {
            alert(e.name + "\n" + e.message);
        }
    }    
}

function createItemStyle()
{
    if(xmlhttp && xmlhttp.readyState == 4 && xmlhttp.status == 200)
    {
        try
        {
            var itemNodes = xmlhttp.responseXML.getElementsByTagName("itemStyleList")[0].childNodes;
            for(var index = 0; index != itemNodes.length; ++index)
            {
                 var template = utf8to16(base64decode(itemNodes[index].getAttribute("template")));
                 var newIbItemStyle = new ibItemStyle(
                     itemNodes[index].getAttribute("id"),
                     template
                 );
                 
                 ibItemStyleArray.push(newIbItemStyle);
                 
                 if(newIbItemStyle.id > ibItemStyleIdMax) ibItemStyleIdMax = newIbItemStyle.id;
            }
            
            xmlhttp = null;
        }
        catch(e)
        {
            alert("Exception in createItemStyle " + e.name + "\n" + e.message);
        }
    }
}

function createDom()
{
    if(xmlhttp && xmlhttp.readyState == 4 && xmlhttp.status == 200)
    {
        try
        {
            var pageNodes = xmlhttp.responseXML.getElementsByTagName("shelfContainer")[0].childNodes;
            for(var pageIndex = 0; pageIndex != pageNodes.length; ++pageIndex)
            {
                var page = pageNodes[pageIndex];
                var newPage = document.createElement("div");
                newPage.style.cssText = pageInitCssText;
                newPage.setAttribute("type","page");
                var pageName = utf8to16(base64decode(page.getAttribute("name")));
                newPage.setAttribute("name",pageName);
                
                registerMouseDown(newPage);
                //newPage.attachEvent("onkeydown",keyDown);
                
                document.getElementById("pageContainer").appendChild(newPage);
                newPage.setAttribute("id","page" + pageTitleIndex);
                newPage.style.display = "none";
                
                var newLi = document.createElement("li");
                newLi.setAttribute("pageId","page" + pageTitleIndex);
                var textNode = document.createTextNode(pageName);
                newLi.appendChild(textNode);
                newLi.style.cssText = pageTitleCssText;//"float:left;margin:0px;text-align:center;	padding:2px;width:50px;	height:10px;border-top:solid 1px blue;	border-left:solid 1px black;border-right:solid 1px black;background:white;";
                newLi.attachEvent("onclick",pageClick);
                document.getElementById("pageTitle").appendChild(newLi);
                
                if(pageTitleIndex == 0)
                {
                    newPage.style.display = "";
                    newLi.style.background = SelectPageTitleBackgroundColor;
                    selectPage = newLi;
                }
                
                ++pageTitleIndex;
                
                for(var panelIndex = 0; panelIndex != page.childNodes.length; ++panelIndex)
                {
                    var panel = page.childNodes[panelIndex];
                    
                    var newPanel = addPanel(
                        newPage,
                        panel.getAttribute("left"),
                        panel.getAttribute("top"),
                        panel.getAttribute("width"),
                        panel.getAttribute("height"),
                        panel.getAttribute("margin"),
                        panel.getAttribute("cellWidth"),
                        panel.getAttribute("cellHeight"),
                        panel.getAttribute("titleWidth"),
                        panel.getAttribute("titleHeight"),
                        utf8to16(base64decode(panel.getAttribute("titleBody"))),
                        panel.getAttribute("theme")
                    );
                    
                    for(var cellIndex = 0; cellIndex != panel.childNodes.length; ++cellIndex)
                    {
                        var cell = panel.childNodes[cellIndex];
                        
                        var newCell = addCell(
                            newPanel,
                            panel.getAttribute("margin"),                        
                            panel.getAttribute("cellWidth"),
                            panel.getAttribute("cellHeight"),
                            cell.getAttribute("style")
                        );
                        var itemId = cell.getAttribute("itemId")
                        newCell.setAttribute("itemId",itemId);
                        var existIbItem = getIbItemById(itemId);
                        if(existIbItem)
                        {
                            var templateId = existIbItem.genre + "," + existIbItem.detail + "," + existIbItem.particular + "," + existIbItem.level;
                            var item = getItem(templateId);
                            newCell.setAttribute("templateId",templateId);
                            if(item)
                            {
                                var ele = document.createElement("div");
                                ele.style.cssText = itemInitCssText;
                                ele.setAttribute("type","item");
                                ele.setAttribute("templateId",item.templateId);
                                ele.innerHTML = item.name;
                                ele.style.position = "absolute";
                                ele.style.top = 5;
                                ele.style.left = 5;
                                registerMouseDown(ele);
                                registerMouseOverAndOut(ele);
                                newCell.appendChild(ele);
                            }
                            else
                            {
                                var unknowItem = document.createElement("div");
                                unknowItem.style.cssText = itemInitCssText;
                                unknowItem.innerHTML = "<%= Resources.StringDef.UnknowItem %>";
                                newCell.appendChild(unknowItem);
                            }
                        }
                        else
                        {
                            alert("<%= Resources.StringDef.MsgItemNotFound %>" + cell.getAttribute("itemId"));
                        }
                    }
                }
            }
            
            xmlhttp = null;
        }
        catch(e)
        {
            
        }
    }
}

function addItemToContainer(item)
{
    var containerElement = document.getElementById("itemContainer");
    if(containerElement && item)
    {
        var ele = document.createElement("div");
        ele.style.cssText = itemInitCssText;
        ele.setAttribute("type","item");
        ele.setAttribute("templateId",item.templateId);
        ele.innerHTML = item.name;
        registerMouseDown(ele);
        registerMouseOverAndOut(ele);
        containerElement.appendChild(ele);
    }
    else
    {
        alert("容器或者物品为空");
    }
}

function itemSearch()
{
    //itemSearching标记位　其作用和lock差不多
    if(!itemSearching)
    {
        itemSearching = true;
        var newKeyWords = document.getElementById("itemSearchText").getAttribute("value");
        if(keyWords != newKeyWords)
        {
            keyWords = newKeyWords;
            //alert("itemSearch");
            var itemContainer = document.getElementById("itemContainer");
            //Remove all child
            if(itemContainer.childNodes.length)
            {
                for(var index = itemContainer.childNodes.length-1; index>=0; --index)
                {
                    itemContainer.removeChild(itemContainer.childNodes[index]);
                }
            }
            for(var index = 0; index != itemArray.length; ++index)
            {
                if(itemArray[index].name.indexOf(keyWords) != -1)
                {
                    addItemToContainer(itemArray[index]);
                }
                if(itemContainer.childNodes.length > 20) break;
            }
        }
        itemSearching = false;
    }
    
    //window.event.cancelBubble = true;
}

function pageClick()
{
    resetSelectObjectStyle();
    
	if(selectPage)
	{
		lastSelectPage = selectPage;
	}

	selectPage = window.event.srcElement;
	if(selectPage != lastSelectPage)
	{
	    selectPage.style.background = SelectPageTitleBackgroundColor;
	    var page = document.getElementById(selectPage.getAttribute("pageId"));
	    page.style.display = "";
        selectObject = page;
        showAttribute();

	    if(lastSelectPage)
	    {
		    lastSelectPage.style.background = "white";
	        var lastPage = document.getElementById(lastSelectPage.getAttribute("pageId"));
	        if(lastPage)
	            lastPage.style.display = "none";
	    }
	}

	window.event.cancelBubble = true;
}

function clearChildNodes(ele)
{
    if(ele.childNodes.length)
    {
        for(var index=ele.childNodes.length-1; index>=0; --index) 
        {
            ele.removeChild(ele.childNodes[index]);
        }
    }
}

function showAttribute()
{
    if(selectObject)
    {
        var type = selectObject.getAttribute("type");
        if(type.localeCompare("cell") == 0)
        {
            var att = document.getElementById("attributeBoard");
            clearChildNodes(att);
            
            var closeDiv = document.createElement("div");
            closeDiv.style.cssText = "float:right; border:solid 1px black; color:red;cursor:hand;";
            closeDiv.innerHTML = "×";
            closeDiv.attachEvent("onclick",closeAttributeDiv);  
            att.appendChild(closeDiv);
            
            var widthInputText = document.createElement("div");
            widthInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            widthInputText.innerHTML = "<%= Resources.StringDef.Width %>";
            att.appendChild(widthInputText);
            var widthInput = document.createElement("input");
            widthInput.setAttribute("value",selectObject.style.width);
            widthInput.setAttribute("id","BoardInputWidth");
            att.appendChild(widthInput);
            
            var heightInputText = document.createElement("div");
            heightInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            heightInputText.innerHTML = "<%= Resources.StringDef.Height %>";
            att.appendChild(heightInputText);
            var heightInput = document.createElement("input");
            heightInput.setAttribute("value",selectObject.style.height);
            heightInput.setAttribute("id","BoardInputHeight");
            att.appendChild(heightInput);
            
            var templateId = selectObject.getAttribute("templateId");
            var price = discount = state = label = 0;
            var existIbItem = null;
            if(templateId)
            {
                existIbItem = getIbItemByTemplateId(templateId);
            }
            if(existIbItem)
            {            
                price = existIbItem.price;
                discount = existIbItem.discount;
                state = existIbItem.state;
                label = existIbItem.label;
            }
            
            var templatIdInputText = document.createElement("div");
            templatIdInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            templatIdInputText.innerHTML =  "<%= Resources.StringDef.TemplateId %>";
            att.appendChild(templatIdInputText);
            var templatIdInput = document.createElement("input");
            templatIdInput.readOnly = true;
            templatIdInput.setAttribute("value",templateId);
            templatIdInput.setAttribute("id","BoardInputTemplateId");
            att.appendChild(templatIdInput);
            
            var priceInputText = document.createElement("div");
            priceInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            priceInputText.innerHTML = "<%= Resources.StringDef.Price %>";
            att.appendChild(priceInputText);
            var priceInput = document.createElement("input");
            priceInput.setAttribute("value",price);
            priceInput.setAttribute("id","BoardInputPrice");
            att.appendChild(priceInput);

            var discountInputText = document.createElement("div");
            discountInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            discountInputText.innerHTML = "<%= Resources.StringDef.Discount %>";
            att.appendChild(discountInputText);
            var discountInput = document.createElement("input");
            discountInput.setAttribute("value",discount);
            discountInput.setAttribute("id","BoardInputDiscount");
            att.appendChild(discountInput);

            var stateInputText = document.createElement("div");
            stateInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            stateInputText.innerHTML = "<%= Resources.StringDef.State %>";
            att.appendChild(stateInputText);
            var stateInput = document.createElement("input");
            stateInput.setAttribute("value",state);
            stateInput.setAttribute("id","BoardInputState");
            att.appendChild(stateInput);

            var labelInputText = document.createElement("div");
            labelInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            labelInputText.innerHTML = "<%= Resources.StringDef.Label %>";
            att.appendChild(labelInputText);
            var labelInput = document.createElement("input");
            labelInput.setAttribute("value",label);
            labelInput.setAttribute("id","BoardInputLabel");
            att.appendChild(labelInput);
                    
            var styleTextInputText = document.createElement("div");
            styleTextInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            styleTextInputText.innerHTML = "<%= Resources.StringDef.StyleText %>";
            att.appendChild(styleTextInputText);
            var styleTextInput = document.createElement("textarea");
            var selectItemStyleId = selectObject.getAttribute("styleId");
            var styleText = null;
            if(selectItemStyleId)
            {
                styleText = getIbItemStyle(selectItemStyleId).template;
            }
            styleTextInput.style.cssText = "height:50px;font-size:12px;";
            styleTextInput.setAttribute("value",styleText);
            styleTextInput.setAttribute("id","BoardInputStyleText");
            att.appendChild(styleTextInput);

            var editDiv = document.createElement("div");
            editDiv.style.cssText = "margin-left:120px;margin-right:120px;margin-top:5px;text-align:center;background-color:gray;padding:2px;cursor:hand;";
            editDiv.innerHTML = "<%= Resources.StringDef.Modify %>";
            editDiv.attachEvent("onclick",editAttribute);
            att.appendChild(editDiv);
            
            setAttributeBoardVisible(true);
        }
        else if(type.localeCompare("panel") == 0)
        {            
            var att = document.getElementById("attributeBoard");
            clearChildNodes(att);            
            
            var closeDiv = document.createElement("div");
            closeDiv.style.cssText = "float:right; border:solid 1px black; color:red;cursor:hand;";
            closeDiv.innerHTML = "×";
            closeDiv.attachEvent("onclick",closeAttributeDiv);
            att.appendChild(closeDiv);
            
            var leftDivText = document.createElement("div");
            leftDivText.style.cssText = "clear:both;width:100px;float:left;text-align:right;";
            leftDivText.innerHTML = "<%= Resources.StringDef.X %>";
            att.appendChild(leftDivText);            
            var leftInput = document.createElement("input");
            leftInput.setAttribute("value",selectObject.style.left);
            leftInput.setAttribute("id","BoardInputLeft");
            att.appendChild(leftInput);
                        
            var topInputText = document.createElement("div");
            topInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            topInputText.innerHTML = "<%= Resources.StringDef.Y %>";
            att.appendChild(topInputText);            
            var topInput = document.createElement("input");
            topInput.setAttribute("value",selectObject.style.top);
            topInput.setAttribute("id","BoardInputTop");
            att.appendChild(topInput);
            
            var widthInputText = document.createElement("div");
            widthInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            widthInputText.innerHTML = "<%= Resources.StringDef.Width %>";
            att.appendChild(widthInputText);
            var widthInput = document.createElement("input");
            widthInput.setAttribute("value",selectObject.style.width);
            widthInput.setAttribute("id","BoardInputWidth");
            att.appendChild(widthInput);
            
            var heightInputText = document.createElement("div");
            heightInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            heightInputText.innerHTML = "<%= Resources.StringDef.Height %>";
            att.appendChild(heightInputText);
            var heightInput = document.createElement("input");
            heightInput.setAttribute("value",selectObject.style.height);
            heightInput.setAttribute("id","BoardInputHeight");
            att.appendChild(heightInput);
            
            var marginInputText = document.createElement("div");
            marginInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            marginInputText.innerHTML = "<%= Resources.StringDef.Margin %>";
            att.appendChild(marginInputText);
            var marginInput = document.createElement("input");
            marginInput.setAttribute("value",selectObject.getAttribute("margin"));
            marginInput.setAttribute("id","BoardInputMargin");
            att.appendChild(marginInput);

            var cellWidthInputText = document.createElement("div");
            cellWidthInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            cellWidthInputText.innerHTML = "<%= Resources.StringDef.CellWidth %>";
            att.appendChild(cellWidthInputText);
            var cellWidthInput = document.createElement("input");
            cellWidthInput.setAttribute("value",selectObject.getAttribute("cellWidth"));
            cellWidthInput.setAttribute("id","BoardInputCellWidth");
            att.appendChild(cellWidthInput);

            var cellHeightInputText = document.createElement("div");
            cellHeightInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            cellHeightInputText.innerHTML = "<%= Resources.StringDef.CellHeight %>";
            att.appendChild(cellHeightInputText);
            var cellHeightInput = document.createElement("input");
            cellHeightInput.setAttribute("value",selectObject.getAttribute("cellHeight"));
            cellHeightInput.setAttribute("id","BoardInputCellHeight");
            att.appendChild(cellHeightInput);
            
            var titleWidthInputText = document.createElement("div");
            titleWidthInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            titleWidthInputText.innerHTML = "<%= Resources.StringDef.TitleWidth %>";
            att.appendChild(titleWidthInputText);
            var titleWidthInput = document.createElement("input");
            titleWidthInput.setAttribute("value",selectObject.getAttribute("titleWidth"));
            titleWidthInput.setAttribute("id","BoardInputTitleWidth");
            att.appendChild(titleWidthInput);
            
            var titleHeightInputText = document.createElement("div");
            titleHeightInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            titleHeightInputText.innerHTML = "<%= Resources.StringDef.TitleHeight %>";
            att.appendChild(titleHeightInputText);
            var titleHeightInput = document.createElement("input");
            titleHeightInput.setAttribute("value",selectObject.getAttribute("titleHeight"));
            titleHeightInput.setAttribute("id","BoardInputTitleHeight");
            att.appendChild(titleHeightInput);
            
            var titleBodyInputText = document.createElement("div");
            titleBodyInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;vertical-align :middle;";
            titleBodyInputText.innerHTML = "<%= Resources.StringDef.TitleBody %>";
            att.appendChild(titleBodyInputText);
            var titleBodyInput = document.createElement("textarea");
            titleBodyInput.setAttribute("value",selectObject.getAttribute("titleBody"));
            titleBodyInput.style.cssText = "height:50px;font-size:12px;";
            titleBodyInput.setAttribute("id","BoardInputTitleBody");
            att.appendChild(titleBodyInput);

            var editDiv = document.createElement("div");
            editDiv.style.cssText = "margin-left:120px;margin-right:120px;margin-top:5px;text-align:center;background-color:gray;padding:2px;cursor:hand;";
            editDiv.innerHTML = "<%= Resources.StringDef.Modify %>";
            editDiv.attachEvent("onclick",editAttribute);
            att.appendChild(editDiv);
            
            setAttributeBoardVisible(true);
        }
        else if(type.localeCompare("page") == 0)
        {
            var att = document.getElementById("attributeBoard");
            clearChildNodes(att);            
            
            var closeDiv = document.createElement("div");
            closeDiv.style.cssText = "float:right; border:solid 1px black; color:red;cursor:hand;";
            closeDiv.innerHTML = "×";
            closeDiv.attachEvent("onclick",closeAttributeDiv);
            att.appendChild(closeDiv);
            
            var pageNameInputText = document.createElement("div");
            pageNameInputText.style.cssText = "clear:both;width:100px;float:left;text-align:right;";
            pageNameInputText.innerHTML = "<%= Resources.StringDef.Name %>";
            att.appendChild(pageNameInputText);
            var pageNameInput = document.createElement("input");
            pageNameInput.setAttribute("value",selectObject.getAttribute("name"));
            pageNameInput.setAttribute("id","BoardPageNameInput");
            att.appendChild(pageNameInput);
            
            var editDiv = document.createElement("div");
            editDiv.style.cssText = "margin-left:120px;margin-right:120px;margin-top:5px;text-align:center;background-color:gray;padding:2px;cursor:hand;";
            editDiv.innerHTML = "<%= Resources.StringDef.Modify %>";
            editDiv.attachEvent("onclick",editAttribute);
            att.appendChild(editDiv);
            
            setAttributeBoardVisible(true);
        }
    }
}

function closeAttributeDiv()
{
    document.getElementById("attributeBoard").style.display = "none";
    window.event.cancelBubble = true;
}

function editAttribute()
{
    if(selectObject)
    {
        var type = selectObject.getAttribute("type");
        if(type.localeCompare("panel") == 0)
        {
            try
            {
                var panelMargin = document.getElementById("BoardInputMargin").getAttribute("value");
                selectObject.style.top = document.getElementById("BoardInputTop").getAttribute("value");
                selectObject.style.left = document.getElementById("BoardInputLeft").getAttribute("value");
                selectObject.style.width = document.getElementById("BoardInputWidth").getAttribute("value");
                selectObject.style.height = document.getElementById("BoardInputHeight").getAttribute("value");                
                selectObject.setAttribute("margin",panelMargin);
                selectObject.setAttribute("cellWidth",document.getElementById("BoardInputCellWidth").getAttribute("value"));
                selectObject.setAttribute("cellHeight",document.getElementById("BoardInputCellHeight").getAttribute("value"));
                selectObject.setAttribute("titleWidth",document.getElementById("BoardInputTitleWidth").getAttribute("value"));
                selectObject.setAttribute("titleHeight",document.getElementById("BoardInputTitleHeight").getAttribute("value"));
                selectObject.setAttribute("titleBody",document.getElementById("BoardInputTitleBody").getAttribute("value"));
                
                if(selectObject.childNodes.length)
                {
                    for(var index = 0; index != selectObject.childNodes.length; ++index)
                    {
                        var type = selectObject.childNodes[index].getAttribute("type");
                        if(type)
                        {
                            if(type.localeCompare("cell") == 0)
                            {
                                selectObject.childNodes[index].style.width = selectObject.getAttribute("cellWidth");
                                selectObject.childNodes[index].style.height = selectObject.getAttribute("cellHeight");
                                selectObject.childNodes[index].style.marginRight = panelMargin;
                                selectObject.childNodes[index].style.marginBottom = panelMargin;                                
                            }
                            else if(type.localeCompare("title") == 0)
                            {
                                selectObject.childNodes[index].style.width = selectObject.getAttribute("titleWidth");
                                selectObject.childNodes[index].style.height = selectObject.getAttribute("titleHeight");
                                selectObject.childNodes[index].style.marginRight = panelMargin;
                                selectObject.childNodes[index].style.marginBottom = panelMargin;
                            }
                        }
                    }
                }
                
                selectObject.style.paddingTop = panelMargin;
                selectObject.style.paddingLeft = panelMargin;
                
                alert("<%= Resources.StringDef.OperationSucceed %>");
            }
            catch(e)
            {
                alert(e.name + e.message);
            }                        
        }
        else if(type.localeCompare("cell") == 0)
        {
            try
            {
                var width = parseInt(document.getElementById("BoardInputWidth").getAttribute("value"));
                var height = parseInt(document.getElementById("BoardInputHeight").getAttribute("value"));
                var parent = selectObject.parentElement;
                var templateId = document.getElementById("BoardInputTemplateId").getAttribute("value");
                var ids = templateId.split(',');
                var price = document.getElementById("BoardInputPrice").getAttribute("value");
                var discount = document.getElementById("BoardInputDiscount").getAttribute("value");
                var state = document.getElementById("BoardInputState").getAttribute("value");
                var label = document.getElementById("BoardInputLabel").getAttribute("value");
                var styleText = document.getElementById("BoardInputStyleText").getAttribute("value");
                //var styleIdSelect = document.getElementById("BoardInputStyleId");
                //var styleId = styleIdSelect.childNodes[styleIdSelect.options.selectedIndex].innerText;
                if(ids.length < 4 || templateId == "0,0,0,0")
                {
                    alert("<%= Resources.StringDef.MsgTemplateIdInputError %>");
                    return;
                }
                
                parent.setAttribute("cellWidth",width);
                parent.setAttribute("cellHeight",height);
                if(parent.childNodes.length)
                {
                    for(var index = 0; index != parent.childNodes.length; ++index)
                    {
                        var type = parent.childNodes[index].getAttribute("type");
                        if(type)
                        {
                            if(type.localeCompare("cell") == 0)
                            {
                                parent.childNodes[index].style.width = width;
                                parent.childNodes[index].style.height = height;                                
                            }
                        }
                    }
                }
                
                var existIbItem = getIbItemByTemplateId(templateId);
                if(existIbItem)
                {
                    //修改IbItem
                    existIbItem.genre = ids[0];
                    existIbItem.detail = ids[1];
                    existIbItem.particular = ids[2];
                    existIbItem.level = ids[3];
                    existIbItem.price = price;
                    existIbItem.discount = discount;
                    existIbItem.state = state;
                    existIbItem.label = label;
                }
                else
                {
                    //新生成IbItem
                    ++ibItemIdMax;
                    var newIbItem = new ibItem(
                        ibItemIdMax,
                        ids[0],
                        ids[1],
                        ids[2],
                        ids[3],
                        price,
                        discount,
                        state,
                        label
                    );
                    
                    ibItemArray.push(newIbItem);
                    
                    selectObject.setAttribute("templateId",templateId);
                    selectObject.setAttribute("itemId",ibItemIdMax);
                }
                
                var existItemStyle = getIbItemStyleByTemplate(styleText);
                if(existItemStyle)
                {
                    selectObject.setAttribute("styleId",existItemStyle.id);
                }
                else
                {
                    //新增ItemStyle
                    ++ibItemStyleIdMax;
                    var newItemStyle = new ibItemStyle(ibItemStyleIdMax,styleText);                    
                    ibItemStyleArray.push(newItemStyle);                    
                    selectObject.setAttribute("styleId",ibItemStyleIdMax);
                }
                
                alert("<%= Resources.StringDef.OperationSucceed %>");
            }
            catch(e)
            {
                alert(e.name + e.message);
            }
        }
        else if(type.localeCompare("page") == 0)
        {
            try
            {
                var pageName = document.getElementById("BoardPageNameInput").getAttribute("value");
                if(pageName)
                {
                    selectObject.setAttribute("name",pageName);
                    var pageTitleContainer = document.getElementById("pageTitle");
                    if(pageTitleContainer && pageTitleContainer.childNodes.length)
                    {
                        for(var index = 0; index != pageTitleContainer.childNodes.length; ++index)
                        {
                            var pageId = pageTitleContainer.childNodes[index].getAttribute("pageId");
                            if(pageId == selectObject.getAttribute("id"))
                            {
                                pageTitleContainer.childNodes[index].firstChild.nodeValue = pageName;
                                break;
                            }
                        }
                    }
                }
            }
            catch(e)
            {
                alert(e.name + e.message);
            }
        }        
        
        showAttribute();
    }
    
    window.event.cancelBubble = true;
}

function mousePosition(evnt)
{
    var x = parseInt(evnt.clientX);
    var y = parseInt(evnt.clientY);
    return new mousePoint(x,y);
}

function getMouseOffset(target,envt)
{
    if(!evnt)
    {
        var evnt = window.event;
    }
    var mousePos = mousePosition(evnt);

    var x = mousePos.x - target.offsetLeft;
    var y = mousePos.y - target.offsetTop;
    return new mousePoint(x,y);
}
    
function getPosition(element)
{
    var left = 0;
	var top  = 0;
	while (element.offsetParent)
	{
		left += element.offsetLeft;
		top += element.offsetTop;
		element = element.offsetParent;
	}

	left += element.offsetLeft;
	top += element.offsetTop;

	return new mousePoint(left,top);
}

function getMouseAbsoluteOffset(target,envt)
{
    if(!evnt)
    {
        var evnt = window.event;
    }
    var mousePos = mousePosition(evnt);
    
    var targetPos = getPosition(target);
    var x = mousePos.x - targetPos.x;
    var y = mousePos.y - targetPos.y;
    return new mousePoint(x,y);
}

function registerMouseDown(item)
{
	if(!item) return;
	if(item.addEventListener)
	{
	    item.addEventListener("mousedown",mouseDown,false);
	}
	else
	{
	    item.attachEvent("onmousedown",mouseDown);
	}    	
}

function registerMouseOverAndOut(item)
{
	if(!item) return;
	if(item.addEventListener)
	{
	    item.addEventListener("mouseover",mouseOver,false);
	    item.addEventListener("mouseout",mouseOut,false);
	}
	else
	{
	    item.attachEvent("onmouseover",mouseOver);
	    item.attachEvent("onmouseout",mouseOut);
	}
}

function registerKeyDown(item)
{
	if(!item) return;
	if(item.addEventListener)
	{
	    item.addEventListener("keydown",keyDown,false);
	}
	else
	{
	    item.attachEvent("onkeydown",keyDown);
	}
}

function unregisterKeyDown(item)
{
	if(!item) return;
	if(item.removeEventListener)
	{
	    item.removeEventListener("keydown",keyDown,false);
	}
	else
	{
	    item.detachEvent("onkeydown",keyDown);
	}
}
    
/// <summary>
/// 判断dragObject(item)是否在node中(包括node的子节点)
/// </summary>
/// <param name="node">document的节点</param>
function isInPanel(node)
{
    var child = node.firstChild;
    while(child)
    {
        //nodeType为1表示为Element
        if(child.nodeType == 1)
        {
            var type = child.getAttribute("type");
            if(type)
            {
                if(type.localeCompare("page") == 0)
                {
                    if(isInPanel(child))
                        return true;
                }
                else if(type.localeCompare("panel") == 0)
                {
                    if(isInPanel(child))
                        return true;
                }
                else if(type.localeCompare("cell") == 0)
                {
                    var target = child;
        	        var targetPos = getPosition(target);
        	        var itemPos = getPosition(dragObject);
        	        //物品Pos在区域里即可
        	        if( itemPos.x >= targetPos.x &&
        	            itemPos.x <= (targetPos.x + parseInt(target.style.width)) &&
        	        	itemPos.y >= targetPos.y &&
        	        	itemPos.y <= (targetPos.y + parseInt(target.style.height)) )
                    {
                        //在cell里面
                        dragObject.style.position = "absolute";
                        dragObject.style.top = 5;
                        dragObject.style.left = 5;
                        //从以前的父亲元素中删除
                        dragObject.parentElement.removeAttribute("itemId");
                        dragObject.parentElement.removeAttribute("templateId");
                        dragObject.parentElement.removeChild(dragObject);
                        //使selectObject为dragObject并调整颜色 
                        setStyle(dragObject,StyleElementColor,SelectStyleColor);
                        selectObject = dragObject;
                        
                        if(target.lastChild && target.lastChild.nodeType == 1)
                        {
                            //替换以前的
                            target.replaceChild(dragObject,target.lastChild);
                        }
                        else
                        {
                            target.appendChild(dragObject);
                        }
                        
                        target.removeAttribute("itemId");
                        target.setAttribute("templateId",dragObject.getAttribute("templateId"));
                        var existIbItem = getIbItemByTemplateId(dragObject.getAttribute("templateId"));
                        if(existIbItem)
                        {
                            target.setAttribute("itemId",existIbItem.id);
                        }
                        return true;
                    }
                }
            }
        }
        
        child = child.nextSibling;
    }
    return false;
}    

function resetSelectObjectStyle()
{
    //恢复上次选中元素的样式
    if(selectObject)
    {
        var type = selectObject.getAttribute("type");
        if(type)
        {
            if(type.localeCompare("cell") == 0 || type.localeCompare("panel") == 0 || type.localeCompare("title") == 0 )
            {
                setStyle(selectObject,StyleElementBorder,OriginalStyleBorder);
                //selectObject.style.border = OriginalStyleBorder;
            }
            else if(type.localeCompare("item") == 0)
            {
                setStyle(selectObject,StyleElementColor,OriginalStyleColor);
            }
        }
    }
}

function moveTotal(direction)
{
    if(selectObject)
    {
        var type = selectObject.getAttribute("type");
        if(type.localeCompare("page") == 0 || type.localeCompare("panel") == 0 || type.localeCompare("cell") == 0)
        {
            move(direction,selectObject,type);
            return;
        }
    }
    alert("<%= Resources.StringDef.MsgIBShopSelectObject %>");
}

function move(direction,element,type)
{
    if(direction && element && type)
    {
        if(type.localeCompare("page") == 0)
        {
            if(direction.localeCompare(DirectionLeft) == 0)
            {
                var preNode = element.previousSibling;
                if(preNode)
                {
                    element.parentElement.insertBefore(element,preNode);

                    var titleEle = document.getElementById("pageTitle");
                    if(titleEle.childNodes.length)
                    {
                        for(var index = 0; index != titleEle.childNodes.length; ++index)
                        {
                            if(titleEle.childNodes[index].getAttribute("pageId") == element.getAttribute("id"))
                            {
                                titleEle.insertBefore(titleEle.childNodes[index],titleEle.childNodes[index].previousSibling);
                                return;
                            }
                        }
                    }
                }
            }            
            else if(direction.localeCompare(DirectionRight) == 0)
            {
                var nextNode = element.nextSibling;
                if(nextNode)
                {
                    element.parentElement.insertBefore(nextNode,selectObject);
                    
                    var titleEle = document.getElementById("pageTitle");
                    for(var index = 0; index != titleEle.childNodes.length; ++index)
                    {
                        if(titleEle.childNodes[index].getAttribute("pageId") == element.getAttribute("id"))
                        {
                            titleEle.insertBefore(titleEle.childNodes[index].nextSibling,titleEle.childNodes[index]);
                            return;
                        }
                    }
                }
            }            
        }
        else if(type.localeCompare("panel") == 0)
        {
            if(direction.localeCompare(DirectionUp) == 0)
            {
                selectObject.style.top = parseInt(selectObject.style.top) - 1;
            }
            else if(direction.localeCompare(DirectionDown) == 0)
            {
                selectObject.style.top = parseInt(selectObject.style.top) + 1;
            }
            else if(direction.localeCompare(DirectionLeft) == 0)
            {
                selectObject.style.left = parseInt(selectObject.style.left) - 1;
            }
            else if(direction.localeCompare(DirectionRight) == 0)
            {
                selectObject.style.left = parseInt(selectObject.style.left) + 1;
            }
        }
        else if(type.localeCompare("cell") == 0)
        {
            if(direction.localeCompare(DirectionLeft) == 0)
            {
                var preNode = element.previousSibling;
                if(preNode)
                {
                    element.parentElement.insertBefore(element,preNode);
                }
            }
            else if(direction.localeCompare(DirectionRight) == 0)
            {
                var nextNode = element.nextSibling;
                if(nextNode)
                {
                    element.parentElement.insertBefore(nextNode,element);                    
                }
            }
        }
    }
}

function scroll(evnt)
{
    evnt = evnt || window.event;
    var board = document.getElementById("attributeBoard");
    if(board.style.display != "none")
    {
        board.style.top = AttributeBoardTop + parseInt(document.documentElement.scrollTop);
        board.style.left = AttributeBoardLeft + parseInt(document.documentElement.scrollLeft);
    }
    
    if(evnt.stopPropagation)
    {
        evnt.stopPropagation();            
    }
    else
    {
        evnt.cancelBubble = true;
    }
}

function keyDown(evnt)
{
    evnt = evnt || window.event;
    if(selectObject)
    {
        var type = selectObject.getAttribute("type");
        if(type)
        {
            if(type.localeCompare("panel") == 0)
            {
                //键盘左                
                if(evnt.keyCode == 37)  move(DirectionLeft,selectObject,type);
                //键盘上
                else if(evnt.keyCode ==38)  move(DirectionUp,selectObject,type);
                //键盘右
                else if(evnt.keyCode == 39) move(DirectionRight,selectObject,type);
                //键盘下
                else if(evnt.keyCode == 40) move(DirectionDown,selectObject,type);
                //Delete键
                else if(evnt.keyCode == 46) deleteObj(selectObject,type);
                
                showAttribute();
            }
            else if(type.localeCompare("page") == 0)
            {
                //todo
            }
        }
    }

    if(evnt.stopPropagation)
    {
        evnt.stopPropagation();
    }
    else
    {
        evnt.cancelBubble = true;
    }
	
	return false;
}
    
function setAttributeBoardVisible(isVisible)
{
    var board = document.getElementById("attributeBoard");
    if(isVisible)
    {        
        board.style.display = "";
        board.style.top = AttributeBoardTop + document.documentElement.scrollTop;
        board.style.left = AttributeBoardLeft + document.documentElement.scrollLeft;
    }
    else
    {
        board.style.display = "none";
    }
}

function setStyle(element,styleElement,styleText)
{
    if(styleElement && styleText)
    {
        if(styleElement == StyleElementBorder)
        {
            element.style.border = styleText;
        }
        else if(styleElement == StyleElementColor)
        {
            element.style.color = styleText;
        }
    }
}

function mouseOver(evnt)
{
    evnt = evnt || window.event;
    var type = evnt.srcElement.getAttribute("type");
    if(type)
    {
        if(type.localeCompare("item") == 0)
        {
            mouseOverColorCache = evnt.srcElement.style.color;
            setStyle(evnt.srcElement,StyleElementColor,MouseOverColor);
        }
        else
        {
            if(selectObject != evnt.srcElement)
            {
                mouseOverBorderCache = evnt.srcElement.style.border;
                setStyle(evnt.srcElement,StyleElementBorder,MouseOverBorderStyle);
            }
        }
    }    
    
 	if(evnt.stopPropagation)
    {
        evnt.stopPropagation();
    }
    else
    {
        evnt.cancelBubble = true;
    }
}

function mouseOut(evnt)
{
    evnt = evnt || window.event;
    var type = evnt.srcElement.getAttribute("type");
    if(type)
    {
        if(type.localeCompare("item") == 0)
        {
            setStyle(evnt.srcElement,StyleElementColor,mouseOverColorCache);
        }
        else
        {
            if(evnt.srcElement != selectObject)
                setStyle(evnt.srcElement,StyleElementBorder,mouseOverBorderCache);
        }
    }
    
 	if(evnt.stopPropagation)
    {
        evnt.stopPropagation();
    }
    else
    {
        evnt.cancelBubble = true;
    }
}

function mouseDown(evnt)
{
    //dragObject非空时有问题
    if(dragObject)
    {
        return false;
    }

    resetSelectObjectStyle();

    evnt = evnt || window.event;
    selectObject = evnt.currentTarget || evnt.srcElement;
    var type = selectObject.getAttribute("type");
    if(type)
    {
        if(type.localeCompare("item") == 0)
        {
            setStyle(selectObject,StyleElementColor,SelectStyleColor);
            
            if(selectObject.parentElement == document.getElementById("itemContainer"))
            {
                var newNode = selectObject.cloneNode(true);
                var newNodePos = getPosition(selectObject);
                newNode.style.position = "absolute";
                newNode.style.top = newNodePos.y;
                newNode.style.left = newNodePos.x;
                dragObject = newNode;
                document.body.appendChild(newNode);
            }
            else				    
            {
                dragObject = selectObject;
            }
        }
        else if(type.localeCompare("cell") == 0)
        {
            //设置选中样式
            setStyle(selectObject,StyleElementBorder,SelectStyleBorder);
            
            dragObject = selectObject;
            //按下Shift键表明开始设置大小
            if(evnt.shiftKey)
            {

                isResize = true;
                dragObject.style.cursor = "se-resize";
                document.body.style.cursor = "se-resize";
            }
            else
            {                    
               dragObject.setAttribute("InitDragHelper","1");
            }
        }
        else if(type.localeCompare("panel") == 0)
        {
            //设置选中样式
            setStyle(selectObject,StyleElementBorder,SelectStyleBorder);
            
            dragObject = selectObject;
            if(evnt.shiftKey)
            {            
                isResize = true;
                dragObject.style.cursor = "se-resize";
                document.body.style.cursor = "se-resize";
                unregisterKeyDown(dragObject);
            }
        }
        else if(type.localeCompare("title") == 0)
        {
             dragObject = selectObject;
             
             setStyle(selectObject,StyleElementBorder,SelectStyleBorder);

             
             //按下Shift键表明开始设置大小
             if(evnt.shiftKey)
             {
                 isResize = true;
                 dragObject.style.cursor = "se-resize";
                 document.body.style.cursor = "se-resize";                 
             }
        }
       	if(dragObject)
       	    mouseOffset = getMouseOffset(dragObject, evnt);
        setAttributeBoardVisible(false);        
        showAttribute();
     }
 	
 	 if(evnt.stopPropagation)
     {
         evnt.stopPropagation();            
     }
     else
     {
         evnt.cancelBubble = true;
     }
}
    
function mouseUp(evnt)
{
    if(dragObject)
    {
        var type = dragObject.getAttribute("type");

        if(type.localeCompare("item") == 0)
        {
            //判断cell坐标并把物品添加到正确的cell中
            if(selectPage)
            {
                //判断选中的Page中的Cell
                var page = document.getElementById(selectPage.getAttribute("pageId"));
                if(page)
                {
                    if(!isInPanel(page))
                    {
                        dragObject.parentElement.removeAttribute("itemId");
                        dragObject.parentElement.removeAttribute("templateId");
                        dragObject.parentElement.removeChild(dragObject);
                    }
                }
            }
            
            showAttribute();
        }
        else if(type.localeCompare("cell") == 0)
        {
            if(!isResize)
            {
                //dragObject 是dragHelper
                dragObject.style.display = "";
                dragObject.style.visibility = "visible";
                
                dragHelper.style.display = "none";
            }
        }
        
        if(isResize)
        {
            isResize = false;
            dragObject.style.cursor = "hand";
            document.body.style.cursor = "default";
            if(type.localeCompare("panel") == 0)
                registerKeyDown(dragObject);
        }
    }
    dragObject = null;
}

function mouseMove(evnt)
{
    if(!dragObject) return;
    evnt = evnt || window.event;
    var mousePos = mousePosition(evnt);
    
    if(dragObject)
    {
        try
        {
            var type = dragObject.getAttribute("type");
            //Cell
            if(type.localeCompare("cell") == 0)
            {
                if(isResize)
                {
                    //改变panel大小
                    var dragPos = getPosition(dragObject);
                    var cellHeight = (mousePos.y + document.documentElement.scrollTop - dragPos.y) > 0 ? (mousePos.y + document.documentElement.scrollTop - dragPos.y) : 0;
                    var cellWidth = (mousePos.x + document.documentElement.scrollLeft - dragPos.x) > 0 ? (mousePos.x + document.documentElement.scrollLeft - dragPos.x) : 0;
                    dragObject.parentElement.setAttribute("cellHeight",cellHeight);
                    dragObject.parentElement.setAttribute("cellWidth",cellWidth);
                    if(dragObject.parentElement.childNodes.length)
                    {
                        for(var index = 0 ;index != dragObject.parentElement.childNodes.length; ++index)
                        {
                            var currObject = dragObject.parentElement.childNodes[index];
                            var type = currObject.getAttribute("type");
                            if(type)
                            {
                                if(type.localeCompare("cell") == 0)
                                {
                                    currObject.style.height = cellHeight;
                                    currObject.style.width = cellWidth;                                    
                                }
                            }
                        }
                    }
                    showAttribute();
                }
                else
                {
                    var initDragHelper = dragObject.getAttribute("InitDragHelper");
                    if(initDragHelper)
                    {
                        //清除dragHelper
                        if(dragHelper.childNodes.length)
                        {
                            for(var i=dragHelper.childNodes.length-1; i>=0; --i)
                            {
                                dragHelper.removeChild(dragHelper.childNodes[i]);
                            }
                        }                            
    		    	    
		    		    mouseOffset = getMouseAbsoluteOffset(dragObject,evnt);
    		    	    
		    		    var dragObjectCopy = dragObject.cloneNode(true);
		    		    dragObjectCopy.style.margin = 0;
                        dragHelper.appendChild(dragObjectCopy);
                        dragHelper.style.display = "";
                        dragObject.removeAttribute("InitDragHelper");
                    }

                    // move our helper div to wherever the mouse is (adjusted by mouseOffset)
	            	dragHelper.style.top  = mousePos.y - mouseOffset.y + "px";
	            	dragHelper.style.left = mousePos.x - mouseOffset.x + "px";

	            	var xPos = mousePos.x - mouseOffset.x + (parseInt(dragObject.offsetWidth) /2);
	            	var yPos = mousePos.y - mouseOffset.y + (parseInt(dragObject.offsetHeight)/2);
    	            
                    var beforeNode = null;
                    
                    //
                    if(dragObject.parentElement)
                    {
                        var relatedElements = dragObject.parentElement.childNodes;

                        if(relatedElements && relatedElements.length)
                        {
		    		        for(var index = 0; index != relatedElements.length; ++index)
		    		        {
            		    		
		    		            var ele = relatedElements[index];
		    		            // if the current item is "After" the item being dragged
		    		            var elePos = getPosition(ele);
		    		            if(dragObject != ele && ((elePos.x + ele.offsetWidth) > xPos) && (elePos.y + parseInt(ele.offsetHeight) > yPos))
                                {
		    				        beforeNode = ele;
		    				        break;
		    		            }
		    		        }
                        }
        		    	
		    		    // the item being dragged belongs before another item
		    	        if(beforeNode)
		    	        {
		    	            if(beforeNode != dragObject.nextSibling)
		    	            {
		    	    	        dragObject.parentElement.insertBefore(dragObject, beforeNode);
		    	            }				    	
		    	        // the item being dragged belongs at the end of the current container
		    	        }
		    	        else
		    	        {
		    	            //if((curTarget.nextSibling) || (curTarget.parentNode!=activeCont))
                            dragObject.parentElement.appendChild(dragObject);
		    	        }
		    	    }
		        	dragObject.style.display = '';
                    dragObject.style.visibility = 'hidden';                                     
                }
            }
            //Item
            else if(type.localeCompare("item") == 0)
            {
                dragObject.style.position = 'absolute';
                dragObject.style.top = mousePos.y - mouseOffset.y + "px";
                dragObject.style.left = mousePos.x - mouseOffset.x + "px";
            }
            //Panel
            else if(type.localeCompare("panel") == 0)
            {
                if(isResize)
                {
                    //改变panel大小
                    var dragPos = getPosition(dragObject);
                    dragObject.style.height = (mousePos.y + document.documentElement.scrollTop - dragPos.y) > 0 ? (mousePos.y +  document.documentElement.scrollTop - dragPos.y) : 0;
                    dragObject.style.width = (mousePos.x + document.documentElement.scrollLeft - dragPos.x) > 0 ? (mousePos.x + document.documentElement.scrollLeft - dragPos.x) : 0;
                }
                else
                {
                    dragObject.style.top = mousePos.y - mouseOffset.y + "px";
                    dragObject.style.left = mousePos.x - mouseOffset.x + "px";                
                }            
                showAttribute();
            }
            //
            else if(type.localeCompare("title") == 0)
            {
                if(isResize)
                {
                    //改变panel大小
                    var dragPos = getPosition(dragObject);
                    var height = (mousePos.y + document.documentElement.scrollTop - dragPos.y) > 0 ? (mousePos.y +  document.documentElement.scrollTop - dragPos.y) : 0;
                    var width = (mousePos.x + document.documentElement.scrollLeft - dragPos.x) > 0 ? (mousePos.x + document.documentElement.scrollLeft - dragPos.x) : 0;
                    dragObject.style.height = height;
                    dragObject.style.width = width;
                    dragObject.parentElement.setAttribute("titleHeight",dragObject.style.height);
                    dragObject.parentElement.setAttribute("titleWidth",dragObject.style.width);
                }
            }
           
        }
        catch(e)
        {
            alert(e.name + e.message);
        }
    }
    if(evnt.stopPropagation)
    {
        evnt.stopPropagation();
    }
    else
    {
        evnt.cancelBubble = true;
    }    
}
    
function createXmlResult()
{
    try
    {
        var result = '<?xml version="1.0" encoding="utf-8" ?>';
        result += '<shelfContainer>';
        var domString = domToString(document.getElementById("pageContainer"));
        if(domString)
            result += domString;
        else
        {
            return false;
        }
        result += '</shelfContainer>';
    
        var ibItemResult = '<?xml version="1.0" encoding="utf-8" ?>';
        ibItemResult += '<itemList>';
        for(var index = 0; index != ibItemArray.length; ++index)
        {
            ibItemResult += '<ibItem id="' + ibItemArray[index].id + '" ';
            ibItemResult += 'genre="' + ibItemArray[index].genre + '" ';
            ibItemResult += 'detail="' + ibItemArray[index].detail + '" ';
            ibItemResult += 'particular="' + ibItemArray[index].particular + '" ';
            ibItemResult += 'level="' + ibItemArray[index].level + '" ';
            ibItemResult += 'price="' + ibItemArray[index].price + '" ';
            ibItemResult += 'discount="' + ibItemArray[index].discount + '" ';
            ibItemResult += 'state="' + ibItemArray[index].state + '" ';
            ibItemResult += 'label="' + ibItemArray[index].label + '" />';
        }
        ibItemResult += '</itemList>';
        
        var ibItemStyleResult = '<?xml version="1.0" encoding="utf-8" ?>';
        ibItemStyleResult += '<itemStyleList>';
        for(var index = 0; index != ibItemStyleArray.length; ++index)
        {
            ibItemStyleResult += '<itemStyle id="' + ibItemStyleArray[index].id + '" ';
            ibItemStyleResult += 'template="' + base64encode(utf16to8(ibItemStyleArray[index].template)) + '" />';
        }
        ibItemStyleResult += '</itemStyleList>';
        
        document.getElementById("<%= IbItemStyleXMLResultClientId %>").setAttribute("value",ibItemStyleResult);
        document.getElementById("<%= IbItemXMLResultClientId %>").setAttribute("value",ibItemResult);
        document.getElementById("<%= XMLResultClientId %>").setAttribute("value",result);
        return true;
    }
    catch(e)
    {
        alert(e.name + ": " + e.message);
        return false;
    }
}

function domToString(node)
{
    var domString = '';
    
    var child = node.firstChild;
    while(child)
    {
        //nodeType为1表示为Element
        if(child.nodeType == 1)
        {
            var type = child.getAttribute("type");
            if(type)
            {
                if(type.localeCompare("page") == 0)
                {                        
                    domString += '<shelf name="';
                    domString += base64encode(utf16to8(child.getAttribute("name")));
                    domString += '">';
                    var childDomToString = domToString(child);
                    if(childDomToString)
                        domString += childDomToString;
                    else 
                        return false;
                    domString += '</shelf>';
                }
                else if(type.localeCompare("panel") == 0)
                {
                    domString += '<panel left="';
                    var left = parseInt(child.style.left);
                    domString += isNaN(left) ? 0 : left;
                    domString += '" top="';
                    var top = parseInt(child.style.top);
                    domString += isNaN(top) ? 0 : top;
                    domString += '" width="';
                    var width = parseInt(child.style.width);
                    domString += isNaN(width) ? 0 : width;
                    domString += '" height="';
                    var height = parseInt(child.style.height);
                    domString += isNaN(height) ? 0 : height;
                    domString += '" margin="';
                    var margin = parseInt(child.getAttribute("margin"));
                    domString += isNaN(margin) ? 0 : margin;
                    domString += '" cellWidth="';
                    var cellWidth = parseInt(child.getAttribute("cellWidth"));
                    domString += isNaN(cellWidth) ? 0 : cellWidth;
                    domString += '" cellHeight="';
                    var cellHeight = parseInt(child.getAttribute("cellHeight"));
                    domString += isNaN(cellHeight) ? 0 : cellHeight;
                    domString += '" titleWidth="';
                    var titleWidth = parseInt(child.getAttribute("titleWidth"));
                    domString += isNaN(titleWidth) ? 0 : titleWidth;
                    domString += '" titleHeight="';
                    var titleHeight = parseInt(child.getAttribute("titleHeight"));
                    domString += isNaN(titleHeight) ? 0 : titleHeight;
                    domString += '" ';
                    domString += 'titleBody="' + base64encode(utf16to8(child.getAttribute("titleBody"))) + '" ';                    
                    var theme = parseInt(child.getAttribute("theme"));
                    domString += 'theme="';
                    domString += isNaN(theme) ? 0 : theme;
                    domString += '">';
                    
                    var childDomToString = domToString(child);
                    if(childDomToString)
                        domString += childDomToString;
                    else
                        return false;
                    
                    domString += '</panel>';
                }
                else if(type.localeCompare("cell") == 0)
                {
                    var itemId = child.getAttribute("itemId");
                    //如果itemId没有则自动生成并加入
                    if(!itemId)
                    {
                        var templateId = child.getAttribute("templateId");
                        if(!templateId)
                        {
                            alert("Item is null in \n cell ");
                            return false;
                        }
                        var existIbItem = getIbItemByTemplateId(templateId);
                        if(existIbItem)
                        {
                            child.setAttribute("itemId",existIbItem.id);
                        }
                        else
                        {
                            var ids = templateId.split(',');
                            if(ids.length < 4)
                            {
                                alert("Item templateId is wrong");
                                return false;
                            }
                            var price = discount = state = label = 0;
                            ++ibItemIdMax;
                            var newIbItem = new ibItem(
                                ibItemIdMax,
                                ids[0],
                                ids[1],
                                ids[2],
                                ids[3],
                                price,
                                discount,
                                state,
                                label
                            );
                            ibItemArray.push(newIbItem);
                            child.setAttribute("itemId",ibItemIdMax);
                        }
                    }
                    domString += '<cell itemId="' + child.getAttribute("itemId") + '" style="' + child.getAttribute("styleId") + '" />';                    
                }
            }
        }
        
        child = child.nextSibling;
    }
    return domString;
}

function addNewPanel()
{
    if(selectObject)
    {
        var type = selectObject.getAttribute("type");
        if(type.localeCompare("page") == 0)
        {
            addPanel(selectObject,
                10,
                10,
                DefaultPanelWidth,
                DefaultPanelHeight,
                DefaultPanelMargin,
                DefaultPanelCellWidth,
                DefaultPanelCellHeight,
                DefaultPanelTitleWidth,
                DefaultPanelTitleHeight,
                DefaultPanelTitleBody,
                DefaultPanelTheme
            );
            return;
        }
    }
    alert("<%= Resources.StringDef.MsgPleaseChooseShelf %>");
}

function addCell(element,margin,width,height,styleId)
{    
    var newCell = document.createElement("div");
    newCell.setAttribute("type","cell");
    newCell.setAttribute("styleId",parseInt(styleId));
    newCell.style.cssText = cellInitCssText + "width:" + parseInt(width) + "px; height:" + parseInt(height) + "px; margin-right:" + parseInt(margin) + "px; margin-bottom:" + parseInt(margin) + "px;";
    registerMouseDown(newCell);
    registerMouseOverAndOut(newCell);
    element.appendChild(newCell);
    
    return newCell;
}

function addNewCell()
{
    if(selectObject)
    {
        try
        {
            var type = selectObject.getAttribute("type");
            if(type.localeCompare("panel") == 0)
            {
                addCell(selectObject,
                    parseInt(selectObject.getAttribute("margin")),
                    parseInt(selectObject.getAttribute("cellWidth")),
                    parseInt(selectObject.getAttribute("cellHeight")),
                    ibItemStyleArray.length > 0 ? ibItemStyleArray[0].id : -1
                );
                
                return;
            }
        }
        catch(e)
        {
            alert(e.name + e.message);
        }
    }
    alert("<%= Resources.StringDef.MsgPleaseChoosePanel %>");
}

function addPanel(element,left,top,width,height,margin,cellWidth,cellHeight,titleWidth,titleHeight,titleBody,theme)
{
    var panel = document.createElement("div");
    panel.setAttribute("type","panel");
    panel.setAttribute("cellWidth",cellWidth);
    panel.setAttribute("cellHeight",cellHeight);
    panel.setAttribute("titleWidth",titleWidth);
    panel.setAttribute("titleHeight",titleHeight);
    panel.setAttribute("titleBody",titleBody);
    panel.setAttribute("margin",margin);
    panel.setAttribute("theme",theme);
    panel.style.cssText = panelInitCssText + "left:" + left + "px; top: " + top + "px; width:" + width + "px; height:" + height + "px; padding-top:" + margin + "px;padding-left:" + margin + "px;";
    registerMouseDown(panel);
    registerMouseOverAndOut(panel);
    registerKeyDown(panel);
    
    element.appendChild(panel);
    
    //Add title
    var titleDiv = document.createElement("div");
    titleDiv.setAttribute("type","title");
    titleDiv.innerHTML = "Title";
    titleDiv.style.cssText = titleInitCssText;
    titleDiv.style.width = titleWidth;
    titleDiv.style.height = titleHeight
    titleDiv.style.marginBottom = margin;
    titleDiv.style.marginRight = margin;
    registerMouseDown(titleDiv);
    registerMouseOverAndOut(titleDiv);
    panel.appendChild(titleDiv);
    
    return panel;
}

function addNewPage()
{
    var name = window.prompt("<%= Resources.StringDef.Name %>", "NewPage");
    if(name)
    {
        var newPage = document.createElement("div");
        newPage.style.cssText = pageInitCssText;
        newPage.setAttribute("type","page");
        newPage.setAttribute("name",name);
        registerMouseDown(newPage);
        document.getElementById("pageContainer").appendChild(newPage);
        newPage.setAttribute("id","page" + pageTitleIndex);
        newPage.style.display = "none";

        var newLi = document.createElement("li");
        newLi.setAttribute("pageId","page" + pageTitleIndex);
        var textNode = document.createTextNode(name);
        newLi.appendChild(textNode);
        newLi.style.cssText = pageTitleCssText;
        newLi.attachEvent("onclick",pageClick);
        document.getElementById("pageTitle").appendChild(newLi);

        ++pageTitleIndex;
    }
}

function deleteObj(element,type)
{
    if(element && type)
    {
        if(type.localeCompare("page") == 0)
        {
            var pageTitle = document.getElementById("pageTitle");
            for(var index = 0; index != pageTitle.childNodes.length; ++index)
            {                    
                var pageId = pageTitle.childNodes[index].getAttribute("pageId");
                if(pageId)
                {
                    if(pageId.localeCompare(selectObject.getAttribute("id")) == 0)
                    {
                        pageTitle.removeChild(pageTitle.childNodes[index]);
                        element.parentElement.removeChild(element);
                        if(pageTitle.childNodes.length > 0)
                        {
                            var selectPageTitle = pageTitle.lastChild;
                            selectPage = selectPageTitle;
                            lastSelectPage = null;
                            selectPageTitle.style.background = SelectPageTitleBackgroundColor;
                            var selectPageId = selectPageTitle.getAttribute("pageId");
                            selectObject = document.getElementById(selectPageId);
                            selectObject.style.display = "";
                        }
                        else
                        {
                            selectObject = null;
                        }
                        break;
                    }
                }
            }
            return;
        }
        else if(type.localeCompare("panel") == 0)
        {
            element.parentElement.removeChild(selectObject);
            return;
        }
        else if(type.localeCompare("cell") == 0)
        {
            element.parentElement.removeChild(selectObject);
            return;
        }
    }
}

function deleteObjTotal()
{
    if(selectObject)
    {
        var confirmResult = window.confirm("<%= Resources.StringDef.MsgDeleteConfirmShort %>");
        if(!confirmResult) return;
        var type = selectObject.getAttribute("type");
        if(type)
        {
            if(type.localeCompare("page") == 0 || type.localeCompare("panel") == 0 || type.localeCompare("cell") == 0)
            {
                deleteObj(selectObject,type);
                return;
            }
        }
    }
    alert("<%= Resources.StringDef.MsgIBShopSelectObject %>");
}

window.onload = loadSettingInit;
</script>
</head>

<div style="width:1000px;overflow:auto;">
    <div id="operationContainer">
        <button id="ButtonAddPage" onclick="addNewPage();"><asp:Literal runat="server" Text="<%$ Resources:StringDef,AddShelf %>" /></button>
        <button id="ButtonAddPanel" onclick="addNewPanel();"><asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef,AddPanel %>" /></button>
        <button id="ButtonAddCell" onclick="addNewCell();"><asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef,AddCell %>" /></button>
        <button id="ButtonDelete" onclick="deleteObjTotal();"><asp:Literal runat="server" Text="<%$ Resources:StringDef,Delete %>" /></button>
        <button id="ButtonMoveUp" onclick="moveTotal('up');"><asp:Literal ID="Literal5" runat="server" Text="<%$ Resources:StringDef,MoveUp %>" /></button>
        <button id="ButtonDown" onclick="moveTotal('down');"><asp:Literal ID="Literal6" runat="server" Text="<%$ Resources:StringDef,MoveDown %>" /></button>
        <button id="ButtonMoveLeft" onclick="moveTotal('left');"><asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef,MoveLeft %>" /></button>
        <button id="ButtonMoveRight" onclick="moveTotal('right');"><asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef,MoveRight %>" /></button>
        <div id="DivOpMsg"></div>
    </div>
    <div style="float:left;">
        <input type="text" id="itemSearchText" onkeyup="itemSearch();" onkeypress="if(event.keyCode == 13)event.returnValue = false;" style="width:96px;height:12px;font-size:12px;"  />
        <div id="itemContainer" style="float:left; width:100px;height:400px;overflow:auto;border:solid 1px gray;margin-top:0px;">
        </div>
    </div>
    <div style="margin-top:0px;width:500px;">
        <ul id="pageTitle" style="margin:0px;height:0px;overflow:visible;border-left:solid 1px black;"></ul>
        <div id="pageContainer" style="position:relative; clear:left;">
        </div>
    </div>
    <div id="attributeBoard" style="position:absolute; z-index:10;top:10px;left:10px; background-color:Black; color:Yellow; border:solid 1px gray; width:300px;height:50px; overflow:visible; padding:5px; filter:alpha(opacity=85);display:none;font-size:12px;">
    </div>
</div>
<input type="hidden" runat="server" id="XMLResult" enableviewstate="false" />
<input type="hidden" runat="server" id="IbItemXMLResult" enableviewstate="false" />
<input type="hidden" runat="server" id="IbItemStyleXMLResult" enableviewstate="false" />
<table width="100%">
    <tr>
        <td align="center">
            <asp:UpdateProgress runat="server" AssociatedUpdatePanelID="UpdatePanel1">
                <ProgressTemplate>
                    <asp:Label runat="server" Text="<%$ Resources:StringDef, PleaseWaitWhileOperating %>" />
                </ProgressTemplate>
            </asp:UpdateProgress>
        </td>
    </tr>
</table>
<asp:UpdatePanel runat="server" ID="UpdatePanel1" >
    <ContentTemplate>
    <table width="100%">
        <tr>
            <td align="center">
                <asp:Label ID="LabelOpMsg" runat="server" />
            </td>
        </tr>
        <tr>
            <td align="center">
                <asp:LinkButton ID="ButtonSave" runat="server" OnClientClick="return createXmlResult();" OnClick="SaveIbConfigSetting" Text="<%$ Resources:StringDef,Save %>" />
                <asp:HyperLink runat="server" NavigateUrl="~/GameMaster/IBShopScriptList.aspx" Text="<%$ Resources:StringDef,IBShopScriptList %>" />
            </td>
        </tr>
    </table>
    </ContentTemplate>
</asp:UpdatePanel>
</asp:Content>