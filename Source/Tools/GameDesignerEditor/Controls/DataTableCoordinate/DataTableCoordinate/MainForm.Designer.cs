namespace DataTableCoordinate
{
    partial class MainForm
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
            this.resultView = new DevComponents.DotNetBar.Controls.DataGridViewX();
            this.progressBar = new DevComponents.DotNetBar.Controls.ProgressBarX();
            this.lProgressInfo = new DevComponents.DotNetBar.LabelX();
            this.bStartSearch = new DevComponents.DotNetBar.ButtonX();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.lMark = new DevComponents.DotNetBar.LabelX();
            this.sourceDatabaseNameBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.destinationDatabaseNameBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            ((System.ComponentModel.ISupportInitialize)(this.resultView)).BeginInit();
            this.SuspendLayout();
            // 
            // resultView
            // 
            this.resultView.AllowUserToAddRows = false;
            this.resultView.AllowUserToDeleteRows = false;
            this.resultView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.resultView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.resultView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.resultView.DefaultCellStyle = dataGridViewCellStyle1;
            this.resultView.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(208)))), ((int)(((byte)(215)))), ((int)(((byte)(229)))));
            this.resultView.Location = new System.Drawing.Point(12, 12);
            this.resultView.Name = "resultView";
            this.resultView.ReadOnly = true;
            this.resultView.RowTemplate.Height = 23;
            this.resultView.Size = new System.Drawing.Size(591, 272);
            this.resultView.TabIndex = 0;
            // 
            // progressBar
            // 
            this.progressBar.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.progressBar.Location = new System.Drawing.Point(12, 303);
            this.progressBar.Name = "progressBar";
            this.progressBar.Size = new System.Drawing.Size(591, 23);
            this.progressBar.TabIndex = 1;
            this.progressBar.Text = "progressBarX1";
            // 
            // lProgressInfo
            // 
            this.lProgressInfo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lProgressInfo.Location = new System.Drawing.Point(12, 332);
            this.lProgressInfo.Name = "lProgressInfo";
            this.lProgressInfo.Size = new System.Drawing.Size(591, 23);
            this.lProgressInfo.TabIndex = 2;
            this.lProgressInfo.Text = "当前进度";
            // 
            // bStartSearch
            // 
            this.bStartSearch.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bStartSearch.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.bStartSearch.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bStartSearch.Location = new System.Drawing.Point(457, 389);
            this.bStartSearch.Name = "bStartSearch";
            this.bStartSearch.Size = new System.Drawing.Size(70, 20);
            this.bStartSearch.TabIndex = 3;
            this.bStartSearch.Text = "开始检索";
            this.bStartSearch.Click += new System.EventHandler(this.bStartSearch_Click);
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX2.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX2.Location = new System.Drawing.Point(533, 389);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(70, 20);
            this.buttonX2.TabIndex = 4;
            this.buttonX2.Text = "开始同步";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // lMark
            // 
            this.lMark.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lMark.Location = new System.Drawing.Point(168, 389);
            this.lMark.Name = "lMark";
            this.lMark.Size = new System.Drawing.Size(20, 21);
            this.lMark.TabIndex = 7;
            this.lMark.Text = "->";
            // 
            // sourceDatabaseNameBox
            // 
            this.sourceDatabaseNameBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.sourceDatabaseNameBox.DisplayMember = "Text";
            this.sourceDatabaseNameBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.sourceDatabaseNameBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.sourceDatabaseNameBox.FormattingEnabled = true;
            this.sourceDatabaseNameBox.ItemHeight = 15;
            this.sourceDatabaseNameBox.Location = new System.Drawing.Point(12, 389);
            this.sourceDatabaseNameBox.Name = "sourceDatabaseNameBox";
            this.sourceDatabaseNameBox.Size = new System.Drawing.Size(150, 21);
            this.sourceDatabaseNameBox.TabIndex = 8;
            // 
            // destinationDatabaseNameBox
            // 
            this.destinationDatabaseNameBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.destinationDatabaseNameBox.DisplayMember = "Text";
            this.destinationDatabaseNameBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.destinationDatabaseNameBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.destinationDatabaseNameBox.FormattingEnabled = true;
            this.destinationDatabaseNameBox.ItemHeight = 15;
            this.destinationDatabaseNameBox.Location = new System.Drawing.Point(194, 389);
            this.destinationDatabaseNameBox.Name = "destinationDatabaseNameBox";
            this.destinationDatabaseNameBox.Size = new System.Drawing.Size(150, 21);
            this.destinationDatabaseNameBox.TabIndex = 9;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(615, 428);
            this.Controls.Add(this.destinationDatabaseNameBox);
            this.Controls.Add(this.sourceDatabaseNameBox);
            this.Controls.Add(this.lMark);
            this.Controls.Add(this.buttonX2);
            this.Controls.Add(this.bStartSearch);
            this.Controls.Add(this.lProgressInfo);
            this.Controls.Add(this.progressBar);
            this.Controls.Add(this.resultView);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "数据表列项同步工具";
            ((System.ComponentModel.ISupportInitialize)(this.resultView)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.Controls.DataGridViewX resultView;
        private DevComponents.DotNetBar.Controls.ProgressBarX progressBar;
        private DevComponents.DotNetBar.LabelX lProgressInfo;
        private DevComponents.DotNetBar.ButtonX bStartSearch;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.LabelX lMark;
        private DevComponents.DotNetBar.Controls.ComboBoxEx sourceDatabaseNameBox;
        private DevComponents.DotNetBar.Controls.ComboBoxEx destinationDatabaseNameBox;
    }
}

