
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
using System.Drawing;

namespace Fireball.Drawing.Drawing2D
{
    public abstract class PixelBufferRenderer:IDisposable
    {
        public abstract event PixelBufferRenderDelegate RenderBegin;
        public abstract event PixelBufferRenderDelegate RenderComplete;

        protected PixelBuffer pixelBuffer;

        private bool _useTransparentBackground;
        private Color _backColor;

        protected PixelBufferRenderer(PixelBuffer pixelBuffer)
        {
            this.pixelBuffer = pixelBuffer;
            _useTransparentBackground = false;
            _backColor = Color.Black;
        }
        ~PixelBufferRenderer()
        {
            this.Dispose();
        }

        public virtual void Dispose()
        {
            pixelBuffer = null;
        }

        public virtual Color BackgroundColor
        {
            get
            {
                return _backColor;
            }
            set
            {
                _backColor = value;
            }
        }
        public virtual bool UseTransparentBackground
        {
            get
            {
                return _useTransparentBackground;
            }
            set
            {
                _useTransparentBackground = value;
            }
        }

        public abstract void Render(PixelBufferRenderInfo renderInfo, int x, int y);
        public abstract void Render(PixelBufferRenderInfo renderInfo, int x, int y, int width, int height);
        public abstract void Render(PixelBufferRenderInfo renderInfo, float x, float y);
        public abstract void Render(PixelBufferRenderInfo renderInfo, float x, float y, float width, float height);

    }
}
