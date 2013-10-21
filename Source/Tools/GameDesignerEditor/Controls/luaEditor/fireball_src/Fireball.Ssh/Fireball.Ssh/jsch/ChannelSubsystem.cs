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


/* -*-mode:java; c-basic-offset:2; indent-tabs-mode:nil -*- */
/*
Copyright (c) 2005 ymnk, JCraft,Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright 
     notice, this list of conditions and the following disclaimer in 
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

using System;
using Fireball.Ssh.java.net;
using Fireball.Ssh.java.lang;

namespace Fireball.Ssh.jsch
{
	public class ChannelSubsystem : ChannelSession
	{
		bool xforwading=false;
		bool pty=false;
		bool want_reply=true;
		String subsystem="";
		public override void setXForwarding(bool foo){ xforwading=true; }
		public void setPty(bool foo){ pty=foo; }
		public void setWantReply(bool foo){ want_reply=foo; }
		public void setSubsystem(String foo){ subsystem=foo; }
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
				request=new RequestSubsystem();
				((RequestSubsystem)request).request(session, this, subsystem, want_reply);
			}
			catch(Exception e)
			{
				if(e is JSchException){ throw (JSchException)e; }
				throw new JSchException("ChannelSubsystem");
			}
			Thread thread=new Thread(this);
			thread.setName("Subsystem for "+session.host);
			thread.start();
		}
		//public void finalize() throws Throwable{ super.finalize(); }
		public override void init()
		{
			io.setInputStream(session.In);
			io.setOutputStream(session.Out);
		}
		public void setErrStream(System.IO.Stream outs)
		{
			setExtOutputStream(outs);
		}
		public java.io.InputStream getErrStream() 
		{
			return getExtInputStream();
		}
	}

}
