namespace GameDesingerTools
{
    partial class ImportInfo
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
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.editBegin = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.editEnd = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.comboKey = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.buttonOK = new DevComponents.DotNetBar.ButtonX();
            this.buttonCancel = new DevComponents.DotNetBar.ButtonX();
            this.radioButton1 = new System.Windows.Forms.RadioButton();
            this.radioButton2 = new System.Windows.Forms.RadioButton();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.panelEx1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorScheme.ItemDesignTimeBorder = System.Drawing.Color.Black;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx1.Controls.Add(this.groupBox1);
            this.panelEx1.Controls.Add(this.radioButton2);
            this.panelEx1.Controls.Add(this.radioButton1);
            this.panelEx1.Controls.Add(this.buttonOK);
            this.panelEx1.Controls.Add(this.buttonCancel);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(481, 334);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 51;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.Location = new System.Drawing.Point(19, 36);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(59, 12);
            this.label1.TabIndex = 16;
            this.label1.Text = "行号字段:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.BackColor = System.Drawing.Color.Transparent;
            this.label3.Location = new System.Drawing.Point(19, 70);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(59, 12);
            this.label3.TabIndex = 44;
            this.label3.Text = "起始行号:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.BackColor = System.Drawing.Color.Transparent;
            this.label4.Location = new System.Drawing.Point(19, 103);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(59, 12);
            this.label4.TabIndex = 50;
            this.label4.Text = "结束行号:";
            // 
            // editBegin
            // 
            // 
            // 
            // 
            this.editBegin.Border.Class = "TextBoxBorder";
            this.editBegin.Location = new System.Drawing.Point(97, 66);
            this.editBegin.Name = "editBegin";
            this.editBegin.Size = new System.Drawing.Size(196, 21);
            this.editBegin.TabIndex = 45;
            // 
            // editEnd
            // 
            // 
            // 
            // 
            this.editEnd.Border.Class = "TextBoxBorder";
            this.editEnd.Location = new System.Drawing.Point(97, 101);
            this.editEnd.Name = "editEnd";
            this.editEnd.Size = new System.Drawing.Size(196, 21);
            this.editEnd.TabIndex = 51;
            // 
            // comboKey
            // 
            this.comboKey.DisplayMember = "Text";
            this.comboKey.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.comboKey.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboKey.FormattingEnabled = true;
            this.comboKey.Location = new System.Drawing.Point(97, 32);
            this.comboKey.Name = "comboKey";
            this.comboKey.Size = new System.Drawing.Size(196, 22);
            this.comboKey.TabIndex = 54;
            // 
            // buttonOK
            // 
            this.buttonOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.buttonOK.Location = new System.Drawing.Point(142, 280);
            this.buttonOK.Name = "buttonOK";
            this.buttonOK.Size = new System.Drawing.Size(75, 23);
            this.buttonOK.TabIndex = 52;
            this.buttonOK.Text = "确定";
            this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
            // 
            // buttonCancel
            // 
            this.buttonCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonCancel.Location = new System.Drawing.Point(244, 280);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(75, 23);
            this.buttonCancel.TabIndex = 53;
            this.buttonCancel.Text = "取消";
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            // 
            // radioButton1
            // 
            this.radioButton1.AutoSize = true;
            this.radioButton1.Location = new System.Drawing.Point(43, 43);
            this.radioButton1.Name = "radioButton1";
            this.radioButton1.Size = new System.Drawing.Size(59, 16);
            this.radioButton1.TabIndex = 55;
            this.radioButton1.TabStop = true;
            this.radioButton1.Text = "全部行";
            this.radioButton1.UseVisualStyleBackColor = true;
            this.radioButton1.CheckedChanged += new System.EventHandler(this.radioButton1_CheckedChanged);
            // 
            // radioButton2
            // 
            this.radioButton2.AutoSize = true;
            this.radioButton2.Location = new System.Drawing.Point(43, 94);
            this.radioButton2.Name = "radioButton2";
            this.radioButton2.Size = new System.Drawing.Size(59, 16);
            this.radioButton2.TabIndex = 56;
            this.radioButton2.TabStop = true;
            this.radioButton2.Text = "指定行";
            this.radioButton2.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.comboKey);
            this.groupBox1.Controls.Add(this.editEnd);
            this.groupBox1.Controls.Add(this.editBegin);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Enabled = false;
            this.groupBox1.Location = new System.Drawing.Point(108, 90);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(311, 148);
            this.groupBox1.TabIndex = 57;
            this.groupBox1.TabStop = false;
            // 
            // ImportInfo
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(481, 334);
            this.Controls.Add(this.panelEx1);
            this.Name = "ImportInfo";
            this.Text = "请选择导入方式";
            this.panelEx1.ResumeLayout(false);
            this.panelEx1.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private DevComponents.DotNetBar.Controls.TextBoxX editBegin;
        private DevComponents.DotNetBar.Controls.TextBoxX editEnd;
        private DevComponents.DotNetBar.Controls.ComboBoxEx comboKey;
        private DevComponents.DotNetBar.ButtonX buttonOK;
        private DevComponents.DotNetBar.ButtonX buttonCancel;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton radioButton2;
        private System.Windows.Forms.RadioButton radioButton1;
    }
}