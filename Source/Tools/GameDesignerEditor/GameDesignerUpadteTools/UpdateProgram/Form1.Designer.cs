namespace UpdateProgram
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
            //int currentProcessId = System.Diagnostics.Process.GetCurrentProcess().Id;
            //System.Diagnostics.Process process = System.Diagnostics.Process.GetProcessById(currentProcessId);

            if (disposing && (components != null))
            {
                components.Dispose();
            }
           // gM.ReleaseMutex();
            base.Dispose(disposing);

            if (forPublic)
            {
                System.Diagnostics.Process.Start("replace_public.exe", "mayflower");
            }
            else
            {
                System.Diagnostics.Process.Start("replace.exe", "mayflower");
            }            
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.smoothProgressBar1 = new System.Windows.Forms.ProgressBar();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(81, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(77, 12);
            this.label1.TabIndex = 1;
            this.label1.Text = "自动更新程序";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(154, 50);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(17, 12);
            this.label2.TabIndex = 5;
            this.label2.Text = "0%";
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // smoothProgressBar1
            // 
            this.smoothProgressBar1.ForeColor = System.Drawing.SystemColors.GradientActiveCaption;
            this.smoothProgressBar1.Location = new System.Drawing.Point(38, 74);
            this.smoothProgressBar1.Name = "smoothProgressBar1";
            this.smoothProgressBar1.Size = new System.Drawing.Size(248, 19);
            this.smoothProgressBar1.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.smoothProgressBar1.TabIndex = 6;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(325, 105);
            this.Controls.Add(this.smoothProgressBar1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "Form1";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "自动更新程序";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.ProgressBar smoothProgressBar1;
        
    }
}

