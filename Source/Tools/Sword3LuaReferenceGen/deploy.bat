@echo off
SET DST_DIR=\\192.168.27.31\xsjonline\jx3gm\jx3_lua_interface

xcopy /R /Y html_utf8\*.html  %DST_DIR%
xcopy /R /Y src\history.html %DST_DIR%
xcopy /R /Y html_gb2312\jx3_lua_doc.chm %DST_DIR%
pause