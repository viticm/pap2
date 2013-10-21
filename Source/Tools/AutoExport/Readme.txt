说明：
服务器自动导出策划表的工具。


第三方库：
引用了Lua51
引用了LuaInterface （是一个.Net调用lua的插件）


用法：
1. 修改AutoExport.ini最后一行，改成这种形式:  RootDir=服务器上客户端的路径
2. 运行 AutoExport.exe -step1 ，导出各个策划用表
3. 运行 AutoExport.exe -step2 ，对client\settings\AIType.tab这个表进行后期加工


建议：
在16:00锁库时执行AutoExport.exe -step1，在16:30或者更晚时（服务器上的客户端已更新）执行AutoExport.exe -step2



有问题请联系rtx：kuangsihao
