namespace BaseExp
{
    partial class ExpForm
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
            this.radioExp = new System.Windows.Forms.RadioButton();
            this.radioConst = new System.Windows.Forms.RadioButton();
            this.txtConst = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.radioEventArg = new System.Windows.Forms.RadioButton();
            this.comboBoxEventArg = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.comboBoxConst = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.btnInput = new DevComponents.DotNetBar.ButtonX();
            this.btnSelect = new DevComponents.DotNetBar.ButtonX();
            this.radioConstSel = new System.Windows.Forms.RadioButton();
            this.btnCancel = new DevComponents.DotNetBar.ButtonX();
            this.btnOK = new DevComponents.DotNetBar.ButtonX();
            this.comboBoxAPI = new BaseExp.SearchComboBox.SearchComboBoxEx();
            this.expControl1 = new BaseExp.ExpControl();
            this.panelEx1.SuspendLayout();
            this.SuspendLayout();
            // 
            // radioExp
            // 
            this.radioExp.AutoSize = true;
            this.radioExp.Location = new System.Drawing.Point(5, 20);
            this.radioExp.Name = "radioExp";
            this.radioExp.Size = new System.Drawing.Size(65, 16);
            this.radioExp.TabIndex = 0;
            this.radioExp.Text = "功能(&F)";
            this.radioExp.UseVisualStyleBackColor = true;
            this.radioExp.Enter += new System.EventHandler(this.radioExp_Enter);
            this.radioExp.EnabledChanged += new System.EventHandler(this.radioButton_EnabledChanged);
            // 
            // radioConst
            // 
            this.radioConst.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.radioConst.AutoSize = true;
            this.radioConst.BackColor = System.Drawing.Color.Transparent;
            this.radioConst.Location = new System.Drawing.Point(5, 206);
            this.radioConst.Name = "radioConst";
            this.radioConst.Size = new System.Drawing.Size(65, 16);
            this.radioConst.TabIndex = 4;
            this.radioConst.Text = "常数(&N)";
            this.radioConst.UseVisualStyleBackColor = false;
            this.radioConst.Enter += new System.EventHandler(this.radioConst_Enter);
            this.radioConst.CheckedChanged += new System.EventHandler(this.radioConst_CheckedChanged);
            this.radioConst.EnabledChanged += new System.EventHandler(this.radioButton_EnabledChanged);
            // 
            // txtConst
            // 
            this.txtConst.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtConst.Location = new System.Drawing.Point(70, 203);
            this.txtConst.Name = "txtConst";
            this.txtConst.Size = new System.Drawing.Size(361, 21);
            this.txtConst.TabIndex = 5;
            this.txtConst.Enter += new System.EventHandler(this.txtConst_Enter);
            // 
            // radioEventArg
            // 
            this.radioEventArg.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.radioEventArg.AutoSize = true;
            this.radioEventArg.BackColor = System.Drawing.Color.Transparent;
            this.radioEventArg.Location = new System.Drawing.Point(5, 263);
            this.radioEventArg.Name = "radioEventArg";
            this.radioEventArg.Size = new System.Drawing.Size(59, 16);
            this.radioEventArg.TabIndex = 10;
            this.radioEventArg.Text = "本事件";
            this.radioEventArg.UseVisualStyleBackColor = false;
            this.radioEventArg.Enter += new System.EventHandler(this.radioEventArg_Enter);
            this.radioEventArg.EnabledChanged += new System.EventHandler(this.radioButton_EnabledChanged);
            // 
            // comboBoxEventArg
            // 
            this.comboBoxEventArg.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.comboBoxEventArg.DisplayMember = "Text";
            this.comboBoxEventArg.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.comboBoxEventArg.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxEventArg.FormattingEnabled = true;
            this.comboBoxEventArg.Location = new System.Drawing.Point(70, 260);
            this.comboBoxEventArg.Name = "comboBoxEventArg";
            this.comboBoxEventArg.Size = new System.Drawing.Size(186, 22);
            this.comboBoxEventArg.TabIndex = 11;
            this.comboBoxEventArg.ValueMember = "Text";
            this.comboBoxEventArg.Enter += new System.EventHandler(this.comboBoxEventArg_Enter);
            // 
            // comboBoxConst
            // 
            this.comboBoxConst.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.comboBoxConst.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.comboBoxConst.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxConst.FormattingEnabled = true;
            this.comboBoxConst.Location = new System.Drawing.Point(70, 231);
            this.comboBoxConst.Name = "comboBoxConst";
            this.comboBoxConst.Size = new System.Drawing.Size(361, 22);
            this.comboBoxConst.TabIndex = 8;
            this.comboBoxConst.Enter += new System.EventHandler(this.comboBoxConst_Enter);
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.Controls.Add(this.btnInput);
            this.panelEx1.Controls.Add(this.btnSelect);
            this.panelEx1.Controls.Add(this.radioConstSel);
            this.panelEx1.Controls.Add(this.btnCancel);
            this.panelEx1.Controls.Add(this.btnOK);
            this.panelEx1.Controls.Add(this.radioExp);
            this.panelEx1.Controls.Add(this.comboBoxConst);
            this.panelEx1.Controls.Add(this.comboBoxAPI);
            this.panelEx1.Controls.Add(this.radioConst);
            this.panelEx1.Controls.Add(this.comboBoxEventArg);
            this.panelEx1.Controls.Add(this.expControl1);
            this.panelEx1.Controls.Add(this.radioEventArg);
            this.panelEx1.Controls.Add(this.txtConst);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(526, 330);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 13;
            // 
            // btnInput
            // 
            this.btnInput.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnInput.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnInput.Enabled = false;
            this.btnInput.Location = new System.Drawing.Point(437, 202);
            this.btnInput.Name = "btnInput";
            this.btnInput.Size = new System.Drawing.Size(72, 22);
            this.btnInput.TabIndex = 6;
            this.btnInput.Text = "输入(&I)";
            this.btnInput.Click += new System.EventHandler(this.btnInput_Click);
            // 
            // btnSelect
            // 
            this.btnSelect.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnSelect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnSelect.Location = new System.Drawing.Point(437, 231);
            this.btnSelect.Name = "btnSelect";
            this.btnSelect.Size = new System.Drawing.Size(72, 22);
            this.btnSelect.TabIndex = 9;
            this.btnSelect.Text = "浏览(&B)";
            this.btnSelect.Click += new System.EventHandler(this.btnSelect_Click);
            // 
            // radioConstSel
            // 
            this.radioConstSel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.radioConstSel.AutoSize = true;
            this.radioConstSel.BackColor = System.Drawing.Color.Transparent;
            this.radioConstSel.Location = new System.Drawing.Point(5, 235);
            this.radioConstSel.Name = "radioConstSel";
            this.radioConstSel.Size = new System.Drawing.Size(59, 16);
            this.radioConstSel.TabIndex = 7;
            this.radioConstSel.Text = "请选择";
            this.radioConstSel.UseVisualStyleBackColor = false;
            this.radioConstSel.Enter += new System.EventHandler(this.radioConstSel_Enter);
            this.radioConstSel.EnabledChanged += new System.EventHandler(this.radioButton_EnabledChanged);
            // 
            // btnCancel
            // 
            this.btnCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(437, 295);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 13;
            this.btnCancel.Text = "取消(&C)";
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnOK
            // 
            this.btnOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.Location = new System.Drawing.Point(356, 295);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 12;
            this.btnOK.Text = "确定(&O)";
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // comboBoxAPI
            // 
            this.comboBoxAPI.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.comboBoxAPI.DisplayMember = "Text";
            this.comboBoxAPI.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.comboBoxAPI.FormattingEnabled = true;
            this.comboBoxAPI.Items_SelectedItem = null;
            this.comboBoxAPI.Location = new System.Drawing.Point(70, 17);
            this.comboBoxAPI.MaxDropDownItems = 20;
            this.comboBoxAPI.Name = "comboBoxAPI";
            this.comboBoxAPI.Size = new System.Drawing.Size(439, 22);
            this.comboBoxAPI.TabIndex = 1;
            this.comboBoxAPI.SelectedIndexChanged += new System.EventHandler(this.comboBoxAPI_SelectedIndexChanged);
            // 
            // expControl1
            // 
            this.expControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.expControl1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.expControl1.Location = new System.Drawing.Point(70, 46);
            this.expControl1.Name = "expControl1";
            this.expControl1.Size = new System.Drawing.Size(439, 151);
            this.expControl1.TabIndex = 2;
            // 
            // ExpForm
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(526, 330);
            this.Controls.Add(this.panelEx1);
            this.MinimumSize = new System.Drawing.Size(324, 278);
            this.Name = "ExpForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "-";
            this.Load += new System.EventHandler(this.ExpForm_Load);
            this.Shown += new System.EventHandler(this.ExpForm_Shown);
            this.panelEx1.ResumeLayout(false);
            this.panelEx1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private ExpControl expControl1;
        //private System.Windows.Forms.ComboBox comboBoxAPI;
        SearchComboBox.SearchComboBoxEx comboBoxAPI;
        private System.Windows.Forms.RadioButton radioExp;
        private System.Windows.Forms.RadioButton radioConst;
        private DevComponents.DotNetBar.Controls.TextBoxX txtConst;
        private System.Windows.Forms.RadioButton radioEventArg;
        private DevComponents.DotNetBar.Controls.ComboBoxEx comboBoxEventArg;
        private DevComponents.DotNetBar.Controls.ComboBoxEx comboBoxConst;
        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.ButtonX btnCancel;
        private DevComponents.DotNetBar.ButtonX btnOK;
        private System.Windows.Forms.RadioButton radioConstSel;
        private DevComponents.DotNetBar.ButtonX btnSelect;
        private DevComponents.DotNetBar.ButtonX btnInput;
    }
}

