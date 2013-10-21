namespace autoConfig
{
    partial class frmMain
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
            this.panel1 = new System.Windows.Forms.Panel();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.ckList = new System.Windows.Forms.CheckedListBox();
            this.cmdAll = new System.Windows.Forms.Button();
            this.cmdDel = new System.Windows.Forms.Button();
            this.cmdCancel = new System.Windows.Forms.Button();
            this.panel1.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.splitContainer1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(641, 354);
            this.panel1.TabIndex = 0;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.ckList);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.AutoScrollMargin = new System.Drawing.Size(0, 200);
            this.splitContainer1.Panel2.Controls.Add(this.cmdAll);
            this.splitContainer1.Panel2.Controls.Add(this.cmdDel);
            this.splitContainer1.Panel2.Controls.Add(this.cmdCancel);
            this.splitContainer1.Size = new System.Drawing.Size(641, 354);
            this.splitContainer1.SplitterDistance = 315;
            this.splitContainer1.TabIndex = 0;
            // 
            // ckList
            // 
            this.ckList.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.ckList.CheckOnClick = true;
            this.ckList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.ckList.FormattingEnabled = true;
            this.ckList.Location = new System.Drawing.Point(0, 0);
            this.ckList.Name = "ckList";
            this.ckList.Size = new System.Drawing.Size(641, 306);
            this.ckList.TabIndex = 0;
            this.ckList.ThreeDCheckBoxes = true;
            // 
            // cmdAll
            // 
            this.cmdAll.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.cmdAll.Location = new System.Drawing.Point(12, 3);
            this.cmdAll.Name = "cmdAll";
            this.cmdAll.Size = new System.Drawing.Size(66, 25);
            this.cmdAll.TabIndex = 2;
            this.cmdAll.Text = "全选(&A)";
            this.cmdAll.UseVisualStyleBackColor = true;
            this.cmdAll.Click += new System.EventHandler(this.cmdAll_Click);
            // 
            // cmdDel
            // 
            this.cmdDel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.cmdDel.Location = new System.Drawing.Point(500, 3);
            this.cmdDel.Name = "cmdDel";
            this.cmdDel.Size = new System.Drawing.Size(66, 25);
            this.cmdDel.TabIndex = 1;
            this.cmdDel.Text = "删除(&D)";
            this.cmdDel.UseVisualStyleBackColor = true;
            this.cmdDel.Click += new System.EventHandler(this.cmdDel_Click);
            // 
            // cmdCancel
            // 
            this.cmdCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.cmdCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cmdCancel.Location = new System.Drawing.Point(572, 2);
            this.cmdCancel.Name = "cmdCancel";
            this.cmdCancel.Size = new System.Drawing.Size(66, 26);
            this.cmdCancel.TabIndex = 0;
            this.cmdCancel.Text = "取消(&C)";
            this.cmdCancel.UseVisualStyleBackColor = true;
            this.cmdCancel.Click += new System.EventHandler(this.cmdCancel_Click);
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cmdCancel;
            this.ClientSize = new System.Drawing.Size(641, 354);
            this.Controls.Add(this.panel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "frmMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.CheckedListBox ckList;
        private System.Windows.Forms.Button cmdCancel;
        private System.Windows.Forms.Button cmdDel;
        private System.Windows.Forms.Button cmdAll;
    }
}

