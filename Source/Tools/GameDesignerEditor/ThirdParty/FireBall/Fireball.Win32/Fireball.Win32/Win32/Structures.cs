
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
using System.Drawing;
using System.Runtime.InteropServices;

namespace Fireball.Win32
{
    [StructLayout(LayoutKind.Sequential)]
    public struct BITMAPINFOHEADER //40 bytes
    {
        public int biSize;
        public int biWidth;
        public int biHeight;
        public short biPlanes;
        public short biBitCount;
        public int biCompression;
        public int biSizeImage;
        public int biXPelsPerMeter;
        public int biYPelsPerMeter;
        public int biClrUsed;
        public int biClrImportant;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct RGBQUAD
    {
        public byte rgbBlue;
        public byte rgbGreen;
        public byte rgbRed;
        public byte rgbReserved;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct BITMAPINFO
    {
        public BITMAPINFOHEADER bmiHeader;
        public RGBQUAD bmiColors;

    }

    [StructLayout(LayoutKind.Sequential)]
    public struct LOGBRUSH
    {
        public uint lbStyle;
        public uint lbColor;
        public uint lbHatch;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PAINTSTRUCT
    {
        public IntPtr hdc;
        public int fErase;
        public Rectangle rcPaint;
        public int fRestore;
        public int fIncUpdate;
        public int Reserved1;
        public int Reserved2;
        public int Reserved3;
        public int Reserved4;
        public int Reserved5;
        public int Reserved6;
        public int Reserved7;
        public int Reserved8;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct POINTAPI
    {
        public POINTAPI(int x, int y)
        {
            X = x;
            Y = y;
        }

        public int X;
        public int Y;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct RECTAPI
    {
        public RECTAPI(int left, int top, int right, int bottom)
        {
            Left = left;
            Top = top;
            Right = right;
            Bottom = bottom;
        }

        public int Left;
        public int Top;
        public int Right;
        public int Bottom;
        public int Width
        {
            get
            {
                return Right - Left;
            }
        }
        public int Height
        {
            get
            {
                return Bottom - Top;
            }
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct SIZE
    {
        public int cx;
        public int cy;
    }


    [StructLayout(LayoutKind.Sequential)]
    public struct TRACKMOUSEEVENTS
    {
        public uint cbSize;
        public uint dwFlags;
        public IntPtr hWnd;
        public uint dwHoverTime;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct MSG
    {
        public IntPtr hwnd;
        public int message;
        public IntPtr wParam;
        public IntPtr lParam;
        public int time;
        public int pt_x;
        public int pt_y;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct WINDOWPOS
    {
        public IntPtr hwnd;
        public IntPtr hwndInsertAfter;
        public int x;
        public int y;
        public int cx;
        public int cy;
        public uint flags;
    }

    public struct WindowInfo
    {
        public int Size;
        public Rectangle WindowRect;
        public Rectangle ClientRect;
        public int Style;
        public int ExStyle;
        public int WindowStatus;
        public uint XWindowBorders;
        public uint YWindowBorders;
        public short AtomWindowType;
        public short CreatorVersion;
    }

    public struct WindowPlacement
    {
        public int Length;
        public int Flags;
        public int ShowCmd;
        public Point MinPosition;
        public Point MaxPosition;
        public Rectangle NormalPosition;
    }

    #region _NCCALCSIZE_PARAMS

    [StructLayout(LayoutKind.Sequential)]
    public struct _NCCALCSIZE_PARAMS
    {
        public RECTAPI NewRect;
        public RECTAPI OldRect;
        public RECTAPI OldClientRect;

        public WINDOWPOS lppos;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct MyStruct
    {
        public int SomeValue;
        public byte b1;
        public byte b2;
        public byte b3;
        public byte b4;
        public byte b5;
        public byte b6;
        public byte b7;
        public byte b8;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct BLENDFUNCTION
    {
        public byte BlendOp;
        public byte BlendFlags;
        public byte SourceConstantAlpha;
        public byte AlphaFormat;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct GDITextMetric
    {
        public int tmMemoryHeight;
        public int tmAscent;
        public int tmDescent;
        public int tmInternalLeading;
        public int tmExternalLeading;
        public int tmAveCharWidth;
        public int tmMaxCharWidth;
        public int tmWeight;
        public int tmOverhang;
        public int tmDigitizedAspectX;
        public int tmDigitizedAspectY;
        public byte tmFirstChar;
        public byte tmLastChar;
        public byte tmDefaultChar;
        public byte tmBreakChar;
        public byte tmItalic;
        public byte tmUnderlined;
        public byte tmStruckOut;
        public byte tmPitchAndFamily;
        public byte tmCharSet;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class LogFont
    {
        public int lfHeight = 0;
        public int lfWidth = 0;
        public int lfEscapement = 0;
        public int lfOrientation = 0;
        public int lfWeight = 0;
        public byte lfItalic = 0;
        public byte lfUnderline = 0;
        public byte lfStrikeOut = 0;
        public byte lfCharSet = 0;
        public byte lfOutPrecision = 0;
        public byte lfClipPrecision = 0;
        public byte lfQuality = 0;
        public byte lfPitchAndFamily = 0;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string lfFaceName = "";
    }

    [StructLayout(LayoutKind.Sequential)]
    public class ENUMLOGFONTEX
    {
        public LogFont elfLogFont = null;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string elfFullName = "";
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        public byte[] elfStyle = null;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        public byte[] elfScript = null;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class COMPOSITIONFORM
    {
        public int dwStyle = 0;
        public POINTAPI ptCurrentPos = new POINTAPI();
        public RECTAPI rcArea = new RECTAPI();
    }
    #endregion

    #region Shell Structures

    public struct SHFILEINFO
    {
        public IntPtr hIcon;
        public int iIcon;
        public uint dwAttributes;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
        public string szDisplayName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 80)]
        public string szTypeName;
    }

    #endregion
}
