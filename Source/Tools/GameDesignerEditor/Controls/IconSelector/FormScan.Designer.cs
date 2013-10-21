namespace IconSelector
{
    partial class FormScan
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormScan));
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.probar1 = new DevComponents.DotNetBar.Controls.ProgressBarX();
            this.txt = new System.Windows.Forms.RichTextBox();
            this.picbox = new System.Windows.Forms.PictureBox();
            this.panelEx1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picbox)).BeginInit();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.Controls.Add(this.label1);
            this.panelEx1.Controls.Add(this.label2);
            this.panelEx1.Controls.Add(this.probar1);
            this.panelEx1.Controls.Add(this.txt);
            this.panelEx1.Controls.Add(this.picbox);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(312, 122);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 1;
            this.panelEx1.UseWaitCursor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.ForeColor = System.Drawing.SystemColors.ControlLightLight;
            this.label1.Location = new System.Drawing.Point(45, 78);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(59, 12);
            this.label1.TabIndex = 5;
            this.label1.Text = "456555   ";
            this.label1.UseWaitCursor = true;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.BackColor = System.Drawing.Color.Transparent;
            this.label2.ForeColor = System.Drawing.SystemColors.ControlLightLight;
            this.label2.Location = new System.Drawing.Point(45, 25);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(95, 12);
            this.label2.TabIndex = 5;
            this.label2.Text = "正在读取图标:  ";
            this.label2.UseWaitCursor = true;
            // 
            // probar1
            // 
            this.probar1.Location = new System.Drawing.Point(47, 49);
            this.probar1.Name = "probar1";
            this.probar1.Size = new System.Drawing.Size(228, 16);
            this.probar1.TabIndex = 4;
            this.probar1.Text = "aaaa";
            this.probar1.UseWaitCursor = true;
            // 
            // txt
            // 
            this.txt.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txt.Location = new System.Drawing.Point(244, 40);
            this.txt.Name = "txt";
            this.txt.Size = new System.Drawing.Size(56, 50);
            this.txt.TabIndex = 3;
            this.txt.Text = "";
            this.txt.UseWaitCursor = true;
            this.txt.Visible = false;
            // 
            // picbox
            // 
            this.picbox.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.picbox.Location = new System.Drawing.Point(12, 60);
            this.picbox.Name = "picbox";
            this.picbox.Size = new System.Drawing.Size(48, 48);
            this.picbox.TabIndex = 1;
            this.picbox.TabStop = false;
            this.picbox.UseWaitCursor = true;
            // 
            // FormScan
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(312, 122);
            this.ControlBox = false;
            this.Controls.Add(this.panelEx1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormScan";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "图标扫描";
            this.TopMost = true;
            this.UseWaitCursor = true;
            this.Shown += new System.EventHandler(this.FormScan_Shown);
            this.Activated += new System.EventHandler(this.FormScan_Activated);
            this.Load += new System.EventHandler(this.FormScan_Load);
            this.panelEx1.ResumeLayout(false);
            this.panelEx1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picbox)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private System.Windows.Forms.PictureBox picbox;
        public System.Windows.Forms.RichTextBox txt;
        private DevComponents.DotNetBar.Controls.ProgressBarX probar1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;

    }
}