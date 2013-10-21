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
using System.IO;
using System.Net;
using System.Net.Sockets;
using Net = System.Net;
using Sock = System.Net.Sockets.Socket;

namespace Fireball.Ssh.java.net
{
	/// <summary>
	/// Summary description for Socket.
	/// </summary>
	public class Socket
	{
		internal Sock sock;
//		public Socket(AddressFamily af, SocketType st, ProtocolType pt)
//		{
//			this.sock = new Sock(af, st, pt);
//			this.sock.Connect();
//		}

		public Socket(string host, int port)
		{	
			IPEndPoint ep = new IPEndPoint(Dns.GetHostByName(host).AddressList[0], port);
			this.sock = new Sock(ep.AddressFamily,
			SocketType.Stream, ProtocolType.Tcp);
			this.sock.Connect(ep);
		}

		public Socket(Sock sock)
		{	
			this.sock = sock;
		}

		public Stream getInputStream()
		{
			return new Net.Sockets.NetworkStream(sock);
		}

		public Stream getOutputStream()
		{
			return new Net.Sockets.NetworkStream(sock);
		}

		public bool isConnected()
		{
			return sock.Connected;
		}

		public void setTcpNoDelay(bool b)
		{
			if(b)
			{
				sock.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.NoDelay, 1);
			}
			else
			{
				sock.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.NoDelay, 0);
			}
		}

		public void setSoTimeout(int t)
		{
			sock.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, t);
			sock.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, t);
		}

		public void close()
		{
			sock.Close();
		}

		public InetAddress getInetAddress()
		{
			return new InetAddress( ((IPEndPoint) sock.RemoteEndPoint).Address );
		}

		public int getPort()
		{
			return ((IPEndPoint) sock.RemoteEndPoint).Port;
		}
	}
}
