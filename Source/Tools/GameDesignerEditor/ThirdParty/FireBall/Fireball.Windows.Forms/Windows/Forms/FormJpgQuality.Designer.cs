namespace Fireball.Windows.Forms
{
    partial class FormJpgQuality
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
            this.trackBar = new System.Windows.Forms.TrackBar();
            this.txtValue = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.buttSave = new System.Windows.Forms.Button();
            this.buttCancel = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar)).BeginInit();
            this.SuspendLayout();
            // 
            // trackBar
            // 
            this.trackBar.LargeChange = 10;
            this.trackBar.Location = new System.Drawing.Point(12, 12);
            this.trackBar.Name = "trackBar";
            this.trackBar.Size = new System.Drawing.Size(332, 45);
            this.trackBar.SmallChange = 10;
            this.trackBar.TabIndex = 0;
            this.trackBar.Value = 10;
            this.trackBar.ValueChanged += new System.EventHandler(this.trackBar_ValueChanged);
            // 
            // txtValue
            // 
            this.txtValue.BackColor = System.Drawing.SystemColors.ControlLight;
            this.txtValue.Location = new System.Drawing.Point(350, 16);
            this.txtValue.Name = "txtValue";
            this.txtValue.ReadOnly = true;
            this.txtValue.Size = new System.Drawing.Size(35, 20);
            this.txtValue.TabIndex = 1;
            this.txtValue.Text = "100";
            this.txtValue.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Arial", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(388, 18);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(16, 15);
            this.label1.TabIndex = 2;
            this.label1.Text = "%";
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.ControlDark;
            this.panel1.Location = new System.Drawing.Point(12, 63);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(390, 1);
            this.panel1.TabIndex = 3;
            // 
            // buttSave
            // 
            this.buttSave.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.buttSave.Location = new System.Drawing.Point(305, 78);
            this.buttSave.Name = "buttSave";
            this.buttSave.Size = new System.Drawing.Size(80, 26);
            this.buttSave.TabIndex = 4;
            this.buttSave.Text = "Salva";
            this.buttSave.UseVisualStyleBackColor = true;
            // 
            // buttCancel
            // 
            this.buttCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttCancel.Location = new System.Drawing.Point(219, 78);
            this.buttCancel.Name = "buttCancel";
            this.buttCancel.Size = new System.Drawing.Size(80, 26);
            this.buttCancel.TabIndex = 5;
            this.buttCancel.Text = "Annulla";
            this.buttCancel.UseVisualStyleBackColor = true;
            // 
            // FormJpgQuality
            // 
            this.AcceptButton = this.buttSave;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttCancel;
            this.ClientSize = new System.Drawing.Size(415, 116);
            this.Controls.Add(this.buttCancel);
            this.Controls.Add(this.buttSave);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtValue);
            this.Controls.Add(this.trackBar);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormJpgQuality";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Jpeg quality";
            ((System.ComponentModel.ISupportInitialize)(this.trackBar)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TrackBar trackBar;
        private System.Windows.Forms.TextBox txtValue;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button buttSave;
        private System.Windows.Forms.Button buttCancel;
    }
}