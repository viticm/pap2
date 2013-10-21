#!/usr/bin/python
# -*- coding: utf-8 -*-
# -------------------------------------------------------------------------
#	file		:	guardinstall.py
#	author		:	wangbin
#	createtime	:	2008-06-02 16:2:00
#	descript	:	guard安装脚本
#					1.把guard和相关脚本解压并拷贝到/home/kingsoft/server/guard
#					2.调用guardaemon.py的Install方法，修改/etc/rc.local脚本，使得系统重启时能够自动启动guarddaemon.py
#					3.执行guarddaemon.py（后台模式）
# -------------------------------------------------------------------------
import os
import pdb
import sys
import time
import stat
import platform
import os.path
import filecmp
import shutil
import tarfile

INSTALL_PACK 	= 'guard.tar.gz'
TARGET_PATH		= '/home/kingsoft/server/guard'

def Install():
	bootscript = '/etc/rc.local'
	if (platform.system() != 'Linux'):
		print 'This python script should be installed in Linux!'
		return False
	if (os.getuid() != 0):
		print 'This python script should be installed by root!'
		return False
	cfg = open(bootscript, 'ra')
	cmd = 'python %s/guarddaemon.py -p %s' % (TARGET_PATH, TARGET_PATH)
	found = False
	line = cfg.readline()
	while (line):
		if (line[0] != '#' and line.find(cmd) >= 0):
			found = True
			break
		line = cfg.readline()
	cfg.close()
	if (not found):
		if (not os.access(bootscript, os.W_OK)):
			mode = os.stat(bootscript)[stat.ST_MODE]
			os.chmod(bootscript, mode | stat.S_IWRITE)
		os.system('echo \'%s\' >> %s' % (cmd, bootscript))
		print 'Install OK'
	else:
		print 'This python script is already installed'
	return True

def Uncompress(zipfile, path):
	try:
		tar = tarfile.open(zipfile)
		file = tar.next()
		while (file):
			tar.extract(file, path)
			file = tar.next()
		tar.close()
		return True
	except Exception:
		return False

def Usage(argv):
	print 'Usage: python %s [target path]' % argv[0]
	
def CreatePath(root, path):
	rootpath = root
	for dir in path.split('/'):
		if (dir == ''):
			continue
		tmppath = os.path.join(rootpath, dir)
		if (not os.path.exists(tmppath)):
			os.mkdir(tmppath)
		if (not os.access(tmppath, os.W_OK | os.X_OK)):
			mode = os.stat(tmppath)[stat.ST_MODE]
			os.chmod(tmppath, mode | stat.S_IWRITE | stat.S_IEXEC)
		rootpath = tmppath
	
def main(argv):
	# pdb.set_trace()
	if (os.getuid() != 0):
		print 'This install script must be executed by root!'
		return
	argc = len(argv)
	if (argc != 1):
		Usage()
		return
	# 检查安装包是否存在
	if (not os.path.exists(INSTALL_PACK)):
		print 'Error: %s does not exists!' % INSTALL_PACK
		return
	elif (not os.access(INSTALL_PACK, os.R_OK)):
		print 'Error: %s is not readable!' % INSTALL_PACK
		return
	# 建立目标目录
	CreatePath('/', TARGET_PATH)
	# 解压到目标路径
	Uncompress(INSTALL_PACK, TARGET_PATH)
	# 检查安装文件
	filelist = ('guard', 'ftpdownload.py', 'updateserver.py', 'guarddaemon.py')
	for file in filelist:
		path = os.path.join(TARGET_PATH, file)
		if (not os.path.exists(path)):
			print '%s does not exist in install pack!' % file
			return
		# 设置文件属性为可执行
		mode = os.stat(path)[stat.ST_MODE]
		os.chmod(path, mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
	cur_path = os.getcwd()
	os.chdir(TARGET_PATH)
	if (not Install()):
		print 'Error: failed to install guarddaemon.py!'
		return
	os.system('python guarddaemon.py')
	os.chdir(cur_path)	
	print 'Install finished and guaddaemon is running now!'

if (__name__ == '__main__'):
	main(sys.argv)
