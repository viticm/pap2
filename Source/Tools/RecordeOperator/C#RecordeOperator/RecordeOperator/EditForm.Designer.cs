namespace RecordeOperator
{
    partial class EditForm
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
            this.EventOK = new System.Windows.Forms.Button();
            this.ListBox = new System.Windows.Forms.ListBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.Cancel = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // EventOK
            // 
            this.EventOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.EventOK.Location = new System.Drawing.Point(53, 411);
            this.EventOK.Name = "EventOK";
            this.EventOK.Size = new System.Drawing.Size(70, 30);
            this.EventOK.TabIndex = 2;
            this.EventOK.Text = "确定";
            this.EventOK.UseVisualStyleBackColor = true;
            this.EventOK.Click += new System.EventHandler(this.EventOK_Click);
            // 
            // ListBox
            // 
            this.ListBox.FormattingEnabled = true;
            this.ListBox.ItemHeight = 12;
            this.ListBox.Location = new System.Drawing.Point(12, 12);
            this.ListBox.Name = "ListBox";
            this.ListBox.Size = new System.Drawing.Size(273, 136);
            this.ListBox.TabIndex = 6;
            this.ListBox.SelectedIndexChanged += new System.EventHandler(this.ListBox_SelectedIndexChanged);
            // 
            // panel1
            // 
            this.panel1.Location = new System.Drawing.Point(12, 160);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(273, 215);
            this.panel1.TabIndex = 5;
            // 
            // Cancel
            // 
            this.Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.Cancel.Location = new System.Drawing.Point(169, 411);
            this.Cancel.Name = "Cancel";
            this.Cancel.Size = new System.Drawing.Size(70, 30);
            this.Cancel.TabIndex = 7;
            this.Cancel.Text = "取消";
            this.Cancel.UseVisualStyleBackColor = true;
            this.Cancel.Click += new System.EventHandler(this.Cancel_Click);
            // 
            // EditForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(297, 457);
            this.Controls.Add(this.Cancel);
            this.Controls.Add(this.ListBox);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.EventOK);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "EditForm";
            this.ShowIcon = false;
            this.Text = "EditForm";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button EventOK;
        private System.Windows.Forms.ListBox ListBox;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button Cancel;
    }
}