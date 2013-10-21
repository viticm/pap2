namespace ChooseItem
{
    partial class OperateTree
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
            this.components = new System.ComponentModel.Container();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.multipleAddToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.computeTotalToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.multipleAddToolStripMenuItem,
            this.computeTotalToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(153, 70);
            // 
            // multipleAddToolStripMenuItem
            // 
            this.multipleAddToolStripMenuItem.Name = "multipleAddToolStripMenuItem";
            this.multipleAddToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.multipleAddToolStripMenuItem.Text = "批量添加";
            this.multipleAddToolStripMenuItem.Click += new System.EventHandler(this.multipleAddToolStripMenuItem_Click);
            // 
            // computeTotalToolStripMenuItem
            // 
            this.computeTotalToolStripMenuItem.Name = "computeTotalToolStripMenuItem";
            this.computeTotalToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.computeTotalToolStripMenuItem.Text = "计算掉落总值";
            this.computeTotalToolStripMenuItem.Click += new System.EventHandler(this.computeTotalToolStripMenuItem_Click);
            // 
            // treeView1
            // 
            this.treeView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeView1.HideSelection = false;
            this.treeView1.HotTracking = true;
            this.treeView1.Location = new System.Drawing.Point(0, 0);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(292, 266);
            this.treeView1.TabIndex = 1;
            // 
            // OperateTree
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.Controls.Add(this.treeView1);
            this.Name = "OperateTree";
            this.Text = "OperateTree";
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem multipleAddToolStripMenuItem;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.ToolStripMenuItem computeTotalToolStripMenuItem;
    }
}