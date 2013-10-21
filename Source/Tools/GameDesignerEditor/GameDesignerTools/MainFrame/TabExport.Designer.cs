namespace GameDesingerTools
{
    partial class TabExport
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.Export_button = new DevComponents.DotNetBar.ButtonX();
            this.SelectAll_button = new DevComponents.DotNetBar.ButtonX();
            this.checkedListBox1 = new System.Windows.Forms.CheckedListBox();
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.btOpenDictionary = new DevComponents.DotNetBar.ButtonX();
            this.btOpenExportedFile = new DevComponents.DotNetBar.ButtonX();
            this.panelEx1.SuspendLayout();
            this.SuspendLayout();
            // 
            // Export_button
            // 
            this.Export_button.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.Export_button.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.Export_button.Location = new System.Drawing.Point(147, 252);
            this.Export_button.Name = "Export_button";
            this.Export_button.Size = new System.Drawing.Size(114, 26);
            this.Export_button.TabIndex = 2;
            this.Export_button.Text = "导出所选表";
            this.Export_button.Click += new System.EventHandler(this.Export_button_Click);
            // 
            // SelectAll_button
            // 
            this.SelectAll_button.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.SelectAll_button.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.SelectAll_button.Location = new System.Drawing.Point(12, 252);
            this.SelectAll_button.Name = "SelectAll_button";
            this.SelectAll_button.Size = new System.Drawing.Size(114, 26);
            this.SelectAll_button.TabIndex = 1;
            this.SelectAll_button.Text = "全选";
            this.SelectAll_button.Click += new System.EventHandler(this.SelectAll_button_Click);
            // 
            // checkedListBox1
            // 
            this.checkedListBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.checkedListBox1.FormattingEnabled = true;
            this.checkedListBox1.Location = new System.Drawing.Point(0, 0);
            this.checkedListBox1.Name = "checkedListBox1";
            this.checkedListBox1.Size = new System.Drawing.Size(615, 212);
            this.checkedListBox1.TabIndex = 0;
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.Controls.Add(this.checkedListBox1);
            this.panelEx1.Controls.Add(this.btOpenExportedFile);
            this.panelEx1.Controls.Add(this.btOpenDictionary);
            this.panelEx1.Controls.Add(this.SelectAll_button);
            this.panelEx1.Controls.Add(this.Export_button);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(615, 316);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 3;
            // 
            // btOpenDictionary
            // 
            this.btOpenDictionary.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btOpenDictionary.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btOpenDictionary.Location = new System.Drawing.Point(298, 252);
            this.btOpenDictionary.Name = "btOpenDictionary";
            this.btOpenDictionary.Size = new System.Drawing.Size(131, 26);
            this.btOpenDictionary.TabIndex = 1;
            this.btOpenDictionary.Text = "打开导出文件的目录";
            this.btOpenDictionary.Click += new System.EventHandler(this.btOpenDictionary_Click);
            // 
            // btOpenExportedFile
            // 
            this.btOpenExportedFile.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btOpenExportedFile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btOpenExportedFile.Location = new System.Drawing.Point(466, 252);
            this.btOpenExportedFile.Name = "btOpenExportedFile";
            this.btOpenExportedFile.Size = new System.Drawing.Size(124, 26);
            this.btOpenExportedFile.TabIndex = 1;
            this.btOpenExportedFile.Text = "打开所导出的文件";
            this.btOpenExportedFile.Click += new System.EventHandler(this.btOpenExportedFile_Click);
            // 
            // TabExport
            // 
            this.AcceptButton = this.Export_button;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(615, 316);
            this.Controls.Add(this.panelEx1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "TabExport";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "导出数据";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.TabExport_FormClosed);
            this.panelEx1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.ButtonX Export_button;
        private DevComponents.DotNetBar.ButtonX SelectAll_button;
        private System.Windows.Forms.CheckedListBox checkedListBox1;
        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.ButtonX btOpenDictionary;
        private DevComponents.DotNetBar.ButtonX btOpenExportedFile;
    }
}