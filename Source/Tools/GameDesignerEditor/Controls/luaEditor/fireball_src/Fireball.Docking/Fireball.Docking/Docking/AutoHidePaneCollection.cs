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
	/// <include file='CodeDoc\AutoHidePaneCollection.xml' path='//CodeDoc/Class[@name="AutoHidePaneCollection"]/ClassDef/*'/>>
	public sealed class AutoHidePaneCollection : IEnumerable
	{
		#region class AutoHidePaneEnumerator
		private class AutoHidePaneEnumerator : IEnumerator
		{
			private AutoHidePaneCollection m_panes;
			private int m_index;

			public AutoHidePaneEnumerator(AutoHidePaneCollection panes)
			{
				m_panes = panes;
				Reset();
			}

			public bool MoveNext() 
			{
				m_index++;
				return(m_index < m_panes.Count);
			}

			public object Current 
			{
				get	{	return m_panes[m_index];	}
			}

			public void Reset()
			{
				m_index = -1;
			}
		}
		#endregion

		#region IEnumerable Members
		/// <exclude />
		public IEnumerator GetEnumerator()
		{
			return new AutoHidePaneEnumerator(this);
		}
		#endregion

		internal AutoHidePaneCollection(DockContainer panel, DockState dockState)
		{
			m_dockPanel = panel;
			m_states = new AutoHideStateCollection();
			States[DockState.DockTopAutoHide].Selected = (dockState==DockState.DockTopAutoHide);
			States[DockState.DockBottomAutoHide].Selected = (dockState==DockState.DockBottomAutoHide);
			States[DockState.DockLeftAutoHide].Selected = (dockState==DockState.DockLeftAutoHide);
			States[DockState.DockRightAutoHide].Selected = (dockState==DockState.DockRightAutoHide);
		}

		private DockContainer m_dockPanel;
		/// <include file='CodeDoc\AutoHidePaneCollection.xml' path='//CodeDoc/Class[@name="AutoHidePaneCollection"]/Property[@name="DockPanel"]/*'/>>
		public DockContainer DockPanel
		{
			get	{	return m_dockPanel;	}
		}

		private AutoHideStateCollection m_states;
		private AutoHideStateCollection States
		{
			get	{	return m_states;	}
		}

		/// <include file='CodeDoc\AutoHidePaneCollection.xml' path='//CodeDoc/Class[@name="AutoHidePaneCollection"]/Property[@name="Count"]/*'/>>
		public int Count
		{
			get
			{
				int count = 0;
				foreach(DockPane pane in DockPanel.Panes)
				{
					if (States.ContainsPane(pane))
						count ++;
				}

				return count;
			}
		}

		/// <include file='CodeDoc\AutoHidePaneCollection.xml' path='//CodeDoc/Class[@name="AutoHidePaneCollection"]/Property[@name="Item"]/*'/>>
		public AutoHidePane this[int index]
		{
			get
			{	
				int count = 0;
				foreach (DockPane pane in DockPanel.Panes)
				{
					if (!States.ContainsPane(pane))
						continue;

					if (count == index)
						return pane.AutoHidePane;

					count++;
				}
				throw new IndexOutOfRangeException();
			}
		}

		/// <include file='CodeDoc\AutoHidePaneCollection.xml' path='//CodeDoc/Class[@name="AutoHidePaneCollection"]/Method[@name="Contains"]/*'/>>
		/// <include file='CodeDoc\AutoHidePaneCollection.xml' path='//CodeDoc/Class[@name="AutoHidePaneCollection"]/Method[@name="Contains(AutoHidePane)"]/*'/>>
		public bool Contains(AutoHidePane autoHidePane)
		{
			return (IndexOf(autoHidePane) != -1);
		}

		/// <include file='CodeDoc\AutoHidePaneCollection.xml' path='//CodeDoc/Class[@name="AutoHidePaneCollection"]/Method[@name="Contains(DockPane)"]/*'/>>
		public bool Contains(DockPane pane)
		{
			return (IndexOf(pane) != -1);
		}

		/// <include file='CodeDoc\AutoHidePaneCollection.xml' path='//CodeDoc/Class[@name="AutoHidePaneCollection"]/Method[@name="IndexOf"]/*'/>>
		/// <include file='CodeDoc\AutoHidePaneCollection.xml' path='//CodeDoc/Class[@name="AutoHidePaneCollection"]/Method[@name="IndexOf(AutoHidePane)"]/*'/>>
		public int IndexOf(AutoHidePane autoHidePane)
		{
			return IndexOf(autoHidePane.DockPane);
		}

		/// <include file='CodeDoc\AutoHidePaneCollection.xml' path='//CodeDoc/Class[@name="AutoHidePaneCollection"]/Method[@name="IndexOf(DockPane)"]/*'/>>
		public int IndexOf(DockPane pane)
		{
			int index = 0;
			foreach (DockPane dockPane in DockPanel.Panes)
			{
				if (!States.ContainsPane(pane))
					continue;

				if (pane == dockPane)
					return index;

				index++;
			}
			return -1;
		}
	}
}
