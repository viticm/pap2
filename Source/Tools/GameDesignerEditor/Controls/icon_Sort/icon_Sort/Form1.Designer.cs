namespace icon_Sort
{
    partial class Form1
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
            this.btnOpenFile = new System.Windows.Forms.Button();
            this.tbxSourcePath = new System.Windows.Forms.TextBox();
            this.btnSaveFile = new System.Windows.Forms.Button();
            this.fbdOpen = new System.Windows.Forms.FolderBrowserDialog();
            this.fbdSave = new System.Windows.Forms.FolderBrowserDialog();
            this.tbxDescPath = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.btnCopy = new System.Windows.Forms.Button();
            this.tbxCopying = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.lbCnt = new System.Windows.Forms.Label();
            this.lbFailedCnt = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // btnOpenFile
            // 
            this.btnOpenFile.Location = new System.Drawing.Point(339, 12);
            this.btnOpenFile.Name = "btnOpenFile";
            this.btnOpenFile.Size = new System.Drawing.Size(130, 23);
            this.btnOpenFile.TabIndex = 0;
            this.btnOpenFile.Text = "浏览...";
            this.btnOpenFile.UseVisualStyleBackColor = true;
            this.btnOpenFile.Click += new System.EventHandler(this.btnOpenFile_Click);
            // 
            // tbxSourcePath
            // 
            this.tbxSourcePath.Location = new System.Drawing.Point(79, 12);
            this.tbxSourcePath.Name = "tbxSourcePath";
            this.tbxSourcePath.Size = new System.Drawing.Size(243, 21);
            this.tbxSourcePath.TabIndex = 1;
            // 
            // btnSaveFile
            // 
            this.btnSaveFile.Location = new System.Drawing.Point(339, 39);
            this.btnSaveFile.Name = "btnSaveFile";
            this.btnSaveFile.Size = new System.Drawing.Size(130, 23);
            this.btnSaveFile.TabIndex = 0;
            this.btnSaveFile.Text = "浏览...";
            this.btnSaveFile.UseVisualStyleBackColor = true;
            this.btnSaveFile.Click += new System.EventHandler(this.btnSaveFile_Click);
            // 
            // tbxDescPath
            // 
            this.tbxDescPath.Location = new System.Drawing.Point(79, 39);
            this.tbxDescPath.Name = "tbxDescPath";
            this.tbxDescPath.Size = new System.Drawing.Size(243, 21);
            this.tbxDescPath.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(5, 17);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "From";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(5, 44);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(17, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "To";
            // 
            // btnCopy
            // 
            this.btnCopy.Location = new System.Drawing.Point(521, 33);
            this.btnCopy.Name = "btnCopy";
            this.btnCopy.Size = new System.Drawing.Size(130, 23);
            this.btnCopy.TabIndex = 0;
            this.btnCopy.Text = "拷贝";
            this.btnCopy.UseVisualStyleBackColor = true;
            this.btnCopy.Click += new System.EventHandler(this.btnCopy_Click);
            // 
            // tbxCopying
            // 
            this.tbxCopying.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tbxCopying.Location = new System.Drawing.Point(12, 68);
            this.tbxCopying.Multiline = true;
            this.tbxCopying.Name = "tbxCopying";
            this.tbxCopying.ReadOnly = true;
            this.tbxCopying.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.tbxCopying.Size = new System.Drawing.Size(623, 318);
            this.tbxCopying.TabIndex = 3;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(641, 81);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(65, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "拷贝计数：";
            // 
            // lbCnt
            // 
            this.lbCnt.AutoSize = true;
            this.lbCnt.Location = new System.Drawing.Point(641, 128);
            this.lbCnt.Name = "lbCnt";
            this.lbCnt.Size = new System.Drawing.Size(11, 12);
            this.lbCnt.TabIndex = 2;
            this.lbCnt.Text = "0";
            // 
            // lbFailedCnt
            // 
            this.lbFailedCnt.AutoSize = true;
            this.lbFailedCnt.Location = new System.Drawing.Point(641, 176);
            this.lbFailedCnt.Name = "lbFailedCnt";
            this.lbFailedCnt.Size = new System.Drawing.Size(11, 12);
            this.lbFailedCnt.TabIndex = 2;
            this.lbFailedCnt.Text = "0";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(641, 150);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(35, 12);
            this.label5.TabIndex = 2;
            this.label5.Text = "失败:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(641, 106);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(35, 12);
            this.label6.TabIndex = 2;
            this.label6.Text = "成功:";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(704, 386);
            this.Controls.Add(this.tbxCopying);
            this.Controls.Add(this.lbFailedCnt);
            this.Controls.Add(this.lbCnt);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.tbxDescPath);
            this.Controls.Add(this.tbxSourcePath);
            this.Controls.Add(this.btnCopy);
            this.Controls.Add(this.btnSaveFile);
            this.Controls.Add(this.btnOpenFile);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnOpenFile;
        private System.Windows.Forms.TextBox tbxSourcePath;
        private System.Windows.Forms.Button btnSaveFile;
        private System.Windows.Forms.FolderBrowserDialog fbdOpen;
        private System.Windows.Forms.FolderBrowserDialog fbdSave;
        private System.Windows.Forms.TextBox tbxDescPath;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btnCopy;
        private System.Windows.Forms.TextBox tbxCopying;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label lbCnt;
        private System.Windows.Forms.Label lbFailedCnt;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;

    }
}

