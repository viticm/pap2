
//  Copyright (C) 2006  Riccardo Marzi <riccardo@dotnetfireball.org>
//	
//	This library is free software; you can redistribute it and/or
//	modify it under the terms of the GNU Lesser General Public
//	License as published by the Free Software Foundation; either
//	version 2.1 of the License, or (at your option) any later version.
//	
//	This library is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//	Lesser General Public License for more details.
//	
//	You should have received a copy of the GNU Lesser General Public
//	License along with this library; if not, write to the Free Software
//	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace Fireball.Drawing.Drawing2D
{
    public class PixelBufferRenderInfo:IDisposable
    {
        private IntPtr _hdc;
        private Graphics _gfx;

        public PixelBufferRenderInfo(IntPtr hdc)
        {
            _hdc = hdc;
        }
        public PixelBufferRenderInfo(Graphics gfx)
        {
            _gfx = gfx;
        }
        public PixelBufferRenderInfo(Graphics gfx, IntPtr hdc)
        {
            _gfx = gfx;
            _hdc = hdc;
        }

        public IntPtr Hdc
        {
            get
            {
                return _hdc;
            }
        }
        public Graphics Graphics
        {
            get
            {
                return _gfx;
            }
        }

        public void Dispose()
        {
            _hdc = IntPtr.Zero;
        }
        public void Dispose(bool disposeGraphics)
        {
            this.Dispose();
            if (disposeGraphics && _gfx != null)
            {
                _gfx.Dispose();
            }
        }
    }
}
