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
using System.Collections;
using Fireball.Ssh.jsch;

/* 
 * SshBase.cs
 * 
 * Copyright (c) 2006 Tamir Gal, http://www.tamirgal.com, All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  	1. Redistributions of source code must retain the above copyright notice,
 *		this list of conditions and the following disclaimer.
 *
 *	    2. Redistributions in binary form must reproduce the above copyright 
 *		notice, this list of conditions and the following disclaimer in 
 *		the documentation and/or other materials provided with the distribution.
 *
 *	    3. The names of the authors may not be used to endorse or promote products
 *		derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR
 *  *OR ANY CONTRIBUTORS TO THIS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 **/
namespace Fireball.Ssh
{
	/// <summary>
	/// A wrapper class for JSch's SSH channel
	/// </summary>
	public abstract class SshBase
	{
		protected string    m_host;
		protected string	m_user;
		protected string	m_pass;
		protected JSch      m_jsch;
		protected Session   m_session;
		protected Channel   m_channel;

		/// <summary>
		/// Default TCP port of SSH protocol
		/// </summary>
		private static int SSH_TCP_PORT = 22;

		/// <summary>
		/// Constructs a new SSH instance
		/// </summary>
		/// <param name="sftpHost">The remote SSH host</param>
		/// <param name="user">The login username</param>
		/// <param name="password">The login password</param>
		public SshBase(string sftpHost, string user, string password)
		{
			this.m_host = sftpHost;
			this.m_user = user;
			this.Password = password;
			m_jsch = new JSch();
		}

		/// <summary>
		/// Constructs a new SSH instance
		/// </summary>
		/// <param name="sftpHost">The remote SSH host</param>
		/// <param name="user">The login username</param>
		public SshBase(string sftpHost, string user)
			: this(sftpHost, user, null)
		{
		}

		/// <summary>
		/// Adds identity file for publickey user authentication
		/// </summary>
		/// <param name="privateKeyFile"></param>
		public virtual void AddIdentityFile(string privateKeyFile)
		{
			m_jsch.addIdentity(privateKeyFile);
		}

		protected abstract string ChannelType{get;}

		/// <summary>
		/// Connect to remote SSH server
		/// </summary>
		public virtual void Connect()
		{
			this.Connect(SSH_TCP_PORT);
		}

		/// <summary>
		/// Connect to remote SSH server
		/// </summary>
		/// <param name="tcpPort">The destination TCP port for this connection</param>
		public virtual void Connect(int tcpPort)
		{
			this.ConnectSession(tcpPort);
			this.ConnectChannel();			
		}

		protected virtual void ConnectSession(int tcpPort)
		{
			m_session = m_jsch.getSession(m_user, m_host, tcpPort);
			if (Password != null)
				m_session.setPassword(Password);
			Hashtable config = new Hashtable();
			config.Add("StrictHostKeyChecking", "no");
			m_session.setConfig(config);
			m_session.connect();
		}

		protected virtual void ConnectChannel()
		{
			m_channel = m_session.openChannel(ChannelType);
			this.OnChannelReceived();
			m_channel.connect();
			this.OnConnected();
		}

		protected virtual void OnConnected()
		{
		}

		protected virtual void OnChannelReceived()
		{
		}

		/// <summary>
		/// Closes the SSH subsystem
		/// </summary>
		public virtual void Close()
		{
			if (m_channel != null)
			{
				m_channel.close();
				m_channel.disconnect();
				m_channel = null;
			}
			if (m_session != null)
			{
				m_session.disconnect();
				m_session = null;
			}
		}

		/// <summary>
		/// Return true if the SSH subsystem is connected
		/// </summary>
		public virtual bool Connected
		{
			get 
			{
				if (m_session != null)
					return m_session.isConnected();
				return false;
			}
		}

		/// <summary>
		/// Gets the Cipher algorithm name used in this SSH connection.
		/// </summary>
		public string Cipher
		{
			get{return m_session.getCipher();}
		}

		/// <summary>
		/// Gets the MAC algorithm name used in this SSH connection.
		/// </summary>
		public string Mac
		{
			get{return m_session.getMac();}
		}

		/// <summary>
		/// Gets the server SSH version string.
		/// </summary>
		public string ServerVersion
		{
			get{return m_session.getServerVersion();}
		}

		/// <summary>
		/// Gets the client SSH version string.
		/// </summary>
		public string ClientVersion
		{
			get{return m_session.getClientVersion();}
		}

		public string Host
		{
			get{return m_session.getHost();}
		}

		public HostKey HostKey
		{
			get{return m_session.getHostKey();}
		}

		public int Port
		{
			get{return m_session.getPort();}
		}

		/// <summary>
		/// The password string of the SSH subsystem
		/// </summary>
		public string Password
		{
			get { return m_pass; }
			set { m_pass = value; }
		}
		public string Username
		{
			get { return m_user; }
		}

		public static Version Version
		{
			get
			{
				System.Reflection.Assembly asm
					= System.Reflection.Assembly.GetAssembly(typeof(Fireball.Ssh.SshBase));
				return asm.GetName().Version;
			}
		}
	}
}
