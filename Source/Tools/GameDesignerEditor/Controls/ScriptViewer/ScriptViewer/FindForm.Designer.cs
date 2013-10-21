namespace ScriptViewer
{
    partial class ScriptFindForm
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
            if (htNode2Codes != null)
            {
                htNode2Codes = null;
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
            this.tbxFindWhat = new System.Windows.Forms.TextBox();
            this.btnFindAll = new System.Windows.Forms.Button();
            this.lboxFindResult = new System.Windows.Forms.ListBox();
            this.label1 = new System.Windows.Forms.Label();
            this.ckbCaseSensitive = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // tbxFindWhat
            // 
            this.tbxFindWhat.Location = new System.Drawing.Point(83, 22);
            this.tbxFindWhat.Name = "tbxFindWhat";
            this.tbxFindWhat.Size = new System.Drawing.Size(144, 21);
            this.tbxFindWhat.TabIndex = 0;
            // 
            // btnFindAll
            // 
            this.btnFindAll.Location = new System.Drawing.Point(342, 20);
            this.btnFindAll.Name = "btnFindAll";
            this.btnFindAll.Size = new System.Drawing.Size(75, 23);
            this.btnFindAll.TabIndex = 1;
            this.btnFindAll.Text = "查找所有";
            this.btnFindAll.UseVisualStyleBackColor = true;
            this.btnFindAll.Click += new System.EventHandler(this.btnFindAll_Click);
            // 
            // lboxFindResult
            // 
            this.lboxFindResult.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lboxFindResult.FormattingEnabled = true;
            this.lboxFindResult.ItemHeight = 12;
            this.lboxFindResult.Location = new System.Drawing.Point(0, 55);
            this.lboxFindResult.Name = "lboxFindResult";
            this.lboxFindResult.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.lboxFindResult.Size = new System.Drawing.Size(429, 196);
            this.lboxFindResult.TabIndex = 2;
            this.lboxFindResult.DoubleClick += new System.EventHandler(this.lboxFindResult_DoubleClick);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(65, 12);
            this.label1.TabIndex = 3;
            this.label1.Text = "查找内容：";
            // 
            // ckbCaseSensitive
            // 
            this.ckbCaseSensitive.AutoSize = true;
            this.ckbCaseSensitive.Location = new System.Drawing.Point(249, 24);
            this.ckbCaseSensitive.Name = "ckbCaseSensitive";
            this.ckbCaseSensitive.Size = new System.Drawing.Size(84, 16);
            this.ckbCaseSensitive.TabIndex = 4;
            this.ckbCaseSensitive.Text = "区分大小写";
            this.ckbCaseSensitive.UseVisualStyleBackColor = true;
            // 
            // ScriptFindForm
            // 
            this.AcceptButton = this.btnFindAll;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(429, 255);
            this.Controls.Add(this.ckbCaseSensitive);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.lboxFindResult);
            this.Controls.Add(this.btnFindAll);
            this.Controls.Add(this.tbxFindWhat);
            this.Name = "ScriptFindForm";
            this.Text = "查找";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ScriptFindForm_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tbxFindWhat;
        private System.Windows.Forms.Button btnFindAll;
        private System.Windows.Forms.ListBox lboxFindResult;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox ckbCaseSensitive;
    }
}