namespace GameDesingerTools
{
    partial class InputBoxForm
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
            this.tbxMarks = new System.Windows.Forms.TextBox();
            this.btnOK = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.combxHistory = new System.Windows.Forms.ComboBox();
            this.btnCancle = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // tbxMarks
            // 
            this.tbxMarks.Location = new System.Drawing.Point(33, 53);
            this.tbxMarks.Multiline = true;
            this.tbxMarks.Name = "tbxMarks";
            this.tbxMarks.Size = new System.Drawing.Size(246, 129);
            this.tbxMarks.TabIndex = 0;
            // 
            // btnOK
            // 
            this.btnOK.Location = new System.Drawing.Point(136, 237);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 1;
            this.btnOK.Text = "确定(&O)";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(31, 22);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(161, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "请添加修改备注(100字以内):";
            // 
            // combxHistory
            // 
            this.combxHistory.FormattingEnabled = true;
            this.combxHistory.Location = new System.Drawing.Point(33, 188);
            this.combxHistory.Name = "combxHistory";
            this.combxHistory.Size = new System.Drawing.Size(246, 20);
            this.combxHistory.TabIndex = 3;
            this.combxHistory.SelectedIndexChanged += new System.EventHandler(this.combxHistory_SelectedIndexChanged);
            // 
            // btnCancle
            // 
            this.btnCancle.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancle.Location = new System.Drawing.Point(231, 237);
            this.btnCancle.Name = "btnCancle";
            this.btnCancle.Size = new System.Drawing.Size(75, 23);
            this.btnCancle.TabIndex = 4;
            this.btnCancle.Text = "取消(Es&c)";
            this.btnCancle.UseVisualStyleBackColor = true;
            this.btnCancle.Click += new System.EventHandler(this.btnCancle_Click);
            // 
            // InputBoxForm
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancle;
            this.ClientSize = new System.Drawing.Size(321, 273);
            this.Controls.Add(this.btnCancle);
            this.Controls.Add(this.combxHistory);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.tbxMarks);
            this.Name = "InputBoxForm";
            this.Text = "添加修改备注";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tbxMarks;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox combxHistory;
        private System.Windows.Forms.Button btnCancle;
    }
}