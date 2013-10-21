namespace ClientDiff
{
    partial class DiffViewForm
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
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.button1 = new System.Windows.Forms.Button();
            this.buttonClear = new System.Windows.Forms.Button();
            this.buttonReverse = new System.Windows.Forms.Button();
            this.buttonAll = new System.Windows.Forms.Button();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.buttonDisplayDiffCol = new System.Windows.Forms.Button();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            this.SuspendLayout();
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
            this.splitContainer1.Panel1.Controls.Add(this.buttonDisplayDiffCol);
            this.splitContainer1.Panel1.Controls.Add(this.button1);
            this.splitContainer1.Panel1.Controls.Add(this.buttonClear);
            this.splitContainer1.Panel1.Controls.Add(this.buttonReverse);
            this.splitContainer1.Panel1.Controls.Add(this.buttonAll);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.dataGridView1);
            this.splitContainer1.Size = new System.Drawing.Size(762, 504);
            this.splitContainer1.SplitterDistance = 105;
            this.splitContainer1.TabIndex = 0;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(255, 12);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 1;
            this.button1.Text = "保存";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // buttonClear
            // 
            this.buttonClear.Location = new System.Drawing.Point(174, 12);
            this.buttonClear.Name = "buttonClear";
            this.buttonClear.Size = new System.Drawing.Size(75, 23);
            this.buttonClear.TabIndex = 0;
            this.buttonClear.Text = "全不选 ×";
            this.buttonClear.UseVisualStyleBackColor = true;
            this.buttonClear.Click += new System.EventHandler(this.buttonClear_Click);
            // 
            // buttonReverse
            // 
            this.buttonReverse.Location = new System.Drawing.Point(93, 12);
            this.buttonReverse.Name = "buttonReverse";
            this.buttonReverse.Size = new System.Drawing.Size(75, 23);
            this.buttonReverse.TabIndex = 0;
            this.buttonReverse.Text = "反选 ∪";
            this.buttonReverse.UseVisualStyleBackColor = true;
            this.buttonReverse.Click += new System.EventHandler(this.buttonReverse_Click);
            // 
            // buttonAll
            // 
            this.buttonAll.Location = new System.Drawing.Point(12, 12);
            this.buttonAll.Name = "buttonAll";
            this.buttonAll.Size = new System.Drawing.Size(75, 23);
            this.buttonAll.TabIndex = 0;
            this.buttonAll.Text = "全选 √";
            this.buttonAll.UseVisualStyleBackColor = true;
            this.buttonAll.Click += new System.EventHandler(this.buttonAll_Click);
            // 
            // dataGridView1
            // 
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataGridView1.Location = new System.Drawing.Point(0, 0);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.RowTemplate.Height = 23;
            this.dataGridView1.Size = new System.Drawing.Size(762, 395);
            this.dataGridView1.TabIndex = 0;
            // 
            // buttonDisplayDiffCol
            // 
            this.buttonDisplayDiffCol.Location = new System.Drawing.Point(336, 12);
            this.buttonDisplayDiffCol.Name = "buttonDisplayDiffCol";
            this.buttonDisplayDiffCol.Size = new System.Drawing.Size(91, 23);
            this.buttonDisplayDiffCol.TabIndex = 1;
            this.buttonDisplayDiffCol.Text = "显示差异的列";
            this.buttonDisplayDiffCol.UseVisualStyleBackColor = true;
            this.buttonDisplayDiffCol.Click += new System.EventHandler(this.buttonDisplayDiffCol_Click);
            // 
            // DiffViewForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(762, 504);
            this.Controls.Add(this.splitContainer1);
            this.Name = "DiffViewForm";
            this.Text = "TAB 差异查看";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.Button buttonAll;
        private System.Windows.Forms.Button buttonReverse;
        private System.Windows.Forms.Button buttonClear;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button buttonDisplayDiffCol;
    }
}

