namespace DesignerSceneEditor
{
	partial class CheckRelive
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
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.BN_Ignore = new System.Windows.Forms.Button();
            this.BN_Recheck = new System.Windows.Forms.Button();
            this.CK_Run = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // treeView1
            // 
            this.treeView1.Location = new System.Drawing.Point(2, 1);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(333, 342);
            this.treeView1.TabIndex = 0;
            this.treeView1.NodeMouseDoubleClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.NodeMouseDbClick);
            // 
            // BN_Ignore
            // 
            this.BN_Ignore.Location = new System.Drawing.Point(249, 360);
            this.BN_Ignore.Name = "BN_Ignore";
            this.BN_Ignore.Size = new System.Drawing.Size(75, 23);
            this.BN_Ignore.TabIndex = 1;
            this.BN_Ignore.Text = "忽略(&C)";
            this.BN_Ignore.UseVisualStyleBackColor = true;
            this.BN_Ignore.Click += new System.EventHandler(this.BN_Ignore_Click);
            // 
            // BN_Recheck
            // 
            this.BN_Recheck.Location = new System.Drawing.Point(149, 360);
            this.BN_Recheck.Name = "BN_Recheck";
            this.BN_Recheck.Size = new System.Drawing.Size(85, 23);
            this.BN_Recheck.TabIndex = 2;
            this.BN_Recheck.Text = "重新检查(&R)";
            this.BN_Recheck.UseVisualStyleBackColor = true;
            this.BN_Recheck.Click += new System.EventHandler(this.BN_Recheck_Click);
            // 
            // CK_Run
            // 
            this.CK_Run.AutoSize = true;
            this.CK_Run.Location = new System.Drawing.Point(12, 364);
            this.CK_Run.Name = "CK_Run";
            this.CK_Run.Size = new System.Drawing.Size(120, 16);
            this.CK_Run.TabIndex = 3;
            this.CK_Run.Text = "打开、保存时运行";
            this.CK_Run.UseVisualStyleBackColor = true;
            this.CK_Run.CheckedChanged += new System.EventHandler(this.CK_Run_CheckedChanged);
            // 
            // CheckRelive
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(336, 395);
            this.Controls.Add(this.CK_Run);
            this.Controls.Add(this.BN_Recheck);
            this.Controls.Add(this.BN_Ignore);
            this.Controls.Add(this.treeView1);
            this.Name = "CheckRelive";
            this.Text = "重生组检查";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ClosingE);
            this.ResumeLayout(false);
            this.PerformLayout();

		}

		#endregion

        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.Button BN_Ignore;
        private System.Windows.Forms.Button BN_Recheck;
        private System.Windows.Forms.CheckBox CK_Run;
	}
}