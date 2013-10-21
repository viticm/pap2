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
using System.ComponentModel;
using System.Drawing.Drawing2D;

namespace Fireball.Windows.Forms
{
    public class DocumentPresenter : ContainerControl
    {
        private Color _ActiveInnerBorderColor;
        private Color _InactiveInnerBorderColor;
        private Color _InnerColor;
        private Color _OuterColor;
        private int _BorderSize = 0;
        private bool _ShowInnerBorder;
        private bool _ShowOuterBorder;
        private bool _ShowShadow;

        public DocumentPresenter()
        {


            this.ShowInnerBorder = true;
            this.ShowOuterBorder = true;
            this.ShowShadow = true;


            this._ActiveInnerBorderColor = SystemColors.Highlight;
            this.BackColor = SystemColors.Window;
            this._InactiveInnerBorderColor = SystemColors.ControlText;
            this._InnerColor = Color.FromArgb(106, 112, 128);
            this._OuterColor = Color.FromArgb(138, 146, 166);
            this._BorderSize = 6;

            this.SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
            this.SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            this.SetStyle(ControlStyles.ResizeRedraw, true);
            this.SetStyle(ControlStyles.UserPaint, true);
            this.SetStyle(ControlStyles.ContainerControl, true);
            this.SetStyle(ControlStyles.Selectable, true);
        }


        [Category("Graphics"),
        DefaultValue(typeof(Color), "Highlight")]
        public Color ActiveInnerBorderColor
        {
            get
            {
                return this._ActiveInnerBorderColor;
            }
            set
            {
                this._ActiveInnerBorderColor = value;
                base.Invalidate();
            }
        }

        [DefaultValue(typeof(Color), "Window")]
        public override Color BackColor
        {
            get
            {
                return base.BackColor;
            }
            set
            {
                base.BackColor = value;
            }
        }

        [Browsable(false)]
        public override Image BackgroundImage
        {
            get
            {
                return base.BackgroundImage;
            }
            set
            {
                base.BackgroundImage = value;
            }
        }

        [Category("Appearance"), DefaultValue(6)]
        public int BorderSize
        {
            get
            {
                return this._BorderSize;
            }
            set
            {
                this._BorderSize = value;
                base.PerformLayout();
                base.Invalidate();
            }
        }

        protected override Size DefaultSize
        {
            get
            {
                return new Size(300, 300);
            }
        }

        [Browsable(false)]
        public override Color ForeColor
        {
            get
            {
                return base.ForeColor;
            }
            set
            {
                base.ForeColor = value;
            }
        }

        [DefaultValue(typeof(Color), "ControlText"), Category("Graphics")]
        public Color InactiveInnerBorderColor
        {
            get
            {
                return this._InactiveInnerBorderColor;
            }
            set
            {
                this._InactiveInnerBorderColor = value;
                base.Invalidate();
            }
        }

        [DefaultValue(typeof(Color), "106, 112, 128"),
        Category("Graphics")]
        public Color InnerColor
        {
            get
            {
                return this._InnerColor;
            }
            set
            {
                this._InnerColor = value;
                base.Invalidate();
            }
        }

        [DefaultValue(typeof(Color), "138, 146, 166"), Category("Appearance")]
        public Color OuterColor
        {
            get
            {
                return this._OuterColor;
            }
            set
            {
                this._OuterColor = value;
                base.Invalidate();
            }
        }


        [Category("Graphics"), DefaultValue(true)]
        public bool ShowInnerBorder
        {
            get
            {
                return this._ShowInnerBorder;
            }
            set
            {
                this._ShowInnerBorder = value;
                base.PerformLayout();
                base.Invalidate();
            }
        }


        [Category("Graphics"),DefaultValue(true)]
        public bool ShowOuterBorder
        {
            get
            {
                return this._ShowOuterBorder;
            }
            set
            {
                this._ShowOuterBorder = value;
                base.PerformLayout();
                base.Invalidate();
            }
        }

        [DefaultValue(true), Category("Graphics")]
        public bool ShowShadow
        {
            get
            {
                return this._ShowShadow;
            }
            set
            {
                this._ShowShadow = value;
                base.PerformLayout();
                base.Invalidate();
            }
        }

        private void _drawPath(Graphics gfx, Point[] points, int index)
        {
            using (GraphicsPath gp = new GraphicsPath())
            {
                gp.AddPolygon(points);
                using (PathGradientBrush pathBrush = new PathGradientBrush(gp))
                {
                    pathBrush.CenterPoint = (PointF)points[index];

                    pathBrush.CenterColor = this.InnerColor;

                    Color[] colors = new Color[] { this.OuterColor };

                    pathBrush.SurroundColors = colors;

                    gfx.FillPath(pathBrush, gp);
                }
            }
        }



        protected override void OnPaint(PaintEventArgs e)
        {
            Rectangle borderRectangle;

            Rectangle clientRectangle = base.ClientRectangle;
            int borderSize = 0;
            if (this.ShowOuterBorder)
            {
                borderSize += 2;
            }
            if (this.ShowShadow)
            {
                borderSize += (this.BorderSize * 2);
            }
            if (this.ShowInnerBorder)
            {
                borderSize += 2;
            }
            if ((clientRectangle.Width < borderSize) || (clientRectangle.Height < borderSize))
            {
                e.Graphics.Clear(this.OuterColor);
                return;
            }
            if (this.ShowOuterBorder)
            {
                borderRectangle = clientRectangle;
                borderRectangle.Width--;
                borderRectangle.Height--;
                using (Pen pen1 = new Pen(this.OuterColor))
                {
                    e.Graphics.DrawRectangle(pen1, borderRectangle);
                }
                clientRectangle.Inflate(-1, -1);
            }
            if (this.ShowShadow)
            {
                borderRectangle = new Rectangle(clientRectangle.Left + this.BorderSize, clientRectangle.Top, clientRectangle.Width - (this.BorderSize * 2), this.BorderSize);
                using (LinearGradientBrush brush1 = new LinearGradientBrush(borderRectangle, this.OuterColor, this.InnerColor, LinearGradientMode.Vertical))
                {
                    e.Graphics.FillRectangle(brush1, borderRectangle);
                }
                borderRectangle = new Rectangle(clientRectangle.Left, clientRectangle.Top + this.BorderSize, this.BorderSize, clientRectangle.Height - (this.BorderSize * 2));
                using (LinearGradientBrush brush2 = new LinearGradientBrush(borderRectangle, this.OuterColor, this.InnerColor, LinearGradientMode.Horizontal))
                {
                    e.Graphics.FillRectangle(brush2, borderRectangle);
                }
                borderRectangle = new Rectangle(clientRectangle.Left + this.BorderSize, clientRectangle.Bottom - this.BorderSize, clientRectangle.Width - (this.BorderSize * 2), this.BorderSize);
                using (LinearGradientBrush brush3 = new LinearGradientBrush(borderRectangle, this.InnerColor, this.OuterColor, LinearGradientMode.Vertical))
                {
                    e.Graphics.FillRectangle(brush3, borderRectangle);
                }
                borderRectangle = new Rectangle(clientRectangle.Right - this.BorderSize, clientRectangle.Top + this.BorderSize, this.BorderSize, clientRectangle.Height - (this.BorderSize * 2));
                using (LinearGradientBrush innerBrush = new LinearGradientBrush(borderRectangle, this.InnerColor, this.OuterColor, LinearGradientMode.Horizontal))
                {
                    e.Graphics.FillRectangle(innerBrush, borderRectangle);
                }
                Point[] points = new Point[] { new Point(clientRectangle.Left, clientRectangle.Top), new Point(clientRectangle.Left + this.BorderSize, clientRectangle.Top), new Point(clientRectangle.Left + this.BorderSize, clientRectangle.Top + this.BorderSize), new Point(clientRectangle.Left, clientRectangle.Top + this.BorderSize) };
                this._drawPath(e.Graphics, points, 2);
                points = new Point[] { new Point(clientRectangle.Right - this.BorderSize, clientRectangle.Top), new Point(clientRectangle.Right, clientRectangle.Top), new Point(clientRectangle.Right, clientRectangle.Top + this.BorderSize), new Point(clientRectangle.Right - this.BorderSize, clientRectangle.Top + this.BorderSize) };
                this._drawPath(e.Graphics, points, 3);
                points = new Point[] { new Point(clientRectangle.Right - this.BorderSize, clientRectangle.Bottom - this.BorderSize), new Point(clientRectangle.Right, clientRectangle.Bottom - this.BorderSize), new Point(clientRectangle.Right, clientRectangle.Bottom), new Point(clientRectangle.Right - this.BorderSize, clientRectangle.Bottom) };
                this._drawPath(e.Graphics, points, 0);
                points = new Point[] { new Point(clientRectangle.Left, clientRectangle.Bottom - this.BorderSize), new Point(clientRectangle.Left + this.BorderSize, clientRectangle.Bottom - this.BorderSize), new Point(clientRectangle.Left + this.BorderSize, clientRectangle.Bottom), new Point(clientRectangle.Left, clientRectangle.Bottom) };
                this._drawPath(e.Graphics, points, 1);
                clientRectangle.Inflate(-this.BorderSize, -this.BorderSize);
            }

            if (this.ShowInnerBorder)
            {
                borderRectangle = clientRectangle;
                if (base.ContainsFocus)
                {
                    using (Pen penna = new Pen(this.ActiveInnerBorderColor, 2f))
                    {
                        e.Graphics.DrawRectangle(penna, borderRectangle);
                    }
                }
                using (Pen penna = new Pen(this.InactiveInnerBorderColor))
                {
                    e.Graphics.DrawRectangle(penna, borderRectangle);
                }
            }
        }

        protected override void OnEnter(EventArgs e)
        {
            base.Invalidate();
            base.OnEnter(e);
        }

        protected override void OnLeave(EventArgs e)
        {
            base.Invalidate();
            base.OnLeave(e);
        }

        public override Rectangle DisplayRectangle
        {
            get
            {
                Rectangle dispRect = base.DisplayRectangle;
                if (this.ShowInnerBorder)
                {
                    dispRect.Inflate(-1, -1);
                }
                if (this.ShowOuterBorder)
                {
                    dispRect.Inflate(-1, -1);
                }
                if (this.ShowShadow)
                {
                    dispRect.Inflate(-this.BorderSize, -this.BorderSize);
                }
                return dispRect;
            }
        }
 

    }
}
