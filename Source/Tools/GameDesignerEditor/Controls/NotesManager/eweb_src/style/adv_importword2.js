config.ButtonDir = "blue";
config.StyleUploadDir = "uploadfile/";
config.InitMode = "EDIT";
config.AutoDetectPasteFromWord = "1";
config.BaseUrl = "1";
config.BaseHref = "";
config.AutoRemote = "1";
config.ShowBorder = "0";
config.StateFlag = "1";
config.CssDir = "importword";
config.AutoDetectLanguage = "1";
config.DefaultLanguage = "zh-cn";
config.AllowBrowse = "0";
config.AllowImageSize = "100";
config.AllowFlashSize = "100";
config.AllowMediaSize = "100";
config.AllowFileSize = "100";
config.AllowRemoteSize = "100";
config.AllowLocalSize = "100";

function showToolbar(){

	document.write ("<table border=0 cellpadding=0 cellspacing=0 width='100%' class='Toolbar' id='eWebEditor_Toolbar'><tr><td><div class=yToolbar><DIV CLASS=TBHandle></DIV><DIV CLASS=Btn TITLE='"+lang["ImportWord"]+"' onclick=\"showDialog('importword.htm', true)\"><IMG CLASS=Ico SRC='buttonimage/blue/importword.gif'></DIV><DIV CLASS=TBSep></DIV><DIV CLASS=Btn TITLE='"+lang["About"]+"' onclick=\"showDialog('about.htm')\"><IMG CLASS=Ico SRC='buttonimage/blue/about.gif'></DIV></div></td></tr></table>");

}
