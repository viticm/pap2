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
using System.Windows.Forms;
using Fireball.Windows.Forms.TuxBar.Themes;
using Fireball.Windows.Forms.TuxBar;

namespace Fireball.Windows.Forms.TuxBar
{
	internal class TuxBarItemHeader : Panel
	{

		private TuxTheme m_TuxTheme = null;

		public event EventHandler Toogle;

		private TuxBarItemHeaderToogleButton m_ToogleButton = null;

		private Image m_Icon = null;
	
		public TuxBarItemHeader()
		{
			m_TuxTheme = new FireTheme();

			this.SetStyle(ControlStyles.UserPaint 
				| ControlStyles.ResizeRedraw 
				| ControlStyles.AllPaintingInWmPaint
				| ControlStyles.OptimizedDoubleBuffer
				| ControlStyles.SupportsTransparentBackColor
				, true
				);
			this.BackColor = Color.Transparent;

			m_ToogleButton = new TuxBarItemHeaderToogleButton();

			this.Controls.Add(m_ToogleButton);

			m_ToogleButton.Size = new Size(24,24);

			m_ToogleButton.Toogle+=new EventHandler(m_ToogleButton_Toogle);

			this.Height = 24;

			//this.Text = "Provaaa";
		}



		public Point IconLocation
		{
			get
			{
				return new Point(4,-10);
			}
		}
		public Image Icon 
		{
			get
			{
				return m_Icon;
			}
			set
			{
				/*if(value.Width != 32 || value.Height != 16)
					throw new Exception("Invalid Icon Size");*/

				m_Icon = value;
			}
		}

		public bool Expanded
		{
			get
			{
				return m_ToogleButton.Expanded;
			}
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			Rectangle rect = this.ClientRectangle;
			rect.Width--;
			rect.Height--;

			m_TuxTheme.DrawTuxBarItemHeaderBK(e.Graphics, rect, m_ToogleButton.Expanded ? HeaderState.Expanded : HeaderState.Collapsed);

			int padd = 15;

			if(Icon != null)
				padd = IconLocation.X+ Icon.Size.Width + 3;

            rect = new Rectangle(padd, 3, m_ToogleButton.Left - 5 - padd, this.Height - 3);

			if(Icon != null)
				e.Graphics.DrawImageUnscaled(Icon,new Rectangle(IconLocation,Icon.Size));

			m_TuxTheme.DrawTuxBarItemHeaderText(e.Graphics,rect,this.Text );
		}

		protected override void OnResize(EventArgs e)
		{

			base.OnResize(e);
			Rectangle rect = this.ClientRectangle;
			//rect.Width--;
			rect.Height--;

			m_ToogleButton.Location = new Point(rect.Right - 26,(rect.Bottom /2)-11);
			this.Invalidate();
		}
		
				
		public TuxTheme  Theme
		{
			get
			{
				return m_TuxTheme;
			}
			set
			{
				m_TuxTheme = value;

                m_ToogleButton.Theme = value;
			}
		}

		private void m_ToogleButton_Toogle(object sender, EventArgs e)
		{
			if(Toogle != null)
				Toogle(this, new EventArgs());
		}
	}

	//***************************************************//
	internal class TuxBarItemHeaderToogleButton : Control
	{
        private TuxTheme m_TuxTheme = null;

        public TuxTheme Theme
        {
            get { return m_TuxTheme; }
            set { m_TuxTheme = value; }
        }

		private int m_Status = 1;
		public event EventHandler Toogle;

		public TuxBarItemHeaderToogleButton()
		{
			this.SetStyle(ControlStyles.UserPaint 
				| ControlStyles.ResizeRedraw 
				| ControlStyles.AllPaintingInWmPaint
				| ControlStyles.OptimizedDoubleBuffer
				| ControlStyles.SupportsTransparentBackColor
				, true
				);
			this.BackColor = Color.Transparent;

			m_TuxTheme = new FireTheme();
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			Rectangle rect = this.ClientRectangle;
			rect.Width--;
			rect.Height--;

			m_TuxTheme.DrawTuxBarItemHeaderButton(e.Graphics, rect, Expanded ? HeaderState.Expanded : HeaderState.Collapsed);
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
			m_Status = (m_Status == 0 ? 1 : 0);
			if(Toogle != null)
				Toogle(this,new EventArgs());
			this.Invalidate();
		}


		protected override void OnMouseMove(MouseEventArgs e)
		{
			if(this.Cursor == Cursors.Default)
			{
				this.Cursor = Cursors.Hand;
			}
		}


		protected override void OnMouseLeave(EventArgs e)
		{
			this.Cursor = Cursors.Default;
		}

		public bool Expanded
		{
			get
			{
				return (m_Status == 0 ? false : true);
			}
		}
	}
	//***************************************************//
	
}
