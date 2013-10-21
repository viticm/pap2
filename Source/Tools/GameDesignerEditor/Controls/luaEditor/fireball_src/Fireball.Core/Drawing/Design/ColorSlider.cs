using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using Fireball.Drawing.Drawing2D;

namespace Fireball.Drawing.Design
{
    public class ColorSlider : Control
    {
        public enum ColorSliderOrientation
        {
            Vertical=0,Horizontal=1
        }

        private ColorHLS _baseColor;
        private ColorHLS _currentColor;
        private ColorHLS _previusColor;
        private ColorSliderMode _colorMode;
        private Rectangle _cursorRect;
        private bool _onDrag;
        private ColorSliderOrientation _orientation;

        private float _saturation;
        private float _luminance;
        private float _hue;

        private PixelBuffer _pixBuffer;

        internal event ColorChangedDelegate ColorChanged;

        public ColorSlider()
            : this(ColorSliderMode.Hue,ColorSliderOrientation.Vertical)
        {
        }
        public ColorSlider(ColorSliderMode colorMode,ColorSliderOrientation orientation)
        {
            _colorMode = colorMode;
            _orientation = orientation;

            _baseColor = new ColorHLS(255, 255, 0, 0);
            _currentColor = _baseColor.Clone();
            _previusColor = _baseColor.Clone();

            this.SetStyle(
                ControlStyles.AllPaintingInWmPaint |
                ControlStyles.FixedHeight |
                ControlStyles.FixedWidth |
                ControlStyles.OptimizedDoubleBuffer |
                ControlStyles.UserPaint,
                true);

            if (_orientation == ColorSliderOrientation.Vertical)
            {
                this.Size = new System.Drawing.Size(20, 256);
                _cursorRect = new Rectangle(0, 256 - 4, 20, 7);
            }
            else
            {
                this.Size = new System.Drawing.Size(256, 20);
                _cursorRect = new Rectangle(- 3,0 , 7, 20);
            }

            
            _onDrag = false;

            //_pixBuffer = new PixelBuffer(this.Width, this.Height);
            //FillBuffer();

            this.Invalidate();
        }

        public ColorSliderMode ColorMode
        {
            get
            {
                return _colorMode;
            }
            set
            {
                _colorMode = value;
                UpdateControl();
                this.Invalidate();
            }
        }
        public ColorHLS BaseColor
        {
            get
            {
                return _baseColor;
            }
            set
            {
                _baseColor = value;
                ChangeBaseColor(_baseColor);
            }
        }
        /*public ColorHLS CurrentColor
        {
            get 
            { 
                return _currentColor; 
            }
            set 
            { 
                _currentColor = value; 
            }
        }*/
        /*public float Saturation
        {
            get
            {
                return _saturation;
            }
        }
        public float Brightness
        {
            get
            {
                return _brightness;
            }
        }
        public float Hue
        {
            get
            {
                return _hue; ;
            }
        }*/
        public float Value
        {
            get
            {
                switch (_colorMode)
                {
                    default:
                    case ColorSliderMode.Hue:
                        return _hue;
                    case ColorSliderMode.Saturation:
                        return _saturation;
                    case ColorSliderMode.Luminance:
                        return _luminance;
                }
            }
            set
            {
                SetValue(value);
            }
        }
        public ColorSliderOrientation Orientation
        {
            get
            {
                return _orientation;
            }
            set
            {
                _orientation = value;
                this.OnSizeChanged(new EventArgs());
                
            }
        }

        private void SetValue(float value)
        {

            if (_orientation == ColorSliderOrientation.Vertical)
            {
                float y = -3;
                switch (_colorMode)
                {
                    default:
                    case ColorSliderMode.Hue:
                        value %= 360;
                        y = 255 - (((value / 359) * 255) + 3);
                        _hue = value;
                        break;
                    case ColorSliderMode.Saturation:
                        if (value < 0 || value > 1)
                        {
                            throw new ArgumentOutOfRangeException("value");
                        }
                        y = (((1 - value) * 255)) - 3;
                        _saturation = value;
                        break;
                    case ColorSliderMode.Luminance:
                        if (value < 0 || value > 1)
                        {
                            throw new ArgumentOutOfRangeException("value");
                        }
                        y = ((1 - value) * 255) - 3;
                        _luminance = value;
                        break;
                }
                _cursorRect.Y = (int)y;
                _currentColor = GetColorAt(1, _cursorRect.Y + 3);
            }
            else
            {
                float x = -3;
                switch (_colorMode)
                {
                    default:
                    case ColorSliderMode.Hue:
                        value %= 360;
                        x = (((value / 359) * 255) - 3);
                        _hue = value;
                        break;
                    case ColorSliderMode.Saturation:
                        if (value < 0 || value > 1)
                        {
                            throw new ArgumentOutOfRangeException("value");
                        }
                        x = (value * 255) - 3;
                        _saturation = value;
                        break;
                    case ColorSliderMode.Luminance:
                        if (value < 0 || value > 1)
                        {
                            throw new ArgumentOutOfRangeException("value");
                        }
                        x = (value * 255) - 3;
                        _luminance = value;
                        break;
                }
                _cursorRect.X = (int)x;
                _currentColor = GetColorAt(_cursorRect.X+3, 1);
            }
            
            this.Invalidate();
        }
        private void ChangeBaseColor(ColorHLS color)
        {
            FillBuffer();
            this.Invalidate();
        }
        private void ChangeCurrentColor(ColorHLS color)
        {
            throw new NotImplementedException();
        }
        private void UpdateControl()
        {
            if (_orientation == ColorSliderOrientation.Vertical)
            {
                _cursorRect = new Rectangle(0, 256 - 4, 20, 7);
            }
            else
            {
                _cursorRect = new Rectangle(-3, 0, 7, 20);
            }
            _pixBuffer = new PixelBuffer(this.Width, this.Height);
            FillBuffer();
        }
        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);

            _previusColor = _currentColor.Clone();
            MouseEvent(e);
            _onDrag = true;
        }
        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);

            if (_onDrag)
            {
                MouseEvent(e);
            }
        }
        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);

            _onDrag = false;

            MouseEvent(e);
        }
        protected override void OnSizeChanged(EventArgs e)
        {
            if (_orientation == ColorSliderOrientation.Vertical)
            {
                this.Size = new System.Drawing.Size(20, 256);
            }
            else
            {
                this.Size = new System.Drawing.Size(256, 20);
            }

            UpdateControl();
            
            this.Invalidate();
        }
        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            DrawBackground(e);
            DrawCursor(e);
        }

        private void FillBuffer()
        {
            ColorHLS[] colors = new ColorHLS[0];

            switch (_colorMode)
            {
                default:
                case ColorSliderMode.Hue:
                    colors = ColorUtils.CreateGradientColorArray(
                        new ColorHLS[]
                            {
                                new ColorHLS(255,   255,    0,      0), 
                                new ColorHLS(255,   255,    255,    0),
                                new ColorHLS(255,   0,      255,    0),
                                new ColorHLS(255,   0,      255,    255),
                                new ColorHLS(255,   0,      0,      255),
                                new ColorHLS(255,   255,    0,      255),
                                new ColorHLS(255,   255,    0,      0)
                            },
                        256
                        );
                    break;
                case ColorSliderMode.Saturation:
                    colors = ColorUtils.CreateGradientColorArray(
                        new ColorHLS[]
                            {
                                new ColorHLS(255,   255,    255,      255), 
                                _baseColor
                            },
                        256
                        );
                    break;
                case ColorSliderMode.Luminance:
                    colors = ColorUtils.CreateGradientColorArray(
                        new ColorHLS[]
                            {
                                new ColorHLS(255,   0,    0,      0), 
                                _baseColor
                            },
                        256
                        );
                    break;
            }

            if (_orientation == ColorSliderOrientation.Vertical)
            {
                //ColorHLS[] colors = ColorUtils.CreateGradientColorArray(new ColorHLS(255,255, 0, 0), new ColorHLS(255,255, 255, 0), 256);
                for (int y = 0; y < this.Height; y++)
                {
                    _pixBuffer.DrawLine(0, this.Height - 1 - y, this.Width, this.Height - 1 - y, colors[y]);
                }
            }
            else
            {
                for (int x = 0; x < this.Width; x++)
                {
                    _pixBuffer.DrawLine(x, 0, x, this.Height, colors[x]);
                }
            }
        }
        private void MouseEvent(MouseEventArgs e)
        {
            if (_orientation == ColorSliderOrientation.Vertical)
            {
                Point newPos = new Point(0, e.Y - 3);
                if (newPos.Y < -3) newPos.Y = -3;
                if (newPos.Y > (256 - 4)) newPos.Y = (256 - 4);
                _cursorRect.Location = newPos;

                _currentColor = GetColorAt(1, newPos.Y + 3);

                switch (_colorMode)
                {
                    case ColorSliderMode.Saturation:
                        _saturation = 1 - ((float)(newPos.Y + 3) / 255);
                        break;
                    case ColorSliderMode.Luminance:
                        _luminance = 1 - ((float)(newPos.Y + 3) / 255); //_currentColor.Luminance * 2;
                        break;
                    case ColorSliderMode.Hue:
                        _hue = (float)Math.Round(((double)(255 - (newPos.Y + 3)) / 255) * 359, MidpointRounding.ToEven); //_currentColor.Hue;
                        break;
                }
            }
            else
            {
                Point newPos = new Point(e.X-3, 0);
                if (newPos.X < -3) newPos.X = -3;
                if (newPos.X > (256 - 4)) newPos.X = (256 - 4);
                _cursorRect.Location = newPos;

                _currentColor = GetColorAt(newPos.X+3, 1);

                switch (_colorMode)
                {
                    case ColorSliderMode.Saturation:
                        _saturation = ((float)(newPos.X + 3) / 255);
                        break;
                    case ColorSliderMode.Luminance:
                        _luminance = ((float)(newPos.X + 3) / 255); //_currentColor.Luminance * 2;
                        break;
                    case ColorSliderMode.Hue:
                        _hue = (float)Math.Round(((double)((newPos.Y + 3)) / 255) * 359, MidpointRounding.ToEven); //_currentColor.Hue;
                        break;
                }
            }

            this.Invalidate();

            if (this.ColorChanged != null)
                this.ColorChanged(this, new ColorChangedEventArgs(_previusColor, _currentColor));
        }
        private void DrawBackground(PaintEventArgs e)
        {
            _pixBuffer.Render(e.Graphics, 0, 0);
        }
        private void DrawCursor(PaintEventArgs e)
        {
            Pen cursorPen = new Pen(_currentColor.Complementary);
            cursorPen.Width = 1f;

            if (_orientation == ColorSliderOrientation.Vertical)
            {
                e.Graphics.DrawLine(cursorPen, _cursorRect.X, _cursorRect.Y, _cursorRect.Right, _cursorRect.Y);
                e.Graphics.DrawLine(cursorPen, _cursorRect.X, _cursorRect.Y + 1, _cursorRect.Right, _cursorRect.Y + 1);
                e.Graphics.DrawLine(cursorPen, _cursorRect.X, _cursorRect.Y + 2, _cursorRect.Right, _cursorRect.Y + 2);

                e.Graphics.DrawLine(cursorPen, _cursorRect.X, _cursorRect.Y + 4, _cursorRect.Right, _cursorRect.Y + 4);
                e.Graphics.DrawLine(cursorPen, _cursorRect.X, _cursorRect.Y + 5, _cursorRect.Right, _cursorRect.Y + 5);
                e.Graphics.DrawLine(cursorPen, _cursorRect.X, _cursorRect.Y + 6, _cursorRect.Right, _cursorRect.Y + 6);
            }
            else
            {
                e.Graphics.DrawLine(cursorPen, _cursorRect.X, _cursorRect.Y, _cursorRect.X, _cursorRect.Bottom);
                e.Graphics.DrawLine(cursorPen, _cursorRect.X + 1, _cursorRect.Y, _cursorRect.X + 1, _cursorRect.Bottom);
                e.Graphics.DrawLine(cursorPen, _cursorRect.X + 2, _cursorRect.Y, _cursorRect.X + 2, _cursorRect.Bottom);

                e.Graphics.DrawLine(cursorPen, _cursorRect.X + 4, _cursorRect.Y, _cursorRect.X + 4, _cursorRect.Bottom);
                e.Graphics.DrawLine(cursorPen, _cursorRect.X + 5, _cursorRect.Y, _cursorRect.X + 5, _cursorRect.Bottom);
                e.Graphics.DrawLine(cursorPen, _cursorRect.X + 6, _cursorRect.Y, _cursorRect.X + 6, _cursorRect.Bottom);
            }
        }
        private ColorHLS GetColorAt(int x, int y)
        {
            ColorHLS color;
            _pixBuffer.GetPixel(x, y, out color);
            return color;
        }
    }
}
