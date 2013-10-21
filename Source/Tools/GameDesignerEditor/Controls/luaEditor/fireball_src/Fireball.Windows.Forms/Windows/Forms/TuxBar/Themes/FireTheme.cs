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
using System.Drawing.Drawing2D;
using Fireball.Windows.Forms.TuxBar.Graphic;
using Fireball.Windows.Forms.TuxBar;

namespace Fireball.Windows.Forms.TuxBar.Themes
{
	public class FireTheme : TuxTheme
	{

		private Color BaseColor
		{
			get
			{
				return SystemColors.Highlight;
			}
		}

        public override void DrawTuxBarItemHeaderText(Graphics gfx, Rectangle dest, string s)
        {
            Font font = new Font("Arial", 10, FontStyle.Bold);

            string real = TuxBar.Graphic.Utils.GetCuttedString(s, font, gfx, dest.Width);

            StringFormat sf = new StringFormat();

            sf.LineAlignment = StringAlignment.Center;

            sf.Trimming = StringTrimming.EllipsisCharacter;

            sf.FormatFlags = StringFormatFlags.NoWrap;

            SolidBrush sbText = new SolidBrush(TuxBar.Graphic.Utils.GetForeColor(BaseColor));

            Rectangle newDest = dest;

            gfx.DrawString(s, font, sbText, newDest, sf);
        }

		public override void DrawTuxBarItemHeaderBK(Graphics gfx, Rectangle dest, HeaderState state)
		{
			gfx.SmoothingMode = SmoothingMode.HighQuality;

			GraphicsPath path = new GraphicsPath();

			int right = (dest.X + dest.Width);
			int left = dest.X;
			int bottom = dest.X + dest.Height + 1;

			path.AddArc(left, dest.Y, 20, 20, 180, 90);
			path.AddArc(right - 20, 0, 20, 20, 270, 90);
			path.AddLine(right, 20, right, bottom);

			
				path.AddLine(right, bottom, 0, bottom);
	

			//else



			path.AddLine(left, bottom, left, 20);
			path.CloseFigure();

			Color color2 = TuxBar.Graphic.Utils.GetLightColor(BaseColor, 220.5f);

			LinearGradientBrush lnb = new LinearGradientBrush(dest, BaseColor, color2, 45);

			//gfx.FillRectangle( Brushes.Green,dest);
			gfx.FillPath(lnb, path);

			//if (state == HeaderState.Collapsed)
			gfx.DrawPath(Pens.Black, path);
			/*else
				gfx.DrawPath(Pens.DarkBlue, path);*/

			/*if (state == HeaderState.Collapsed)
				gfx.DrawLine(new Pen(Utils.GetLightColor(Color.Black,100.0f)), 2, bottom - 1, right-1, bottom - 1);*/


			path = new GraphicsPath();

			dest.Y++;
			right = (dest.X + dest.Width) - 2;
			left = dest.X + 1;
			bottom = dest.X + dest.Height + 2;

			path.AddArc(left, dest.Y, 20, 20, 180, 90);
			path.AddArc(right - 20, dest.Y, 20, 20, 270, 90);
			path.AddLine(right, 20, right, bottom);
			path.AddLine(right, bottom, 0, bottom);
			path.AddLine(left, bottom, left, 20);
			path.CloseFigure();


			gfx.DrawPath(Pens.White, path);



		}
		
		public override void DrawTuxBarItemHeaderButton(Graphics gfx, Rectangle bounds,HeaderState state)
		{
			//gfx.FillRectangle( Brushes.Red,bounds);
			if(state == HeaderState.Collapsed)
			{
				gfx.DrawImage(FireIcons.ButtonDown34,bounds);
			}
			else
			{
				gfx.DrawImage(FireIcons.ButtonUp34,bounds);
			}
		}
		
		public override void DrawTuxBarItemMainBK(Graphics gfx, Rectangle dest)
		{
			//gfx.FillRectangle( Brushes.DarkGreen,dest);	
			dest.Width--;

			Color color = TuxBar.Graphic.Utils.GetLightColor(BaseColor, 40.5f);

			gfx.FillRectangle( new SolidBrush(color) ,dest);	

		}
		
		public override void DrawTuxBarItemMainBorder(Graphics gfx, Rectangle dest)
		{
			//dest.Height-=2;
			dest.Width--;
			gfx.DrawRectangle(Pens.Black,dest);

			dest.Inflate(-1,-1);

			gfx.DrawRectangle(Pens.White,dest);
		}

		public override void DrawTuxBarContainerBK(Graphics gfx, Rectangle dest)
		{
			//Color color = Utils.GetLightColor(BaseColor,60);

			//gfx.FillRectangle( new SolidBrush(color) ,dest);	
			gfx.FillRectangle( new SolidBrush(BaseColor) ,dest);	
		}
						
	} 
}