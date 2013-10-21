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

namespace Fireball.Printing
{
    public class Rect
    {
        private float _x;
        private float _y;
        private float _height;
        private float _width;

        public static readonly Rect Empty;

        static Rect()
        {
            Rect.Empty = new Rect(0,0,0,0);
        }

        public Rect(float x, float y, float width, float height)
        {
            this._x = x;
            this._y = y;
            this._width = width;
            this._height = height;
        }


        public float Right
        {
            get
            {
                return this.X + this.Width;
            }
        }

        public float Left
        {
            get
            {
                return this.X;
            }
        }

        public float Top
        {
            get
            {
                return this.Y;
            }
        }

        public float Bottom
        {
            get
            {
                return this.Y + this.Height;
            }
        }

        public float X
        {
            get { return _x; }
            set { _x = value; }
        }
        
        public float Y
        {
            get { return _y; }
            set { _y = value; }
        }

        public float Width
        {
            get { return _width; }
            set { _width = value; }
        }
        
        public float Height
        {
            get { return _height; }
            set { _height = value; }
        }

        public void Extend(Rect rect)
        {
            if (this.Width < rect.Width)
                this.Width = rect.Width;

            if (this.Height < rect.Height)
                this.Height = rect.Height;
        }
    }
}
