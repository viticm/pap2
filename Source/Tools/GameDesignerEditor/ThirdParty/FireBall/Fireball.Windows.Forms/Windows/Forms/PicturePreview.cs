//    Copyright (C) 2005  Riccardo Marzi
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
using System.Collections.Generic;
//using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Imaging;

namespace Fireball.Windows.Forms
{
    public class PicturePreview:Control
    {
        private Image _img;
        private float _zoom;
        private string _info;
        private Brush _infoBrush;
        private Font _infoFont;
        private StringFormat _sf;
        private bool _onPan;
        private Point _panStart;
        private Point _offset;
        private SizeF _viewDim;
        private Cursor _handCursor;
        private bool _showInfo;

        private const float ZOOM_STEP = 0.1f;
        private const float ZOOM_MIN = 0.1f;
        private const float ZOOM_MAX = 8f;

        public PicturePreview()
        {
            this.SetStyle(
                ControlStyles.AllPaintingInWmPaint
                | ControlStyles.OptimizedDoubleBuffer
                | ControlStyles.ResizeRedraw
                | ControlStyles.UserPaint
                ,true
            );

            Initialize();

            System.IO.MemoryStream ms = new System.IO.MemoryStream(Properties.Resources.hand, false);
            ms.Position = 0;
            _handCursor = new Cursor(ms);
            ms.Close();

            _infoBrush = new SolidBrush(Color.FromArgb(180, SystemColors.Window));
            _infoFont = new Font("MS Sans Serif", 8.5f, FontStyle.Regular);
            _sf = new StringFormat();
            _sf.LineAlignment = StringAlignment.Center;
        }

        private void Initialize()
        {
            _img = null;
            _zoom = 1f;
            _offset = new Point(0, 0);
        }

        public override void Refresh()
        {
            base.Refresh();
            this.Invalidate();
        }
        protected override void OnPaint(PaintEventArgs e)
        {
            if (_img != null)
            {
                _viewDim = new SizeF(_img.Width * _zoom, _img.Height * _zoom);

                if (_viewDim.Width <= this.Width)
                {
                    _offset.X = 0;
                }
                else
                {
                    if (_offset.X >= GetMaxRightOffset()) _offset.X = GetMaxRightOffset();
                    if (_offset.X <= GetMaxLeftOffset()) _offset.X = GetMaxLeftOffset();
                }

                if (_viewDim.Height <= this.Height)
                {
                    _offset.Y = 0;
                }
                else
                {
                    if (_offset.Y >= GetMaxDownOffset()) _offset.Y = GetMaxDownOffset();
                    if (_offset.Y <= GetMaxUpOffset()) _offset.Y = GetMaxUpOffset();
                }

                e.Graphics.TranslateTransform(((this.Width - _viewDim.Width) / 2) + _offset.X, ((this.Height - _viewDim.Height) / 2) + _offset.Y);
                e.Graphics.ScaleTransform(_zoom, _zoom);

                e.Graphics.DrawImage(_img, 0, 0, _img.Width, _img.Height);

                e.Graphics.ResetTransform();

                if (_showInfo)
                {
                    Rectangle infoRect = new Rectangle(0, this.Height - 22, this.Width, 22);
                    e.Graphics.FillRectangle(_infoBrush, infoRect);
                    e.Graphics.DrawLine(SystemPens.ControlDark, infoRect.X, infoRect.Y, infoRect.Width, infoRect.Y);
                    e.Graphics.DrawLine(SystemPens.ControlDark, infoRect.X, infoRect.Bottom - 1, infoRect.Width, infoRect.Bottom - 1);
                    e.Graphics.DrawString(_info, _infoFont, SystemBrushes.WindowText, new Point(infoRect.X + 4, infoRect.Y + (infoRect.Height / 2)), _sf);
                }

            }
        }
        protected override void OnMouseDown(MouseEventArgs e)
        {
            _panStart = e.Location;
            _onPan = true;
        }
        protected override void OnMouseUp(MouseEventArgs e)
        {
            Point panDelta = new Point(e.Location.X - _panStart.X, e.Location.Y - _panStart.Y);
            _onPan = false;
            _offset.X += panDelta.X;
            _offset.Y += panDelta.Y;
            this.Invalidate();
            //MessageBox.Show(panDelta.ToString());
        }
        protected override void OnMouseMove(MouseEventArgs e)
        {
            if (_onPan)
            {
                Point panDelta = new Point(e.Location.X - _panStart.X, e.Location.Y - _panStart.Y);
                _offset.X += panDelta.X;
                _offset.Y += panDelta.Y;
                _panStart = e.Location;
                this.Invalidate();
            }
        }

        private int GetMaxDownOffset()
        {
            return (int)(_viewDim.Height - this.Height) / 2;
        }
        private int GetMaxUpOffset()
        {
            return (int)-(_viewDim.Height - this.Height) / 2;
        }
        private int GetMaxRightOffset()
        {
            return (int)(_viewDim.Width - this.Width) / 2;
        }
        private int GetMaxLeftOffset()
        {
            return (int)-(_viewDim.Width - this.Width) / 2;
        }

        private ImageCodecInfo GetCodecInfo(ImageFormat format)
        {
            string formatName = "BMP";

            if (format == ImageFormat.Bmp)
            {
                formatName = "BMP";
            }
            else if (format == ImageFormat.Gif)
            {
                formatName = "GIF";
            }
            else if (format == ImageFormat.Jpeg)
            {
                formatName = "JPEG";
            }
            else if (format == ImageFormat.Png)
            {
                formatName = "PNG";
            }
            else if (format == ImageFormat.Tiff)
            {
                formatName = "TIFF";
            }

            ImageCodecInfo[] codecs = ImageCodecInfo.GetImageEncoders();
            foreach (ImageCodecInfo codec in codecs)
            {
                if (codec.FormatDescription == formatName)
                {
                    return codec;
                }
            }

            return null;
        }

        public Image Image
        {
            get
            {
                return _img;
            }
            set
            {
                Initialize();
                _img = value;
                this.Invalidate();
            }
        }
        public float Zoom
        {
            get
            {
                return _zoom;
            }
            set
            {
                if (value < 0.01f) value = 0.01f;
                if (value > 8) value = 8f;
                _zoom = value;
                this.Invalidate();
            }
        }
        public string Info
        {
            get
            {
                return _info;
            }
            set
            {
                _info = value;
                this.Invalidate();
            }
        }
        public bool UseHandCursor
        {
            get
            {
                return this.Cursor.Equals(_handCursor);
            }
            set
            {
                if (value)
                    this.Cursor = _handCursor;
                else
                    this.Cursor = Cursors.Default;
            }
        }
        public bool ShowInfo
        {
            get
            {
                return _showInfo;
            }
            set
            {
                _showInfo = value;
                this.Invalidate();
            }
        }
        public Size ImageSize
        {
            get
            {
                return _img.Size;
            }
        }

        public void Load(string filename)
        {
            if (filename == null)
            {
                _img = null;
                this.Invalidate();
            }
            if (System.IO.File.Exists(filename))
            {
                _img = System.Drawing.Image.FromFile(filename);
                _info = "";
                _zoom = 1;
                this.Invalidate();
            }
        }
        public void Save(string filename, ImageFormat format)
        {
            ImageCodecInfo codecInfo = GetCodecInfo(format);

            if (codecInfo == null)
            {
                throw new SystemException(codecInfo.CodecName + " codec isn't installed");
            }

            EncoderParameters pars = null;

            if (format == ImageFormat.Jpeg)
            {
                FormJpgQuality fjpg = new FormJpgQuality("Qualità Jpeg",100);
                if (fjpg.ShowDialog(this) == DialogResult.Cancel) return;

                pars = new EncoderParameters(1);
                pars.Param[0] = new EncoderParameter(System.Drawing.Imaging.Encoder.Quality,fjpg.JpegQuality);
            }
            else if (format== ImageFormat.Bmp)
            {
                pars = new EncoderParameters(1);
                pars.Param[0] = new EncoderParameter(System.Drawing.Imaging.Encoder.ColorDepth, 24);
            }
            else if (format == ImageFormat.Png)
            {
                pars = new EncoderParameters(1);
                pars.Param[0] = new EncoderParameter(System.Drawing.Imaging.Encoder.ColorDepth, 32);
            }

            if (pars == null)
            {
                _img.Save(filename, format);
            }
            else
            {
                _img.Save(filename, codecInfo, pars);
            }
        }

        public bool CanIncreaseZoom()
        {
            if (_zoom + (_zoom * ZOOM_STEP) > ZOOM_MAX) return false;
            return true;
        }
        public bool CanDecreaseZoom()
        {
            if (_zoom - (_zoom * ZOOM_STEP) < ZOOM_MIN) return false;
            return true;
        }
        public void IncreaseZoom()
        {
            if (CanIncreaseZoom())
            {
                _zoom += _zoom * ZOOM_STEP;
                this.Invalidate();
            }
            else
            {
                _zoom = ZOOM_MAX;
            }
        }
        public void DecreaseZoom()
        {
            if (CanDecreaseZoom())
            {
                _zoom -= _zoom * ZOOM_STEP;
                this.Invalidate();
            }
            else
            {
                _zoom = ZOOM_MIN;
            }
        }

        public void ZoomFit()
        {
            SizeF imgRect = new SizeF(_img.Width, _img.Height);
            SizeF newRect = imgRect;
            float boxRapp = this.Height / (float)this.Width;
            float imgRapp = _img.Height / (float)_img.Width;

            if (boxRapp < imgRapp)
            {
                newRect.Height = this.Height;
                newRect.Width = newRect.Height / imgRapp;
            }
            else
            { 
                newRect.Width = this.Width;
                newRect.Height = newRect.Width * imgRapp;
            }

            _zoom = newRect.Width / imgRect.Width;
            this.Invalidate();
        }
        public void ZoomOriginal()
        {
            _zoom = 1;
            this.Invalidate();
        }

    }
}
