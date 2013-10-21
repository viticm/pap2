//    Copyright (C) 2005  Sebastian Faltoni
//	  Copyright (C) 2005  Riccardo Marzi
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
using sys = System;

namespace Fireball.Windows.Forms.TuxBar.Graphic
{
	/// <summary>
	/// Descrizione di riepilogo per Utils.
	/// </summary>
	public sealed class Utils
	{
		public static Color GetLightColor(Color color,double light)
		{
//			double r = color.R + light;
//			double g = color.G + light;
//			double b = color.B + light;
			double r = color.R +((color.R /100)*light);
			double g = color.G +((color.G /100)*light);
			double b = color.B +((color.B /100)*light);

			if(r > 255)
				r = 255;
			if(g > 255)
				g = 255;
			if(b > 255)
				b = 255;

			return Color.FromArgb((int)r,(int)g,(int)b);
		}

		public static string GetCuttedString(string s,sys.Drawing.Font font,
			sys.Drawing.Graphics gfx, int width)
		{
			int stringWidth = (int)gfx.MeasureString(s,font,width).Width;

			if(stringWidth < width)
				return s;

			int charWidth = (int)gfx.MeasureString("#",font,width).Width;

			int finalCut = stringWidth - width;

			finalCut = finalCut / charWidth;

			string temp = s.Substring(0,s.Length - finalCut - 1);

			return temp;
		}

		public static Color GetForeColor(Color backgroundColor)
		{
			int sum = backgroundColor.R + backgroundColor.G +backgroundColor.B;

			int div = sum / 3;

			if(div > 128)
			{
				return Color.Black;
			}
			else
			{
				return Color.White;
			}
		}
	}
}
