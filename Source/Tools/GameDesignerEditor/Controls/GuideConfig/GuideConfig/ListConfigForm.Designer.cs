namespace GuideConfig
{
    partial class ListConfigForm
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
            this.listConfigControl = new DevComponents.DotNetBar.TabControl();
            this.tabControlPanel1 = new DevComponents.DotNetBar.TabControlPanel();
            this.listItemValueBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.bMoveDown = new DevComponents.DotNetBar.ButtonX();
            this.bMoveUp = new DevComponents.DotNetBar.ButtonX();
            this.bRemove = new DevComponents.DotNetBar.ButtonX();
            this.bAddListItem = new DevComponents.DotNetBar.ButtonX();
            this.listItemNameBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.listTextBox = new System.Windows.Forms.ListBox();
            this.tabItem1 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel2 = new DevComponents.DotNetBar.TabControlPanel();
            this.valueFieldBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.displayFieldBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.labelX3 = new DevComponents.DotNetBar.LabelX();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.tableNameBox = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.tabItem2 = new DevComponents.DotNetBar.TabItem(this.components);
            this.bOK = new DevComponents.DotNetBar.ButtonX();
            this.bCancel = new DevComponents.DotNetBar.ButtonX();
            ((System.ComponentModel.ISupportInitialize)(this.listConfigControl)).BeginInit();
            this.listConfigControl.SuspendLayout();
            this.tabControlPanel1.SuspendLayout();
            this.tabControlPanel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // listConfigControl
            // 
            this.listConfigControl.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(194)))), ((int)(((byte)(217)))), ((int)(((byte)(247)))));
            this.listConfigControl.CanReorderTabs = false;
            this.listConfigControl.Controls.Add(this.tabControlPanel1);
            this.listConfigControl.Controls.Add(this.tabControlPanel2);
            this.listConfigControl.Location = new System.Drawing.Point(12, 12);
            this.listConfigControl.Name = "listConfigControl";
            this.listConfigControl.SelectedTabFont = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Bold);
            this.listConfigControl.SelectedTabIndex = 0;
            this.listConfigControl.Size = new System.Drawing.Size(408, 250);
            this.listConfigControl.TabIndex = 0;
            this.listConfigControl.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.listConfigControl.Tabs.Add(this.tabItem1);
            this.listConfigControl.Tabs.Add(this.tabItem2);
            this.listConfigControl.TabsVisible = false;
            this.listConfigControl.Text = "tabControl1";
            // 
            // tabControlPanel1
            // 
            this.tabControlPanel1.Controls.Add(this.listItemValueBox);
            this.tabControlPanel1.Controls.Add(this.bMoveDown);
            this.tabControlPanel1.Controls.Add(this.bMoveUp);
            this.tabControlPanel1.Controls.Add(this.bRemove);
            this.tabControlPanel1.Controls.Add(this.bAddListItem);
            this.tabControlPanel1.Controls.Add(this.listItemNameBox);
            this.tabControlPanel1.Controls.Add(this.listTextBox);
            this.tabControlPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel1.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel1.Name = "tabControlPanel1";
            this.tabControlPanel1.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel1.Size = new System.Drawing.Size(408, 224);
            this.tabControlPanel1.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel1.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel1.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel1.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel1.Style.GradientAngle = 90;
            this.tabControlPanel1.TabIndex = 0;
            this.tabControlPanel1.TabItem = this.tabItem1;
            // 
            // listItemValueBox
            // 
            // 
            // 
            // 
            this.listItemValueBox.Border.Class = "TextBoxBorder";
            this.listItemValueBox.Location = new System.Drawing.Point(121, 171);
            this.listItemValueBox.Name = "listItemValueBox";
            this.listItemValueBox.Size = new System.Drawing.Size(100, 21);
            this.listItemValueBox.TabIndex = 1;
            this.listItemValueBox.WatermarkText = "选项值";
            // 
            // bMoveDown
            // 
            this.bMoveDown.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bMoveDown.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bMoveDown.Location = new System.Drawing.Point(321, 200);
            this.bMoveDown.Name = "bMoveDown";
            this.bMoveDown.Size = new System.Drawing.Size(70, 20);
            this.bMoveDown.TabIndex = 5;
            this.bMoveDown.Text = "下移选项";
            this.bMoveDown.Click += new System.EventHandler(this.bMoveDown_Click);
            // 
            // bMoveUp
            // 
            this.bMoveUp.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bMoveUp.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bMoveUp.Location = new System.Drawing.Point(227, 200);
            this.bMoveUp.Name = "bMoveUp";
            this.bMoveUp.Size = new System.Drawing.Size(70, 20);
            this.bMoveUp.TabIndex = 4;
            this.bMoveUp.Text = "上移选项";
            this.bMoveUp.Click += new System.EventHandler(this.bMoveUp_Click);
            // 
            // bRemove
            // 
            this.bRemove.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bRemove.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bRemove.Location = new System.Drawing.Point(321, 171);
            this.bRemove.Name = "bRemove";
            this.bRemove.Size = new System.Drawing.Size(70, 20);
            this.bRemove.TabIndex = 3;
            this.bRemove.Text = "删除选项";
            this.bRemove.Click += new System.EventHandler(this.bRemove_Click);
            // 
            // bAddListItem
            // 
            this.bAddListItem.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bAddListItem.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bAddListItem.Location = new System.Drawing.Point(227, 171);
            this.bAddListItem.Name = "bAddListItem";
            this.bAddListItem.Size = new System.Drawing.Size(70, 20);
            this.bAddListItem.TabIndex = 2;
            this.bAddListItem.Text = "添加选项";
            this.bAddListItem.Click += new System.EventHandler(this.bAddListItem_Click);
            // 
            // listItemNameBox
            // 
            // 
            // 
            // 
            this.listItemNameBox.Border.Class = "TextBoxBorder";
            this.listItemNameBox.Location = new System.Drawing.Point(15, 171);
            this.listItemNameBox.Name = "listItemNameBox";
            this.listItemNameBox.Size = new System.Drawing.Size(100, 21);
            this.listItemNameBox.TabIndex = 0;
            this.listItemNameBox.WatermarkText = "选项名称";
            // 
            // listTextBox
            // 
            this.listTextBox.FormattingEnabled = true;
            this.listTextBox.ItemHeight = 12;
            this.listTextBox.Location = new System.Drawing.Point(15, 15);
            this.listTextBox.Name = "listTextBox";
            this.listTextBox.ScrollAlwaysVisible = true;
            this.listTextBox.Size = new System.Drawing.Size(376, 136);
            this.listTextBox.TabIndex = 6;
            // 
            // tabItem1
            // 
            this.tabItem1.AttachedControl = this.tabControlPanel1;
            this.tabItem1.Name = "tabItem1";
            this.tabItem1.Text = "文本";
            // 
            // tabControlPanel2
            // 
            this.tabControlPanel2.Controls.Add(this.valueFieldBox);
            this.tabControlPanel2.Controls.Add(this.displayFieldBox);
            this.tabControlPanel2.Controls.Add(this.labelX3);
            this.tabControlPanel2.Controls.Add(this.labelX2);
            this.tabControlPanel2.Controls.Add(this.tableNameBox);
            this.tabControlPanel2.Controls.Add(this.labelX1);
            this.tabControlPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel2.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel2.Name = "tabControlPanel2";
            this.tabControlPanel2.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel2.Size = new System.Drawing.Size(408, 224);
            this.tabControlPanel2.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel2.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel2.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel2.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel2.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel2.Style.GradientAngle = 90;
            this.tabControlPanel2.TabIndex = 2;
            this.tabControlPanel2.TabItem = this.tabItem2;
            // 
            // valueFieldBox
            // 
            this.valueFieldBox.DisplayMember = "Text";
            this.valueFieldBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.valueFieldBox.FormattingEnabled = true;
            this.valueFieldBox.ItemHeight = 15;
            this.valueFieldBox.Location = new System.Drawing.Point(91, 86);
            this.valueFieldBox.Name = "valueFieldBox";
            this.valueFieldBox.Size = new System.Drawing.Size(300, 21);
            this.valueFieldBox.TabIndex = 5;
            // 
            // displayFieldBox
            // 
            this.displayFieldBox.DisplayMember = "Text";
            this.displayFieldBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.displayFieldBox.FormattingEnabled = true;
            this.displayFieldBox.ItemHeight = 15;
            this.displayFieldBox.Location = new System.Drawing.Point(91, 50);
            this.displayFieldBox.Name = "displayFieldBox";
            this.displayFieldBox.Size = new System.Drawing.Size(300, 21);
            this.displayFieldBox.TabIndex = 4;
            // 
            // labelX3
            // 
            this.labelX3.BackColor = System.Drawing.Color.Transparent;
            this.labelX3.Location = new System.Drawing.Point(15, 86);
            this.labelX3.Name = "labelX3";
            this.labelX3.Size = new System.Drawing.Size(70, 20);
            this.labelX3.TabIndex = 3;
            this.labelX3.Text = "取值字段";
            // 
            // labelX2
            // 
            this.labelX2.BackColor = System.Drawing.Color.Transparent;
            this.labelX2.Location = new System.Drawing.Point(15, 50);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(70, 20);
            this.labelX2.TabIndex = 2;
            this.labelX2.Text = "显示字段";
            // 
            // tableNameBox
            // 
            this.tableNameBox.DisplayMember = "Text";
            this.tableNameBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.tableNameBox.FormattingEnabled = true;
            this.tableNameBox.ItemHeight = 15;
            this.tableNameBox.Location = new System.Drawing.Point(91, 15);
            this.tableNameBox.Name = "tableNameBox";
            this.tableNameBox.Size = new System.Drawing.Size(300, 21);
            this.tableNameBox.TabIndex = 1;
            this.tableNameBox.SelectedIndexChanged += new System.EventHandler(this.tableNameBox_SelectedIndexChanged);
            // 
            // labelX1
            // 
            this.labelX1.BackColor = System.Drawing.Color.Transparent;
            this.labelX1.Location = new System.Drawing.Point(15, 15);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(70, 20);
            this.labelX1.TabIndex = 0;
            this.labelX1.Text = "数据表名";
            // 
            // tabItem2
            // 
            this.tabItem2.AttachedControl = this.tabControlPanel2;
            this.tabItem2.Name = "tabItem2";
            this.tabItem2.Text = "数据库索引";
            // 
            // bOK
            // 
            this.bOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bOK.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bOK.Location = new System.Drawing.Point(263, 288);
            this.bOK.Name = "bOK";
            this.bOK.Size = new System.Drawing.Size(70, 20);
            this.bOK.TabIndex = 1;
            this.bOK.Text = "确定";
            this.bOK.Click += new System.EventHandler(this.bOK_Click);
            // 
            // bCancel
            // 
            this.bCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bCancel.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.bCancel.Location = new System.Drawing.Point(350, 288);
            this.bCancel.Name = "bCancel";
            this.bCancel.Size = new System.Drawing.Size(70, 20);
            this.bCancel.TabIndex = 2;
            this.bCancel.Text = "取消";
            this.bCancel.Click += new System.EventHandler(this.bCancel_Click);
            // 
            // ListConfigForm
            // 
            this.AcceptButton = this.bOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.bCancel;
            this.ClientSize = new System.Drawing.Size(432, 325);
            this.Controls.Add(this.bCancel);
            this.Controls.Add(this.bOK);
            this.Controls.Add(this.listConfigControl);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ListConfigForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "下拉框配置";
            ((System.ComponentModel.ISupportInitialize)(this.listConfigControl)).EndInit();
            this.listConfigControl.ResumeLayout(false);
            this.tabControlPanel1.ResumeLayout(false);
            this.tabControlPanel2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.TabControl listConfigControl;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel1;
        private DevComponents.DotNetBar.TabItem tabItem1;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel2;
        private DevComponents.DotNetBar.TabItem tabItem2;
        private DevComponents.DotNetBar.ButtonX bOK;
        private DevComponents.DotNetBar.ButtonX bCancel;
        private DevComponents.DotNetBar.ButtonX bRemove;
        private DevComponents.DotNetBar.ButtonX bAddListItem;
        private DevComponents.DotNetBar.Controls.TextBoxX listItemNameBox;
        private System.Windows.Forms.ListBox listTextBox;
        private DevComponents.DotNetBar.ButtonX bMoveUp;
        private DevComponents.DotNetBar.ButtonX bMoveDown;
        private DevComponents.DotNetBar.Controls.ComboBoxEx tableNameBox;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.LabelX labelX3;
        private DevComponents.DotNetBar.LabelX labelX2;
        private DevComponents.DotNetBar.Controls.ComboBoxEx valueFieldBox;
        private DevComponents.DotNetBar.Controls.ComboBoxEx displayFieldBox;
        private DevComponents.DotNetBar.Controls.TextBoxX listItemValueBox;
    }
}