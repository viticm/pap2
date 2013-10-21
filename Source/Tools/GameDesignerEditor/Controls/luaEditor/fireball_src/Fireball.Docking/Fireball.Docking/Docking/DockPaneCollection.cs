#region Fireball License
//    Copyright (C) 2005  Sebastian Faltoni sebastian{at}dotnetfireball{dot}net
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
#region Original License
// *****************************************************************************
// 
//  Copyright 2004, Weifen Luo
//  All rights reserved. The software and associated documentation 
//  supplied hereunder are the proprietary information of Weifen Luo
//  and are supplied subject to licence terms.
// 
//  WinFormsUI Library Version 1.0
// *****************************************************************************
#endregion


using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;

namespace Fireball.Docking
{
	/// <include file='CodeDoc\DockPaneCollection.xml' path='//CodeDoc/Class[@name="DockPaneCollection"]/ClassDef/*'/>>
	public class DockPaneCollection : ReadOnlyCollectionBase
	{
		internal DockPaneCollection()
		{
		}

		/// <include file='CodeDoc\DockPaneCollection.xml' path='//CodeDoc/Class[@name="DockPaneCollection"]/Property[@name="Item"]/*'/>>
		public DockPane this[int index]
		{
			get {  return InnerList[index] as DockPane;  }
		}

		internal int Add(DockPane pane)
		{
			if (InnerList.Contains(pane))
				return InnerList.IndexOf(pane);

			return InnerList.Add(pane);
		}

		internal void AddAt(DockPane pane, int index)
		{
			if (index < 0 || index > InnerList.Count - 1)
				return;
			
			if (Contains(pane))
				return;

			InnerList.Insert(index, pane);
		}

		internal void AddAt(DockPane pane, DockPane paneBefore)
		{
			AddAt(pane, IndexOf(paneBefore));
		}

		internal void Add(DockPane pane, DockPane paneBefore)
		{
			if (paneBefore == null)
				Add(pane);
			else
				InnerList.Insert(IndexOf(paneBefore), pane);
		}

		/// <include file='CodeDoc\DockPaneCollection.xml' path='//CodeDoc/Class[@name="DockPaneCollection"]/Method[@name="Contains(DockPane)"]/*'/>>
		public bool Contains(DockPane pane)
		{
			return InnerList.Contains(pane);
		}

		internal void Dispose()
		{
			for (int i=Count - 1; i>=0; i--)
				this[i].Close();
		}

		/// <include file='CodeDoc\DockPaneCollection.xml' path='//CodeDoc/Class[@name="DockPaneCollection"]/Method[@name="IndexOf(DockPane)"]/*'/>>
		public int IndexOf(DockPane pane)
		{
			return InnerList.IndexOf(pane);
		}

		internal void Remove(DockPane pane)
		{
			InnerList.Remove(pane);
		}

		internal void Clear()
		{
			InnerList.Clear();
		}
	}
}
