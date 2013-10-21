namespace luaEditor
{
    partial class luaEditorControl
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

        #region 组件设计器生成的代码

        /// <summary> 
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            Fireball.Windows.Forms.LineMarginRender lineMarginRender1 = new Fireball.Windows.Forms.LineMarginRender();
            this.syntaxDocument = new Fireball.Syntax.SyntaxDocument(this.components);
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.cmbFunctionList = new System.Windows.Forms.ComboBox();
            this.codeEditorControl = new Fireball.Windows.Forms.CodeEditorControl();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // syntaxDocument1
            // 
            this.syntaxDocument.Lines = new string[] {
        ""};
            this.syntaxDocument.MaxUndoBufferSize = 1000;
            this.syntaxDocument.Modified = false;
            this.syntaxDocument.UndoStep = 0;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.cmbFunctionList);
            this.splitContainer1.Panel1MinSize = 20;
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.codeEditorControl);
            this.splitContainer1.Size = new System.Drawing.Size(565, 380);
            this.splitContainer1.SplitterDistance = 20;
            this.splitContainer1.SplitterWidth = 1;
            this.splitContainer1.TabIndex = 1;
            // 
            // cmbFunctionList
            // 
            this.cmbFunctionList.Dock = System.Windows.Forms.DockStyle.Top;
            this.cmbFunctionList.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbFunctionList.FormattingEnabled = true;
            this.cmbFunctionList.Location = new System.Drawing.Point(0, 0);
            this.cmbFunctionList.Name = "cmbFunctionList";
            this.cmbFunctionList.Size = new System.Drawing.Size(565, 20);
            this.cmbFunctionList.Sorted = true;
            this.cmbFunctionList.TabIndex = 0;
            this.cmbFunctionList.SelectedIndexChanged += new System.EventHandler(this.cmbFunctionList_SelectedIndexChanged);
            this.cmbFunctionList.MouseEnter += new System.EventHandler(this.cmbFunctionList_MouseEnter);
            this.cmbFunctionList.MouseLeave += new System.EventHandler(this.cmbFunctionList_MouseLeave);
            // 
            // ce1
            // 
            this.codeEditorControl.ActiveView = Fireball.Windows.Forms.CodeEditor.ActiveView.BottomRight;
            this.codeEditorControl.AutoListPosition = null;
            this.codeEditorControl.AutoListSelectedText = "a123";
            this.codeEditorControl.AutoListVisible = false;
            this.codeEditorControl.CopyAsRTF = false;
            this.codeEditorControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.codeEditorControl.Document = this.syntaxDocument;
            this.codeEditorControl.FontName = "新宋体";
            this.codeEditorControl.FontSize = 9.5F;
            this.codeEditorControl.InfoTipCount = 1;
            this.codeEditorControl.InfoTipPosition = null;
            this.codeEditorControl.InfoTipSelectedIndex = 1;
            this.codeEditorControl.InfoTipVisible = false;
            lineMarginRender1.Bounds = new System.Drawing.Rectangle(19, 0, 17, 13);
            this.codeEditorControl.LineMarginRender = lineMarginRender1;
            this.codeEditorControl.Location = new System.Drawing.Point(0, 0);
            this.codeEditorControl.LockCursorUpdate = false;
            this.codeEditorControl.Name = "ce1";
            this.codeEditorControl.RowPadding = 1;
            this.codeEditorControl.Saved = false;
            this.codeEditorControl.ShowScopeIndicator = false;
            this.codeEditorControl.Size = new System.Drawing.Size(565, 359);
            this.codeEditorControl.SmoothScroll = false;
            this.codeEditorControl.SplitviewH = -4;
            this.codeEditorControl.SplitviewV = -4;
            this.codeEditorControl.TabGuideColor = System.Drawing.Color.FromArgb(((int)(((byte)(244)))), ((int)(((byte)(243)))), ((int)(((byte)(234)))));
            this.codeEditorControl.TabIndex = 1;
            this.codeEditorControl.Text = "codeEditorControl1";
            this.codeEditorControl.WhitespaceColor = System.Drawing.SystemColors.ControlDark;
            this.codeEditorControl.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.CodeEditor_KeyPress);
            this.codeEditorControl.KeyDown += new System.Windows.Forms.KeyEventHandler(this.CodeEditor_KeyDown);
            // 
            // luaEditorControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.splitContainer1);
            this.Name = "luaEditorControl";
            this.Size = new System.Drawing.Size(565, 380);
            this.Load += new System.EventHandler(this.luaEditorControl_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private Fireball.Syntax.SyntaxDocument syntaxDocument;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ComboBox cmbFunctionList;
        private Fireball.Windows.Forms.CodeEditorControl codeEditorControl;
    }
}
