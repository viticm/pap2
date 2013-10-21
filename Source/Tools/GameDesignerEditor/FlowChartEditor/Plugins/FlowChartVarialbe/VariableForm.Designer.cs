namespace FlowChartVarialbe
{
    partial class VariableForm
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
            this.lboxVarialbes = new System.Windows.Forms.ListBox();
            this.btnOK = new DevComponents.DotNetBar.ButtonX();
            this.btnCancle = new DevComponents.DotNetBar.ButtonX();
            this.btnAddNew = new DevComponents.DotNetBar.ButtonX();
            this.btnReName = new DevComponents.DotNetBar.ButtonX();
            this.btnDelete = new DevComponents.DotNetBar.ButtonX();
            this.tbxNewVariable = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.lbChoosedName = new DevComponents.DotNetBar.LabelX();
            this.lbTypeName = new DevComponents.DotNetBar.LabelX();
            this.balloonTip1 = new DevComponents.DotNetBar.BalloonTip();
            this.SuspendLayout();
            // 
            // lboxVarialbes
            // 
            this.lboxVarialbes.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lboxVarialbes.FormattingEnabled = true;
            this.lboxVarialbes.ItemHeight = 12;
            this.lboxVarialbes.Location = new System.Drawing.Point(12, 24);
            this.lboxVarialbes.Name = "lboxVarialbes";
            this.lboxVarialbes.Size = new System.Drawing.Size(518, 268);
            this.lboxVarialbes.TabIndex = 0;
            this.lboxVarialbes.SelectedIndexChanged += new System.EventHandler(this.lboxVarialbes_SelectedIndexChanged);
            // 
            // btnOK
            // 
            this.btnOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btnOK.Location = new System.Drawing.Point(313, 359);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(78, 23);
            this.btnOK.TabIndex = 4;
            this.btnOK.Text = "确定";
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancle
            // 
            this.btnCancle.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnCancle.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancle.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btnCancle.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancle.Location = new System.Drawing.Point(430, 359);
            this.btnCancle.Name = "btnCancle";
            this.btnCancle.Size = new System.Drawing.Size(78, 23);
            this.btnCancle.TabIndex = 5;
            this.btnCancle.Text = "取消";
            this.btnCancle.Click += new System.EventHandler(this.btnCancle_Click);
            // 
            // btnAddNew
            // 
            this.btnAddNew.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnAddNew.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnAddNew.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btnAddNew.Location = new System.Drawing.Point(13, 359);
            this.btnAddNew.Name = "btnAddNew";
            this.btnAddNew.Size = new System.Drawing.Size(56, 23);
            this.btnAddNew.TabIndex = 5;
            this.btnAddNew.Text = "添加";
            this.btnAddNew.Tooltip = "添加新的变量名到列表里";
            this.btnAddNew.Click += new System.EventHandler(this.btnAddNew_Click);
            // 
            // btnReName
            // 
            this.btnReName.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnReName.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnReName.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btnReName.Enabled = false;
            this.btnReName.Location = new System.Drawing.Point(102, 359);
            this.btnReName.Name = "btnReName";
            this.btnReName.Size = new System.Drawing.Size(56, 23);
            this.btnReName.TabIndex = 5;
            this.btnReName.Text = "修改";
            this.btnReName.Tooltip = "把列表中选中的变量名修改为新的变量名";
            this.btnReName.Click += new System.EventHandler(this.btnReName_Click);
            // 
            // btnDelete
            // 
            this.btnDelete.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnDelete.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnDelete.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btnDelete.Enabled = false;
            this.btnDelete.Location = new System.Drawing.Point(191, 359);
            this.btnDelete.Name = "btnDelete";
            this.btnDelete.Size = new System.Drawing.Size(56, 23);
            this.btnDelete.TabIndex = 5;
            this.btnDelete.Text = "删除";
            this.btnDelete.Tooltip = "删除列表中选中的变量";
            this.btnDelete.Click += new System.EventHandler(this.btnDelete_Click);
            // 
            // tbxNewVariable
            // 
            this.tbxNewVariable.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            // 
            // 
            // 
            this.tbxNewVariable.Border.Class = "TextBoxBorder";
            this.tbxNewVariable.Location = new System.Drawing.Point(97, 318);
            this.tbxNewVariable.Name = "tbxNewVariable";
            this.tbxNewVariable.Size = new System.Drawing.Size(151, 21);
            this.tbxNewVariable.TabIndex = 6;
            this.tbxNewVariable.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.tbxNewVariable_KeyPress);
            // 
            // labelX1
            // 
            this.labelX1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.labelX1.Location = new System.Drawing.Point(313, 318);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(82, 30);
            this.labelX1.TabIndex = 7;
            this.labelX1.Text = "选中的变量：";
            // 
            // labelX2
            // 
            this.labelX2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.labelX2.Location = new System.Drawing.Point(13, 318);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(82, 23);
            this.labelX2.TabIndex = 7;
            this.labelX2.Text = "新的变量名：";
            // 
            // lbChoosedName
            // 
            this.lbChoosedName.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.lbChoosedName.Location = new System.Drawing.Point(398, 318);
            this.lbChoosedName.Name = "lbChoosedName";
            this.lbChoosedName.Size = new System.Drawing.Size(143, 30);
            this.lbChoosedName.TabIndex = 7;
            this.lbChoosedName.Text = "未选中任何变量";
            // 
            // lbTypeName
            // 
            this.lbTypeName.Location = new System.Drawing.Point(13, 2);
            this.lbTypeName.Name = "lbTypeName";
            this.lbTypeName.Size = new System.Drawing.Size(137, 18);
            this.lbTypeName.TabIndex = 7;
            this.lbTypeName.Text = "变量类型";
            // 
            // VariableForm
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancle;
            this.ClientSize = new System.Drawing.Size(553, 394);
            this.Controls.Add(this.lbTypeName);
            this.Controls.Add(this.labelX2);
            this.Controls.Add(this.lbChoosedName);
            this.Controls.Add(this.labelX1);
            this.Controls.Add(this.tbxNewVariable);
            this.Controls.Add(this.btnDelete);
            this.Controls.Add(this.btnReName);
            this.Controls.Add(this.btnAddNew);
            this.Controls.Add(this.btnCancle);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.lboxVarialbes);
            this.Name = "VariableForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "变量输入";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListBox lboxVarialbes;
        private DevComponents.DotNetBar.ButtonX btnOK;
        private DevComponents.DotNetBar.ButtonX btnCancle;
        private DevComponents.DotNetBar.ButtonX btnAddNew;
        private DevComponents.DotNetBar.ButtonX btnReName;
        private DevComponents.DotNetBar.ButtonX btnDelete;
        private DevComponents.DotNetBar.Controls.TextBoxX tbxNewVariable;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.LabelX labelX2;
        private DevComponents.DotNetBar.LabelX lbChoosedName;
        private DevComponents.DotNetBar.LabelX lbTypeName;
        private DevComponents.DotNetBar.BalloonTip balloonTip1;
    }
}