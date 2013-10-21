cd /d %1
xcopy serverlist.ini .\operation\
packfile.exe P -Poperation.pak -R%1 -Doperation