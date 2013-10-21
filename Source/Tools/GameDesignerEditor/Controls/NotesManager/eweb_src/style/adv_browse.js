config.ButtonDir = "blue";
config.StyleUploadDir = "uploadfile/";
config.InitMode = "EDIT";
config.AutoDetectPasteFromWord = "1";
config.BaseUrl = "1";
config.BaseHref = "";
config.AutoRemote = "1";
config.ShowBorder = "0";
config.StateFlag = "1";
config.CssDir = "coolblue";
config.AutoDetectLanguage = "1";
config.DefaultLanguage = "zh-cn";
config.AllowBrowse = "1";
config.AllowImageSize = "100";
config.AllowFlashSize = "100";
config.AllowMediaSize = "100";
config.AllowFileSize = "100";
config.AllowRemoteSize = "100";
config.AllowLocalSize = "100";

function showToolbar(){

	document.write ("<table border=0 cellpadding=0 cellspacing=0 width='100%' class='Toolbar' id='eWebEditor_Toolbar'><tr><td><div class=yToolbar><DIV CLASS=TBHandle></DIV><DIV CLASS=Btn TITLE='"+lang["Image"]+"' onclick=\"showDialog('img.htm', true)\"><IMG CLASS=Ico SRC='buttonimage/blue/img.gif'></DIV><DIV CLASS=Btn TITLE='"+lang["Flash"]+"' onclick=\"showDialog('flash.htm', true)\"><IMG CLASS=Ico SRC='buttonimage/blue/flash.gif'></DIV><DIV CLASS=Btn TITLE='"+lang["Media"]+"' onclick=\"showDialog('media.htm', true)\"><IMG CLASS=Ico SRC='buttonimage/blue/media.gif'></DIV><DIV CLASS=Btn TITLE='"+lang["File"]+"' onclick=\"showDialog('file.htm', true)\"><IMG CLASS=Ico SRC='buttonimage/blue/file.gif'></DIV><DIV CLASS=TBSep></DIV><DIV CLASS=Btn TITLE='"+lang["GalleryMenu"]+"' onclick=\"showToolMenu('gallery')\"><IMG CLASS=Ico SRC='buttonimage/blue/gallerymenu.gif'></DIV><DIV CLASS=Btn TITLE='"+lang["GalleryImage"]+"' onclick=\"showDialog('browse.htm?type=image', true)\"><IMG CLASS=Ico SRC='buttonimage/blue/galleryimage.gif'></DIV><DIV CLASS=Btn TITLE='"+lang["GalleryFlash"]+"' onclick=\"showDialog('browse.htm?type=flash', true)\"><IMG CLASS=Ico SRC='buttonimage/blue/galleryflash.gif'></DIV><DIV CLASS=Btn TITLE='"+lang["GalleryMedia"]+"' onclick=\"showDialog('browse.htm?type=media', true)\"><IMG CLASS=Ico SRC='buttonimage/blue/galleryimage.gif'></DIV><DIV CLASS=Btn TITLE='"+lang["GalleryFile"]+"' onclick=\"showDialog('browse.htm?type=file', true)\"><IMG CLASS=Ico SRC='buttonimage/blue/galleryfile.gif'></DIV><DIV CLASS=TBSep></DIV><DIV CLASS=Btn TITLE='"+lang["About"]+"' onclick=\"showDialog('about.htm')\"><IMG CLASS=Ico SRC='buttonimage/blue/about.gif'></DIV></div></td></tr></table>");

}
