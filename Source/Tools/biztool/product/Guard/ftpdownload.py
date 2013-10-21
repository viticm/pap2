#!/usr/bin/python
# -*- coding: utf-8 -*-
# -------------------------------------------------------------------------
#	file		:	updateserver.py
#	author		:	wangbin
#	createtime	:	2008-02-26 17:11:00
#	descript	:	服务器自动更新
# -------------------------------------------------------------------------
import pdb
import struct
import socket
import ftplib
import shutil
import os.path
import os
import sys
import stat
import md5
import time

TMPDIR		= 'tmp'
INI_UPDATE	= 'update.ini'
SEC_UPDATE	= 'Update'
MD5SUM		= 'md5sum'

def MakeMd5sum(path):
	f = file(path, 'rb')
	m = md5.new()
	msg = f.read(10240)
	while (msg):
		m.update(msg)
		msg = f.read(10240)
	f.close()
	return m.hexdigest()

# 下载回调接口
class KDownloadCallback:
	def __init__(self, file, size, param):
		self.file = file
		self.cursize = 0
		self.maxsize = size
		self.percent = 0;
		self.param = param
	def Process(self, data):
		self.file.write(data)
		self.cursize += len(data)
		percent = self.cursize * 100 / self.maxsize
		if (percent != self.percent and percent != 100):
			self.param.res.m_nProgress = percent
			self.param.res.m_nResult = 1
			self.param.client.Send(self.param.res)
			print 'Ftp download percent:%d%%' % percent
			self.percent = percent
	
# 协议类基类
class KProtocol:
	def __init__(self):
		self.m_nProtocol = 0
		self.m_nServer = 0
	def Pack(self):
		return struct.pack("=HH", self.m_nProtocol, self.m_nServer)

# 日志协议
class KProtocol_Log(KProtocol):
	def __init__(self, msg):
		KProtocol.__init__(self)
		self.m_nProtocol = 80
		self.m_strMsg = msg
	def Pack(self):
		nMsgLen = len(self.m_strMsg) + 1
		strFormat = '=H%ds' % nMsgLen
		cPack = struct.pack(strFormat, nMsgLen, self.m_strMsg)
		return KProtocol.Pack(self) + cPack

# 更新结果协议类
class KProtocol_Result(KProtocol):
	def __init__(self):
		KProtocol.__init__(self)
		self.m_nProtocol 	= 78
		self.m_nProgress 	= 0
		self.m_nResult		= 0
		self.m_strMessage 	= ""
	def Pack(self):
		nMsgLen = len(self.m_strMessage) + 1
		strFormat = '=BBH%ds' % nMsgLen
		cPack = struct.pack(strFormat, self.m_nProgress, self.m_nResult, nMsgLen, self.m_strMessage)
		return KProtocol.Pack(self) + cPack

class KClient:
	def __init__(self, nPort):
		self.m_cSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.m_sAddr = ('127.0.0.1', nPort)
	def __del__(self):
		self.m_cSock.close()
	def Send(self, cProtocol):
		self.m_cSock.sendto(cProtocol.Pack(), 0, self.m_sAddr)

class KParam:
	def __init__(self):
		self.client = None
		self.res = None
		self.data1 = None
		self.data2 = None

class KProcess:
	def __init__(self):
		self.m_cOwner = None
	def SetOwner(self, owner):
		self.m_cOwner = owner
	def Log(self, msg):
		self.m_cOwner.Log(msg)
	def Process(self, param):
		return (0, '')

class KUpdate:
	def __init__(self, nPort):
		self.m_cClient = KClient(nPort)
	def Log(self, msg):
		log = KProtocol_Log(msg)
		print msg
		self.m_cClient.Send(log)
	def Update(self, proclist):
		count = len(proclist)
		param = KParam()
		param.client = self.m_cClient
		param.res = KProtocol_Result()
		param.res.m_nProgress = 0
		param.res.m_nResult = 1
		step = 1
		for process in proclist:
			try:
				process.SetOwner(self)
				param.res.m_nResult, param.res.m_strMessage = process.Process(param)
			except Exception:
				param.res.m_nResult = 0
				param.res.m_strMessage = 'step[%d/%d] throw exception' % (step, len(proclist))
			step += 1
			self.m_cClient.Send(param.res)
			self.Log(param.res.m_strMessage)
			if (param.res.m_nResult == 0):
				break

# process1: 创建或者清理临时目录
class KProcess_Prepare(KProcess):
	def __init__(self):
		KProcess.__init__(self)
	def MakeDir(self, path):
		if (os.path.exists(path)):
			if (os.path.isdir(path)):
				shutil.rmtree(path)
			else:
				os.remove(path)
		os.mkdir(path)
	def ClearDirectory(self, path):
		for root, dirs, files in os.walk(path, False):
			for name in files:
				path = os.path.join(root, name)
				if (not os.access(path, os.W_OK)):
					os.chmod(path, stat.S_IWRITE)
				os.remove(path)
			for name in dirs:
				path = os.path.join(root, name)
				if (not os.access(path, os.W_OK)):
					os.chmod(path, stat.S_IWRITE)
		for root, dirs, files in os.walk(path, False):
			for name in files:
				path = os.path.join(root, name)
				os.remove(path)
			for name in dirs:
				path = os.path.join(root, name)
				shutil.rmtree(path)
	def Process(self, param):
		tmppath = os.path.join(os.getcwd(), TMPDIR)
		if (not os.path.exists(tmppath)):
			self.MakeDir(tmppath)
		elif (not os.path.isdir(tmppath)):
			return (0, '%s is not a directory' % tmppath)
		elif (not os.access(tmppath, os.W_OK)):
			return (0, '%s is not writable' % tmppath)
		# else:
		# 	self.ClearDirectory(tmppath)
		param.data1 = tmppath
		return (1, 'clean tmp directory OK')
	
# process2: 从FTP下载压缩包
class KProcess_Download(KProcess):
	def __init__(self, strHost, strPath, strUser, strPass):
		KProcess.__init__(self)
		self.m_strHost = strHost
		self.m_strPath = strPath
		self.m_strUser = strUser
		self.m_strPass = strPass
		self.m_bPassive = True
		self.m_cParam = None
	def Download(self, path):
		ftp = ftplib.FTP(self.m_strHost)
		ftp.login(self.m_strUser, self.m_strPass)
		ftp.set_pasv(self.m_bPassive)
		filename = os.path.join(path, os.path.basename(self.m_strPath))
		file = open(filename, 'wb')
		size = ftp.size(self.m_strPath)
		
		self.Log('Begin download from ftp://%s%s ......' % (self.m_strHost, self.m_strPath))
		cb = KDownloadCallback(file, size, self.m_cParam)
		ftp.retrbinary('RETR ' + self.m_strPath, cb.Process)
		self.m_cParam.res.m_nProgress = 100
		self.m_cParam.res.m_nResult = 1
		self.m_cParam.res.m_strMessage = MakeMd5sum(filename)
		self.m_cParam.client.Send(self.m_cParam.res)

		ftp.close()
		file.close()
		return filename
	def Process(self, param):
		# pass file path to next process
		ftpfile = 'ftp://%s/%s' % (self.m_strHost, self.m_strPath)
		tmppath = param.data1
		self.m_cParam = param
		try:
			param.data2 = self.Download(tmppath)
		except Exception:
			try:
				self.m_bPassive = not self.m_bPassive
				param.data2 = self.Download(tmppath)
			except Exception:
				return (0, 'failed to download %s' % ftpfile)
		return (1, 'download %s OK' % ftpfile)

def main(argv):
	# pdb.set_trace()
	if (len(argv) != 6):
		print 'Usage: python %s <udp-port> <ftp-host> <ftp-path> <ftp-user> <ftp-password>' % argv[0]
		return None
	port = int(argv[1])
	host = argv[2]
	path = argv[3]
	user = argv[4]
	pawd = argv[5]
	
	proc1 = KProcess_Prepare()
	proc2 = KProcess_Download(host, path, user, pawd)
	proclist = (proc1, proc2)
	update = KUpdate(port)
	update.Log('****************Begin task[ftp download]****************')
	update.Update(proclist)
	update.Log('*****************End task[ftp download]*****************')

if (__name__ == '__main__'):
	argv = sys.argv
	main(argv)
