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

using Fireball.Drawing.Shapes;

namespace Fireball.Windows.Forms
{
    public enum ButtonBorder
    {
        None = 0,
        Rectangle = 1,
        RoundRectangle = 2,
        Ellipse = 3,
    }

    public class CustomButtonStyleDefault : CustomButtonStyle
    {
        private int _width;
        private int _height;

        public override ButtonBorder Border
        {
            get
            {
                return ButtonBorder.None;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }
        public override int CornerRadius
        {
            get
            {
                return 0;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }
        public override ShapeRectangle Shape
        {
            get
            {
                return null;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }

        public override Brush FillBrushNormal
        {
            get
            {
                return null;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }
        public override Pen BorderPenNormal
        {
            get
            {
                return null;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }
        public override Brush FillBrushHot
        {
            get
            {
                return null;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }
        public override Pen BorderPenHot
        {
            get
            {
                return null;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }
        public override Brush FillBrushPressed
        {
            get
            {
                return null;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }
        public override Pen BorderPenPressed
        {
            get
            {
                return null;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }
        public override Brush FillBrushDisabled
        {
            get
            {
                return null;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }
        public override Pen BorderPenDisabled
        {
            get
            {
                return null;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }

        public CustomButtonStyleDefault(int width, int height)
            : base()
        {
            _width = width;
            _height = height;
        }

        public override void Resize(int width, int height)
        {
            _width = width;
            _height = height;
        }
        public override void Paint(Graphics graphics, bool focused,CustomButton.ButtonState state)
        {
            Rectangle r = new Rectangle(0, 0, _width, _height);
            switch (state)
            {
                case CustomButton.ButtonState.Normal:
                    if (focused)
                    {
                        ButtonRenderer.DrawButton(graphics, r, false, System.Windows.Forms.VisualStyles.PushButtonState.Default);
                    }
                    else
                    {
                        ButtonRenderer.DrawButton(graphics, r, false, System.Windows.Forms.VisualStyles.PushButtonState.Normal);
                    }
                    break;
                case CustomButton.ButtonState.Hot:
                    ButtonRenderer.DrawButton(graphics, r, focused, System.Windows.Forms.VisualStyles.PushButtonState.Hot);
                    break;
                case CustomButton.ButtonState.Pressed:
                    ButtonRenderer.DrawButton(graphics, r, false, System.Windows.Forms.VisualStyles.PushButtonState.Pressed);
                    break;
                case CustomButton.ButtonState.Disabled:
                    ButtonRenderer.DrawButton(graphics, r, false, System.Windows.Forms.VisualStyles.PushButtonState.Disabled);
                    break;
            }
        }

    }

    public class CustomButtonStyle
    {
        public static CustomButtonStyleDefault CreateDefaultStyle(int width,int height)
        {
            return new CustomButtonStyleDefault(width, height);
        }

        private Brush _fillBrushNormal;
        private Pen _borderPenNormal;
        private Brush _fillBrushHot;
        private Pen _borderPenHot;
        private Brush _fillBrushPressed;
        private Pen _borderPenPressed;
        private Brush _fillBrushDisabled;
        private Pen _borderPenDisabled;

        private ButtonBorder _border;
        private int _cornerRadius;
        private int _width;
        private int _height;
        private ShapeRectangle _shape;

        protected CustomButtonStyle()
        {
        }

        public CustomButtonStyle(int width, int height)
            : this(width, height, ButtonBorder.RoundRectangle)
        {
        }
        public CustomButtonStyle(int width, int height, ButtonBorder border)
        {
            _width = width;
            _height = height;
            _fillBrushNormal = new LinearGradientBrush(new Rectangle(0, 0, _width, _height), SystemColors.ControlLightLight, SystemColors.Control, LinearGradientMode.Vertical);
            _borderPenNormal = new Pen( SystemColors.ControlDark,1.2f);
            _fillBrushHot = new LinearGradientBrush(new Rectangle(0, 0, _width, _height), SystemColors.ControlLightLight, SystemColors.Control, LinearGradientMode.Vertical);
            _borderPenHot = new Pen(Color.FromArgb(150,SystemColors.Highlight),2f);
            _fillBrushPressed = new LinearGradientBrush(new Rectangle(0, 0, _width, _height), SystemColors.Control, SystemColors.ControlLightLight, LinearGradientMode.Vertical);
            _borderPenPressed = new Pen(SystemColors.Highlight, 1.2f);
            _fillBrushDisabled = new LinearGradientBrush(new Rectangle(0, 0, _width, _height), SystemColors.Control, SystemColors.Control, LinearGradientMode.Vertical);
            _borderPenDisabled = SystemPens.ControlDark;
            _border = border;
            _cornerRadius = 3;

            _shape = ConvertShape(_border);
        }

        public CustomButtonStyle Clone()
        {
            CustomButtonStyle style = new CustomButtonStyle(this._width, this._height, _border);

            style.FillBrushNormal = _fillBrushNormal;
            style.BorderPenNormal = _borderPenNormal;
            style.FillBrushHot = _fillBrushHot;
            style.BorderPenHot = _borderPenHot;
            style.FillBrushPressed = _fillBrushPressed;
            style.BorderPenPressed = _borderPenPressed;

            style.CornerRadius = _cornerRadius;
            style.Shape = ConvertShape(_border);

            return style;
        }
        public virtual void Resize(int width, int height)
        {
            _width = width;
            _height = height;

            _shape.Width = _width - 1;
            _shape.Height = _height - 1;
            _shape.Update();
        }
        public virtual void Paint(Graphics graphics, bool focused, CustomButton.ButtonState state)
        {
            switch (state)
            {
                case CustomButton.ButtonState.Disabled:
                    _shape.Brush = _fillBrushDisabled;
                    _shape.Pen = _borderPenDisabled;
                    break;
                case CustomButton.ButtonState.Normal:
                    _shape.Brush = _fillBrushNormal;
                    _shape.Pen = _borderPenNormal;
                    break;
                case CustomButton.ButtonState.Hot:
                    _shape.Brush = _fillBrushHot;
                    _shape.Pen = _borderPenHot;
                    break;
                case CustomButton.ButtonState.Pressed:
                    _shape.Brush = _fillBrushPressed;
                    _shape.Pen = _borderPenPressed;
                    break;
            }

            _shape.Fill(graphics);

            Rectangle light = new Rectangle(0, 0, this.Width, this.Height);
            graphics.FillEllipse(
                new LinearGradientBrush(
                    light,
                    Color.FromArgb(80, Color.White),
                    Color.FromArgb(0, Color.White),
                    LinearGradientMode.Vertical
                ),
                light
            );

            _shape.Draw(graphics, SmoothingMode.AntiAlias);

            if (focused)
            {
                Rectangle r = new Rectangle(0, 0, _width, _height);
                r.Inflate(-2, -2);
                ControlPaint.DrawFocusRectangle(graphics, r);
            }
        }

        public virtual Pen BorderPenNormal
        {
            get
            {
                return _borderPenNormal;
            }
            set
            {
                _borderPenNormal = value;
            }
        }
        public virtual Brush FillBrushNormal
        {
            get
            {
                return _fillBrushNormal;
            }
            set
            {
                _fillBrushNormal = value;
            }
        }
        public virtual Pen BorderPenHot
        {
            get
            {
                return _borderPenHot;
            }
            set
            {
                _borderPenHot = value;
            }
        }
        public virtual Brush FillBrushHot
        {
            get
            {
                return _fillBrushHot;
            }
            set
            {
                _fillBrushHot = value;
            }
        }
        public virtual Pen BorderPenPressed
        {
            get
            {
                return _borderPenPressed;
            }
            set
            {
                _borderPenPressed = value;
            }
        }
        public virtual Brush FillBrushPressed
        {
            get
            {
                return _fillBrushPressed;
            }
            set
            {
                _fillBrushPressed = value;
            }
        }
        public virtual Pen BorderPenDisabled
        {
            get
            {
                return _borderPenDisabled;
            }
            set
            {
                _borderPenDisabled = value;
            }
        }
        public virtual Brush FillBrushDisabled
        {
            get
            {
                return _fillBrushDisabled;
            }
            set
            {
                _fillBrushDisabled = value;
            }
        }

        public virtual ShapeRectangle Shape
        {
            get
            {
                return _shape;
            }
            set
            {
                _shape = value;
                _shape.Width = _width - 1;
                _shape.Height = _height - 1;
                _shape.Pen = _borderPenNormal;
                _shape.Brush = _fillBrushNormal;

                _shape.Update();

                _border = ButtonBorder.None;
            }
        }
        public virtual ButtonBorder Border
        {
            get
            {
                return _border;
            }
            set
            {
                if (Enum.IsDefined(typeof(ButtonBorder), value))
                {
                    if (_border != value)
                    {
                        _border = value;
                        _shape = ConvertShape(_border);
                    }
                }
            }
        }
        public virtual int CornerRadius
        {
            get
            {
                return _cornerRadius;
            }
            set
            {
                if (value >= 0)
                {
                    _cornerRadius = value;
                    if (_shape is ShapeRoundRectangle)
                    {
                        ((ShapeRoundRectangle)_shape).Radius = _cornerRadius;
                        _shape.Update();
                    }
                }
            }
        }

        public int Width
        {
            get
            {
                return _width;
            }
        }
        public int Height
        {
            get
            {
                return _height;
            }
        }

        private ShapeRectangle ConvertShape(ButtonBorder border)
        {
            ShapeRectangle shape = null;
            switch (border)
            {
                case ButtonBorder.Rectangle:
                    shape = new ShapeRectangle(0, 0, _width - 1, _height - 1);
                    break;
                case ButtonBorder.RoundRectangle:
                    shape = new ShapeRoundRectangle(0, 0, _width - 1, _height - 1, _cornerRadius);
                    break;
                case ButtonBorder.Ellipse:
                    shape = new ShapeEllipse(0, 0, _width - 1, _height - 1);
                    break;
            }
            if (shape != null)
            {
                shape.Brush = _fillBrushNormal;
                shape.Pen = _borderPenNormal;
            }
            return shape;
        }
    }
}
