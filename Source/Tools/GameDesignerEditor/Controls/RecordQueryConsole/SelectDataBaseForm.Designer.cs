namespace RecordQueryConsole
{
    partial class SelectDataBaseForm
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
            this.combxDataBases = new System.Windows.Forms.ComboBox();
            this.tbnOK = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.chkSetDefault = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // combxDataBases
            // 
            this.combxDataBases.FormattingEnabled = true;
            this.combxDataBases.Location = new System.Drawing.Point(30, 78);
            this.combxDataBases.Name = "combxDataBases";
            this.combxDataBases.Size = new System.Drawing.Size(217, 20);
            this.combxDataBases.TabIndex = 0;         
            // 
            // tbnOK
            // 
            this.tbnOK.Location = new System.Drawing.Point(172, 171);
            this.tbnOK.Name = "tbnOK";
            this.tbnOK.Size = new System.Drawing.Size(75, 23);
            this.tbnOK.TabIndex = 1;
            this.tbnOK.Text = "确定";
            this.tbnOK.UseVisualStyleBackColor = true;
            this.tbnOK.Click += new System.EventHandler(this.tbnOK_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(28, 47);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(95, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "选择数据库名称:";
            // 
            // chkSetDefault
            // 
            this.chkSetDefault.AutoSize = true;
            this.chkSetDefault.Checked = true;
            this.chkSetDefault.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkSetDefault.Location = new System.Drawing.Point(30, 171);
            this.chkSetDefault.Name = "chkSetDefault";
            this.chkSetDefault.Size = new System.Drawing.Size(108, 16);
            this.chkSetDefault.TabIndex = 3;
            this.chkSetDefault.Text = "设为默认数据库";
            this.chkSetDefault.UseVisualStyleBackColor = true;
            // 
            // SelectDataBaseForm
            // 
            this.AcceptButton = this.tbnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(261, 222);
            this.Controls.Add(this.chkSetDefault);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.tbnOK);
            this.Controls.Add(this.combxDataBases);
            this.Name = "SelectDataBaseForm";
            this.Text = "请选择数据库";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox combxDataBases;
        private System.Windows.Forms.Button tbnOK;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox chkSetDefault;
    }
}