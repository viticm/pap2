namespace ScriptViewer
{
    partial class frmScriptEdit
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
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.luaEditorControl1 = new luaEditor.luaEditorControl();
            this.panelEx1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.Controls.Add(this.luaEditorControl1);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(675, 461);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 1;
            this.panelEx1.Text = "panelEx1";
            // 
            // luaEditorControl1
            // 
            this.luaEditorControl1.ContentMenu = null;
            this.luaEditorControl1.DebugRowIndex = -1;
            this.luaEditorControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.luaEditorControl1.FunctionWordColor = System.Drawing.Color.Brown;
            this.luaEditorControl1.GameObjectDescription = "npc|doodad|player|scene|item|skill";
            this.luaEditorControl1.GameObjectWordColor = System.Drawing.Color.DeepSkyBlue;
            this.luaEditorControl1.Location = new System.Drawing.Point(0, 0);
            this.luaEditorControl1.Name = "luaEditorControl1";
            this.luaEditorControl1.ReadOnly = false;
            this.luaEditorControl1.ScriptFont = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.luaEditorControl1.ScriptForeColor = System.Drawing.SystemColors.ControlText;
            this.luaEditorControl1.Size = new System.Drawing.Size(675, 461);
            this.luaEditorControl1.TabIndex = 0;
            this.luaEditorControl1.OnChange += new luaEditor.OnChangeHandle(this.luaEditorControl1_OnChange);
            this.luaEditorControl1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.luaEditorControl1_KeyDown);
            // 
            // frmScriptEdit
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(675, 461);
            this.Controls.Add(this.panelEx1);
            this.Name = "frmScriptEdit";
            this.Text = "frmScriptEdit";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmScriptEdit_FormClosing);
            this.panelEx1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        public luaEditor.luaEditorControl luaEditorControl1;




    }
}