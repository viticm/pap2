using System;
using System.Collections.Generic;
using System.Text;
using Fireball.Windows.Forms;
using System.Drawing.Drawing2D;
using System.Drawing;

namespace FireEdit.Themes
{
    class FireEditLineMarginRender :  LineMarginRender
    {
        private LinearGradientBrush _brush = null;

        public override System.Drawing.Rectangle Bounds
        {
            get
            {
                return base.Bounds;
            }
            set
            {
                if (!this.Bounds.Equals(value))
                {
                    _brush = new LinearGradientBrush(value,
                        SystemColors.Control, SystemColors.ControlLightLight,
                        LinearGradientMode.Horizontal);
                }
                base.Bounds = value;
            }
        }

        public override void Render(Graphics gfx)
        {
            gfx.FillRectangle(_brush, this.Bounds);
        }
    }
}
