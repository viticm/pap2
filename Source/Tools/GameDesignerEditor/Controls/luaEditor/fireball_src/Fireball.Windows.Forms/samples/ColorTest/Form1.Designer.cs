namespace ColorTest
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
            this.colorHlsUI1 = new Fireball.Drawing.Design.ColorHlsUI();
            this.button1 = new System.Windows.Forms.Button();
            this.customButton1 = new Fireball.Windows.Forms.CustomButton();
            this.colorChooseControl1 = new Fireball.Windows.Forms.ColorChooseControl();
            this.groupPanel1 = new Fireball.Windows.Forms.GroupPanel();
            this.SuspendLayout();
            // 
            // colorHlsUI1
            // 
            this.colorHlsUI1.Location = new System.Drawing.Point(279, 325);
            this.colorHlsUI1.Name = "colorHlsUI1";
            this.colorHlsUI1.Size = new System.Drawing.Size(291, 168);
            this.colorHlsUI1.TabIndex = 3;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(620, 23);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(59, 32);
            this.button1.TabIndex = 5;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // customButton1
            // 
            this.customButton1.BackColor = System.Drawing.Color.Transparent;
            this.customButton1.Location = new System.Drawing.Point(555, 23);
            this.customButton1.Name = "customButton1";
            this.customButton1.Size = new System.Drawing.Size(59, 32);
            this.customButton1.TabIndex = 4;
            this.customButton1.Text = "Button1";
            // 
            // colorChooseControl1
            // 
            this.colorChooseControl1.BackColor = System.Drawing.SystemColors.Window;
            this.colorChooseControl1.Location = new System.Drawing.Point(23, 23);
            this.colorChooseControl1.Name = "colorChooseControl1";
            this.colorChooseControl1.Size = new System.Drawing.Size(467, 275);
            this.colorChooseControl1.TabIndex = 2;
            // 
            // groupPanel1
            // 
            this.groupPanel1.BorderColor = System.Drawing.SystemColors.ControlDark;
            this.groupPanel1.FillColor = System.Drawing.SystemColors.Window;
            this.groupPanel1.Location = new System.Drawing.Point(12, 12);
            this.groupPanel1.Name = "groupPanel1";
            this.groupPanel1.Size = new System.Drawing.Size(482, 290);
            this.groupPanel1.TabIndex = 1;
            this.groupPanel1.Text = "groupPanel1";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(730, 514);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.customButton1);
            this.Controls.Add(this.colorHlsUI1);
            this.Controls.Add(this.colorChooseControl1);
            this.Controls.Add(this.groupPanel1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);

        }

        #endregion

        private Fireball.Windows.Forms.GroupPanel groupPanel1;
        private Fireball.Windows.Forms.ColorChooseControl colorChooseControl1;
        private Fireball.Drawing.Design.ColorHlsUI colorHlsUI1;
        private Fireball.Windows.Forms.CustomButton customButton1;
        private System.Windows.Forms.Button button1;
    }
}

