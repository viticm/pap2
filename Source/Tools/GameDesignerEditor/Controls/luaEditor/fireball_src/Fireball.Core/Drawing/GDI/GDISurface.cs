#region Copyright

//    Copyright (C) 2005  Sebastian Faltoni <sebastian@dotnetfireball.net>
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

using System;
using System.Drawing;
using System.Windows.Forms;
using Fireball.Win32;

namespace Fireball.Drawing.GDI
{
	public class GDISurface : GDIObject
	{
		protected IntPtr mhDC;
		protected IntPtr mhBMP;
		protected int mWidth;
		protected int mHeight;
		protected int mTabSize = 4;
		protected IntPtr _OldFont = IntPtr.Zero;
		protected IntPtr _OldPen = IntPtr.Zero;
		protected IntPtr _OldBrush = IntPtr.Zero;
		protected IntPtr _OldBmp = IntPtr.Zero;


		private WeakReference _Control = null;

		private Control Control
		{
			get
			{
				if (_Control != null)
					return (Control) _Control.Target;
				else
					return null;
			}
			set { _Control = new WeakReference(value); }
		}

		public GDISurface(IntPtr hDC)
		{
			mhDC = hDC;
		}

		public GDISurface(int width, int height)
		{
            // added: 31/01/06
			//TODO: test it
            IntPtr deskDC = NativeUser32Api.GetDC(new IntPtr(0));
            Init(width, height, deskDC);
            Create();
		}

		public GDISurface(int width, int height, IntPtr hdc)
		{
			Init(width, height, hdc);
			Create();
		}

		protected void Init(int width, int height, IntPtr hdc)
		{
			mWidth = width;
			mHeight = height;
            mhDC = NativeGdi32Api.CreateCompatibleDC(hdc);

            mhBMP = NativeGdi32Api.CreateCompatibleBitmap(hdc, width, height);

            IntPtr ret = NativeGdi32Api.SelectObject(mhDC, mhBMP);
			_OldBmp = ret;

			if (mhDC == (IntPtr) 0)
				MessageBox.Show("hDC creation FAILED!!");

			if (mhDC == (IntPtr) 0)
				MessageBox.Show("hBMP creation FAILED!!");


		}

		public GDISurface(int width, int height, GDISurface surface)
		{
			Init(width, height, surface.hDC);
			Create();
		}


		public GDISurface(int width, int height, Control compatibleControl, bool bindControl)
		{
            IntPtr hDCControk = NativeUser32Api.ControlDC(compatibleControl);
			Init(width, height, hDCControk);
            NativeUser32Api.ReleaseDC(compatibleControl.Handle, hDCControk);

			if (bindControl)
			{
				Control = compatibleControl;
			}
			else
			{
			}

			Create();
		}


		public IntPtr hDC
		{
			get { return mhDC; }
		}

		public IntPtr hBMP
		{
			get { return mhBMP; }
		}

		public Color TextForeColor
		{
			//map get,settextcolor
            get { return NativeUser32Api.IntToColor(NativeGdi32Api.GetTextColor(mhDC)); }
            set { NativeGdi32Api.SetTextColor(mhDC, NativeUser32Api.ColorToInt(value)); }
		}

		public Color TextBackColor
		{
			//map get,setbkcolor
            get { return NativeUser32Api.IntToColor(NativeGdi32Api.GetBkColor(mhDC)); }
            set { NativeGdi32Api.SetBkColor(mhDC, NativeUser32Api.ColorToInt(value)); }
		}


		public bool FontTransparent
		{
			//map get,setbkmode
			//1=transparent , 2=solid
            get { return NativeGdi32Api.GetBkMode(mhDC) < 2; }
            set { NativeGdi32Api.SetBkMode(mhDC, value ? 1 : 2); }
		}


		public Size MeasureString(string Text)
		{
			//map GetTabbedTextExtent
			//to be implemented
			return new Size(0, 0);
		}

		public Size MeasureTabbedString(string Text, int tabsize)
		{
            uint ret = NativeUser32Api.GetTabbedTextExtent(mhDC, Text, Text.Length, 1, ref tabsize);
			return new Size((int)(ret & 0xFFFF), (int)((ret >> 16) & 0xFFFF));
		}

		public void DrawString(string Text, int x, int y, int width, int height)
		{
			//to be implemented
			//map DrawText

		}

		public Size DrawTabbedString(string Text, int x, int y, int taborigin, int tabsize)
		{
            int ret = NativeUser32Api.TabbedTextOut(mhDC, x, y, Text, Text.Length, 1, ref tabsize, taborigin);
			return new Size(ret & 0xFFFF, (ret >> 16) & 0xFFFF);
		}


		//---------------------------------------
		//render methods , 
		//render to dc ,
		//render to control
		//render to gdisurface

		public void RenderTo(IntPtr hdc, int x, int y)
		{
            NativeGdi32Api.BitBlt(hdc, x, y, mWidth, mHeight, mhDC, 0, 0, (int) GDIRop.SrcCopy);
		}


		public void RenderTo(GDISurface target, int x, int y)
		{
			RenderTo(target.hDC, x, y);
		}

		public void RenderTo(GDISurface target, int SourceX, int SourceY, int Width, int Height, int DestX, int DestY)
		{
            NativeGdi32Api.BitBlt(target.hDC, DestX, DestY, Width, Height, this.hDC, SourceX, SourceY, (int)GDIRop.SrcCopy);
		}

		public void RenderToControl(int x, int y)
		{
            IntPtr hdc = NativeUser32Api.ControlDC(Control);

			RenderTo(hdc, x, y);
            NativeUser32Api.ReleaseDC(Control.Handle, hdc);
		}

		//---------------------------------------

		public Graphics CreateGraphics()
		{
			return Graphics.FromHdc(mhDC);
		}

		//---------------------------------------

		public GDIFont Font
		{
			get
			{
				GDITextMetric tm = new GDITextMetric();
				string fontname = "                                                ";

                NativeGdi32Api.GetTextMetrics(mhDC, ref tm);
                NativeGdi32Api.GetTextFace(mhDC, 79, fontname);

				GDIFont gf = new GDIFont();
				gf.FontName = fontname;
				gf.Bold = (tm.tmWeight > 400); //400=fw_normal
				gf.Italic = (tm.tmItalic != 0);
				gf.Underline = (tm.tmUnderlined != 0);
				gf.Strikethrough = (tm.tmStruckOut != 0);

				gf.Size = (int) (((double) (tm.tmMemoryHeight)/(double) tm.tmDigitizedAspectY)*72);
				return gf;
			}
			set
			{
                IntPtr res = NativeGdi32Api.SelectObject(mhDC, value.hFont);
				if (_OldFont == IntPtr.Zero)
					_OldFont = res;
			}
		}

		public void FillRect(GDIBrush brush, int x, int y, int width, int height)
		{
            RECTAPI gr;
			gr.Top = y;
			gr.Left = x;
			gr.Right = width + x;
			gr.Bottom = height + y;

            NativeUser32Api.FillRect(mhDC, ref gr, brush.hBrush);
		}

		public void DrawFocusRect(int x, int y, int width, int height)
		{
            RECTAPI gr;
			gr.Top = y;
			gr.Left = x;
			gr.Right = width + x;
			gr.Bottom = height + y;

            NativeUser32Api.DrawFocusRect(mhDC, ref gr);
		}

		public void FillRect(Color color, int x, int y, int width, int height)
		{
			GDIBrush b = new GDIBrush(color);
			FillRect(b, x, y, width, height);
			b.Dispose();
		}

		public void InvertRect(int x, int y, int width, int height)
		{
            RECTAPI gr;
			gr.Top = y;
			gr.Left = x;
			gr.Right = width + x;
			gr.Bottom = height + y;

            NativeUser32Api.InvertRect(mhDC, ref gr);
		}

		public void DrawLine(GDIPen pen, Point p1, Point p2)
		{
            IntPtr oldpen = NativeGdi32Api.SelectObject(mhDC, pen.hPen);
			POINTAPI gp;
			gp.X = 0;
			gp.Y = 0;
            NativeGdi32Api.MoveToEx(mhDC, p1.X, p1.Y, ref gp);
            NativeGdi32Api.LineTo(mhDC, p2.X, p2.Y);
            IntPtr crap = NativeGdi32Api.SelectObject(mhDC, oldpen);
		}

		public void DrawLine(Color color, Point p1, Point p2)
		{
			GDIPen p = new GDIPen(color, 1);
			DrawLine(p, p1, p2);
			p.Dispose();
		}

		public void DrawRect(Color color, int left, int top, int width, int height)
		{
			GDIPen p = new GDIPen(color, 1);
			this.DrawRect(p, left, top, width, height);
			p.Dispose();
		}

		public void DrawRect(GDIPen pen, int left, int top, int width, int height)
		{
			this.DrawLine(pen, new Point(left, top), new Point(left + width, top));
			this.DrawLine(pen, new Point(left, top + height), new Point(left + width, top + height));
			this.DrawLine(pen, new Point(left, top), new Point(left, top + height));
			this.DrawLine(pen, new Point(left + width, top), new Point(left + width, top + height + 1));
		}

		public void Clear(Color color)
		{
			GDIBrush b = new GDIBrush(color);
			Clear(b);
			b.Dispose();
		}

		public void Clear(GDIBrush brush)
		{
			FillRect(brush, 0, 0, mWidth, mHeight);
		}

		public void Flush()
		{
			NativeGdi32Api.GdiFlush();
		}

		protected override void Destroy()
		{
			if (_OldBmp != IntPtr.Zero)
                NativeGdi32Api.SelectObject(this.hDC, _OldBmp);

			if (_OldFont != IntPtr.Zero)
                NativeGdi32Api.SelectObject(this.hDC, _OldFont);

			if (_OldPen != IntPtr.Zero)
                NativeGdi32Api.SelectObject(this.hDC, _OldPen);

			if (_OldBrush != IntPtr.Zero)
                NativeGdi32Api.SelectObject(this.hDC, _OldBrush);

			if (mhBMP != (IntPtr) 0)
                NativeGdi32Api.DeleteObject(mhBMP);

			if (mhDC != (IntPtr) 0)
                NativeGdi32Api.DeleteDC(mhDC);

			mhBMP = (IntPtr) 0;
			mhDC = (IntPtr) 0;

			base.Destroy();
		}

		public void SetBrushOrg(int x, int y)
		{
            POINTAPI p;
			p.X = 0;
			p.Y = 0;
            NativeGdi32Api.SetBrushOrgEx(mhDC, x, y, ref p);
		}

		protected override void Create()
		{
			base.Create();
		}

        

	}
}