namespace LogicalEditor
{
    partial class Preview
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
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.statusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.statusLabel2 = new System.Windows.Forms.ToolStripStatusLabel();
            this.statusLabel4 = new System.Windows.Forms.ToolStripStatusLabel();
            this.statusLabel3 = new System.Windows.Forms.ToolStripStatusLabel();
            this.bar1 = new DevComponents.DotNetBar.Bar();
            this.checkShowDir = new DevComponents.DotNetBar.CheckBoxItem();
            this.checkShowObjName = new DevComponents.DotNetBar.CheckBoxItem();
            this.panel1 = new LogicalEditor.Canvas();
            this.statusStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).BeginInit();
            this.SuspendLayout();
            // 
            // statusStrip1
            // 
            this.statusStrip1.BackColor = System.Drawing.SystemColors.Control;
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusLabel1,
            this.statusLabel2,
            this.statusLabel4,
            this.statusLabel3});
            this.statusStrip1.Location = new System.Drawing.Point(0, 617);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(703, 22);
            this.statusStrip1.TabIndex = 8;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // statusLabel1
            // 
            this.statusLabel1.Name = "statusLabel1";
            this.statusLabel1.Size = new System.Drawing.Size(59, 17);
            this.statusLabel1.Text = "鼠标: N/A";
            // 
            // statusLabel2
            // 
            this.statusLabel2.Name = "statusLabel2";
            this.statusLabel2.Size = new System.Drawing.Size(59, 17);
            this.statusLabel2.Text = "地图: N/A";
            // 
            // statusLabel4
            // 
            this.statusLabel4.Name = "statusLabel4";
            this.statusLabel4.Size = new System.Drawing.Size(71, 17);
            this.statusLabel4.Text = "比例尺: N/A";
            // 
            // statusLabel3
            // 
            this.statusLabel3.Name = "statusLabel3";
            this.statusLabel3.Size = new System.Drawing.Size(59, 17);
            this.statusLabel3.Text = "对象: N/A";
            // 
            // bar1
            // 
            this.bar1.Dock = System.Windows.Forms.DockStyle.Top;
            this.bar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.checkShowDir,
            this.checkShowObjName});
            this.bar1.Location = new System.Drawing.Point(0, 0);
            this.bar1.Name = "bar1";
            this.bar1.Size = new System.Drawing.Size(703, 25);
            this.bar1.Stretch = true;
            this.bar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.bar1.TabIndex = 9;
            this.bar1.TabStop = false;
            this.bar1.Text = "bar1";
            // 
            // checkShowDir
            // 
            this.checkShowDir.Name = "checkShowDir";
            this.checkShowDir.Text = "显示朝向";
            this.checkShowDir.CheckedChanged += new DevComponents.DotNetBar.CheckBoxChangeEventHandler(this.checkShowDir_CheckedChanged);
            // 
            // checkShowObjName
            // 
            this.checkShowObjName.Name = "checkShowObjName";
            this.checkShowObjName.Text = "显示名字";
            this.checkShowObjName.CheckedChanged += new DevComponents.DotNetBar.CheckBoxChangeEventHandler(this.checkShowObjName_CheckedChanged);
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.Location = new System.Drawing.Point(0, 26);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(703, 588);
            this.panel1.TabIndex = 0;
            this.panel1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.panel1_MouseMove);
            this.panel1.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.panel1_MouseDoubleClick);
            this.panel1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.panel1_MouseDown);
            this.panel1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.panel1_MouseUp);
            this.panel1.SizeChanged += new System.EventHandler(this.panel1_SizeChanged);
            // 
            // Preview
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.ClientSize = new System.Drawing.Size(703, 639);
            this.Controls.Add(this.bar1);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.panel1);
            this.Name = "Preview";
            this.Text = "地图预览";
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Preview_KeyUp);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Preview_FormClosing);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Preview_KeyDown);
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.bar1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private Canvas panel1;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel statusLabel1;
        private System.Windows.Forms.ToolStripStatusLabel statusLabel2;
        private DevComponents.DotNetBar.Bar bar1;
        private DevComponents.DotNetBar.CheckBoxItem checkShowDir;
        private System.Windows.Forms.ToolStripStatusLabel statusLabel3;
        private System.Windows.Forms.ToolStripStatusLabel statusLabel4;
        private DevComponents.DotNetBar.CheckBoxItem checkShowObjName;

    }
}