//    Copyright (C) 2005  Sebastian Faltoni
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
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace Fireball.Windows.Forms
{
    public enum DiscoverHeadPosition
    {
        Head = 0,
        SubHead = 1
    }

    public class DiscoverGroupControl : ContainerControl
    {
        private IDiscoverRenderer _DiscoverRenderer = null;

        private DiscoverHeadPosition _HeadPosition = DiscoverHeadPosition.Head;

        private Font _HeaderFont = new Font("Tahoma", 12f, FontStyle.Bold, GraphicsUnit.Point);

        private Image _Image = null;

        private DiscoverPane _FriendPane = null;

        public DiscoverPane FriendPane
        {
            get
            {
                return _FriendPane;
            }
            set
            {
                if (_FriendPane != null)
                    _FriendPane.VisibleChanged -= new EventHandler(_FriendPane_VisibleChanged);

                _FriendPane = value;

                if (_FriendPane != null)
                {
                    _FriendPane.VisibleChanged += new EventHandler(_FriendPane_VisibleChanged);

                    _FriendPane_VisibleChanged(null, new EventArgs());
                }
            }
        }

        void _FriendPane_VisibleChanged(object sender, EventArgs e)
        {
            base.Visible = _FriendPane.Visible;
        }


        //protected override void SetVisibleCore(bool value)
        //{
        //    if (this.FriendPane != null)
        //    {
        //        value = (value && (this.FriendPane.Parent is DiscoverControl)) &&
        //            (((DiscoverControl)this.FriendPane.Parent).SelectedPane == this.FriendPane);
        //    }
        //    base.SetVisibleCore(value);

        //}

        public Image Image
        {
            get { return _Image; }
            set { _Image = value; }
        }

        public Font HeaderFont
        {
            get { return _HeaderFont; }
            set { _HeaderFont = value; }
        }

        public DiscoverHeadPosition HeadPosition
        {
            get
            {
                return _HeadPosition;
            }
            set
            {
                _HeadPosition = value;
                this.PerformLayout();
                this.Invalidate();
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            bool rtl = this.RightToLeft == RightToLeft.Yes;

            if (_HeadPosition == DiscoverHeadPosition.Head)
            {
                _DiscoverRenderer.DrawBackground(e.Graphics, base.ClientRectangle, this.BackColor);
                _DiscoverRenderer.DrawHeader(e.Graphics, this.HeaderRect,
                    this.Text, this._HeaderFont, this._Image, rtl);
            }
            else
            {
                e.Graphics.Clear(this.BackColor);
                _DiscoverRenderer.DrawSeparator(e.Graphics, this.HeaderRect, this.Text,
                    this.Font, this.ForeColor, rtl);
            }
        }

        public override Rectangle DisplayRectangle
        {
            get
            {
                Rectangle displayRect = base.DisplayRectangle;
                //if (this._HeadPosition == HeadPosition.Head)
                //{
                //    displayRect.Inflate(-1, -1);
                //}
                displayRect.Y = (this.HeaderRect.Bottom);
                displayRect.Height -= (this.HeaderRect.Bottom);
                return displayRect;

            }
        }

        private Rectangle HeaderRect
        {
            get
            {
                Rectangle rect;

                int h = this.HeaderFont.Height + 6;

                rect = new Rectangle(0, 0, this.ClientSize.Width - 1, h);

                if (_HeadPosition == DiscoverHeadPosition.SubHead)
                {
                    rect.Inflate(1, 1);

                    rect.Height -= 5;
                }

                return rect;
            }
        }

        public DiscoverGroupControl()
        {
            _DiscoverRenderer = new DiscoverProfessionalRender();
        }
    }
}
