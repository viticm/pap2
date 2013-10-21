namespace CustomButtonTest
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
            this.customButton1 = new Fireball.Windows.Forms.CustomButton();
            this.customButton2 = new Fireball.Windows.Forms.CustomButton();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.customButton3 = new Fireball.Windows.Forms.CustomButton();
            this.label3 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // customButton1
            // 
            this.customButton1.BackColor = System.Drawing.Color.Transparent;
            this.customButton1.Location = new System.Drawing.Point(12, 12);
            this.customButton1.Name = "customButton1";
            this.customButton1.Size = new System.Drawing.Size(80, 32);
            this.customButton1.TabIndex = 0;
            this.customButton1.Text = "Button1";
            // 
            // customButton2
            // 
            this.customButton2.BackColor = System.Drawing.Color.Transparent;
            this.customButton2.Location = new System.Drawing.Point(12, 50);
            this.customButton2.Name = "customButton2";
            this.customButton2.Size = new System.Drawing.Size(80, 32);
            this.customButton2.TabIndex = 1;
            this.customButton2.Text = "Button2";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(98, 21);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(158, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Custom button with Default style";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(98, 59);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(148, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Custom button with Base style";
            // 
            // customButton3
            // 
            this.customButton3.BackColor = System.Drawing.Color.Transparent;
            this.customButton3.Location = new System.Drawing.Point(12, 88);
            this.customButton3.Name = "customButton3";
            this.customButton3.Size = new System.Drawing.Size(80, 32);
            this.customButton3.TabIndex = 4;
            this.customButton3.Text = "Button3";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(98, 98);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(211, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "Custom button with Custom Advanced style";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(580, 266);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.customButton3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.customButton2);
            this.Controls.Add(this.customButton1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private Fireball.Windows.Forms.CustomButton customButton1;
        private Fireball.Windows.Forms.CustomButton customButton2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private Fireball.Windows.Forms.CustomButton customButton3;
        private System.Windows.Forms.Label label3;

    }
}

