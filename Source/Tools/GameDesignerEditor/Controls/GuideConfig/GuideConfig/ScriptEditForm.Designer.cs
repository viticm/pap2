namespace GuideConfig
{
    partial class ScriptEditForm
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
            this.luaEditorBox = new luaEditor.luaEditorControl();
            this.bOK = new DevComponents.DotNetBar.ButtonX();
            this.bCancel = new DevComponents.DotNetBar.ButtonX();
            this.SuspendLayout();
            // 
            // luaEditorBox
            // 
            this.luaEditorBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.luaEditorBox.ContentMenu = null;
            this.luaEditorBox.DebugRowIndex = -1;
            this.luaEditorBox.FunctionWordColor = System.Drawing.Color.Brown;
            this.luaEditorBox.GameObjectDescription = "npc|doodad|player|scene|item|skill";
            this.luaEditorBox.GameObjectWordColor = System.Drawing.Color.DeepSkyBlue;
            this.luaEditorBox.Location = new System.Drawing.Point(12, 12);
            this.luaEditorBox.Name = "luaEditorBox";
            this.luaEditorBox.ReadOnly = false;            
            this.luaEditorBox.ScriptForeColor = System.Drawing.SystemColors.ControlText;
            this.luaEditorBox.Size = new System.Drawing.Size(646, 430);
            this.luaEditorBox.TabIndex = 0;
            // 
            // bOK
            // 
            this.bOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.bOK.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bOK.Location = new System.Drawing.Point(503, 474);
            this.bOK.Name = "bOK";
            this.bOK.Size = new System.Drawing.Size(70, 20);
            this.bOK.TabIndex = 1;
            this.bOK.Text = "确定";
            this.bOK.Click += new System.EventHandler(this.bOK_Click);
            // 
            // bCancel
            // 
            this.bCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.bCancel.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.bCancel.Location = new System.Drawing.Point(588, 474);
            this.bCancel.Name = "bCancel";
            this.bCancel.Size = new System.Drawing.Size(70, 20);
            this.bCancel.TabIndex = 2;
            this.bCancel.Text = "取消";
            this.bCancel.Click += new System.EventHandler(this.bCancel_Click);
            // 
            // ScriptEditForm
            // 
            this.AcceptButton = this.bOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.bCancel;
            this.ClientSize = new System.Drawing.Size(670, 515);
            this.Controls.Add(this.bCancel);
            this.Controls.Add(this.bOK);
            this.Controls.Add(this.luaEditorBox);
            this.Name = "ScriptEditForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "脚本编辑";
            this.ResumeLayout(false);

        }

        #endregion

        private luaEditor.luaEditorControl luaEditorBox;
        private DevComponents.DotNetBar.ButtonX bOK;
        private DevComponents.DotNetBar.ButtonX bCancel;
    }
}