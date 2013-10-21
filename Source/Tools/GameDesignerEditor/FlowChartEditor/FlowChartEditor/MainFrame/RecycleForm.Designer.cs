namespace FlowChartEditor
{
    partial class BackupForm
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            this.infoView = new DevComponents.DotNetBar.Controls.DataGridViewX();
            this.bRestore = new DevComponents.DotNetBar.ButtonX();
            this.bCancel = new DevComponents.DotNetBar.ButtonX();
            this.bView = new DevComponents.DotNetBar.ButtonX();
            this.treeContainer = new DevComponents.DotNetBar.TabControl();
            ((System.ComponentModel.ISupportInitialize)(this.infoView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.treeContainer)).BeginInit();
            this.SuspendLayout();
            // 
            // infoView
            // 
            this.infoView.AllowUserToAddRows = false;
            this.infoView.AllowUserToDeleteRows = false;
            this.infoView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.infoView.DefaultCellStyle = dataGridViewCellStyle1;
            this.infoView.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(208)))), ((int)(((byte)(215)))), ((int)(((byte)(229)))));
            this.infoView.Location = new System.Drawing.Point(12, 380);
            this.infoView.Name = "infoView";
            this.infoView.ReadOnly = true;
            this.infoView.RowTemplate.Height = 23;
            this.infoView.Size = new System.Drawing.Size(503, 94);
            this.infoView.TabIndex = 0;
            // 
            // bRestore
            // 
            this.bRestore.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bRestore.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bRestore.Location = new System.Drawing.Point(356, 510);
            this.bRestore.Name = "bRestore";
            this.bRestore.Size = new System.Drawing.Size(70, 20);
            this.bRestore.TabIndex = 1;
            this.bRestore.Text = "恢复";
            this.bRestore.Click += new System.EventHandler(this.bOK_Click);
            // 
            // bCancel
            // 
            this.bCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bCancel.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bCancel.Location = new System.Drawing.Point(445, 510);
            this.bCancel.Name = "bCancel";
            this.bCancel.Size = new System.Drawing.Size(70, 20);
            this.bCancel.TabIndex = 2;
            this.bCancel.Text = "取消";
            this.bCancel.Click += new System.EventHandler(this.bCancel_Click);
            // 
            // bView
            // 
            this.bView.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bView.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bView.Location = new System.Drawing.Point(12, 510);
            this.bView.Name = "bView";
            this.bView.Size = new System.Drawing.Size(70, 20);
            this.bView.TabIndex = 3;
            this.bView.Text = "查看";
            this.bView.Click += new System.EventHandler(this.bView_Click);
            // 
            // treeContainer
            // 
            this.treeContainer.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(194)))), ((int)(((byte)(217)))), ((int)(((byte)(247)))));
            this.treeContainer.CanReorderTabs = true;
            this.treeContainer.Location = new System.Drawing.Point(12, 12);
            this.treeContainer.Name = "treeContainer";
            this.treeContainer.SelectedTabFont = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Bold);
            this.treeContainer.SelectedTabIndex = -1;
            this.treeContainer.Size = new System.Drawing.Size(503, 348);
            this.treeContainer.TabIndex = 5;
            this.treeContainer.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.treeContainer.Text = "tabControl1";
            // 
            // BackupForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(527, 556);
            this.Controls.Add(this.treeContainer);
            this.Controls.Add(this.bView);
            this.Controls.Add(this.bCancel);
            this.Controls.Add(this.bRestore);
            this.Controls.Add(this.infoView);
            this.Name = "BackupForm";
            this.Text = "绘图回收站";
            ((System.ComponentModel.ISupportInitialize)(this.infoView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.treeContainer)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.Controls.DataGridViewX infoView;
        private DevComponents.DotNetBar.ButtonX bRestore;
        private DevComponents.DotNetBar.ButtonX bCancel;
        private DevComponents.DotNetBar.ButtonX bView;
        private DevComponents.DotNetBar.TabControl treeContainer;
    }
}