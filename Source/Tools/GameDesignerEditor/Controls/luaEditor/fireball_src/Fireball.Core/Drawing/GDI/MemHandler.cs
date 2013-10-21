#region Copyright

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

#endregion

using System.Collections;

namespace Fireball.Drawing.GDI
{
	/// <summary>
	/// Summary description for MemHandler.
	/// </summary>
	public class MemHandler
	{
		private static ArrayList mHeap = new ArrayList();

		public static void Add(GDIObject item)
		{
			mHeap.Add(item);
		}

		public static void Remove(GDIObject item)
		{
			if (mHeap.Contains(item))
			{
				mHeap.Remove(item);
			}
		}

		public static GDIObject[] Items
		{
			get
			{
				ArrayList al = new ArrayList();

				foreach (GDIObject go in mHeap)
				{
					if (go != null)
						al.Add(go);
				}

				GDIObject[] gos = new GDIObject[al.Count];
				al.CopyTo(0, gos, 0, al.Count);
				return gos;
			}
		}

		public static void DestroyAll()
		{
			foreach (GDIObject go in Items)
			{
				go.Dispose();
			}
		}
	}
}