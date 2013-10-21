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
	internal class DockPaneTabVS2005 : DockPaneTab
	{
        internal DockPaneTabVS2005(IDockableWindow content)
            : base(content)
		{

		}

		private int m_tabX;
		protected internal int TabX
		{
			get	{	return m_tabX;	}
			set	{	m_tabX = value;	}
		}

		private int m_tabWidth;
        protected internal int TabWidth
        {
            get { return m_tabWidth; }
            set { m_tabWidth = value; }
        }

		private int m_maxWidth;
		protected internal int MaxWidth
		{
			get	{	return m_maxWidth;	}
			set	{	m_maxWidth = value;	}
		}

		private bool m_flag;
		protected internal bool Flag
		{
			get	{	return m_flag;	}
			set	{	m_flag = value;	}
		}
	}
}
