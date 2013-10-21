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
using Fireball.Win32;

namespace Fireball.Drawing.GDI
{
	//wrapper class for gdi brushes
	public class GDIBrush : GDIObject
	{
		public IntPtr hBrush;
		protected bool mSystemBrush;

		public GDIBrush(Color color)
		{
            hBrush = NativeGdi32Api.CreateSolidBrush(NativeUser32Api.ColorToInt(color));
			Create();
		}


		public GDIBrush(Bitmap pattern)
		{
            hBrush = NativeGdi32Api.CreatePatternBrush(pattern.GetHbitmap());
			Create();
		}

		public GDIBrush(IntPtr hBMP_Pattern)
		{
            hBrush = NativeGdi32Api.CreatePatternBrush(hBMP_Pattern);
			//if (hBrush==(IntPtr)0)
			//Puzzle.Debug.Debugger.WriteLine ("Failed to create brush with color : {0}",color.ToString());

			Create();
		}

		public GDIBrush(int Style, Color color)
		{
            hBrush = NativeGdi32Api.CreateHatchBrush(Style, NativeUser32Api.ColorToInt(color));
			Create();
		}

		public GDIBrush(int BrushIndex)
		{
			hBrush = (IntPtr) BrushIndex;
			mSystemBrush = true;
			Create();
		}

		protected override void Destroy()
		{
			//only destroy if brush is created by us
			if (!mSystemBrush)
			{
				if (hBrush != (IntPtr) 0)
                    NativeGdi32Api.DeleteObject(hBrush);
			}

			base.Destroy();
			hBrush = (IntPtr) 0;
		}

		protected override void Create()
		{
			base.Create();
		}


	}


	//needs to be recoded , cant create new instances for the same colors
	public class GDIBrushes
	{
		public static GDIBrush Black
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush White
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush Red
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush Cyan
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush Green
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush Blue
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush Yellow
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush Orange
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush Purple
		{
			get { return new GDIBrush(0); }
		}
	}

	public class GDISystemBrushes
	{
		public static GDIBrush ActiveBorder
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush ActiveCaption
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush ActiveCaptionText
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush AppWorkspace
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush Control
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush ControlDark
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush ControlDarkDark
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush ControlLight
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush ControlLightLight
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush ControlText
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush Desktop
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush Highlight
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush HighlightText
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush HotTrack
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush InactiveBorder
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush InactiveCaption
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush Info
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush Menu
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush ScrollBar
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush Window
		{
			get { return new GDIBrush(0); }
		}

		public static GDIBrush WindowText
		{
			get { return new GDIBrush(0); }
		}
	}
}