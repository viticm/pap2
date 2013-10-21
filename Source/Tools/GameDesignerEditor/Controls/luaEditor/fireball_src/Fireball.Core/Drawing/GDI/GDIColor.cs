#region Copyright

//    Copyright (C) 2005  Sebastian Faltoni  <sebastian@dotnetfireball.net>
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

namespace Fireball.Drawing.GDI
{
    public sealed class GDIColor
    {
        private int _NativeColor = 0;

        public GDIColor(int red, int green, int blue)
        {
            this.calculateColor(red, green, blue);
        }

        public GDIColor(Color color)
        {
            _NativeColor = (color.B << 16 | color.G << 8 | color.R);
            this.calculateColor(color.R, color.G, color.B);
        }

        public GDIColor(int nativeColor)
        {
            this._NativeColor = nativeColor;
        }


        public int Red
        {
            get
            {
              return (this.NativeColor) & 0xFF;
            }
            set
            {
                this.calculateColor(value, this.Green, this.Blue);
            }
        }

        public int Green
        {
            get
            {                
                return  (this.NativeColor >> 8) & 0xFF;
            }
            set
            {
                this.calculateColor(this.Red, value, this.Blue);
            }
        }

        public int Blue
        {
            get
            {
                return (this.NativeColor >> 16) & 0xFF;
            }
            set
            {
                this.calculateColor(this.Red,this.Green,value);
            }
        }

        public int NativeColor
        {
            get
            {
                return _NativeColor;
            }
        }

        private void calculateColor(int r,int g,int b)
        {
            _NativeColor = (b << 16 | g << 8 | r);
        }
    }
}
