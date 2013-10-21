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
using System.Drawing;
using Fireball.Drawing;
using System.Drawing.Drawing2D;

namespace Fireball.Windows.Forms
{
    class TabStripCloseButton
    {
        private Rectangle crossRect = Rectangle.Empty;
        private bool _IsMouseOver = false;
        private bool _Activated = false;
        static Color _HighLightColor = Color.Empty;

        static TabStripCloseButton()
        {
            ColorHLS hls = new ColorHLS(SystemColors.Highlight);
            hls.Lighten(0.3f);
            hls.Alpha = 150;

            _HighLightColor = hls.Color;
        }

        public bool Activated
        {
            get { return _Activated; }
            set { _Activated = value; }
        }

        public bool IsMouseOver
        {
            get { return _IsMouseOver; }
            set { _IsMouseOver = value; }
        }

        public Rectangle CrossRect
        {
            get { return crossRect; }
            set { crossRect = value; }
        }

        public void DrawCross(Graphics gfx)
        {
            if (_IsMouseOver)
            {
                 

                gfx.FillRectangle(new SolidBrush(_HighLightColor), crossRect);

                Rectangle borderRect = crossRect;

                borderRect.Width--;
                borderRect.Height--;

                gfx.DrawRectangle(SystemPens.Highlight, borderRect);
            }

            using (Pen pen = new Pen(Color.Black, 1.6f))
            {
                gfx.DrawLine(pen, crossRect.Left + 3, crossRect.Top + 3,
                    crossRect.Right - 5, crossRect.Bottom - 4);

                gfx.DrawLine(pen, crossRect.Right - 5, crossRect.Top + 3,
                    crossRect.Left + 3, crossRect.Bottom - 4);
            }
        }
    }
}
