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
	/// <include file='CodeDoc\Interfaces.xml' path='//CodeDoc/Interface[@name="IDockContent"]/InterfaceDef/*'/>
	public interface IDockableWindow
	{
		/// <include file='CodeDoc\Interfaces.xml' path='//CodeDoc/Interface[@name="IDockListContainer"]/Property[@name="DockHandler"]/*'/>
		DockContentHandler DockHandler	{	get;	}
	}

	/// <include file='CodeDoc\Interfaces.xml' path='//CodeDoc/Interface[@name="IDockListContainer"]/InterfaceDef/*'/>
	public interface IDockListContainer
	{
		/// <include file='CodeDoc\Interfaces.xml' path='//CodeDoc/Interface[@name="IDockListContainer"]/Property[@name="DockState"]/*'/>
		DockState DockState	{	get;	}
		/// <include file='CodeDoc\Interfaces.xml' path='//CodeDoc/Interface[@name="IDockListContainer"]/Property[@name="DisplayingRectangle"]/*'/>
		Rectangle DisplayingRectangle	{	get;	}
		/// <include file='CodeDoc\Interfaces.xml' path='//CodeDoc/Interface[@name="IDockListContainer"]/Property[@name="DockList"]/*'/>
		DockList DockList	{	get;	}
		/// <include file='CodeDoc\Interfaces.xml' path='//CodeDoc/Interface[@name="IDockListContainer"]/Property[@name="DisplayingList"]/*'/>
		DisplayingDockList DisplayingList	{	get;	}
		/// <include file='CodeDoc\Interfaces.xml' path='//CodeDoc/Interface[@name="IDockListContainer"]/Property[@name="IsDisposed"]/*'/>
		bool IsDisposed	{	get;	}
		/// <include file='CodeDoc\Interfaces.xml' path='//CodeDoc/Interface[@name="IDockListContainer"]/Property[@name="IsFloat"]/*'/>
		bool IsFloat	{	get;	}
	}
}
