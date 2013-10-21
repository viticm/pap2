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
using System.Drawing;

namespace Fireball.Docking
{
	/// <include file='CodeDoc/AutoHidePane.xml' path='//CodeDoc/Class[@name="AutoHidePane"]/ClassDef/*'/>
	public class AutoHidePane : IDisposable
	{
		private DockPane m_dockPane;

		/// <include file='CodeDoc/AutoHidePane.xml' path='//CodeDoc/Class[@name="AutoHidePane"]/Construct[@name="(DockPane)"]/*'/>
		public AutoHidePane(DockPane pane)
		{
			m_dockPane = pane;
			m_tabs = new AutoHideTabCollection(DockPane);
		}

		/// <exclude/>
		~AutoHidePane()
		{
			Dispose(false);
		}

		/// <include file='CodeDoc/AutoHidePane.xml' path='//CodeDoc/Class[@name="AutoHidePane"]/Property[@name="DockPane"]/*'/>
		public DockPane DockPane
		{
			get	{	return m_dockPane;	}
		}

		private AutoHideTabCollection m_tabs;
		/// <include file='CodeDoc/AutoHidePane.xml' path='//CodeDoc/Class[@name="AutoHidePane"]/Property[@name="Tabs"]/*'/>
		public AutoHideTabCollection Tabs
		{
			get	{	return m_tabs;	}
		}

		/// <include file='CodeDoc/AutoHidePane.xml' path='//CodeDoc/Class[@name="AutoHidePane"]/Method[@name="Dispose"]/*'/>
		/// <include file='CodeDoc/AutoHidePane.xml' path='//CodeDoc/Class[@name="AutoHidePane"]/Method[@name="Dispose()"]/*'/>
		public void Dispose()
		{
			Dispose(true);
		}

		/// <include file='CodeDoc/AutoHidePane.xml' path='//CodeDoc/Class[@name="AutoHidePane"]/Method[@name="Dispose(bool)"]/*'/>
		protected virtual void Dispose(bool disposing)
		{
		}
	}
}
