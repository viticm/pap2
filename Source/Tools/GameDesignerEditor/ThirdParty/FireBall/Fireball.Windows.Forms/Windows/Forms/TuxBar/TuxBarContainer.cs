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

namespace Fireball.Windows.Forms.TuxBar
{
	public class TuxBarContainer : Panel
	{
		private TuxBarContainerPanel m_Container = null;
        private TuxTheme m_Theme = null;
        private bool m_ShowBorder = true;
        private Color m_BorderColor = SystemColors.ControlDark;

		public Color BorderColor
		{
			get
			{
				return m_BorderColor;
			}
			set
			{
				m_BorderColor = value;
			}
		}

        public bool ShowBorder
        {
            get
            {
                return m_ShowBorder;
            }
            set
            {
                m_ShowBorder = value;

                if (m_ShowBorder)
                    this.Padding = new Padding(2);
                else
                    this.Padding = new Padding(0);
            }
        }

        public TuxTheme Theme
        {
            get
            {
                return m_Theme;
            }
            set
            {
                m_Theme = value;

                m_Container.Theme = value;
            }
        }

		public TuxBarContainer()
		{
			m_Container = new TuxBarContainerPanel();
			
			this.Controls.Add(m_Container);

			this.AutoScroll = true;

			this.AutoScrollMinSize = new Size(10, 10);

			m_Theme = new FireTheme();			
		}



		public TuxBarItemCollection Items
		{
			get
			{
				return m_Container.Items;
			}
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint(e);

			Rectangle rect = this.ClientRectangle;
			rect.Width--;
			rect.Height--;

            m_Theme.DrawTuxBarContainerBK(e.Graphics, rect);

            if (m_ShowBorder)
            {
                e.Graphics.DrawRectangle(new Pen(m_BorderColor), rect);
            }
		}
		protected override void OnResize(EventArgs eventargs)
		{
			base.OnResize(eventargs);

            if (this.IsHandleCreated)
            {

                if (this.Parent.Width < m_Container.MinimunSize)
                {
                    m_Container.Width = m_Container.MinimunSize;
                }
                else
                {
                    m_Container.Width = this.Width;
                }

                if (this.VerticalScroll.Visible)
                {
                    m_Container.Width -= SystemInformation.VerticalScrollBarWidth;
                }

                m_Container.ResetWidth();
            }
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);

            this.OnResize(e);
        }

		internal class TuxBarContainerPanel : Panel
		{
			private TuxTheme m_TuxTheme = null;
			private TuxBarItemCollection m_Items = null;

			private const int ITEM_TOP_SPACING = 15;

			private const int ITEM_LATERAL_SPACING = 20;

			private const int ITEM_MINIMUN_SIZE = 120;


			public TuxBarContainerPanel()
			{
				m_TuxTheme = new FireTheme();

				this.SetStyle(ControlStyles.UserPaint 
					| ControlStyles.ResizeRedraw 
					| ControlStyles.AllPaintingInWmPaint
					| ControlStyles.OptimizedDoubleBuffer
					| ControlStyles.SupportsTransparentBackColor
					| ControlStyles.Selectable
					| ControlStyles.StandardClick
					, true
					);

				this.BackColor = Color.Transparent;

				m_Items = new TuxBarItemCollection();

				m_Items.AddItem+=new TuxBar.TuxBarItemCollection.TuxBarItemCollectionHandler(m_Items_AddItem);

				m_Items.RemoveItem+=new TuxBar.TuxBarItemCollection.TuxBarItemCollectionHandler(m_Items_RemoveItem);

				m_Items.InsertItem+=new TuxBar.TuxBarItemCollection.TuxBarItemCollectionInsertHandler(m_Items_InsertItem);

				m_Items.LayoutChanged+=new EventHandler(m_Items_LayoutChanged);

                m_Items.ClearItems += new TuxBarItemCollection.TuxBarItemCollectionClear(m_Items_ClearItems);
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

                    foreach (TuxBarItem current in m_Items)
                    {
                        current.Theme = value;
                    }
                }
            }

            void m_Items_ClearItems(TuxBarItemCollection sender)
            {
                foreach (TuxBarItem current in sender)
                {
                    this.Controls.Remove(current);
                }
				this.Invalidate();
            }

			protected override void OnMouseDown(MouseEventArgs e)
			{
				base.OnMouseDown(e);

				this.Select();
			}

			public TuxBarItemCollection Items
			{
				get
				{
					return m_Items;
				}
			}

			private void m_Items_AddItem(TuxBarItem item)
			{
                item.Theme = m_TuxTheme;
                item.Resize += new EventHandler(item_Resize);
				item.Toogle += new EventHandler(item_Toogle);
				this.Controls.Add(item);
				AlignControls();
                this.Refresh();
			}

			void item_Toogle(object sender, EventArgs e)
			{
				AlignControls();
			}

            void item_Resize(object sender, EventArgs e)
            {
                AlignControls();
            }

			private void m_Items_RemoveItem(TuxBarItem item)
			{
				this.Controls.Remove(item);
				AlignControls();
                this.Refresh();
			}

			private void m_Items_InsertItem(TuxBarItem item, int index)
			{
                item.Theme = m_TuxTheme;
                item.Resize += new EventHandler(item_Resize);
				item.Toogle += new EventHandler(item_Toogle);
				this.Controls.RemoveAt(index);
				AlignControls();
			}

			public int MinimunSize
			{
				get
				{
					return ITEM_MINIMUN_SIZE;
				}
			}

			internal void ForceAlign()
			{
				AlignControls();
			}

			protected override void OnResize(EventArgs eventargs)
			{
				base.OnResize(eventargs);
				AlignControls();
			}

			private void AlignControls()
			{
				this.SuspendLayout();
			

				for(int i = 0; i  <  m_Items.Count;i++)
				{
					TuxBarItem current = m_Items[i];

					if(i == 0)
					{
						current.Top = ITEM_TOP_SPACING;
					}
					else
					{
						TuxBarItem prev = m_Items[i-1];

						current.Top = ITEM_TOP_SPACING + prev.Top + prev.Height;
					}

					current.Width = this.ClientSize.Width - (ITEM_LATERAL_SPACING * 2);

					/*if (((Panel)this.Parent).VerticalScroll.Visible)
					{
						current.Width -= SystemInformation.VerticalScrollBarWidth - ITEM_LATERAL_SPACING;
					}*/

					current.Left = ITEM_LATERAL_SPACING;
				}
				if(m_Items.Count >0)
				{
					TuxBarItem last = m_Items[m_Items.Count-1];

					this.Height = last.Top + last.Height + (ITEM_TOP_SPACING*2);
				}

				this.ResumeLayout();
			}

			internal void ResetWidth()
			{
				this.SuspendLayout();

				for (int i = 0; i < m_Items.Count; i++)
				{
					TuxBarItem current = m_Items[i];

					current.Width = this.ClientSize.Width - (ITEM_LATERAL_SPACING * 2);
				}

				this.ResumeLayout();
			}

			private void m_Items_LayoutChanged(object sender, EventArgs e)
			{
				AlignControls();
			}

		}
	}
}