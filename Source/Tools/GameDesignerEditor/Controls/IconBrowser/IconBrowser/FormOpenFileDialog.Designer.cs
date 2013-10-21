namespace CustomControls
{
    partial class FormOpenFileDialog
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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.pbxPreview = new System.Windows.Forms.PictureBox();
            this.lblColors = new System.Windows.Forms.Label();
            this.lblFormat = new System.Windows.Forms.Label();
            this.lblSize = new System.Windows.Forms.Label();
            this.lblSizeValue = new System.Windows.Forms.Label();
            this.lblIDValue = new System.Windows.Forms.Label();
            this.lblRect = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbxPreview)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.pbxPreview);
            this.groupBox1.Location = new System.Drawing.Point(5, 30);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(345, 299);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "‘§¿¿";
            // 
            // pbxPreview
            // 
            this.pbxPreview.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pbxPreview.Location = new System.Drawing.Point(3, 17);
            this.pbxPreview.Name = "pbxPreview";
            this.pbxPreview.Size = new System.Drawing.Size(339, 279);
            this.pbxPreview.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pbxPreview.TabIndex = 4;
            this.pbxPreview.TabStop = false;
            this.pbxPreview.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pbxPreview_MouseMove);
            this.pbxPreview.Paint += new System.Windows.Forms.PaintEventHandler(this.pbxPreview_Paint);
            this.pbxPreview.MouseClick += new System.Windows.Forms.MouseEventHandler(this.pbxPreview_MouseClick);
            // 
            // lblColors
            // 
            this.lblColors.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lblColors.Location = new System.Drawing.Point(2, 357);
            this.lblColors.Name = "lblColors";
            this.lblColors.Size = new System.Drawing.Size(42, 13);
            this.lblColors.TabIndex = 3;
            this.lblColors.Text = "≥ﬂ¥Á:";
            // 
            // lblFormat
            // 
            this.lblFormat.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lblFormat.Location = new System.Drawing.Point(2, 339);
            this.lblFormat.Name = "lblFormat";
            this.lblFormat.Size = new System.Drawing.Size(42, 13);
            this.lblFormat.TabIndex = 4;
            this.lblFormat.Text = "ID∫≈:";
            // 
            // lblSize
            // 
            this.lblSize.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lblSize.Location = new System.Drawing.Point(2, 375);
            this.lblSize.Name = "lblSize";
            this.lblSize.Size = new System.Drawing.Size(42, 13);
            this.lblSize.TabIndex = 5;
            this.lblSize.Text = "¥Û–°:";
            // 
            // lblSizeValue
            // 
            this.lblSizeValue.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lblSizeValue.Location = new System.Drawing.Point(50, 375);
            this.lblSizeValue.Name = "lblSizeValue";
            this.lblSizeValue.Size = new System.Drawing.Size(297, 13);
            this.lblSizeValue.TabIndex = 8;
            // 
            // lblIDValue
            // 
            this.lblIDValue.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lblIDValue.Location = new System.Drawing.Point(50, 339);
            this.lblIDValue.Name = "lblIDValue";
            this.lblIDValue.Size = new System.Drawing.Size(297, 13);
            this.lblIDValue.TabIndex = 7;
            // 
            // lblRect
            // 
            this.lblRect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lblRect.Location = new System.Drawing.Point(50, 357);
            this.lblRect.Name = "lblRect";
            this.lblRect.Size = new System.Drawing.Size(297, 13);
            this.lblRect.TabIndex = 6;
            // 
            // FormOpenFileDialog
            // 
            this.Controls.Add(this.lblSizeValue);
            this.Controls.Add(this.lblIDValue);
            this.Controls.Add(this.lblRect);
            this.Controls.Add(this.lblSize);
            this.Controls.Add(this.lblFormat);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.lblColors);
            this.Name = "FormOpenFileDialog";
            this.Size = new System.Drawing.Size(356, 397);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.pbxPreview_Paint);
            this.groupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pbxPreview)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label lblColors;
        private System.Windows.Forms.PictureBox pbxPreview;
        private System.Windows.Forms.Label lblFormat;
        private System.Windows.Forms.Label lblSize;
        private System.Windows.Forms.Label lblSizeValue;
        private System.Windows.Forms.Label lblIDValue;
        private System.Windows.Forms.Label lblRect;
    }
}