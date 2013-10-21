using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

using Fireball.Drawing.Shapes;

namespace Fireball.Windows.Forms
{
    public class GroupPanel:Control
    {
        private ShapeRoundRectangle _shape;
        private Color _fillColor;
        private Color _borderColor;

        public GroupPanel()
        {
            _shape = new ShapeRoundRectangle(new Rectangle(0,0, 100,100), 6);

            _fillColor = Color.Transparent;
            _borderColor = SystemColors.ControlDark;

            _shape.Pen = new Pen(_borderColor);
            _shape.Brush = new SolidBrush(_fillColor);
        }

        public Color FillColor
        {
            get
            {
                return _fillColor;
            }
            set
            {
                _fillColor = value;
                _shape.Brush = new SolidBrush(_fillColor);
                this.Invalidate();
            }
        }

        public Color BorderColor
        {
            get
            {
                return _borderColor;
            }
            set
            {
                _borderColor = value;
                _shape.Pen = new Pen(_borderColor);
                this.Invalidate();
            }
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            _shape.Width = this.ClientRectangle.Width-1;
            _shape.Height = this.ClientRectangle.Height-1;

            base.OnSizeChanged(e);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            _shape.Draw(e.Graphics,System.Drawing.Drawing2D.SmoothingMode.AntiAlias);

            base.OnPaint(e);
        }
    }
}
