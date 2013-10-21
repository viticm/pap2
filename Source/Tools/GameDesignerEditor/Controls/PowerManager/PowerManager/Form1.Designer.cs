namespace PowerManager
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
            this.btnQurey = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.btnGrant = new System.Windows.Forms.Button();
            this.btnRevoke = new System.Windows.Forms.Button();
            this.combxHostName = new System.Windows.Forms.ComboBox();
            this.lbQureyResult = new System.Windows.Forms.Label();
            this.btnQuit = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnQurey
            // 
            this.btnQurey.Location = new System.Drawing.Point(275, 57);
            this.btnQurey.Name = "btnQurey";
            this.btnQurey.Size = new System.Drawing.Size(75, 23);
            this.btnQurey.TabIndex = 1;
            this.btnQurey.Text = "查询";
            this.btnQurey.UseVisualStyleBackColor = true;
            this.btnQurey.Click += new System.EventHandler(this.btnQurey_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(16, 61);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(101, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "完整的主机名称：";
            // 
            // btnGrant
            // 
            this.btnGrant.Enabled = false;
            this.btnGrant.Location = new System.Drawing.Point(393, 39);
            this.btnGrant.Name = "btnGrant";
            this.btnGrant.Size = new System.Drawing.Size(75, 23);
            this.btnGrant.TabIndex = 1;
            this.btnGrant.Text = "开通权限";
            this.btnGrant.UseVisualStyleBackColor = true;
            this.btnGrant.Click += new System.EventHandler(this.btnGrant_Click);
            // 
            // btnRevoke
            // 
            this.btnRevoke.Enabled = false;
            this.btnRevoke.Location = new System.Drawing.Point(393, 81);
            this.btnRevoke.Name = "btnRevoke";
            this.btnRevoke.Size = new System.Drawing.Size(75, 23);
            this.btnRevoke.TabIndex = 1;
            this.btnRevoke.Text = "关闭权限";
            this.btnRevoke.UseVisualStyleBackColor = true;
            this.btnRevoke.Click += new System.EventHandler(this.btnRevoke_Click);
            // 
            // combxHostName
            // 
            this.combxHostName.FormattingEnabled = true;
            this.combxHostName.Location = new System.Drawing.Point(133, 57);
            this.combxHostName.Name = "combxHostName";
            this.combxHostName.Size = new System.Drawing.Size(121, 20);
            this.combxHostName.TabIndex = 3;
            this.combxHostName.SelectedIndexChanged += new System.EventHandler(this.combxHostName_SelectedIndexChanged);
            // 
            // lbQureyResult
            // 
            this.lbQureyResult.AutoSize = true;
            this.lbQureyResult.Location = new System.Drawing.Point(16, 117);
            this.lbQureyResult.Name = "lbQureyResult";
            this.lbQureyResult.Size = new System.Drawing.Size(59, 12);
            this.lbQureyResult.TabIndex = 2;
            this.lbQureyResult.Text = "查询结果:";
            // 
            // btnQuit
            // 
            this.btnQuit.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnQuit.Location = new System.Drawing.Point(393, 179);
            this.btnQuit.Name = "btnQuit";
            this.btnQuit.Size = new System.Drawing.Size(75, 23);
            this.btnQuit.TabIndex = 1;
            this.btnQuit.Text = "退出";
            this.btnQuit.UseVisualStyleBackColor = true;
            this.btnQuit.Click += new System.EventHandler(this.btnQuit_Click);
            // 
            // Form1
            // 
            this.AcceptButton = this.btnQurey;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnQuit;
            this.ClientSize = new System.Drawing.Size(500, 232);
            this.Controls.Add(this.combxHostName);
            this.Controls.Add(this.lbQureyResult);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnRevoke);
            this.Controls.Add(this.btnGrant);
            this.Controls.Add(this.btnQuit);
            this.Controls.Add(this.btnQurey);
            this.Name = "Form1";
            this.Text = "策划工具分支库(1-0-8-1380)访问权限控制";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnQurey;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnGrant;
        private System.Windows.Forms.Button btnRevoke;
        private System.Windows.Forms.ComboBox combxHostName;
        private System.Windows.Forms.Label lbQureyResult;
        private System.Windows.Forms.Button btnQuit;
    }
}

