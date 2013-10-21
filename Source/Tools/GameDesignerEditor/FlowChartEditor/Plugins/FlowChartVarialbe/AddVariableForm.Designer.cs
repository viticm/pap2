namespace FlowChartVarialbe
{
    partial class AddVariableForm
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
            this.lbTip = new System.Windows.Forms.Label();
            this.tbxVariable = new System.Windows.Forms.TextBox();
            this.btnCancle = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // lbTip
            // 
            this.lbTip.AutoSize = true;
            this.lbTip.Location = new System.Drawing.Point(30, 22);
            this.lbTip.Name = "lbTip";
            this.lbTip.Size = new System.Drawing.Size(113, 12);
            this.lbTip.TabIndex = 0;
            this.lbTip.Text = "请输入新的变量名：";
            // 
            // tbxVariable
            // 
            this.tbxVariable.Location = new System.Drawing.Point(32, 54);
            this.tbxVariable.Name = "tbxVariable";
            this.tbxVariable.Size = new System.Drawing.Size(148, 21);
            this.tbxVariable.TabIndex = 1;
            // 
            // btnCancle
            // 
            this.btnCancle.Location = new System.Drawing.Point(128, 107);
            this.btnCancle.Name = "btnCancle";
            this.btnCancle.Size = new System.Drawing.Size(52, 23);
            this.btnCancle.TabIndex = 3;
            this.btnCancle.Text = "取消";
            this.btnCancle.UseVisualStyleBackColor = true;
            this.btnCancle.Click += new System.EventHandler(this.btnCancle_Click);
            // 
            // btnOK
            // 
            this.btnOK.Location = new System.Drawing.Point(32, 107);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(52, 23);
            this.btnOK.TabIndex = 2;
            this.btnOK.Text = "确定";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // AddVariableForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(233, 155);
            this.Controls.Add(this.btnCancle);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.tbxVariable);
            this.Controls.Add(this.lbTip);
            this.Name = "AddVariableForm";
            this.Text = "AddVariableForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lbTip;
        private System.Windows.Forms.TextBox tbxVariable;
        private System.Windows.Forms.Button btnCancle;
        private System.Windows.Forms.Button btnOK;
    }
}