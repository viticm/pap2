namespace MakeIDL
{
    partial class MakeIDLForm
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
            this.lbxInterfaces = new System.Windows.Forms.ListBox();
            this.tbxFileName = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.btnBrowse = new System.Windows.Forms.Button();
            this.btnMake = new System.Windows.Forms.Button();
            this.btnFinish = new System.Windows.Forms.Button();
            this.lbTip = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.lbMakeInfo = new System.Windows.Forms.Label();
            this.btnChooseIDL = new System.Windows.Forms.Button();
            this.tbxClass = new System.Windows.Forms.TextBox();
            this.btnReset = new System.Windows.Forms.Button();
            this.tbxRecords = new System.Windows.Forms.TextBox();
            this.btnSaveIdl = new System.Windows.Forms.Button();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.btnQuit = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // lbxInterfaces
            // 
            this.lbxInterfaces.FormattingEnabled = true;
            this.lbxInterfaces.ItemHeight = 12;
            this.lbxInterfaces.Location = new System.Drawing.Point(3, 28);
            this.lbxInterfaces.Name = "lbxInterfaces";
            this.lbxInterfaces.Size = new System.Drawing.Size(184, 412);
            this.lbxInterfaces.TabIndex = 0;
            this.lbxInterfaces.SelectedIndexChanged += new System.EventHandler(this.lbxInterfaces_SelectedIndexChanged);
            // 
            // tbxFileName
            // 
            this.tbxFileName.Location = new System.Drawing.Point(424, 68);
            this.tbxFileName.Name = "tbxFileName";
            this.tbxFileName.Size = new System.Drawing.Size(132, 21);
            this.tbxFileName.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(374, 71);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(47, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "in file";
            // 
            // btnBrowse
            // 
            this.btnBrowse.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnBrowse.Location = new System.Drawing.Point(561, 66);
            this.btnBrowse.Name = "btnBrowse";
            this.btnBrowse.Size = new System.Drawing.Size(65, 23);
            this.btnBrowse.TabIndex = 3;
            this.btnBrowse.Text = "选择文件";
            this.btnBrowse.UseVisualStyleBackColor = true;
            this.btnBrowse.Click += new System.EventHandler(this.btnBrowse_Click);
            // 
            // btnMake
            // 
            this.btnMake.Enabled = false;
            this.btnMake.Location = new System.Drawing.Point(634, 172);
            this.btnMake.Name = "btnMake";
            this.btnMake.Size = new System.Drawing.Size(75, 23);
            this.btnMake.TabIndex = 3;
            this.btnMake.Text = "生成";
            this.btnMake.UseVisualStyleBackColor = true;
            this.btnMake.Click += new System.EventHandler(this.btnMake_Click);
            // 
            // btnFinish
            // 
            this.btnFinish.Location = new System.Drawing.Point(634, 66);
            this.btnFinish.Name = "btnFinish";
            this.btnFinish.Size = new System.Drawing.Size(65, 23);
            this.btnFinish.TabIndex = 4;
            this.btnFinish.Text = "完毕";
            this.btnFinish.UseVisualStyleBackColor = true;
            this.btnFinish.Click += new System.EventHandler(this.btnFinish_Click);
            // 
            // lbTip
            // 
            this.lbTip.AutoSize = true;
            this.lbTip.Location = new System.Drawing.Point(209, 43);
            this.lbTip.Name = "lbTip";
            this.lbTip.Size = new System.Drawing.Size(0, 12);
            this.lbTip.TabIndex = 4;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 12);
            this.label2.TabIndex = 4;
            this.label2.Text = "接口列表";
            // 
            // lbMakeInfo
            // 
            this.lbMakeInfo.AutoSize = true;
            this.lbMakeInfo.Location = new System.Drawing.Point(199, 183);
            this.lbMakeInfo.Name = "lbMakeInfo";
            this.lbMakeInfo.Size = new System.Drawing.Size(53, 12);
            this.lbMakeInfo.TabIndex = 4;
            this.lbMakeInfo.Text = "接口列表";
            // 
            // btnChooseIDL
            // 
            this.btnChooseIDL.Location = new System.Drawing.Point(92, 4);
            this.btnChooseIDL.Name = "btnChooseIDL";
            this.btnChooseIDL.Size = new System.Drawing.Size(84, 23);
            this.btnChooseIDL.TabIndex = 5;
            this.btnChooseIDL.Text = "选择IDL文件";
            this.btnChooseIDL.UseVisualStyleBackColor = true;
            this.btnChooseIDL.Click += new System.EventHandler(this.btnChooseIDL_Click);
            // 
            // tbxClass
            // 
            this.tbxClass.Location = new System.Drawing.Point(199, 68);
            this.tbxClass.Name = "tbxClass";
            this.tbxClass.Size = new System.Drawing.Size(169, 21);
            this.tbxClass.TabIndex = 1;
            // 
            // btnReset
            // 
            this.btnReset.Location = new System.Drawing.Point(199, 124);
            this.btnReset.Name = "btnReset";
            this.btnReset.Size = new System.Drawing.Size(65, 23);
            this.btnReset.TabIndex = 8;
            this.btnReset.Text = "重置";
            this.btnReset.UseVisualStyleBackColor = true;
            this.btnReset.Click += new System.EventHandler(this.btnReset_Click);
            // 
            // tbxRecords
            // 
            this.tbxRecords.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbxRecords.Location = new System.Drawing.Point(199, 215);
            this.tbxRecords.Multiline = true;
            this.tbxRecords.Name = "tbxRecords";
            this.tbxRecords.ReadOnly = true;
            this.tbxRecords.Size = new System.Drawing.Size(388, 200);
            this.tbxRecords.TabIndex = 10;
            this.tbxRecords.Text = "操作记录.....\r\n";
            // 
            // btnSaveIdl
            // 
            this.btnSaveIdl.Location = new System.Drawing.Point(634, 210);
            this.btnSaveIdl.Name = "btnSaveIdl";
            this.btnSaveIdl.Size = new System.Drawing.Size(75, 23);
            this.btnSaveIdl.TabIndex = 3;
            this.btnSaveIdl.Text = "另存为";
            this.btnSaveIdl.UseVisualStyleBackColor = true;
            this.btnSaveIdl.Visible = false;
            this.btnSaveIdl.Click += new System.EventHandler(this.btnSaveIdl_Click);
            // 
            // saveFileDialog1
            // 
            this.saveFileDialog1.Filter = "idl (*.idl)|*.idl|All|*.*";
            // 
            // btnQuit
            // 
            this.btnQuit.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnQuit.Location = new System.Drawing.Point(634, 402);
            this.btnQuit.Name = "btnQuit";
            this.btnQuit.Size = new System.Drawing.Size(75, 23);
            this.btnQuit.TabIndex = 11;
            this.btnQuit.Text = "退出";
            this.btnQuit.UseVisualStyleBackColor = true;
            this.btnQuit.Click += new System.EventHandler(this.btnQuit_Click);
            // 
            // MakeIDLForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnQuit;
            this.ClientSize = new System.Drawing.Size(772, 452);
            this.Controls.Add(this.btnQuit);
            this.Controls.Add(this.tbxRecords);
            this.Controls.Add(this.btnReset);
            this.Controls.Add(this.btnChooseIDL);
            this.Controls.Add(this.lbMakeInfo);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.lbTip);
            this.Controls.Add(this.btnSaveIdl);
            this.Controls.Add(this.btnMake);
            this.Controls.Add(this.btnFinish);
            this.Controls.Add(this.btnBrowse);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.tbxFileName);
            this.Controls.Add(this.tbxClass);
            this.Controls.Add(this.lbxInterfaces);
            this.Name = "MakeIDLForm";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox lbxInterfaces;
        private System.Windows.Forms.TextBox tbxClass;
        private System.Windows.Forms.TextBox tbxFileName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnBrowse;
        private System.Windows.Forms.Button btnMake;
        private System.Windows.Forms.Button btnFinish;
        private System.Windows.Forms.Label lbTip;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label lbMakeInfo;

        private System.Windows.Forms.Button btnChooseIDL;
        private System.Windows.Forms.Button btnReset;
        private System.Windows.Forms.TextBox tbxRecords;
        private System.Windows.Forms.Button btnSaveIdl;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.Button btnQuit;
    }
}

