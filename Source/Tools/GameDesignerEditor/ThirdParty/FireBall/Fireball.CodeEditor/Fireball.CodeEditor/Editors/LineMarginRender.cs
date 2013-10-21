#region License

//    Copyright (C) 2004  Sebastian Faltoni sebastian(at)dotnetfireball(dot)net
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


#endregion License

using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace Fireball.Windows.Forms
{
    public class LineMarginRender
    {
        protected Brush fillBrush = null;

        private Rectangle _Bounds = Rectangle.Empty;

        public LineMarginRender()
        {
            fillBrush = Brushes.White;
        }

        public virtual Rectangle Bounds
        {
            get
            {
                return _Bounds;
            }
            set
            {

                _Bounds = value;
            }
        }


        public virtual void Render(Graphics gfx)
        {
            if (_Bounds == Rectangle.Empty)
                return;

            gfx.FillRectangle(fillBrush, _Bounds);
        }
    }
}
