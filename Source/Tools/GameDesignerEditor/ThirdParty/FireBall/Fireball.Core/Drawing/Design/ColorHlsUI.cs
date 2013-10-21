using System;
using System.ComponentModel;
using System.Windows.Forms.Design;
using System.Windows.Forms;

namespace Fireball.Drawing.Design
{
    public class ColorHlsUI : UserControl
    {
        private ColorHLSEditor _editor;
        private IWindowsFormsEditorService _edSvc;
        private Label label1;
        private NumericUpDown numR;
        private Label label2;
        private NumericUpDown numG;
        private Label label3;
        private NumericUpDown numB;
        private Label label4;
        private NumericUpDown numH;
        private Label label5;
        private NumericUpDown numS;
        private Label label6;
        private NumericUpDown numL;
        private Panel panelColor;
        private Panel panel1;
        private ColorSlider colorSliderHue;
        private ColorSlider colorSliderSat;
        private ColorSlider colorSliderLum;
        private ColorHLS _value;

        public ColorHlsUI()
        {
            InitializeComponent();

            colorSliderHue.Value = 360;
            colorSliderSat.Value = 0;
            colorSliderLum.Value = 1;

            _value = new ColorHLS(colorSliderHue.Value, colorSliderLum.Value, colorSliderSat.Value);
            panelColor.BackColor = _value.Color;
        }

        public ColorHlsUI(ColorHLSEditor editor)
            :this()
        {
            _editor = editor;
        }

        public void Start(IWindowsFormsEditorService edSvc, object value)
        {
            this._edSvc = edSvc;
            this._value = (ColorHLS)value;
            if (value != null)
            {

            }
        }

        public object Value
        {
            get
            {
                return this._value;
            }
        }

        public void End()
        {
            this._edSvc = null;
            this._value = null;
        }

        private void Initialize()
        {

        }
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.numR = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.numG = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.numB = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.numH = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.numS = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.numL = new System.Windows.Forms.NumericUpDown();
            this.panelColor = new System.Windows.Forms.Panel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.colorSliderHue = new Fireball.Drawing.Design.ColorSlider();
            this.colorSliderSat = new Fireball.Drawing.Design.ColorSlider();
            this.colorSliderLum = new Fireball.Drawing.Design.ColorSlider();
            ((System.ComponentModel.ISupportInitialize)(this.numR)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numG)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numH)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numS)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numL)).BeginInit();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(96, 5);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(18, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "R:";
            // 
            // numR
            // 
            this.numR.Location = new System.Drawing.Point(120, 3);
            this.numR.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.numR.Name = "numR";
            this.numR.Size = new System.Drawing.Size(46, 20);
            this.numR.TabIndex = 1;
            this.numR.Value = new decimal(new int[] {
            255,
            0,
            0,
            0});
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(96, 31);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(18, 13);
            this.label2.TabIndex = 0;
            this.label2.Text = "G:";
            // 
            // numG
            // 
            this.numG.Location = new System.Drawing.Point(120, 29);
            this.numG.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.numG.Name = "numG";
            this.numG.Size = new System.Drawing.Size(46, 20);
            this.numG.TabIndex = 1;
            this.numG.Value = new decimal(new int[] {
            255,
            0,
            0,
            0});
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(96, 57);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(17, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "B:";
            // 
            // numB
            // 
            this.numB.Location = new System.Drawing.Point(120, 55);
            this.numB.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.numB.Name = "numB";
            this.numB.Size = new System.Drawing.Size(46, 20);
            this.numB.TabIndex = 1;
            this.numB.Value = new decimal(new int[] {
            255,
            0,
            0,
            0});
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(189, 5);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(18, 13);
            this.label4.TabIndex = 0;
            this.label4.Text = "H:";
            // 
            // numH
            // 
            this.numH.Location = new System.Drawing.Point(213, 3);
            this.numH.Maximum = new decimal(new int[] {
            359,
            0,
            0,
            0});
            this.numH.Name = "numH";
            this.numH.Size = new System.Drawing.Size(46, 20);
            this.numH.TabIndex = 1;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(189, 31);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(17, 13);
            this.label5.TabIndex = 0;
            this.label5.Text = "S:";
            // 
            // numS
            // 
            this.numS.Location = new System.Drawing.Point(213, 29);
            this.numS.Name = "numS";
            this.numS.Size = new System.Drawing.Size(46, 20);
            this.numS.TabIndex = 1;
            this.numS.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(189, 57);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(16, 13);
            this.label6.TabIndex = 0;
            this.label6.Text = "L:";
            // 
            // numL
            // 
            this.numL.Location = new System.Drawing.Point(213, 55);
            this.numL.Name = "numL";
            this.numL.Size = new System.Drawing.Size(46, 20);
            this.numL.TabIndex = 1;
            this.numL.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // panelColor
            // 
            this.panelColor.BackColor = System.Drawing.SystemColors.Control;
            this.panelColor.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelColor.Location = new System.Drawing.Point(1, 1);
            this.panelColor.Name = "panelColor";
            this.panelColor.Size = new System.Drawing.Size(69, 69);
            this.panelColor.TabIndex = 2;
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.ControlDark;
            this.panel1.Controls.Add(this.panelColor);
            this.panel1.Location = new System.Drawing.Point(3, 3);
            this.panel1.Name = "panel1";
            this.panel1.Padding = new System.Windows.Forms.Padding(1);
            this.panel1.Size = new System.Drawing.Size(71, 71);
            this.panel1.TabIndex = 3;
            // 
            // colorSlider1
            // 
            this.colorSliderHue.ColorMode = Fireball.Drawing.Design.ColorSliderMode.Hue;
            this.colorSliderHue.Location = new System.Drawing.Point(3, 81);
            this.colorSliderHue.Name = "colorSlider1";
            this.colorSliderHue.Orientation = Fireball.Drawing.Design.ColorSlider.ColorSliderOrientation.Horizontal;
            this.colorSliderHue.Size = new System.Drawing.Size(256, 20);
            this.colorSliderHue.TabIndex = 4;
            this.colorSliderHue.Text = "colorSlider1";
            this.colorSliderHue.Value = 0F;
            // 
            // colorSlider2
            // 
            this.colorSliderSat.ColorMode = Fireball.Drawing.Design.ColorSliderMode.Saturation;
            this.colorSliderSat.Location = new System.Drawing.Point(3, 107);
            this.colorSliderSat.Name = "colorSlider2";
            this.colorSliderSat.Orientation = Fireball.Drawing.Design.ColorSlider.ColorSliderOrientation.Horizontal;
            this.colorSliderSat.Size = new System.Drawing.Size(256, 20);
            this.colorSliderSat.TabIndex = 5;
            this.colorSliderSat.Text = "colorSlider2";
            this.colorSliderSat.Value = 0F;
            // 
            // colorSlider3
            // 
            this.colorSliderLum.ColorMode = Fireball.Drawing.Design.ColorSliderMode.Luminance;
            this.colorSliderLum.Location = new System.Drawing.Point(3, 133);
            this.colorSliderLum.Name = "colorSlider3";
            this.colorSliderLum.Orientation = Fireball.Drawing.Design.ColorSlider.ColorSliderOrientation.Horizontal;
            this.colorSliderLum.Size = new System.Drawing.Size(256, 20);
            this.colorSliderLum.TabIndex = 5;
            this.colorSliderLum.Text = "colorSlider2";
            this.colorSliderLum.Value = 0F;
            // 
            // ColorHlsUI
            // 
            this.Controls.Add(this.colorSliderLum);
            this.Controls.Add(this.colorSliderSat);
            this.Controls.Add(this.colorSliderHue);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.numL);
            this.Controls.Add(this.numB);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.numS);
            this.Controls.Add(this.numG);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.numH);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.numR);
            this.Controls.Add(this.label1);
            this.Name = "ColorHlsUI";
            this.Size = new System.Drawing.Size(264, 158);
            ((System.ComponentModel.ISupportInitialize)(this.numR)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numG)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numH)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numS)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numL)).EndInit();
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

    }
}
