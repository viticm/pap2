
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
using System.Drawing;
using System.Windows.Forms;

namespace Fireball.Win32
{

    public static class NativeUser32Api
    {


        public const int GWL_STYLE = -16;
        public const int WS_CHILD = 0x40000000;

        [DllImport("user32.dll")]
        public static extern Int32 SendMessage(IntPtr pWnd, UInt32 uMsg, UInt32 wParam, IntPtr lParam);

        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool AnimateWindow(IntPtr hWnd, uint dwTime, uint dwFlags);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr BeginPaint(IntPtr hWnd, ref PAINTSTRUCT ps);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool ClientToScreen(IntPtr hWnd, ref POINTAPI pt);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool DispatchMessage(ref MSG msg);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool DrawFocusRect(IntPtr hWnd, ref RECTAPI rect);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool EndPaint(IntPtr hWnd, ref PAINTSTRUCT ps);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr GetDC(IntPtr hWnd);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr GetFocus();
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern ushort GetKeyState(int virtKey);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool GetMessage(ref MSG msg, int hWnd, uint wFilterMin, uint wFilterMax);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr GetParent(IntPtr hWnd);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern int GetWindowLong(IntPtr hWnd, int nIndex);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool GetWindowRect(IntPtr hWnd, ref RECTAPI rect);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool HideCaret(IntPtr hWnd);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool InvalidateRect(IntPtr hWnd, ref RECTAPI rect, bool erase);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr LoadCursor(IntPtr hInstance, uint cursor);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool MoveWindow(IntPtr hWnd, int x, int y, int width, int height, bool repaint);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool PeekMessage(ref MSG msg, int hWnd, uint wFilterMin, uint wFilterMax, uint wFlag);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool PostMessage(IntPtr hWnd, int Msg, uint wParam, uint lParam);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool ReleaseCapture();
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern int ReleaseDC(IntPtr hWnd, IntPtr hDC);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool ScreenToClient(IntPtr hWnd, ref POINTAPI pt);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern uint SendMessage(IntPtr hWnd, int Msg, uint wParam, uint lParam);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr SetCursor(IntPtr hCursor);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr SetFocus(IntPtr hWnd);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern int SetWindowLong(IntPtr hWnd, int nIndex, int newLong);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern int SetWindowPos(IntPtr hWnd, IntPtr hWndAfter, int X, int Y, int Width, int Height, uint flags);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool SetWindowRgn(IntPtr hWnd, IntPtr hRgn, bool redraw);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool ShowCaret(IntPtr hWnd);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern int ShowWindow(IntPtr hWnd, short cmdShow);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool SystemParametersInfo(uint uiAction, uint uiParam, ref int bRetValue, uint fWinINI);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool TrackMouseEvent(ref TRACKMOUSEEVENTS tme);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool TranslateMessage(ref MSG msg);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool UpdateLayeredWindow(IntPtr hwnd, IntPtr hdcDst, ref POINTAPI pptDst, ref SIZE psize, IntPtr hdcSrc, ref POINTAPI pprSrc, int crKey, ref BLENDFUNCTION pblend, int dwFlags);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool UpdateWindow(IntPtr hwnd);
        [DllImport("user32.dll", CharSet=CharSet.Auto)]
        public static extern bool WaitMessage();

        [DllImport("user32.dll")]
        public static extern int SendMessage(IntPtr hWnd, int msg, int wParam, COMPOSITIONFORM lParam);

        [DllImport("user32.dll")]
        public static extern int SendMessage(IntPtr hWnd, int msg, int wParam, LogFont lParam);

        [DllImport("user32.dll", SetLastError = false, CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern int DrawText(IntPtr hDC, string lpString, int nCount, ref RECTAPI Rect, int wFormat);


        [DllImport("user32.dll")]
        public static extern IntPtr GetDesktopWindow();

        [DllImport("user32.dll", EntryPoint = "SendMessage")]
        public static extern int SendMessage(IntPtr hWND, int message, int WParam, int LParam);


        [DllImport("user32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern IntPtr GetWindowDC(IntPtr hWND);

        [DllImport("user32.dll")]
        public static extern IntPtr WindowFromDC(int hdc);

        [DllImport("user32.dll", SetLastError = false, CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern int TabbedTextOut(IntPtr hDC, int x, int y, string lpString, int nCount, int nTabPositions, ref int lpnTabStopPositions, int nTabOrigin);

        [DllImport("user32.dll", SetLastError = false, CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern int FillRect(IntPtr hDC, ref RECTAPI rect, IntPtr hBrush);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = false)]
        public static extern uint GetTabbedTextExtent(IntPtr hDC, string lpString, int nCount, int nTabPositions, ref int lpnTabStopPositions);

        [DllImport("user32.dll", SetLastError = false, CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern int InvertRect(IntPtr hDC, ref RECTAPI rect);


        [DllImport("user32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern UInt16 GetAsyncKeyState(int vKey);

        [DllImport("user32.dll")]
        public static extern IntPtr CreateWindowEx(
           uint dwExStyle,
           string lpClassName,
           string lpWindowName,
           uint dwStyle,
           int x,
           int y,
           int nWidth,
           int nHeight,
           IntPtr hWndParent,
           IntPtr hMenu,
           IntPtr hInstance,
           IntPtr lpParam);


        public static bool IsKeyPressed(Keys k)
        {
            int s = (int)GetAsyncKeyState((int)k);
            s = (s & 0x8000) >> 15;
            return (s == 1);
        }

        //---------------------------------------
        //helper , return DC of a control
        public static IntPtr ControlDC(Control control)
        {
            return GetDC(control.Handle);
        }

        //---------------------------------------

        //---------------------------------------
        //helper , convert from and to colors from int values
        public static int ColorToInt(Color color)
        {
            return (color.B << 16 | color.G << 8 | color.R);
        }

        public static Color IntToColor(int color)
        {
            int b = (color >> 16) & 0xFF;
            int g = (color >> 8) & 0xFF;
            int r = (color) & 0xFF;
            return Color.FromArgb(r, g, b);
        }
    }
}

