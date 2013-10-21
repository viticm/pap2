#region Copyright
//    Copyright (C) 2005  Sebastian Faltoni
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

using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace Fireball.Printing
{
    public class PrintPen
    {
        private float _Width = 1.0f;
        private PrintColor _Color = new PrintColor(255,0,0,0);

        private float[] _DashStyle = new float[0];

        public float[] DashStyle
        {
            get { return _DashStyle; }
            set { _DashStyle = value; }
        }

        public PrintColor Color
        {
            get { return _Color; }
            set { _Color = value; }
        }

        public float Width
        {
            get { return _Width; }
            set { _Width = value; }
        }

        public PrintPen(float width, float[] dashStyle, PrintColor color)
        {
            this.Width = width;
            this.DashStyle = dashStyle;
            this.Color = color;
        }


        public PrintPen()
        {

        }
    }
}
