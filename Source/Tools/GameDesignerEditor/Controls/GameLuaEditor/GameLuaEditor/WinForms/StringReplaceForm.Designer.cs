namespace GameLuaEditor
{
    partial class StringReplaceForm
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
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.replaceTextBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.findTextBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.cbMaps = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.treePreview = new System.Windows.Forms.TreeView();
            this.wbPriview = new System.Windows.Forms.WebBrowser();
            this.btnFind = new DevComponents.DotNetBar.ButtonX();
            this.btnCancel = new DevComponents.DotNetBar.ButtonX();
            this.label4 = new System.Windows.Forms.Label();
            this.btnReplace = new DevComponents.DotNetBar.ButtonX();
            this.panelEx1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.Controls.Add(this.splitContainer1);
            this.panelEx1.Controls.Add(this.buttonX1);
            this.panelEx1.Controls.Add(this.groupBox1);
            this.panelEx1.Controls.Add(this.btnFind);
            this.panelEx1.Controls.Add(this.btnCancel);
            this.panelEx1.Controls.Add(this.label4);
            this.panelEx1.Controls.Add(this.btnReplace);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(611, 540);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonX1.Location = new System.Drawing.Point(12, 508);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(84, 23);
            this.buttonX1.TabIndex = 12;
            this.buttonX1.Text = "打开文件(&O)";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.replaceTextBox);
            this.groupBox1.Controls.Add(this.findTextBox);
            this.groupBox1.Controls.Add(this.cbMaps);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.groupBox1.Location = new System.Drawing.Point(12, 10);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(589, 148);
            this.groupBox1.TabIndex = 11;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "替换选项";
            // 
            // replaceTextBox
            // 
            this.replaceTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.replaceTextBox.DisplayMember = "Text";
            this.replaceTextBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.replaceTextBox.FormattingEnabled = true;
            this.replaceTextBox.ItemHeight = 15;
            this.replaceTextBox.Location = new System.Drawing.Point(94, 108);
            this.replaceTextBox.Name = "replaceTextBox";
            this.replaceTextBox.Size = new System.Drawing.Size(479, 21);
            this.replaceTextBox.TabIndex = 7;
            // 
            // findTextBox
            // 
            this.findTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.findTextBox.DisplayMember = "Text";
            this.findTextBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.findTextBox.FormattingEnabled = true;
            this.findTextBox.ItemHeight = 15;
            this.findTextBox.Location = new System.Drawing.Point(94, 78);
            this.findTextBox.Name = "findTextBox";
            this.findTextBox.Size = new System.Drawing.Size(479, 21);
            this.findTextBox.TabIndex = 6;
            // 
            // cbMaps
            // 
            this.cbMaps.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cbMaps.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbMaps.FormattingEnabled = true;
            this.cbMaps.Location = new System.Drawing.Point(94, 28);
            this.cbMaps.Name = "cbMaps";
            this.cbMaps.Size = new System.Drawing.Size(479, 20);
            this.cbMaps.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 28);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(65, 12);
            this.label1.TabIndex = 1;
            this.label1.Text = "所属地图：";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(13, 108);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(65, 12);
            this.label3.TabIndex = 4;
            this.label3.Text = "新字符串：";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(13, 78);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(65, 12);
            this.label2.TabIndex = 4;
            this.label2.Text = "原字符串：";
            // 
            // splitContainer1
            // 
            this.splitContainer1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer1.Location = new System.Drawing.Point(12, 218);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.treePreview);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.wbPriview);
            this.splitContainer1.Size = new System.Drawing.Size(589, 269);
            this.splitContainer1.SplitterDistance = 204;
            this.splitContainer1.TabIndex = 0;
            // 
            // treePreview
            // 
            this.treePreview.CheckBoxes = true;
            this.treePreview.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treePreview.HideSelection = false;
            this.treePreview.Location = new System.Drawing.Point(0, 0);
            this.treePreview.Name = "treePreview";
            this.treePreview.Size = new System.Drawing.Size(204, 269);
            this.treePreview.TabIndex = 0;
            this.treePreview.AfterCheck += new System.Windows.Forms.TreeViewEventHandler(this.treePreview_AfterCheck);
            this.treePreview.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treePreview_AfterSelect);
            // 
            // wbPriview
            // 
            this.wbPriview.Dock = System.Windows.Forms.DockStyle.Fill;
            this.wbPriview.Location = new System.Drawing.Point(0, 0);
            this.wbPriview.MinimumSize = new System.Drawing.Size(20, 20);
            this.wbPriview.Name = "wbPriview";
            this.wbPriview.Size = new System.Drawing.Size(381, 269);
            this.wbPriview.TabIndex = 0;
            this.wbPriview.WebBrowserShortcutsEnabled = false;
            // 
            // btnFind
            // 
            this.btnFind.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnFind.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnFind.Location = new System.Drawing.Point(337, 508);
            this.btnFind.Name = "btnFind";
            this.btnFind.Size = new System.Drawing.Size(84, 23);
            this.btnFind.TabIndex = 8;
            this.btnFind.Text = "搜索(&F)";
            this.btnFind.Click += new System.EventHandler(this.btnFind_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(517, 508);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(84, 23);
            this.btnCancel.TabIndex = 7;
            this.btnCancel.Text = "取消(&C)";
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 193);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(89, 12);
            this.label4.TabIndex = 4;
            this.label4.Text = "预览代码更改：";
            // 
            // btnReplace
            // 
            this.btnReplace.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnReplace.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnReplace.Enabled = false;
            this.btnReplace.Location = new System.Drawing.Point(427, 508);
            this.btnReplace.Name = "btnReplace";
            this.btnReplace.Size = new System.Drawing.Size(84, 23);
            this.btnReplace.TabIndex = 6;
            this.btnReplace.Text = "全部保存(&S)";
            this.btnReplace.Click += new System.EventHandler(this.btnReplace_Click);
            // 
            // StringReplaceForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(611, 540);
            this.Controls.Add(this.panelEx1);
            this.MinimumSize = new System.Drawing.Size(400, 400);
            this.Name = "StringReplaceForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "字符串替换";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.StringReplaceForm_FormClosing);
            this.panelEx1.ResumeLayout(false);
            this.panelEx1.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.ButtonX btnCancel;
        private DevComponents.DotNetBar.ButtonX btnReplace;
        private DevComponents.DotNetBar.ButtonX btnFind;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TreeView treePreview;
        private System.Windows.Forms.WebBrowser wbPriview;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ComboBox cbMaps;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.Controls.ComboBoxEx replaceTextBox;
        private DevComponents.DotNetBar.Controls.ComboBoxEx findTextBox;
    }
}