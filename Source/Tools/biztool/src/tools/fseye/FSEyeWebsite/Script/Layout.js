//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-8
//      File_base        : Layout
//      File_ext         : js
//      Author           : 徐晓刚
//      Description      : 排版
//
//////////////////////////////////////////////////////////////////////

/* 排版 */

var LAYOUT_MODE_VERTICAL = 1;
var LAYOUT_MODE_HORIZONTAL = 2;
var LAYOUT_MODE_OVERLAP = 3;

var layout_divList = [];
var layout_layoutModeList = [];
var layout_spacingList = [];
var layout_baseElement;

/* 设置排版基准元素 */
function layoutSetBase(parentId) {
	layout_baseElement = document.all[parentId];
}

/* 添加排版元素 */
function layoutAdd(divId, layoutMode, spacing) {
	layout_divList[layout_divList.length] = divId;
	layout_layoutModeList[layout_layoutModeList.length] = layoutMode;
	layout_spacingList[layout_spacingList.length] = spacing;
	var element = document.all[divId];
	element.runtimeStyle.position = "absolute";
}

/* 重新排版 */
function layoutDo() {
	var baseTop = calculateOffset(layout_baseElement, "offsetTop");
	var baseLeft = calculateOffset(layout_baseElement, "offsetLeft");
	var top = baseTop;
	var left = baseLeft;
	var last_top = top;
	var last_left = left;
	var last_element;
	
	for(var i = 0; i < layout_divList.length; i++) {
		var element = document.all[layout_divList[i]];				
		var mode = layout_layoutModeList[i];
		var spacing = layout_spacingList[i];
		var height = element.offsetHeight;
		var width = element.offsetWidth;
			
		if (mode == LAYOUT_MODE_VERTICAL) {	
			if (i < layout_layoutModeList.length - 1 && layout_layoutModeList[i + 1] == LAYOUT_MODE_OVERLAP) {
				var backHeight = document.all[layout_divList[i + 1]].offsetHeight;
				var backWidth = document.all[layout_divList[i + 1]].offsetWidth;
				
				if (backHeight > height) {
					height = backHeight;
				}
				if (backWidth > width) {
					width = backWidth;
				}
			}
			
			left = baseLeft;
			top += spacing;
			element.runtimeStyle.left = left;
			element.runtimeStyle.top = top;
			top += height;
			left += width;
		}
		else if (mode == LAYOUT_MODE_HORIZONTAL) {
			left += spacing;
			element.runtimeStyle.top = last_top;
			element.runtimeStyle.left = left;
			var newTop = last_top + height;
			if (newTop > top) {
				top = newTop;
			}
			left += width;
		}
		else if (mode == LAYOUT_MODE_OVERLAP) {
			element.runtimeStyle.top = last_top;
			element.runtimeStyle.left = last_left;
		}
		
		last_left = element.offsetLeft;
		last_top = element.offsetTop;
		last_element = element;
	}
	
	layout_baseElement.runtimeStyle.height = top - baseTop;
}

/* 计算元素在页面上的偏移 */
function calculateOffset(element, attribute) {
	var offset = 0;		
	while(element) {
		offset += element[attribute];
		element = element.offsetParent;
	}
	return offset;
}