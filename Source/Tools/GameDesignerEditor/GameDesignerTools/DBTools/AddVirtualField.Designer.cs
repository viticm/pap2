namespace GameDesingerTools
{
    partial class AddVirtualField
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
            this.FieldName_comboBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.TableName_comboBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.buttonCancel = new DevComponents.DotNetBar.ButtonX();
            this.label4 = new System.Windows.Forms.Label();
            this.edit_FieldCNName = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.buttonOK = new DevComponents.DotNetBar.ButtonX();
            this.edit_TableName = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.edit_FieldName = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.panelEx1.SuspendLayout();
            this.SuspendLayout();
            // 
            // FieldName_comboBox
            // 
            this.FieldName_comboBox.DisplayMember = "Text";
            this.FieldName_comboBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.FieldName_comboBox.FormattingEnabled = true;
            this.FieldName_comboBox.Location = new System.Drawing.Point(-145, 133);
            this.FieldName_comboBox.Name = "FieldName_comboBox";
            this.FieldName_comboBox.Size = new System.Drawing.Size(194, 22);
            this.FieldName_comboBox.TabIndex = 55;
            this.FieldName_comboBox.SelectedIndexChanged += new System.EventHandler(this.FieldName_comboBox_SelectedIndexChanged);
            // 
            // TableName_comboBox
            // 
            this.TableName_comboBox.DisplayMember = "Text";
            this.TableName_comboBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.TableName_comboBox.FormattingEnabled = true;
            this.TableName_comboBox.Location = new System.Drawing.Point(113, 116);
            this.TableName_comboBox.Name = "TableName_comboBox";
            this.TableName_comboBox.Size = new System.Drawing.Size(196, 22);
            this.TableName_comboBox.TabIndex = 54;
            // 
            // buttonCancel
            // 
            this.buttonCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonCancel.Location = new System.Drawing.Point(234, 144);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(75, 23);
            this.buttonCancel.TabIndex = 53;
            this.buttonCancel.Text = "取消";
            this.buttonCancel.MouseClick += new System.Windows.Forms.MouseEventHandler(this.buttonCancel_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.BackColor = System.Drawing.Color.Transparent;
            this.label4.Location = new System.Drawing.Point(23, 91);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(71, 12);
            this.label4.TabIndex = 50;
            this.label4.Text = "字段中文名:";
            // 
            // edit_FieldCNName
            // 
            // 
            // 
            // 
            this.edit_FieldCNName.Border.Class = "TextBoxBorder";
            this.edit_FieldCNName.Location = new System.Drawing.Point(113, 89);
            this.edit_FieldCNName.Name = "edit_FieldCNName";
            this.edit_FieldCNName.Size = new System.Drawing.Size(196, 21);
            this.edit_FieldCNName.TabIndex = 51;
            // 
            // buttonOK
            // 
            this.buttonOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.buttonOK.Location = new System.Drawing.Point(144, 144);
            this.buttonOK.Name = "buttonOK";
            this.buttonOK.Size = new System.Drawing.Size(75, 23);
            this.buttonOK.TabIndex = 52;
            this.buttonOK.Text = "确定";
            this.buttonOK.MouseClick += new System.Windows.Forms.MouseEventHandler(this.buttonOK_Click);
            // 
            // edit_TableName
            // 
            // 
            // 
            // 
            this.edit_TableName.Border.Class = "TextBoxBorder";
            this.edit_TableName.Location = new System.Drawing.Point(113, 18);
            this.edit_TableName.Name = "edit_TableName";
            this.edit_TableName.Size = new System.Drawing.Size(196, 21);
            this.edit_TableName.TabIndex = 48;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.Location = new System.Drawing.Point(59, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 12);
            this.label1.TabIndex = 16;
            this.label1.Text = "表名:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.BackColor = System.Drawing.Color.Transparent;
            this.label3.Location = new System.Drawing.Point(23, 56);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(71, 12);
            this.label3.TabIndex = 44;
            this.label3.Text = "字段英文名:";
            // 
            // edit_FieldName
            // 
            // 
            // 
            // 
            this.edit_FieldName.Border.Class = "TextBoxBorder";
            this.edit_FieldName.Enabled = false;
            this.edit_FieldName.Location = new System.Drawing.Point(113, 54);
            this.edit_FieldName.Name = "edit_FieldName";
            this.edit_FieldName.Size = new System.Drawing.Size(196, 21);
            this.edit_FieldName.TabIndex = 45;
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorScheme.ItemDesignTimeBorder = System.Drawing.Color.Black;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx1.Controls.Add(this.label1);
            this.panelEx1.Controls.Add(this.label3);
            this.panelEx1.Controls.Add(this.label4);
            this.panelEx1.Controls.Add(this.edit_TableName);
            this.panelEx1.Controls.Add(this.edit_FieldName);
            this.panelEx1.Controls.Add(this.FieldName_comboBox);
            this.panelEx1.Controls.Add(this.edit_FieldCNName);
            this.panelEx1.Controls.Add(this.TableName_comboBox);
            this.panelEx1.Controls.Add(this.buttonOK);
            this.panelEx1.Controls.Add(this.buttonCancel);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(347, 191);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 50;
            // 
            // AddVirtualField
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(347, 191);
            this.Controls.Add(this.panelEx1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "AddVirtualField";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "新增字段";
            this.panelEx1.ResumeLayout(false);
            this.panelEx1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.Controls.TextBoxX edit_TableName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private DevComponents.DotNetBar.Controls.TextBoxX edit_FieldName;
        private System.Windows.Forms.Label label4;
        private DevComponents.DotNetBar.Controls.TextBoxX edit_FieldCNName;
        private DevComponents.DotNetBar.ButtonX buttonCancel;
        private DevComponents.DotNetBar.ButtonX buttonOK;
        private DevComponents.DotNetBar.Controls.ComboBoxEx FieldName_comboBox;
        private DevComponents.DotNetBar.Controls.ComboBoxEx TableName_comboBox;
        private DevComponents.DotNetBar.PanelEx panelEx1;
    }
}