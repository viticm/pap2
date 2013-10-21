namespace IconSelector
{
    partial class FormSelector
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormSelector));
            System.Windows.Forms.ListViewItem listViewItem1 = new System.Windows.Forms.ListViewItem("fff", 2);
            System.Windows.Forms.ListViewItem listViewItem2 = new System.Windows.Forms.ListViewItem("aaaa", 6);
            System.Windows.Forms.ListViewItem listViewItem3 = new System.Windows.Forms.ListViewItem("fasd", 3);
            System.Windows.Forms.ListViewItem listViewItem4 = new System.Windows.Forms.ListViewItem("dfdfdf", 10);
            System.Windows.Forms.ListViewItem listViewItem5 = new System.Windows.Forms.ListViewItem("");
            this.popList = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.imageIcons = new System.Windows.Forms.ImageList(this.components);
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.textBoxFindID = new System.Windows.Forms.TextBox();
            this.checkSoundIdOnly = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.txtSoundID = new System.Windows.Forms.ComboBox();
            this.txtTag2 = new System.Windows.Forms.ComboBox();
            this.txtTag1 = new System.Windows.Forms.ComboBox();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.panelEx2 = new DevComponents.DotNetBar.PanelEx();
            this.panelEx3 = new DevComponents.DotNetBar.PanelEx();
            this.listview = new System.Windows.Forms.ListView();
            this.splitter1 = new System.Windows.Forms.Splitter();
            this.treeView = new System.Windows.Forms.TreeView();
            this.popTree = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.popNewNode = new System.Windows.Forms.ToolStripMenuItem();
            this.popDelNode = new System.Windows.Forms.ToolStripMenuItem();
            this.popRename = new System.Windows.Forms.ToolStripMenuItem();
            this.btnXCancel = new DevComponents.DotNetBar.ButtonX();
            this.btnXOK = new DevComponents.DotNetBar.ButtonX();
            this.CD1 = new System.Windows.Forms.OpenFileDialog();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.panelEx1.SuspendLayout();
            this.panelEx2.SuspendLayout();
            this.panelEx3.SuspendLayout();
            this.popTree.SuspendLayout();
            this.SuspendLayout();
            // 
            // popList
            // 
            this.popList.Name = "popList";
            this.popList.Size = new System.Drawing.Size(61, 4);
            // 
            // imageIcons
            // 
            this.imageIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageIcons.ImageStream")));
            this.imageIcons.TransparentColor = System.Drawing.Color.Transparent;
            this.imageIcons.Images.SetKeyName(0, "asaaab11.ico");
            this.imageIcons.Images.SetKeyName(1, "3a222a233a3a22.ico");
            this.imageIcons.Images.SetKeyName(2, "3a222a233a3a222.ico");
            this.imageIcons.Images.SetKeyName(3, "3a222a233a3a.ico");
            this.imageIcons.Images.SetKeyName(4, "3aa23.ico");
            this.imageIcons.Images.SetKeyName(5, "a2.ico");
            this.imageIcons.Images.SetKeyName(6, "a2s2.ico");
            this.imageIcons.Images.SetKeyName(7, "a2s23.ico");
            this.imageIcons.Images.SetKeyName(8, "a2s23d3a.ico");
            this.imageIcons.Images.SetKeyName(9, "a2s23d3aaac33aaab.ico");
            this.imageIcons.Images.SetKeyName(10, "a2s23d3aaac33aaabdd.ico");
            this.imageIcons.Images.SetKeyName(11, "a2s23d3aaac33aab.ico");
            this.imageIcons.Images.SetKeyName(12, "a2s23d3aaac33b.ico");
            this.imageIcons.Images.SetKeyName(13, "a2s23d3aaacb.ico");
            this.imageIcons.Images.SetKeyName(14, "a2s23d3aab.ico");
            this.imageIcons.Images.SetKeyName(15, "a2s23d3ab.ico");
            this.imageIcons.Images.SetKeyName(16, "a2s.ico");
            this.imageIcons.Images.SetKeyName(17, "a.ico");
            this.imageIcons.Images.SetKeyName(18, "ab.ico");
            this.imageIcons.Images.SetKeyName(19, "abc.ico");
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.Controls.Add(this.textBoxFindID);
            this.panelEx1.Controls.Add(this.checkSoundIdOnly);
            this.panelEx1.Controls.Add(this.txtSoundID);
            this.panelEx1.Controls.Add(this.txtTag2);
            this.panelEx1.Controls.Add(this.txtTag1);
            this.panelEx1.Controls.Add(this.buttonX2);
            this.panelEx1.Controls.Add(this.buttonX1);
            this.panelEx1.Controls.Add(this.label3);
            this.panelEx1.Controls.Add(this.label2);
            this.panelEx1.Controls.Add(this.label1);
            this.panelEx1.Controls.Add(this.panelEx2);
            this.panelEx1.Controls.Add(this.btnXCancel);
            this.panelEx1.Controls.Add(this.btnXOK);
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(893, 513);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 3;
            this.panelEx1.Click += new System.EventHandler(this.panelEx1_Click);
            // 
            // textBoxFindID
            // 
            this.textBoxFindID.Location = new System.Drawing.Point(59, 452);
            this.textBoxFindID.Name = "textBoxFindID";
            this.textBoxFindID.Size = new System.Drawing.Size(78, 21);
            this.textBoxFindID.TabIndex = 16;
            this.textBoxFindID.TextChanged += new System.EventHandler(this.textBoxFindID_TextChanged);
            // 
            // checkSoundIdOnly
            // 
            this.checkSoundIdOnly.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.checkSoundIdOnly.Location = new System.Drawing.Point(764, 453);
            this.checkSoundIdOnly.Name = "checkSoundIdOnly";
            this.checkSoundIdOnly.Size = new System.Drawing.Size(119, 23);
            this.checkSoundIdOnly.TabIndex = 15;
            this.checkSoundIdOnly.Text = "隐藏已填soundid";
            this.checkSoundIdOnly.Visible = false;
            this.checkSoundIdOnly.CheckedChanged += new System.EventHandler(this.checkSoundIdOnly_CheckedChanged);
            // 
            // txtSoundID
            // 
            this.txtSoundID.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.txtSoundID.FormattingEnabled = true;
            this.txtSoundID.Location = new System.Drawing.Point(563, 452);
            this.txtSoundID.Name = "txtSoundID";
            this.txtSoundID.Size = new System.Drawing.Size(192, 20);
            this.txtSoundID.TabIndex = 14;
            this.txtSoundID.Visible = false;
            this.txtSoundID.Leave += new System.EventHandler(this.txtSoundID_Leave);
            // 
            // txtTag2
            // 
            this.txtTag2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.txtTag2.FormattingEnabled = true;
            this.txtTag2.Location = new System.Drawing.Point(371, 452);
            this.txtTag2.Name = "txtTag2";
            this.txtTag2.Size = new System.Drawing.Size(121, 20);
            this.txtTag2.TabIndex = 14;
            this.txtTag2.Visible = false;
            this.txtTag2.Leave += new System.EventHandler(this.txtTag_Leave);
            // 
            // txtTag1
            // 
            this.txtTag1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.txtTag1.FormattingEnabled = true;
            this.txtTag1.Location = new System.Drawing.Point(214, 452);
            this.txtTag1.Name = "txtTag1";
            this.txtTag1.Size = new System.Drawing.Size(121, 20);
            this.txtTag1.TabIndex = 13;
            this.txtTag1.Visible = false;
            this.txtTag1.Leave += new System.EventHandler(this.txtTag_Leave);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX1.AutoExpandOnClick = true;
            this.buttonX1.Location = new System.Drawing.Point(650, 481);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(75, 23);
            this.buttonX1.TabIndex = 14;
            this.buttonX1.Text = "导入";
            this.buttonX1.Visible = false;
            // 
            // label3
            // 
            this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(513, 456);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(47, 12);
            this.label3.TabIndex = 10;
            this.label3.Text = "soundID";
            this.label3.Visible = false;
            // 
            // label2
            // 
            this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(341, 456);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 12);
            this.label2.TabIndex = 10;
            this.label2.Text = "Tag2";
            this.label2.Visible = false;
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(184, 456);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 12);
            this.label1.TabIndex = 9;
            this.label1.Text = "Tag1";
            this.label1.Visible = false;
            // 
            // panelEx2
            // 
            this.panelEx2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panelEx2.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx2.Controls.Add(this.panelEx3);
            this.panelEx2.Controls.Add(this.splitter1);
            this.panelEx2.Controls.Add(this.treeView);
            this.panelEx2.Location = new System.Drawing.Point(3, 3);
            this.panelEx2.Name = "panelEx2";
            this.panelEx2.Size = new System.Drawing.Size(887, 441);
            this.panelEx2.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx2.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx2.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx2.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx2.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx2.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx2.Style.GradientAngle = 90;
            this.panelEx2.TabIndex = 8;
            // 
            // panelEx3
            // 
            this.panelEx3.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx3.Controls.Add(this.listview);
            this.panelEx3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx3.Location = new System.Drawing.Point(139, 0);
            this.panelEx3.Name = "panelEx3";
            this.panelEx3.Size = new System.Drawing.Size(748, 441);
            this.panelEx3.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx3.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx3.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx3.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx3.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx3.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx3.Style.GradientAngle = 90;
            this.panelEx3.TabIndex = 8;
            this.panelEx3.Text = "panelEx3";
            // 
            // listview
            // 
            this.listview.Activation = System.Windows.Forms.ItemActivation.OneClick;
            this.listview.ContextMenuStrip = this.popList;
            this.listview.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listview.HideSelection = false;
            this.listview.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem1,
            listViewItem2,
            listViewItem3,
            listViewItem4,
            listViewItem5});
            this.listview.LargeImageList = this.imageIcons;
            this.listview.Location = new System.Drawing.Point(0, 0);
            this.listview.Name = "listview";
            this.listview.Size = new System.Drawing.Size(748, 441);
            this.listview.SmallImageList = this.imageIcons;
            this.listview.TabIndex = 12;
            this.listview.UseCompatibleStateImageBehavior = false;
            this.listview.SelectedIndexChanged += new System.EventHandler(this.listview_SelectedIndexChanged);
            this.listview.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.listview_ItemDrag);
            // 
            // splitter1
            // 
            this.splitter1.Location = new System.Drawing.Point(136, 0);
            this.splitter1.Name = "splitter1";
            this.splitter1.Size = new System.Drawing.Size(3, 441);
            this.splitter1.TabIndex = 7;
            this.splitter1.TabStop = false;
            // 
            // treeView
            // 
            this.treeView.AllowDrop = true;
            this.treeView.ContextMenuStrip = this.popTree;
            this.treeView.Dock = System.Windows.Forms.DockStyle.Left;
            this.treeView.HideSelection = false;
            this.treeView.LabelEdit = true;
            this.treeView.Location = new System.Drawing.Point(0, 0);
            this.treeView.Name = "treeView";
            this.treeView.Size = new System.Drawing.Size(136, 441);
            this.treeView.TabIndex = 6;
            this.treeView.DragLeave += new System.EventHandler(this.treeView_DragLeave);
            this.treeView.AfterLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.treeView_AfterLabelEdit);
            this.treeView.DragDrop += new System.Windows.Forms.DragEventHandler(this.treeView_DragDrop);
            this.treeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView_AfterSelect);
            this.treeView.DragEnter += new System.Windows.Forms.DragEventHandler(this.treeView_DragEnter);
            this.treeView.DragOver += new System.Windows.Forms.DragEventHandler(this.treeView_DragOver);
            // 
            // popTree
            // 
            this.popTree.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.popNewNode,
            this.popDelNode,
            this.popRename});
            this.popTree.Name = "popTree";
            this.popTree.Size = new System.Drawing.Size(124, 70);
            // 
            // popNewNode
            // 
            this.popNewNode.Name = "popNewNode";
            this.popNewNode.Size = new System.Drawing.Size(123, 22);
            this.popNewNode.Text = "新建";
            this.popNewNode.Click += new System.EventHandler(this.popNewNode_Click);
            // 
            // popDelNode
            // 
            this.popDelNode.Name = "popDelNode";
            this.popDelNode.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.popDelNode.Size = new System.Drawing.Size(123, 22);
            this.popDelNode.Text = "删除";
            this.popDelNode.Click += new System.EventHandler(this.popDelNode_Click);
            // 
            // popRename
            // 
            this.popRename.Name = "popRename";
            this.popRename.ShortcutKeys = System.Windows.Forms.Keys.F2;
            this.popRename.Size = new System.Drawing.Size(123, 22);
            this.popRename.Text = "重命名";
            this.popRename.Click += new System.EventHandler(this.popRename_Click);
            // 
            // btnXCancel
            // 
            this.btnXCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnXCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnXCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnXCancel.Location = new System.Drawing.Point(815, 481);
            this.btnXCancel.Name = "btnXCancel";
            this.btnXCancel.Size = new System.Drawing.Size(66, 23);
            this.btnXCancel.TabIndex = 5;
            this.btnXCancel.Text = "取消";
            // 
            // btnXOK
            // 
            this.btnXOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnXOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnXOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnXOK.Location = new System.Drawing.Point(736, 481);
            this.btnXOK.Name = "btnXOK";
            this.btnXOK.Size = new System.Drawing.Size(73, 23);
            this.btnXOK.TabIndex = 4;
            this.btnXOK.Text = "确定";
            this.btnXOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // CD1
            // 
            this.CD1.FileName = "openFileDialog1";
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX2.AutoExpandOnClick = true;
            this.buttonX2.Location = new System.Drawing.Point(9, 452);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(46, 23);
            this.buttonX2.TabIndex = 14;
            this.buttonX2.Text = "查找→";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // FormSelector
            // 
            this.AcceptButton = this.btnXOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnXCancel;
            this.ClientSize = new System.Drawing.Size(893, 513);
            this.Controls.Add(this.panelEx1);
            this.MinimizeBox = false;
            this.Name = "FormSelector";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "图标选择工具";
            this.Load += new System.EventHandler(this.FormSelector_Load);
            this.Shown += new System.EventHandler(this.FormSelector_Shown);
            this.panelEx1.ResumeLayout(false);
            this.panelEx1.PerformLayout();
            this.panelEx2.ResumeLayout(false);
            this.panelEx3.ResumeLayout(false);
            this.popTree.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.ImageList imageIcons;
        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.ButtonX btnXOK;
        private DevComponents.DotNetBar.ButtonX btnXCancel;
        private System.Windows.Forms.ContextMenuStrip popList;
        private System.Windows.Forms.ContextMenuStrip popTree;
        private System.Windows.Forms.ToolStripMenuItem popNewNode;
        private System.Windows.Forms.ToolStripMenuItem popDelNode;
        private System.Windows.Forms.ToolStripMenuItem popRename;
        private DevComponents.DotNetBar.PanelEx panelEx2;
        private DevComponents.DotNetBar.PanelEx panelEx3;
        private System.Windows.Forms.ListView listview;
        private System.Windows.Forms.Splitter splitter1;
        private System.Windows.Forms.TreeView treeView;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private System.Windows.Forms.OpenFileDialog CD1;
        private System.Windows.Forms.ComboBox txtTag2;
        private System.Windows.Forms.ComboBox txtTag1;
        private System.Windows.Forms.ComboBox txtSoundID;
        private System.Windows.Forms.Label label3;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkSoundIdOnly;
        private System.Windows.Forms.TextBox textBoxFindID;
        private DevComponents.DotNetBar.ButtonX buttonX2;
    }
}