namespace ReadPreview
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
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.RowCnttbx = new System.Windows.Forms.TextBox();
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.ColCnttbx = new System.Windows.Forms.TextBox();
            this.OKbtn = new System.Windows.Forms.Button();
            this.Cancelbtn = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(149, 511);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(41, 12);
            this.label2.TabIndex = 9;
            this.label2.Text = "行数：";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 511);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 12);
            this.label1.TabIndex = 10;
            this.label1.Text = "列数：";
            // 
            // RowCnttbx
            // 
            this.RowCnttbx.Location = new System.Drawing.Point(202, 508);
            this.RowCnttbx.Name = "RowCnttbx";
            this.RowCnttbx.Size = new System.Drawing.Size(79, 21);
            this.RowCnttbx.TabIndex = 8;
            this.RowCnttbx.TextChanged += new System.EventHandler(this.RowCnttbx_TextChanged);
            // 
            // richTextBox1
            // 
            this.richTextBox1.Font = new System.Drawing.Font("Microsoft YaHei", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.richTextBox1.Location = new System.Drawing.Point(0, 311);
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(646, 191);
            this.richTextBox1.TabIndex = 5;
            this.richTextBox1.Text = "";
            this.richTextBox1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.richTextBox1_KeyPress);
            this.richTextBox1.TextChanged += new System.EventHandler(this.richTextBox1_TextChanged);
            // 
            // ColCnttbx
            // 
            this.ColCnttbx.Location = new System.Drawing.Point(56, 508);
            this.ColCnttbx.Name = "ColCnttbx";
            this.ColCnttbx.Size = new System.Drawing.Size(79, 21);
            this.ColCnttbx.TabIndex = 7;
            this.ColCnttbx.TextChanged += new System.EventHandler(this.ColCnttbx_TextChanged);
            // 
            // OKbtn
            // 
            this.OKbtn.Location = new System.Drawing.Point(299, 511);
            this.OKbtn.Name = "OKbtn";
            this.OKbtn.Size = new System.Drawing.Size(49, 20);
            this.OKbtn.TabIndex = 11;
            this.OKbtn.Text = "确定";
            this.OKbtn.UseVisualStyleBackColor = true;
            this.OKbtn.Click += new System.EventHandler(this.OKbtn_Click);
            // 
            // Cancelbtn
            // 
            this.Cancelbtn.Location = new System.Drawing.Point(371, 511);
            this.Cancelbtn.Name = "Cancelbtn";
            this.Cancelbtn.Size = new System.Drawing.Size(62, 20);
            this.Cancelbtn.TabIndex = 11;
            this.Cancelbtn.Text = "取消";
            this.Cancelbtn.UseVisualStyleBackColor = true;
            this.Cancelbtn.Click += new System.EventHandler(this.Cancelbtn_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(645, 532);
            this.Controls.Add(this.Cancelbtn);
            this.Controls.Add(this.OKbtn);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.RowCnttbx);
            this.Controls.Add(this.richTextBox1);
            this.Controls.Add(this.ColCnttbx);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ClientSizeChanged += new System.EventHandler(this.Form1_ClientSizeChanged);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.Form1_Paint_1);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox RowCnttbx;
        private System.Windows.Forms.RichTextBox richTextBox1;
        private System.Windows.Forms.TextBox ColCnttbx;
        private System.Windows.Forms.Button OKbtn;
        private System.Windows.Forms.Button Cancelbtn;
    }
}