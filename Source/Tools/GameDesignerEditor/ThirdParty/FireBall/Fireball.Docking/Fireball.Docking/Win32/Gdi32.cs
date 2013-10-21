#region Fireball License
//    Copyright (C) 2005  Sebastian Faltoni sebastian{at}dotnetfireball{dot}net
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

#endregion
#region Original License
// *****************************************************************************
// 
//  Copyright 2004, Weifen Luo
//  All rights reserved. The software and associated documentation 
//  supplied hereunder are the proprietary information of Weifen Luo
//  and are supplied subject to licence terms.
// 
//  WinFormsUI Library Version 1.0
// *****************************************************************************
#endregion


using System;
using System.Runtime.InteropServices;
using Fireball.Docking.Win32;

namespace Fireball.Docking
{
    internal class Gdi32
    {
        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern int CombineRgn(IntPtr dest, IntPtr src1, IntPtr src2, int flags);

        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr CreateRectRgnIndirect(ref Win32.RECT rect); 

		[DllImport("gdi32.dll", CharSet=CharSet.Auto)]
		public static extern bool FillRgn(IntPtr hDC, IntPtr hrgn, IntPtr hBrush); 
		
		[DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern int GetClipBox(IntPtr hDC, ref Win32.RECT rectBox); 

        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern int SelectClipRgn(IntPtr hDC, IntPtr hRgn); 

        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr CreateBrushIndirect(ref LOGBRUSH brush); 

        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern bool PatBlt(IntPtr hDC, int x, int y, int width, int height, uint flags); 

        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr DeleteObject(IntPtr hObject);

        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern bool DeleteDC(IntPtr hDC);

        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr SelectObject(IntPtr hDC, IntPtr hObject);

        [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
        public static extern IntPtr CreateCompatibleDC(IntPtr hDC);
    }
}