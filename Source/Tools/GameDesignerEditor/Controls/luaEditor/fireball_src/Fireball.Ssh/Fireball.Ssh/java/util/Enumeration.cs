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

namespace Fireball.Ssh.java.util
{
	/// <summary>
	/// Summary description for Enumeration.
	/// </summary>
	public class Enumeration
	{
		private IEnumerator e;
		private bool hasMore;
		public Enumeration(IEnumerator e)
		{
			this.e=e;
			hasMore = e.MoveNext();
		}

		public bool hasMoreElements()
		{
			return hasMore;
		}

		public object nextElement()
		{
			object o = e.Current;
			hasMore = e.MoveNext();
			return o;
		}
	}
}
