#region Copyright

//    Copyright (C) 2005  Riccardo Marzi  <riccardo@dotnetfireball.net>
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
using Fireball.Win32;

namespace Fireball.Drawing.GDI
{
    public class GDIBitmap:GDIObject
    {
        private IntPtr _hBmp;
        private int _width;
        private int _height;

        private IntPtr _destDC = IntPtr.Zero;

        public GDIBitmap(IntPtr hBmp)
        {
            _hBmp = hBmp;
        }
        public GDIBitmap(string filename)
        {
            FreeImage img = new FreeImage(filename);
            IntPtr deskDC = NativeUser32Api.GetDC(new IntPtr(0));
            _destDC = NativeGdi32Api.CreateCompatibleDC(deskDC);
            IntPtr oldObj = NativeGdi32Api.SelectObject(_destDC, _hBmp);
            _hBmp = NativeGdi32Api.CreateCompatibleBitmap(_destDC, img.Width, img.Height);
            img.PaintToDevice(_destDC, 0, 0, img.Width, img.Height, 0, 0, 0, img.Height, 0);
            NativeGdi32Api.SelectObject(_destDC, oldObj);

            _width = img.Width;
            _height = img.Height;

            NativeGdi32Api.DeleteDC(deskDC);
            //NativeGdi32Api.DeleteDC(destDC);
            img.Dispose();
        }

        public IntPtr Hdc
        {
            get
            {
                return _destDC;
            }
        }

        public IntPtr Handle
        {
            get
            {
                return _hBmp;
            }
        }

        public int Width
        {
            get
            {
                return _width;
            }
        }

        public int Height
        {
            get
            {
                return _height;
            }
        }

        //public void SetPixel(GDIColor color,int x,int y)
        //{
        //    NativeGdi32Api.SetPixel(_destDC, x, y, color.NativeColor);
        //}

        //public GDIColor GetPixel(int x, int y)
        //{
        //    int cl = NativeGdi32Api.GetPixel(_destDC,x,y);
        //    GDIColor color = new GDIColor(cl);
        //    return color;
        //}

        protected override void Destroy()
        {
            if (_hBmp != (IntPtr)0)
            {
                NativeGdi32Api.DeleteObject(_hBmp);
                NativeGdi32Api.DeleteDC(_destDC);
            }

            base.Destroy();
            _hBmp = (IntPtr)0;
        }

        protected override void Create()
        {
            base.Create();
        }
    }


}
