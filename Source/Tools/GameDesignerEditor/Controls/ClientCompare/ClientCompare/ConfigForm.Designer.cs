namespace ClientCompare
{
    partial class ConfigForm
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
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.textBoxX1 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.buttonX3 = new DevComponents.DotNetBar.ButtonX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.groupPanel1 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.luaCheckBox = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.otherCheckBox = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.logicalCheckBox = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.txtCheckBox = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.iniCheckBox = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.tabCheckBox = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.groupPanel2 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.lsCheckBox = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.lhCheckBox = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.groupPanel1.SuspendLayout();
            this.groupPanel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX1.Location = new System.Drawing.Point(308, 261);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(70, 20);
            this.buttonX1.TabIndex = 0;
            this.buttonX1.Text = "确定";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX2.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonX2.Location = new System.Drawing.Point(394, 261);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(70, 20);
            this.buttonX2.TabIndex = 1;
            this.buttonX2.Text = "取消";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // textBoxX1
            // 
            // 
            // 
            // 
            this.textBoxX1.Border.Class = "TextBoxBorder";
            this.textBoxX1.Location = new System.Drawing.Point(8, 32);
            this.textBoxX1.Name = "textBoxX1";
            this.textBoxX1.Size = new System.Drawing.Size(369, 21);
            this.textBoxX1.TabIndex = 2;
            // 
            // buttonX3
            // 
            this.buttonX3.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX3.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX3.Location = new System.Drawing.Point(383, 32);
            this.buttonX3.Name = "buttonX3";
            this.buttonX3.Size = new System.Drawing.Size(50, 20);
            this.buttonX3.TabIndex = 3;
            this.buttonX3.Text = "浏览";
            this.buttonX3.Click += new System.EventHandler(this.buttonX3_Click);
            // 
            // labelX1
            // 
            this.labelX1.BackColor = System.Drawing.Color.Transparent;
            this.labelX1.Location = new System.Drawing.Point(8, 3);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(425, 23);
            this.labelX1.TabIndex = 4;
            this.labelX1.Text = "程序在比较未支持格式的文件时，会以命令行方式调用指定的程序进行比较。";
            // 
            // groupPanel1
            // 
            this.groupPanel1.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel1.Controls.Add(this.lhCheckBox);
            this.groupPanel1.Controls.Add(this.lsCheckBox);
            this.groupPanel1.Controls.Add(this.luaCheckBox);
            this.groupPanel1.Controls.Add(this.otherCheckBox);
            this.groupPanel1.Controls.Add(this.logicalCheckBox);
            this.groupPanel1.Controls.Add(this.txtCheckBox);
            this.groupPanel1.Controls.Add(this.iniCheckBox);
            this.groupPanel1.Controls.Add(this.tabCheckBox);
            this.groupPanel1.Location = new System.Drawing.Point(12, 12);
            this.groupPanel1.Name = "groupPanel1";
            this.groupPanel1.Size = new System.Drawing.Size(452, 98);
            // 
            // 
            // 
            this.groupPanel1.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel1.Style.BackColorGradientAngle = 90;
            this.groupPanel1.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel1.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderBottomWidth = 1;
            this.groupPanel1.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel1.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderLeftWidth = 1;
            this.groupPanel1.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderRightWidth = 1;
            this.groupPanel1.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderTopWidth = 1;
            this.groupPanel1.Style.CornerDiameter = 4;
            this.groupPanel1.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel1.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.groupPanel1.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel1.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.groupPanel1.TabIndex = 5;
            this.groupPanel1.Text = "比较文件类型";
            // 
            // luaCheckBox
            // 
            this.luaCheckBox.BackColor = System.Drawing.Color.Transparent;
            this.luaCheckBox.Checked = true;
            this.luaCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.luaCheckBox.CheckValue = "Y";
            this.luaCheckBox.Location = new System.Drawing.Point(266, 14);
            this.luaCheckBox.Name = "luaCheckBox";
            this.luaCheckBox.Size = new System.Drawing.Size(80, 20);
            this.luaCheckBox.TabIndex = 5;
            this.luaCheckBox.Text = ".lua";
            // 
            // otherCheckBox
            // 
            this.otherCheckBox.BackColor = System.Drawing.Color.Transparent;
            this.otherCheckBox.Location = new System.Drawing.Point(180, 40);
            this.otherCheckBox.Name = "otherCheckBox";
            this.otherCheckBox.Size = new System.Drawing.Size(200, 20);
            this.otherCheckBox.TabIndex = 4;
            this.otherCheckBox.Text = "其他（二进制比较，速度较慢）";
            // 
            // logicalCheckBox
            // 
            this.logicalCheckBox.BackColor = System.Drawing.Color.Transparent;
            this.logicalCheckBox.Checked = true;
            this.logicalCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.logicalCheckBox.CheckValue = "Y";
            this.logicalCheckBox.Location = new System.Drawing.Point(94, 40);
            this.logicalCheckBox.Name = "logicalCheckBox";
            this.logicalCheckBox.Size = new System.Drawing.Size(80, 20);
            this.logicalCheckBox.TabIndex = 3;
            this.logicalCheckBox.Text = ".Logical";
            // 
            // txtCheckBox
            // 
            this.txtCheckBox.BackColor = System.Drawing.Color.Transparent;
            this.txtCheckBox.Checked = true;
            this.txtCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.txtCheckBox.CheckValue = "Y";
            this.txtCheckBox.Location = new System.Drawing.Point(94, 14);
            this.txtCheckBox.Name = "txtCheckBox";
            this.txtCheckBox.Size = new System.Drawing.Size(80, 20);
            this.txtCheckBox.TabIndex = 2;
            this.txtCheckBox.Text = ".txt";
            // 
            // iniCheckBox
            // 
            this.iniCheckBox.BackColor = System.Drawing.Color.Transparent;
            this.iniCheckBox.Checked = true;
            this.iniCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.iniCheckBox.CheckValue = "Y";
            this.iniCheckBox.Location = new System.Drawing.Point(180, 14);
            this.iniCheckBox.Name = "iniCheckBox";
            this.iniCheckBox.Size = new System.Drawing.Size(80, 20);
            this.iniCheckBox.TabIndex = 1;
            this.iniCheckBox.Text = ".ini";
            // 
            // tabCheckBox
            // 
            this.tabCheckBox.BackColor = System.Drawing.Color.Transparent;
            this.tabCheckBox.Checked = true;
            this.tabCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.tabCheckBox.CheckValue = "Y";
            this.tabCheckBox.Location = new System.Drawing.Point(8, 14);
            this.tabCheckBox.Name = "tabCheckBox";
            this.tabCheckBox.Size = new System.Drawing.Size(80, 20);
            this.tabCheckBox.TabIndex = 0;
            this.tabCheckBox.Text = ".tab";
            // 
            // groupPanel2
            // 
            this.groupPanel2.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel2.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel2.Controls.Add(this.labelX1);
            this.groupPanel2.Controls.Add(this.textBoxX1);
            this.groupPanel2.Controls.Add(this.buttonX3);
            this.groupPanel2.Location = new System.Drawing.Point(12, 125);
            this.groupPanel2.Name = "groupPanel2";
            this.groupPanel2.Size = new System.Drawing.Size(452, 99);
            // 
            // 
            // 
            this.groupPanel2.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel2.Style.BackColorGradientAngle = 90;
            this.groupPanel2.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel2.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel2.Style.BorderBottomWidth = 1;
            this.groupPanel2.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel2.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel2.Style.BorderLeftWidth = 1;
            this.groupPanel2.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel2.Style.BorderRightWidth = 1;
            this.groupPanel2.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel2.Style.BorderTopWidth = 1;
            this.groupPanel2.Style.CornerDiameter = 4;
            this.groupPanel2.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel2.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.groupPanel2.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel2.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.groupPanel2.TabIndex = 6;
            this.groupPanel2.Text = "外部比较程序";
            // 
            // lsCheckBox
            // 
            this.lsCheckBox.BackColor = System.Drawing.Color.Transparent;
            this.lsCheckBox.Checked = true;
            this.lsCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.lsCheckBox.CheckValue = "Y";
            this.lsCheckBox.Location = new System.Drawing.Point(352, 14);
            this.lsCheckBox.Name = "lsCheckBox";
            this.lsCheckBox.Size = new System.Drawing.Size(80, 20);
            this.lsCheckBox.TabIndex = 6;
            this.lsCheckBox.Text = ".ls";
            // 
            // lhCheckBox
            // 
            this.lhCheckBox.Checked = true;
            this.lhCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.lhCheckBox.CheckValue = "Y";
            this.lhCheckBox.Location = new System.Drawing.Point(8, 40);
            this.lhCheckBox.Name = "lhCheckBox";
            this.lhCheckBox.Size = new System.Drawing.Size(80, 20);
            this.lhCheckBox.TabIndex = 7;
            this.lhCheckBox.Text = ".lh";
            // 
            // ConfigForm
            // 
            this.AcceptButton = this.buttonX1;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonX2;
            this.ClientSize = new System.Drawing.Size(481, 304);
            this.Controls.Add(this.groupPanel2);
            this.Controls.Add(this.groupPanel1);
            this.Controls.Add(this.buttonX2);
            this.Controls.Add(this.buttonX1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ConfigForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "选项配置";
            this.groupPanel1.ResumeLayout(false);
            this.groupPanel2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX1;
        private DevComponents.DotNetBar.ButtonX buttonX3;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel1;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel2;
        private DevComponents.DotNetBar.Controls.CheckBoxX tabCheckBox;
        private DevComponents.DotNetBar.Controls.CheckBoxX iniCheckBox;
        private DevComponents.DotNetBar.Controls.CheckBoxX txtCheckBox;
        private DevComponents.DotNetBar.Controls.CheckBoxX otherCheckBox;
        private DevComponents.DotNetBar.Controls.CheckBoxX logicalCheckBox;
        private DevComponents.DotNetBar.Controls.CheckBoxX luaCheckBox;
        private DevComponents.DotNetBar.Controls.CheckBoxX lsCheckBox;
        private DevComponents.DotNetBar.Controls.CheckBoxX lhCheckBox;
    }
}