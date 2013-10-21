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

namespace Fireball.Docking
{
	/// <include file='CodeDoc\DockContentCollection.xml' path='//CodeDoc/Class[@name="DockContentCollection"]/ClassDef/*'/>>
	public class DockContentCollection : ReadOnlyCollectionBase
	{
		internal DockContentCollection()
		{
		}

		internal DockContentCollection(DockPane pane)
		{
			m_dockPane = pane;
		}

		private DockPane m_dockPane = null;
		private DockPane DockPane
		{
			get	{	return m_dockPane;	}
		}

		/// <include file='CodeDoc\DockContentCollection.xml' path='//CodeDoc/Class[@name="DockContentCollection"]/Property[@name="Item"]/*'/>>
		public IDockableWindow this[int index]
		{
			get 
			{
				if (DockPane == null)
					return InnerList[index] as IDockableWindow;
				else
					return GetVisibleContent(index);
			}
		}

		internal int Add(IDockableWindow content)
		{
			#if DEBUG
			if (DockPane != null)
				throw new InvalidOperationException();
			#endif

			if (Contains(content))
				return IndexOf(content);

			return InnerList.Add(content);
		}

		internal void AddAt(IDockableWindow content, int index)
		{
			#if DEBUG
			if (DockPane != null)
				throw new InvalidOperationException();
			#endif

			if (index < 0 || index > InnerList.Count - 1)
				return;

			if (Contains(content))
				return;

			InnerList.Insert(index, content);
		}

		internal void AddAt(IDockableWindow content, IDockableWindow before)
		{
			#if DEBUG
			if (DockPane != null)
				throw new InvalidOperationException();
			#endif

			if (!Contains(before))
				return;

			if (Contains(content))
				return;

			AddAt(content, IndexOf(before));
		}

		internal void Clear()
		{
			#if DEBUG
			if (DockPane != null)
				throw new InvalidOperationException();
			#endif

			InnerList.Clear();
		}

		/// <include file='CodeDoc\DockContentCollection.xml' path='//CodeDoc/Class[@name="DockContentCollection"]/Method[@name="Contains(IDockContent)"]/*'/>
		public bool Contains(IDockableWindow content)
		{
			if (DockPane == null)
				return InnerList.Contains(content);
			else
				return (GetIndexOfVisibleContents(content) != -1);
		}

		/// <exclude/>
		public new int Count
		{
			get
			{
				if (DockPane == null)
					return base.Count;
				else
					return CountOfVisibleContents;
			}
		}

		/// <include file='CodeDoc\DockContentCollection.xml' path='//CodeDoc/Class[@name="DockContentCollection"]/Method[@name="IndexOf(IDockContent)"]/*'/>
		public int IndexOf(IDockableWindow content)
		{
			if (DockPane == null)
			{
				if (!Contains(content))
					return -1;
				else
					return InnerList.IndexOf(content);
			}
			else
				return GetIndexOfVisibleContents(content);
		}

		internal void Remove(IDockableWindow content)
		{
			if (DockPane != null)
				throw new InvalidOperationException();

			if (!Contains(content))
				return;

			InnerList.Remove(content);
		}

		internal IDockableWindow[] Select(DockAreas stateFilter)
		{
			if (DockPane != null)
				throw new InvalidOperationException();

			int count = 0;
			foreach (IDockableWindow c in this)
				if (DockHelper.IsDockStateValid(c.DockHandler.DockState, stateFilter))
					count ++;

			IDockableWindow[] contents = new IDockableWindow[count];

			count = 0;
			foreach (IDockableWindow c in this)
				if (DockHelper.IsDockStateValid(c.DockHandler.DockState, stateFilter))
					contents[count++] = c;

			return contents;
		}

		private int CountOfVisibleContents
		{
			get
			{
				#if DEBUG
				if (DockPane == null)
					throw new InvalidOperationException();
				#endif

				int count = 0;
				foreach (IDockableWindow content in DockPane.Contents)
				{
					if (content.DockHandler.DockState == DockPane.DockState)
						count ++;
				}
				return count;
			}
		}

		private IDockableWindow GetVisibleContent(int index)
		{
			#if DEBUG
			if (DockPane == null)
				throw new InvalidOperationException();
			#endif

			int currentIndex = -1;
			foreach (IDockableWindow content in DockPane.Contents)
			{
				if (content.DockHandler.DockState == DockPane.DockState)
					currentIndex ++;

				if (currentIndex == index)
					return content;
			}
			throw(new ArgumentOutOfRangeException());
		}

		private int GetIndexOfVisibleContents(IDockableWindow content)
		{
			#if DEBUG
			if (DockPane == null)
				throw new InvalidOperationException();
			#endif

			if (content == null)
				return -1;

			int index = -1;
			foreach (IDockableWindow c in DockPane.Contents)
			{
				if (c.DockHandler.DockState == DockPane.DockState)
				{
					index++;

					if (c == content)
						return index;
				}
			}
			return -1;
		}
	}
}
