namespace CreateAICustomFunction
{
    partial class CustomFunctionForm
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
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.codeEditBox = new luaEditor.luaEditorControl();
            this.textBoxX6 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX7 = new DevComponents.DotNetBar.LabelX();
            this.buttonX3 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.textBoxX5 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.textBoxX4 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.comboBoxEx1 = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.comboItem1 = new DevComponents.Editors.ComboItem();
            this.comboItem2 = new DevComponents.Editors.ComboItem();
            this.textBoxX3 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.textBoxX2 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.textBoxX1 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX6 = new DevComponents.DotNetBar.LabelX();
            this.labelX5 = new DevComponents.DotNetBar.LabelX();
            this.labelX4 = new DevComponents.DotNetBar.LabelX();
            this.labelX3 = new DevComponents.DotNetBar.LabelX();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.panelEx1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx1.Controls.Add(this.codeEditBox);
            this.panelEx1.Controls.Add(this.textBoxX6);
            this.panelEx1.Controls.Add(this.labelX7);
            this.panelEx1.Controls.Add(this.buttonX3);
            this.panelEx1.Controls.Add(this.buttonX2);
            this.panelEx1.Controls.Add(this.buttonX1);
            this.panelEx1.Controls.Add(this.textBoxX5);
            this.panelEx1.Controls.Add(this.textBoxX4);
            this.panelEx1.Controls.Add(this.comboBoxEx1);
            this.panelEx1.Controls.Add(this.textBoxX3);
            this.panelEx1.Controls.Add(this.textBoxX2);
            this.panelEx1.Controls.Add(this.textBoxX1);
            this.panelEx1.Controls.Add(this.labelX6);
            this.panelEx1.Controls.Add(this.labelX5);
            this.panelEx1.Controls.Add(this.labelX4);
            this.panelEx1.Controls.Add(this.labelX3);
            this.panelEx1.Controls.Add(this.labelX2);
            this.panelEx1.Controls.Add(this.labelX1);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(532, 593);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            // 
            // codeEditBox
            // 
            this.codeEditBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.codeEditBox.ContentMenu = null;
            this.codeEditBox.Location = new System.Drawing.Point(12, 209);
            this.codeEditBox.Name = "codeEditBox";
            this.codeEditBox.Size = new System.Drawing.Size(504, 324);
            this.codeEditBox.TabIndex = 17;
            // 
            // textBoxX6
            // 
            this.textBoxX6.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            // 
            // 
            // 
            this.textBoxX6.Border.Class = "TextBoxBorder";
            this.textBoxX6.Location = new System.Drawing.Point(366, 169);
            this.textBoxX6.Name = "textBoxX6";
            this.textBoxX6.Size = new System.Drawing.Size(150, 21);
            this.textBoxX6.TabIndex = 16;
            this.textBoxX6.TextChanged += new System.EventHandler(this.textBoxX6_TextChanged);
            // 
            // labelX7
            // 
            this.labelX7.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.labelX7.Location = new System.Drawing.Point(240, 170);
            this.labelX7.Name = "labelX7";
            this.labelX7.Size = new System.Drawing.Size(120, 20);
            this.labelX7.TabIndex = 15;
            this.labelX7.Text = "返回值（逗号分隔）";
            // 
            // buttonX3
            // 
            this.buttonX3.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX3.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX3.Location = new System.Drawing.Point(174, 170);
            this.buttonX3.Name = "buttonX3";
            this.buttonX3.Size = new System.Drawing.Size(50, 20);
            this.buttonX3.TabIndex = 14;
            this.buttonX3.Text = "编辑";
            this.buttonX3.Click += new System.EventHandler(this.buttonX3_Click);
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX2.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX2.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonX2.Location = new System.Drawing.Point(446, 552);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(70, 20);
            this.buttonX2.TabIndex = 13;
            this.buttonX2.Text = "取消";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX1.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX1.Location = new System.Drawing.Point(357, 552);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(70, 20);
            this.buttonX1.TabIndex = 12;
            this.buttonX1.Text = "确定";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // textBoxX5
            // 
            // 
            // 
            // 
            this.textBoxX5.Border.Class = "TextBoxBorder";
            this.textBoxX5.Location = new System.Drawing.Point(68, 169);
            this.textBoxX5.Name = "textBoxX5";
            this.textBoxX5.ReadOnly = true;
            this.textBoxX5.Size = new System.Drawing.Size(100, 21);
            this.textBoxX5.TabIndex = 11;
            this.textBoxX5.TextChanged += new System.EventHandler(this.textBoxX5_TextChanged);
            // 
            // textBoxX4
            // 
            // 
            // 
            // 
            this.textBoxX4.Border.Class = "TextBoxBorder";
            this.textBoxX4.Location = new System.Drawing.Point(68, 117);
            this.textBoxX4.Name = "textBoxX4";
            this.textBoxX4.Size = new System.Drawing.Size(100, 21);
            this.textBoxX4.TabIndex = 10;
            // 
            // comboBoxEx1
            // 
            this.comboBoxEx1.DisplayMember = "Text";
            this.comboBoxEx1.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.comboBoxEx1.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxEx1.FormattingEnabled = true;
            this.comboBoxEx1.ItemHeight = 15;
            this.comboBoxEx1.Items.AddRange(new object[] {
            this.comboItem1,
            this.comboItem2});
            this.comboBoxEx1.Location = new System.Drawing.Point(68, 82);
            this.comboBoxEx1.Name = "comboBoxEx1";
            this.comboBoxEx1.Size = new System.Drawing.Size(100, 21);
            this.comboBoxEx1.TabIndex = 9;
            this.comboBoxEx1.SelectedIndexChanged += new System.EventHandler(this.comboBoxEx1_SelectedIndexChanged);
            // 
            // comboItem1
            // 
            this.comboItem1.Text = "自定义";
            // 
            // comboItem2
            // 
            this.comboItem2.Text = "系统";
            // 
            // textBoxX3
            // 
            this.textBoxX3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            // 
            // 
            // 
            this.textBoxX3.Border.Class = "TextBoxBorder";
            this.textBoxX3.Location = new System.Drawing.Point(261, 12);
            this.textBoxX3.Multiline = true;
            this.textBoxX3.Name = "textBoxX3";
            this.textBoxX3.Size = new System.Drawing.Size(255, 90);
            this.textBoxX3.TabIndex = 8;
            // 
            // textBoxX2
            // 
            // 
            // 
            // 
            this.textBoxX2.Border.Class = "TextBoxBorder";
            this.textBoxX2.Location = new System.Drawing.Point(68, 47);
            this.textBoxX2.Name = "textBoxX2";
            this.textBoxX2.Size = new System.Drawing.Size(100, 21);
            this.textBoxX2.TabIndex = 7;
            this.textBoxX2.TextChanged += new System.EventHandler(this.textBoxX2_TextChanged);
            // 
            // textBoxX1
            // 
            // 
            // 
            // 
            this.textBoxX1.Border.Class = "TextBoxBorder";
            this.textBoxX1.Location = new System.Drawing.Point(68, 12);
            this.textBoxX1.Name = "textBoxX1";
            this.textBoxX1.Size = new System.Drawing.Size(100, 21);
            this.textBoxX1.TabIndex = 6;
            this.textBoxX1.TextChanged += new System.EventHandler(this.textBoxX1_TextChanged);
            // 
            // labelX6
            // 
            this.labelX6.Location = new System.Drawing.Point(12, 170);
            this.labelX6.Name = "labelX6";
            this.labelX6.Size = new System.Drawing.Size(50, 20);
            this.labelX6.TabIndex = 5;
            this.labelX6.Text = "参数";
            // 
            // labelX5
            // 
            this.labelX5.Location = new System.Drawing.Point(12, 117);
            this.labelX5.Name = "labelX5";
            this.labelX5.Size = new System.Drawing.Size(50, 20);
            this.labelX5.TabIndex = 4;
            this.labelX5.Text = "分类";
            // 
            // labelX4
            // 
            this.labelX4.Location = new System.Drawing.Point(12, 82);
            this.labelX4.Name = "labelX4";
            this.labelX4.Size = new System.Drawing.Size(50, 20);
            this.labelX4.TabIndex = 3;
            this.labelX4.Text = "类型";
            // 
            // labelX3
            // 
            this.labelX3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.labelX3.Location = new System.Drawing.Point(205, 12);
            this.labelX3.Name = "labelX3";
            this.labelX3.Size = new System.Drawing.Size(50, 20);
            this.labelX3.TabIndex = 2;
            this.labelX3.Text = "说明";
            // 
            // labelX2
            // 
            this.labelX2.Location = new System.Drawing.Point(12, 47);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(50, 20);
            this.labelX2.TabIndex = 1;
            this.labelX2.Text = "中文名";
            // 
            // labelX1
            // 
            this.labelX1.Location = new System.Drawing.Point(12, 12);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(50, 20);
            this.labelX1.TabIndex = 0;
            this.labelX1.Text = "英文名";
            // 
            // CustomFunctionForm
            // 
            this.AcceptButton = this.buttonX1;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonX2;
            this.ClientSize = new System.Drawing.Size(532, 593);
            this.Controls.Add(this.panelEx1);
            this.Name = "CustomFunctionForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "自定义动作配置";
            this.panelEx1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX3;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX2;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX1;
        private DevComponents.DotNetBar.LabelX labelX6;
        private DevComponents.DotNetBar.LabelX labelX5;
        private DevComponents.DotNetBar.LabelX labelX4;
        private DevComponents.DotNetBar.LabelX labelX3;
        private DevComponents.DotNetBar.LabelX labelX2;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX4;
        private DevComponents.DotNetBar.Controls.ComboBoxEx comboBoxEx1;
        private DevComponents.Editors.ComboItem comboItem1;
        private DevComponents.Editors.ComboItem comboItem2;
        private DevComponents.DotNetBar.LabelX labelX7;
        private DevComponents.DotNetBar.ButtonX buttonX3;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX5;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX6;
        private luaEditor.luaEditorControl codeEditBox;
    }
}