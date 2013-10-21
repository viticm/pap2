namespace AIDiagramPlugin
{
    partial class TParmEditForm
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
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.txtEname = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.txtName = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX3 = new DevComponents.DotNetBar.LabelX();
            this.cmbVarType = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.cmbDefaultValue = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.labelX4 = new DevComponents.DotNetBar.LabelX();
            this.txtDefaultValue = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.labelX5 = new DevComponents.DotNetBar.LabelX();
            this.labelX6 = new DevComponents.DotNetBar.LabelX();
            this.SuspendLayout();
            // 
            // labelX2
            // 
            this.labelX2.AutoSize = true;
            this.labelX2.Location = new System.Drawing.Point(43, 89);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(93, 18);
            this.labelX2.TabIndex = 3;
            this.labelX2.Text = "变量名称(&E)：";
            // 
            // labelX1
            // 
            this.labelX1.AutoSize = true;
            this.labelX1.Location = new System.Drawing.Point(43, 59);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(93, 18);
            this.labelX1.TabIndex = 5;
            this.labelX1.Text = "显示名称(&N)：";
            // 
            // txtEname
            // 
            this.txtEname.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            // 
            // 
            // 
            this.txtEname.Border.Class = "TextBoxBorder";
            this.txtEname.Location = new System.Drawing.Point(136, 86);
            this.txtEname.Name = "txtEname";
            this.txtEname.Size = new System.Drawing.Size(363, 21);
            this.txtEname.TabIndex = 2;
            // 
            // txtName
            // 
            this.txtName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            // 
            // 
            // 
            this.txtName.Border.Class = "TextBoxBorder";
            this.txtName.Location = new System.Drawing.Point(136, 59);
            this.txtName.Name = "txtName";
            this.txtName.Size = new System.Drawing.Size(363, 21);
            this.txtName.TabIndex = 1;
            // 
            // labelX3
            // 
            this.labelX3.AutoSize = true;
            this.labelX3.Location = new System.Drawing.Point(43, 117);
            this.labelX3.Name = "labelX3";
            this.labelX3.Size = new System.Drawing.Size(93, 18);
            this.labelX3.TabIndex = 3;
            this.labelX3.Text = "变量类型(&T)：";
            // 
            // cmbVarType
            // 
            this.cmbVarType.DisplayMember = "Text";
            this.cmbVarType.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.cmbVarType.FormattingEnabled = true;
            this.cmbVarType.ItemHeight = 15;
            this.cmbVarType.Location = new System.Drawing.Point(136, 114);
            this.cmbVarType.Name = "cmbVarType";
            this.cmbVarType.Size = new System.Drawing.Size(251, 21);
            this.cmbVarType.TabIndex = 3;
            this.cmbVarType.SelectedIndexChanged += new System.EventHandler(this.cmbVarType_SelectedIndexChanged);
            // 
            // cmbDefaultValue
            // 
            this.cmbDefaultValue.DisplayMember = "Text";
            this.cmbDefaultValue.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.cmbDefaultValue.FormattingEnabled = true;
            this.cmbDefaultValue.ItemHeight = 15;
            this.cmbDefaultValue.Location = new System.Drawing.Point(136, 141);
            this.cmbDefaultValue.Name = "cmbDefaultValue";
            this.cmbDefaultValue.Size = new System.Drawing.Size(251, 21);
            this.cmbDefaultValue.TabIndex = 3;
            this.cmbDefaultValue.SelectedIndexChanged += new System.EventHandler(this.cmbDefaultValue_SelectedIndexChanged);
            // 
            // labelX4
            // 
            this.labelX4.AutoSize = true;
            this.labelX4.Location = new System.Drawing.Point(62, 144);
            this.labelX4.Name = "labelX4";
            this.labelX4.Size = new System.Drawing.Size(74, 18);
            this.labelX4.TabIndex = 3;
            this.labelX4.Text = "默认值&D)：";
            // 
            // txtDefaultValue
            // 
            this.txtDefaultValue.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            // 
            // 
            // 
            this.txtDefaultValue.Border.Class = "TextBoxBorder";
            this.txtDefaultValue.Location = new System.Drawing.Point(136, 141);
            this.txtDefaultValue.Name = "txtDefaultValue";
            this.txtDefaultValue.Size = new System.Drawing.Size(363, 21);
            this.txtDefaultValue.TabIndex = 4;
            this.txtDefaultValue.Visible = false;
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX2.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonX2.Location = new System.Drawing.Point(461, 309);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(75, 23);
            this.buttonX2.TabIndex = 6;
            this.buttonX2.Text = "取消(&C)";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX1.Location = new System.Drawing.Point(380, 309);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(75, 23);
            this.buttonX1.TabIndex = 5;
            this.buttonX1.Text = "确定(&O)";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // labelX5
            // 
            this.labelX5.Location = new System.Drawing.Point(12, 6);
            this.labelX5.Name = "labelX5";
            this.labelX5.Size = new System.Drawing.Size(500, 28);
            this.labelX5.TabIndex = 23;
            this.labelX5.Text = "<b>可以通过以下界面添加/修改AI图模板参数。然后在Action中可以选择到这里设定过的变量。\r\n</b> \r\n";
            // 
            // labelX6
            // 
            this.labelX6.Location = new System.Drawing.Point(43, 198);
            this.labelX6.Name = "labelX6";
            this.labelX6.Size = new System.Drawing.Size(196, 76);
            this.labelX6.TabIndex = 23;
            this.labelX6.Text = "例如：<br/>\r\n<i>显示名称：搜索范围<br/>\r\n变量名称：SEARCH_RANGE<br/>\r\n变量类系：数字<br/>\r\n默认值：100</i>\r\n";
            // 
            // TParmEditForm
            // 
            this.AcceptButton = this.buttonX1;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonX2;
            this.ClientSize = new System.Drawing.Size(556, 344);
            this.Controls.Add(this.labelX6);
            this.Controls.Add(this.labelX5);
            this.Controls.Add(this.buttonX2);
            this.Controls.Add(this.buttonX1);
            this.Controls.Add(this.cmbDefaultValue);
            this.Controls.Add(this.cmbVarType);
            this.Controls.Add(this.labelX4);
            this.Controls.Add(this.labelX3);
            this.Controls.Add(this.labelX2);
            this.Controls.Add(this.labelX1);
            this.Controls.Add(this.txtDefaultValue);
            this.Controls.Add(this.txtEname);
            this.Controls.Add(this.txtName);
            this.Name = "TParmEditForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "模板参数编辑";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private DevComponents.DotNetBar.LabelX labelX2;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.Controls.TextBoxX txtEname;
        private DevComponents.DotNetBar.Controls.TextBoxX txtName;
        private DevComponents.DotNetBar.LabelX labelX3;
        private DevComponents.DotNetBar.Controls.ComboBoxEx cmbVarType;
        private DevComponents.DotNetBar.Controls.ComboBoxEx cmbDefaultValue;
        private DevComponents.DotNetBar.LabelX labelX4;
        private DevComponents.DotNetBar.Controls.TextBoxX txtDefaultValue;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.LabelX labelX5;
        private DevComponents.DotNetBar.LabelX labelX6;
    }
}