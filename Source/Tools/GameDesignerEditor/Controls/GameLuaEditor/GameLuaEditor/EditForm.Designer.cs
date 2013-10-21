namespace GameLuaEditor
{
    partial class EditForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.luaEditBox = new luaEditor.luaEditorControl();
            this.SuspendLayout();
            // 
            // luaEditBox
            // 
            this.luaEditBox.ContentMenu = null;
            this.luaEditBox.DebugRowIndex = -1;
            this.luaEditBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.luaEditBox.Location = new System.Drawing.Point(0, 0);
            this.luaEditBox.Name = "luaEditBox";
            this.luaEditBox.ReadOnly = false;
            this.luaEditBox.Size = new System.Drawing.Size(679, 504);
            this.luaEditBox.TabIndex = 0;
            this.luaEditBox.queryObjects += new luaEditor.QueryObjectHandle(this.luaEdit1_queryObjects);
            this.luaEditBox.OnBreakPointRemoved += new luaEditor.BreakPointRemoved(this.luaEditBox_OnBreakPointRemoved);
            this.luaEditBox.OnChange += new luaEditor.OnChangeHandle(this.luaEdit1_OnChange);
            this.luaEditBox.OnWordMouseHover += new luaEditor.WordMouseHover(this.luaEditBox_OnWordMouseHover);
            this.luaEditBox.OnBreakPointAdded += new luaEditor.BreakPointAdded(this.luaEditBox_OnBreakPointAdded);
            // 
            // frmEdit
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(679, 504);
            this.ControlBox = false;
            this.Controls.Add(this.luaEditBox);
            this.MinimizeBox = false;
            this.Name = "frmEdit";
            this.Text = "frmEdit";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmEdit_FormClosing);
            this.ResumeLayout(false);

        }

        #endregion

        public luaEditor.luaEditorControl luaEditBox;
    }
}