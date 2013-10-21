//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-9
//      File_base        : Control
//      File_ext         : js
//      Author           : 徐晓刚
//      Description      : 控件
//
//////////////////////////////////////////////////////////////////////

/* 控件 */
/* Tooltip 开始 */

var tooltip_element;

function setTooltip(tooltipId) {
	tooltip_element = document.all[tooltipId];
}

function showTooltip() {
    tooltip_element.style.visibility = "visible";
}

function hideTooltip() {
    tooltip_element.style.visibility = "hidden";
}

function moveTooltip() {
    tooltip_element.style.left = event.x + 10;
    tooltip_element.style.top = event.y + 10;
}
/* Tooltip 结束 */