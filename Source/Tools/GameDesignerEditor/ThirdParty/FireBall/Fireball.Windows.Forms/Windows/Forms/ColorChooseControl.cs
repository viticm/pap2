//  Copyright (C) 2005/2006  Riccardo Marzi <riccardo@dotnetfireball.org>
//	
//	This library is free software; you can redistribute it and/or
//	modify it under the terms of the GNU Lesser General Public
//	License as published by the Free Software Foundation; either
//	version 2.1 of the License, or (at your option) any later version.
//	
//	This library is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//	Lesser General Public License for more details.
//	
//	You should have received a copy of the GNU Lesser General Public
//	License along with this library; if not, write to the Free Software
//	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;

using Fireball.Drawing;

namespace Fireball.Windows.Forms
{
    public class ColorChooseControl:UserControl
    {
        private ColorPicker lsView;
        private ColorSlider hueView;
        private Label label1;
        private Label label2;
        private Label label3;
        private NumericUpDown numR;
        private NumericUpDown numG;
        private NumericUpDown numB;
        private Panel panelColor1;
        private Panel panelColor2;
        private Panel panelColors;
        private Panel panel1;
        private Panel panel2;
        private ColorChooseControl.ColorSlider saturationView;
        private Panel panel3;
        private ColorChooseControl.ColorSlider luminanceView;
        private Panel panel4;
        private Label label4;
        private Label label5;
        private Label label6;
        private NumericUpDown numHue;
        private NumericUpDown numSat;
        private NumericUpDown numLum;

        private ColorHLS _currentColor;

        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.numR = new System.Windows.Forms.NumericUpDown();
            this.numG = new System.Windows.Forms.NumericUpDown();
            this.numB = new System.Windows.Forms.NumericUpDown();
            this.panelColor1 = new System.Windows.Forms.Panel();
            this.panelColor2 = new System.Windows.Forms.Panel();
            this.panelColors = new System.Windows.Forms.Panel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.panel4 = new System.Windows.Forms.Panel();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.numHue = new System.Windows.Forms.NumericUpDown();
            this.numSat = new System.Windows.Forms.NumericUpDown();
            this.numLum = new System.Windows.Forms.NumericUpDown();
            this.luminanceView = new Fireball.Windows.Forms.ColorChooseControl.ColorSlider();
            this.saturationView = new Fireball.Windows.Forms.ColorChooseControl.ColorSlider();
            this.lsView = new Fireball.Windows.Forms.ColorChooseControl.ColorPicker();
            this.hueView = new Fireball.Windows.Forms.ColorChooseControl.ColorSlider();
            ((System.ComponentModel.ISupportInitialize)(this.numR)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numG)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numB)).BeginInit();
            this.panelColors.SuspendLayout();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel3.SuspendLayout();
            this.panel4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numHue)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numSat)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numLum)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(386, 100);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(18, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "R:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(386, 126);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(18, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "G:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(386, 152);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(17, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "B:";
            // 
            // numR
            // 
            this.numR.Location = new System.Drawing.Point(408, 98);
            this.numR.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.numR.Name = "numR";
            this.numR.Size = new System.Drawing.Size(48, 20);
            this.numR.TabIndex = 6;
            this.numR.Value = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.numR.ValueChanged += new System.EventHandler(this.numR_ValueChanged);
            // 
            // numG
            // 
            this.numG.Location = new System.Drawing.Point(408, 124);
            this.numG.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.numG.Name = "numG";
            this.numG.Size = new System.Drawing.Size(48, 20);
            this.numG.TabIndex = 6;
            this.numG.ValueChanged += new System.EventHandler(this.numG_ValueChanged);
            // 
            // numB
            // 
            this.numB.Location = new System.Drawing.Point(408, 150);
            this.numB.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.numB.Name = "numB";
            this.numB.Size = new System.Drawing.Size(48, 20);
            this.numB.TabIndex = 6;
            this.numB.ValueChanged += new System.EventHandler(this.numB_ValueChanged);
            // 
            // panelColor1
            // 
            this.panelColor1.BackColor = System.Drawing.Color.Red;
            this.panelColor1.Location = new System.Drawing.Point(1, 1);
            this.panelColor1.Name = "panelColor1";
            this.panelColor1.Size = new System.Drawing.Size(68, 34);
            this.panelColor1.TabIndex = 7;
            // 
            // panelColor2
            // 
            this.panelColor2.BackColor = System.Drawing.Color.Red;
            this.panelColor2.Location = new System.Drawing.Point(1, 35);
            this.panelColor2.Name = "panelColor2";
            this.panelColor2.Size = new System.Drawing.Size(68, 34);
            this.panelColor2.TabIndex = 7;
            // 
            // panelColors
            // 
            this.panelColors.BackColor = System.Drawing.SystemColors.ControlDark;
            this.panelColors.Controls.Add(this.panelColor1);
            this.panelColors.Controls.Add(this.panelColor2);
            this.panelColors.Location = new System.Drawing.Point(387, 5);
            this.panelColors.Name = "panelColors";
            this.panelColors.Size = new System.Drawing.Size(70, 70);
            this.panelColors.TabIndex = 8;
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.ControlDark;
            this.panel1.Controls.Add(this.hueView);
            this.panel1.Location = new System.Drawing.Point(347, 5);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(22, 258);
            this.panel1.TabIndex = 9;
            // 
            // panel2
            // 
            this.panel2.BackColor = System.Drawing.SystemColors.ControlDark;
            this.panel2.Controls.Add(this.lsView);
            this.panel2.Location = new System.Drawing.Point(3, 5);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(258, 258);
            this.panel2.TabIndex = 10;
            // 
            // panel3
            // 
            this.panel3.BackColor = System.Drawing.SystemColors.ControlDark;
            this.panel3.Controls.Add(this.saturationView);
            this.panel3.Location = new System.Drawing.Point(274, 5);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(22, 258);
            this.panel3.TabIndex = 11;
            // 
            // panel4
            // 
            this.panel4.BackColor = System.Drawing.SystemColors.ControlDark;
            this.panel4.Controls.Add(this.luminanceView);
            this.panel4.Location = new System.Drawing.Point(310, 5);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(22, 258);
            this.panel4.TabIndex = 12;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(386, 192);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(18, 13);
            this.label4.TabIndex = 5;
            this.label4.Text = "H:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(386, 218);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(17, 13);
            this.label5.TabIndex = 5;
            this.label5.Text = "S:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(386, 244);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(16, 13);
            this.label6.TabIndex = 5;
            this.label6.Text = "L:";
            // 
            // numHue
            // 
            this.numHue.Location = new System.Drawing.Point(408, 190);
            this.numHue.Maximum = new decimal(new int[] {
            359,
            0,
            0,
            0});
            this.numHue.Name = "numHue";
            this.numHue.Size = new System.Drawing.Size(48, 20);
            this.numHue.TabIndex = 6;
            this.numHue.Value = new decimal(new int[] {
            359,
            0,
            0,
            0});
            this.numHue.ValueChanged += new System.EventHandler(this.numHue_ValueChanged);
            // 
            // numSat
            // 
            this.numSat.DecimalPlaces = 1;
            this.numSat.Location = new System.Drawing.Point(408, 216);
            this.numSat.Name = "numSat";
            this.numSat.Size = new System.Drawing.Size(48, 20);
            this.numSat.TabIndex = 6;
            this.numSat.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.numSat.ValueChanged += new System.EventHandler(this.numSat_ValueChanged);
            // 
            // numLum
            // 
            this.numLum.DecimalPlaces = 1;
            this.numLum.Location = new System.Drawing.Point(408, 242);
            this.numLum.Name = "numLum";
            this.numLum.Size = new System.Drawing.Size(48, 20);
            this.numLum.TabIndex = 6;
            this.numLum.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.numLum.ValueChanged += new System.EventHandler(this.numLum_ValueChanged);
            // 
            // luminanceView
            // 
            this.luminanceView.ColorMode = Fireball.Windows.Forms.ColorChooseControl.ColorSliderMode.Hue;
            this.luminanceView.Location = new System.Drawing.Point(1, 1);
            this.luminanceView.Name = "luminanceView";
            this.luminanceView.Size = new System.Drawing.Size(20, 256);
            this.luminanceView.TabIndex = 1;
            this.luminanceView.Value = 0F;
            // 
            // saturationView
            // 
            this.saturationView.ColorMode = Fireball.Windows.Forms.ColorChooseControl.ColorSliderMode.Hue;
            this.saturationView.Location = new System.Drawing.Point(1, 1);
            this.saturationView.Name = "saturationView";
            this.saturationView.Size = new System.Drawing.Size(20, 256);
            this.saturationView.TabIndex = 1;
            this.saturationView.Value = 0F;
            // 
            // lsView
            // 
            this.lsView.Hue = 360F;
            this.lsView.Location = new System.Drawing.Point(1, 1);
            this.lsView.Luminance = 16.13725F;
            this.lsView.Name = "lsView";
            this.lsView.Saturation = 1.019608F;
            this.lsView.Size = new System.Drawing.Size(256, 256);
            this.lsView.TabIndex = 0;
            // 
            // hueView
            // 
            this.hueView.ColorMode = Fireball.Windows.Forms.ColorChooseControl.ColorSliderMode.Hue;
            this.hueView.Location = new System.Drawing.Point(1, 1);
            this.hueView.Name = "hueView";
            this.hueView.Size = new System.Drawing.Size(20, 256);
            this.hueView.TabIndex = 1;
            this.hueView.Value = 0F;
            // 
            // ColorChooseControl
            // 
            this.Controls.Add(this.panel4);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.panelColors);
            this.Controls.Add(this.numLum);
            this.Controls.Add(this.numSat);
            this.Controls.Add(this.numB);
            this.Controls.Add(this.numHue);
            this.Controls.Add(this.numG);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.numR);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Name = "ColorChooseControl";
            this.Size = new System.Drawing.Size(491, 300);
            ((System.ComponentModel.ISupportInitialize)(this.numR)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numG)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numB)).EndInit();
            this.panelColors.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.panel4.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.numHue)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numSat)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numLum)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        public ColorChooseControl()
        {
            InitializeComponent();
            _currentColor = new ColorHLS(255,255, 0, 0);

            lsView.BaseColor = _currentColor;
            hueView.BaseColor = _currentColor;
            saturationView.BaseColor = _currentColor;
            luminanceView.BaseColor = _currentColor;

            hueView.ColorMode = ColorSliderMode.Hue;
            saturationView.ColorMode = ColorSliderMode.Saturation;
            luminanceView.ColorMode = ColorSliderMode.Brightness;

            hueView.Value = 0;
            saturationView.Value = 1;
            luminanceView.Value = 1;

            lsView.ColorChanged += new ColorChangedDelegate(lsView_ColorChanged);
            hueView.ColorChanged += new ColorChangedDelegate(hueView_ColorChanged);
            saturationView.ColorChanged += new ColorChangedDelegate(saturationView_ColorChanged);
            luminanceView.ColorChanged += new ColorChangedDelegate(luminanceView_ColorChanged);

            lsView.Invalidate();

        }

        void luminanceView_ColorChanged(object sender, ColorChooseControl.ColorChangedEventArgs e)
        {
            numLum.Value = new decimal(luminanceView.Value * 100);
        }
        void saturationView_ColorChanged(object sender, ColorChooseControl.ColorChangedEventArgs e)
        {
            numSat.Value = new decimal(saturationView.Value * 100);
        }
        void hueView_ColorChanged(object sender, ColorChooseControl.ColorChangedEventArgs e)
        {
            numHue.Value = new decimal(hueView.Value);
            luminanceView.BaseColor = e.NewColor;
        }
        void lsView_ColorChanged(object sender, ColorChooseControl.ColorChangedEventArgs e)
        {
            _currentColor = lsView.CurrentColor;

            numR.Value = _currentColor.Red;
            numG.Value = _currentColor.Green;
            numB.Value = _currentColor.Blue;

            numHue.Value = new decimal(lsView.Hue);
            numSat.Value = new decimal(lsView.Saturation * 100);
            numLum.Value = new decimal(lsView.Luminance * 100);
        }
        private void numR_ValueChanged(object sender, EventArgs e)
        {
            if (!lsView.IsFocused)
            {
                ColorHLS newColor = lsView.CurrentColor.Clone();
                newColor.Red = (byte)numR.Value;

                hueView.Value = newColor.Hue;
                lsView.SetHue(newColor.Hue);
            }
        }
        private void numG_ValueChanged(object sender, EventArgs e)
        {
            if (!lsView.IsFocused)
            {
                ColorHLS newColor = lsView.CurrentColor.Clone();
                newColor.Green = (byte)numG.Value;

                hueView.Value = newColor.Hue;
                lsView.SetHue(newColor.Hue);
            }
        }
        private void numB_ValueChanged(object sender, EventArgs e)
        {
            if (!lsView.IsFocused)
            {
                ColorHLS newColor = lsView.CurrentColor.Clone();
                newColor.Blue = (byte)numR.Value;

                hueView.Value = newColor.Hue;
                lsView.SetHue(newColor.Hue);
            }
        }

        private void numHue_ValueChanged(object sender, EventArgs e)
        {
            lsView.SetHue((float)numHue.Value);
            hueView.Value = (float)numHue.Value;

            ColorHLS baseColor = new ColorHLS((float)numHue.Value, 0.5f, 1);
            saturationView.BaseColor = baseColor;
            luminanceView.BaseColor = baseColor;

            this.panelColor2.BackColor = lsView.CurrentColor.Color;
        }
        private void numSat_ValueChanged(object sender, EventArgs e)
        {
            lsView.SetSaturation((float)(numSat.Value / 100));
            saturationView.Value = (float)(numSat.Value / 100);

            this.panelColor2.BackColor = lsView.CurrentColor.Color;
        }
        private void numLum_ValueChanged(object sender, EventArgs e)
        {
            lsView.SetLuminance((float)(numLum.Value / 100));
            luminanceView.Value = (float)(numLum.Value / 100);

            this.panelColor2.BackColor = lsView.CurrentColor.Color;
        }


        public ColorHLS CurrentColor
        {
            get
            {
                return _currentColor;
            }
            set
            {
                _currentColor = value;
                lsView.SetHue(value.Hue);
                lsView.SetSaturation(value.Saturation);
                lsView.SetLuminance(value.Luminance);
                hueView.BaseColor = value;
            }
        }

        #region Nested Types

        private delegate void ColorChangedDelegate(object sender, ColorChangedEventArgs e);

        private enum ColorSliderMode
        {
            Hue=0,Saturation=1,Brightness=2
        }

        private class ColorChangedEventArgs
        {
            public ColorHLS OldColor;
            public ColorHLS NewColor;

            public ColorChangedEventArgs(ColorHLS oldColor, ColorHLS newColor)
            {
                OldColor = oldColor;
                NewColor = newColor;
            }
        }
        private class ColorPicker : Control
        {
            private ColorHLS _baseColor;
            private ColorHLS _currentColor;
            private Rectangle _cursorRect;
            private bool _onDrag;
            private ColorHLS _previusColor;

            private PixelBuffer _pixBuffer;

            internal event ColorChangedDelegate ColorChanged;

            public ColorPicker()
            {
                _baseColor = new ColorHLS(255,255, 0, 0);
                _currentColor = _baseColor.Clone();
                _previusColor = _baseColor.Clone();
                this.SetStyle(
                    ControlStyles.AllPaintingInWmPaint |
                    ControlStyles.FixedHeight |
                    ControlStyles.FixedWidth |
                    ControlStyles.OptimizedDoubleBuffer |
                    ControlStyles.UserPaint,
                    true);
                this.Size = new System.Drawing.Size(256, 256);

                _cursorRect = new Rectangle(252, -3, 9, 9);
                _onDrag = false;

                _pixBuffer = new PixelBuffer(256, 256);
                FillBuffer();

                

                MouseEvent(new MouseEventArgs(MouseButtons.Left, 1, _cursorRect.X + 4, _cursorRect.Y + 4, 0));

            }

            public bool IsFocused
            {
                get
                {
                    return _onDrag;
                }
            }

            private void FillBuffer()
            {
                ColorHLS startColor=new ColorHLS(255, 255, 255, 255);
                ColorHLS endColor = _baseColor.Clone();
                float[,] colorsStepStart = ColorUtils.GetGradientColorSteps(startColor, new ColorHLS(255,0,0,0), 256);
                float[,] colorsStepEnd = ColorUtils.GetGradientColorSteps(endColor, new ColorHLS(255, 0, 0, 0), 256);
                
                for (int i = 0; i < 256; i++)
                {
                    startColor.SetRGB((byte)colorsStepStart[i, 0], (byte)colorsStepStart[i, 1], (byte)colorsStepStart[i, 2]);
                    endColor.SetRGB((byte)colorsStepEnd[i,0],(byte)colorsStepEnd[i,1],(byte)colorsStepEnd[i,2]);
                    _pixBuffer.DrawLine(0, i, 256, i, startColor, endColor);
                }

            }

            protected override void OnSizeChanged(EventArgs e)
            {
                this.Size = new System.Drawing.Size(256, 256);
            }
            protected override void OnPaint(PaintEventArgs e)
            {
                base.OnPaint(e);

                DrawBackground(e);

                DrawCursor(e);
            }
            protected override void OnMouseDown(MouseEventArgs e)
            {
                _previusColor = _currentColor.Clone();
                MouseEvent(e);
                _onDrag = true;
            }
            protected override void OnMouseMove(MouseEventArgs e)
            {
                if (_onDrag)
                {
                    MouseEvent(e);
                }
            }
            protected override void OnMouseUp(MouseEventArgs e)
            {
                _onDrag = false;
                MouseEvent(e);
            }

            public ColorHLS BaseColor
            {
                get
                {
                    return _baseColor;
                }
                set
                {
                    ColorHLS oldColor = _baseColor.Clone();
                    _baseColor = value;
                    FillBuffer();
                    this.Invalidate();

                    _currentColor = this.GetColorAt(
                                    _cursorRect.Left + 4,
                                    _cursorRect.Top + 4
                                    );

                    if (this.ColorChanged != null)
                        this.ColorChanged(
                            this,
                            new ColorChangedEventArgs(
                                oldColor,
                                _currentColor
                                )
                        );
                }
            }
            public ColorHLS CurrentColor
            {
                get
                {
                    return _currentColor;
                }
            }
            public float Hue
            {
                get
                {
                    return _baseColor.Hue;
                }
                set
                {
                    ColorHLS oldColor = _currentColor.Clone();

                    _baseColor = new ColorHLS(value, 0.5f, 1);

                    FillBuffer();

                    _currentColor = this.GetColorAt(
                                    _cursorRect.Left + 4,
                                    _cursorRect.Top + 4
                                    );

                    this.Invalidate();

                    if (this.ColorChanged != null)
                        this.ColorChanged(
                            this,
                            new ColorChangedEventArgs(
                                oldColor,
                                _currentColor
                                )
                        );
                }
            }
            public float Saturation
            {
                get
                {
                    float x = _cursorRect.X + 4;
                    return (x / 255);
                }
                set
                {
                    ColorHLS oldColor = _currentColor.Clone();

                    _cursorRect.X = (int)(255 * value)-4;

                    this.Invalidate();

                    _currentColor = this.GetColorAt(
                                    _cursorRect.Left + 4,
                                    _cursorRect.Top + 4
                                    );

                    if (this.ColorChanged != null)
                        this.ColorChanged(
                            this,
                            new ColorChangedEventArgs(
                                oldColor,
                                _currentColor
                                )
                        );
                }
            }
            public float Luminance
            {
                get
                {
                    float y = _cursorRect.Y + 4;
                    return 1 - (  y / 255);
                    //return _currentColor.Luminance;
                }
                set
                {
                    ColorHLS oldColor = _currentColor.Clone();

                    _cursorRect.Y = 255 - (int)(255 * value) - 4;

                    this.Invalidate();

                    _currentColor = this.GetColorAt(
                                    _cursorRect.Left + 4,
                                    _cursorRect.Top + 4
                                    );

                    if (this.ColorChanged != null)
                        this.ColorChanged(
                            this,
                            new ColorChangedEventArgs(
                                oldColor,
                                _currentColor
                                )
                        );
                }
            }
            public void SetHue(float hue)
            {
                _baseColor = new ColorHLS(hue, 0.5f, 1);
                FillBuffer();

                this.Invalidate();

                _currentColor = this.GetColorAt(
                                    _cursorRect.Left + 4,
                                    _cursorRect.Top + 4
                                    );
            }
            public void SetSaturation(float saturation)
            {
                _cursorRect.X = (int)(255 * saturation) - 4;

                this.Invalidate();

                _currentColor = this.GetColorAt(
                                    _cursorRect.Left + 4,
                                    _cursorRect.Top + 4
                                    );
            }
            public void SetLuminance(float luminance)
            {
                _cursorRect.Y = 255 - (int)(255 * luminance) - 4;

                this.Invalidate();

                _currentColor = this.GetColorAt(
                                    _cursorRect.Left + 4,
                                    _cursorRect.Top + 4
                                    );
            }

            private void MouseEvent(MouseEventArgs e)
            {
                Point newPos = new Point(e.X - 4, e.Y - 4);
                if (newPos.X < -4) newPos.X = -4;
                if (newPos.Y < -4) newPos.Y = -4;
                if (newPos.X > (256 - 5)) newPos.X = (256 - 5);
                if (newPos.Y > (256 - 5)) newPos.Y = (256 - 5);
                _cursorRect.Location = newPos;

                _currentColor = GetColorAt(newPos.X + 4, newPos.Y + 4);

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
                SmoothingMode smooth = e.Graphics.SmoothingMode;
                e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
                Pen cursorPen = new Pen(_currentColor.Complementary);
                cursorPen.Width = 2f;
                e.Graphics.DrawEllipse(cursorPen, _cursorRect);
                e.Graphics.SmoothingMode = smooth;
            }
            private ColorHLS GetColorAt(int x,int y)
            {
                ColorHLS color;
                _pixBuffer.GetPixel(x, y, out color);
                return color;
            }

            public void UpdateView()
            {

            }
        }
        private class ColorSlider : Control
        {
            private ColorHLS _baseColor;
            private ColorHLS _currentColor;
            private ColorHLS _previusColor;
            private ColorSliderMode _colorMode;
            private Rectangle _cursorRect;
            private bool _onDrag;

            private float _saturation;
            private float _luminance;
            private float _hue;

            private PixelBuffer _pixBuffer;

            internal event ColorChangedDelegate ColorChanged;

            public ColorSlider()
                :this(ColorSliderMode.Hue)
            {
            }
            public ColorSlider(ColorSliderMode colorMode)
            {
                _colorMode = colorMode;

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
                this.Size = new System.Drawing.Size(20, 256);

                _cursorRect = new Rectangle(0, 256-4, 20, 7);
                _onDrag = false;

                _pixBuffer = new PixelBuffer(20, 256);
                FillBuffer();

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
                    FillBuffer();
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
                        case ColorSliderMode.Brightness:
                            return _luminance;
                    }
                }
                set
                {
                    SetValue(value);
                }
            }

            private void SetValue(float value)
            {
                float y = -3;
                switch (_colorMode)
                {
                    default:
                    case ColorSliderMode.Hue:
                        value %= 360;
                        y = 255- (((value / 359) * 255)+3);
                        _hue = value;
                        break;
                    case ColorSliderMode.Saturation:
                        if (value < 0 || value > 1)
                        {
                            throw new ArgumentOutOfRangeException("value");
                        }
                        y = (((1 - value) * 255))-3;
                        _saturation = value;
                        break;
                    case ColorSliderMode.Brightness:
                        if (value < 0 || value > 1)
                        {
                            throw new ArgumentOutOfRangeException("value");
                        }
                        y = ((1 - value) * 255)-3;
                        _luminance = value;
                        break;
                }
                _cursorRect.Y = (int)y;
                _currentColor = GetColorAt(1, _cursorRect.Y + 3);
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
                this.Size = new System.Drawing.Size(20, 256);
            }
            protected override void OnPaint(PaintEventArgs e)
            {
                base.OnPaint(e);

                DrawBackground(e);
                DrawCursor(e);
            }

            private void FillBuffer()
            {
                ColorHLS[] colors=new ColorHLS[0];

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
                    case ColorSliderMode.Brightness:
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

                //ColorHLS[] colors = ColorUtils.CreateGradientColorArray(new ColorHLS(255,255, 0, 0), new ColorHLS(255,255, 255, 0), 256);
                for (int y = 0; y < this.Height; y++)
                {
                    _pixBuffer.DrawLine(0, this.Height-1-y, this.Width, this.Height-1-y, colors[y]);
                }
            }
            private void MouseEvent(MouseEventArgs e)
            {
                Point newPos = new Point(0, e.Y - 3);
                if (newPos.Y < -3) newPos.Y = -3;
                if (newPos.Y > (256 - 4)) newPos.Y = (256 - 4);
                _cursorRect.Location = newPos;

                _currentColor = GetColorAt(1, newPos.Y + 3);

                switch (_colorMode)
                {
                    case ColorSliderMode.Saturation:
                        _saturation =  1-((float)(newPos.Y + 3) / 255);
                        break;
                    case ColorSliderMode.Brightness:
                        _luminance = 1-((float)(newPos.Y + 3) / 255); //_currentColor.Luminance * 2;
                        break;
                    case ColorSliderMode.Hue:
                        _hue = (float)Math.Round(((double)(255 - (newPos.Y + 3)) / 255)*359,MidpointRounding.ToEven); //_currentColor.Hue;
                        break;
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

                e.Graphics.DrawLine(cursorPen, _cursorRect.X,_cursorRect.Y,_cursorRect.Right,_cursorRect.Y);
                e.Graphics.DrawLine(cursorPen, _cursorRect.X, _cursorRect.Y+1,_cursorRect.Right,_cursorRect.Y+1);
                e.Graphics.DrawLine(cursorPen, _cursorRect.X, _cursorRect.Y + 2, _cursorRect.Right, _cursorRect.Y + 2);

                e.Graphics.DrawLine(cursorPen, _cursorRect.X, _cursorRect.Y+4, _cursorRect.Right, _cursorRect.Y+4);
                e.Graphics.DrawLine(cursorPen, _cursorRect.X, _cursorRect.Y + 5, _cursorRect.Right, _cursorRect.Y +5);
                e.Graphics.DrawLine(cursorPen, _cursorRect.X, _cursorRect.Y + 6, _cursorRect.Right, _cursorRect.Y + 6);

            }
            private ColorHLS GetColorAt(int x, int y)
            {
                ColorHLS color;
                _pixBuffer.GetPixel(x, y, out color);
                return color;
            }
        }

        #endregion

        

        
    }
}
