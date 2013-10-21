@echo off
echo Running dump... 自动备份mysql数据库
set a="%time:~0,1%"
if %a% == " " goto morning

:night
set fp="e:\jxeyes.%DATE:~0,4%%DATE:~5,2%%DATE:~8,2%%time:~0,2%%time:~3,2%.sql" 
e:\mysql_5\bin\mysqldump --user jxeyes --password=abc --all-databases --default-character-set=gb2312 --result-file="e:\autobackup\jxeyes.%DATE:~0,4%%DATE:~5,2%%DATE:~8,2%%time:~0,2%%time:~3,2%.sql" 
set fp="e:\autobackup\jxeyes.%DATE:~0,4%%DATE:~5,2%%DATE:~8,2%%time:~0,2%%time:~3,2%.sql" 
goto end

:morning
e:\mysql_5\bin\mysqldump --user jxeyes --password=abc --all-databases --default-character-set=gb2312 --result-file="e:\autobackup\jxeyes.%DATE:~0,4%%DATE:~5,2%%DATE:~8,2%0%time:~1,1%%time:~3,2%.sql" 
set fp="e:\autobackup\jxeyes.%DATE:~0,4%%DATE:~5,2%%DATE:~8,2%0%time:~1,1%%time:~3,2%.sql" 
goto end

:end
echo AutoCreateBackup "%fp%"
echo Done! 