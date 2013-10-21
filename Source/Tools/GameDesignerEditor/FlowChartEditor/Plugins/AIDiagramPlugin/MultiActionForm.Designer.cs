namespace AIDiagramPlugin
{
    partial class MultiActionForm
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
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX3 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX4 = new DevComponents.DotNetBar.ButtonX();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.buttonCopy = new DevComponents.DotNetBar.ButtonX();
            this.btnMoveDown = new DevComponents.DotNetBar.ButtonX();
            this.btnMoveUp = new DevComponents.DotNetBar.ButtonX();
            this.SuspendLayout();
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX2.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonX2.Location = new System.Drawing.Point(541, 354);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(70, 20);
            this.buttonX2.TabIndex = 7;
            this.buttonX2.Text = "取消(&C)";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX1.Location = new System.Drawing.Point(465, 354);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(70, 20);
            this.buttonX1.TabIndex = 6;
            this.buttonX1.Text = "确定(&O)";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // buttonX3
            // 
            this.buttonX3.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonX3.Location = new System.Drawing.Point(12, 354);
            this.buttonX3.Name = "buttonX3";
            this.buttonX3.Size = new System.Drawing.Size(60, 20);
            this.buttonX3.TabIndex = 0;
            this.buttonX3.Text = "添加(&A)";
            this.buttonX3.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // buttonX4
            // 
            this.buttonX4.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonX4.Location = new System.Drawing.Point(78, 354);
            this.buttonX4.Name = "buttonX4";
            this.buttonX4.Size = new System.Drawing.Size(60, 20);
            this.buttonX4.TabIndex = 1;
            this.buttonX4.Text = "删除(&D)";
            this.buttonX4.Click += new System.EventHandler(this.btnDel_Click);
            // 
            // listBox1
            // 
            this.listBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listBox1.FormattingEnabled = true;
            this.listBox1.ItemHeight = 12;
            this.listBox1.Location = new System.Drawing.Point(12, 12);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(599, 316);
            this.listBox1.TabIndex = 5;
            this.listBox1.DoubleClick += new System.EventHandler(this.listBox1_DoubleClick);
            // 
            // buttonCopy
            // 
            this.buttonCopy.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonCopy.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonCopy.Location = new System.Drawing.Point(144, 354);
            this.buttonCopy.Name = "buttonCopy";
            this.buttonCopy.Size = new System.Drawing.Size(60, 20);
            this.buttonCopy.TabIndex = 4;
            this.buttonCopy.Text = "复制";
            this.buttonCopy.Click += new System.EventHandler(this.buttonCopy_Click);
            // 
            // btnMoveDown
            // 
            this.btnMoveDown.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnMoveDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnMoveDown.Location = new System.Drawing.Point(276, 354);
            this.btnMoveDown.Name = "btnMoveDown";
            this.btnMoveDown.Size = new System.Drawing.Size(60, 20);
            this.btnMoveDown.TabIndex = 3;
            this.btnMoveDown.Text = "下移";
            this.btnMoveDown.Click += new System.EventHandler(this.btnMoveDown_Click);
            // 
            // btnMoveUp
            // 
            this.btnMoveUp.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnMoveUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnMoveUp.Location = new System.Drawing.Point(210, 354);
            this.btnMoveUp.Name = "btnMoveUp";
            this.btnMoveUp.Size = new System.Drawing.Size(60, 20);
            this.btnMoveUp.TabIndex = 2;
            this.btnMoveUp.Text = "上移";
            this.btnMoveUp.Click += new System.EventHandler(this.btnMoveUp_Click);
            // 
            // MultiActionForm
            // 
            this.AcceptButton = this.buttonX1;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonX2;
            this.ClientSize = new System.Drawing.Size(623, 392);
            this.Controls.Add(this.buttonCopy);
            this.Controls.Add(this.btnMoveDown);
            this.Controls.Add(this.btnMoveUp);
            this.Controls.Add(this.listBox1);
            this.Controls.Add(this.buttonX2);
            this.Controls.Add(this.buttonX4);
            this.Controls.Add(this.buttonX3);
            this.Controls.Add(this.buttonX1);
            this.Name = "MultiActionForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "动作组";
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.ButtonX buttonX3;
        private DevComponents.DotNetBar.ButtonX buttonX4;
        private System.Windows.Forms.ListBox listBox1;
        private DevComponents.DotNetBar.ButtonX buttonCopy;
        private DevComponents.DotNetBar.ButtonX btnMoveDown;
        private DevComponents.DotNetBar.ButtonX btnMoveUp;

    }
}