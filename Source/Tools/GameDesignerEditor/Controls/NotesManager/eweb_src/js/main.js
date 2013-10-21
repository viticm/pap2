/*
*######################################
* eWebEditor v4.00 - Advanced online web based WYSIWYG HTML editor.
* Copyright (c) 2003-2007 eWebSoft.com
*
* For further information go to http://www.ewebsoft.com/
* This copyright notice MUST stay intact for use.
*######################################
*/

var sEditorRootPath=document.location.pathname;
sEditorRootPath=sEditorRootPath.substr(0,sEditorRootPath.length-15);
config.StyleMenuHeader="<head>"+"<link href='"+sEditorRootPath+"/language/"+AvailableLangs["Active"]+".css' type='text/css' rel='stylesheet'>"+"<link href='"+sEditorRootPath+"/css/"+config.CssDir+"/menuarea.css' type='text/css' rel='stylesheet'>"+"</head>"+"<body scroll='no' onConTextMenu='event.returnValue=false;'>";
var sBaseHref="";
if(config.BaseHref!="")
{
	sBaseHref="<base href='"+document.location.protocol+"//"+document.location.host+config.BaseHref+"'>";
	
}
config.StyleEditorHeader1="<head>"+"<link href='"+sEditorRootPath+"/css/"+config.CssDir+"/editorarea.css' type='text/css' rel='stylesheet'>"+"<link href='"+sEditorRootPath+"/css/"+config.CssDir+"/editorarea."+AvailableLangs["Active"]+".css' type='text/css' rel='stylesheet'>";
config.StyleEditorHeader2=sBaseHref+"</head><body MONOSPACE>";
if(sExtCSS)
{
	sExtCSS="<link href='"+relative2fullpath(sExtCSS)+"' type='text/css' rel='stylesheet'>";
	
}else 
{
	sExtCSS="";
	
}
document.write("<title>eWebEditor</title>");
document.write("<link href='language/"+AvailableLangs["Active"]+".css' type='text/css' rel='stylesheet'>");
document.write("<link href='css/"+config.CssDir+"/editor.css' type='text/css' rel='stylesheet'>");
document.write("<script type='text/javascript' src='js/editor.js'><\/script>");
document.write("<script type='text/javascript' src='js/table.js'><\/script>");
document.write("<script type='text/javascript' src='js/menu.js'><\/script>");
document.write("</head>");
document.write("<body SCROLLING=no onConTextMenu='event.returnValue=false;'>");
document.write("<script type='text/javascript' src='js/show.js'><\/script>");
document.write("</body></html>");
function relative2fullpath(url)
{
	if(url.indexOf("://")>=0)
	{
		return url;
		
	}if(url.substr(0,1)=="/")
	{
		return url;
		
	}var sPath=sEditorRootPath;
	while(url.substr(0,3)=="../")
	{
		url=url.substr(3);
		sPath=sPath.substring(0,sPath.lastIndexOf("/"));
		
	}return sPath+"/"+url;
	
}