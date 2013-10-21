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
import ConfigParser
import tarfile
import filecmp
import stat
import md5

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
		self.m_nProtocol 	= 69
		self.m_nCurrentStep = 0
		self.m_nStepCount 	= 6
		self.m_nResult		= 0
		self.m_strMessage 	= ""
	def Pack(self):
		nMsgLen = len(self.m_strMessage) + 1
		strFormat = '=HHHH%ds' % nMsgLen
		cPack = struct.pack(strFormat, self.m_nCurrentStep, self.m_nStepCount, self.m_nResult, nMsgLen, self.m_strMessage)
		return KProtocol.Pack(self) + cPack

# 版本协议
class KProtocol_Version(KProtocol):
	def __init__(self):
		KProtocol.__init__(self)
		self.m_nProtocol = 86
		self.m_nMajor = 0
		self.m_nMinor = 0
		self.m_nRevision = 0
		self.m_nBuild = 0
	def Pack(self):
		cPack = struct.pack('=iiii', self.m_nMajor, self.m_nMinor, self.m_nRevision, self.m_nBuild)
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
		self.data1 = None
		self.data2 = None

class KProcess:
	def __init__(self):
		self.m_cOwner = None
	def SetOwner(self, owner):
		self.m_cOwner = owner
	def Log(self, msg):
		self.m_cOwner.Log(msg)
	def Send(self, proto):
		self.m_cOwner.Send(proto)
	def Process(self, param):
		return (0, '')

class KUpdate:
	def __init__(self, nPort):
		self.m_cClient = KClient(nPort)
	def Log(self, msg):
		log = KProtocol_Log(msg)
		print msg
		self.m_cClient.Send(log)
	def Send(self, proto):
		self.m_cClient.Send(proto)
	def Update(self, proclist):
		count = len(proclist)
		res = KProtocol_Result()
		param = KParam()
		res.m_nStepCount = len(proclist)
		res.m_nCurrentStep = 1
		for process in proclist:
			try:
				process.SetOwner(self)
				res.m_nResult, res.m_strMessage = process.Process(param)
			except Exception:
				res.m_nResult = 0
				res.m_strMessage = 'step[%d/%d] throw exception' % (res.m_nCurrentStep, res.m_nStepCount)
			# res.m_nResult, res.m_strMessage = process.Process(param)
			self.Log(res.m_strMessage)
			self.m_cClient.Send(res)
			if (res.m_nResult == 0):
				break
			else:
				res.m_nCurrentStep += 1
	
class KProcess_CheckPackFile(KProcess):
	def __init__(self, file, code):
		KProcess.__init__(self)
		self.file = file
		self.code = code
	def Process(self, param):
		path = os.path.join(os.getcwd(), TMPDIR)
		file = os.path.join(path, self.file)
		if (not os.path.exists(file)):
			return (0, '%s does not exist' % self.file)
		if (not os.path.isfile(file)):
			return (0, '%s is not a file' % self.file)
		if (not os.access(file, os.F_OK)):
			return (0, 'Failed to access %s' % self.file)
		md = MakeMd5sum(file)
		if (self.code.lower() != md.lower()):
			return (0, '%s\'s md5sum[%s] is not equal to %s' % (self.file, md, self.code))
		param.data1 = path
		param.data2 = file
		return (1, 'Check md5sum of %s OK' % self.file)

# process3: 在临时目录解压压缩包
class KProcess_Uncompress1(KProcess):
	def __init__(self):
		KProcess.__init__(self)
	# 把tar.gz文件解压到指定路径
	def Uncompress(self, zipfile, path):
		try:
			self.Log('uncompress %s to %s' % (zipfile, path))
			tar = tarfile.open(zipfile)
			file = tar.next()
			while (file):
				tar.extract(file, path)
				file = tar.next()
			tar.close()
			return True
		except Exception:
			return False
	def Process(self, param):
		tmppath = param.data1
		zipfile = param.data2
		filename = os.path.basename(zipfile)
		if (self.Uncompress(zipfile, tmppath)):
			param.data2 = os.getcwd()
			return (1, 'uncompress %s OK' % filename)
		else:
			return (0, 'failed to uncompress %s' % filename)
		
# process4: 在临时目录解压各服务器压缩包
class KProcess_Uncompress2(KProcess_Uncompress1):
	def __init__(self):
		KProcess.__init__(self)
	def SendVersion(self, inifile, param):
		proto = KProtocol_Version()
		try:
			ini = ConfigParser.ConfigParser()
			ini.read(inifile)
			proto.m_nMajor = int(ini.get('Version', 'Major'))
			proto.m_nMinor = int(ini.get('Version', 'Minor'))
			proto.m_nRevision = int(ini.get('Version', 'Revision'))
			proto.m_nBuild = int(ini.get('Version', 'Build'))
		except Exception:
			self.Log('Failed to get version from update.ini')
		#self.Send(proto)
		#self.Log('send version, major:%d, minor:%d, revision:%d,build:%d' % (proto.m_nMajor, proto.m_nMinor, proto.m_nRevision, proto.m_nBuild))
		param.version = '%d.%d.%d.%d' % (proto.m_nMajor, proto.m_nMinor, proto.m_nRevision, proto.m_nBuild)
		# return (1, 'version=%d.%d.%d.:%d' % (proto.m_nMajor, proto.m_nMinor, proto.m_nRevision, proto.m_nBuild))

	def Extract(self, inifile, tmppath, curpath):
		ini = ConfigParser.ConfigParser()
		ini.read(inifile)
		count = int(ini.get(SEC_UPDATE, 'Count'))
		createtime = ini.get('CreateInfo', 'CreateTime')
		index = 1
		filelist = []
		packlist = {}	# 保存包名对应的临时路径
		while (index <= count):
			filekey = 'File%d' % index
			pathkey = 'Path%d' % index
			fileval = ini.get(SEC_UPDATE, filekey)
			pathval = ini.get(SEC_UPDATE, pathkey)
			srcpath = None
			dstpath = pathval
			# 压缩文件
			srcfile = os.path.join(tmppath, fileval)
			size = os.path.getsize(srcfile)
			if (not os.path.isabs(dstpath)):
				dstpath = os.path.join(curpath, dstpath)
			if (not fileval in packlist.keys()):
				# 源路径
				srcpath = os.path.join(tmppath, filekey)
				if (os.path.exists(srcpath)):
					if (os.path.isdir(srcpath)):
						shutil.rmtree(srcpath)
					else:
						os.remove(srcpath)
				os.mkdir(srcpath)
				# if (not os.access(srcpath, os.F_OK)):
				#	os.mkdir(srcpath)
				if (not os.path.isdir(srcpath)):
					return (False, '%s should be a directory' % srcpath)
				elif (not os.access(srcpath, os.W_OK)):
					mode = os.stat(srcpath)[stat.ST_MODE]
					os.chmod(srcpath, mode | stat.S_IWRITE)
				# 无此压缩包时忽略
				if (os.path.exists(srcfile)):
					if (not os.path.isfile(srcfile)):
						return (False, '%s is not a file' % fileval)
					# 将压缩文件解压到临时目录
					if (not KProcess_Uncompress1.Uncompress(self, srcfile, srcpath)):
						return (False, 'failed to uncompress %s' % fileval)
					packlist[fileval] = srcpath
			else:
				srcpath = packlist[fileval]
			# 如果目标路径不存在则不更新
			if (os.path.exists(dstpath)):
				filelist += [(srcpath, dstpath, fileval, size)]
			else:
				self.Log('Pass not existing directory: %s' % dstpath)
			index += 1
		return (True, filelist, createtime)
	def Process(self, param):
		tmppath = param.data1
		curpath = param.data2
		# 分析update.ini文件
		inifile = os.path.join(tmppath, INI_UPDATE)
		if (not os.path.exists(inifile)):
			return (0, '%s not found!' % inifile)
		if (not os.path.isfile(inifile)):
			return (0, '%s is not a file!' % inifile)
		self.SendVersion(inifile, param)
		flag, obj, createtime = self.Extract(inifile, tmppath, curpath)
		if (not flag):
			return (0, obj)
		param.data1 = obj
		param.data2 = tmppath
		filelist = ''
		index = 1
		count = len(obj)
		for srcpath, dstpath, file, size in obj:
			filelist += '%s(size:%d)' % (file, size)
			if (index < count):
				filelist += '\n' 
			index += 1
		if (len(filelist) == 0):
			return (1, 'Create time of pack file: %s, uncompress OK' % createtime)
		else:
			return (1, 'Create time of pack file: %s, uncompress %s OK' % (createtime, filelist))
# process5: 比较MD5校验码
class KProcess_CheckMd5(KProcess):
	def __init__(self):
		KProcess.__init__(self)
	def Check(self, path, code):
		fileobj = file(path, 'rb')
		if (not fileobj):
			return False
		md5obj = md5.new()
		msg = fileobj.read(10240)
		while (msg):
			md5obj.update(msg)
			msg = fileobj.read(10240)
		fileobj.close()
		return code.lower() == md5obj.hexdigest().lower()
	def Process(self, param):
		tmppath = param.data2
		filename = os.path.join(tmppath, MD5SUM)
		if (not os.path.exists(filename)):
			return (0, '%s does not exists' % filename)
		elif (os.path.isdir(filename)):
			return (0, '%s should not be a directory' % filename)
		retmsg = "Check MD5 OK. "
		try:
			file = open(filename)
			if (not file):
				return (0, 'failed to open %s' % filename)
			lines = file.readlines()
			file.close()
			index = 1
			count = len(lines)
			for line in lines:
				pair = line.rstrip().split()
				if (len(pair) != 2):
					return (0, '%s is invalid' % filename)
				code = pair[0]	# 校验码
				name = pair[1]	# 文件名
				path = os.path.join(tmppath, name)
				if (not os.path.exists(path)):
					continue
				elif (not os.access(path, os.R_OK)):
					return (0, 'no permission to read %s' % name)
				elif (not self.Check(path, code)):
					return (0, 'checking %s md5sum fail' % name)
				retmsg = retmsg + "%s[%s]" % (name, code)
				if (index < count):
					retmsg += '\n'
				index += 1
		except Exception:
			return (0, 'Exception')
		return (1, retmsg)

# process6: 把解压后的文件和目录拷贝到指定目录
class KProcess_CopyFile(KProcess):
	def __init__(self):
		KProcess.__init__(self)
	def CopyDirectory(self, src, dst):
		self.Log('Copy files from %s to %s' % (src, dst))
		if (not os.access(dst, os.W_OK)):
			mode = os.stat(dst)[stat.ST_MODE]
			os.chmod(dst, mode | stat.S_IWRITE)
		filecount = 0
		for item in os.listdir(src):
			srcitem = os.path.join(src, item)
			dstitem = os.path.join(dst, item)
			if (os.path.exists(dstitem) and not os.access(dstitem, os.W_OK)):
				mode = os.stat(dstitem)[stat.ST_MODE]
				os.chmod(dstitem, stat.S_IWRITE)
			if (os.path.isdir(srcitem)):
				# copy directory
				if (not os.path.exists(dstitem)):
					os.mkdir(dstitem)
				if (not os.path.isdir(dstitem)):
					return (False, 0, '%s should be a directory' % dstitem)
				else:
					flag, count, str = self.CopyDirectory(srcitem, dstitem)
					if (not flag):
						return (False, 0, str)
					filecount += count
			elif (os.path.isfile(srcitem)):
				# copy file
				try:
					shutil.copyfile(srcitem, dstitem)
					# 如果文件名不包含'.'或者以'.so'结尾，则认为是可执行文件
					if (item.find('.') < 0 or item.rfind('.so') == len(item) - 3):
						mode = os.stat(dstitem)[stat.ST_MODE]
						os.chmod(dstitem, mode | stat.S_IEXEC | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
					if (not filecmp.cmp(srcitem, dstitem)):
						return (False, 0, '%s is not equal to %s in pack' % (dstitem, item))
				except Exception:
					return (False, 0, 'Copy file %s to %s with exception!' % (item, dstitem))
				filecount += 1
			else:
				return (False, 0, '%s in pack is corrupt' % item)
		return (True, filecount, '')
	def Process(self, param):
		filelist = param.data1
		filecount = 0;
		for item in filelist:
			srcpath = item[0]
			dstpath = item[1]
			if (not os.path.exists(dstpath)):
				# os.mkdir(dstpath)
				return (0, '%s does not exist' % dstpath)
			if (not os.path.isdir(dstpath)):
				return (0, '%s is not a directory' % dstpath)
			elif (not os.access(dstpath, os.W_OK)):
				return (0, 'no permission to write %s' % dstpath)
			flag, count, str = self.CopyDirectory(srcpath, dstpath)
			if (not flag):
				return (0, str)
			filecount += count
		return (1, 'copy %d files OK' % filecount)

# 报告版本号
class KProcess_Version(KProcess):
	def __init__(self):
		KProcess.__init__(self)
	def Process(self, param):
		return (1, 'version=%s' % param.version)
def main(argv):
	if (len(argv) != 5):
		print 'Usage: python %s <udp-port> <filename> <md5sum> <mode>' % argv[0]
		return None
	port = int(argv[1])
	file = argv[2]
	code = argv[3]
	mode = int(argv[4])	# 0=更新，1＝仅MD5校验
	
	proc1 = KProcess_CheckPackFile(file, code)
	proc2 = KProcess_Uncompress1()
	proc3 = KProcess_Uncompress2()
	proc4 = KProcess_CheckMd5()
	proc5 = KProcess_CopyFile()
	proc6 = KProcess_Version()
	proclist = []
	if (mode != 0):
		proclist += [(proc1)]
	else:
		proclist += (proc1, proc2, proc3, proc4, proc5, proc6)
	update = KUpdate(port)
	update.Log('****************Begin task[server update]****************')
	update.Update(proclist)
	update.Log('*****************End task[server update]*****************')

if (__name__ == '__main__'):
	# pdb.set_trace()
	argv = sys.argv
	# argv = ('updateserver.py', 5555, 'up.tar.gz', '35d41e76e1892cd37ffbb18b785320ba', 0)
	main(argv)
