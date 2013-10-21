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
using Fireball.Win32;

namespace Fireball.Drawing.GDI
{
	public class GDIFont : GDIObject
	{
		public bool Bold;
		public bool Italic;
		public bool Underline;
		public bool Strikethrough;
		public float Size;
		public string FontName;
		public byte Charset;
		public IntPtr hFont;


		public GDIFont()
		{
			Create();
		}

		public GDIFont(string fontname, float size)
		{
			Init(fontname, size, false, false, false, false);
			Create();
		}

		public GDIFont(string fontname, float size, bool bold, bool italic, bool underline, bool strikethrough)
		{
			Init(fontname, size, bold, italic, underline, strikethrough);
			Create();
		}

		protected void Init(string fontname, float size, bool bold, bool italic, bool underline, bool strikethrough)
		{
			FontName = fontname;
			Size = size;
			Bold = bold;
			Italic = italic;
			Underline = underline;
			Strikethrough = strikethrough;

			LogFont tFont = new LogFont();
			tFont.lfItalic = (byte) (this.Italic ? 1 : 0);
			tFont.lfStrikeOut = (byte) (this.Strikethrough ? 1 : 0);
			tFont.lfUnderline = (byte) (this.Underline ? 1 : 0);
			tFont.lfWeight = this.Bold ? 700 : 400;
			tFont.lfWidth = 0;
			tFont.lfHeight = (int) (-this.Size*1.3333333333333);
			tFont.lfCharSet = 1;

			tFont.lfFaceName = this.FontName;


            hFont = NativeGdi32Api.CreateFontIndirect(tFont);
		}

		~GDIFont()
		{
			Destroy();
		}

		protected override void Destroy()
		{
			if (hFont != (IntPtr) 0)
				NativeGdi32Api.DeleteObject(hFont);
			base.Destroy();
			hFont = (IntPtr) 0;
		}

		protected override void Create()
		{
			base.Create();
		}

	}
}