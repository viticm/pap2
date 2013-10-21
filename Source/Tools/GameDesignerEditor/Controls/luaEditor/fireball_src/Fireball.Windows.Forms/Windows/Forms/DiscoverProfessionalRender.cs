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
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;

namespace Fireball.Windows.Forms
{
    public class DiscoverProfessionalRender : DiscoverRenderBase,IDiscoverRenderer,IDisposable
    {
        private ProfessionalColorTable _ProfessionalColorTable = null;


        public DiscoverProfessionalRender()
        {
     
            InitializeColors();
        }

        protected override void OnSystemColorsChanged()
        {
            base.OnSystemColorsChanged();
            InitializeColors();
        }

        #region IDiscoverRenderer Members


        public void DrawBackground(System.Drawing.Graphics graphics, System.Drawing.Rectangle bounds, System.Drawing.Color backColor)
        {
            graphics.FillRectangle(new SolidBrush(backColor), bounds);

            using (Pen borderPen = new Pen(_ProfessionalColorTable.ToolStripBorder, 1))
            {
                bounds.Width--;
                bounds.Height--;

                graphics.DrawRectangle(borderPen, bounds);
            }
        }

        public void DrawChevron(System.Drawing.Graphics graphics, System.Drawing.Rectangle bounds, System.Drawing.Color color)
        {
            int xMiddle = (bounds.X + (bounds.Width / 2)) - 2;
            int yMiddle = bounds.Y + (bounds.Height / 2);
            graphics.SmoothingMode = SmoothingMode.HighQuality;
            using (Pen linePen = new Pen(color))
            {
                graphics.DrawLine(linePen, (int)(xMiddle - 1), (int)(yMiddle - 5), (int)(xMiddle + 1), (int)(yMiddle - 3));
                graphics.DrawLine(linePen, (int)(xMiddle - 1), (int)(yMiddle - 1), (int)(xMiddle + 1), (int)(yMiddle - 3));
                graphics.DrawLine(linePen, xMiddle, (int)(yMiddle - 5), (int)(xMiddle + 2), (int)(yMiddle - 3));
                graphics.DrawLine(linePen, xMiddle, (int)(yMiddle - 1), (int)(xMiddle + 2), (int)(yMiddle - 3));
                graphics.DrawLine(linePen, (int)(xMiddle + 2), (int)(yMiddle - 5), (int)(xMiddle + 4), (int)(yMiddle - 3));
                graphics.DrawLine(linePen, (int)(xMiddle + 2), (int)(yMiddle - 1), (int)(xMiddle + 4), (int)(yMiddle - 3));
                graphics.DrawLine(linePen, (int)(xMiddle + 3), (int)(yMiddle - 5), (int)(xMiddle + 5), (int)(yMiddle - 3));
                graphics.DrawLine(linePen, (int)(xMiddle + 3), (int)(yMiddle - 1), (int)(xMiddle + 5), (int)(yMiddle - 3));
                graphics.DrawLine(linePen, xMiddle, (int)(yMiddle + 4), (int)(xMiddle + 4), (int)(yMiddle + 4));
                graphics.DrawLine(linePen, (int)(xMiddle + 1), (int)(yMiddle + 5), (int)(xMiddle + 3), (int)(yMiddle + 5));
                graphics.DrawLine(linePen, (int)(xMiddle + 2), (int)(yMiddle + 6), (int)(xMiddle + 2), (int)(yMiddle + 4));
            }

        }

        public void DrawDiscoverContentPane(System.Drawing.Graphics graphics, System.Drawing.Rectangle bounds, System.Windows.Forms.DrawItemState state, DiscoverPane pane, System.Drawing.Font font, bool rtl)
        {
            int num1 = 5;
            bounds.Inflate(-7, 0);
            Rectangle imgRect = Rectangle.Empty;
            Rectangle rectangle2 = bounds;
            if (rtl)
            {
                if (pane.LargeImage != null)
                {
                    imgRect = new Rectangle(bounds.Right - pane.LargeImage.Width, (bounds.Top
                        + (bounds.Height / 2)) - (pane.LargeImage.Height / 2), pane.LargeImage.Width,
                        pane.LargeImage.Height);
                    rectangle2.Width -= (pane.LargeImage.Width + num1);
                }
            }
            else if (pane.LargeImage != null)
            {
                imgRect = new Rectangle(bounds.Left, (bounds.Top + (bounds.Height / 2))
                    - (pane.LargeImage.Height / 2), pane.LargeImage.Width, pane.LargeImage.Height);
                rectangle2.X += (pane.LargeImage.Width + num1);
                rectangle2.Width -= (pane.LargeImage.Width + num1);
            }
            if (imgRect != Rectangle.Empty)
            {
                Image image1 = pane.LargeImage;
                if ((state & DrawItemState.Disabled) == DrawItemState.Disabled)
                {
                    graphics.DrawImage(image1, imgRect, 0, 0, image1.Width, image1.Height,
                        GraphicsUnit.Pixel, new System.Drawing.Imaging.ImageAttributes()); //TODO:ImageAttributes
                }
                else
                {
                    graphics.DrawImage(image1, imgRect);
                }
            }
            using (StringFormat strFormat = new StringFormat(StringFormat.GenericTypographic))
            {
                strFormat.Trimming = StringTrimming.EllipsisCharacter;
                strFormat.FormatFlags |= StringFormatFlags.NoWrap;
                strFormat.LineAlignment = StringAlignment.Center;
                strFormat.HotkeyPrefix = HotkeyPrefix.Show;
                if (rtl)
                {
                    strFormat.FormatFlags |= StringFormatFlags.DirectionRightToLeft;
                }
                if ((state & DrawItemState.Disabled) == DrawItemState.Disabled)
                {
                    graphics.DrawString(pane.Text, font, SystemBrushes.ControlDark,
                        (RectangleF)rectangle2, strFormat);
                }
                else
                {
                    using (SolidBrush stringBrush = new SolidBrush(((state & DrawItemState.Selected) ==
                        DrawItemState.Selected) ? this.BorderColor : pane.ForeColor)) //TODO:this.BorderColor
                    {
                        graphics.DrawString(pane.Text, font, stringBrush, (RectangleF)rectangle2, strFormat);
                    }
                }
            }

        }

        public void DrawDiscoverContentPaneBackground(System.Drawing.Graphics graphics, System.Drawing.Rectangle bounds, System.Windows.Forms.DrawItemState state)
        {
            this._DrawFooterPaneBackground(graphics, bounds, state);

            using (Pen pen1 = new Pen(this.BorderColor))
            {
                graphics.DrawLine(pen1, bounds.X, bounds.Y, bounds.Right, bounds.Y);
            }
        }

        public void DrawSeparator(System.Drawing.Graphics graphics, System.Drawing.Rectangle bounds, string text, System.Drawing.Font font, System.Drawing.Color foreColor, bool rtl)
        {
            if ((bounds.Width > 0) && (bounds.Height > 0))
            {
                using (LinearGradientBrush brush1 = new LinearGradientBrush(new Point(bounds.X, bounds.Y - 1),
                    new Point(bounds.X, bounds.Bottom), this.DividerBackgroundColor1,
                    this.DividerBackgroundColor2))
                {
                    graphics.FillRectangle(brush1, bounds);
                }
                using (Pen pen1 = new Pen(this.DividerBorderColor))
                {
                    graphics.DrawLine(pen1, bounds.X, (int)(bounds.Bottom - 1), (int)(bounds.Right - 1), (int)(bounds.Bottom - 1));
                }
                using (Pen pen2 = new Pen(this.DividerBorderColor))
                {
                    graphics.DrawLine(pen2, bounds.X, bounds.Y, (int)(bounds.Right - 1), bounds.Y);
                }
                using (StringFormat format1 = this.GetStringFormat(rtl))
                {
                    using (SolidBrush brush2 = new SolidBrush(foreColor))
                    {
                        graphics.DrawString(text, font, brush2, (RectangleF)bounds, format1);
                    }
                }
            }

        }

        private StringFormat GetStringFormat(bool rtl)
        {
            StringFormat strFormat = new StringFormat(StringFormat.GenericDefault);
            strFormat.Trimming = StringTrimming.EllipsisCharacter;
            strFormat.FormatFlags = StringFormatFlags.NoWrap;
            strFormat.LineAlignment = StringAlignment.Center;
            strFormat.HotkeyPrefix = HotkeyPrefix.Hide;
            if (rtl)
            {
                strFormat.FormatFlags |= StringFormatFlags.DirectionRightToLeft;
            }
            return strFormat;
        }

        public void DrawDiscoverFooterPane(System.Drawing.Graphics graphics, System.Drawing.Rectangle bounds, System.Windows.Forms.DrawItemState state, DiscoverPane pane, System.Drawing.Font font)
        {
            Rectangle imgBounds = new Rectangle((bounds.X + (bounds.Width / 2)) - 8,
                (bounds.Y + (bounds.Height / 2)) - 8, 0x10, 0x10);
            Image drawImage = (pane.SmallImage != null) ? pane.SmallImage : pane.LargeImage;

            if (drawImage != null)
            {
                if ((state & DrawItemState.Disabled) == DrawItemState.Disabled)
                {
                    graphics.DrawImage(drawImage, imgBounds, 0, 0, drawImage.Width, drawImage.Height, GraphicsUnit.Pixel,
                        new System.Drawing.Imaging.ImageAttributes());//TODO: ImageAttributes
                }
                else
                {
                    graphics.DrawImage(drawImage, imgBounds);
                }
            }

        }

        public void DrawDiscoverFooterPaneBackground(System.Drawing.Graphics graphics, System.Drawing.Rectangle bounds, System.Windows.Forms.DrawItemState itemState)
        {
            if (itemState != DrawItemState.None)
            {
                _DrawFooterPaneBackground(graphics, bounds, itemState);
            }
        }

        private void _DrawFooterPaneBackground(Graphics gfx, Rectangle bounds, DrawItemState itemState)
        {
            Color beginColor;
            Color endColor;

            if (((itemState & DrawItemState.Selected) == DrawItemState.Selected) || ((itemState & (DrawItemState.HotLight | DrawItemState.Checked)) == (DrawItemState.HotLight | DrawItemState.Checked)))
            {
                beginColor = this.PaneSelectedBackgroundColor1;
                endColor = this.PaneSelectedBackgroundColor2;
            }
            else if ((itemState & DrawItemState.HotLight) == DrawItemState.HotLight)
            {
                beginColor = this._ProfessionalColorTable.ButtonPressedGradientBegin;
                endColor = this._ProfessionalColorTable.ButtonPressedGradientEnd;
            }
            else if ((itemState & DrawItemState.Checked) == DrawItemState.Checked)
            {
                beginColor = this.PaneCheckedBackgroundColor1;
                endColor = this.PaneCheckedBackgroundColor2;
            }
            else
            {
                beginColor = this.PaneBackgroundColor1;
                endColor = this.PaneBackgroundColor2;
            }
            using (LinearGradientBrush fillBrush = new LinearGradientBrush(
                new Point(bounds.X, bounds.Y - 1), 
                new Point(bounds.X, bounds.Bottom), beginColor, endColor))
            {
                gfx.FillRectangle(fillBrush, bounds);
            }
        }


        public void DrawGripper(System.Drawing.Graphics graphics, System.Drawing.Rectangle bounds)
        {
            using (LinearGradientBrush lnBrush = new LinearGradientBrush(new Point(bounds.X, bounds.Y - 1),
                new Point(bounds.X, bounds.Bottom), this.GripperColor1,
               this.GripperColor2))
            {
                graphics.FillRectangle(lnBrush, bounds);
            }

            int num1 = (bounds.Width / 2)-16;
            int num2 = bounds.Y + 3;

            using (SolidBrush circleBrush = new SolidBrush(Color.FromArgb(40, 50, 0x47)))
            {
                for (int num3 = 0; num3 < 9; num3++)
                {
                    bounds = new Rectangle((num1 + (num3 * 4)) + 1, num2 + 1, 2, 2);
                    graphics.FillRectangle(Brushes.White, bounds);
                    bounds = new Rectangle(num1 + (num3 * 4), num2, 2, 2);
                    graphics.FillRectangle(circleBrush, bounds);
                }
            }
        }

        public void DrawHeader(System.Drawing.Graphics graphics, System.Drawing.Rectangle bounds, string text, System.Drawing.Font font, System.Drawing.Image image, bool rtl)
        {
            if ((bounds.Width > 0) && (bounds.Height > 0))
            {
                using (LinearGradientBrush lnBrush = new LinearGradientBrush(bounds,
                    this.HeaderBackgroundColor1,
                    this.HeaderBackgroundColor2, LinearGradientMode.Vertical))
                {
                    graphics.FillRectangle(lnBrush, bounds);
                }

                if (image != null)
                {
                    Rectangle imgBounds = bounds;
                    imgBounds.Y = (bounds.Top + (bounds.Height / 2)) - (image.Height / 2);
                    if (rtl)
                    {
                        imgBounds.X = imgBounds.Y - bounds.Y;
                    }
                    else
                    {
                        imgBounds.X = imgBounds.Right - (image.Width + (imgBounds.Y - bounds.Y));
                    }
                    imgBounds.Size = image.Size;
                    graphics.DrawImage(image, imgBounds);
                    if (rtl)
                    {
                        bounds.X += imgBounds.Right;
                        bounds.Width -= imgBounds.Right;
                    }
                    else
                    {
                        bounds.Width -= (bounds.Right - imgBounds.Left);
                    }
                }
                bounds.Height--;

                using (StringFormat strFormat = this.GetStringFormat(rtl))
                {
                    using (SolidBrush brush2 = new SolidBrush(this.HeaderTextColor))
                    {
                        graphics.DrawString(text, font, brush2, (RectangleF)bounds, strFormat);
                    }
                }
            }

        }

        public System.Drawing.Color BorderColor
        {
            get
            {
                return _ProfessionalColorTable.ToolStripBorder;
            }
        }

        #endregion


        private void InitializeColors()
        {
            _ProfessionalColorTable = new ProfessionalColorTable();
            _DividerBackgroundColor1 = this._ProfessionalColorTable.ToolStripGradientBegin;
            _DividerBackgroundColor2 = this._ProfessionalColorTable.ToolStripGradientEnd;

            _DividerBorderColor = _ProfessionalColorTable.SeparatorDark;

            _GripperColor1 = _ProfessionalColorTable.SeparatorLight;
            _GripperColor2 = _ProfessionalColorTable.SeparatorDark;

            _HeaderBackgroundColor2 = SystemColors.ActiveCaption;
            _HeaderBackgroundColor1 = SystemColors.InactiveCaption;

            _HeaderTextColor = Color.White;


            _PaneBackgroundColor1 = this._ProfessionalColorTable.ToolStripGradientBegin;
            _PaneBackgroundColor2 = this._ProfessionalColorTable.ToolStripGradientEnd;

            _PaneSelectedBackgroundColor1 = this._ProfessionalColorTable.ButtonSelectedGradientBegin;
            _PaneSelectedBackgroundColor2 = this._ProfessionalColorTable.ButtonSelectedGradientEnd;

            //beginColor = this._ProfessionalColorTable.ButtonCheckedGradientBegin;
            //endColor = this._ProfessionalColorTable.ButtonCheckedGradientEnd;
        }

        private Color _DividerBackgroundColor1;
        private Color _DividerBackgroundColor2;
        private Color _DividerBorderColor;
        private Color _GripperColor1;
        private Color _GripperColor2;
        private Color _HeaderBackgroundColor1;
        private Color _HeaderBackgroundColor2;
        private Color _HeaderTextColor;
        private Color _PaneBackgroundColor1;
        private Color _PaneBackgroundColor2;
        private Color _PaneSelectedBackgroundColor1;
        private Color _PaneSelectedBackgroundColor2;

        private Color _PaneCheckedBackgroundColor1;
        private Color _PaneCheckedBackgroundColor2;


        public Color DividerBackgroundColor1
        {
            get
            {
                return _DividerBackgroundColor1;
            }
            set
            {
                _DividerBackgroundColor1 = value;
            }
        }

        public Color DividerBackgroundColor2
        {
            get
            {
                return _DividerBackgroundColor2;
            }
            set
            {
                _DividerBackgroundColor2 = value;
            }
        }

        public Color DividerBorderColor
        {
            get
            {
                return _DividerBorderColor;
            }
            set
            {
                _DividerBorderColor = value; 
            }
        }

        public Color GripperColor1
        {
            get
            {
                return _GripperColor1;
            }
            set
            {
                _GripperColor1 = value;
            }
        }

        public Color GripperColor2
        {
            get
            {
                return _GripperColor2;
            }
            set
            {
                _GripperColor2 = value;
            }
        }

        public Color HeaderBackgroundColor1
        {
            get
            {
                return _HeaderBackgroundColor1;
            }
            set
            {
                _HeaderBackgroundColor1 = value;
            }
        }

        public Color HeaderBackgroundColor2
        {
            get
            {
                return _HeaderBackgroundColor2;
            }
            set
            {
                _HeaderBackgroundColor2 = value;
            }
        }

        public Color HeaderTextColor
        {
            get
            {
                return _HeaderTextColor;
            }
            set
            {
                _HeaderTextColor = value;
            }
        }

        public Color PaneBackgroundColor1
        {
            get
            {
                return _PaneBackgroundColor1;
            }
            set
            {
                _PaneBackgroundColor1 = value;
            }
        }

        public Color PaneBackgroundColor2
        {
            get
            {
                return _PaneBackgroundColor2;
            }
            set
            {
                _PaneBackgroundColor2 = value;
            }
        }

        public Color PaneSelectedBackgroundColor1
        {
            get
            {
                return _PaneSelectedBackgroundColor1;
            }
            set
            {
                _PaneSelectedBackgroundColor1 = value;
            }
        }

        public Color PaneSelectedBackgroundColor2
        {
            get
            {
                return _PaneSelectedBackgroundColor2;
            }
            set
            {
                _PaneSelectedBackgroundColor2 = value;
            }
        }

        public Color PaneCheckedBackgroundColor1
        {
            get
            {
                return _PaneCheckedBackgroundColor1;
            }
            set
            {
                _PaneCheckedBackgroundColor1 = value;
            }
        }

        public Color PaneCheckedBackgroundColor2
        {
            get
            {
                return _PaneCheckedBackgroundColor2;
            }
            set
            {
                _PaneCheckedBackgroundColor2 = value;
            }
        }
    }
}
