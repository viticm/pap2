
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
using User32 = Fireball.Win32.NativeUser32Api;
using Gdi32 = Fireball.Win32.NativeGdi32Api;

namespace Fireball.Drawing.Drawing2D
{
    public class PixelBufferManagedGraphics:PixelBufferGraphics
    {
        private Bitmap _bmp;
        private Graphics _gfx;
        //private IntPtr _memDC;

        public PixelBufferManagedGraphics(PixelBuffer pixelBuffer)
            : base(pixelBuffer)
        {
            _bmp = new Bitmap(pixelBuffer.Width, pixelBuffer.Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
            _gfx = Graphics.FromImage(_bmp);

            pixelBuffer.Renderer.RenderBegin += new PixelBufferRenderDelegate(Renderer_RenderBegin);
        }

        #region Events

        void Renderer_RenderBegin(PixelBufferRenderInfo renderInfo)
        {
            pixelBuffer.CopyBitmap(_bmp);
        }

        #endregion

        public override void Dispose()
        {
            if (_gfx != null) _gfx.Dispose();
            //if (_memDC != IntPtr.Zero) Gdi32.DeleteDC(_memDC);
            if (_bmp != null) _bmp.Dispose();
        }

        internal override void ClearGraphics()
        {
            _gfx.Clear(Color.Transparent);
        }

        #region Graphics Functions

        public override void BlendRectangle(int x1, int y1, int x2, int y2, Color color)
        {
            this.FillRectangle(x1, y1, x2, y2, color);
        }
        public override void BlendRectangle(Point pt1, Point pt2, Color color)
        {
            this.FillRectangle(pt1.X, pt1.Y, pt2.X, pt2.Y, color);
        }
        public override void DrawLine(int x1, int y1, int x2, int y2, Color color)
        {
            _gfx.DrawLine(new Pen(color), x1, y1, x2, y2);
            pixelBuffer.CopyBitmap(_bmp);
        }
        public override void DrawLine(Point pt1, Point pt2, Color color)
        {
            this.DrawLine(pt1.X, pt1.Y, pt2.X, pt2.Y, color);
        }
        public override void FillRectangle(int x1, int y1, int x2, int y2, Color color)
        {
            _gfx.FillRectangle(new SolidBrush(color), x1, y1, x2 - x1, y2 - y1);
            //pixelBuffer.CopyBitmap(_bmp);
        }
        public override void FillRectangle(Point pt1, Point pt2, Color color)
        {
            this.FillRectangle(pt1.X, pt1.Y, pt2.X, pt2.Y, color);
        }
        public override void FillEllipse(int x1, int y1, int x2, int y2, Color color)
        {
            //_gfx.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
            _gfx.FillEllipse(new SolidBrush(color), x1, y1, x2-x1, y2-y1 );
        }
        public override void FillEllipse(Point pt1, Point pt2, Color color)
        {
            this.FillEllipse(pt1.X, pt1.Y, pt2.X, pt2.Y, color);
        }

        #endregion
    }
}
