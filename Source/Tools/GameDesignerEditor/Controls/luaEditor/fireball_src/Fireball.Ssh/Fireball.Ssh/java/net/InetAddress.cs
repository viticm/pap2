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

namespace Fireball.Ssh.java.net
{
	/// <summary>
	/// Summary description for InetAddress.
	/// </summary>
	public class InetAddress
	{
		internal IPAddress addr;
		public InetAddress(string addr)
		{
			this.addr = IPAddress.Parse(addr);
		}
		public InetAddress(IPAddress addr)
		{
			this.addr = addr;
		}

		public bool isAnyLocalAddress()
		{
			return IPAddress.IsLoopback(addr);
		}

		public bool equals(InetAddress addr)
		{
			return addr.ToString().Equals( addr.ToString());
		}

		public bool equals(string addr)
		{
			return addr.ToString().Equals( addr.ToString());
		}

		public override string ToString()
		{
			return addr.ToString ();
		}

		public override bool Equals(object obj)
		{
			return equals (obj.ToString());
		}

		public string getHostAddress()
		{
			return ToString();
		}

		public override int GetHashCode()
		{
			return base.GetHashCode ();
		}

		public static InetAddress getByName(string name)
		{
			return new InetAddress( Dns.GetHostByName(name).AddressList[0] );
		}
	}
}
