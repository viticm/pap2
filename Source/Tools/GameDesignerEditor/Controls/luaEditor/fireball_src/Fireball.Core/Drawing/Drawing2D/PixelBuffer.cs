//  Copyright (C) 2005/2006  Riccardo Marzi <riccardo@dotnetfireball.org>
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
using System.Drawing.Imaging;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Fireball.Collections.Generic;



namespace Fireball.Drawing.Drawing2D
{
    public class PixelBuffer : IDisposable
    {
        #region Nested Classes

        public unsafe class PixelIterator
        {
            private int* _pixel;
            private int _startX;
            private int _startY;

            private int _width;
            private int _height;

            private int _startIndex;

            private int _count;

            private int _relativeX;
            private int _relativeY;
            private int _relativeIndex;

            private int _lineOffset;

            internal PixelIterator(int* buffer, int x, int y,int width,int height, int bufferWidth)
            {
                _startIndex = ((bufferWidth * y) + x);

                buffer += _startIndex;

                _pixel = buffer;
                _startX = x;
                _startY = y;
                _width = width;
                _height = height;
                _count = _width * _height;

                _relativeX = 0;
                _relativeY = 0;
                _relativeIndex = 0;

                _lineOffset = bufferWidth - _width;
            }

            public int RelativeX
            {
                get
                {
                    return _relativeX;
                }
            }
            public int RelativeY
            {
                get
                {
                    return _relativeY;
                }
            }
            public int RelativeIndex
            {
                get
                {
                    return _relativeIndex;
                }
            }

            public int CurrentX
            {
                get
                {
                    return _startX + _relativeX;
                }
            }
            public int CurrenY
            {
                get
                {
                    return _startY + _relativeY;
                }
            }
            public int CurrentIndex
            {
                get
                {
                    return _startIndex + _relativeIndex; 
                }
            }

            public int* CurrentPixel
            {
                get
                {
                    return _pixel;
                }
            }

            public bool MoveNext()
            {
                _relativeIndex++;
                if (_relativeIndex < _count)
                {
                    _relativeX++;
                    if (_relativeX == _width)
                    {
                        _relativeX = 0;
                        _relativeY++;
                        _pixel += _lineOffset;
                    }

                    _pixel++;

                    return true;
                }

                return false;
            }
            public void Reset()
            {
                _pixel -= _relativeIndex;

                _relativeX = 0;
                _relativeY = 0;
                _relativeIndex = 0;

            }

            public int GetAlpha()
            {
                return ((*(_pixel)) >> 24) & 0xff;
            }
            public int GetRed()
            {
                return ((*(_pixel)) >> 16) & 0xff;
            }
            public int GetGreen()
            {
                return ((*(_pixel)) >> 8) & 0xff;
            }
            public int GetBlue()
            {
                return (*(_pixel)) & 0xff;
            }
        }

        #endregion

        #region Enums

        public enum FlipMode
        {
            FlipHorizontal = 1,
            FlipVertical = 2,
            FlipBoth = 3
        }
        public enum ResizePivot
        {
            TopLeft=0,
            TopCenter=1,
            TopRight=2,
            MiddleLeft=3,
            MiddleCenter=4,
            MiddleRight=5,
            BottomLeft=6,
            BottomCenter=7,
            BottomRight=8
        }

        #endregion

        #region Fields

        private int[] _buffer;
        private int _width;
        private int _height;
        private int _bytesCount;
        private int _pixelsCount;

        private PixelBufferGraphics _graphics;

        private PixelBufferRenderer _renderer;

        #endregion

        #region Constructors

        private unsafe PixelBuffer(int width, int height, int* buffer)
        {
            Initialize(width, height);
            fixed (int* dest = _buffer)
            {
                CopyBuffer(dest, buffer, _buffer.Length);
            }
        }
        internal unsafe PixelBuffer(int width, int height, int[] buffer)
        {
            Initialize(width, height);

            fixed (int* dest = _buffer)
            {
                fixed (int* src = buffer)
                {
                    CopyBuffer(dest, src, _buffer.Length);
                }
            }
        }
        public PixelBuffer(int width, int height)
        {
            Initialize(width, height);
        }
        public PixelBuffer(Bitmap bmp)
            : this(bmp.Width, bmp.Height)
        {
            CopyBitmapToBuffer(bmp);
        }

        #endregion

        #region Destructors

        ~PixelBuffer()
        {
            this.Dispose();
        }
        public void Dispose()
        {
            _graphics.Dispose();
            _buffer = null;
        }

        #endregion

        #region Initialize

        private unsafe void Initialize(int width, int height)
        {
            _width = width;
            _height = height;

            _pixelsCount = _width * _height;
            _bytesCount = _pixelsCount * 4;

            _buffer = new int[_pixelsCount];

            _renderer = new PixelBufferManagedRenderer(this);

            //_graphics = new PixelBufferGraphics(this);
            _graphics = new PixelBufferManagedGraphics(this);
            
        }

        #endregion

        #region Private Members

        private int BlendColorByAlpha(int backColor, int foreColor)
        {
            int foreBlue = foreColor & 0xff;
            int foreGreen = (foreColor >> 8) & 0xff;
            int foreRed = (foreColor >> 16) & 0xff;
            int foreAlpha = (foreColor >> 24) & 0xff;

            int backBlue = backColor & 0xff;
            int backGreen = (backColor >> 8) & 0xff;
            int backRed = (backColor >> 16) & 0xff;
            int backAlpha = (backColor >> 24) & 0xff;

            int displayAlpha = foreAlpha * foreAlpha / 255 + backAlpha * (255 - foreAlpha) / 255;
            int displayRed = foreRed * foreAlpha / 255 + backRed * (255 - foreAlpha) / 255;
            int displayGreen = foreGreen * foreAlpha / 255 + backGreen * (255 - foreAlpha) / 255;
            int displayBlue = foreBlue * foreAlpha / 255 + backBlue * (255 - foreAlpha) / 255;

            int color = ((displayAlpha << 24) | (displayRed << 16) | (displayGreen << 8) | displayBlue);

            return color;

        }
        private unsafe void BlendBufferColor(int* dest, int color)
        {
            for (int i = 0; i < _pixelsCount; i++)
            {
                int cDest = *((int*)dest);
                *((int*)dest) = BlendColorByAlpha(cDest, color);
                dest++;
            }
        }
        private unsafe void BlendBufferRectBuffer(int* dest, int* src, int x, int y, int width, int height)
        {
            int lineOffset = (_width - width);
            int start = GetPixelBytesIndex(x, y,_width);
            dest += start;

            for (int iy = 0; iy < height; iy++)
            {
                for (int ix = 0; ix < width; ix++)
                {
                    int cDest = *((int*)dest);
                    int cSrc = *((int*)src);
                    *((int*)dest) = BlendColorByAlpha(cDest, cSrc);
                    dest++;
                    src++;
                }
                dest += lineOffset;
            }
        }
        private unsafe void BlendBufferRectBufferOutside(int[] srcBuffer, int x, int y, int width, int height)
        {
            int sx = 0;
            int sy = 0;
            int sw = width;
            int sh = height;

            if (x < 0)
            {
                sx = -x;
                sw += x;
            }
            else if ((x + width) > _width)
            {
                sx = 0;
                sw = _width - x;
            }

            if (y < 0)
            {
                sy = -y;
                sh += y;
            }
            else if ((y + height) > _height)
            {
                sy = 0;
                sh = _height - y;
            }

            if (sw <= 0 || sh <= 0)
            {
                return;
            }

            //TODO: problema con l'indice del pixel di un buffer senza istanza
            //
            //int[] blitBufferArray = new int[sw * sh];
            //fixed (int* src = srcBuffer)
            //{
            //    fixed (int* dest = blitBufferArray)
            //    {
            //        CopyBufferRect(dest, src, sx, sy, sw, sh);
            //    }
            //}
            PixelBuffer srcBufferHelper = new PixelBuffer(width, height, srcBuffer);
            PixelBuffer blitBufferArray = srcBufferHelper.GetSubBuffer(sx, sy, sw, sh);

            int fx = x;
            int fy = y;
            if (x < 0) fx = 0;
            if (y < 0) fy = 0;

            fixed (int* src = blitBufferArray.InternalBuffer)
            {
                fixed (int* dest = _buffer)
                {
                    BlendBufferRectBuffer(dest, src, fx, fy, sw, sh);
                }
            }

            srcBufferHelper.Dispose();
            blitBufferArray.Dispose();
        }
        private unsafe void BlendBufferRectColor(int* dest, int color, int x, int y, int width, int height)
        {
            int lineOffset = (_width - width);
            int start = GetPixelBytesIndex(x, y,_width);
            dest += start;

            for (int iy = 0; iy < height; iy++)
            {
                for (int ix = 0; ix < width; ix++)
                {
                    int cDest = *((int*)dest);
                    *((int*)dest) = BlendColorByAlpha(cDest, color);
                    dest++;
                }
                dest += lineOffset;
            }
        }
        private unsafe void BlendBufferRectColorOutside(int color, int x, int y, int width, int height)
        {
            int sx = x;
            int sy = y;
            int sw = width;
            int sh = height;

            if (x < 0)
            {
                sx = 0;
                sw += x;
            }
            else if ((x + width) > _width)
            {
                sw = _width - x;
            }

            if (y < 0)
            {
                sy = 0;
                sh += y;
            }
            else if ((y + height) > _height)
            {
                sh = _height - y;
            }

            if (sw <= 0 || sh <= 0)
            {
                return;
            }

            fixed (int* dest = _buffer)
            {
                BlendBufferRectColor(dest, color, sx, sy, sw, sh);
            }

        }
        private unsafe int CountColor(int* buff, int color)
        {
            int count = 0;
            for (int i = 0; i < _pixelsCount; i++)
            {
                if (*((int*)buff) == color)
                {
                    count++;
                }
            }
            return count;
        }
        private unsafe int ChangeColor(int* buff, int oldColor, int newColor)
        {
            int count = 0;
            for (int i = 0; i < _pixelsCount; i++)
            {
                if (*((int*)buff) == oldColor)
                {
                    *((int*)buff) = newColor;
                    count++;
                }
            }
            return count;
        }
        private bool ClipRect(ref int x, ref int y, ref int w, ref int h, int width, int height)
        {
            if (x >= width) return false;
            if (y >= height) return false;
            if ((x + w) <= 0) return false;
            if ((y + h) <= 0) return false;

            if ((x + w) > width) w = width - x;
            if ((y + h) > height) h = height - y;

            if (x < 0)
            {
                w += x;
                x = 0;
            }
            if (y < 0)
            {
                h += y;
                y = 0;
            }

            return true;
        }
        private unsafe void CopyBitmapToBuffer(Bitmap bmp)
        {
            BitmapData bmpData = bmp.LockBits(new Rectangle(0, 0, _width, _height), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            int* src = (int*)bmpData.Scan0;

            fixed (int* dest = _buffer)
            {
                CopyBuffer(dest, src, _buffer.Length);
            }

            bmp.UnlockBits(bmpData);
        }
        private unsafe void CopyBuffer(int* dest, int* src, int lenght)
        {
            for (int i = 0; i < lenght; i++)
            {
                *((int*)dest) = *((int*)src);
                dest++;
                src++;
            }
        }
        private unsafe void CopyBufferRect(int* dest, int* src, int x, int y, int width, int height)
        {
            int lineOffset = (_width - width);
            int start = GetPixelBytesIndex(x, y,_width);
            src += start;

            for (int iy = 0; iy < height; iy++)
            {
                for (int ix = 0; ix < width; ix++)
                {
                    *((int*)dest) = *((int*)src);
                    dest++;
                    src++;
                }
                src += lineOffset;
            }
        }
        private unsafe Bitmap CopyBufferToBitmap()
        {
            Bitmap bmp = new Bitmap(_width, _height, PixelFormat.Format32bppArgb);
            BitmapData bmpData = bmp.LockBits(new Rectangle(0, 0, _width, _height), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
            int* dest = (int*)bmpData.Scan0;

            fixed (int* src = _buffer)
            {
                CopyBuffer(dest, src, _buffer.Length);
            }

            bmp.UnlockBits(bmpData);
            return bmp;
        }
        private unsafe void FillZeroBuffer(int* buff)
        {
            SetBufferColor(buff, 0);
        }
        private unsafe void FlipBufferBoth(int* src, int* dest)
        {
            dest += GetPixelBytesIndex(_width - 1, _height - 1, _width);
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    *((int*)dest) = *((int*)src);
                    src++;
                    dest--;
                }
            }
        }
        private unsafe void FlipBufferHorizontal(int* src, int* dest)
        {
            for (int y = 0; y < _height; y++)
            {
                dest += _width - 1;
                for (int x = 0; x < _width; x++)
                {
                    *((int*)dest) = *((int*)src);
                    src++;
                    dest--;
                }
                dest += _width + 1;
            }
        }
        private unsafe void FlipBufferVertical(int* src, int* dest)
        {
            dest += GetPixelBytesIndex(0, _height - 1,_width);
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    *((int*)dest) = *((int*)src);
                    src++;
                    dest++;
                }
                dest -= _width * 2;
            }
        }
        private unsafe int[] GetBufferColorArray(int* src)
        {
            int[] colors = new int[_pixelsCount];
            for (int i = 0; i < _pixelsCount; i++)
            {
                colors[i] = *((int*)src);
                src++;
            }
            return colors;
        }
        private unsafe int[] GetBufferUsedColorArray(int* src)
        {
            LightCollection<int> colors = new LightCollection<int>();
            for (int i = 0; i < _pixelsCount; i++)
            {
                int c = *((int*)src);
                if (!colors.Contains(c))
                {
                    colors.Add(c);
                }
                src++;
            }
            return colors.GetItems();
        }
        private int GetPixelBytesIndex(int x, int y,int width)
        {
            return ((width * y) + x);
        }
        private unsafe int GetPixelColorByIndex(int* buff, int index)
        {
            buff += index;
            int i = *((int*)buff);

            return i;
        }
        private unsafe PixelBuffer GetSubBufferOutside(int x, int y, int width, int height)
        {
            PixelBuffer retBuff = new PixelBuffer(width, height);

            int sx = x;
            int sy = y;
            int sw = width;
            int sh = height;

            if (x < 0)
            {
                sx = 0;
                sw += x;
            }
            else if ((x + width) > _width)
            {
                sw = _width - x;
            }

            if (y < 0)
            {
                sy = 0;
                sh += y;
            }
            else if ((y + height) > _height)
            {
                sh = _height - y;
            }

            if (sw <= 0 || sh <= 0)
            {
                return new PixelBuffer(width,height);
            }

            int defaultColor = 0;
            fixed (int* src = _buffer)
            {
                int index = this.GetPixelBytesIndex(sx, sy, _width);
                defaultColor = this.GetPixelColorByIndex(src, index);
            }

            fixed (int* dest = retBuff.InternalBuffer)
            {
                retBuff.SetBufferColor(dest, defaultColor);
            }

            using (PixelBuffer subBuff = this.GetSubBuffer(sx, sy, sw, sh))
            {
                int fx = 0;
                int fy = 0;
                if (x < 0) fx = -x;
                if (y < 0) fy = -y;

                retBuff.SetSubBuffer(subBuff, fx, fy);
            }

            return retBuff;
        }
        private unsafe void InvertBufferRGB(int* src)
        {
            for (int i = 0; i < _pixelsCount; i++)
            {
                int color = *src;
                int b = 255 - (color & 0xff);
                int g = 255 - ((color >> 8) & 0xff);
                int r = 255 - ((color >> 16) & 0xff);
                int a = (color >> 24) & 0xff;
                *src = ((a << 24) | (r << 16) | (g << 8) | b);
                src++;
            }
        }
        private unsafe void SetPixelColorByIndex(int* buff, int index, int color)
        {
            buff += index;
            *((int*)buff) = color;
        }
        private unsafe void SetPixels(int* buff, int index, int* offsets, int offsetsCount, int color)
        {
            buff += index;
            for (int i = 0; i < (offsetsCount+1); i++)
            {
                *buff = color;
                buff += *offsets;
                offsets++;
            }
        }
        private unsafe void SetBufferColor(int* buff, int color)
        {
            for (int i = 0; i < _pixelsCount; i++)
            {
                *((int*)buff) = color;
                buff++;
            }
        }
        private unsafe void SetBufferRectBuffer(int* dest, int* src, int x, int y, int width, int height)
        {
            int lineOffset = (_width - width);
            int start = GetPixelBytesIndex(x, y, _width);
            dest += start;

            for (int iy = 0; iy < height; iy++)
            {
                for (int ix = 0; ix < width; ix++)
                {
                    *((int*)dest) = *((int*)src);
                    dest++;
                    src++;
                }
                dest += lineOffset;
            }
        }
        private unsafe void SetBufferRectBufferOutside(int[] srcBuffer, int x, int y, int width, int height)
        {
            int sx = 0;
            int sy = 0;
            int sw = width;
            int sh = height;

            if (x < 0)
            {
                sx = -x;
                sw += x;
            }
            else if ((x + width) > _width)
            {
                sx = 0;
                sw = _width - x;
            }

            if (y < 0)
            {
                sy = -y;
                sh += y;
            }
            else if ((y + height) > _height)
            {
                sy = 0;
                sh = _height - y;
            }

            if (sw <= 0 || sh <= 0)
            {
                return;
            }

            //TODO: problema con l'indice del pixel di un buffer senza istanza
            //
            //int[] blitBufferArray = new int[sw * sh];
            //fixed (int* src = srcBuffer)
            //{
            //    fixed (int* dest = blitBufferArray)
            //    {
            //        CopyBufferRect(dest, src, sx, sy, sw, sh);
            //    }
            //}
            PixelBuffer srcBufferHelper = new PixelBuffer(width, height, srcBuffer);
            PixelBuffer blitBufferArray = srcBufferHelper.GetSubBuffer(sx, sy, sw, sh);

            int fx = x;
            int fy = y;
            if (x < 0) fx = 0;
            if (y < 0) fy = 0;

            fixed (int* src = blitBufferArray.InternalBuffer)
            {
                fixed (int* dest = _buffer)
                {
                    SetBufferRectBuffer(dest, src, fx, fy, sw, sh);
                }
            }

            srcBufferHelper.Dispose();
            blitBufferArray.Dispose();
        }
        private unsafe void SetBufferRectColor(int* dest, int color, int x, int y, int width, int height)
        {
            int lineOffset = (_width - width);
            int start = GetPixelBytesIndex(x, y, _width);
            dest += start;

            for (int iy = 0; iy < height; iy++)
            {
                for (int ix = 0; ix < width; ix++)
                {
                    *((int*)dest) = color;
                    dest++;
                }
                dest += lineOffset;
            }
        }
        private unsafe void SetBufferRectColorOutside(int color, int x, int y, int width, int height)
        {
            int sx = x;
            int sy = y;
            int sw = width;
            int sh = height;

            if (x < 0)
            {
                sx = -x;
                sw += x;
            }
            else if ((x + width) > _width)
            {
                sx = 0;
                sw = _width - x;
            }

            if (y < 0)
            {
                sy = -y;
                sh += y;
            }
            else if ((y + height) > _height)
            {
                sy = 0;
                sh = _height - y;
            }

            if (sw <= 0 || sh <= 0)
            {
                return;
            }

            fixed (int* dest = _buffer)
            {
                SetBufferRectColor(dest, color, sx, sy, sw, sh);
            }

        }

        #endregion

        #region Public Members

        public unsafe void BlendBufferColor(int color)
        {
            fixed (int* dest = _buffer)
            {
                BlendBufferColor(dest, color);
            }
        }
        public unsafe void BlendBufferColor(Color color)
        {
            BlendBufferColor(color.ToArgb());
        }
        public unsafe void BlendSubBuffer(int color, int x, int y, int width, int height)
        {
            if ((x >= _width) || (y >= _height) || ((x + width) <= 0) || ((y + height) <= 0))
            {
                return;
            }
            if ((x < 0) || (y < 0) || ((x + width) > _width) || ((y + height) > _height))
            {
                BlendBufferRectColorOutside(color, x, y, width, height);
                return;
            }

            fixed (int* dest = _buffer)
            {
                BlendBufferRectColor(dest, color, x, y, width, height);
            }
        }
        public unsafe void BlendSubBuffer(Color color, int x, int y, int width, int height)
        {
            BlendSubBuffer(color.ToArgb(), x, y, width, height);
        }
        public unsafe void BlendSubBuffer(int[] subBuffer, int x, int y, int width, int height)
        {
            if ((x >= _width) || (y >= _height) || ((x + width) <= 0) || ((y + height) <= 0))
            {
                return;
            }
            if ((x < 0) || (y < 0) || ((x + width) > _width) || ((y + height) > _height))
            {
                BlendBufferRectBufferOutside(subBuffer, x, y, width, height);
                return;
            }

            fixed (int* src = subBuffer)
            {
                fixed (int* dest = _buffer)
                {
                    BlendBufferRectBuffer(dest, src, x, y, width, height);
                }
            }
        }
        public unsafe void BlendSubBuffer(PixelBuffer subBuffer, int x, int y)
        {
            BlendSubBuffer(subBuffer.InternalBuffer, x, y, subBuffer.Width, subBuffer.Height);
        }
        public unsafe int ChangeColor(int oldColor, int newColor)
        {
            fixed (int* buff = _buffer)
            {
                return ChangeColor(buff, oldColor, newColor);
            }
        }
        public unsafe int ChangeColor(Color oldColor, Color newColor)
        {
            return ChangeColor(oldColor.ToArgb(), newColor.ToArgb());
        }
        public unsafe void ClearBuffer()
        {
            fixed (int* buff = _buffer)
            {
                FillZeroBuffer(buff);
            }
            this._graphics.ClearGraphics();
        }
        public unsafe void ClearBuffer(int color)
        {
            fixed (int* buff = _buffer)
            {
                SetBufferColor(buff, color);
            }
        }
        public unsafe void ClearBuffer(Color color)
        {
            ClearBuffer(color.ToArgb());
        }
        public PixelBuffer Clone()
        {
            return new PixelBuffer(_width, _height, _buffer);
        }
        public unsafe void CopyTo(IntPtr destDeviceData, int length)
        {
            Marshal.Copy(_buffer, 0, destDeviceData, length);
        }
        public unsafe void CopyTo(int[] destDeviceData, int length)
        {
            fixed (int* src = _buffer)
            {
                fixed (int* dest = destDeviceData)
                {
                    CopyBuffer(src, dest, length);
                }
            }
        }
        public void CopyBitmap(Bitmap bmp)
        {
            int w = bmp.Width;
            int h = bmp.Height;
            bool resize = false;
            if (w != _width)
            {
                w = _width;
                resize = true;
            }
            if (h != _height)
            {
                h = _height;
                resize = true;
            }
            if (resize)
            {
                bmp = new Bitmap(bmp, new Size(w, h));
            }
            this.CopyBitmapToBuffer(bmp);
        }
        public unsafe int CountColor(int color)
        {
            fixed (int* buff = _buffer)
            {
                return CountColor(buff, color);
            }
        }
        public unsafe int CountColor(Color color)
        {
            return CountColor(color.ToArgb());
        }
        public unsafe void FlipBuffer(FlipMode flipMode)
        {
            PixelBuffer pb = new PixelBuffer(_width, _height);
            fixed (int* src = _buffer)
            {
                fixed (int* dest = pb.InternalBuffer)
                {
                    switch (flipMode)
                    {
                        default:
                        case FlipMode.FlipHorizontal:
                            FlipBufferHorizontal(src, dest);
                            break;
                        case FlipMode.FlipVertical:
                            FlipBufferVertical(src, dest);
                            break;
                        case FlipMode.FlipBoth:
                            FlipBufferBoth(src, dest);
                            break;
                    }
                }
            }
            fixed (int* src = pb.InternalBuffer)
            {
                fixed (int* dest = _buffer)
                {
                    CopyBuffer(dest, src, _pixelsCount);
                }
            }
            pb.Dispose();
        }
        public unsafe Color[] GetAllPixels()
        {
            int[] icols = new int[0];
            fixed (int* src = _buffer)
            {
                icols = GetBufferColorArray(src);
            }
            Color[] cols = new Color[icols.Length];
            for (int i = 0; i < icols.Length; i++)
            {
                cols[i] = Color.FromArgb(icols[i]);
            }
            return cols;
        }
        public unsafe int[] GetBuffer()
        {
            int[] retBuffer = new int[_pixelsCount];

            fixed (int* src = _buffer)
            {
                fixed (int* dest = retBuffer)
                {
                    CopyBuffer(dest, src, _pixelsCount);
                }
            }
            return retBuffer;
        }
        public unsafe int GetBufferPixel(int index)
        {
            if (index < 0 || index >= _pixelsCount)
            {
                return 0;
            }

            fixed (int* buff = _buffer)
            {
                return GetPixelColorByIndex(buff, index);
            }
        }
        public unsafe int GetBufferPixel(int x, int y)
        {
            if (x < 0 || y < 0 || x >= _width || y >= _height)
            {
                return 0;
            }

            int index = GetPixelBytesIndex(x, y, _width);
            return GetBufferPixel(index);
        }
        public unsafe int[] GetBufferUsedColors()
        {
            int[] icols = new int[0];
            fixed (int* src = _buffer)
            {
                icols = GetBufferUsedColorArray(src);
            }
            return icols;
        }
        public unsafe Color GetPixel(int index)
        {
            return Color.FromArgb(GetBufferPixel(index));
        }
        public unsafe Color GetPixel(int x, int y)
        {
            return Color.FromArgb(GetBufferPixel(x, y));
        }
        public PixelIterator GetPixelIterator()
        {
            return GetPixelIterator(0, 0, _width, _height);
        }
        public PixelIterator GetPixelIterator(Rectangle rect)
        {
            return GetPixelIterator(rect.X, rect.Y, rect.Width, rect.Height);
        }
        public unsafe PixelIterator GetPixelIterator(int x, int y, int width, int height)
        {
            fixed (int* buff = _buffer)
            {
                if (!ClipRect(ref x, ref y, ref width, ref height, _width, _height)) return null;
                return new PixelIterator(buff, x, y, width, height, _width);
            }
        }
        public unsafe PixelBuffer GetSubBuffer(int x, int y, int width, int height)
        {
            if ((x >= _width) || (y >= _height) || ((x + width) <= 0) || ((y + height) <= 0))
            {
                new PixelBuffer(width, height);
            }
            if ((x < 0) || (y < 0) || ((x + width) > _width) || ((y + height) > _height))
            {
                return GetSubBufferOutside(x, y, width, height);
            }

            //TODO: use ClipRect(...)

            int[] pixels = new int[width * height];
            fixed (int* src = _buffer)
            {
                fixed (int* dest = pixels)
                {
                    CopyBufferRect(dest, src, x, y, width, height);
                }
            }
            return new PixelBuffer(width, height, pixels);
        }
        public unsafe Color[] GetUsedColors()
        {
            int[] icols = GetBufferUsedColors();
            Color[] cols = new Color[icols.Length];
            for (int i = 0; i < icols.Length; i++)
            {
                cols[i] = Color.FromArgb(icols[i]);
            }
            return cols;
        }
        public unsafe void InvertBufferRGB()
        {
            fixed (int* src = _buffer)
            {
                InvertBufferRGB(src);
            }
        }
        public void ResizeBuffer(ResizePivot pivot, int width, int height, Color backColor)
        {
            if (width == _width && height == _height) return;

            PixelBuffer newBuffer = new PixelBuffer(width, height);
            newBuffer.ClearBuffer(backColor);

            int dataWidth = Math.Min(width, _width);
            int dataHeight = Math.Min(height, _height);

            int deltaX = Math.Abs(width - _width);
            int deltaY = Math.Abs(height - _height);
            int midDeltaX = (int)Math.Round(deltaX / 2f, MidpointRounding.AwayFromZero);
            int midDeltaY = (int)Math.Round(deltaY / 2f, MidpointRounding.AwayFromZero);

            bool inflateX = (width > _width);
            bool inflateY = (height > _height);

            int srcX = 0;
            int srcY = 0;
            int destX = 0;
            int destY = 0;

            int srcW = width;
            int srcH = height;

            switch (pivot)
            {
                case ResizePivot.TopLeft:
                    srcX = 0;
                    destX = 0;
                    srcY = 0;
                    destY = 0;
                    break;
                case ResizePivot.TopCenter:
                    if (inflateX)
                    {
                        srcX = 0;
                        destX = midDeltaX;
                    }
                    else
                    {
                        srcX = midDeltaX;
                        destX = 0;
                    }
                    srcY = 0;
                    destY = 0;
                    break;
                case ResizePivot.TopRight:
                    if (inflateX)
                    {
                        srcX = 0;
                        destX = deltaX;
                    }
                    else
                    {
                        srcX = deltaX;
                        destX = 0;
                    }
                    srcY = 0;
                    destY = 0;
                    break;
                case ResizePivot.MiddleLeft:
                    srcX = 0;
                    destX = 0;
                    if (inflateY)
                    {
                        srcY = 0;
                        destY = midDeltaY;
                    }
                    else
                    {
                        srcY = midDeltaY;
                        destY = 0;
                    }
                    break;
                case ResizePivot.MiddleCenter:
                    if (inflateX)
                    {
                        srcX = 0;
                        destX = midDeltaX;
                    }
                    else
                    {
                        srcX = midDeltaX;
                        destX = 0;
                    }
                    if (inflateY)
                    {
                        srcY = 0;
                        destY = midDeltaY;
                    }
                    else
                    {
                        srcY = midDeltaY;
                        destY = 0;
                    }
                    break;
                case ResizePivot.MiddleRight:
                    if (inflateX)
                    {
                        srcX = 0;
                        destX = deltaX;
                    }
                    else
                    {
                        srcX = deltaX;
                        destX = 0;
                    }
                    if (inflateY)
                    {
                        srcY = 0;
                        destY = midDeltaY;
                    }
                    else
                    {
                        srcY = midDeltaY;
                        destY = 0;
                    }
                    break;
                case ResizePivot.BottomLeft:
                    srcX = 0;
                    destX = 0;
                    if (inflateY)
                    {
                        srcY = 0;
                        destY = deltaY;
                    }
                    else
                    {
                        srcY = deltaY;
                        destY = 0;
                    }
                    break;
                case ResizePivot.BottomCenter:
                    if (inflateX)
                    {
                        srcX = 0;
                        destX = midDeltaX;
                    }
                    else
                    {
                        srcX = midDeltaX;
                        destX = 0;
                    }
                    if (inflateY)
                    {
                        srcY = 0;
                        destY = deltaY;
                    }
                    else
                    {
                        srcY = deltaY;
                        destY = 0;
                    }
                    break;
                case ResizePivot.BottomRight:
                    if (inflateX)
                    {
                        srcX = 0;
                        destX = deltaX;
                    }
                    else
                    {
                        srcX = deltaX;
                        destX = 0;
                    }
                    if (inflateY)
                    {
                        srcY = 0;
                        destY = deltaY;
                    }
                    else
                    {
                        srcY = deltaY;
                        destY = 0;
                    }
                    break;
            }

            if (inflateX && inflateY)
            {
                newBuffer.SetSubBuffer(_buffer, destX, destY, dataWidth, dataHeight);
            }
            else
            {
                PixelBuffer blitBuffer = this.GetSubBuffer(srcX, srcY, dataWidth, dataHeight);
                newBuffer.SetSubBuffer(blitBuffer, destX, destY);
            }

            this.Initialize(width, height);
            Array.Copy(newBuffer.InternalBuffer, _buffer, _buffer.Length);

            newBuffer.Dispose();

        }
        public unsafe void SetBuffer(int[] buffer)
        {
            fixed (int* src = buffer)
            {
                fixed (int* dest = _buffer)
                {
                    CopyBuffer(dest, src, buffer.Length);
                }
            }
        }
        public unsafe void SetBufferPixel(int index, int color)
        {
            if (index < 0 || index >= _pixelsCount)
            {
                return;
            }
            fixed (int* buff = _buffer)
            {
                SetPixelColorByIndex(buff, index, color);
            }
        }
        public unsafe void SetBufferPixel(int x, int y, int color)
        {
            if (x < 0 || y < 0 || x >= _width || y >= _height)
            {
                return;
            }

            int index = GetPixelBytesIndex(x, y, _width);
            SetBufferPixel(index, color);
        }
        public unsafe void SetPixel(int index, Color color)
        {
            SetBufferPixel(index, color.ToArgb());
        }
        public unsafe void SetPixel(int x, int y, Color color)
        {
            SetBufferPixel(x, y, color.ToArgb());
        }
        public unsafe void SetPixels(int startPixelX, int startPixelY, int[] offsets, Color color)
        {
            int index = GetPixelBytesIndex(startPixelX, startPixelY, _width);
            fixed (int* buff = _buffer)
            {
                fixed (int* offsetsArray = offsets)
                {
                    SetPixels(buff, index, offsetsArray, offsets.Length, color.ToArgb());
                }
            }
        }
        public unsafe void SetSubBuffer(int color, int x, int y, int width, int height)
        {
            if ((x >= _width) || (y >= _height) || ((x + width) <= 0) || ((y + height) <= 0))
            {
                return;
            }
            if ((x < 0) || (y < 0) || ((x + width) > _width) || ((y + height) > _height))
            {
                SetBufferRectColorOutside(color, x, y, width, height);
                return;
            }

            fixed (int* dest = _buffer)
            {
                SetBufferRectColor(dest, color, x, y, width, height);
            }
        }
        public unsafe void SetSubBuffer(Color color, int x, int y, int width, int height)
        {
            SetSubBuffer(color.ToArgb(), x, y, width, height);
        }
        public unsafe void SetSubBuffer(PixelBuffer subBuffer, int x, int y)
        {
            SetSubBuffer(subBuffer.InternalBuffer, x, y, subBuffer.Width, subBuffer.Height);
        }
        public unsafe void SetSubBuffer(int[] subBuffer, int x, int y, int width, int height)
        {
            if ((x >= _width) || (y >= _height) || ((x + width) <= 0) || ((y + height) <= 0))
            {
                return;
            }
            if ((x < 0) || (y < 0) || ((x + width) > _width) || ((y + height) > _height))
            {

                SetBufferRectBufferOutside(subBuffer, x, y, width, height);
                return;
            }

            fixed (int* src = subBuffer)
            {
                fixed (int* dest = _buffer)
                {
                    SetBufferRectBuffer(dest, src, x, y, width, height);
                }
            }
        }
        public void TranslateBuffer(int dx, int dy, Color backColor)
        {
            int absDX = Math.Abs(dx);
            int absDY = Math.Abs(dy);

            if ((absDX > _width) || (absDY > _height))
            {
                this.ClearBuffer(backColor.ToArgb());
                return;
            }

            int newWidth = _width - absDX;
            int newHeight = _height - absDY;

            int srcX = 0;
            int srcY = 0;
            int destX = 0;
            int destY = 0;

            if (dx > 0)
            {
                srcX = 0;
                destX = dx;
            }
            else
            {
                srcX = -dx;
                destX = 0;
            }
            if (dy > 0)
            {
                srcY = 0;
                destY = dy;
            }
            else
            {
                srcY = -dy;
                destY = 0;
            }

            PixelBuffer subBuffer = this.GetSubBuffer(srcX, srcY, newWidth, newHeight);

            this.ClearBuffer(backColor.ToArgb());

            this.SetSubBuffer(subBuffer, destX, destY);

            subBuffer.Dispose();
        }
        public Bitmap ToBitmap()
        {
            return CopyBufferToBitmap();
        }

        public int BytesCount
        {
            get
            {
                return _bytesCount;
            }
        }
        public int BytesPerLine
        {
            get
            {
                return _width * 4;
            }
        }
        public int Height
        {
            get
            {
                return _height;
            }
        }
        public int PixelsCount
        {
            get
            {
                return _pixelsCount;
            }
        }
        public unsafe int this[int pixelIndex]
        {
            get
            {
                return GetBufferPixel(pixelIndex);
            }
        }
        public int Width
        {
            get
            {
                return _width;
            }
        }
        public int[] InternalBuffer
        {
            get
            {
                return _buffer;
            }
        }
        public PixelBufferGraphics Graphics
        {
            get
            {
                return _graphics;
            }
            set
            {
                _graphics = value;
            }
        }
        public PixelBufferRenderer Renderer
        {
            get { return _renderer; }
            set { _renderer = value; }
        }

        #endregion

        #region Stubs

        public void ResizeBuffer(int width, int height, bool stretch)
        {
            if (!stretch)
            {
                ResizeBuffer(ResizePivot.TopLeft, width, height, Color.Transparent);
            }
            else
            {
                throw new NotImplementedException();
            }
        }
        public void RotateBuffer(float angle)
        {
            throw new NotImplementedException();
        }

        //public unsafe PixelBuffer[] InterpolateBuffers(PixelBuffer pixelBuffer1, PixelBuffer pixelBuffer2, int steps)
        //{
        //    IntPtr arrayPtr = Marshal.AllocHGlobal((steps + 2) * 4);
        //    int** array = (int*)(void*)arrayPtr;

        //    PixelBuffer[] buffers = new PixelBuffer[steps + 2];
        //    buffers[0] = pixelBuffer1.Clone();
        //    fixed (int* fbuff = buffers[0].InternalBuffer)
        //    {
        //        *array = fbuff;
        //        array++;
        //    }
        //    for (int i = 1; i < buffers.Length - 1; i++)
        //    {
        //        buffers[i] = new PixelBuffer(pixelBuffer1.Width, pixelBuffer1.Height);
        //        fixed (int* sbuff = buffers[i].InternalBuffer)
        //        {
        //            *array = sbuff;
        //            array++;
        //        }
        //    }
        //    buffers[buffers.Length - 1] = pixelBuffer2.Clone();
        //    fixed (int* ebuff = buffers[buffers.Length - 1].InternalBuffer)
        //    {
        //        *array = ebuff;
        //        array++;
        //    }

        //    array -= buffers.Length;

        //    fixed (int* buff1 = pixelBuffer1.InternalBuffer)
        //    {
        //        fixed (int* buff2 = pixelBuffer2.InternalBuffer)
        //        {
        //            InterpolateBuffers(buff1, buff2, array, pixelBuffer1.PixelsCount);
        //        }
        //    }

        //}
        private unsafe void InterpolateBuffers(int* buffer1, int* buffer2, int* destBufferArray, int count,int steps)
        {

            for (int i = 0; i < count; i++)
            {
                int color1 = *buffer1;
                int b1 = (color1 & 0xff);
                int g1 = ((color1 >> 8) & 0xff);
                int r1 = ((color1 >> 16) & 0xff);
                int a1 = (color1 >> 24) & 0xff;

                int color2 = *buffer2;
                int b2 = (color2 & 0xff);
                int g2 = ((color2 >> 8) & 0xff);
                int r2 = ((color2 >> 16) & 0xff);
                int a2 = (color2 >> 24) & 0xff;

                int dB = ((b2 - b1) / (steps + 1));
                int dG = ((g2 - g1) / (steps + 1));
                int dR = ((r2 - r1) / (steps + 1));
                int dA = ((a2 - a1) / (steps + 1));

                for (int j = 0; j < steps; j++)
                {
                    int* dest = (int*)*destBufferArray;
                    dest += i;
                    
                    *dest = (((a1 + (dA * j)) << 24) | ((r1 + (dR * j)) << 16) | ((g1 + (dG * j)) << 8) | (b1 + (dB * j)));

                    destBufferArray++;
                }

                destBufferArray -= steps;

                buffer1++;
                buffer2++;
            }

        }
        private unsafe void InterpolateBuffers(int* buffer1, int* buffer2, int* destBuffer, int count)
        {
            InterpolateBuffers(buffer1, buffer2, destBuffer, count, 1, 0);
        }
        private unsafe void InterpolateBuffers(int* buffer1, int* buffer2, int* destBuffer, int count,int steps,int index)
        {
            if (index < 0 || index >= steps)
            {
                throw new ArgumentOutOfRangeException("index");
            }

            for (int i = 0; i < count; i++)
            {
                InterpolatePixel(buffer1, buffer2, destBuffer, steps, index);

                buffer1++;
                buffer2++;
                destBuffer++;
            }
        }
        private unsafe void InterpolatePixel(int* pix1, int* pix2,int* destPix, int steps, int index)
        {
            steps++;
            index++;

            int color1 = *pix1;
            int b1 = (color1 & 0xff);
            int g1 = ((color1 >> 8) & 0xff);
            int r1 = ((color1 >> 16) & 0xff);
            int a1 = (color1 >> 24) & 0xff;

            int color2 = *pix2;
            int b2 = (color2 & 0xff);
            int g2 = ((color2 >> 8) & 0xff);
            int r2 = ((color2 >> 16) & 0xff);
            int a2 = (color2 >> 24) & 0xff;

            int b = b1 + (((b2 - b1) / steps) * index);
            int g = g1 + (((g2 - g1) / steps) * index);
            int r = r1 + (((r2 - r1) / steps) * index);
            int a = a1 + (((a2 - a1) / steps) * index);

            *destPix = ((a << 24) | (r << 16) | (g << 8) | b);
        }


        #endregion

    }
}
