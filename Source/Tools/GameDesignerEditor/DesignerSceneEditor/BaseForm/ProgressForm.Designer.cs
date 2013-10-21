namespace DesignerSceneEditor
{
    partial class ProgressForm
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
            this.BasePanel = new DevComponents.DotNetBar.TabControlPanel();
            this.progressbar1 = new DevComponents.DotNetBar.Controls.ProgressBarX();
            this.label1 = new System.Windows.Forms.Label();
            this.contextMenuBar1 = new DevComponents.DotNetBar.ContextMenuBar();
            this.bTabContext = new DevComponents.DotNetBar.ButtonItem();
            this.cmdContextSave = new DevComponents.DotNetBar.ButtonItem();
            this.cmdContextClose = new DevComponents.DotNetBar.ButtonItem();
            this.bTabColor = new DevComponents.DotNetBar.ButtonItem();
            this.cmNodeRightClick = new DevComponents.DotNetBar.ButtonItem();
            this.buttonAddSameLevel = new DevComponents.DotNetBar.ButtonItem();
            this.buttonAddSub = new DevComponents.DotNetBar.ButtonItem();
            this.buttonRename = new DevComponents.DotNetBar.ButtonItem();
            this.buttonCopy = new DevComponents.DotNetBar.ButtonItem();
            this.buttonPaste = new DevComponents.DotNetBar.ButtonItem();
            this.buttonDelete = new DevComponents.DotNetBar.ButtonItem();
            this.cmScriptOp = new DevComponents.DotNetBar.ButtonItem();
            this.bShowLuaCode = new DevComponents.DotNetBar.ButtonItem();
            this.bShowMetainfo = new DevComponents.DotNetBar.ButtonItem();
            this.bEditFile = new DevComponents.DotNetBar.ButtonItem();
            this.bDeleteRecord = new DevComponents.DotNetBar.ButtonItem();
            this.bClearContent = new DevComponents.DotNetBar.ButtonItem();
            this.BasePanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).BeginInit();
            this.SuspendLayout();
            // 
            // BasePanel
            // 
            this.BasePanel.Controls.Add(this.progressbar1);
            this.BasePanel.Controls.Add(this.label1);
            this.BasePanel.Controls.Add(this.contextMenuBar1);
            this.BasePanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.BasePanel.Location = new System.Drawing.Point(0, 0);
            this.BasePanel.Name = "BasePanel";
            this.BasePanel.Padding = new System.Windows.Forms.Padding(1);
            this.BasePanel.Size = new System.Drawing.Size(347, 121);
            this.BasePanel.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.BasePanel.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.BasePanel.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.BasePanel.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.BasePanel.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.BasePanel.Style.GradientAngle = 90;
            this.BasePanel.TabIndex = 14;
            // 
            // progressbar1
            // 
            this.progressbar1.Location = new System.Drawing.Point(12, 69);
            this.progressbar1.Name = "progressbar1";
            this.progressbar1.Size = new System.Drawing.Size(323, 23);
            this.progressbar1.TabIndex = 34;
            this.progressbar1.Text = "progressBarX1";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.Location = new System.Drawing.Point(10, 31);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 12);
            this.label1.TabIndex = 33;
            this.label1.Text = "label1";
            // 
            // contextMenuBar1
            // 
            this.contextMenuBar1.DockSide = DevComponents.DotNetBar.eDockSide.Document;
            this.contextMenuBar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bTabContext,
            this.cmNodeRightClick,
            this.cmScriptOp});
            this.contextMenuBar1.Location = new System.Drawing.Point(64, 464);
            this.contextMenuBar1.Name = "contextMenuBar1";
            this.contextMenuBar1.Size = new System.Drawing.Size(359, 25);
            this.contextMenuBar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.contextMenuBar1.TabIndex = 31;
            this.contextMenuBar1.TabStop = false;
            // 
            // bTabContext
            // 
            this.bTabContext.AutoExpandOnClick = true;
            this.bTabContext.GlobalName = "bTabContext";
            this.bTabContext.Name = "bTabContext";
            this.bTabContext.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.cmdContextSave,
            this.cmdContextClose,
            this.bTabColor});
            this.bTabContext.Text = "bTabContext";
            // 
            // cmdContextSave
            // 
            this.cmdContextSave.GlobalItem = false;
            this.cmdContextSave.ImageIndex = 17;
            this.cmdContextSave.Name = "cmdContextSave";
            this.cmdContextSave.Text = "保存";
            this.cmdContextSave.Tooltip = "保存活动文档";
            // 
            // cmdContextClose
            // 
            this.cmdContextClose.Enabled = false;
            this.cmdContextClose.GlobalItem = false;
            this.cmdContextClose.Name = "cmdContextClose";
            this.cmdContextClose.Text = "关闭";
            this.cmdContextClose.Tooltip = "关闭活动文档";
            // 
            // bTabColor
            // 
            this.bTabColor.BeginGroup = true;
            this.bTabColor.GlobalName = "bTabColor";
            this.bTabColor.Name = "bTabColor";
            this.bTabColor.PopupType = DevComponents.DotNetBar.ePopupType.Container;
            this.bTabColor.Text = "更改标签背景色";
            // 
            // cmNodeRightClick
            // 
            this.cmNodeRightClick.AutoExpandOnClick = true;
            this.cmNodeRightClick.Name = "cmNodeRightClick";
            this.cmNodeRightClick.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonAddSameLevel,
            this.buttonAddSub,
            this.buttonRename,
            this.buttonCopy,
            this.buttonPaste,
            this.buttonDelete});
            this.cmNodeRightClick.Text = "cmNodeRightClick";
            // 
            // buttonAddSameLevel
            // 
            this.buttonAddSameLevel.Name = "buttonAddSameLevel";
            this.buttonAddSameLevel.Text = "添加本层";
            // 
            // buttonAddSub
            // 
            this.buttonAddSub.Name = "buttonAddSub";
            this.buttonAddSub.Text = "添加子层";
            // 
            // buttonRename
            // 
            this.buttonRename.BeginGroup = true;
            this.buttonRename.Name = "buttonRename";
            this.buttonRename.Text = "重命名";
            // 
            // buttonCopy
            // 
            this.buttonCopy.Name = "buttonCopy";
            this.buttonCopy.Text = "复制";
            // 
            // buttonPaste
            // 
            this.buttonPaste.Name = "buttonPaste";
            this.buttonPaste.Text = "粘贴";
            // 
            // buttonDelete
            // 
            this.buttonDelete.BeginGroup = true;
            this.buttonDelete.Name = "buttonDelete";
            this.buttonDelete.Text = "删除";
            // 
            // cmScriptOp
            // 
            this.cmScriptOp.AutoExpandOnClick = true;
            this.cmScriptOp.Name = "cmScriptOp";
            this.cmScriptOp.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bShowLuaCode,
            this.bShowMetainfo,
            this.bEditFile,
            this.bDeleteRecord,
            this.bClearContent});
            this.cmScriptOp.Text = "cmScriptOp";
            // 
            // bShowLuaCode
            // 
            this.bShowLuaCode.Name = "bShowLuaCode";
            this.bShowLuaCode.Text = "显示代码(&S)";
            // 
            // bShowMetainfo
            // 
            this.bShowMetainfo.BeginGroup = true;
            this.bShowMetainfo.Name = "bShowMetainfo";
            this.bShowMetainfo.Text = "编辑元信息(&X)";
            // 
            // bEditFile
            // 
            this.bEditFile.Name = "bEditFile";
            this.bEditFile.Text = "编辑文件(&E)";
            // 
            // bDeleteRecord
            // 
            this.bDeleteRecord.BeginGroup = true;
            this.bDeleteRecord.Name = "bDeleteRecord";
            this.bDeleteRecord.Text = "删除从表记录(&D)";
            // 
            // bClearContent
            // 
            this.bClearContent.Name = "bClearContent";
            this.bClearContent.Text = "清空";
            // 
            // ProgressForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(347, 121);
            this.Controls.Add(this.BasePanel);
            this.Name = "ProgressForm";
            this.Text = "ProgressForm";
            this.BasePanel.ResumeLayout(false);
            this.BasePanel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.TabControlPanel BasePanel;
        private DevComponents.DotNetBar.ContextMenuBar contextMenuBar1;
        private DevComponents.DotNetBar.ButtonItem bTabContext;
        private DevComponents.DotNetBar.ButtonItem cmdContextSave;
        private DevComponents.DotNetBar.ButtonItem cmdContextClose;
        private DevComponents.DotNetBar.ButtonItem bTabColor;
        private DevComponents.DotNetBar.ButtonItem cmNodeRightClick;
        private DevComponents.DotNetBar.ButtonItem buttonAddSameLevel;
        private DevComponents.DotNetBar.ButtonItem buttonAddSub;
        private DevComponents.DotNetBar.ButtonItem buttonRename;
        private DevComponents.DotNetBar.ButtonItem buttonCopy;
        private DevComponents.DotNetBar.ButtonItem buttonPaste;
        private DevComponents.DotNetBar.ButtonItem buttonDelete;
        private DevComponents.DotNetBar.ButtonItem cmScriptOp;
        private DevComponents.DotNetBar.ButtonItem bShowLuaCode;
        private DevComponents.DotNetBar.ButtonItem bShowMetainfo;
        private DevComponents.DotNetBar.ButtonItem bEditFile;
        private DevComponents.DotNetBar.ButtonItem bDeleteRecord;
        private DevComponents.DotNetBar.ButtonItem bClearContent;
        private DevComponents.DotNetBar.Controls.ProgressBarX progressbar1;
        private System.Windows.Forms.Label label1;
    }
}