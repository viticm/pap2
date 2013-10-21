namespace CustomLoadTree
{
    partial class QuestStatForm
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
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.SiteName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Total = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.QType0 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.QType1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.QType2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.QType3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.QType4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.QType5 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.QType6 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.QType7 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.QType8 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.QType10 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            this.SuspendLayout();
            // 
            // dataGridView1
            // 
            this.dataGridView1.AllowUserToAddRows = false;
            this.dataGridView1.AllowUserToDeleteRows = false;
            this.dataGridView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.SiteName,
            this.Total,
            this.QType0,
            this.QType1,
            this.QType2,
            this.QType3,
            this.QType4,
            this.QType5,
            this.QType6,
            this.QType7,
            this.QType8,
            this.QType10});
            this.dataGridView1.Location = new System.Drawing.Point(1, 1);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.ReadOnly = true;
            this.dataGridView1.RowTemplate.Height = 23;
            this.dataGridView1.Size = new System.Drawing.Size(1151, 413);
            this.dataGridView1.TabIndex = 0;
            // 
            // SiteName
            // 
            this.SiteName.DataPropertyName = "SiteName";
            this.SiteName.HeaderText = "场景";
            this.SiteName.Name = "SiteName";
            this.SiteName.ReadOnly = true;
            // 
            // Total
            // 
            this.Total.DataPropertyName = "Total";
            this.Total.HeaderText = "任务总数";
            this.Total.Name = "Total";
            this.Total.ReadOnly = true;
            // 
            // QType0
            // 
            this.QType0.DataPropertyName = "QType0";
            this.QType0.HeaderText = "前置不可接任务";
            this.QType0.Name = "QType0";
            this.QType0.ReadOnly = true;
            this.QType0.Width = 120;
            // 
            // QType1
            // 
            this.QType1.DataPropertyName = "QType1";
            this.QType1.HeaderText = "脚本任务";
            this.QType1.Name = "QType1";
            this.QType1.ReadOnly = true;
            // 
            // QType2
            // 
            this.QType2.DataPropertyName = "QType2";
            this.QType2.HeaderText = "采集任务";
            this.QType2.Name = "QType2";
            this.QType2.ReadOnly = true;
            // 
            // QType3
            // 
            this.QType3.DataPropertyName = "QType3";
            this.QType3.HeaderText = "收集任务";
            this.QType3.Name = "QType3";
            this.QType3.ReadOnly = true;
            // 
            // QType4
            // 
            this.QType4.DataPropertyName = "QType4";
            this.QType4.HeaderText = "杀怪任务";
            this.QType4.Name = "QType4";
            this.QType4.ReadOnly = true;
            // 
            // QType5
            // 
            this.QType5.DataPropertyName = "QType5";
            this.QType5.HeaderText = "送道具任务";
            this.QType5.Name = "QType5";
            this.QType5.ReadOnly = true;
            // 
            // QType6
            // 
            this.QType6.DataPropertyName = "QType6";
            this.QType6.HeaderText = "对话任务";
            this.QType6.Name = "QType6";
            this.QType6.ReadOnly = true;
            // 
            // QType7
            // 
            this.QType7.DataPropertyName = "QType7";
            this.QType7.HeaderText = "X_计时任务";
            this.QType7.Name = "QType7";
            this.QType7.ReadOnly = true;
            // 
            // QType8
            // 
            this.QType8.DataPropertyName = "QType8";
            this.QType8.HeaderText = "前置任务集合";
            this.QType8.Name = "QType8";
            this.QType8.ReadOnly = true;
            // 
            // QType10
            // 
            this.QType10.DataPropertyName = "QType10";
            this.QType10.HeaderText = "后续任务";
            this.QType10.Name = "QType10";
            this.QType10.ReadOnly = true;
            // 
            // QuestStatForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1153, 534);
            this.Controls.Add(this.dataGridView1);
            this.Name = "QuestStatForm";
            this.Text = "各类型任务数量统计";
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion


        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.DataGridViewTextBoxColumn SiteName;
        private System.Windows.Forms.DataGridViewTextBoxColumn Total;
        private System.Windows.Forms.DataGridViewTextBoxColumn QType0;
        private System.Windows.Forms.DataGridViewTextBoxColumn QType1;
        private System.Windows.Forms.DataGridViewTextBoxColumn QType2;
        private System.Windows.Forms.DataGridViewTextBoxColumn QType3;
        private System.Windows.Forms.DataGridViewTextBoxColumn QType4;
        private System.Windows.Forms.DataGridViewTextBoxColumn QType5;
        private System.Windows.Forms.DataGridViewTextBoxColumn QType6;
        private System.Windows.Forms.DataGridViewTextBoxColumn QType7;
        private System.Windows.Forms.DataGridViewTextBoxColumn QType8;
        private System.Windows.Forms.DataGridViewTextBoxColumn QType10;
    }
}