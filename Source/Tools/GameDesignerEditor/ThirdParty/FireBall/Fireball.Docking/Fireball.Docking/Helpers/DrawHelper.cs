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
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Windows.Forms;

namespace Fireball.Docking
{
	internal class DrawHelper
	{
		public static GraphicsPath CalculateGraphicsPathFromBitmap(Bitmap bitmap)
		{
			return CalculateGraphicsPathFromBitmap(bitmap, Color.Empty);
		}

		// From http://edu.cnzz.cn/show_3281.html
		public static GraphicsPath CalculateGraphicsPathFromBitmap(Bitmap bitmap, Color colorTransparent) 
		{ 
			GraphicsPath graphicsPath = new GraphicsPath(); 
			if (colorTransparent == Color.Empty)
				colorTransparent = bitmap.GetPixel(0, 0); 

			for(int row = 0; row < bitmap.Height; row ++) 
			{ 
				int colOpaquePixel = 0;
				for(int col = 0; col < bitmap.Width; col ++) 
				{ 
					if(bitmap.GetPixel(col, row) != colorTransparent) 
					{ 
						colOpaquePixel = col; 
						int colNext = col; 
						for(colNext = colOpaquePixel; colNext < bitmap.Width; colNext ++) 
							if(bitmap.GetPixel(colNext, row) == colorTransparent) 
								break;
 
						graphicsPath.AddRectangle(new Rectangle(colOpaquePixel, row, colNext - colOpaquePixel, 1)); 
						col = colNext; 
					} 
				} 
			} 
			return graphicsPath; 
		} 
	}
}
