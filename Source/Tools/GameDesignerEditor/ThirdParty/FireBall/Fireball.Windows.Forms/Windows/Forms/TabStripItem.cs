//    Copyright (C) 2005  Sebastian Faltoni sebastian(dot)faltoni(at)gmail(dot)com
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
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

namespace Fireball.Windows.Forms
{
    public class TabStripItem : Control
    {
        private DrawItemState _DrawState = DrawItemState.None;

        public DrawItemState DrawState
        {
            get { return _DrawState; }
            set { _DrawState = value; }
        }


        private bool _CanClose = true;

        public bool CanClose
        {
            get { return _CanClose; }
            set { _CanClose = value; }
        }


        private RectangleF _StripRect = Rectangle.Empty;

        internal RectangleF StripRect
        {
            get { return _StripRect; }
            set { _StripRect = value; }
        }


        public TabStripItem()
        {
            base.Dock = DockStyle.Fill;           
        }

        [DefaultValue("Name")]
        public new string Text
        {
            get
            {
                return base.Text;
            }
            set
            {
                base.Text = value;
                //this.Invalidate();
                if (this.Parent != null)
                    this.Parent.Invalidate();
            }
        }

        public override DockStyle Dock
        {
            get
            {
                return base.Dock;
            }
            set
            {
                base.Dock = DockStyle.Fill; ;
            }
        }       
    }
}
