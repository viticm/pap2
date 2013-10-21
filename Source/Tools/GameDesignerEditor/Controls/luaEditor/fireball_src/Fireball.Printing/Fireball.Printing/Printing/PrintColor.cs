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
    public class PrintColor
    {
        private byte alpha;
        private byte red;
        private byte green;
        private byte blue;

        public byte Blue
        {
            get { return blue; }
            set { blue = value; }
        }

        public byte Green
        {
            get { return green; }
            set { green = value; }
        }

        public byte Red
        {
            get { return red; }
            set { red = value; }
        }

        public byte Alpha
        {
            get { return alpha; }
            set { alpha = value; }
        }

        public PrintColor(byte alpha, byte red, byte green, byte blue)
        {
            this.alpha = alpha;
            this.red = red;
            this.green = green;
            this.blue = blue;
        }

        public PrintColor(byte red, byte green, byte blue)
            :this(255,red,green,blue)
        {

        }

        public Color ToSystemDrawingColor()
        {
            return Color.FromArgb((int)this.Alpha, (int)this.Red, (int)this.Green, (int)this.Blue);
        }


        public static PrintColor Empty;


        static PrintColor()
        {
            Empty = new PrintColor(255, 0, 0, 0);
        }
    }
}
