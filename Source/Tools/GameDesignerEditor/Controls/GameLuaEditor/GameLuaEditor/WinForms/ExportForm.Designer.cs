namespace GameLuaEditor
{
    partial class ExportForm
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
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.cExportCondenseString = new System.Windows.Forms.CheckBox();
            this.btnCancel = new DevComponents.DotNetBar.ButtonX();
            this.btnExport = new DevComponents.DotNetBar.ButtonX();
            this.ckBakBeforeOverride = new System.Windows.Forms.CheckBox();
            this.ckOverride = new System.Windows.Forms.CheckBox();
            this.btnReverse = new DevComponents.DotNetBar.ButtonX();
            this.btnAll = new DevComponents.DotNetBar.ButtonX();
            this.label1 = new System.Windows.Forms.Label();
            this.listFiles = new System.Windows.Forms.CheckedListBox();
            this.panelEx1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.Controls.Add(this.cExportCondenseString);
            this.panelEx1.Controls.Add(this.btnCancel);
            this.panelEx1.Controls.Add(this.btnExport);
            this.panelEx1.Controls.Add(this.ckBakBeforeOverride);
            this.panelEx1.Controls.Add(this.ckOverride);
            this.panelEx1.Controls.Add(this.btnReverse);
            this.panelEx1.Controls.Add(this.btnAll);
            this.panelEx1.Controls.Add(this.label1);
            this.panelEx1.Controls.Add(this.listFiles);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(560, 342);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            this.panelEx1.Text = "panelEx1";
            // 
            // cExportCondenseString
            // 
            this.cExportCondenseString.AutoSize = true;
            this.cExportCondenseString.Enabled = false;
            this.cExportCondenseString.Location = new System.Drawing.Point(12, 314);
            this.cExportCondenseString.Name = "cExportCondenseString";
            this.cExportCondenseString.Size = new System.Drawing.Size(108, 16);
            this.cExportCondenseString.TabIndex = 8;
            this.cExportCondenseString.Text = "精简字符串导出";
            this.cExportCondenseString.UseVisualStyleBackColor = true;
            // 
            // btnCancel
            // 
            this.btnCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(473, 307);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 7;
            this.btnCancel.Text = "取消(&C)";
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnExport
            // 
            this.btnExport.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnExport.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnExport.Location = new System.Drawing.Point(386, 307);
            this.btnExport.Name = "btnExport";
            this.btnExport.Size = new System.Drawing.Size(75, 23);
            this.btnExport.TabIndex = 6;
            this.btnExport.Text = "导出(&E)";
            this.btnExport.Click += new System.EventHandler(this.btnExport_Click);
            // 
            // ckBakBeforeOverride
            // 
            this.ckBakBeforeOverride.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.ckBakBeforeOverride.AutoSize = true;
            this.ckBakBeforeOverride.Location = new System.Drawing.Point(115, 290);
            this.ckBakBeforeOverride.Name = "ckBakBeforeOverride";
            this.ckBakBeforeOverride.Size = new System.Drawing.Size(126, 16);
            this.ckBakBeforeOverride.TabIndex = 5;
            this.ckBakBeforeOverride.Text = "覆盖前备份(*.bak)";
            this.ckBakBeforeOverride.UseVisualStyleBackColor = true;
            // 
            // ckOverride
            // 
            this.ckOverride.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.ckOverride.AutoSize = true;
            this.ckOverride.Checked = true;
            this.ckOverride.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ckOverride.Location = new System.Drawing.Point(12, 290);
            this.ckOverride.Name = "ckOverride";
            this.ckOverride.Size = new System.Drawing.Size(96, 16);
            this.ckOverride.TabIndex = 4;
            this.ckOverride.Text = "覆盖本地文件";
            this.ckOverride.UseVisualStyleBackColor = true;
            this.ckOverride.CheckedChanged += new System.EventHandler(this.ckOverride_CheckedChanged);
            // 
            // btnReverse
            // 
            this.btnReverse.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnReverse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnReverse.Location = new System.Drawing.Point(490, 9);
            this.btnReverse.Name = "btnReverse";
            this.btnReverse.Size = new System.Drawing.Size(58, 23);
            this.btnReverse.TabIndex = 3;
            this.btnReverse.Text = "反选";
            this.btnReverse.Click += new System.EventHandler(this.btnReverse_Click);
            // 
            // btnAll
            // 
            this.btnAll.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnAll.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnAll.Location = new System.Drawing.Point(428, 9);
            this.btnAll.Name = "btnAll";
            this.btnAll.Size = new System.Drawing.Size(56, 23);
            this.btnAll.TabIndex = 2;
            this.btnAll.Text = "全选";
            this.btnAll.Click += new System.EventHandler(this.btnAll_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(185, 12);
            this.label1.TabIndex = 1;
            this.label1.Text = "将要写入物理驱动器的文件列表：";
            // 
            // listFiles
            // 
            this.listFiles.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listFiles.FormattingEnabled = true;
            this.listFiles.Location = new System.Drawing.Point(12, 36);
            this.listFiles.Name = "listFiles";
            this.listFiles.Size = new System.Drawing.Size(536, 244);
            this.listFiles.TabIndex = 0;
            // 
            // ExportForm
            // 
            this.AcceptButton = this.btnExport;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(560, 342);
            this.Controls.Add(this.panelEx1);
            this.Name = "ExportForm";
            this.Text = "导出到本地磁盘";
            this.panelEx1.ResumeLayout(false);
            this.panelEx1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.ButtonX btnAll;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckedListBox listFiles;
        private DevComponents.DotNetBar.ButtonX btnCancel;
        private DevComponents.DotNetBar.ButtonX btnExport;
        private System.Windows.Forms.CheckBox ckBakBeforeOverride;
        private System.Windows.Forms.CheckBox ckOverride;
        private DevComponents.DotNetBar.ButtonX btnReverse;
        private System.Windows.Forms.CheckBox cExportCondenseString;
    }
}