#!/usr/bin/python
# -*- coding: utf-8 -*-
# -------------------------------------------------------------------------
#	file		:	guarddaemon.py
#	author		:	wangbin
#	createtime	:	2008-05-27 16:44:00
#	descript	:	guard监控守护进程，保证guard的运行和更新
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
import signal
import getopt

GUARD 		= 'guard'
UPD_PATH	= './update'
RUN_PATH	= './'

class Daemon:
	def __init__(self):
		self.system = platform.system()
		self.log = None
		self.lastmew = 0
	def BecomeDaemon(self):
		if (self.system != 'Linux'):
			return
		signal.signal(signal.SIGCHLD, signal.SIG_IGN)
		# Do first fork
		if (os.fork() > 0):
			sys.exit(0)
		# Decouple from parent environment
		# os.chdir("/")
		os.umask(0)
		os.setsid()
		# Do second fork
		if (os.fork() > 0):
			sys.exit(0)
		# close file descriptors
		os.close(sys.stdin.fileno())
		os.close(sys.stdout.fileno())
		os.close(sys.stderr.fileno())
	def Initialize(self):
		self.BecomeDaemon()
		self.log = open('guarddaemon.log', 'a')
	def Uninitialize(self):
		self.log.close()
	def Print(self, msg):
		t = time.localtime()
		log = '%s %s' % (time.strftime("[%Y-%m-%d %H:%M:%S]", t), msg)
		print log
	def Log(self, msg):
		t = time.localtime()
		log = '%s %s' % (time.strftime("[%Y-%m-%d %H:%M:%S]", t), msg)
		self.log.write('%s\n' % log)
		self.log.flush()
		print log
	def FindProgram(self, process):
		if (self.system == 'Linux'):
			return os.path.exists(process)
		else:
			return os.path.exists('%s.exe' % process)
	def FindProcess(self, process):
		output = 'findprocess.tmp'
		if (self.system == 'Linux'):
			os.system('pgrep %s > %s' % (process, output))
			return os.path.getsize(output) != 0
		else:
			os.system('tasklist /NH /FI \"IMAGENAME eq %s.exe\" >%s' % (process, output))
			log = open(output, 'r')
			msg = log.read(1024)
			log.close()
			return msg.lower().find(GUARD.lower()) >= 0
	def Update(self, srcpath, dstpath):
		if (not os.path.exists(srcpath)):
			return
		if (not os.path.exists(dstpath)):
			os.mkdir(dstpath)
		elif (not os.path.isdir(dstpath)):
			self.Log('Error: %s is not a directory' % dstpath)
			return
		elif (not os.access(srcpath, os.R_OK)):
			self.Log('Error: no permission to read from %s' % srcpath)
			return
		elif (not os.access(dstpath, os.W_OK)):
			self.Log('Error: no permission to write into %s' % dstpath)
			return
		itemlist = os.listdir(srcpath)
		for item in itemlist:
			src = os.path.join(srcpath, item)
			dst = os.path.join(dstpath, item)
			if (os.path.isdir(src)):
				self.Update(src, dst)
			elif (os.path.isfile(src)):
				try:
					if (not os.path.exists(dst) or not filecmp.cmp(src, dst)):
						shutil.copyfile(src, dst)
						mode = os.stat(dst).st_mode
						os.chmod(dst, mode | stat.S_IEXEC | stat.S_IXGRP)
						self.Log('Update %s with %s' % (dst, src))
				except Exception:
					self.Log('Failed to update %s with %s' % (dst, src))
	def CreateProcess(self, process):
		if (not self.FindProgram(process)):
			self.Log("Error: can't find program[%s]" % process)
			return
		try:
			if (self.system == 'Linux'):
				if (os.fork() == 0):
					os.execv(GUARD, (GUARD, ''))
			else:
				os.spawnl(os.P_DETACH, GUARD)
			self.Log('Create process[%s]' % process)
		except Exception:
			self.Log('Error: failed to create process[%s]' % process)
	def Work(self):
		if (not os.path.exists(UPD_PATH)):
			os.mkdir(UPD_PATH)
		elif (not os.path.isdir(UPD_PATH)):
			self.Log('Error: %s is not a directory' % UPD_PATH)
		elif (not os.access(UPD_PATH, os.R_OK)):
			self.Log('Error: no permission to read from %s' % UPD_PATH)
		try:
			if (not self.FindProcess(GUARD)):
				self.Update(UPD_PATH, RUN_PATH)
				self.CreateProcess(GUARD)
		except Exception:
			self.Log('Error: exception')
		if (self.system != 'Linux'):
			now = time.time()
			if (now - self.lastmew >= 600):
				self.Print('GuardDaemon is running ......')
				self.lastmew = now

def Install():
	bootscript = '/etc/rc.local'
	if (platform.system() != 'Linux'):
		print 'This python script should be installed in Linux'
		return False
	if (os.getuid() != 0):
		print 'This python script should be installed by root'
		return False
	cfg = open(bootscript, 'ra')
	cmd = 'python /home/kingsoft/server/guard/guarddaemon.py -p /home/kingsoft/server/guard'
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
	
def Run():
	daemon = Daemon()
	daemon.Initialize()
	while (True):
		daemon.Work()
		time.sleep(10)
	daemon.Uninitialize()

def Usage(argv):
	print 'Usage: python %s [-i|--install] [-p|--path execute path]' % argv[0]

def main(argv):
	# pdb.set_trace()
	argc = len(argv)
	if (argc == 1):
		Run()
	elif (argc == 2):
		option = argv[1]
		if (option == '-i' or option == '--install'):
			Install()
		else:
			Usage(argv)
	elif (argc == 3):
		option = argv[1]
		if (option == '-p' or option == '--path'):
			path = argv[2]
			if (not os.path.exists(path) or not os.path.isdir(path)):
				print '%s does not exists or is not a directory' % path
			else:
				os.chdir(path)
				Run()
		else:
			Usage(argv)
	else:
		Usage(argv)

if (__name__ == '__main__'):
	main(sys.argv)
