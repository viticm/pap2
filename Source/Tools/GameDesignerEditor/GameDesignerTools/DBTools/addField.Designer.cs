namespace GameDesingerTools
{
    partial class addField
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
            this.label1 = new System.Windows.Forms.Label();
            this.edit_FieldName = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.combo_DataType = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.groupPanel1 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.edit_TableName = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.buttonOK = new DevComponents.DotNetBar.ButtonX();
            this.buttonCancel = new DevComponents.DotNetBar.ButtonX();
            this.buttonCheck = new DevComponents.DotNetBar.ButtonX();
            this.groupPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.Location = new System.Drawing.Point(43, 14);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 12);
            this.label1.TabIndex = 16;
            this.label1.Text = "表名:";
            // 
            // edit_FieldName
            // 
            // 
            // 
            // 
            this.edit_FieldName.Border.Class = "TextBoxBorder";
            this.edit_FieldName.Location = new System.Drawing.Point(84, 37);
            this.edit_FieldName.Name = "edit_FieldName";
            this.edit_FieldName.Size = new System.Drawing.Size(196, 21);
            this.edit_FieldName.TabIndex = 45;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.BackColor = System.Drawing.Color.Transparent;
            this.label3.Location = new System.Drawing.Point(31, 41);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(47, 12);
            this.label3.TabIndex = 44;
            this.label3.Text = "字段名:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.BackColor = System.Drawing.Color.Transparent;
            this.label2.Location = new System.Drawing.Point(19, 70);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(59, 12);
            this.label2.TabIndex = 46;
            this.label2.Text = "数据类型:";
            // 
            // combo_DataType
            // 
            this.combo_DataType.DisplayMember = "Text";
            this.combo_DataType.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.combo_DataType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.combo_DataType.FormattingEnabled = true;
            this.combo_DataType.Location = new System.Drawing.Point(84, 64);
            this.combo_DataType.Name = "combo_DataType";
            this.combo_DataType.Size = new System.Drawing.Size(196, 22);
            this.combo_DataType.TabIndex = 47;
            // 
            // groupPanel1
            // 
            this.groupPanel1.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel1.Controls.Add(this.buttonCheck);
            this.groupPanel1.Controls.Add(this.edit_TableName);
            this.groupPanel1.Controls.Add(this.label1);
            this.groupPanel1.Controls.Add(this.combo_DataType);
            this.groupPanel1.Controls.Add(this.label2);
            this.groupPanel1.Controls.Add(this.label3);
            this.groupPanel1.Controls.Add(this.edit_FieldName);
            this.groupPanel1.Location = new System.Drawing.Point(12, 12);
            this.groupPanel1.Name = "groupPanel1";
            this.groupPanel1.Size = new System.Drawing.Size(397, 140);
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
            this.groupPanel1.TabIndex = 48;
            this.groupPanel1.Text = "数据表";
            // 
            // edit_TableName
            // 
            // 
            // 
            // 
            this.edit_TableName.Border.Class = "TextBoxBorder";
            this.edit_TableName.Location = new System.Drawing.Point(84, 10);
            this.edit_TableName.Name = "edit_TableName";
            this.edit_TableName.Size = new System.Drawing.Size(196, 21);
            this.edit_TableName.TabIndex = 48;
            // 
            // buttonOK
            // 
            this.buttonOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonOK.Location = new System.Drawing.Point(235, 164);
            this.buttonOK.Name = "buttonOK";
            this.buttonOK.Size = new System.Drawing.Size(75, 23);
            this.buttonOK.TabIndex = 49;
            this.buttonOK.Text = "确定";
            this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
            // 
            // buttonCancel
            // 
            this.buttonCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonCancel.Location = new System.Drawing.Point(326, 164);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(75, 23);
            this.buttonCancel.TabIndex = 50;
            this.buttonCancel.Text = "取消";
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            // 
            // buttonCheck
            // 
            this.buttonCheck.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonCheck.Location = new System.Drawing.Point(286, 35);
            this.buttonCheck.Name = "buttonCheck";
            this.buttonCheck.Size = new System.Drawing.Size(102, 23);
            this.buttonCheck.TabIndex = 49;
            this.buttonCheck.Text = "字段使用了吗?";
            this.buttonCheck.Click += new System.EventHandler(this.buttonCheck_Click);
            // 
            // addField
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            this.ClientSize = new System.Drawing.Size(421, 197);
            this.Controls.Add(this.buttonCancel);
            this.Controls.Add(this.buttonOK);
            this.Controls.Add(this.groupPanel1);
            this.Name = "addField";
            this.Text = "添加新字段";
            this.groupPanel1.ResumeLayout(false);
            this.groupPanel1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private DevComponents.DotNetBar.Controls.TextBoxX edit_FieldName;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private DevComponents.DotNetBar.Controls.ComboBoxEx combo_DataType;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel1;
        private DevComponents.DotNetBar.Controls.TextBoxX edit_TableName;
        private DevComponents.DotNetBar.ButtonX buttonOK;
        private DevComponents.DotNetBar.ButtonX buttonCancel;
        private DevComponents.DotNetBar.ButtonX buttonCheck;
    }
}