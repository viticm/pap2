namespace GameDesingerTools
{
    partial class MetainfoEditorForm
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
            DevComponents.Editors.ComboItem 自定义编辑器;
            this.txtConn = new System.Windows.Forms.TextBox();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.descriptiontxt = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.listconditiontxt = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.listfield_comboBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.keyfield_comboBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.listtable_comboBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.txtBoxListValues = new System.Windows.Forms.TextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label11 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.txtBoxRelativePath = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.label12 = new System.Windows.Forms.Label();
            this.add_button = new DevComponents.DotNetBar.ButtonX();
            this.modification_button = new DevComponents.DotNetBar.ButtonX();
            this.delete_button = new DevComponents.DotNetBar.ButtonX();
            this.tablename_comboBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.fieldname_comboBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.fieldcnnametxt = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.catname_comboBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.editortype_comboBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.visible_checkBox = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.readonly_checkBox = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.addField_button = new DevComponents.DotNetBar.ButtonX();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            自定义编辑器 = new DevComponents.Editors.ComboItem();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // 自定义编辑器
            // 
            自定义编辑器.Text = "自定义编辑器";
            // 
            // txtConn
            // 
            this.txtConn.Location = new System.Drawing.Point(22, 174);
            this.txtConn.Multiline = true;
            this.txtConn.Name = "txtConn";
            this.txtConn.Size = new System.Drawing.Size(50, 56);
            this.txtConn.TabIndex = 1;
            this.txtConn.Text = "Server = jx3web; DataBase = s3design; Uid = s3design; Password = ddd12345678;";
            this.txtConn.Visible = false;
            // 
            // dataGridView1
            // 
            this.dataGridView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Location = new System.Drawing.Point(12, 236);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.ReadOnly = true;
            this.dataGridView1.RowTemplate.Height = 23;
            this.dataGridView1.Size = new System.Drawing.Size(793, 298);
            this.dataGridView1.TabIndex = 2;
            this.dataGridView1.SelectionChanged += new System.EventHandler(this.dataGridView1_SelectionChanged);
            // 
            // descriptiontxt
            // 
            this.descriptiontxt.Location = new System.Drawing.Point(97, 131);
            this.descriptiontxt.Multiline = true;
            this.descriptiontxt.Name = "descriptiontxt";
            this.descriptiontxt.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.descriptiontxt.Size = new System.Drawing.Size(246, 86);
            this.descriptiontxt.TabIndex = 7;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(56, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 12);
            this.label1.TabIndex = 15;
            this.label1.Text = "表名:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(20, 43);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(71, 12);
            this.label2.TabIndex = 16;
            this.label2.Text = "字段英文名:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(20, 75);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(71, 12);
            this.label3.TabIndex = 17;
            this.label3.Text = "字段中文名:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(372, 12);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(59, 12);
            this.label4.TabIndex = 18;
            this.label4.Text = "编辑类型:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(44, 103);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(47, 12);
            this.label5.TabIndex = 19;
            this.label5.Text = "类别名:";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(32, 134);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(59, 12);
            this.label8.TabIndex = 22;
            this.label8.Text = "详细说明:";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.listconditiontxt);
            this.groupBox1.Controls.Add(this.listfield_comboBox);
            this.groupBox1.Controls.Add(this.keyfield_comboBox);
            this.groupBox1.Controls.Add(this.listtable_comboBox);
            this.groupBox1.Controls.Add(this.label10);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Controls.Add(this.label7);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Location = new System.Drawing.Point(374, 39);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(332, 145);
            this.groupBox1.TabIndex = 28;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "下拉框(数据表)设置";
            // 
            // listconditiontxt
            // 
            // 
            // 
            // 
            this.listconditiontxt.Border.Class = "TextBoxBorder";
            this.listconditiontxt.Location = new System.Drawing.Point(117, 111);
            this.listconditiontxt.Name = "listconditiontxt";
            this.listconditiontxt.Size = new System.Drawing.Size(209, 21);
            this.listconditiontxt.TabIndex = 39;
            // 
            // listfield_comboBox
            // 
            this.listfield_comboBox.DisplayMember = "Text";
            this.listfield_comboBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.listfield_comboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.listfield_comboBox.FormattingEnabled = true;
            this.listfield_comboBox.Location = new System.Drawing.Point(117, 78);
            this.listfield_comboBox.Name = "listfield_comboBox";
            this.listfield_comboBox.Size = new System.Drawing.Size(209, 22);
            this.listfield_comboBox.TabIndex = 38;
            // 
            // keyfield_comboBox
            // 
            this.keyfield_comboBox.DisplayMember = "Text";
            this.keyfield_comboBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.keyfield_comboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.keyfield_comboBox.FormattingEnabled = true;
            this.keyfield_comboBox.Location = new System.Drawing.Point(117, 48);
            this.keyfield_comboBox.Name = "keyfield_comboBox";
            this.keyfield_comboBox.Size = new System.Drawing.Size(209, 22);
            this.keyfield_comboBox.TabIndex = 37;
            // 
            // listtable_comboBox
            // 
            this.listtable_comboBox.DisplayMember = "Text";
            this.listtable_comboBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.listtable_comboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.listtable_comboBox.FormattingEnabled = true;
            this.listtable_comboBox.Location = new System.Drawing.Point(117, 18);
            this.listtable_comboBox.Name = "listtable_comboBox";
            this.listtable_comboBox.Size = new System.Drawing.Size(209, 22);
            this.listtable_comboBox.TabIndex = 36;
            this.listtable_comboBox.SelectedIndexChanged += new System.EventHandler(this.listtable_comboBox_SelectedIndexChanged);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(16, 114);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(95, 12);
            this.label10.TabIndex = 35;
            this.label10.Text = "关联表过滤条件:";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(4, 82);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(107, 12);
            this.label9.TabIndex = 33;
            this.label9.Text = "关联表的显示字段:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(4, 51);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(107, 12);
            this.label7.TabIndex = 32;
            this.label7.Text = "关联表的键值字段:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(64, 23);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(47, 12);
            this.label6.TabIndex = 31;
            this.label6.Text = "关联表:";
            // 
            // txtBoxListValues
            // 
            this.txtBoxListValues.Location = new System.Drawing.Point(12, 38);
            this.txtBoxListValues.Multiline = true;
            this.txtBoxListValues.Name = "txtBoxListValues";
            this.txtBoxListValues.Size = new System.Drawing.Size(314, 112);
            this.txtBoxListValues.TabIndex = 34;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label11);
            this.groupBox2.Controls.Add(this.txtBoxListValues);
            this.groupBox2.Location = new System.Drawing.Point(420, 249);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(361, 156);
            this.groupBox2.TabIndex = 36;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "下拉框(手工输入)设置";
            this.groupBox2.Visible = false;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(10, 17);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(149, 12);
            this.label11.TabIndex = 35;
            this.label11.Text = "键值及内容:(例:1,abcdef)";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.txtBoxRelativePath);
            this.groupBox3.Controls.Add(this.label12);
            this.groupBox3.Location = new System.Drawing.Point(54, 249);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(332, 156);
            this.groupBox3.TabIndex = 37;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "文件相对路径";
            this.groupBox3.Visible = false;
            // 
            // txtBoxRelativePath
            // 
            // 
            // 
            // 
            this.txtBoxRelativePath.Border.Class = "TextBoxBorder";
            this.txtBoxRelativePath.Location = new System.Drawing.Point(6, 79);
            this.txtBoxRelativePath.Name = "txtBoxRelativePath";
            this.txtBoxRelativePath.Size = new System.Drawing.Size(320, 21);
            this.txtBoxRelativePath.TabIndex = 36;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(10, 17);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(305, 48);
            this.label12.TabIndex = 35;
            this.label12.Text = "相对路径:(例:\\Scripts_s\\skill\\，此路径将会与工具中\r\n设置的根目录组合为绝对路径，如工具选项中设置根\r\n路径为:e:\\client\\，则组合后" +
                "的路径为:\r\ne:\\client\\Scripts_s\\skill)";
            // 
            // add_button
            // 
            this.add_button.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.add_button.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.add_button.Location = new System.Drawing.Point(722, 89);
            this.add_button.Name = "add_button";
            this.add_button.Size = new System.Drawing.Size(83, 23);
            this.add_button.TabIndex = 38;
            this.add_button.Text = "添加";
            this.add_button.Click += new System.EventHandler(this.add_button_Click);
            // 
            // modification_button
            // 
            this.modification_button.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.modification_button.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.modification_button.Location = new System.Drawing.Point(722, 131);
            this.modification_button.Name = "modification_button";
            this.modification_button.Size = new System.Drawing.Size(83, 23);
            this.modification_button.TabIndex = 39;
            this.modification_button.Text = "修改";
            this.modification_button.Click += new System.EventHandler(this.modification_button_Click);
            // 
            // delete_button
            // 
            this.delete_button.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.delete_button.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.delete_button.Location = new System.Drawing.Point(722, 172);
            this.delete_button.Name = "delete_button";
            this.delete_button.Size = new System.Drawing.Size(83, 23);
            this.delete_button.TabIndex = 40;
            this.delete_button.Text = "删除";
            this.delete_button.Click += new System.EventHandler(this.delete_button_Click);
            // 
            // tablename_comboBox
            // 
            this.tablename_comboBox.DisplayMember = "Text";
            this.tablename_comboBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.tablename_comboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.tablename_comboBox.FormattingEnabled = true;
            this.tablename_comboBox.Location = new System.Drawing.Point(97, 7);
            this.tablename_comboBox.Name = "tablename_comboBox";
            this.tablename_comboBox.Size = new System.Drawing.Size(246, 22);
            this.tablename_comboBox.TabIndex = 41;
            this.tablename_comboBox.SelectedIndexChanged += new System.EventHandler(this.comboBox1_SelectedIndexChanged);
            // 
            // fieldname_comboBox
            // 
            this.fieldname_comboBox.DisplayMember = "Text";
            this.fieldname_comboBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.fieldname_comboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.fieldname_comboBox.FormattingEnabled = true;
            this.fieldname_comboBox.Location = new System.Drawing.Point(97, 39);
            this.fieldname_comboBox.Name = "fieldname_comboBox";
            this.fieldname_comboBox.Size = new System.Drawing.Size(246, 22);
            this.fieldname_comboBox.TabIndex = 42;
            this.fieldname_comboBox.SelectedIndexChanged += new System.EventHandler(this.fieldname_comboBox_SelectedIndexChanged);
            // 
            // fieldcnnametxt
            // 
            // 
            // 
            // 
            this.fieldcnnametxt.Border.Class = "TextBoxBorder";
            this.fieldcnnametxt.Location = new System.Drawing.Point(97, 71);
            this.fieldcnnametxt.Name = "fieldcnnametxt";
            this.fieldcnnametxt.Size = new System.Drawing.Size(246, 21);
            this.fieldcnnametxt.TabIndex = 43;
            // 
            // catname_comboBox
            // 
            this.catname_comboBox.DisplayMember = "Text";
            this.catname_comboBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.catname_comboBox.Location = new System.Drawing.Point(97, 99);
            this.catname_comboBox.Name = "catname_comboBox";
            this.catname_comboBox.Size = new System.Drawing.Size(246, 22);
            this.catname_comboBox.TabIndex = 44;
            // 
            // editortype_comboBox
            // 
            this.editortype_comboBox.DisplayMember = "Text";
            this.editortype_comboBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.editortype_comboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.editortype_comboBox.FormattingEnabled = true;
            this.editortype_comboBox.Items.AddRange(new object[] {
            "文本",
            "文件路径浏览",
            "文件名浏览",
            "真假值",
            "下拉框(数据库)",
            "下拉框(手工输入)",
            自定义编辑器});
            this.editortype_comboBox.Location = new System.Drawing.Point(437, 7);
            this.editortype_comboBox.Name = "editortype_comboBox";
            this.editortype_comboBox.Size = new System.Drawing.Size(209, 22);
            this.editortype_comboBox.TabIndex = 45;
            this.editortype_comboBox.SelectedIndexChanged += new System.EventHandler(this.editortype_comboBox_SelectedIndexChanged);
            // 
            // visible_checkBox
            // 
            this.visible_checkBox.Checked = true;
            this.visible_checkBox.Location = new System.Drawing.Point(392, 198);
            this.visible_checkBox.Name = "visible_checkBox";
            this.visible_checkBox.Size = new System.Drawing.Size(75, 23);
            this.visible_checkBox.TabIndex = 46;
            this.visible_checkBox.Text = "显示";
            // 
            // readonly_checkBox
            // 
            this.readonly_checkBox.Location = new System.Drawing.Point(473, 198);
            this.readonly_checkBox.Name = "readonly_checkBox";
            this.readonly_checkBox.Size = new System.Drawing.Size(75, 23);
            this.readonly_checkBox.TabIndex = 47;
            this.readonly_checkBox.Text = "只读";
            // 
            // addField_button
            // 
            this.addField_button.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.addField_button.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.addField_button.Location = new System.Drawing.Point(722, 7);
            this.addField_button.Name = "addField_button";
            this.addField_button.Size = new System.Drawing.Size(83, 23);
            this.addField_button.TabIndex = 48;
            this.addField_button.Text = "添加新字段";
            this.addField_button.Click += new System.EventHandler(this.addField_button_Click);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX1.Location = new System.Drawing.Point(722, 49);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(83, 25);
            this.buttonX1.TabIndex = 49;
            this.buttonX1.Text = "添加关联字段";
            // 
            // MetainfoEditorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            this.ClientSize = new System.Drawing.Size(813, 546);
            this.Controls.Add(this.buttonX1);
            this.Controls.Add(this.addField_button);
            this.Controls.Add(this.readonly_checkBox);
            this.Controls.Add(this.visible_checkBox);
            this.Controls.Add(this.editortype_comboBox);
            this.Controls.Add(this.catname_comboBox);
            this.Controls.Add(this.fieldcnnametxt);
            this.Controls.Add(this.fieldname_comboBox);
            this.Controls.Add(this.tablename_comboBox);
            this.Controls.Add(this.delete_button);
            this.Controls.Add(this.modification_button);
            this.Controls.Add(this.add_button);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.descriptiontxt);
            this.Controls.Add(this.dataGridView1);
            this.Controls.Add(this.txtConn);
            this.Name = "MetainfoEditorForm";
            this.Text = "表元信息维护工具";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MetainfoEditorForm_FormClosed);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MetainfoEditorForm_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txtConn;
        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.TextBox descriptiontxt;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtBoxListValues;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label12;
        private DevComponents.DotNetBar.ButtonX add_button;
        private DevComponents.DotNetBar.ButtonX modification_button;
        private DevComponents.DotNetBar.ButtonX delete_button;
        private DevComponents.DotNetBar.Controls.ComboBoxEx tablename_comboBox;
        private DevComponents.DotNetBar.Controls.ComboBoxEx fieldname_comboBox;
        private DevComponents.DotNetBar.Controls.TextBoxX fieldcnnametxt;
        private DevComponents.DotNetBar.Controls.ComboBoxEx catname_comboBox;
        private DevComponents.DotNetBar.Controls.ComboBoxEx editortype_comboBox;
        private DevComponents.DotNetBar.Controls.ComboBoxEx listtable_comboBox;
        private DevComponents.DotNetBar.Controls.ComboBoxEx keyfield_comboBox;
        private DevComponents.DotNetBar.Controls.ComboBoxEx listfield_comboBox;
        private DevComponents.DotNetBar.Controls.TextBoxX listconditiontxt;
        private DevComponents.DotNetBar.Controls.CheckBoxX visible_checkBox;
        private DevComponents.DotNetBar.Controls.CheckBoxX readonly_checkBox;
        private DevComponents.DotNetBar.Controls.TextBoxX txtBoxRelativePath;
        private DevComponents.DotNetBar.ButtonX addField_button;
        private DevComponents.DotNetBar.ButtonX buttonX1;
    }
}

