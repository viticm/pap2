namespace GameDesingerTools
{
    partial class AddFieldRelationForm
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
            this.bCancel = new DevComponents.DotNetBar.ButtonX();
            this.bOK = new DevComponents.DotNetBar.ButtonX();
            this.fieldNameBox = new System.Windows.Forms.ListBox();
            this.tableNameBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.tableNameFilter = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.fieldNameFilter = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.panelEx1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.panelEx1.Controls.Add(this.fieldNameFilter);
            this.panelEx1.Controls.Add(this.tableNameFilter);
            this.panelEx1.Controls.Add(this.bCancel);
            this.panelEx1.Controls.Add(this.bOK);
            this.panelEx1.Controls.Add(this.fieldNameBox);
            this.panelEx1.Controls.Add(this.tableNameBox);
            this.panelEx1.Controls.Add(this.labelX2);
            this.panelEx1.Controls.Add(this.labelX1);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(275, 463);
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
            this.bCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.bCancel.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.bCancel.Location = new System.Drawing.Point(192, 420);
            this.bCancel.Name = "bCancel";
            this.bCancel.Size = new System.Drawing.Size(70, 20);
            this.bCancel.TabIndex = 5;
            this.bCancel.Text = "取消";
            this.bCancel.Click += new System.EventHandler(this.bCancel_Click);
            // 
            // bOK
            // 
            this.bOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.bOK.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bOK.Location = new System.Drawing.Point(107, 420);
            this.bOK.Name = "bOK";
            this.bOK.Size = new System.Drawing.Size(70, 20);
            this.bOK.TabIndex = 4;
            this.bOK.Text = "确定";
            this.bOK.Click += new System.EventHandler(this.bOK_Click);
            // 
            // fieldNameBox
            // 
            this.fieldNameBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.fieldNameBox.FormattingEnabled = true;
            this.fieldNameBox.ItemHeight = 12;
            this.fieldNameBox.Location = new System.Drawing.Point(12, 104);
            this.fieldNameBox.Name = "fieldNameBox";
            this.fieldNameBox.SelectionMode = System.Windows.Forms.SelectionMode.MultiSimple;
            this.fieldNameBox.Size = new System.Drawing.Size(250, 292);
            this.fieldNameBox.TabIndex = 3;
            // 
            // tableNameBox
            // 
            this.tableNameBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tableNameBox.DisplayMember = "Text";
            this.tableNameBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.tableNameBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.tableNameBox.FormattingEnabled = true;
            this.tableNameBox.ItemHeight = 15;
            this.tableNameBox.Location = new System.Drawing.Point(12, 39);
            this.tableNameBox.Name = "tableNameBox";
            this.tableNameBox.Size = new System.Drawing.Size(250, 21);
            this.tableNameBox.TabIndex = 2;
            this.tableNameBox.SelectedIndexChanged += new System.EventHandler(this.tableNameBox_SelectedIndexChanged);
            // 
            // labelX2
            // 
            this.labelX2.Location = new System.Drawing.Point(12, 77);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(70, 20);
            this.labelX2.TabIndex = 1;
            this.labelX2.Text = "字段名";
            // 
            // labelX1
            // 
            this.labelX1.Location = new System.Drawing.Point(12, 12);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(70, 20);
            this.labelX1.TabIndex = 0;
            this.labelX1.Text = "数据表名";
            // 
            // tableNameFilter
            // 
            this.tableNameFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            // 
            // 
            // 
            this.tableNameFilter.Border.Class = "TextBoxBorder";
            this.tableNameFilter.Location = new System.Drawing.Point(88, 12);
            this.tableNameFilter.Name = "tableNameFilter";
            this.tableNameFilter.Size = new System.Drawing.Size(174, 21);
            this.tableNameFilter.TabIndex = 6;
            this.tableNameFilter.WatermarkText = "数据表名过滤";
            this.tableNameFilter.TextChanged += new System.EventHandler(this.tableNameFilter_TextChanged);
            // 
            // fieldNameFilter
            // 
            this.fieldNameFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            // 
            // 
            // 
            this.fieldNameFilter.Border.Class = "TextBoxBorder";
            this.fieldNameFilter.Location = new System.Drawing.Point(88, 77);
            this.fieldNameFilter.Name = "fieldNameFilter";
            this.fieldNameFilter.Size = new System.Drawing.Size(174, 21);
            this.fieldNameFilter.TabIndex = 7;
            this.fieldNameFilter.WatermarkText = "字段名过滤";
            this.fieldNameFilter.TextChanged += new System.EventHandler(this.fieldNameFilter_TextChanged);
            // 
            // AddFieldRelationForm
            // 
            this.AcceptButton = this.bOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.bCancel;
            this.ClientSize = new System.Drawing.Size(275, 463);
            this.Controls.Add(this.panelEx1);
            this.Name = "AddFieldRelationForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "添加关联关系";
            this.panelEx1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.LabelX labelX1;
        private System.Windows.Forms.ListBox fieldNameBox;
        private DevComponents.DotNetBar.Controls.ComboBoxEx tableNameBox;
        private DevComponents.DotNetBar.LabelX labelX2;
        private DevComponents.DotNetBar.ButtonX bCancel;
        private DevComponents.DotNetBar.ButtonX bOK;
        private DevComponents.DotNetBar.Controls.TextBoxX tableNameFilter;
        private DevComponents.DotNetBar.Controls.TextBoxX fieldNameFilter;
    }
}