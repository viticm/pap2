namespace GameDesingerTools
{
    partial class DBTableEditor
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DBTableEditor));
            this.tree = new System.Windows.Forms.TreeView();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.groupField = new System.Windows.Forms.GroupBox();
            this.textBox_Xiaoshu_len = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.label5 = new System.Windows.Forms.Label();
            this.textBox_Zhengshu_len = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.label4 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_data_len = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.label1 = new System.Windows.Forms.Label();
            this.combo_data_type = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.type_decimal = new DevComponents.Editors.ComboItem();
            this.type_string = new DevComponents.Editors.ComboItem();
            this.type_bool = new DevComponents.Editors.ComboItem();
            this.type_int = new DevComponents.Editors.ComboItem();
            this.groupTable = new System.Windows.Forms.GroupBox();
            this.bConfigExcludeField = new DevComponents.DotNetBar.ButtonX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.textBox_Exclude_Field = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.label6 = new System.Windows.Forms.Label();
            this.comboExportRule = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.comboItem1 = new DevComponents.Editors.ComboItem();
            this.comboItem2 = new DevComponents.Editors.ComboItem();
            this.checkHasHead = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkBrowseFolder = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkHasDefault = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.label3 = new System.Windows.Forms.Label();
            this.textBox_rela_path = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.contextMenuBar1 = new DevComponents.DotNetBar.ContextMenuBar();
            this.cTblNodeClick = new DevComponents.DotNetBar.ButtonItem();
            this.bImportTbl = new DevComponents.DotNetBar.ButtonItem();
            this.bRenameTbl = new DevComponents.DotNetBar.ButtonItem();
            this.bNewTbl = new DevComponents.DotNetBar.ButtonItem();
            this.bDelTbl = new DevComponents.DotNetBar.ButtonItem();
            this.bEmptyTbl = new DevComponents.DotNetBar.ButtonItem();
            this.bNewField = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem1 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItem2 = new DevComponents.DotNetBar.ButtonItem();
            this.buttonItemModColOrder = new DevComponents.DotNetBar.ButtonItem();
            this.cFieldNodeClick = new DevComponents.DotNetBar.ButtonItem();
            this.bRenameField = new DevComponents.DotNetBar.ButtonItem();
            this.bDelField = new DevComponents.DotNetBar.ButtonItem();
            this.bSetKey = new DevComponents.DotNetBar.ButtonItem();
            this.bUnsetKey = new DevComponents.DotNetBar.ButtonItem();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.UpdateTab_button = new DevComponents.DotNetBar.ButtonX();
            this.bool_true = new DevComponents.Editors.ComboItem();
            this.bool_false = new DevComponents.Editors.ComboItem();
            this.tabControlPanel1 = new DevComponents.DotNetBar.TabControlPanel();
            this.splitter1 = new System.Windows.Forms.Splitter();
            this.groupField.SuspendLayout();
            this.groupTable.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).BeginInit();
            this.tabControlPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tree
            // 
            this.tree.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tree.Dock = System.Windows.Forms.DockStyle.Left;
            this.tree.HideSelection = false;
            this.tree.ImageIndex = 0;
            this.tree.ImageList = this.imageList1;
            this.tree.ItemHeight = 18;
            this.tree.Location = new System.Drawing.Point(0, 0);
            this.tree.Margin = new System.Windows.Forms.Padding(3, 3, 3, 6);
            this.tree.Name = "tree";
            this.tree.SelectedImageIndex = 0;
            this.tree.Size = new System.Drawing.Size(242, 542);
            this.tree.StateImageList = this.imageList1;
            this.tree.TabIndex = 2;
            this.tree.AfterLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.Tree_AfterLabelEdit);
            this.tree.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.tree_BeforeExpand);
            this.tree.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.TreeNodeClick);
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "");
            this.imageList1.Images.SetKeyName(1, "");
            this.imageList1.Images.SetKeyName(2, "");
            this.imageList1.Images.SetKeyName(3, "table.ico");
            this.imageList1.Images.SetKeyName(4, "");
            this.imageList1.Images.SetKeyName(5, "");
            this.imageList1.Images.SetKeyName(6, "");
            this.imageList1.Images.SetKeyName(7, "");
            this.imageList1.Images.SetKeyName(8, "");
            // 
            // groupField
            // 
            this.groupField.BackColor = System.Drawing.Color.Transparent;
            this.groupField.Controls.Add(this.textBox_Xiaoshu_len);
            this.groupField.Controls.Add(this.label5);
            this.groupField.Controls.Add(this.textBox_Zhengshu_len);
            this.groupField.Controls.Add(this.label4);
            this.groupField.Controls.Add(this.label2);
            this.groupField.Controls.Add(this.textBox_data_len);
            this.groupField.Controls.Add(this.label1);
            this.groupField.Controls.Add(this.combo_data_type);
            this.groupField.Location = new System.Drawing.Point(41, 251);
            this.groupField.Name = "groupField";
            this.groupField.Size = new System.Drawing.Size(321, 191);
            this.groupField.TabIndex = 36;
            this.groupField.TabStop = false;
            this.groupField.Text = "字段编辑";
            this.groupField.Visible = false;
            // 
            // textBox_Xiaoshu_len
            // 
            // 
            // 
            // 
            this.textBox_Xiaoshu_len.Border.Class = "TextBoxBorder";
            this.textBox_Xiaoshu_len.Location = new System.Drawing.Point(120, 148);
            this.textBox_Xiaoshu_len.Name = "textBox_Xiaoshu_len";
            this.textBox_Xiaoshu_len.Size = new System.Drawing.Size(157, 21);
            this.textBox_Xiaoshu_len.TabIndex = 9;
            // 
            // label5
            // 
            this.label5.BackColor = System.Drawing.Color.Transparent;
            this.label5.Location = new System.Drawing.Point(20, 144);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(76, 26);
            this.label5.TabIndex = 10;
            this.label5.Text = "小数位数：";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // textBox_Zhengshu_len
            // 
            // 
            // 
            // 
            this.textBox_Zhengshu_len.Border.Class = "TextBoxBorder";
            this.textBox_Zhengshu_len.Location = new System.Drawing.Point(121, 108);
            this.textBox_Zhengshu_len.Name = "textBox_Zhengshu_len";
            this.textBox_Zhengshu_len.Size = new System.Drawing.Size(157, 21);
            this.textBox_Zhengshu_len.TabIndex = 7;
            // 
            // label4
            // 
            this.label4.BackColor = System.Drawing.Color.Transparent;
            this.label4.Location = new System.Drawing.Point(21, 104);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(76, 26);
            this.label4.TabIndex = 8;
            this.label4.Text = "整数位数：";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label2
            // 
            this.label2.BackColor = System.Drawing.Color.Transparent;
            this.label2.Location = new System.Drawing.Point(20, 26);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(76, 26);
            this.label2.TabIndex = 5;
            this.label2.Text = "数据类型：";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // textBox_data_len
            // 
            // 
            // 
            // 
            this.textBox_data_len.Border.Class = "TextBoxBorder";
            this.textBox_data_len.Location = new System.Drawing.Point(120, 69);
            this.textBox_data_len.Name = "textBox_data_len";
            this.textBox_data_len.Size = new System.Drawing.Size(157, 21);
            this.textBox_data_len.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.Location = new System.Drawing.Point(20, 65);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(76, 26);
            this.label1.TabIndex = 3;
            this.label1.Text = "数据长度：";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // combo_data_type
            // 
            this.combo_data_type.DisplayMember = "Text";
            this.combo_data_type.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.combo_data_type.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.combo_data_type.FormattingEnabled = true;
            this.combo_data_type.Items.AddRange(new object[] {
            this.type_decimal,
            this.type_string,
            this.type_bool,
            this.type_int});
            this.combo_data_type.Location = new System.Drawing.Point(120, 29);
            this.combo_data_type.Name = "combo_data_type";
            this.combo_data_type.Size = new System.Drawing.Size(157, 22);
            this.combo_data_type.TabIndex = 6;
            this.combo_data_type.SelectedIndexChanged += new System.EventHandler(this.combo_data_type_SelectedIndexChanged);
            // 
            // type_decimal
            // 
            this.type_decimal.Text = "数值";
            // 
            // type_string
            // 
            this.type_string.Text = "字符串";
            // 
            // type_bool
            // 
            this.type_bool.Text = "真假值";
            // 
            // type_int
            // 
            this.type_int.Text = "整型";
            // 
            // groupTable
            // 
            this.groupTable.BackColor = System.Drawing.Color.Transparent;
            this.groupTable.Controls.Add(this.bConfigExcludeField);
            this.groupTable.Controls.Add(this.labelX1);
            this.groupTable.Controls.Add(this.textBox_Exclude_Field);
            this.groupTable.Controls.Add(this.label6);
            this.groupTable.Controls.Add(this.comboExportRule);
            this.groupTable.Controls.Add(this.checkHasHead);
            this.groupTable.Controls.Add(this.checkBrowseFolder);
            this.groupTable.Controls.Add(this.checkHasDefault);
            this.groupTable.Controls.Add(this.label3);
            this.groupTable.Controls.Add(this.textBox_rela_path);
            this.groupTable.Location = new System.Drawing.Point(22, 23);
            this.groupTable.Name = "groupTable";
            this.groupTable.Size = new System.Drawing.Size(449, 201);
            this.groupTable.TabIndex = 35;
            this.groupTable.TabStop = false;
            this.groupTable.Text = "数据表编辑";
            this.groupTable.Visible = false;
            // 
            // bConfigExcludeField
            // 
            this.bConfigExcludeField.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bConfigExcludeField.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bConfigExcludeField.Location = new System.Drawing.Point(361, 73);
            this.bConfigExcludeField.Name = "bConfigExcludeField";
            this.bConfigExcludeField.Size = new System.Drawing.Size(70, 20);
            this.bConfigExcludeField.TabIndex = 41;
            this.bConfigExcludeField.Text = "配置";
            this.bConfigExcludeField.Click += new System.EventHandler(this.bConfigExcludeField_Click);
            // 
            // labelX1
            // 
            this.labelX1.ForeColor = System.Drawing.Color.Black;
            this.labelX1.Location = new System.Drawing.Point(22, 73);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(92, 21);
            this.labelX1.TabIndex = 40;
            this.labelX1.Text = "导出过滤字段：";
            // 
            // textBox_Exclude_Field
            // 
            // 
            // 
            // 
            this.textBox_Exclude_Field.Border.Class = "TextBoxBorder";
            this.textBox_Exclude_Field.Location = new System.Drawing.Point(120, 73);
            this.textBox_Exclude_Field.Name = "textBox_Exclude_Field";
            this.textBox_Exclude_Field.ReadOnly = true;
            this.textBox_Exclude_Field.Size = new System.Drawing.Size(235, 21);
            this.textBox_Exclude_Field.TabIndex = 39;
            // 
            // label6
            // 
            this.label6.BackColor = System.Drawing.Color.Transparent;
            this.label6.Location = new System.Drawing.Point(20, 150);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(76, 26);
            this.label6.TabIndex = 38;
            this.label6.Text = "保存规则：";
            this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // comboExportRule
            // 
            this.comboExportRule.DisplayMember = "Text";
            this.comboExportRule.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.comboExportRule.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboExportRule.FormattingEnabled = true;
            this.comboExportRule.Items.AddRange(new object[] {
            this.comboItem1,
            this.comboItem2});
            this.comboExportRule.Location = new System.Drawing.Point(121, 154);
            this.comboExportRule.Name = "comboExportRule";
            this.comboExportRule.Size = new System.Drawing.Size(234, 22);
            this.comboExportRule.TabIndex = 37;
            // 
            // comboItem1
            // 
            this.comboItem1.Text = "与默认值比较，相同则保存NULL";
            // 
            // comboItem2
            // 
            this.comboItem2.Text = "保存实际的值";
            // 
            // checkHasHead
            // 
            this.checkHasHead.Location = new System.Drawing.Point(22, 115);
            this.checkHasHead.Name = "checkHasHead";
            this.checkHasHead.Size = new System.Drawing.Size(75, 23);
            this.checkHasHead.TabIndex = 36;
            this.checkHasHead.Text = "有表头";
            this.checkHasHead.TextColor = System.Drawing.Color.Black;
            // 
            // checkBrowseFolder
            // 
            this.checkBrowseFolder.Enabled = false;
            this.checkBrowseFolder.Location = new System.Drawing.Point(184, 115);
            this.checkBrowseFolder.Name = "checkBrowseFolder";
            this.checkBrowseFolder.Size = new System.Drawing.Size(115, 23);
            this.checkBrowseFolder.TabIndex = 35;
            this.checkBrowseFolder.Text = "导入时浏览目录";
            this.checkBrowseFolder.TextColor = System.Drawing.Color.DodgerBlue;
            this.checkBrowseFolder.CheckedChanged += new System.EventHandler(this.checkHasDefault_CheckedChanged);
            // 
            // checkHasDefault
            // 
            this.checkHasDefault.Location = new System.Drawing.Point(103, 115);
            this.checkHasDefault.Name = "checkHasDefault";
            this.checkHasDefault.Size = new System.Drawing.Size(75, 23);
            this.checkHasDefault.TabIndex = 35;
            this.checkHasDefault.Text = "有默认行";
            this.checkHasDefault.TextColor = System.Drawing.Color.Black;
            this.checkHasDefault.CheckedChanged += new System.EventHandler(this.checkHasDefault_CheckedChanged);
            // 
            // label3
            // 
            this.label3.BackColor = System.Drawing.Color.Transparent;
            this.label3.Location = new System.Drawing.Point(20, 32);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(94, 26);
            this.label3.TabIndex = 34;
            this.label3.Text = "导出相对路径：";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // textBox_rela_path
            // 
            // 
            // 
            // 
            this.textBox_rela_path.Border.Class = "TextBoxBorder";
            this.textBox_rela_path.Location = new System.Drawing.Point(120, 32);
            this.textBox_rela_path.Name = "textBox_rela_path";
            this.textBox_rela_path.Size = new System.Drawing.Size(235, 21);
            this.textBox_rela_path.TabIndex = 33;
            // 
            // contextMenuBar1
            // 
            this.contextMenuBar1.DockSide = DevComponents.DotNetBar.eDockSide.Document;
            this.contextMenuBar1.Items.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.cTblNodeClick,
            this.cFieldNodeClick});
            this.contextMenuBar1.Location = new System.Drawing.Point(173, 230);
            this.contextMenuBar1.Name = "contextMenuBar1";
            this.contextMenuBar1.Size = new System.Drawing.Size(326, 25);
            this.contextMenuBar1.Stretch = true;
            this.contextMenuBar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.contextMenuBar1.TabIndex = 32;
            this.contextMenuBar1.TabStop = false;
            // 
            // cTblNodeClick
            // 
            this.cTblNodeClick.AutoExpandOnClick = true;
            this.cTblNodeClick.ImagePaddingHorizontal = 8;
            this.cTblNodeClick.Name = "cTblNodeClick";
            this.cTblNodeClick.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bImportTbl,
            this.bRenameTbl,
            this.bNewTbl,
            this.bDelTbl,
            this.bEmptyTbl,
            this.bNewField,
            this.buttonItemModColOrder,
            this.buttonItem1,
            this.buttonItem2});
            this.cTblNodeClick.Text = "cTblNodeClick";
            // 
            // bImportTbl
            // 
            this.bImportTbl.ImagePaddingHorizontal = 8;
            this.bImportTbl.Name = "bImportTbl";
            this.bImportTbl.Text = "导入";
            this.bImportTbl.Click += new System.EventHandler(this.bImportTbl_Click);
            // 
            // bRenameTbl
            // 
            this.bRenameTbl.ImagePaddingHorizontal = 8;
            this.bRenameTbl.Name = "bRenameTbl";
            this.bRenameTbl.Text = "重命名";
            this.bRenameTbl.Click += new System.EventHandler(this.bRenameTbl_Click);
            // 
            // bNewTbl
            // 
            this.bNewTbl.BeginGroup = true;
            this.bNewTbl.ImagePaddingHorizontal = 8;
            this.bNewTbl.Name = "bNewTbl";
            this.bNewTbl.Text = "新建表";
            this.bNewTbl.Click += new System.EventHandler(this.bNewTbl_Click);
            // 
            // bDelTbl
            // 
            this.bDelTbl.ImagePaddingHorizontal = 8;
            this.bDelTbl.Name = "bDelTbl";
            this.bDelTbl.Text = "删除表";
            this.bDelTbl.Click += new System.EventHandler(this.bDelTbl_Click);
            // 
            // bEmptyTbl
            // 
            this.bEmptyTbl.ImagePaddingHorizontal = 8;
            this.bEmptyTbl.Name = "bEmptyTbl";
            this.bEmptyTbl.Text = "清空表数据";
            this.bEmptyTbl.Click += new System.EventHandler(this.bEmptyTbl_Click);
            // 
            // bNewField
            // 
            this.bNewField.BeginGroup = true;
            this.bNewField.ImagePaddingHorizontal = 8;
            this.bNewField.Name = "bNewField";
            this.bNewField.Text = "新建字段";
            this.bNewField.Click += new System.EventHandler(this.bNewField_Click);
            // 
            // buttonItem1
            // 
            this.buttonItem1.BeginGroup = true;
            this.buttonItem1.ImagePaddingHorizontal = 8;
            this.buttonItem1.Name = "buttonItem1";
            this.buttonItem1.Text = "预览";
            this.buttonItem1.Click += new System.EventHandler(this.buttonItem1_Click);
            // 
            // buttonItem2
            // 
            this.buttonItem2.BeginGroup = true;
            this.buttonItem2.ImagePaddingHorizontal = 8;
            this.buttonItem2.Name = "buttonItem2";
            this.buttonItem2.Text = "buttonItem2";
            this.buttonItem2.Visible = false;
            this.buttonItem2.Click += new System.EventHandler(this.buttonItem2_Click);
            // 
            // buttonItemModColOrder
            // 
            this.buttonItemModColOrder.ImagePaddingHorizontal = 8;
            this.buttonItemModColOrder.Name = "buttonItemModColOrder";
            this.buttonItemModColOrder.Text = "修改字段顺序";
            this.buttonItemModColOrder.Click += new System.EventHandler(this.buttonItemModColOrder_Click);
            // 
            // cFieldNodeClick
            // 
            this.cFieldNodeClick.AutoExpandOnClick = true;
            this.cFieldNodeClick.ImagePaddingHorizontal = 8;
            this.cFieldNodeClick.Name = "cFieldNodeClick";
            this.cFieldNodeClick.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bRenameField,
            this.bDelField,
            this.bSetKey,
            this.bUnsetKey});
            this.cFieldNodeClick.Text = "cFieldNodeClick";
            this.cFieldNodeClick.Click += new System.EventHandler(this.cFieldNodeClick_Click);
            // 
            // bRenameField
            // 
            this.bRenameField.ImagePaddingHorizontal = 8;
            this.bRenameField.Name = "bRenameField";
            this.bRenameField.Text = "重命名";
            this.bRenameField.Click += new System.EventHandler(this.bRenameField_Click);
            // 
            // bDelField
            // 
            this.bDelField.ImagePaddingHorizontal = 8;
            this.bDelField.Name = "bDelField";
            this.bDelField.Text = "删除字段";
            this.bDelField.Click += new System.EventHandler(this.bDelField_Click);
            // 
            // bSetKey
            // 
            this.bSetKey.ImagePaddingHorizontal = 8;
            this.bSetKey.Name = "bSetKey";
            this.bSetKey.Text = "设为主键";
            this.bSetKey.Click += new System.EventHandler(this.bSetKey_Click);
            // 
            // bUnsetKey
            // 
            this.bUnsetKey.ImagePaddingHorizontal = 8;
            this.bUnsetKey.Name = "bUnsetKey";
            this.bUnsetKey.Text = "删除主键";
            this.bUnsetKey.Click += new System.EventHandler(this.bUnsetKey_Click);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Location = new System.Drawing.Point(271, 462);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(108, 27);
            this.buttonX1.TabIndex = 10;
            this.buttonX1.Text = "取消";
            this.buttonX1.Click += new System.EventHandler(this.Cancel_button_Click);
            // 
            // UpdateTab_button
            // 
            this.UpdateTab_button.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.UpdateTab_button.Location = new System.Drawing.Point(144, 462);
            this.UpdateTab_button.Name = "UpdateTab_button";
            this.UpdateTab_button.Size = new System.Drawing.Size(108, 27);
            this.UpdateTab_button.TabIndex = 9;
            this.UpdateTab_button.Text = "修改";
            this.UpdateTab_button.Click += new System.EventHandler(this.OK_button_Click);
            // 
            // bool_true
            // 
            this.bool_true.Text = "是";
            // 
            // bool_false
            // 
            this.bool_false.Text = "否";
            // 
            // tabControlPanel1
            // 
            this.tabControlPanel1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.tabControlPanel1.Controls.Add(this.groupField);
            this.tabControlPanel1.Controls.Add(this.groupTable);
            this.tabControlPanel1.Controls.Add(this.contextMenuBar1);
            this.tabControlPanel1.Controls.Add(this.buttonX1);
            this.tabControlPanel1.Controls.Add(this.UpdateTab_button);
            this.tabControlPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel1.Location = new System.Drawing.Point(242, 0);
            this.tabControlPanel1.Name = "tabControlPanel1";
            this.tabControlPanel1.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel1.Size = new System.Drawing.Size(511, 542);
            this.tabControlPanel1.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel1.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel1.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel1.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel1.Style.GradientAngle = 90;
            this.tabControlPanel1.TabIndex = 3;
            // 
            // splitter1
            // 
            this.splitter1.Location = new System.Drawing.Point(242, 0);
            this.splitter1.Name = "splitter1";
            this.splitter1.Size = new System.Drawing.Size(3, 542);
            this.splitter1.TabIndex = 4;
            this.splitter1.TabStop = false;
            // 
            // DBTableEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(753, 542);
            this.Controls.Add(this.splitter1);
            this.Controls.Add(this.tabControlPanel1);
            this.Controls.Add(this.tree);
            this.Name = "DBTableEditor";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "数据表编辑器";
            this.Load += new System.EventHandler(this.DBTableEditor_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.DBTableEditor_FormClosing);
            this.groupField.ResumeLayout(false);
            this.groupTable.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.contextMenuBar1)).EndInit();
            this.tabControlPanel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.TreeView tree;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.ButtonX UpdateTab_button;
        private DevComponents.DotNetBar.Controls.ComboBoxEx combo_data_type;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private DevComponents.DotNetBar.Controls.TextBoxX textBox_data_len;
        private DevComponents.DotNetBar.ContextMenuBar contextMenuBar1;
        private DevComponents.DotNetBar.ButtonItem cTblNodeClick;
        private DevComponents.DotNetBar.ButtonItem bNewTbl;
        private DevComponents.DotNetBar.ButtonItem bImportTbl;
        private DevComponents.DotNetBar.ButtonItem bDelTbl;
        private DevComponents.DotNetBar.ButtonItem cFieldNodeClick;
        private DevComponents.DotNetBar.ButtonItem bDelField;
        private DevComponents.DotNetBar.ButtonItem bSetKey;
        private DevComponents.DotNetBar.ButtonItem bUnsetKey;
        private System.Windows.Forms.ImageList imageList1;
        private DevComponents.Editors.ComboItem type_decimal;
        private DevComponents.Editors.ComboItem type_string;
        private System.Windows.Forms.Label label3;
        private DevComponents.DotNetBar.Controls.TextBoxX textBox_rela_path;
        private System.Windows.Forms.GroupBox groupTable;
        private System.Windows.Forms.GroupBox groupField;
        private DevComponents.DotNetBar.ButtonItem bRenameTbl;
        private DevComponents.DotNetBar.ButtonItem bNewField;
        private DevComponents.Editors.ComboItem type_bool;
        private DevComponents.Editors.ComboItem bool_true;
        private DevComponents.Editors.ComboItem bool_false;
        private DevComponents.DotNetBar.ButtonItem bRenameField;
        private DevComponents.Editors.ComboItem type_int;
        private DevComponents.DotNetBar.ButtonItem bEmptyTbl;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkHasDefault;
        private DevComponents.DotNetBar.Controls.TextBoxX textBox_Xiaoshu_len;
        private System.Windows.Forms.Label label5;
        private DevComponents.DotNetBar.Controls.TextBoxX textBox_Zhengshu_len;
        private System.Windows.Forms.Label label4;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkHasHead;
        private System.Windows.Forms.Label label6;
        private DevComponents.DotNetBar.Controls.ComboBoxEx comboExportRule;
        private DevComponents.Editors.ComboItem comboItem1;
        private DevComponents.Editors.ComboItem comboItem2;
        private DevComponents.DotNetBar.ButtonItem buttonItem1;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel1;
        private System.Windows.Forms.Splitter splitter1;
        private DevComponents.DotNetBar.ButtonItem buttonItem2;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBrowseFolder;
        private DevComponents.DotNetBar.ButtonX bConfigExcludeField;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.Controls.TextBoxX textBox_Exclude_Field;
        private DevComponents.DotNetBar.ButtonItem buttonItemModColOrder;
    }
}