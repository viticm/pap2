
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
    public class PixelBufferManagedRenderer:PixelBufferRenderer
    {
        public override event PixelBufferRenderDelegate RenderBegin;
        public override event PixelBufferRenderDelegate RenderComplete;

        private SolidBrush backColorBrush;

        public PixelBufferManagedRenderer(PixelBuffer pixelBuffer)
            : base(pixelBuffer)
        {
            this.UseTransparentBackground = true;
            this.backColorBrush = new SolidBrush(Color.Black);
        }

        private void RaiseRenderBeginEvent(PixelBufferRenderInfo renderInfo)
        {
            if (this.RenderBegin != null)
            {
                this.RenderBegin(renderInfo);
            }
        }
        private void RaiseRenderCompleteEvent(PixelBufferRenderInfo renderInfo)
        {
            if (this.RenderComplete != null)
            {
                this.RenderComplete(renderInfo);
            }
        }

        public override Color BackgroundColor
        {
            get
            {
                return backColorBrush.Color;
            }
            set
            {
                backColorBrush = new SolidBrush(value);
            }
        }

        public override void Render(PixelBufferRenderInfo renderInfo, int x, int y)
        {
            this.Render(renderInfo, x, y, pixelBuffer.Width, pixelBuffer.Height);
        }
        public override void Render(PixelBufferRenderInfo renderInfo, int x, int y, int width, int height)
        {
            RaiseRenderBeginEvent(renderInfo);

            if (!this.UseTransparentBackground)
            {
                renderInfo.Graphics.FillRectangle(backColorBrush, x, y, width, height);
            }
            using (System.Drawing.Bitmap bmp = pixelBuffer.ToBitmap())
            {
                renderInfo.Graphics.DrawImage(bmp, x, y, width, height);
            }

            RaiseRenderCompleteEvent(renderInfo);
        }
        public override void Render(PixelBufferRenderInfo renderInfo, float x, float y)
        {
            this.Render(renderInfo, x, y, pixelBuffer.Width, pixelBuffer.Height);
        }
        public override void Render(PixelBufferRenderInfo renderInfo, float x, float y, float width, float height)
        {
            RaiseRenderBeginEvent(renderInfo);

            if (!this.UseTransparentBackground)
            {
                renderInfo.Graphics.FillRectangle(backColorBrush, x, y, width, height);
            }
            using (System.Drawing.Bitmap bmp = pixelBuffer.ToBitmap())
            {
                renderInfo.Graphics.DrawImage(bmp, x, y, width, height);
            }

            RaiseRenderCompleteEvent(renderInfo);
        }

        public override string ToString()
        {
            return "{ PixelBufferManagedRenderer }";
        }

    }
}
