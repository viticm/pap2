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
	/// <include file='CodeDoc/DisplayingDockList.xml' path='//CodeDoc/Class[@name="DisplayingDockList"]/ClassDef/*'/>
	public class DisplayingDockList : ReadOnlyCollectionBase
	{
		private DockList m_dockList;

		internal DisplayingDockList(DockList dockList)
		{
			m_dockList = dockList;
		}

		/// <include file='CodeDoc/DisplayingDockList.xml' path='//CodeDoc/Class[@name="DisplayingDockList"]/Property[@name="DockList"]/*'/>
		public DockList DockList
		{
			get	{	return m_dockList;	}
		}

		/// <include file='CodeDoc\DisplayingDockList.xml' path='//CodeDoc/Class[@name="DisplayingDockList"]/Property[@name="Container"]/*'/>
		public IDockListContainer Container
		{
			get	{	return DockList.Container;	}
		}

		/// <include file='CodeDoc\DisplayingDockList.xml' path='//CodeDoc/Class[@name="DisplayingDockList"]/Property[@name="DockState"]/*'/>
		public DockState DockState
		{
			get	{	return DockList.DockState;	}
		}

		/// <include file='CodeDoc\DisplayingDockList.xml' path='//CodeDoc/Class[@name="DisplayingDockList"]/Property[@name="IsFloat"]/*'/>
		public bool IsFloat
		{
			get	{	return DockList.IsFloat;	}
		}

		/// <include file='CodeDoc\DisplayingDockList.xml' path='//CodeDoc/Class[@name="DisplayingDockList"]/Method[@name="Contains(DockPane)"]/*'/>
		public bool Contains(DockPane pane)
		{
			return InnerList.Contains(pane);
		}

		/// <include file='CodeDoc\DisplayingDockList.xml' path='//CodeDoc/Class[@name="DisplayingDockList"]/Method[@name="IndexOf(DockPane)"]/*'/>
		public int IndexOf(DockPane pane)
		{
			return InnerList.IndexOf(pane);
		}

		/// <include file='CodeDoc\DisplayingDockList.xml' path='//CodeDoc/Class[@name="DisplayingDockList"]/Property[@name="Item"]/*'/>
		public DockPane this[int index]
		{
			get	{	return InnerList[index] as DockPane;	}
		}

		internal void Refresh()
		{
			InnerList.Clear();
			for (int i=0; i<DockList.Count; i++)
			{
				DockPane pane = DockList[i];
				NestedDockingStatus status = pane.NestedDockingStatus;
				status.SetDisplayingStatus(true, status.PrevPane, status.Alignment, status.Proportion);
				InnerList.Add(pane);
			}

			foreach (DockPane pane in DockList)
				if (pane.DockState != DockState || pane.IsHidden)
				{
					pane.Bounds = Rectangle.Empty;
					pane.Splitter.Bounds = Rectangle.Empty;
					Remove(pane);
				}

			CalculateBounds();

			foreach (DockPane pane in this)
			{
				NestedDockingStatus status = pane.NestedDockingStatus;
				pane.Bounds = status.PaneBounds;
				pane.Splitter.Bounds = status.SplitterBounds;
				pane.Splitter.Alignment = status.Alignment;
			}
		}

		private void Remove(DockPane pane)
		{
			if (!Contains(pane))
				return;

			NestedDockingStatus statusPane = pane.NestedDockingStatus;
			DockPane lastNestedPane = null;
			for (int i=Count - 1; i> IndexOf(pane); i--)
			{
				if (this[i].NestedDockingStatus.PrevPane == pane)
				{
					lastNestedPane = this[i];
					break;
				}
			}

			if (lastNestedPane != null)
			{
				int indexLastNestedPane = IndexOf(lastNestedPane);
				InnerList.Remove(lastNestedPane);
				InnerList[IndexOf(pane)] = lastNestedPane;
				NestedDockingStatus lastNestedDock = lastNestedPane.NestedDockingStatus;
				lastNestedDock.SetDisplayingStatus(true, statusPane.DisplayingPrevPane, statusPane.DisplayingAlignment, statusPane.DisplayingProportion);
				for (int i=indexLastNestedPane - 1; i>IndexOf(lastNestedPane); i--)
				{
					NestedDockingStatus status = this[i].NestedDockingStatus;
					if (status.PrevPane == pane)
						status.SetDisplayingStatus(true, lastNestedPane, status.DisplayingAlignment, status.DisplayingProportion);
				}
			}
			else
				InnerList.Remove(pane);

			statusPane.SetDisplayingStatus(false, null, DockAlignment.Left, 0.5);
		}

		private void CalculateBounds()
		{
			if (Count == 0)
				return;

			this[0].NestedDockingStatus.SetDisplayingBounds(Container.DisplayingRectangle, Container.DisplayingRectangle, Rectangle.Empty);

			for (int i=1; i<Count; i++)
			{
				DockPane pane = this[i];
				NestedDockingStatus status = pane.NestedDockingStatus;
				DockPane prevPane = status.DisplayingPrevPane;
				NestedDockingStatus statusPrev = prevPane.NestedDockingStatus;

				Rectangle rect = statusPrev.PaneBounds;
				bool bVerticalSplitter = (status.DisplayingAlignment == DockAlignment.Left || status.DisplayingAlignment == DockAlignment.Right);

				Rectangle rectThis = rect;
				Rectangle rectPrev = rect;
				Rectangle rectSplitter = rect;
				if (status.DisplayingAlignment == DockAlignment.Left)
				{
					rectThis.Width = (int)((double)rect.Width * status.DisplayingProportion) - (Measures.SplitterSize / 2);
					rectSplitter.X = rectThis.X + rectThis.Width;
					rectSplitter.Width = Measures.SplitterSize;
					rectPrev.X = rectSplitter.X + rectSplitter.Width;
					rectPrev.Width = rect.Width - rectThis.Width - rectSplitter.Width;
				}
				else if (status.DisplayingAlignment == DockAlignment.Right)
				{
					rectPrev.Width = (rect.Width - (int)((double)rect.Width * status.DisplayingProportion)) - (Measures.SplitterSize / 2);
					rectSplitter.X = rectPrev.X + rectPrev.Width;
					rectSplitter.Width = Measures.SplitterSize;
					rectThis.X = rectSplitter.X + rectSplitter.Width;
					rectThis.Width = rect.Width - rectPrev.Width - rectSplitter.Width;
				}
				else if (status.DisplayingAlignment == DockAlignment.Top)
				{
					rectThis.Height = (int)((double)rect.Height * status.DisplayingProportion) - (Measures.SplitterSize / 2);
					rectSplitter.Y = rectThis.Y + rectThis.Height;
					rectSplitter.Height = Measures.SplitterSize;
					rectPrev.Y = rectSplitter.Y + rectSplitter.Height;
					rectPrev.Height = rect.Height - rectThis.Height - rectSplitter.Height;
				}
				else if (status.DisplayingAlignment == DockAlignment.Bottom)
				{
					rectPrev.Height = (rect.Height - (int)((double)rect.Height * status.DisplayingProportion)) - (Measures.SplitterSize / 2);
					rectSplitter.Y = rectPrev.Y + rectPrev.Height;
					rectSplitter.Height = Measures.SplitterSize;
					rectThis.Y = rectSplitter.Y + rectSplitter.Height;
					rectThis.Height = rect.Height - rectPrev.Height - rectSplitter.Height;
				}
				else
					rectThis = Rectangle.Empty;

				rectSplitter.Intersect(rect);
				rectThis.Intersect(rect);
				rectPrev.Intersect(rect);
				status.SetDisplayingBounds(rect, rectThis, rectSplitter);
				statusPrev.SetDisplayingBounds(statusPrev.LogicalBounds, rectPrev, statusPrev.SplitterBounds);
			}
		}
	}
}
