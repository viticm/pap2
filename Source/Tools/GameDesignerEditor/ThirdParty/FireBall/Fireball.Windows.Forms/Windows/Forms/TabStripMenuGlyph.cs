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
using System.Drawing;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Drawing2D;
using Fireball.Drawing;

namespace Fireball.Windows.Forms
{
    class TabStripMenuGlyph
    {
        private Rectangle glyphRect = Rectangle.Empty;
        private bool _IsMouseOver = false;

        public bool IsMouseOver
        {
            get { return _IsMouseOver; }
            set { _IsMouseOver = value; }
        }

        public Rectangle GlyphRect
        {
            get { return glyphRect; }
            set { glyphRect = value; }
        }

        public void DrawGlyph(Graphics gfx)
        {
            if (_IsMouseOver)
            {
                ColorHLS hls = new ColorHLS(SystemColors.Highlight);
                hls.Lighten(0.3f);
                hls.Alpha = 150;

                gfx.FillRectangle(new SolidBrush(hls.Color), glyphRect);

                Rectangle borderRect = glyphRect;

                borderRect.Width--;
                borderRect.Height--;

                gfx.DrawRectangle(SystemPens.Highlight, borderRect);
            }

            SmoothingMode bak = gfx.SmoothingMode;

            gfx.SmoothingMode = SmoothingMode.Default;

            using (Pen pen = new Pen(Color.Black))
            {
                pen.Width = 2;

                gfx.DrawLine(pen, new Point(glyphRect.Left + (glyphRect.Width / 3) - 2, glyphRect.Height / 2-1),
                    new Point(glyphRect.Right - (glyphRect.Width / 3), glyphRect.Height / 2-1));
            }

            gfx.FillPolygon(Brushes.Black, new Point[]{
                new Point(glyphRect.Left + (glyphRect.Width / 3)-2, glyphRect.Height / 2+2),
                new Point(glyphRect.Right - (glyphRect.Width / 3), glyphRect.Height / 2+2),
                new Point(glyphRect.Left + glyphRect.Width / 2-1,glyphRect.Bottom-4)});

            gfx.SmoothingMode = bak;
        }


    }
}
