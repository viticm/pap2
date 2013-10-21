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

namespace Fireball.Docking
{
	internal class AutoHideState
	{
		public DockState m_dockState;
		public bool m_selected;

		public AutoHideState(DockState dockState)
		{
			m_dockState = dockState;
			m_selected = false;
		}

		public DockState DockState
		{
			get	{	return m_dockState;	}
		}

		public bool Selected
		{
			get	{	return m_selected;	}
			set	{	m_selected = value;	}
		}
	}

	internal class AutoHideStateCollection
	{
		private AutoHideState[] m_states;

		public AutoHideStateCollection()
		{
			m_states = new AutoHideState[]	{	
												new AutoHideState(DockState.DockTopAutoHide),
												new AutoHideState(DockState.DockBottomAutoHide),
												new AutoHideState(DockState.DockLeftAutoHide),
												new AutoHideState(DockState.DockRightAutoHide)
											};
		}

		public int Count
		{
			get	{	return m_states.Length;	}
		}

		public AutoHideState this[DockState dockState]
		{
			get
			{
				for (int i=0; i<m_states.Length; i++)
				{
					if (m_states[i].DockState == dockState)
						return m_states[i];
				}
				throw new IndexOutOfRangeException();
			}
		}

		public bool ContainsPane(DockPane pane)
		{
			if (pane.IsHidden)
				return false;

			for (int i=0; i<m_states.Length; i++)
			{
				if (m_states[i].DockState == pane.DockState && m_states[i].Selected)
					return true;
			}
			return false;
		}

		public void DeselectAll()
		{
			for (int i=0; i<m_states.Length; i++)
				m_states[i].Selected = false;
		}
	}
}