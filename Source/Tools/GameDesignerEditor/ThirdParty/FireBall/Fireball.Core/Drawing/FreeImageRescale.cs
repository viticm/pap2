
//    Copyright (C) 2005  Sebastian Faltoni <sebastian@dotnetfireball.net>
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
using Fireball.Drawing.Internal;

namespace Fireball.Drawing
{
    public class FreeImageRescale : FreeImageGeometryTransformation
    {
        private int _height = 0;
        private int _width = 0;
        private FreeImage.FreeImageFilter _Filter;

        public FreeImage.FreeImageFilter Filter
        {
            get { return _Filter; }
            set { _Filter = value; }
        }

        public int Height
        {
            get { return _height; }
            set { _height = value; }
        }

        public int Width
        {
            get { return _width; }
            set { _width = value; }
        }

        internal override void Run(FreeImage image)
        {
            int hwnd = FreeImageApi.Rescale(image.GetFreeImageHwnd(), this.Width
            , this.Height, _Filter);

            image.DisposeAndSetHandle(hwnd);
        }

        public FreeImageRescale(int width, int height, 
            FreeImage.FreeImageFilter filter)
        {
            _width = width;
            _height = height;
        }

        public FreeImageRescale(int width, int height):
            this(width,height, FreeImage.FreeImageFilter.BICUBIC)
        {

        }
    }
}
