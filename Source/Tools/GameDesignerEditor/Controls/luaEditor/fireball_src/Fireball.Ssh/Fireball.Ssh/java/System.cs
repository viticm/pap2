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

namespace Fireball.Ssh.java
{
	/// <summary>
	/// Summary description for System.
	/// </summary>
	public class System
	{
		public Out Out = new Out();
		public Err err = new Err();
		public static void arraycopy(Array a1, long sourceIndex, Array a2, long destIndex, long len)
		{
			Array.Copy(a1, sourceIndex, a2, destIndex, len);
		}
	}

	public class Out
	{
		public void print(string v)
		{
			Console.Write(v);
		}

		public void println(string v)
		{
			Console.WriteLine(v);
		}
	}

	public class Err
	{
		public void print(string v)
		{
			Console.Error.Write(v);
		}

		public void println(string v)
		{
			Console.Error.WriteLine(v);
		}
	}
}
