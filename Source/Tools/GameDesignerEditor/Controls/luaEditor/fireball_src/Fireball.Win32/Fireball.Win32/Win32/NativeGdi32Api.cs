
//    Copyright (C) 2005/2006  Sebastian Faltoni <sebastian@dotnetfireball.net>
//
//    Copyright (C) 2005/2006  Riccardo Marzi <riccardo@dotnetfireball.net>
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
using System.Runtime.InteropServices;

namespace Fireball.Win32
{

    public delegate int FONTENUMPROC(ENUMLOGFONTEX f, int lpntme, int FontType, int lParam);

    public static class NativeGdi32Api
    {
        [DllImport("gdi32.dll")]
        public static extern int SetDIBits(IntPtr hdc, IntPtr hBitmap, int nStartScan, int nNumScans, IntPtr lpBits, IntPtr lpBI, int wUsage);

        [DllImport("gdi32.dll")]
        public static extern int SetDIBitsToDevice(IntPtr hdc, int x, int y, int dx, int dy, int SrcX, int SrcY, int Scan, int NumScans, IntPtr Bits, IntPtr BitsInfo, int wUsage);

        [DllImport("gdi32.dll")]
        public static extern IntPtr CreateDIBSection(IntPtr hdc, IntPtr pBitmapInfo, int un, IntPtr lplpVoid, IntPtr handle, int dw);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto)]
        public static extern int SetPixel(IntPtr hdc, int x, int y, int crColor);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto)]
        public static extern int GetPixel(IntPtr hdc, int x, int y);


        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern int CombineRgn(IntPtr dest, IntPtr src1, IntPtr src2, int flags);

        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr CreateBrushIndirect(ref LOGBRUSH brush);


        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr CreateRectRgnIndirect(ref RECTAPI rect);


        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern int GetClipBox(IntPtr hDC, ref RECTAPI rectBox);

        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern bool PatBlt(IntPtr hDC, int x, int y, int width, int height, uint flags);

        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern int SelectClipRgn(IntPtr hDC, IntPtr hRgn);



        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern IntPtr MoveToEx(IntPtr hDC, int x, int y, ref POINTAPI lpPoint);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern IntPtr LineTo(IntPtr hDC, int x, int y);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern IntPtr CreatePen(int nPenStyle, int nWidth, int crColor);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern int SetBrushOrgEx(IntPtr hDC, int x, int y, ref POINTAPI p);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern IntPtr CreatePatternBrush(IntPtr hBMP);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern int GetTextFace(IntPtr hDC, int nCount, string lpFacename);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern int GetTextMetrics(IntPtr hDC, ref GDITextMetric TextMetric);

        [DllImport("gdi32.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern IntPtr CreateFontIndirect([MarshalAs(UnmanagedType.LPStruct)]LogFont LogFont);

        [DllImport("gdi32.dll", SetLastError = true, CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern int BitBlt(IntPtr hDestDC, int x, int y, int nWidth, int nHeight, IntPtr hSrcDC, int xSrc, int ySrc, int dwRop);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern IntPtr CreateSolidBrush(int crColor);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern int Rectangle(IntPtr hDC, int left, int top, int right, int bottom);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern IntPtr CreateHatchBrush(int Style, int crColor);


        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern IntPtr CreateCompatibleBitmap(IntPtr hDC, int nWidth, int nHeight);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern IntPtr CreateCompatibleDC(IntPtr hDC);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern IntPtr SelectObject(IntPtr hDC, IntPtr hObject);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern IntPtr DeleteObject(IntPtr hObject);

        [DllImport("gdi32.dll", SetLastError = true, CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern int GetTextColor(IntPtr hDC);

        [DllImport("gdi32.dll", SetLastError = true, CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern int SetTextColor(IntPtr hDC, int crColor);

        [DllImport("gdi32.dll", SetLastError = true, CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern int GetBkColor(IntPtr hDC);

        [DllImport("gdi32.dll", SetLastError = true, CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern int GetBkMode(IntPtr hDC);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern IntPtr DeleteDC(IntPtr hDC);

        [DllImport("gdi32.dll", SetLastError = true, CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern int SetBkColor(IntPtr hDC, int crColor);

        [DllImport("gdi32.dll", SetLastError = true, CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern int SetBkMode(IntPtr hDC, int Mode);

        [DllImport("gdi32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall,SetLastError=true)]
        public static extern int GdiFlush();

        [DllImport("gdi32.dll", SetLastError = true, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        public static extern int EnumFontFamiliesEx(IntPtr hDC, [MarshalAs(UnmanagedType.LPStruct)] LogFont lf, FONTENUMPROC proc, Int64 LParam, Int64 DW);

        [DllImport("gdi32.dll", EntryPoint = "GdiAlphaBlend")]
        public static extern bool AlphaBlend(
          IntPtr hdcDest,                 // handle to destination DC
          int nXOriginDest,            // x-coord of upper-left corner
          int nYOriginDest,            // y-coord of upper-left corner
          int nWidthDest,              // destination width
          int nHeightDest,             // destination height
          IntPtr hdcSrc,                  // handle to source DC
          int nXOriginSrc,             // x-coord of upper-left corner
          int nYOriginSrc,             // y-coord of upper-left corner
          int nWidthSrc,               // source width
          int nHeightSrc,              // source height
          BLENDFUNCTION blendFunction  // alpha-blending function
        );
    }
}

