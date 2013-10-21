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
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.ComponentModel.Design;

namespace Fireball.Windows.Forms.TuxBar
{
    //,

    [Designer("System.Windows.Forms.Design.UserControlDocumentDesigner, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a", 
        typeof(IRootDesigner)),DesignerCategory("UserControl"),  ClassInterface(ClassInterfaceType.AutoDispatch), 
    Designer("System.Windows.Forms.Design.ControlDesigner, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a")]
	public class TuxBarItem : UserControl
	{
        private TuxTheme m_TuxTheme = null;

		private TuxBarItemHeader m_Header = null;
		private TuxBarItemContainer m_Container = null;
		private int m_storedHeight = 0;
		public event EventHandler Toogle;
		private Image m_RealIcon = null;
		private Image m_PartIcon = null;
        private bool _EnableResize = false;
        private int _MinHeight = 150;

		public TuxBarItem()
		{
			m_Header = new TuxBarItemHeader();
								
			m_TuxTheme = new FireTheme();

			this.SetStyle(ControlStyles.UserPaint 
				| ControlStyles.ResizeRedraw 
				| ControlStyles.AllPaintingInWmPaint
				| ControlStyles.OptimizedDoubleBuffer
				| ControlStyles.SupportsTransparentBackColor
				, true
				);
			this.BackColor = Color.Transparent;


			m_Header.Dock = DockStyle.Top;

			m_Container = new TuxBarItemContainer();			


			m_Container.Padding = new Padding(2);

			m_Header.Toogle+=new EventHandler(m_Header_Toogle);

			this.Height = 150;
            this.Width = 100;

			this.Controls.Add(m_Container);
			this.Controls.Add(m_Header);
		}

		protected override void OnResize(EventArgs e)
		{
			base.OnResize(e);

			m_Container.Left = m_Container.Padding.Left;
			m_Container.Width = this.ClientSize.Width - m_Container.Left - m_Container.Padding.Right * 2;
			m_Container.Top = m_Header.Height + m_Container.Padding.Top;
			m_Container.Height = this.Height - m_Header.Height - m_Container.Padding.Bottom * 2;

            //if (this.Height <= this.MinHeight)
            //{
            //    this.Height = this.MinHeight;
            //}
		}


		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint (e);

			Rectangle rect = this.ClientRectangle;

			rect.Height--;
			rect.Y = m_Header.Height;
			rect.Height -= m_Header.Height;

			m_TuxTheme.DrawTuxBarItemMainBK(e.Graphics, rect);
			m_TuxTheme.DrawTuxBarItemMainBorder(e.Graphics, rect);

			using(Graphics g = this.Parent.CreateGraphics())
			{
				if(Icon != null)
				{
					//TODO: g.DrawImageUnscaled(Icon,this.Left ,10,24,12);
					g.DrawImageUnscaled(Icon,this.Left+ m_Header.IconLocation.X ,this.Top - 10,24,12);
				}
			}
		}

        private bool resize_bottom = false;
        private Point resize_bottom_old_pos;

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);

            if (this.ResizeBottomArea.Contains(e.Location))
            {
                resize_bottom = true;
                resize_bottom_old_pos = e.Location;
                this.Cursor = Cursors.SizeNS;
            }
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);

            if (resize_bottom && this.EnableResize)
            {
                int y = resize_bottom_old_pos.Y - e.Y;
                int x = resize_bottom_old_pos.X - e.X;

                this.Height -= y;

                resize_bottom_old_pos = e.Location;// new Point(e.X - x, e.Y - y);
            }

            if (this.EnableResize && this.ResizeBottomArea.Contains(e.Location))
            {
                this.Cursor = Cursors.SizeNS;
            }
            else
            {
                this.Cursor = Cursors.Default;
            }
        }

        public int MinHeight
        {
            get { return _MinHeight; }
            set { _MinHeight = value; }
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);

            resize_bottom = false;

            this.Cursor = Cursors.Default;
        }

        private Rectangle ResizeBottomArea
        {
            get
            {
                Rectangle rect = new Rectangle(0,
                    this.ClientSize.Height - 9,
                    this.ClientSize.Width, 9);

                return rect;
            }
        }

        public bool EnableResize
        {
            get { return _EnableResize; }
            set { _EnableResize = value; }
        }
		

		public ControlCollection ChildControls
		{
			get
			{
				return m_Container.Controls;
			}
		}

		public Image Icon
		{
			get
			{
				return m_RealIcon;
			}
			set
			{
				if(value == null)
				{
					m_PartIcon = null;
					m_RealIcon = null;
					return;
				}
				if(value.Width != 32 || value.Height != 32)
					throw new Exception("Invalid Icon Size! need icon size of 32x32");

				m_RealIcon = new Bitmap(value, 32, 32);

				m_Header.Icon = new Bitmap(value, 32, 32);
			}
		}

		public new Size Size
		{
			get
			{
				return base.Size;
			}
			set
			{
				base.Size = value;

				m_storedHeight = value.Height;
			}
		}

        public TuxTheme Theme
		{
			get
			{
				return m_TuxTheme;
			}
			set
			{
				m_TuxTheme = value;

                m_Header.Theme = value;

                m_Container.Theme = value;
			}
		}

		public new string Text
		{
			get
			{
				return m_Header.Text;
			}
			set
			{
				m_Header.Text = value;
			}
		}

		private void m_Header_Toogle(object sender, EventArgs e)
		{
			if(m_Header.Expanded == false)
			{
				m_storedHeight = this.Height;
				this.Height  = m_Header.Height;
			}
			else
			{
				this.Height = m_storedHeight;
			}
			if(Toogle != null)
				Toogle(sender,new EventArgs());

			m_Header.Invalidate();
		}	

	}
	
	

	
}