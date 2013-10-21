/*
*######################################
* eWebEditor v4.00 - Advanced online web based WYSIWYG HTML editor.
* Copyright (c) 2003-2007 eWebSoft.com
*
* For further information go to http://www.ewebsoft.com/
* This copyright notice MUST stay intact for use.
*######################################
*/

var lang=new Object();
var AvailableLangs=new Object();
AvailableLangs["en"]=true;
AvailableLangs["zh-cn"]=true;
AvailableLangs["zh-tw"]=true;
AvailableLangs.GetActiveLanguage=function ()
{
	if(config.AutoDetectLanguage=="1")
	{
		var sUserLang=navigator.userLanguage.toLowerCase();
		if(this[sUserLang])
		{
			return sUserLang;
			
		}else if(sUserLang.length>2)
		{
			sUserLang=sUserLang.substr(0,2);
			if(this[sUserLang])
			{
				return sUserLang;
				
			}
		}
	}return config.DefaultLanguage;
	
};
AvailableLangs["Active"]=AvailableLangs.GetActiveLanguage();
document.write('<script src="language/'+AvailableLangs["Active"]+'.js" type="text/javascript"><\/script>');
AvailableLangs.TranslatePage=function (targetDocument)
{
	var aInputs=targetDocument.getElementsByTagName("INPUT");
	for(i=0;i<aInputs.length;i++)
	{
		if(aInputs[i].lang)
		{
			aInputs[i].value=lang[aInputs[i].lang];
			
		}
	}var aSpans=targetDocument.getElementsByTagName("SPAN");
	for(i=0;i<aSpans.length;i++)
	{
		if(aSpans[i].lang)
		{
			aSpans[i].innerText=lang[aSpans[i].lang];
			
		}
	}var aOptions=targetDocument.getElementsByTagName("OPTION");
	for(i=0;i<aOptions.length;i++)
	{
		if(aOptions[i].lang)
		{
			aOptions[i].innerText=lang[aOptions[i].lang];
			
		}
	}
};
