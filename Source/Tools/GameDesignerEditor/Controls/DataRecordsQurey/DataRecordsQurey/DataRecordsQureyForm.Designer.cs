namespace DataRecordsQurey
{
    partial class DataRecordsQureyForm
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
            this.label7 = new System.Windows.Forms.Label();
            this.dtpEnd = new System.Windows.Forms.DateTimePicker();
            this.label5 = new System.Windows.Forms.Label();
            this.dtpBegin = new System.Windows.Forms.DateTimePicker();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.tbxFieldName = new System.Windows.Forms.TextBox();
            this.tbxUserName = new System.Windows.Forms.TextBox();
            this.btnQuery = new System.Windows.Forms.Button();
            this.tbxNodeName = new System.Windows.Forms.TextBox();
            this.combxModelName = new System.Windows.Forms.ComboBox();
            this.cbxFieldName = new System.Windows.Forms.CheckBox();
            this.cbxModelName = new System.Windows.Forms.CheckBox();
            this.cbxNodeName = new System.Windows.Forms.CheckBox();
            this.cbxUserName = new System.Windows.Forms.CheckBox();
            this.ckbxRemarks = new System.Windows.Forms.CheckBox();
            this.ckbxCatFieldName = new System.Windows.Forms.CheckBox();
            this.ckbxNewValue = new System.Windows.Forms.CheckBox();
            this.ckbxTabName = new System.Windows.Forms.CheckBox();
            this.ckbxOldValue = new System.Windows.Forms.CheckBox();
            this.ckbxFieldDiaplayName = new System.Windows.Forms.CheckBox();
            this.ckbxModelName = new System.Windows.Forms.CheckBox();
            this.gvDataRecord = new System.Windows.Forms.DataGridView();
            this.ckbxTime = new System.Windows.Forms.CheckBox();
            this.ckbxNodeName = new System.Windows.Forms.CheckBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.ckbxUserName = new System.Windows.Forms.CheckBox();
            this.btExport = new System.Windows.Forms.Button();
            this.btnQuit = new System.Windows.Forms.Button();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gvDataRecord)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(13, 32);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(53, 12);
            this.label7.TabIndex = 22;
            this.label7.Text = "起始时间";
            // 
            // dtpEnd
            // 
            this.dtpEnd.Location = new System.Drawing.Point(86, 59);
            this.dtpEnd.MaxDate = System.DateTime.Now;
            this.dtpEnd.Name = "dtpEnd";
            this.dtpEnd.Size = new System.Drawing.Size(119, 21);
            this.dtpEnd.TabIndex = 24;
            
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(13, 64);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(53, 12);
            this.label5.TabIndex = 25;
            this.label5.Text = "终止时间";
            // 
            // dtpBegin
            // 
            this.dtpBegin.Location = new System.Drawing.Point(86, 28);
            this.dtpBegin.MaxDate = System.DateTime.Now;
            this.dtpBegin.Name = "dtpBegin";
            this.dtpBegin.Size = new System.Drawing.Size(119, 21);
            this.dtpBegin.TabIndex = 23;
            
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.tbxFieldName);
            this.groupBox2.Controls.Add(this.tbxUserName);
            this.groupBox2.Controls.Add(this.btnQuery);
            this.groupBox2.Controls.Add(this.tbxNodeName);
            this.groupBox2.Controls.Add(this.combxModelName);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this.dtpBegin);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.cbxFieldName);
            this.groupBox2.Controls.Add(this.dtpEnd);
            this.groupBox2.Controls.Add(this.cbxModelName);
            this.groupBox2.Controls.Add(this.cbxNodeName);
            this.groupBox2.Controls.Add(this.cbxUserName);
            this.groupBox2.Location = new System.Drawing.Point(12, 432);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(819, 86);
            this.groupBox2.TabIndex = 35;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "筛选条件：";
            // 
            // tbxFieldName
            // 
            this.tbxFieldName.Enabled = false;
            this.tbxFieldName.Location = new System.Drawing.Point(497, 63);
            this.tbxFieldName.Name = "tbxFieldName";
            this.tbxFieldName.Size = new System.Drawing.Size(119, 21);
            this.tbxFieldName.TabIndex = 26;
            // 
            // tbxUserName
            // 
            this.tbxUserName.Enabled = false;
            this.tbxUserName.Location = new System.Drawing.Point(294, 32);
            this.tbxUserName.Name = "tbxUserName";
            this.tbxUserName.Size = new System.Drawing.Size(119, 21);
            this.tbxUserName.TabIndex = 26;
            // 
            // btnQuery
            // 
            this.btnQuery.Location = new System.Drawing.Point(631, 64);
            this.btnQuery.Name = "btnQuery";
            this.btnQuery.Size = new System.Drawing.Size(75, 23);
            this.btnQuery.TabIndex = 36;
            this.btnQuery.Text = "查询";
            this.btnQuery.UseVisualStyleBackColor = true;
            this.btnQuery.Click += new System.EventHandler(this.btnQuery_Click);
            // 
            // tbxNodeName
            // 
            this.tbxNodeName.Enabled = false;
            this.tbxNodeName.Location = new System.Drawing.Point(294, 63);
            this.tbxNodeName.Name = "tbxNodeName";
            this.tbxNodeName.Size = new System.Drawing.Size(119, 21);
            this.tbxNodeName.TabIndex = 26;
            // 
            // combxModelName
            // 
            this.combxModelName.Enabled = false;
            this.combxModelName.Location = new System.Drawing.Point(497, 32);
            this.combxModelName.Name = "combxModelName";
            this.combxModelName.Size = new System.Drawing.Size(119, 20);
            this.combxModelName.TabIndex = 0;
            // 
            // cbxFieldName
            // 
            this.cbxFieldName.AutoSize = true;
            this.cbxFieldName.Location = new System.Drawing.Point(438, 63);
            this.cbxFieldName.Name = "cbxFieldName";
            this.cbxFieldName.Size = new System.Drawing.Size(60, 16);
            this.cbxFieldName.TabIndex = 0;
            this.cbxFieldName.Text = "字段名";
            this.cbxFieldName.UseVisualStyleBackColor = true;
            this.cbxFieldName.CheckedChanged += new System.EventHandler(this.cbxFieldName_CheckedChanged);
            // 
            // cbxModelName
            // 
            this.cbxModelName.AutoSize = true;
            this.cbxModelName.Location = new System.Drawing.Point(438, 32);
            this.cbxModelName.Name = "cbxModelName";
            this.cbxModelName.Size = new System.Drawing.Size(60, 16);
            this.cbxModelName.TabIndex = 0;
            this.cbxModelName.Text = "模块名";
            this.cbxModelName.UseVisualStyleBackColor = true;
            this.cbxModelName.CheckedChanged += new System.EventHandler(this.cbxModelName_CheckedChanged);
            // 
            // cbxNodeName
            // 
            this.cbxNodeName.AutoSize = true;
            this.cbxNodeName.Location = new System.Drawing.Point(221, 63);
            this.cbxNodeName.Name = "cbxNodeName";
            this.cbxNodeName.Size = new System.Drawing.Size(72, 16);
            this.cbxNodeName.TabIndex = 0;
            this.cbxNodeName.Text = "树节点名";
            this.cbxNodeName.UseVisualStyleBackColor = true;
            this.cbxNodeName.CheckedChanged += new System.EventHandler(this.cbxNodeName_CheckedChanged);
            // 
            // cbxUserName
            // 
            this.cbxUserName.AutoSize = true;
            this.cbxUserName.Location = new System.Drawing.Point(221, 32);
            this.cbxUserName.Name = "cbxUserName";
            this.cbxUserName.Size = new System.Drawing.Size(60, 16);
            this.cbxUserName.TabIndex = 0;
            this.cbxUserName.Text = "用户名";
            this.cbxUserName.UseVisualStyleBackColor = true;
            this.cbxUserName.CheckedChanged += new System.EventHandler(this.cbxUserName_CheckedChanged);
            // 
            // ckbxRemarks
            // 
            this.ckbxRemarks.AutoSize = true;
            this.ckbxRemarks.Checked = true;
            this.ckbxRemarks.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ckbxRemarks.Location = new System.Drawing.Point(15, 219);
            this.ckbxRemarks.Name = "ckbxRemarks";
            this.ckbxRemarks.Size = new System.Drawing.Size(48, 16);
            this.ckbxRemarks.TabIndex = 0;
            this.ckbxRemarks.Text = "备注";
            this.ckbxRemarks.UseVisualStyleBackColor = true;
            this.ckbxRemarks.CheckedChanged += new System.EventHandler(this.ckbxUserName_CheckedChanged);
            // 
            // ckbxCatFieldName
            // 
            this.ckbxCatFieldName.AutoSize = true;
            this.ckbxCatFieldName.Checked = true;
            this.ckbxCatFieldName.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ckbxCatFieldName.Location = new System.Drawing.Point(15, 197);
            this.ckbxCatFieldName.Name = "ckbxCatFieldName";
            this.ckbxCatFieldName.Size = new System.Drawing.Size(72, 16);
            this.ckbxCatFieldName.TabIndex = 0;
            this.ckbxCatFieldName.Text = "所属分类";
            this.ckbxCatFieldName.UseVisualStyleBackColor = true;
            this.ckbxCatFieldName.CheckedChanged += new System.EventHandler(this.ckbxUserName_CheckedChanged);
            // 
            // ckbxNewValue
            // 
            this.ckbxNewValue.AutoSize = true;
            this.ckbxNewValue.Checked = true;
            this.ckbxNewValue.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ckbxNewValue.Location = new System.Drawing.Point(15, 175);
            this.ckbxNewValue.Name = "ckbxNewValue";
            this.ckbxNewValue.Size = new System.Drawing.Size(48, 16);
            this.ckbxNewValue.TabIndex = 0;
            this.ckbxNewValue.Text = "新值";
            this.ckbxNewValue.UseVisualStyleBackColor = true;
            this.ckbxNewValue.CheckedChanged += new System.EventHandler(this.ckbxUserName_CheckedChanged);
            // 
            // ckbxTabName
            // 
            this.ckbxTabName.AutoSize = true;
            this.ckbxTabName.Checked = true;
            this.ckbxTabName.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ckbxTabName.Location = new System.Drawing.Point(15, 153);
            this.ckbxTabName.Name = "ckbxTabName";
            this.ckbxTabName.Size = new System.Drawing.Size(72, 16);
            this.ckbxTabName.TabIndex = 0;
            this.ckbxTabName.Text = "属性页名";
            this.ckbxTabName.UseVisualStyleBackColor = true;
            this.ckbxTabName.CheckedChanged += new System.EventHandler(this.ckbxUserName_CheckedChanged);
            // 
            // ckbxOldValue
            // 
            this.ckbxOldValue.AutoSize = true;
            this.ckbxOldValue.Checked = true;
            this.ckbxOldValue.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ckbxOldValue.Location = new System.Drawing.Point(15, 130);
            this.ckbxOldValue.Name = "ckbxOldValue";
            this.ckbxOldValue.Size = new System.Drawing.Size(48, 16);
            this.ckbxOldValue.TabIndex = 0;
            this.ckbxOldValue.Text = "旧值";
            this.ckbxOldValue.UseVisualStyleBackColor = true;
            this.ckbxOldValue.CheckedChanged += new System.EventHandler(this.ckbxUserName_CheckedChanged);
            // 
            // ckbxFieldDiaplayName
            // 
            this.ckbxFieldDiaplayName.AutoSize = true;
            this.ckbxFieldDiaplayName.Checked = true;
            this.ckbxFieldDiaplayName.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ckbxFieldDiaplayName.Location = new System.Drawing.Point(15, 86);
            this.ckbxFieldDiaplayName.Name = "ckbxFieldDiaplayName";
            this.ckbxFieldDiaplayName.Size = new System.Drawing.Size(60, 16);
            this.ckbxFieldDiaplayName.TabIndex = 0;
            this.ckbxFieldDiaplayName.Text = "字段名";
            this.ckbxFieldDiaplayName.UseVisualStyleBackColor = true;
            this.ckbxFieldDiaplayName.CheckedChanged += new System.EventHandler(this.ckbxUserName_CheckedChanged);
            // 
            // ckbxModelName
            // 
            this.ckbxModelName.AutoSize = true;
            this.ckbxModelName.Checked = true;
            this.ckbxModelName.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ckbxModelName.Location = new System.Drawing.Point(15, 108);
            this.ckbxModelName.Name = "ckbxModelName";
            this.ckbxModelName.Size = new System.Drawing.Size(60, 16);
            this.ckbxModelName.TabIndex = 0;
            this.ckbxModelName.Text = "模块名";
            this.ckbxModelName.UseVisualStyleBackColor = true;
            this.ckbxModelName.CheckedChanged += new System.EventHandler(this.ckbxUserName_CheckedChanged);
            // 
            // gvDataRecord
            // 
            this.gvDataRecord.AllowUserToAddRows = false;
            this.gvDataRecord.AllowUserToDeleteRows = false;
            this.gvDataRecord.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.gvDataRecord.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.gvDataRecord.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.gvDataRecord.GridColor = System.Drawing.SystemColors.Control;
            this.gvDataRecord.Location = new System.Drawing.Point(2, -1);
            this.gvDataRecord.Name = "gvDataRecord";
            this.gvDataRecord.ReadOnly = true;
            this.gvDataRecord.RowTemplate.Height = 23;
            this.gvDataRecord.Size = new System.Drawing.Size(829, 427);
            this.gvDataRecord.TabIndex = 39;
            // 
            // ckbxTime
            // 
            this.ckbxTime.AutoSize = true;
            this.ckbxTime.Checked = true;
            this.ckbxTime.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ckbxTime.Location = new System.Drawing.Point(15, 64);
            this.ckbxTime.Name = "ckbxTime";
            this.ckbxTime.Size = new System.Drawing.Size(72, 16);
            this.ckbxTime.TabIndex = 0;
            this.ckbxTime.Text = "修改时间";
            this.ckbxTime.UseVisualStyleBackColor = true;
            this.ckbxTime.CheckedChanged += new System.EventHandler(this.ckbxUserName_CheckedChanged);
            // 
            // ckbxNodeName
            // 
            this.ckbxNodeName.AutoSize = true;
            this.ckbxNodeName.Checked = true;
            this.ckbxNodeName.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ckbxNodeName.Location = new System.Drawing.Point(15, 42);
            this.ckbxNodeName.Name = "ckbxNodeName";
            this.ckbxNodeName.Size = new System.Drawing.Size(72, 16);
            this.ckbxNodeName.TabIndex = 0;
            this.ckbxNodeName.Text = "树结点名";
            this.ckbxNodeName.UseVisualStyleBackColor = true;
            this.ckbxNodeName.CheckedChanged += new System.EventHandler(this.ckbxUserName_CheckedChanged);
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.ckbxRemarks);
            this.groupBox1.Controls.Add(this.ckbxCatFieldName);
            this.groupBox1.Controls.Add(this.ckbxNewValue);
            this.groupBox1.Controls.Add(this.ckbxTabName);
            this.groupBox1.Controls.Add(this.ckbxOldValue);
            this.groupBox1.Controls.Add(this.ckbxModelName);
            this.groupBox1.Controls.Add(this.ckbxFieldDiaplayName);
            this.groupBox1.Controls.Add(this.ckbxTime);
            this.groupBox1.Controls.Add(this.ckbxNodeName);
            this.groupBox1.Controls.Add(this.ckbxUserName);
            this.groupBox1.Location = new System.Drawing.Point(834, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(96, 260);
            this.groupBox1.TabIndex = 40;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "选择显示列:";
            // 
            // ckbxUserName
            // 
            this.ckbxUserName.AutoSize = true;
            this.ckbxUserName.Checked = true;
            this.ckbxUserName.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ckbxUserName.Location = new System.Drawing.Point(15, 20);
            this.ckbxUserName.Name = "ckbxUserName";
            this.ckbxUserName.Size = new System.Drawing.Size(60, 16);
            this.ckbxUserName.TabIndex = 0;
            this.ckbxUserName.Text = "用户名";
            this.ckbxUserName.UseVisualStyleBackColor = true;
            this.ckbxUserName.CheckedChanged += new System.EventHandler(this.ckbxUserName_CheckedChanged);
            // 
            // btExport
            // 
            this.btExport.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btExport.Location = new System.Drawing.Point(834, 355);
            this.btExport.Name = "btExport";
            this.btExport.Size = new System.Drawing.Size(75, 23);
            this.btExport.TabIndex = 36;
            this.btExport.Text = "导出";
            this.btExport.UseVisualStyleBackColor = true;
            this.btExport.Click += new System.EventHandler(this.btExport_Click);
            // 
            // btnQuit
            // 
            this.btnQuit.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnQuit.Location = new System.Drawing.Point(834, 495);
            this.btnQuit.Name = "btnQuit";
            this.btnQuit.Size = new System.Drawing.Size(75, 23);
            this.btnQuit.TabIndex = 36;
            this.btnQuit.Text = "退出";
            this.btnQuit.UseVisualStyleBackColor = true;
            this.btnQuit.Click += new System.EventHandler(this.btnQuit_Click);
            // 
            // DataRecordsQureyForm
            // 
            this.AcceptButton = this.btnQuery;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnQuit;
            this.ClientSize = new System.Drawing.Size(935, 524);
            this.Controls.Add(this.gvDataRecord);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.btnQuit);
            this.Controls.Add(this.btExport);
            this.Controls.Add(this.groupBox2);
            this.Name = "DataRecordsQureyForm";
            this.Text = "Form1";
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gvDataRecord)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.DateTimePicker dtpEnd;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.DateTimePicker dtpBegin;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button btnQuery;
        private System.Windows.Forms.ComboBox combxModelName;
        private System.Windows.Forms.CheckBox cbxModelName;
        private System.Windows.Forms.CheckBox cbxFieldName;
        private System.Windows.Forms.CheckBox cbxNodeName;
        private System.Windows.Forms.CheckBox cbxUserName;
        private System.Windows.Forms.TextBox tbxFieldName;
        private System.Windows.Forms.TextBox tbxNodeName;
        private System.Windows.Forms.TextBox tbxUserName;
        private System.Windows.Forms.CheckBox ckbxRemarks;
        private System.Windows.Forms.CheckBox ckbxCatFieldName;
        private System.Windows.Forms.CheckBox ckbxNewValue;
        private System.Windows.Forms.CheckBox ckbxTabName;
        private System.Windows.Forms.CheckBox ckbxOldValue;
        private System.Windows.Forms.CheckBox ckbxFieldDiaplayName;
        private System.Windows.Forms.CheckBox ckbxModelName;
        private System.Windows.Forms.DataGridView gvDataRecord;
        private System.Windows.Forms.CheckBox ckbxTime;
        private System.Windows.Forms.CheckBox ckbxNodeName;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox ckbxUserName;
        private System.Windows.Forms.Button btExport;
        private System.Windows.Forms.Button btnQuit;
    }
}

