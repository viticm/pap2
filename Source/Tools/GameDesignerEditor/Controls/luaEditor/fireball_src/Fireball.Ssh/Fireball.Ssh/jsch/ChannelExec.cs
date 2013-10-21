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
using System.Net.Sockets;
using System.IO;
using Fireball.Ssh.java.lang;

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

	public class ChannelExec : ChannelSession
	{
		bool xforwading=false;
		bool pty=false;
		String command="";
		/*
		ChannelExec(){
		  super();
		  type="session".getBytes();
		  io=new IO();
		}
		*/
		public override void setXForwarding(bool foo){ xforwading=foo; }
		public void setPty(bool foo){ pty=foo; }
		public override void start()
		{
			try
			{
				Request request;

				if(xforwading)
				{
					request=new RequestX11();
					request.request(session, this);
				}

				if(pty)
				{
					request=new RequestPtyReq();
					request.request(session, this);
				}

				request=new RequestExec(command);
				request.request(session, this);
			}
			catch(Exception e)
			{
				throw new JSchException("ChannelExec");
			}
			thread=new Thread(this);
			thread.setName("Exec thread "+session.getHost());
			thread.start();
		}
		public void setCommand(String foo){ command=foo;}
		public override void init()
		{
			io.setInputStream(session.In);
			io.setOutputStream(session.Out);
		}
		//public void finalize() throws java.lang.Throwable{ super.finalize(); }
		public void setErrStream(Stream Out)
		{
			setExtOutputStream(Out);
		}
		public Stream getErrStream() 
		{
			return getExtInputStream();
		}
	}

}
