			剑侠世界Guard (For Linux)安装说明

[环境要求]
1、Linux系统已安装python软件
2、/etc/selinux/config文件中SELINUX配置为
SELINUX=disabled
3、确保有/home/kingsoft目录
4、确保tar软件安装
5、确保系统没有guard以及guarddeamon.py程序正在运行中！如果有，请先kill掉,才能安装。


[安装步骤]
1、使用root登陆linux
2、拷贝guard.tar.gz, guardinstall.py至临时目录下
3、在linux控制台输入以下指令：
	python guardinstall 
4、出现如下信息即表示安装及执行guard成功！
5、删除临时目录
举例：
[root@linuxpc douhao] python guardinstall.py
This python script is already installed
Install finished and guaddaemon is running now!


[其它]
本安装包，安装后将guard软件安装至/home/kingsoft/server/guard目录下，
并自动运行guard以及guarddaemon.py程序
并自动设定guard为系统每次重启必自动运行的后台程序。

							窦昊
						2008年6月3日
