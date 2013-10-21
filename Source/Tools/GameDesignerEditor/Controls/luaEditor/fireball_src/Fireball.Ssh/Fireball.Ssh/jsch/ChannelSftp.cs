#region Licences
//    Copyright (C) 2005  Sebastian Faltoni <sebastian@dotnetfireball.net>
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#endregion Licences

using System;
using System.Net;
using System.IO;
using System.Collections;
using System.Threading;
using System.Runtime.CompilerServices;
using Fireball.Streams;

namespace Fireball.Ssh.jsch
{
	/* -*-mode:java; c-basic-offset:2; -*- */
	/*
	Copyright (c) 2002,2003,2004 ymnk, JCraft,Inc. All rights reserved.

	Redistribution and use ins source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	  1. Redistributions of source code must retain the above copyright notice,
		 this list of conditions and the following disclaimer.

	  2. Redistributions ins binary form must reproduce the above copyright 
		 notice, this list of conditions and the following disclaimer ins 
		 the documentation and/or other materials provided with the distribution.

	  3. The names of the authors may not be used to endorse or promote products
		 derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL JCRAFT,
	INC. OR ANY CONTRIBUTORS TO THIS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
	OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
	EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	*/

	public class ChannelSftp : ChannelSession
	{
		private const byte  SSH_FXP_INIT=               1;
		private const byte  SSH_FXP_VERSION=            2;
		private const byte  SSH_FXP_OPEN=               3;
		private const byte  SSH_FXP_CLOSE=              4;
		private const byte  SSH_FXP_READ=               5;
		private const byte  SSH_FXP_WRITE=              6;
		private const byte  SSH_FXP_LSTAT=              7;
		private const byte  SSH_FXP_FSTAT=              8;
		private const byte  SSH_FXP_SETSTAT=            9;
		private const byte  SSH_FXP_FSETSTAT=          10;
		private const byte  SSH_FXP_OPENDIR=           11;
		private const byte  SSH_FXP_READDIR=           12;
		private const byte  SSH_FXP_REMOVE=            13;
		private const byte  SSH_FXP_MKDIR=             14;
		private const byte  SSH_FXP_RMDIR=             15;
		private const byte  SSH_FXP_REALPATH=          16;
		private const byte  SSH_FXP_STAT=              17;
		private const byte  SSH_FXP_RENAME=            18;
		private const byte  SSH_FXP_READLINK=          19;
		private const byte  SSH_FXP_SYMLINK=           20;
		private const byte  SSH_FXP_STATUS=           101;
		private const byte  SSH_FXP_HANDLE=           102;
		private const byte  SSH_FXP_DATA=             103;
		private const byte  SSH_FXP_NAME=             104;
		private const byte  SSH_FXP_ATTRS=            105;
		private const byte  SSH_FXP_EXTENDED=         (byte)200;
		private const byte  SSH_FXP_EXTENDED_REPLY=   (byte)201;

		// pflags
		private const int SSH_FXF_READ=           0x00000001;
		private const int SSH_FXF_WRITE=          0x00000002;
		private const int SSH_FXF_APPEND=         0x00000004;
		private const int SSH_FXF_CREAT=          0x00000008;
		private const int SSH_FXF_TRUNC=          0x00000010;
		private const int SSH_FXF_EXCL=           0x00000020;

		private const int SSH_FILEXFER_ATTR_SIZE=         0x00000001;
		private const int SSH_FILEXFER_ATTR_UIDGID=       0x00000002;
		private const int SSH_FILEXFER_ATTR_PERMISSIONS=  0x00000004;
		private const int SSH_FILEXFER_ATTR_ACMODTIME=    0x00000008;
		private const uint SSH_FILEXFER_ATTR_EXTENDED=     0x80000000;

		public  const int SSH_FX_OK=                            0;
		public  const int SSH_FX_EOF=                           1;
		public  const int SSH_FX_NO_SUCH_FILE=                  2;
		public  const int SSH_FX_PERMISSION_DENIED=             3;
		public  const int SSH_FX_FAILURE=                       4;
		public  const int SSH_FX_BAD_MESSAGE=                   5;
		public  const int SSH_FX_NO_CONNECTION=                 6;
		public  const int SSH_FX_CONNECTION_LOST=               7;
		public  const int SSH_FX_OP_UNSUPPORTED=                8;
		/*
		   SSH_FX_OK
			  Indicates successful completion of the operation.
		   SSH_FX_EOF
			 indicates end-of-file condition; for SSH_FX_READ it means that no
			   more data is available in the file, and for SSH_FX_READDIR it
			  indicates that no more files are contained in the directory.
		   SSH_FX_NO_SUCH_FILE
			  is returned when a reference is made to a file which should exist
			  but doesn't.
		   SSH_FX_PERMISSION_DENIED
			  is returned when the authenticated user does not have sufficient
			  permissions to perform the operation.
		   SSH_FX_FAILURE
			  is a generic catch-all error message; it should be returned if an
			  error occurs for which there is no more specific error code
			  defined.
		   SSH_FX_BAD_MESSAGE
			  may be returned if a badly formatted packet or protocol
			  incompatibility is detected.
		   SSH_FX_NO_CONNECTION
			  is a pseudo-error which indicates that the client has no
			  connection to the server (it can only be generated locally by the
			  client, and MUST NOT be returned by servers).
		   SSH_FX_CONNECTION_LOST
			  is a pseudo-error which indicates that the connection to the
			  server has been lost (it can only be generated locally by the
			  client, and MUST NOT be returned by servers).
		   SSH_FX_OP_UNSUPPORTED
			  indicates that an attempt was made to perform an operation which
			  is not supported for the server (it may be generated locally by
			  the client if e.g.  the version number exchange indicates that a
			  required feature is not supported by the server, or it may be
			  returned by the server if the server does not implement an
			  operation).
		*/

		public  const int OVERWRITE=0;
		public  const int RESUME=1;
		public  const int APPEND=2;

		//  private bool interactive=true;
		private bool interactive=false;
		private int count=1;
		private Buffer buf=null;
		private Packet packet;

		private String _version="3";
		private int server_version=3;
		/*
		10. Changes from previous protocol versions
		  The SSH File Transfer Protocol has changed over time, before it's
		   standardization.  The following is a description of the incompatible
		   changes between different versions.
		10.1 Changes between versions 3 and 2
		   o  The SSH_FXP_READLINK and SSH_FXP_SYMLINK messages were added.
		   o  The SSH_FXP_EXTENDED and SSH_FXP_EXTENDED_REPLY messages were added.
		   o  The SSH_FXP_STATUS message was changed to include fields `error
			  message' and `language tag'.
		10.2 Changes between versions 2 and 1
		   o  The SSH_FXP_RENAME message was added.
		10.3 Changes between versions 1 and 0
		   o  Implementation changes, no actual protocol changes.
		*/

		private String file_separator=Path.DirectorySeparatorChar.ToString();
		private char file_separatorc=Path.DirectorySeparatorChar;

		private String cwd;
		private String home;
		private String lcwd;

		public ChannelSftp()
		{
			packet=new Packet(buf);
		}

		public override void init()
		{
			/*
			io.setInputStream(session.in);
			io.setOutputStream(session.out);
			*/
		}

		public override void start() 
		{
			try
			{
				PipedOutputStream pos=new PipedOutputStream();
				io.setOutputStream(pos);
				//      PipedInputStream pis=new PipedInputStream(pos);
				PipedInputStream pis=new MyPipedInputStream(pos, 32*1024);
				io.setInputStream(pis);

				Request request=new RequestSftp();
				request.request(session, this);

				//    thread=Thread.currentThread();

				//      buf=new Buffer();
				buf=new Buffer(rmpsize);
				packet=new Packet(buf);
				int i=0;
				int j=0;
				int length;
				int type;
				byte[] str;

				// send SSH_FXP_INIT
				sendINIT();

				// receive SSH_FXP_VERSION
				buf.rewind();
				i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				//System.out.println(io+" "+io.ins+" "+io.out);
				length=buf.getInt();
				type=buf.getByte();           // 2 -> SSH_FXP_VERSION
				server_version=buf.getInt();
				//System.out.println("SFTP protocol server-version="+server_version);

				// send SSH_FXP_REALPATH
				sendREALPATH(Util.getBytes("."));

				// receive SSH_FXP_NAME
				buf.rewind();
				i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				length=buf.getInt();
				type=buf.getByte();          // 104 -> SSH_FXP_NAME
				buf.getInt();                //
				i=buf.getInt();              // count
				str=buf.getString();         // filename
				home=cwd=Util.getString(str);
				str=buf.getString();         // logname
				//    SftpATTRS.getATTR(buf);           // attrs

				lcwd=Path.GetFullPath(".");

				//thread=new Thread(this);
				//thread.setName("Sftp for "+session.host);
				//thread.start();
			}
			catch(Exception e)
			{
				//System.out.println(e);
				if(e is JSchException) throw (JSchException)e;
				throw new JSchException(e.ToString());
			}
		}

		public void quit(){ disconnect();}
		public void exit(){ disconnect();}
		public void lcd(String path) 
		{
			//    if(!path.StartsWith("/")){ path=lcwd+file_separator+path; }
			if(!isLocalAbsolutePath(path)){ path=lcwd+file_separator+path; }
			if(Directory.Exists(path))
			{
				try
				{
					path=Path.GetFullPath(path);
				}
				catch(Exception e){}
				lcwd=path;
				return;
			}
			throw new SftpException(SSH_FX_NO_SUCH_FILE, "No such directory");
		}

		/*
			cd /tmp
			c->s REALPATH
			s->c NAME
			c->s STAT
			s->c ATTR 
		*/
		public void cd(String path) 
		{
			try
			{
				if(!path.StartsWith("/")){ path=cwd+"/"+path; }

				ArrayList v=glob_remote(path);
				if(v.Count!=1)
				{
					throw new SftpException(SSH_FX_FAILURE, v.ToString());
				}
				path=(String)(v[0]);

				sendREALPATH(Util.getBytes(path));

				buf.rewind();
				int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				int length=buf.getInt();
				int type=buf.getByte();
				if(type!=101 && type!=104)
				{
					throw new SftpException(SSH_FX_FAILURE, "");
				}
				if(type==101)
				{
					buf.getInt();
					i=buf.getInt();
					throwStatusError(buf, i);
					//	byte[] str=buf.getString();
					//	throw new SftpException(i, Util.getString(str));
				}
				buf.getInt();
				i=buf.getInt();
				byte[] str=buf.getString();
				if(str!=null && str[0]!='/')
				{
					str=Util.getBytes(cwd+"/"+Util.getString(str));
				}
				cwd=Util.getString(str);
				str=buf.getString();         // logname
				i=buf.getInt();              // attrs
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}

		/*
			put foo
			c->s OPEN
			s->c HANDLE
			c->s WRITE
			s->c STATUS
			c->s CLOSE 
			s->c STATUS 
		*/
		public void put(String src, String dst) 
		{
			put(src, dst, null, OVERWRITE);
		}
		public void put(String src, String dst, int mode) 
		{
			put(src, dst, null, mode);
		}
		public void put(String src, String dst, 
			SftpProgressMonitor monitor) 
		{
			put(src, dst, monitor, OVERWRITE);
		}
		public void put(String src, String dst, 
			SftpProgressMonitor monitor, int mode) 
		{
			//    if(!src.StartsWith("/")){ src=lcwd+file_separator+src; } 
			if(!isLocalAbsolutePath(src)){ src=lcwd+file_separator+src; } 
			if(!dst.StartsWith("/")){ dst=cwd+"/"+dst; }
			//System.out.println("src: "+src+", "+dst);
			try
			{
				ArrayList v=glob_remote(dst);
				if(v.Count!=1)
				{
					throw new SftpException(SSH_FX_FAILURE, v.ToString());
				}
				dst=(String)(v[0]);

				bool _isRemoteDir=isRemoteDir(dst);

				v=glob_local(src);
				//System.out.println("glob_local: "+v+" dst="+dst);
				for(int j=0; j<v.Count; j++)
				{
					String _src=(String)(v[j]);
					String _dst=dst;
					if(_isRemoteDir)
					{
						if(!_dst.EndsWith("/"))
						{
							_dst+="/";
						}
						int i=_src.LastIndexOf(file_separatorc);
						if(i==-1) _dst+=_src;
						else _dst+=_src.Substring(i+1);
					}
					//System.out.println("_dst "+_dst);
					long size_of_dst=0;
					if(mode==RESUME)
					{
						try
						{
							SftpATTRS attr=stat(_dst);
							size_of_dst=attr.getSize();
						}
						catch(Exception eee)
						{
							//System.out.println(eee);
						}
						long size_of_src=new FileInfo(_src).Length;
						if(size_of_src<size_of_dst)
						{
							throw new SftpException(SSH_FX_FAILURE, "failed to resume for "+_dst);
						}
						if(size_of_src==size_of_dst)
						{
							return;
						}
					}

					if(monitor!=null)
					{
						monitor.init(SftpProgressMonitor.PUT, _src, _dst,
							(new FileInfo(_src)).Length);
						if(mode==RESUME)
						{
							monitor.count(size_of_dst);
						}
					}
					FileStream fis=null;
					try
					{
						fis=File.OpenRead(_src);
						put(fis, _dst, monitor, mode);
					}
					finally
					{
						if(fis!=null) 
						{
							//	    try{
							fis.Close();
							//	    }catch(Exception ee){};
						}
					}
				}
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, e.ToString());
			}
		}
		public void put(Stream src, String dst) 
		{
			put(src, dst, null, OVERWRITE);
		}
		public void put(Stream src, String dst, int mode) 
		{
			put(src, dst, null, mode);
		}
		public void put(Stream src, String dst, 
			SftpProgressMonitor monitor) 
		{
			put(src, dst, monitor, OVERWRITE);
		}
		public void put(Stream src, String dst, 
			SftpProgressMonitor monitor, int mode) 
		{
			try
			{
				if(!dst.StartsWith("/")){ dst=cwd+"/"+dst; } 
				ArrayList v=glob_remote(dst);
				if(v.Count!=1)
				{
					throw new SftpException(SSH_FX_FAILURE, v.ToString());
				}
				dst=(String)(v[0]);

				if(isRemoteDir(dst))
				{
					throw new SftpException(SSH_FX_FAILURE, dst+" is a directory");
				}
				long skip=0;
				if(mode==RESUME || mode==APPEND)
				{
					try
					{
						SftpATTRS attr=stat(dst);
						skip=attr.getSize();
					}
					catch(Exception eee)
					{
						//System.out.println(eee);
					}
				}
				if(mode==RESUME && skip>0)
				{
					long skipped=src.Seek(skip, SeekOrigin.Current);
					if(skipped<skip)
					{
						throw new SftpException(SSH_FX_FAILURE, "failed to resume for "+dst);
					}
				}

				if(mode==OVERWRITE)
				{
					sendOPENW(Util.getBytes(dst));
				}
				else
				{
					sendOPENA(Util.getBytes(dst));
				}

				buf.rewind();
				int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				int length=buf.getInt();
				int type=buf.getByte();
				if(type!=SSH_FXP_STATUS && type!=SSH_FXP_HANDLE)
				{
					throw new SftpException(SSH_FX_FAILURE, "");
				}
				if(type==SSH_FXP_STATUS)
				{
					buf.getInt();
					i=buf.getInt();
					throwStatusError(buf, i);
				}
				buf.getInt();
				byte[] handle=buf.getString();         // filename
				byte[] data=new byte[buf.buffer.Length-1024];
				long offset=0;
				if(mode==RESUME || mode==APPEND)
				{
					offset+=skip;
				}
				while(true)
				{
					//i=src.read(data, 0, data.Length);

					i=0;
					int nread=0;
					do
					{
						nread=src.Read(data, i, data.Length-i);
						if(nread>0)
						{
							i+=nread;
						}
					}
					while(i<data.Length && nread>0); 
					if(i<=0)break;
					sendWRITE(handle, offset, data, 0, i);
					offset+=i;
					if(!checkStatus()){ break; }
					if(monitor!=null && !monitor.count(i))
					{
						break;
					}
				}
				if(monitor!=null)monitor.end();
				_sendCLOSE(handle);
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}
		/**/
		public Stream put(String dst) 
		{
			return put(dst, (SftpProgressMonitor)null, OVERWRITE);
		}
		public Stream put(String dst, int mode) 
		{
			return put(dst, (SftpProgressMonitor)null, mode);
		}
		public Stream put(String dst,  SftpProgressMonitor monitor, int mode) 
		{
			if(!dst.StartsWith("/")){ dst=cwd+"/"+dst; } 
			try
			{
				ArrayList v=glob_remote(dst);
				if(v.Count!=1)
				{
					throw new SftpException(SSH_FX_FAILURE, v.ToString());
				}
				dst=(String)(v[0]);
				if(isRemoteDir(dst))
				{
					throw new SftpException(SSH_FX_FAILURE, dst+" is a directory");
				}

				long skip=0;
				if(mode==RESUME || mode==APPEND)
				{
					try
					{
						SftpATTRS attr=stat(dst);
						skip=attr.getSize();
					}
					catch(Exception eee)
					{
						//System.out.println(eee);
					}
				}

				if(mode==OVERWRITE){ sendOPENW(Util.getBytes(dst)); }
				else{ sendOPENA(Util.getBytes(dst)); }

				buf.rewind();
				int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				int length=buf.getInt();
				int type=buf.getByte();
				if(type!=SSH_FXP_STATUS && type!=SSH_FXP_HANDLE)
				{
					throw new SftpException(SSH_FX_FAILURE, "");
				}
				if(type==SSH_FXP_STATUS)
				{
					buf.getInt();
					i=buf.getInt();
					throwStatusError(buf, i);
				}
				buf.getInt();
				byte[] handle=buf.getString();         // filename
				long offset=0;
				if(mode==RESUME || mode==APPEND)
				{
					offset+=skip;
				}

				long[] _offset=new long[1];
				_offset[0]=offset;
				OutputStreamPut outs = new OutputStreamPut
					(this,handle,_offset,monitor);
						
				return outs;
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}

		internal class OutputStreamPut : java.io.OutputStream
		{
			ChannelSftp sftp;
			byte[] handle;
			long[] _offset;
			SftpProgressMonitor monitor;

			internal OutputStreamPut
				(ChannelSftp sftp,
				byte[] handle,
				long[] _offset,
				SftpProgressMonitor monitor):base()
			{
				this.sftp=sftp;
				this.handle=handle;
				this._offset=_offset;
				this.monitor=monitor;
			}

			public override void Write(byte[] d, int s, int len) 
			{
				try
				{
					sftp.sendWRITE(handle, _offset[0], d, s, len);
					_offset[0]+=len;
					if(!sftp.checkStatus())
					{ 
						throw new IOException("jsch status error");
					}
					if(monitor!=null && !monitor.count(1))
					{
						throw new IOException("canceled");
					}
				}
				catch(IOException e){ throw e; }
				catch(Exception e){ throw new IOException(e.ToString());  }
			}
			byte[] _data=new byte[1];
			public override void WriteByte(byte foo) 
			{
				_data[0]=foo;
				Write(_data, 0, 1);
			}
			public void Write(int foo)
			{
				this.WriteByte((byte)foo);
			}
			public override void  Close() 
			{
				if(monitor!=null)monitor.end();
				try{ sftp._sendCLOSE(handle); }
				catch(IOException e){ throw e; }
				catch(Exception e)
				{
					throw new IOException(e.ToString());
				}
			}
		}

		/**/
		public void get(String src, String dst) 
		{
			get(src, dst, null, OVERWRITE);
		}
		public void get(String src, String dst,
			SftpProgressMonitor monitor) 
		{
			get(src, dst, monitor, OVERWRITE);
		}
		public void get(String src, String dst,
			SftpProgressMonitor monitor, int mode) 
		{
			if(!src.StartsWith("/")){ src=cwd+"/"+src; } 
			//    if(!dst.StartsWith("/")){ dst=lcwd+file_separator+dst; } 
			if(!isLocalAbsolutePath(dst)){ dst=lcwd+file_separator+dst; } 
			try
			{
				ArrayList v=glob_remote(src);
				if(v.Count==0)
				{
					throw new SftpException(SSH_FX_NO_SUCH_FILE, "No such file");
				}
				for(int j=0; j<v.Count; j++)
				{
					String _dst=dst;
					String _src=(String)(v[j]);
					if(Directory.Exists(_dst))
					{
						if(!_dst.EndsWith(file_separator))
						{
							_dst+=file_separator;
						}
						int i=_src.LastIndexOf('/');
						if(i==-1) _dst+=src;
						else _dst+=_src.Substring(i+1);
					}

					SftpATTRS attr=stat(_src);
					if(mode==RESUME)
					{
						long size_of_src=attr.getSize();
						long size_of_dst=new FileInfo(_dst).Length;
						if(size_of_dst>size_of_src)
						{
							throw new SftpException(SSH_FX_FAILURE, "failed to resume for "+_dst);
						}
						if(size_of_dst==size_of_src)
						{
							return;
						}
					}

					if(monitor!=null)
					{
						monitor.init(SftpProgressMonitor.GET, _src, _dst, attr.getSize());
						if(mode==RESUME)
						{
							monitor.count(new FileInfo(_dst).Length);
						}
					}
					FileStream fos=null;
					if(mode==OVERWRITE)
					{
						fos=new FileStream(_dst, FileMode.Create);
					}
					else
					{
						fos=new FileStream(_dst, FileMode.Append); // append
					}
					_get(_src, fos, monitor, mode, new FileInfo(_dst).Length);
					fos.Close();
				}
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}
		public void get(String src, Stream dst) 
		{
			get(src, dst, null, OVERWRITE, 0);
		}
		public void get(String src, Stream dst,
			SftpProgressMonitor monitor) 
		{
			get(src, dst, monitor, OVERWRITE, 0);
		}
		public void get(String src, Stream dst,
			SftpProgressMonitor monitor, int mode, long skip) 
		{
			//System.out.println("get: "+src+", "+dst);
			try
			{
				if(!src.StartsWith("/")){ src=cwd+"/"+src; } 
				ArrayList v=glob_remote(src);
				if(v.Count!=1)
				{
					throw new SftpException(SSH_FX_FAILURE, v.ToString());
				}
				src=(String)(v[0]);

				if(monitor!=null)
				{
					SftpATTRS attr=stat(src);
					monitor.init(SftpProgressMonitor.GET, src, "??", attr.getSize());
					if(mode==RESUME)
					{
						monitor.count(skip);
					}
				}
				_get(src, dst, monitor, mode, skip);
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}
		private void _get(String src, Stream dst,
			SftpProgressMonitor monitor, int mode, long skip) 
		{
			//System.out.println("_get: "+src+", "+dst);
			try
			{
				sendOPENR(Util.getBytes(src));
				buf.rewind();
				int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				int length=buf.getInt();
				int type=buf.getByte();
				if(type!=SSH_FXP_STATUS && type!=SSH_FXP_HANDLE)
				{
					throw new SftpException(SSH_FX_FAILURE, "");
				}
				if(type==SSH_FXP_STATUS)
				{
					buf.getInt();
					i=buf.getInt();
					throwStatusError(buf, i);
				}
				buf.getInt();
				byte[] handle=buf.getString();         // filename

				long offset=0;
				if(mode==RESUME)
				{
					offset+=skip;
				}

				int request_len=0;
			loop:
				while(true)
				{

					request_len=buf.buffer.Length-13;
					if(server_version==0){ request_len=1024; }
					sendREAD(handle, offset, request_len);
	
					buf.rewind();
					i=io.ins.Read(buf.buffer, 0, 13);  // 4 + 1 + 4 + 4
					if(i!=13)
					{
						goto BREAK;
					}
					length=buf.getInt();
					type=buf.getByte();  
					length--;
					buf.getInt();        
					length-=4;
					if(type!=SSH_FXP_STATUS && type!=SSH_FXP_DATA)
					{ 
						goto BREAK;
					}
					if(type==SSH_FXP_STATUS)
					{
						i=buf.getInt();    
						length-=4;
						io.ins.Read(buf.buffer, 13, length);
						if(i==SSH_FX_EOF)
						{
							goto BREAK;
						}
						throwStatusError(buf, i);
					}

					i=buf.getInt();    
					length-=4;
					int foo=i;
					while(foo>0)
					{
						int bar=length;
						if(bar>buf.buffer.Length)
						{
							bar=buf.buffer.Length;
						}
						i=io.ins.Read(buf.buffer, 0, bar);
						if(i<0)
						{
							goto BREAK;
						}
						int data_len=i;
						length-=data_len;
						dst.Write(buf.buffer, 0, data_len);
	  
						if(monitor!=null)
						{
							if(!monitor.count(data_len))
							{
								goto BREAK;
							}
						}

						offset+=data_len;
						foo-=data_len;
					}
					//System.out.println("length: "+length);  // length should be 0
				}
			BREAK:
				dst.Flush();

				if(monitor!=null)monitor.end();
				_sendCLOSE(handle);
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}

		public Stream get(String src) 
		{
			return get(src, null, OVERWRITE);
		}
		public Stream get(String src, SftpProgressMonitor monitor) 
		{
			return get(src, monitor, OVERWRITE);
		}
		public Stream get(String src, int mode) 
		{
			return get(src, null, mode);
		}
		public Stream get(String src,  SftpProgressMonitor monitor,  int mode) 
		{
			if(mode==RESUME)
			{
				throw new SftpException(SSH_FX_FAILURE, "faile to resume from "+src);
			}
			if(!src.StartsWith("/")){ src=cwd+"/"+src; } 
			try
			{
				ArrayList v=glob_remote(src);
				if(v.Count!=1)
				{
					throw new SftpException(SSH_FX_FAILURE, v.ToString());
				}
				src=(String)(v[0]);

				SftpATTRS attr=stat(src);
				if(monitor!=null)
				{
					monitor.init(SftpProgressMonitor.GET, src, "??", attr.getSize());
				}

				sendOPENR(Util.getBytes(src));
				buf.rewind();
				int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				int length=buf.getInt();
				int type=buf.getByte();
				if(type!=SSH_FXP_STATUS && type!=SSH_FXP_HANDLE)
				{
					throw new SftpException(SSH_FX_FAILURE, "");
				}
				if(type==SSH_FXP_STATUS)
				{
					buf.getInt();
					i=buf.getInt();
					throwStatusError(buf, i);
				}
				buf.getInt();
				byte[] handle=buf.getString();         // filename

				long[] _offset=new long[1];
				//_offset[0]=0;
				int[]  _server_version=new int[1];
				_server_version[0]=server_version;

				InputStreamGet ins=new InputStreamGet(this, handle, _offset, _server_version, monitor);
				return ins;
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}

		public class InputStreamGet : java.io.InputStream
		{
			bool closed=false;
			int rest_length=0;
			byte[] _data=new byte[1];
			ChannelSftp sftp;
			byte[] handle;
			long[] _offset;
			int[]_server_version;
			SftpProgressMonitor monitor;

			public InputStreamGet( 
				ChannelSftp sftp,
				byte[] handle,
				long[] _offset,
				int[] _server_version,
				SftpProgressMonitor monitor)
			{
				this.sftp=sftp;
				this.handle=handle;
				this._offset=_offset;
				this._server_version=_server_version;
				this.monitor=monitor;
			}
			public override int ReadByte() 
			{
				int i=Read(_data, 0, 1);
				if (i==-1) { return -1; }
				else 
				{
					return _data[0]&0xff;
				}
			}
			public int Read(byte[] d) 
			{
				return Read(d, 0, d.Length);
			}
			public override int Read(byte[] d, int s, int len) 
			{
				if(d==null){throw new NullReferenceException();}
				if(s<0 || len <0 || s+len>d.Length)
				{
					throw new IndexOutOfRangeException();
				} 
				if(len==0){ return 0; }

				if(rest_length>0)
				{
					int foo=rest_length;
					if(foo>len) foo=len;
					int i=sftp.io.ins.Read(d, s, foo);
					if(i<0)
					{
						throw new IOException("error");
					}
					rest_length-=foo;
					return i;
				}

				if(sftp.buf.buffer.Length-13<len)
				{
					len=sftp.buf.buffer.Length-13;
				}
				if(_server_version[0]==0 && len>1024)
				{
					len=1024; 
				}

				try{sftp.sendREAD(handle, _offset[0], len);}
				catch(Exception e){ throw new IOException("error"); }

				sftp.buf.rewind();
				int ii=sftp.io.ins.Read(sftp.buf.buffer, 0, 13);  // 4 + 1 + 4 + 4
				if(ii!=13)
				{ 
					throw new IOException("error");
				}

				rest_length=sftp.buf.getInt();
				int type=sftp.buf.getByte();  
				rest_length--;
				sftp.buf.getInt();        
				rest_length-=4;
				if(type!=SSH_FXP_STATUS && type!=SSH_FXP_DATA)
				{ 
					throw new IOException("error");
				}
				if(type==SSH_FXP_STATUS)
				{
					ii=sftp.buf.getInt();    
					rest_length-=4;
					sftp.io.ins.Read(sftp.buf.buffer, 13, rest_length);
					rest_length=0;
					if(ii==SSH_FX_EOF)
					{
						Close();
						return -1;
					}
					//throwStatusError(buf, i);
					throw new IOException("error");
				}

				ii=sftp.buf.getInt();    
				rest_length-=4;
				int goo=ii;
				if(goo>0)
				{
					int bar=rest_length;
					if(bar>len)
					{
						bar=len;
					}
					ii=sftp.io.ins.Read(d, s, bar);
					if(ii<0)
					{
						return -1;
					}
					int data_len=ii;
					rest_length-=data_len;
  
					if(monitor!=null)
					{
						if(!monitor.count(data_len))
						{
							return -1;
						}
					}
					_offset[0]+=data_len;
					return ii;
				}
				return 0; // ??
			}
			public override void Close() 
			{
				if(closed)return;
				closed=true;
				/*
					while(rest_length>0){
					  int foo=rest_length;
					  if(foo>buf.buffer.Length){
						foo=buf.buffer.Length;
					  }
					  io.ins.Read(buf.buffer, 0, foo);
					  rest_length-=foo;
					}
					*/
				if(monitor!=null)monitor.end();
				try{sftp._sendCLOSE(handle);}
				catch(Exception e){throw new IOException("error");}
			}
		}

		public ArrayList ls(String path) 
		{
			try
			{
				if(!path.StartsWith("/")){ path=cwd+"/"+path; }

				String dir=path;
				byte[] pattern=null;
				SftpATTRS attr=null;
				if(isPattern(dir) || 
					((attr=stat(dir))!=null && !attr.isDir()))
				{
					int foo=path.LastIndexOf('/');
					dir=path.Substring(0, ((foo==0)?1:foo));
					pattern=Util.getBytes( path.Substring(foo+1) );
				}
				sendOPENDIR(Util.getBytes(dir));

				buf.rewind();
				int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				int length=buf.getInt();
				int type=buf.getByte();
				if(type!=SSH_FXP_STATUS && type!=SSH_FXP_HANDLE)
				{
					throw new SftpException(SSH_FX_FAILURE, "");
				}
				if(type==SSH_FXP_STATUS)
				{
					buf.getInt();
					i=buf.getInt();
					throwStatusError(buf, i);
				}
				buf.getInt();
				byte[] handle=buf.getString();         // filename

				ArrayList v=new ArrayList();
				while(true)
				{
					sendREADDIR(handle);
					buf.rewind();
					i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
					buf.index=i;
					length=buf.getInt();
					length=length-(i-4);
					type=buf.getByte();

					if(type!=SSH_FXP_STATUS && type!=SSH_FXP_NAME)
					{
						throw new SftpException(SSH_FX_FAILURE, "");
					}
					if(type==SSH_FXP_STATUS)
					{ 
						/*
						buf.getInt();
						i=buf.getInt();
						System.out.println("i="+i);
						if(i==SSH_FX_EOF) break;
						byte[] str=buf.getString();
						throw new SftpException(i, Util.getString(str));
						*/
						break;
					}

					buf.getInt();
					int count=buf.getInt();

					byte[] str;
					int flags;

					while(count>0)
					{
						if(length>0)
						{
							buf.shift();
							i=io.ins.Read(buf.buffer, buf.index, buf.buffer.Length-buf.index);
							if(i<=0)break;
							buf.index+=i;
							length-=i;
						}

						byte[] filename=buf.getString();
						//System.out.println("filename: "+Util.getString(filename));
						str=buf.getString();
						String longname=Util.getString(str);
						// System.out.println("longname: "+longname);

						SftpATTRS attrs=SftpATTRS.getATTR(buf);
						if(pattern==null || Util.glob(pattern, filename))
						{
							//System.out.println(Util.getString(filename)+" |"+longname+"|");
							//  	    v.Add(longname);
							v.Add(new LsEntry(Util.getString(filename), longname, attrs));
						}

						count--; 
					}
				}
				_sendCLOSE(handle);
				return v;
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}
		public void symlink(String oldpath, String newpath) 
		{
			if(server_version<3)
			{
				throw new SftpException(SSH_FX_FAILURE, 
					"The remote sshd is too old to support symlink operation.");
			}

			try
			{
				if(!oldpath.StartsWith("/")){ oldpath=cwd+"/"+oldpath; } 
				if(!newpath.StartsWith("/")){ newpath=cwd+"/"+newpath; } 

				ArrayList v=glob_remote(oldpath);
				if(v.Count!=1)
				{
					throw new SftpException(SSH_FX_FAILURE, v.ToString());
				}
				oldpath=(String)(v[0]);

				sendSYMLINK(Util.getBytes(oldpath), Util.getBytes(newpath));
				buf.rewind();
				int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				int length=buf.getInt();
				int type=buf.getByte();
				if(type!=SSH_FXP_STATUS)
				{
					throw new SftpException(SSH_FX_FAILURE, "");
				}
				buf.getInt();
				i=buf.getInt();
				if(i==SSH_FX_OK) return;
				throwStatusError(buf, i);
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}
		public void rename(String oldpath, String newpath) 
		{
			if(server_version<2)
			{
				throw new SftpException(SSH_FX_FAILURE, 
					"The remote sshd is too old to support rename operation.");
			}
			try
			{
				if(!oldpath.StartsWith("/")){ oldpath=cwd+"/"+oldpath; } 
				if(!newpath.StartsWith("/")){ newpath=cwd+"/"+newpath; } 

				ArrayList v=glob_remote(oldpath);
				if(v.Count!=1)
				{
					throw new SftpException(SSH_FX_FAILURE, v.ToString());
				}
				oldpath=(String)(v[0]);

				v=glob_remote(newpath);
				if(v.Count>=2)
				{
					throw new SftpException(SSH_FX_FAILURE, v.ToString());
				}
				if(v.Count==1)
				{
					newpath=(String)(v[0]);
				}

				sendRENAME(Util.getBytes(oldpath), Util.getBytes(newpath));
				buf.rewind();
				int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				int length=buf.getInt();
				int type=buf.getByte();
				if(type!=SSH_FXP_STATUS)
				{
					throw new SftpException(SSH_FX_FAILURE, "");
				}
				buf.getInt();
				i=buf.getInt();
				if(i==SSH_FX_OK) return;
				throwStatusError(buf, i);
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}
		public void rm(String path) 
		{
			try
			{
				if(!path.StartsWith("/")){ path=cwd+"/"+path; }
				ArrayList v=glob_remote(path);
				for(int j=0; j<v.Count; j++)
				{
					path=(String)(v[j]);
					sendREMOVE(Util.getBytes(path));
					buf.rewind();
					int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
					int length=buf.getInt();
					int type=buf.getByte();
					if(type!=SSH_FXP_STATUS)
					{
						throw new SftpException(SSH_FX_FAILURE, "");
					}
					buf.getInt();
					i=buf.getInt();
					if(i!=SSH_FX_OK)
					{
						throwStatusError(buf, i);
					}
				}
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}
		private bool isRemoteDir(String path)
		{
			try
			{
				sendSTAT(Util.getBytes(path));
				buf.rewind();
				int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				int length=buf.getInt();
				int type=buf.getByte();
				if(type!=SSH_FXP_ATTRS){ return false; }
				buf.getInt();
				SftpATTRS attr=SftpATTRS.getATTR(buf);
				return attr.isDir();
			}
			catch(Exception e){}
			return false;
		}
		/*
		bool isRemoteDir(String path) {
		  SftpATTRS attr=stat(path);
		  return attr.isDir();
		}
		*/
		public void chgrp(int gid, String path) 
		{
			try
			{
				if(!path.StartsWith("/")){ path=cwd+"/"+path; }

				ArrayList v=glob_remote(path);
				for(int j=0; j<v.Count; j++)
				{
					path=(String)(v[j]);
					sendSTAT(Util.getBytes(path));
 
					buf.rewind();
					int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
					int length=buf.getInt();
					int type=buf.getByte();
					if(type!=SSH_FXP_ATTRS)
					{
						throw new SftpException(SSH_FX_FAILURE, "");
					}
					buf.getInt();
					SftpATTRS attr=SftpATTRS.getATTR(buf);
					attr.setUIDGID(attr.uid, gid); 
					_setStat(path, attr);
				}
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}
		public void chown(int uid, String path) 
		{
			try
			{
				if(!path.StartsWith("/")){ path=cwd+"/"+path; }

				ArrayList v=glob_remote(path);
				for(int j=0; j<v.Count; j++)
				{
					path=(String)(v[j]);

					sendSTAT(Util.getBytes(path));
 
					buf.rewind();
					int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
					int length=buf.getInt();
					int type=buf.getByte();
					if(type!=SSH_FXP_ATTRS)
					{
						throw new SftpException(SSH_FX_FAILURE, "");
					}
					buf.getInt();
					SftpATTRS attr=SftpATTRS.getATTR(buf);
					attr.setUIDGID(uid, attr.gid); 
					_setStat(path, attr);
				}
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}
		public void chmod(int permissions, String path) 
		{
			try
			{
				if(!path.StartsWith("/")){ path=cwd+"/"+path; }

				ArrayList v=glob_remote(path);
				for(int j=0; j<v.Count; j++)
				{
					path=(String)(v[j]);

					sendSTAT(Util.getBytes(path));
 
					buf.rewind();
					int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
					int length=buf.getInt();
					int type=buf.getByte();
					if(type!=SSH_FXP_ATTRS)
					{
						throw new SftpException(SSH_FX_FAILURE, "");
					}
					buf.getInt();
					SftpATTRS attr=SftpATTRS.getATTR(buf);
					attr.setPERMISSIONS(permissions); 
					_setStat(path, attr);
				}
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}
		public void setMtime(String path, int mtime) 
		{
			try
			{
				if(!path.StartsWith("/")){ path=cwd+"/"+path; }

				ArrayList v=glob_remote(path);
				for(int j=0; j<v.Count; j++)
				{
					path=(String)(v[j]);
					sendSTAT(Util.getBytes(path));
					buf.rewind();
					int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
					int length=buf.getInt();
					int type=buf.getByte();
					if(type!=SSH_FXP_ATTRS)
					{
						throw new SftpException(SSH_FX_FAILURE, "");
					}
					buf.getInt();
					SftpATTRS attr=SftpATTRS.getATTR(buf);
					attr.setACMODTIME(attr.getATime(), mtime);
					_setStat(path, attr);
				}
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}
		public void rmdir(String path) 
		{
			try
			{
				if(!path.StartsWith("/")){ path=cwd+"/"+path; }
				ArrayList v=glob_remote(path);
				for(int j=0; j<v.Count; j++)
				{
					path=(String)(v[j]);

					sendRMDIR(Util.getBytes(path));
					buf.rewind();
					int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
					int length=buf.getInt();
					int type=buf.getByte();
					if(type!=SSH_FXP_STATUS)
					{
						throw new SftpException(SSH_FX_FAILURE, "");
					}
					buf.getInt();
					i=buf.getInt();
					if(i!=SSH_FX_OK)
					{
						throwStatusError(buf, i);
					}
				}
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}

		public void mkdir(String path) 
		{
			try
			{
				if(!path.StartsWith("/")){ path=cwd+"/"+path; }
				sendMKDIR(Util.getBytes(path), null);
				buf.rewind();
				int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				int length=buf.getInt();
				int type=buf.getByte();
				if(type!=SSH_FXP_STATUS)
				{
					throw new SftpException(SSH_FX_FAILURE, "");
				}
				buf.getInt();
				i=buf.getInt();
				if(i==SSH_FX_OK) return;
				throwStatusError(buf, i);
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}

		public SftpATTRS stat(String path) 
		{
			try
			{
				if(!path.StartsWith("/")){ path=cwd+"/"+path; }
				sendSTAT(Util.getBytes(path));
				buf.rewind();
				int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				int length=buf.getInt();
				int type=buf.getByte();
				if(type!=SSH_FXP_ATTRS)
				{
					if(type==SSH_FXP_STATUS)
					{
						buf.getInt();
						i=buf.getInt();
						throwStatusError(buf, i);
					}
					throw new SftpException(SSH_FX_FAILURE, "");
				}
				buf.getInt();
				SftpATTRS attr=SftpATTRS.getATTR(buf);
				return attr;
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
			//return null;
		}
		public SftpATTRS lstat(String path) 
		{
			try
			{
				if(!path.StartsWith("/")){ path=cwd+"/"+path; }
				sendLSTAT(Util.getBytes(path));
				buf.rewind();
				int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				int length=buf.getInt();
				int type=buf.getByte();
				if(type!=SSH_FXP_ATTRS)
				{
					if(type==SSH_FXP_STATUS)
					{
						buf.getInt();
						i=buf.getInt();
						throwStatusError(buf, i);
					}
					throw new SftpException(SSH_FX_FAILURE, "");
				}
				buf.getInt();
				SftpATTRS attr=SftpATTRS.getATTR(buf);
				return attr;
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}
		public void setStat(String path, SftpATTRS attr) 
		{
			try
			{
				if(!path.StartsWith("/")){ path=cwd+"/"+path; }
				ArrayList v=glob_remote(path);
				for(int j=0; j<v.Count; j++)
				{
					path=(String)(v[j]);
					_setStat(path, attr);
				}
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}
		private void _setStat(String path, SftpATTRS attr) 
		{
			try
			{
				sendSETSTAT(Util.getBytes(path), attr);

				buf.rewind();
				int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				int length=buf.getInt();
				int type=buf.getByte();
				if(type!=SSH_FXP_STATUS)
				{
					throw new SftpException(SSH_FX_FAILURE, "");
				}
				buf.getInt();
				i=buf.getInt();
				if(i!=SSH_FX_OK)
				{
					throwStatusError(buf, i);
				}
			}
			catch(Exception e)
			{
				if(e is SftpException) throw (SftpException)e;
				throw new SftpException(SSH_FX_FAILURE, "");
			}
		}

		public String pwd(){ return cwd; }
		public String lpwd(){ return lcwd; }
		public String version(){ return _version; }

		private bool checkStatus() 
		{
			buf.rewind();
			io.ins.Read(buf.buffer, 0, buf.buffer.Length);
			int length=buf.getInt();
			int type=buf.getByte();
			if(type!=SSH_FXP_STATUS)
			{ 
				throw new SftpException(SSH_FX_FAILURE, "");
			}
			buf.getInt();
			int i=buf.getInt();
			if(i!=SSH_FX_OK)
			{
				throwStatusError(buf, i);
				//System.out.println("getInt="+buf.getInt());
			}
			return true;
		}
		private void _sendCLOSE(byte[] handle) 
		{
			sendCLOSE(handle);
			buf.rewind();
			int i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
			int length=buf.getInt();
			int type=buf.getByte();
			if(type!=SSH_FXP_STATUS)
			{
				throw new SftpException(SSH_FX_FAILURE, "");
			}
			buf.getInt();
			i=buf.getInt();
			if(i==SSH_FX_OK) return;
			throwStatusError(buf, i);
		}

		private void sendINIT() 
		{
			packet.reset();
			putHEAD(SSH_FXP_INIT, 5);
			buf.putInt(3);                // version 3
			session.write(packet, this, 5+4);
		}

		private void sendREALPATH(byte[] path) 
		{
			sendPacketPath(SSH_FXP_REALPATH, path);
		}
		private void sendSTAT(byte[] path) 
		{
			sendPacketPath(SSH_FXP_STAT, path);
		}
		private void sendLSTAT(byte[] path) 
		{
			sendPacketPath(SSH_FXP_LSTAT, path);
		}
		private void sendFSTAT(byte[] handle) 
		{
			sendPacketPath(SSH_FXP_FSTAT, handle);
		}
		private void sendSETSTAT(byte[] path, SftpATTRS attr) 
		{
			packet.reset();
			putHEAD(SSH_FXP_SETSTAT, 9+path.Length+attr.length());
			buf.putInt(count++);
			buf.putString(path);             // path
			attr.dump(buf);
			session.write(packet, this, 9+path.Length+attr.length()+4);
		}
		private void sendREMOVE(byte[] path) 
		{
			sendPacketPath(SSH_FXP_REMOVE, path);
		}
		private void sendMKDIR(byte[] path, SftpATTRS attr) 
		{
			packet.reset();
			putHEAD(SSH_FXP_MKDIR, 9+path.Length+(attr!=null?attr.length():4));
			buf.putInt(count++);
			buf.putString(path);             // path
			if(attr!=null) attr.dump(buf);
			else buf.putInt(0);
			session.write(packet, this, 9+path.Length+(attr!=null?attr.length():4)+4);
		}
		private void sendRMDIR(byte[] path) 
		{
			sendPacketPath(SSH_FXP_RMDIR, path);
		}
		private void sendSYMLINK(byte[] p1, byte[] p2) 
		{
			sendPacketPath(SSH_FXP_SYMLINK, p1, p2);
		}
		private void sendREADLINK(byte[] path) 
		{
			sendPacketPath(SSH_FXP_READLINK, path);
		}
		private void sendOPENDIR(byte[] path) 
		{
			sendPacketPath(SSH_FXP_OPENDIR, path);
		}
		private void sendREADDIR(byte[] path) 
		{
			sendPacketPath(SSH_FXP_READDIR, path);
		}
		private void sendRENAME(byte[] p1, byte[] p2) 
		{
			sendPacketPath(SSH_FXP_RENAME, p1, p2);
		}
		private void sendCLOSE(byte[] path) 
		{
			sendPacketPath(SSH_FXP_CLOSE, path);
		}
		private void sendOPENR(byte[] path) 
		{
			sendOPEN(path, SSH_FXF_READ);
		}
		private void sendOPENW(byte[] path) 
		{
			sendOPEN(path, SSH_FXF_WRITE|SSH_FXF_CREAT|SSH_FXF_TRUNC);
		}
		private void sendOPENA(byte[] path) 
		{
			sendOPEN(path, SSH_FXF_WRITE|/*SSH_FXF_APPEND|*/SSH_FXF_CREAT);
		}
		private void sendOPEN(byte[] path, int mode) 
		{
			packet.reset();
			putHEAD(SSH_FXP_OPEN, 17+path.Length);
			buf.putInt(count++);
			buf.putString(path);
			buf.putInt(mode);
			buf.putInt(0);           // attrs
			session.write(packet, this, 17+path.Length+4);
		}
		private void sendPacketPath(byte fxp, byte[] path) 
		{
			packet.reset();
			putHEAD(fxp, 9+path.Length);
			buf.putInt(count++);
			buf.putString(path);             // path
			session.write(packet, this, 9+path.Length+4);
		}
		private void sendPacketPath(byte fxp, byte[] p1, byte[] p2) 
		{
			packet.reset();
			putHEAD(fxp, 13+p1.Length+p2.Length);
			buf.putInt(count++);
			buf.putString(p1);
			buf.putString(p2);
			session.write(packet, this, 13+p1.Length+p2.Length+4);
		}

		private void sendWRITE(byte[] handle, long offset, 
			byte[] data, int start, int length) 
		{
			packet.reset();
			putHEAD(SSH_FXP_WRITE, 21+handle.Length+length);
			buf.putInt(count++);
			buf.putString(handle);
			buf.putLong(offset);
			buf.putString(data, start, length);
			session.write(packet, this, 21+handle.Length+length+4);
		}

		private void sendREAD(byte[] handle, long offset, int length) 
		{
			packet.reset();
			putHEAD(SSH_FXP_READ, 21+handle.Length);
			buf.putInt(count++);
			buf.putString(handle);
			buf.putLong(offset);
			buf.putInt(length);
			session.write(packet, this, 21+handle.Length+4);
		}

		private void putHEAD(byte type, int length) 
		{
			buf.putByte((byte)Session.SSH_MSG_CHANNEL_DATA);
			buf.putInt(recipient);
			buf.putInt(length+4);
			buf.putInt(length);
			buf.putByte(type);
		}
		private ArrayList glob_remote(String _path) 
		{
			//System.out.println("glob_remote: "+_path);
			ArrayList v=new ArrayList();
			byte[] path=Util.getBytes(_path);
			int i=path.Length-1;
			while(i>=0){if(path[i]=='*' || path[i]=='?')break;i--;}
			if(i<0){ v.Add(_path); return v;}
			while(i>=0){if(path[i]=='/')break;i--;}
			if(i<0){ v.Add(_path); return v;}
			byte[] dir;
			if(i==0){dir=new byte[]{(byte)'/'};}
			else
			{ 
				dir=new byte[i];
				Array.Copy(path, 0, dir, 0, i);
			}
			//System.out.println("dir: "+Util.getString(dir));
			byte[] pattern=new byte[path.Length-i-1];
			Array.Copy(path, i+1, pattern, 0, pattern.Length);
			//System.out.println("file: "+Util.getString(pattern));

			sendOPENDIR(dir);

			buf.rewind();
			i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
			int length=buf.getInt();
			int type=buf.getByte();
			if(type!=SSH_FXP_STATUS && type!=SSH_FXP_HANDLE)
			{
				throw new SftpException(SSH_FX_FAILURE, "");
			}
			if(type==SSH_FXP_STATUS)
			{
				buf.getInt();
				i=buf.getInt();
				throwStatusError(buf, i);
			}
			buf.getInt();
			byte[] handle=buf.getString();         // filename

			while(true)
			{
				sendREADDIR(handle);
				buf.rewind();
				i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
				buf.index=i;
				length=buf.getInt();
				length=length-(i-4);
				type=buf.getByte();

				if(type!=SSH_FXP_STATUS && type!=SSH_FXP_NAME)
				{
					throw new SftpException(SSH_FX_FAILURE, "");
				}
				if(type==SSH_FXP_STATUS)
				{ 
					break;
				}

				buf.getInt();
				int count=buf.getInt();

				byte[] str;
				int flags;

				while(count>0)
				{
					if(length>0)
					{
						buf.shift();
						i=io.ins.Read(buf.buffer, buf.index, buf.buffer.Length-buf.index);
						if(i<=0)break;
						buf.index+=i;
						length-=i;
					}

					byte[] filename=buf.getString();
					//System.out.println("filename: "+Util.getString(filename));
					str=buf.getString();
					SftpATTRS attrs=SftpATTRS.getATTR(buf);

					if(Util.glob(pattern, filename))
					{
						v.Add(Util.getString(dir)+"/"+Util.getString(filename));
					}

					count--; 
				}
			}

			sendCLOSE(handle);
			buf.rewind();
			i=io.ins.Read(buf.buffer, 0, buf.buffer.Length);
			length=buf.getInt();
			type=buf.getByte();
			if(type!=SSH_FXP_STATUS)
			{
				throw new SftpException(SSH_FX_FAILURE, "");
			}
			buf.getInt();
			i=buf.getInt();
			if(i==SSH_FX_OK) return v;

			return null;
		}

		private ArrayList glob_local(String _path) 
		{
			//System.out.println("glob_local: "+_path);
			ArrayList v=new ArrayList();
			byte[] path=Util.getBytes(_path);
			int i=path.Length-1;
			while(i>=0){if(path[i]=='*' || path[i]=='?')break;i--;}
			if(i<0){ v.Add(_path); return v;}
			while(i>=0){if(path[i]==file_separatorc)break;i--;}
			if(i<0){ v.Add(_path); return v;}
			byte[] dir;
			if(i==0){dir=new byte[]{(byte)file_separatorc};}
			else
			{ 
				dir=new byte[i];
				Array.Copy(path, 0, dir, 0, i);
			}
			byte[] pattern=new byte[path.Length-i-1];
			Array.Copy(path, i+1, pattern, 0, pattern.Length);
			//System.out.println("dir: "+Util.getString(dir)+" pattern: "+Util.getString(pattern));
			try
			{
				ArrayList children;
				children=new ArrayList(Directory.GetDirectories( Util.getString(dir) ));
				children.AddRange( Directory.GetFiles(Util.getString(dir)) );
				
				for(int j=0; j<children.Count; j++)
				{
					//System.out.println("children: "+children[j]);
					string child = (string)children[j];
					if(Util.glob(pattern, Util.getBytes(child)))
					{
						v.Add(Util.getString(dir)+file_separator+child);
					}
				}
			}
			catch(Exception e)
			{
			}
			return v;
		}

		private void throwStatusError(Buffer buf, int i) 
		{
			if(server_version>=3)
			{
				byte[] str=buf.getString();
				//byte[] tag=buf.getString();
				throw new SftpException(i, Util.getString(str));
			}
			else
			{
				throw new SftpException(i, "Failure");
			}
		}

		private static bool isLocalAbsolutePath(String path)
		{
			return Path.IsPathRooted(path);
		}

		/*
		public void finalize() throws Throwable{
		  super.finalize();
		}
		*/

		public override void disconnect()
		{
			//waitForRunningThreadFinish(10000);
			clearRunningThreads();
			base.disconnect();
		}
		private ArrayList threadList=null;

		[MethodImpl(MethodImplOptions.Synchronized)]
		protected  void addRunningThread(Thread thread)
		{
			if(threadList==null)threadList=new ArrayList();
			threadList.Add(thread);
		}

		[MethodImpl(MethodImplOptions.Synchronized)]
		protected  void clearRunningThreads()
		{
			if(threadList==null)return;
			for(int t=0;t<threadList.Count;t++)
			{
				Thread thread=(Thread)threadList[t];
				if(thread!=null)
					if(thread.IsAlive)
						thread.Interrupt();
			}
			threadList.Clear();
		}
		private bool isPattern(String path)
		{
			return path.IndexOf("*")!=-1 || path.IndexOf("?")!=-1;
		}

		public class LsEntry 
		{
			private  String filename;
			private  String longname;
			private  SftpATTRS attrs;
			internal LsEntry(String filename, String longname, SftpATTRS attrs)
			{
				setFilename(filename);
				setLongname(longname);
				setAttrs(attrs);
			}
			public String getFilename(){return filename;}
			void setFilename(String filename){this.filename = filename;}
			public String getLongname(){return longname;}
			void setLongname(String longname){this.longname = longname;}
			public SftpATTRS getAttrs(){return attrs;}
			void setAttrs(SftpATTRS attrs) {this.attrs = attrs;}
			public override String ToString(){ return longname; }
		}
	}
}
