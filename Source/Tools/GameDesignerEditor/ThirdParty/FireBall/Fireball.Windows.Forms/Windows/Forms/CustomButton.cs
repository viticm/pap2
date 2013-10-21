//    Copyright (C) 2006  Riccardo Marzi riccardo@dotnetfireball.org
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
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace Fireball.Windows.Forms
{
    public class CustomButton: Control
    {
        
        public enum ButtonState
        {
            Normal=0,
            Hot=1,
            Pressed=2,
            Disabled=3
        }

        private bool _onMove;
        private bool _onPress;
        private Point _lastPoint;
        private CustomButtonStyle _style;

        public CustomButton()
            :this(80,32)
        {
        }
        public CustomButton(int width,int height)
        {
            this.Width = width;
            this.Height = height;

            base.SetStyle(ControlStyles.StandardDoubleClick | ControlStyles.StandardClick, false);
            this.SetStyle(
                ControlStyles.AllPaintingInWmPaint |
                ControlStyles.OptimizedDoubleBuffer |
                ControlStyles.ResizeRedraw |
                ControlStyles.UserPaint |
                ControlStyles.SupportsTransparentBackColor,
                true
            );

            this.BackColor = Color.Transparent;

            _onMove = false;
            _onPress = false;
            _lastPoint = new Point(-1, -1);

            _style = CustomButtonStyle.CreateDefaultStyle(width, height);
            
        }

        public CustomButtonStyle Style
        {
            get
            {
                return _style;
            }
            set
            {
                _style = value;
                this.Invalidate();
            }
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            base.OnKeyDown(e);

            if (this.Focused && e.KeyCode == Keys.Enter)
            {
                _onPress = true;

                this.Invalidate();

                base.OnClick(new EventArgs());
            }
        }
        protected override void OnKeyUp(KeyEventArgs e)
        {
            if (this.Focused && e.KeyCode == Keys.Enter)
            {
                _onPress = false;

                this.Invalidate();
            }

            base.OnKeyUp(e);
        }
        protected override void OnGotFocus(EventArgs e)
        {
            this.Invalidate();
            base.OnGotFocus(e);
        }
        protected override void OnLostFocus(EventArgs e)
        {
            this.Invalidate();
            base.OnLostFocus(e);
        }
        protected override void OnSizeChanged(EventArgs e)
        {
            if (_style != null)
            {
                _style.Resize(this.Width, this.Height);
            }

            this.Invalidate();

            base.OnSizeChanged(e);
        }
        protected override void OnMouseMove(MouseEventArgs e)
        {
            _onMove = true;

            if (_lastPoint != e.Location)
            {
                _lastPoint = e.Location;
                this.Invalidate();
            }

            base.OnMouseMove(e);
        }
        protected override void OnMouseDown(MouseEventArgs e)
        {
            this.Focus();

            _onPress = true;

            this.Invalidate();

            base.OnMouseDown(e);
        }
        protected override void OnMouseUp(MouseEventArgs e)
        {
            _onPress = false;

            this.Invalidate();

            base.OnMouseUp(e);
        }
        protected override void OnMouseLeave(EventArgs e)
        {
            _onMove = false;

            this.Invalidate();

            base.OnMouseLeave(e);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            PointF center = new Point(this.Width / 2, this.Height / 2);

            ButtonState state = ButtonState.Normal;

            if (_onPress)
            {
                state = ButtonState.Pressed;
            }
            else if (_onMove)
            {
                state = ButtonState.Hot;
            }
            else if (!this.Enabled)
            {
                state = ButtonState.Disabled;
            }

            _style.Paint(e.Graphics,this.Focused, state);

            if (!string.IsNullOrEmpty(this.Text))
            {
                Brush textBrush = SystemBrushes.GrayText;
                if (this.Enabled)
                {
                    textBrush = new SolidBrush(this.ForeColor);
                }

                StringFormat sf = new StringFormat();
                sf.Alignment = StringAlignment.Center;
                sf.LineAlignment = StringAlignment.Center;

                e.Graphics.DrawString(this.Text, this.Font, textBrush, center, sf);
            }

            //base.OnPaint(e);
        }
    }
}
