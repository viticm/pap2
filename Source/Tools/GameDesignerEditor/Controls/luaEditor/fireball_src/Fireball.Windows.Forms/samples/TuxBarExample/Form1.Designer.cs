namespace TuxBarExample
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
            Fireball.Windows.Forms.TuxBar.Themes.FireTheme fireTheme2 = new Fireball.Windows.Forms.TuxBar.Themes.FireTheme();
            this.button1 = new System.Windows.Forms.Button();
            this.tuxBarContainer1 = new Fireball.Windows.Forms.TuxBar.TuxBarContainer();
            this.button2 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(388, 12);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(169, 38);
            this.button1.TabIndex = 1;
            this.button1.Text = "Add";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // tuxBarContainer1
            // 
            this.tuxBarContainer1.AutoScroll = true;
            this.tuxBarContainer1.AutoScrollMinSize = new System.Drawing.Size(10, 10);
            this.tuxBarContainer1.BorderColor = System.Drawing.SystemColors.ControlDark;
            this.tuxBarContainer1.Location = new System.Drawing.Point(12, 12);
            this.tuxBarContainer1.Name = "tuxBarContainer1";
            this.tuxBarContainer1.Padding = new System.Windows.Forms.Padding(2);
            this.tuxBarContainer1.ShowBorder = true;
            this.tuxBarContainer1.Size = new System.Drawing.Size(346, 518);
            this.tuxBarContainer1.TabIndex = 0;
            this.tuxBarContainer1.Theme = fireTheme2;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(388, 56);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(169, 38);
            this.button2.TabIndex = 2;
            this.button2.Text = "Remove";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(675, 557);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.tuxBarContainer1);
            this.Name = "Form1";
            this.Text = "TuxBar Example";
            this.ResumeLayout(false);

        }

        #endregion

        private Fireball.Windows.Forms.TuxBar.TuxBarContainer tuxBarContainer1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
    }
}

