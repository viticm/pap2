
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
using System.Runtime.InteropServices;
using Fireball.Win32;

namespace Fireball.Drawing.Drawing2D
{
    public class PixelBufferGraphics:IDisposable
    {
        private const int CLIP_LINE = 0;
        private const int CLIP_LINE_HORIZONTAL = 1;
        private const int CLIP_LINE_VERTICAL = 2;
        private const int CLIP_LINE_DIAGONAL = 3;

        protected PixelBuffer pixelBuffer;

        public PixelBufferGraphics(PixelBuffer pixelBuffer)
        {
            this.pixelBuffer = pixelBuffer;
        }

        ~PixelBufferGraphics()
        {
            this.Dispose();
        }
        public virtual void Dispose()
        {
            pixelBuffer = null;
        }

        internal virtual void ClearGraphics()
        {
        }

        #region Public Members

        public virtual void DrawLine(Point pt1, Point pt2, Color color)
        {
            this.InternalDrawLine(pt1.X, pt1.Y, pt2.X, pt2.Y, color.ToArgb());
        }
        public virtual void DrawLine(int x1, int y1, int x2, int y2, Color color)
        {
            this.InternalDrawLine(x1, y1, x2, y2, color.ToArgb());
        }

        public virtual void BlendRectangle(Point pt1, Point pt2, Color color)
        {
            this.FillRectangle(pt1.X, pt1.Y, pt2.X, pt2.Y, color);
        }
        public virtual void BlendRectangle(int x1, int y1, int x2, int y2, Color color)
        {
            int _x1 = x1;
            int _y1 = y1;
            int _x2 = x2;
            int _y2 = y2;

            if (_x1 > _x2)
            {
                _x1 = x2;
                _x2 = x1;
            }
            if (_y1 > _y2)
            {
                _y1 = y2;
                _y2 = y1;
            }

            pixelBuffer.BlendSubBuffer(color, _x1, _y1, _x2 - _x1, _y2 - _y1);
        }
        public virtual void FillRectangle(Point pt1, Point pt2, Color color)
        {
            this.FillRectangle(pt1.X, pt1.Y, pt2.X, pt2.Y, color);
        }
        public virtual void FillRectangle(int x1, int y1, int x2, int y2, Color color)
        {
            int _x1 = x1;
            int _y1 = y1;
            int _x2 = x2;
            int _y2 = y2;

            if (_x1 > _x2)
            {
                _x1 = x2;
                _x2 = x1;
            }
            if (_y1 > _y2)
            {
                _y1 = y2;
                _y2 = y1;
            }

            pixelBuffer.SetSubBuffer(color, _x1, _y1, _x2 - _x1, _y2 - _y1);
        }

        #endregion

        #region Stubs

        public virtual void DrawRectangle(Point pt1, Point pt2, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawRectangle(int x1, int y1, int x2, int y2, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawEllipse(Point pt1, Point pt2, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawEllipse(int x1, int y1, int x2, int y2, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawArc(Point pt1, Point pt2, Point pt3, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawArc(int x1, int y1, int x2, int y2, int x3, int y3, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawArc(Point pt1, Point pt2, float startAngle, float sweepAngle, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawArc(int x1, int y1, int x2, int y2, float startAngle, float sweepAngle, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawPolyline(Point[] pts, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawPolyline(int[] xPts, int[] yPts, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawPolygon(Point[] pts, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawPolygon(int[] xPts, int[] yPts, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawCurve(Point[] pts, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawCurve(int[] xPts, int[] yPts, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawClosedCurve(Point[] pts, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawClosedCurve(int[] xPts, int[] yPts, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawBezierCurve(Point pt1, Point pt2, Point pt3, Point pt4, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawBezierCurve(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawPie(int x1, int y1, int x2, int y2, int x3, int y3, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void DrawPie(Point pt1, Point pt2, float startAngle, float sweepAngle, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void FillEllipse(Point pt1, Point pt2, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void FillEllipse(int x1, int y1, int x2, int y2, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void FillPolygon(Point[] pts, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void FillPolygon(int[] xPts, int[] yPts, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void FillClosedCurve(Point[] pts, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void FillClosedCurve(int[] xPts, int[] yPts, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void FillPie(int x1, int y1, int x2, int y2, int x3, int y3, Color color)
        {
            throw new NotImplementedException();
        }
        public virtual void FillPie(Point pt1, Point pt2, float startAngle, float sweepAngle, Color color)
        {
            throw new NotImplementedException();
        }

        #endregion

        #region Private Members

        #region Old Code

        //public void DrawLine(int x1, int y1, int x2, int y2, ColorHLS color)
        //{
        //    ColorHLS pixCol = color.Clone();
        //    PixelFillingDelegate pfd = delegate(int x, int y, int count, int total, bool setPixel)
        //    {

        //        if (setPixel)
        //            SetPixel(x, y, pixCol);

        //        return pixCol;
        //    };

        //    DrawLine(x1, y1, x2, y2, pfd);
        //}
        //public void DrawLine(int x1, int y1, int x2, int y2, ColorHLS color1, ColorHLS color2)
        //{
        //    bool created = false;
        //    ColorHLS color = new ColorHLS(color1);
        //    float[,] steps = new float[0, 0];
        //    ColorHLS[] colors = new ColorHLS[0];

        //    PixelFillingDelegate pfd = delegate(int x, int y, int count, int total, bool setPixel)
        //    {
        //        if (!created)
        //        {
        //            steps = ColorUtils.GetGradientColorSteps(color1, color2, total);
        //            created = true;
        //        }

        //        color.SetRGB(
        //            (byte)steps[count, 0],
        //            (byte)steps[count, 1],
        //            (byte)steps[count, 2]
        //            );

        //        if (setPixel)
        //            SetPixel(x, y, color);

        //        return color;
        //    };

        //    DrawLine(x1, y1, x2, y2, pfd);
        //}
        //public void DrawLine(int x1, int y1, int x2, int y2, PixelFillingDelegate pixFillingDelegate)
        //{
        //    int deltax, deltay;

        //    deltax = (x2 - x1);
        //    deltay = (y2 - y1);

        //    if (deltax == 0)
        //    {
        //        if (deltay == 0)
        //        {
        //            return;
        //        }
        //        DrawVerticalLine(x1, y1, deltay, pixFillingDelegate);
        //        return;
        //    }
        //    else if (deltay == 0)
        //    {
        //        DrawHorizontalLine(x1, y1, deltax, pixFillingDelegate);
        //        return;
        //    }

        //    /* Calcoliamo il deltax ed il deltay della linea, ovvero il numero di pixel presenti a livello
        //        orizzontale e verticale. Utilizziamo la funzione abs() poichè a noi interessa il loro 
        //        valore assoluto. */
        //    deltax = Math.Abs(x2 - x1);
        //    deltay = Math.Abs(y2 - y1);

        //    int i, numpixels,
        //        d, dinc1, dinc2,
        //        x, xinc1, xinc2,
        //        y, yinc1, yinc2;

        //    /* Adesso controlliamo se la linea è più "orizzontale" o "verticale", ed inizializziamo
        //       in maniera appropriate le variabili di comodo. */
        //    if (deltax >= deltay)
        //    {
        //        /* La linea risulta maggiormente schiacciata sull' ascissa */
        //        numpixels = deltax + 1;

        //        /* La nostra variabile decisionale, basata sulla x della linea */
        //        d = (2 * deltay) - deltax;

        //        /* Settiamo gli incrementi */
        //        dinc1 = deltay * 2;
        //        dinc2 = (deltay - deltax) * 2;
        //        xinc1 = xinc2 = 1;
        //        yinc1 = 0;
        //        yinc2 = 1;
        //    }
        //    else
        //    {
        //        /* La linea risulta maggiormente schiacciata sull' ordinata*/
        //        numpixels = deltay + 1;

        //        /* La nostra variabile decisionale, basata sulla y della linea */
        //        d = (2 * deltax) - deltay;

        //        /* Settiamo gli incrementi */
        //        dinc1 = deltax * 2;
        //        dinc2 = (deltax - deltay) * 2;
        //        xinc1 = 0;
        //        xinc2 = 1;
        //        yinc1 = yinc2 = 1;
        //    }

        //    /* Eseguiamo un controllo per settare il corretto 
        //        andamento della linea */
        //    if (x1 > x2)
        //    {
        //        xinc1 = -xinc1;
        //        xinc2 = -xinc2;
        //    }
        //    if (y1 > y2)
        //    {
        //        yinc1 = -yinc1;
        //        yinc2 = -yinc2;
        //    }

        //    /* Settiamo le coordinate iniziali  */
        //    x = x1;
        //    y = y1;

        //    /* Stampiamo la linea */
        //    for (i = 1; i < numpixels; i++)
        //    {
        //        //SetPixel(x, y, color);
        //        pixFillingDelegate(x, y, i - 1, numpixels - 1, true);

        //        /* Scegliamo l' incremento del "passo" a seconda dellla
        //            variabile decisionale */
        //        if (d < 0)
        //        {
        //            d = d + dinc1;
        //            x = x + xinc1;
        //            y = y + yinc1;
        //        }
        //        else
        //        {
        //            d = d + dinc2;
        //            x = x + xinc2;
        //            y = y + yinc2;
        //        }
        //    }
        //}

        #endregion

        private bool ClipLine(ref int x1, ref int y1, ref int x2, ref int y2, int clipLineType)
        {
            switch (clipLineType)
            {
                default:
                case CLIP_LINE:
                    // TODO: CLIP_LINE
                    return false;
                case CLIP_LINE_HORIZONTAL:
                    if (y1 < 0 || y1 >= pixelBuffer.Height) return false; // la linea è fuori dal clipBounds
                    if (x1 >= pixelBuffer.Width) return false; // x2 è di conseguenza > di _width
                    if (x2 < 0) return false; // x1 è di conseguenza < di -1
                    if (x1 < 0) x1 = 0;
                    if (x2 >= pixelBuffer.Width) x2 = pixelBuffer.Width - 1;
                    return true;
                case CLIP_LINE_VERTICAL:
                    if (x1 < 0 || x1 >= pixelBuffer.Width) return false; // la linea è fuori dal clipBounds
                    if (y1 >= pixelBuffer.Height) return false; // y2 è di conseguenza > di _height
                    if (y2 < 0) return false; // y1 è di conseguenza < di -1
                    if (y1 < 0) y1 = 0;
                    if (y2 >= pixelBuffer.Height) y2 = pixelBuffer.Height - 1;
                    return true;
                case CLIP_LINE_DIAGONAL:
                    // TODO: CLIP_LINE_DIAGONAL
                    return false;
            }
        }
        private int GetPixelBytesIndex(int x, int y, int width)
        {
            return ((width * y) + x);
        }
        private unsafe void InternalDrawDiagonalLine(int* buff, int x, int y, int delta, bool rtl, int color)
        {
            int start = GetPixelBytesIndex(x, y, pixelBuffer.Width);
            buff += start;

            int offset = rtl ? -1 : 1;

            for (int i = 0; i < delta; i++)
            {
                *buff = color;
                buff += (pixelBuffer.Width + offset);
            }
        }
        private unsafe void InternalDrawLine(int x1, int y1, int x2, int y2, int color)
        {
            int sX = x1;
            int sY = y1;
            int eX = x2;
            int eY = y2;

            if (y2 < y1)
            {
                sX = x2;
                sY = y2;
                eX = x1;
                eY = y1;
            }

            int dX = x2 - x1;
            int dY = y2 - y1;

            fixed (int* src = pixelBuffer.InternalBuffer)
            {
                if (dX == 0)
                {
                    if (!ClipLine(ref sX, ref sY, ref eX, ref eY, CLIP_LINE_VERTICAL)) return;
                    this.InternalDrawVerticalLine(src, sX, sY, sY + Math.Abs(dY), color);
                }
                else if (dY == 0)
                {
                    if (!ClipLine(ref sX, ref sY, ref eX, ref eY, CLIP_LINE_HORIZONTAL)) return;
                    this.InternalDrawHorizontalLine(src, sX, sY, sX + Math.Abs(dX), color);
                }
                else if (dX == dY)
                {
                    if (!ClipLine(ref sX, ref sY, ref eX, ref eY, CLIP_LINE_DIAGONAL)) return;
                    this.InternalDrawDiagonalLine(src, sX, sY, dY, (dX < 0), color);
                }
                else
                {
                    if (!ClipLine(ref sX, ref sY, ref eX, ref eY, CLIP_LINE)) return;
                    this.InternalDrawLine(src, sX, sY, eX, eY, color);
                }
            }

        }
        private unsafe void InternalDrawLine(int* buff, int x1, int y1, int x2, int y2, int color)
        {
            // TODO: DrawLine
            throw new NotImplementedException();
        }
        private unsafe void InternalDrawHorizontalLine(int* buff, int x1, int y, int x2, int color)
        {
            //if (y < 0 || y >= _height) return;
            //if (x1 < 0) x1 = 0;
            //if (x2 >= _width) x2 = _width - 1;

            int len = x2 - x1 + 1;

            int start = this.GetPixelBytesIndex(x1, y, pixelBuffer.Width);
            buff += start;

            for (int i = 0; i < len; i++)
            {
                *((int*)buff) = color;
                buff++;
            }
        }
        private unsafe void InternalDrawVerticalLine(int* buff, int x, int y1, int y2, int color)
        {
            //if (x < 0 || x >= _width) return;
            //if (y1 < 0) y1 = 0;
            //if (y2 >= _height) y2 = _height - 1;

            int len = y2 - y1 + 1;

            int start = this.GetPixelBytesIndex(x, y1, pixelBuffer.Width);
            buff += start;

            for (int i = 0; i < len; i++)
            {
                *((int*)buff) = color;
                buff += pixelBuffer.Width;
            }
        }

        #endregion

    }
}
