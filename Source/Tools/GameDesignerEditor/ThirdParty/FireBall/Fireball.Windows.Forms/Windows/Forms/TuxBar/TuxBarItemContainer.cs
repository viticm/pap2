//    Copyright (C) 2005  Sebastian Faltoni
//	  Copyright (C) 2005  Riccardo Marzi
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

using System;
using System.Drawing;
using System.Windows.Forms;
using Fireball.Windows.Forms.TuxBar.Themes;

namespace Fireball.Windows.Forms.TuxBar
{
	internal class TuxBarItemContainer : Panel
	{
        private TuxTheme m_TuxTheme = null;

        public TuxTheme Theme
        {
            get { return m_TuxTheme; }
            set { m_TuxTheme = value; }
        }

		public TuxBarItemContainer()
		{								
			m_TuxTheme = new FireTheme();

			this.SetStyle(ControlStyles.UserPaint 
				| ControlStyles.ResizeRedraw 
				| ControlStyles.AllPaintingInWmPaint
				| ControlStyles.OptimizedDoubleBuffer
				| ControlStyles.SupportsTransparentBackColor
				, true
				);

			this.BackColor = Color.Transparent;		
		}

	}
}
