//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 2007-7
//      File_base        : tooltip
//      File_ext         : js
//      Author           : 徐晓刚
//      Description      : Tooltip
//
//////////////////////////////////////////////////////////////////////

safariFlag = ((navigator.userAgent.indexOf("Safari")!=-1)) ? true : false;
isShowing = false;
tipPositionCount = 5;

//初始化tip，指定tip的样式
function inittip(tipStyleClass)
{
	if(!document.getElementById("tooltip"))
	{
		var tip = document.createElement("div");
		tip.id = "tooltip";
		tip.className = tipStyleClass;
		document.getElementsByTagName("body")[0].appendChild(tip);
		hidetip();
	}
}

//设置tip内容
function settip(tipContent)
{
	document.getElementById("tooltip").innerHTML = tipContent;
}

//显示tip
function showtip() {
	isShowing = true;
	document.getElementById("tooltip").style.visibility = "visible";
}

//隐藏tip
function hidetip() {
	isShowing = false;
	document.getElementById("tooltip").style.visibility = "hidden";
}

//重定位tip
function postip(callingEvent)
{
	if (isShowing)	
	{
		tipPositionCount++;
		if (tipPositionCount > 5)
		{
			tipPositionCount = 0;
			if (!callingEvent) callingEvent = window.event;
			mouseX = callingEvent.clientX;
			mouseY = callingEvent.clientY-1;
		
			ttipWindowHeight = (window.innerHeight) ? window.innerHeight : document.body.clientHeight;
			var ttipWindowScrollDistance = (!safariFlag) ? ((document.documentElement.scrollTop) ? document.documentElement.scrollTop : document.body.scrollTop ) : window.pageYOffset;
			document.getElementById("tooltip").style.left = mouseX+10+"px";
			
			var ttipOvershoot = (!safariFlag) ? document.getElementById("tooltip").offsetHeight + mouseY + 10 : document.getElementById("tooltip").offsetHeight + mouseY + 10 - window.pageYOffset;
			
			var tipPosY = 0;
			if(ttipOvershoot > ttipWindowHeight)
			{
				document.getElementById("tooltip").style.top = ttipWindowHeight - document.getElementById("tooltip").offsetHeight+ttipWindowScrollDistance+"px";
			}
			else
			{
				document.getElementById("tooltip").style.top = (!safariFlag) ? mouseY+10+ttipWindowScrollDistance+"px" : mouseY+10+"px";
			}
		}
	}
}

document.onmousemove = postip;
