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
using Collections = System.Collections;

namespace Fireball.Ssh.java.util
{
	/// <summary>
	/// Summary description for Hashtable.
	/// </summary>
	public class Hashtable
	{
		internal Collections.Hashtable h;

		public Hashtable()
		{
			h= new Collections.Hashtable();
		}
		public Hashtable(Collections.Hashtable h)
		{
			this.h=h;
		}

		public void put(object key, object item)
		{
			h.Add(key, item);
		}

		public object get(object key)
		{
			return h[key];
		}

		public Enumeration keys()
		{
			return new Enumeration( h.Keys.GetEnumerator() );
		}

		public object this[object key]
		{
			get{return get(key);}
			set{h[key]=value;}
		}
	}
}
