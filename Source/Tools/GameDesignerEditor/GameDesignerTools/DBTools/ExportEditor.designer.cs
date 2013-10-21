namespace GameDesingerTools
{
    partial class ExportEditor
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
            this.bCancel = new DevComponents.DotNetBar.ButtonX();
            this.bOK = new DevComponents.DotNetBar.ButtonX();
            this.bRemoveTable = new DevComponents.DotNetBar.ButtonX();
            this.bAddTable = new DevComponents.DotNetBar.ButtonX();
            this.exportTableBox = new System.Windows.Forms.ListBox();
            this.tableNameBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.panelEx1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx1.Controls.Add(this.bCancel);
            this.panelEx1.Controls.Add(this.bOK);
            this.panelEx1.Controls.Add(this.bRemoveTable);
            this.panelEx1.Controls.Add(this.bAddTable);
            this.panelEx1.Controls.Add(this.exportTableBox);
            this.panelEx1.Controls.Add(this.tableNameBox);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(443, 327);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            // 
            // bCancel
            // 
            this.bCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bCancel.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bCancel.Location = new System.Drawing.Point(355, 283);
            this.bCancel.Name = "bCancel";
            this.bCancel.Size = new System.Drawing.Size(75, 21);
            this.bCancel.TabIndex = 5;
            this.bCancel.Text = "取消";
            this.bCancel.Click += new System.EventHandler(this.bCancel_Click);
            // 
            // bOK
            // 
            this.bOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bOK.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bOK.Location = new System.Drawing.Point(274, 283);
            this.bOK.Name = "bOK";
            this.bOK.Size = new System.Drawing.Size(75, 21);
            this.bOK.TabIndex = 4;
            this.bOK.Text = "确定";
            this.bOK.Click += new System.EventHandler(this.bOK_Click);
            // 
            // bRemoveTable
            // 
            this.bRemoveTable.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bRemoveTable.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bRemoveTable.Location = new System.Drawing.Point(355, 236);
            this.bRemoveTable.Name = "bRemoveTable";
            this.bRemoveTable.Size = new System.Drawing.Size(75, 21);
            this.bRemoveTable.TabIndex = 3;
            this.bRemoveTable.Text = "删除数据表";
            this.bRemoveTable.Click += new System.EventHandler(this.bRemoveTable_Click);
            // 
            // bAddTable
            // 
            this.bAddTable.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bAddTable.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bAddTable.Location = new System.Drawing.Point(168, 236);
            this.bAddTable.Name = "bAddTable";
            this.bAddTable.Size = new System.Drawing.Size(75, 21);
            this.bAddTable.TabIndex = 2;
            this.bAddTable.Text = "添加数据表";
            this.bAddTable.Click += new System.EventHandler(this.bAddTable_Click);
            // 
            // exportTableBox
            // 
            this.exportTableBox.FormattingEnabled = true;
            this.exportTableBox.ItemHeight = 12;
            this.exportTableBox.Location = new System.Drawing.Point(12, 12);
            this.exportTableBox.Name = "exportTableBox";
            this.exportTableBox.Size = new System.Drawing.Size(418, 208);
            this.exportTableBox.TabIndex = 1;
            // 
            // tableNameBox
            // 
            this.tableNameBox.DisplayMember = "Text";
            this.tableNameBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.tableNameBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.tableNameBox.FormattingEnabled = true;
            this.tableNameBox.ItemHeight = 15;
            this.tableNameBox.Location = new System.Drawing.Point(12, 236);
            this.tableNameBox.Name = "tableNameBox";
            this.tableNameBox.Size = new System.Drawing.Size(150, 21);
            this.tableNameBox.TabIndex = 0;
            // 
            // ExportEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(443, 327);
            this.Controls.Add(this.panelEx1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ExportEditor";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "数据表导出配置";
            this.panelEx1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridViewTextBoxColumn modidDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewComboBoxColumn tablenameDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn scriptDataGridViewTextBoxColumn;
        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.ButtonX bCancel;
        private DevComponents.DotNetBar.ButtonX bOK;
        private DevComponents.DotNetBar.ButtonX bRemoveTable;
        private DevComponents.DotNetBar.ButtonX bAddTable;
        private System.Windows.Forms.ListBox exportTableBox;
        private DevComponents.DotNetBar.Controls.ComboBoxEx tableNameBox;
    }
}