namespace SymCrypt
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
            this.label1 = new System.Windows.Forms.Label();
            this.textCurrentFile = new System.Windows.Forms.TextBox();
            this.buttonEncrypt = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.textResult = new System.Windows.Forms.TextBox();
            this.buttonDecrypt = new System.Windows.Forms.Button();
            this.buttonCopy = new System.Windows.Forms.Button();
            this.buttonClose = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.textKey = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 26);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(179, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "当前文件 （拖拽文件到此窗口）";
            // 
            // textCurrentFile
            // 
            this.textCurrentFile.Location = new System.Drawing.Point(14, 41);
            this.textCurrentFile.Name = "textCurrentFile";
            this.textCurrentFile.Size = new System.Drawing.Size(543, 21);
            this.textCurrentFile.TabIndex = 1;
            // 
            // buttonEncrypt
            // 
            this.buttonEncrypt.Location = new System.Drawing.Point(13, 180);
            this.buttonEncrypt.Name = "buttonEncrypt";
            this.buttonEncrypt.Size = new System.Drawing.Size(75, 23);
            this.buttonEncrypt.TabIndex = 2;
            this.buttonEncrypt.Text = "加密";
            this.buttonEncrypt.UseVisualStyleBackColor = true;
            this.buttonEncrypt.Click += new System.EventHandler(this.buttonEncrypt_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 125);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 12);
            this.label2.TabIndex = 0;
            this.label2.Text = "输出结果";
            // 
            // textResult
            // 
            this.textResult.Location = new System.Drawing.Point(14, 140);
            this.textResult.Name = "textResult";
            this.textResult.Size = new System.Drawing.Size(543, 21);
            this.textResult.TabIndex = 1;
            // 
            // buttonDecrypt
            // 
            this.buttonDecrypt.Location = new System.Drawing.Point(94, 180);
            this.buttonDecrypt.Name = "buttonDecrypt";
            this.buttonDecrypt.Size = new System.Drawing.Size(75, 23);
            this.buttonDecrypt.TabIndex = 2;
            this.buttonDecrypt.Text = "解密";
            this.buttonDecrypt.UseVisualStyleBackColor = true;
            this.buttonDecrypt.Click += new System.EventHandler(this.buttonDecrypt_Click);
            // 
            // buttonCopy
            // 
            this.buttonCopy.Location = new System.Drawing.Point(175, 180);
            this.buttonCopy.Name = "buttonCopy";
            this.buttonCopy.Size = new System.Drawing.Size(111, 23);
            this.buttonCopy.TabIndex = 2;
            this.buttonCopy.Text = "复制结果到剪贴板";
            this.buttonCopy.UseVisualStyleBackColor = true;
            this.buttonCopy.Click += new System.EventHandler(this.buttonCopy_Click);
            // 
            // buttonClose
            // 
            this.buttonClose.Location = new System.Drawing.Point(482, 180);
            this.buttonClose.Name = "buttonClose";
            this.buttonClose.Size = new System.Drawing.Size(75, 23);
            this.buttonClose.TabIndex = 2;
            this.buttonClose.Text = "关闭";
            this.buttonClose.UseVisualStyleBackColor = true;
            this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 76);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(29, 12);
            this.label3.TabIndex = 0;
            this.label3.Text = "密钥";
            // 
            // textKey
            // 
            this.textKey.Enabled = false;
            this.textKey.Location = new System.Drawing.Point(14, 91);
            this.textKey.Name = "textKey";
            this.textKey.Size = new System.Drawing.Size(462, 21);
            this.textKey.TabIndex = 1;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(482, 89);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 2;
            this.button1.Text = "更改密钥";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // Form1
            // 
            this.AllowDrop = true;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(569, 217);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.buttonClose);
            this.Controls.Add(this.buttonCopy);
            this.Controls.Add(this.buttonDecrypt);
            this.Controls.Add(this.buttonEncrypt);
            this.Controls.Add(this.textKey);
            this.Controls.Add(this.textResult);
            this.Controls.Add(this.textCurrentFile);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.Text = "对称加解密工具";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.DragDrop += new System.Windows.Forms.DragEventHandler(this.Form1_DragDrop);
            this.DragEnter += new System.Windows.Forms.DragEventHandler(this.Form1_DragEnter);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textCurrentFile;
        private System.Windows.Forms.Button buttonEncrypt;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textResult;
        private System.Windows.Forms.Button buttonDecrypt;
        private System.Windows.Forms.Button buttonCopy;
        private System.Windows.Forms.Button buttonClose;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textKey;
        private System.Windows.Forms.Button button1;
    }
}

