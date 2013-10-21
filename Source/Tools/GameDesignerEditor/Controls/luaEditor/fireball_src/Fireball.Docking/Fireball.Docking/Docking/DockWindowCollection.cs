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
	/// <include file='CodeDoc\DockWindowCollection.xml' path='//CodeDoc/Class[@name="DockWindowCollection"]/ClassDef/*'/>>
	public class DockWindowCollection : ReadOnlyCollectionBase
	{
		internal DockWindowCollection(DockContainer dockPanel)
		{
			InnerList.Add(new DockWindow(dockPanel, DockState.Document));
			InnerList.Add(new DockWindow(dockPanel, DockState.DockLeft));
			InnerList.Add(new DockWindow(dockPanel, DockState.DockRight));
			InnerList.Add(new DockWindow(dockPanel, DockState.DockTop));
			InnerList.Add(new DockWindow(dockPanel, DockState.DockBottom));
		}

		/// <include file='CodeDoc\DockWindowCollection.xml' path='//CodeDoc/Class[@name="DockWindowCollection"]/Property[@name="Item"]/*'/>>
		public DockWindow this [DockState dockState]
		{
			get
			{
				if (dockState == DockState.Document)
					return InnerList[0] as DockWindow;
				else if (dockState == DockState.DockLeft || dockState == DockState.DockLeftAutoHide)
					return InnerList[1] as DockWindow;
				else if (dockState == DockState.DockRight || dockState == DockState.DockRightAutoHide)
					return InnerList[2] as DockWindow;
				else if (dockState == DockState.DockTop || dockState == DockState.DockTopAutoHide)
					return InnerList[3] as DockWindow;
				else if (dockState == DockState.DockBottom || dockState == DockState.DockBottomAutoHide)
					return InnerList[4] as DockWindow;

				throw (new ArgumentOutOfRangeException());
			}
		}
	}
}
