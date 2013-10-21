namespace SkillScriptControl
{
    partial class FrmScriptEdit
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
            this.components = new System.ComponentModel.Container();
            this.dataGv = new System.Windows.Forms.DataGridView();
            this.popMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.popColAdd = new System.Windows.Forms.ToolStripMenuItem();
            this.重命名ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.删除列ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.btnCancel = new DevComponents.DotNetBar.ButtonX();
            this.btnOK = new DevComponents.DotNetBar.ButtonX();
            ((System.ComponentModel.ISupportInitialize)(this.dataGv)).BeginInit();
            this.popMenu.SuspendLayout();
            this.panelEx1.SuspendLayout();
            this.SuspendLayout();
            // 
            // dataGv
            // 
            this.dataGv.AllowUserToResizeRows = false;
            this.dataGv.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.dataGv.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGv.ContextMenuStrip = this.popMenu;
            this.dataGv.Location = new System.Drawing.Point(3, 3);
            this.dataGv.MultiSelect = false;
            this.dataGv.Name = "dataGv";
            this.dataGv.RowTemplate.Height = 23;
            this.dataGv.Size = new System.Drawing.Size(631, 355);
            this.dataGv.TabIndex = 0;
            // 
            // popMenu
            // 
            this.popMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.popColAdd,
            this.重命名ToolStripMenuItem,
            this.删除列ToolStripMenuItem});
            this.popMenu.Name = "popMenu";
            this.popMenu.Size = new System.Drawing.Size(149, 70);
            // 
            // popColAdd
            // 
            this.popColAdd.Name = "popColAdd";
            this.popColAdd.Size = new System.Drawing.Size(148, 22);
            this.popColAdd.Text = "添加列...";
            this.popColAdd.Click += new System.EventHandler(this.popColAdd_Click);
            // 
            // 重命名ToolStripMenuItem
            // 
            this.重命名ToolStripMenuItem.Name = "重命名ToolStripMenuItem";
            this.重命名ToolStripMenuItem.Size = new System.Drawing.Size(148, 22);
            this.重命名ToolStripMenuItem.Text = "重命名列头...";
            this.重命名ToolStripMenuItem.Click += new System.EventHandler(this.Rename_ToolStripMenuItem_Click);
            // 
            // 删除列ToolStripMenuItem
            // 
            this.删除列ToolStripMenuItem.Name = "删除列ToolStripMenuItem";
            this.删除列ToolStripMenuItem.Size = new System.Drawing.Size(148, 22);
            this.删除列ToolStripMenuItem.Text = "删除列";
            this.删除列ToolStripMenuItem.Click += new System.EventHandler(this.Remove_ToolStripMenuItem_Click);
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.Controls.Add(this.btnCancel);
            this.panelEx1.Controls.Add(this.btnOK);
            this.panelEx1.Controls.Add(this.dataGv);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(640, 398);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 1;
            this.panelEx1.Text = "panelEx1";
            // 
            // btnCancel
            // 
            this.btnCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(553, 367);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 2;
            this.btnCancel.Text = "取消(&C)";
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnOK
            // 
            this.btnOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.Location = new System.Drawing.Point(472, 367);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 1;
            this.btnOK.Text = "确定(&O)";
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // FrmScriptEdit
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(640, 398);
            this.Controls.Add(this.panelEx1);
            this.Name = "FrmScriptEdit";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "FrmScriptEdit";
            ((System.ComponentModel.ISupportInitialize)(this.dataGv)).EndInit();
            this.popMenu.ResumeLayout(false);
            this.panelEx1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridView dataGv;
        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.ButtonX btnCancel;
        private DevComponents.DotNetBar.ButtonX btnOK;
        private System.Windows.Forms.ContextMenuStrip popMenu;
        private System.Windows.Forms.ToolStripMenuItem popColAdd;
        private System.Windows.Forms.ToolStripMenuItem 重命名ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 删除列ToolStripMenuItem;
    }
}