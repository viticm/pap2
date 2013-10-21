namespace CustomLoadTree_CareerEvent
{
    partial class Form_JX3lib
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
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.button2 = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.contextMenuBar1 = new DevComponents.DotNetBar.ContextMenuBar();
            this.cmNodeRightClick = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemAdd = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemAddChild = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemDel = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemRename = new DevComponents.DotNetBar.ButtonItem();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).BeginInit();
            this.SuspendLayout();
            // 
            // treeView1
            // 
            this.treeView1.AllowDrop = true;
            this.treeView1.Location = new System.Drawing.Point(3, 3);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(205, 481);
            this.treeView1.TabIndex = 0;
            this.treeView1.AfterLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.treeView1_AfterLabelEdit);
            this.treeView1.DragDrop += new System.Windows.Forms.DragEventHandler(this.treeView1_DragDrop);
            this.treeView1.DragEnter += new System.Windows.Forms.DragEventHandler(this.treeView1_DragEnter);
            this.treeView1.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView1_OnNodeMouseClick);
            this.treeView1.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.treeView1_ItemDrag);
            this.treeView1.DragOver += new System.Windows.Forms.DragEventHandler(this.treeView1_DragOver);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Location = new System.Drawing.Point(12, 12);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.button2);
            this.splitContainer1.Panel1.Controls.Add(this.button1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.treeView1);
            this.splitContainer1.Size = new System.Drawing.Size(212, 547);
            this.splitContainer1.SplitterDistance = 51;
            this.splitContainer1.TabIndex = 1;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(92, 18);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(72, 21);
            this.button2.TabIndex = 1;
            this.button2.Text = "展开";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(9, 18);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(66, 22);
            this.button1.TabIndex = 0;
            this.button1.Text = "查找";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // contextMenuBar1
            // 
            this.contextMenuBar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.cmNodeRightClick});
            this.contextMenuBar1.Location = new System.Drawing.Point(276, 261);
            this.contextMenuBar1.Name = "contextMenuBar1";
            this.contextMenuBar1.Size = new System.Drawing.Size(219, 25);
            this.contextMenuBar1.Stretch = true;
            this.contextMenuBar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003;
            this.contextMenuBar1.TabIndex = 3;
            this.contextMenuBar1.TabStop = false;
            this.contextMenuBar1.Text = "contextMenuBar1";
            // 
            // cmNodeRightClick
            // 
            this.cmNodeRightClick.AutoExpandOnClick = true;
            this.cmNodeRightClick.ImagePaddingHorizontal = 8;
            this.cmNodeRightClick.Name = "cmNodeRightClick";
            this.cmNodeRightClick.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonItemAdd,
            this.buttonItemAddChild,
            this.buttonItemDel,
            this.buttonItemRename});
            this.cmNodeRightClick.Text = "cmNodeRightClick";
            // 
            // buttonItemAdd
            // 
            this.buttonItemAdd.ImagePaddingHorizontal = 8;
            this.buttonItemAdd.Name = "buttonItemAdd";
            this.buttonItemAdd.Text = "添加";
            this.buttonItemAdd.Click += new System.EventHandler(this.buttonItemAdd_Click);
            // 
            // buttonItemAddChild
            // 
            this.buttonItemAddChild.BeginGroup = true;
            this.buttonItemAddChild.ImagePaddingHorizontal = 8;
            this.buttonItemAddChild.Name = "buttonItemAddChild";
            this.buttonItemAddChild.Text = "添加子节点";
            this.buttonItemAddChild.Click += new System.EventHandler(this.buttonItemAddChild_Click);
            // 
            // buttonItemDel
            // 
            this.buttonItemDel.ImagePaddingHorizontal = 8;
            this.buttonItemDel.Name = "buttonItemDel";
            this.buttonItemDel.Text = "删除";
            this.buttonItemDel.Click += new System.EventHandler(this.buttonItemDel_Click);
            // 
            // buttonItemRename
            // 
            this.buttonItemRename.ImagePaddingHorizontal = 8;
            this.buttonItemRename.Name = "buttonItemRename";
            this.buttonItemRename.Text = "重命名";
            this.buttonItemRename.Click += new System.EventHandler(this.buttonItemRename_Click);
            // 
            // Form_JX3lib
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(655, 620);
            this.Controls.Add(this.contextMenuBar1);
            this.Controls.Add(this.splitContainer1);
            this.Name = "Form_JX3lib";
            this.Text = "Form_JX3lib";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button1;
        private DevComponents.DotNetBar.ContextMenuBar contextMenuBar1;
        private DevComponents.DotNetBar.ButtonItem cmNodeRightClick;
        private DevComponents.DotNetBar.ButtonItem buttonItemAdd;
        private DevComponents.DotNetBar.ButtonItem buttonItemAddChild;
        private DevComponents.DotNetBar.ButtonItem buttonItemDel;
        private DevComponents.DotNetBar.ButtonItem buttonItemRename;


    }
}