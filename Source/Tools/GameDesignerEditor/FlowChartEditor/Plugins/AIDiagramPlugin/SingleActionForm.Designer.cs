namespace AIDiagramPlugin
{
    partial class SingleActionForm
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
            this.components = new System.ComponentModel.Container();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.textBoxX1 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.cell1 = new DevComponents.AdvTree.Cell();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.labelX3 = new DevComponents.DotNetBar.LabelX();
            this.labelX4 = new DevComponents.DotNetBar.LabelX();
            this.labelX5 = new DevComponents.DotNetBar.LabelX();
            this.labelX6 = new DevComponents.DotNetBar.LabelX();
            this.comboBoxEx1 = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.comboBoxEx2 = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.comboBoxEx3 = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.comboBoxEx4 = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.comboBoxEx5 = new DevComponents.DotNetBar.Controls.ComboBoxEx();
            this.actionNameBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX7 = new DevComponents.DotNetBar.LabelX();
            this.returnValue1Box = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX8 = new DevComponents.DotNetBar.LabelX();
            this.codeEditBox = new luaEditor.luaEditorControl();
            this.tabControl1 = new DevComponents.DotNetBar.TabControl();
            this.tabControlPanel1 = new DevComponents.DotNetBar.TabControlPanel();
            this.actionComboBox = new BaseExp.SearchComboBox.SearchComboBoxEx();
            this.actionInfoPanel = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.checkBoxX5 = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkBoxX4 = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkBoxX3 = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkBoxX2 = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.checkBoxX1 = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.actionTree = new DevComponents.AdvTree.AdvTree();
            this.nodeConnector1 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle1 = new DevComponents.DotNetBar.ElementStyle();
            this.tabItem1 = new DevComponents.DotNetBar.TabItem(this.components);
            this.tabControlPanel2 = new DevComponents.DotNetBar.TabControlPanel();
            this.returnValue3Box = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.returnValue2Box = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX10 = new DevComponents.DotNetBar.LabelX();
            this.labelX9 = new DevComponents.DotNetBar.LabelX();
            this.tabItem2 = new DevComponents.DotNetBar.TabItem(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.tabControl1)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabControlPanel1.SuspendLayout();
            this.actionInfoPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.actionTree)).BeginInit();
            this.tabControlPanel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX2.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonX2.Location = new System.Drawing.Point(771, 635);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(100, 25);
            this.buttonX2.TabIndex = 18;
            this.buttonX2.Text = "取消(Esc)";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonX1.Location = new System.Drawing.Point(657, 635);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Shortcuts.Add(DevComponents.DotNetBar.eShortcut.CtrlS);
            this.buttonX1.Size = new System.Drawing.Size(100, 25);
            this.buttonX1.TabIndex = 17;
            this.buttonX1.Text = "确定(Ctrl+S)";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // textBoxX1
            // 
            this.textBoxX1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxX1.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            // 
            // 
            // 
            this.textBoxX1.Border.Class = "TextBoxBorder";
            this.textBoxX1.Location = new System.Drawing.Point(15, 237);
            this.textBoxX1.Multiline = true;
            this.textBoxX1.Name = "textBoxX1";
            this.textBoxX1.Size = new System.Drawing.Size(454, 246);
            this.textBoxX1.TabIndex = 19;
            // 
            // cell1
            // 
            this.cell1.Name = "cell1";
            this.cell1.StyleMouseOver = null;
            // 
            // labelX2
            // 
            this.labelX2.AutoSize = true;
            this.labelX2.Location = new System.Drawing.Point(15, 13);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(37, 18);
            this.labelX2.TabIndex = 5;
            this.labelX2.Text = "参数1";
            // 
            // labelX3
            // 
            this.labelX3.AutoSize = true;
            this.labelX3.Location = new System.Drawing.Point(15, 51);
            this.labelX3.Name = "labelX3";
            this.labelX3.Size = new System.Drawing.Size(37, 18);
            this.labelX3.TabIndex = 5;
            this.labelX3.Text = "参数2";
            // 
            // labelX4
            // 
            this.labelX4.AutoSize = true;
            this.labelX4.Location = new System.Drawing.Point(15, 88);
            this.labelX4.Name = "labelX4";
            this.labelX4.Size = new System.Drawing.Size(37, 18);
            this.labelX4.TabIndex = 5;
            this.labelX4.Text = "参数3";
            // 
            // labelX5
            // 
            this.labelX5.AutoSize = true;
            this.labelX5.Location = new System.Drawing.Point(15, 126);
            this.labelX5.Name = "labelX5";
            this.labelX5.Size = new System.Drawing.Size(37, 18);
            this.labelX5.TabIndex = 5;
            this.labelX5.Text = "参数4";
            // 
            // labelX6
            // 
            this.labelX6.AutoSize = true;
            this.labelX6.Location = new System.Drawing.Point(15, 165);
            this.labelX6.Name = "labelX6";
            this.labelX6.Size = new System.Drawing.Size(37, 18);
            this.labelX6.TabIndex = 5;
            this.labelX6.Text = "参数5";
            // 
            // comboBoxEx1
            // 
            this.comboBoxEx1.DisplayMember = "Text";
            this.comboBoxEx1.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.comboBoxEx1.FormattingEnabled = true;
            this.comboBoxEx1.ItemHeight = 15;
            this.comboBoxEx1.Location = new System.Drawing.Point(289, 15);
            this.comboBoxEx1.Name = "comboBoxEx1";
            this.comboBoxEx1.Size = new System.Drawing.Size(180, 21);
            this.comboBoxEx1.TabIndex = 20;
            // 
            // comboBoxEx2
            // 
            this.comboBoxEx2.DisplayMember = "Text";
            this.comboBoxEx2.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.comboBoxEx2.FormattingEnabled = true;
            this.comboBoxEx2.ItemHeight = 15;
            this.comboBoxEx2.Location = new System.Drawing.Point(289, 51);
            this.comboBoxEx2.Name = "comboBoxEx2";
            this.comboBoxEx2.Size = new System.Drawing.Size(180, 21);
            this.comboBoxEx2.TabIndex = 20;
            // 
            // comboBoxEx3
            // 
            this.comboBoxEx3.DisplayMember = "Text";
            this.comboBoxEx3.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.comboBoxEx3.FormattingEnabled = true;
            this.comboBoxEx3.ItemHeight = 15;
            this.comboBoxEx3.Location = new System.Drawing.Point(289, 88);
            this.comboBoxEx3.Name = "comboBoxEx3";
            this.comboBoxEx3.Size = new System.Drawing.Size(180, 21);
            this.comboBoxEx3.TabIndex = 20;
            // 
            // comboBoxEx4
            // 
            this.comboBoxEx4.DisplayMember = "Text";
            this.comboBoxEx4.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.comboBoxEx4.FormattingEnabled = true;
            this.comboBoxEx4.ItemHeight = 15;
            this.comboBoxEx4.Location = new System.Drawing.Point(289, 126);
            this.comboBoxEx4.Name = "comboBoxEx4";
            this.comboBoxEx4.Size = new System.Drawing.Size(180, 21);
            this.comboBoxEx4.TabIndex = 20;
            // 
            // comboBoxEx5
            // 
            this.comboBoxEx5.DisplayMember = "Text";
            this.comboBoxEx5.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.comboBoxEx5.FormattingEnabled = true;
            this.comboBoxEx5.ItemHeight = 15;
            this.comboBoxEx5.Location = new System.Drawing.Point(289, 165);
            this.comboBoxEx5.Name = "comboBoxEx5";
            this.comboBoxEx5.Size = new System.Drawing.Size(180, 21);
            this.comboBoxEx5.TabIndex = 20;
            // 
            // actionNameBox
            // 
            // 
            // 
            // 
            this.actionNameBox.Border.Class = "TextBoxBorder";
            this.actionNameBox.Location = new System.Drawing.Point(100, 17);
            this.actionNameBox.Name = "actionNameBox";
            this.actionNameBox.Size = new System.Drawing.Size(734, 21);
            this.actionNameBox.TabIndex = 6;
            // 
            // labelX7
            // 
            this.labelX7.BackColor = System.Drawing.Color.Transparent;
            this.labelX7.Location = new System.Drawing.Point(24, 17);
            this.labelX7.Name = "labelX7";
            this.labelX7.Size = new System.Drawing.Size(70, 20);
            this.labelX7.TabIndex = 5;
            this.labelX7.Text = "动作名称：";
            // 
            // returnValue1Box
            // 
            // 
            // 
            // 
            this.returnValue1Box.Border.Class = "TextBoxBorder";
            this.returnValue1Box.Location = new System.Drawing.Point(100, 53);
            this.returnValue1Box.Name = "returnValue1Box";
            this.returnValue1Box.Size = new System.Drawing.Size(180, 21);
            this.returnValue1Box.TabIndex = 4;
            this.returnValue1Box.WatermarkText = "空";
            // 
            // labelX8
            // 
            this.labelX8.BackColor = System.Drawing.Color.Transparent;
            this.labelX8.Location = new System.Drawing.Point(24, 54);
            this.labelX8.Name = "labelX8";
            this.labelX8.Size = new System.Drawing.Size(70, 20);
            this.labelX8.TabIndex = 3;
            this.labelX8.Text = "返回值1：";
            // 
            // codeEditBox
            // 
            this.codeEditBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.codeEditBox.ContentMenu = null;
            this.codeEditBox.DebugRowIndex = -1;
            this.codeEditBox.FunctionWordColor = System.Drawing.Color.Brown;
            this.codeEditBox.GameObjectDescription = "npc|doodad|player|scene|item|skill";
            this.codeEditBox.GameObjectWordColor = System.Drawing.Color.DeepSkyBlue;
            this.codeEditBox.Location = new System.Drawing.Point(24, 93);
            this.codeEditBox.Name = "codeEditBox";
            this.codeEditBox.ReadOnly = false;
            this.codeEditBox.Size = new System.Drawing.Size(810, 456);
            this.codeEditBox.TabIndex = 0;
            this.codeEditBox.queryObjects += new luaEditor.QueryObjectHandle(this.codeEditBox_queryObjects);
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(194)))), ((int)(((byte)(217)))), ((int)(((byte)(247)))));
            this.tabControl1.CanReorderTabs = false;
            this.tabControl1.Controls.Add(this.tabControlPanel1);
            this.tabControl1.Controls.Add(this.tabControlPanel2);
            this.tabControl1.Location = new System.Drawing.Point(12, 12);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedTabFont = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Bold);
            this.tabControl1.SelectedTabIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(859, 599);
            this.tabControl1.TabIndex = 26;
            this.tabControl1.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox;
            this.tabControl1.Tabs.Add(this.tabItem1);
            this.tabControl1.Tabs.Add(this.tabItem2);
            this.tabControl1.Text = "系统库";
            // 
            // tabControlPanel1
            // 
            this.tabControlPanel1.Controls.Add(this.actionComboBox);
            this.tabControlPanel1.Controls.Add(this.actionInfoPanel);
            this.tabControlPanel1.Controls.Add(this.actionTree);
            this.tabControlPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel1.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel1.Name = "tabControlPanel1";
            this.tabControlPanel1.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel1.Size = new System.Drawing.Size(859, 573);
            this.tabControlPanel1.Style.BackColor1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(142)))), ((int)(((byte)(179)))), ((int)(((byte)(231)))));
            this.tabControlPanel1.Style.BackColor2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(223)))), ((int)(((byte)(237)))), ((int)(((byte)(254)))));
            this.tabControlPanel1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.tabControlPanel1.Style.BorderColor.Color = System.Drawing.Color.FromArgb(((int)(((byte)(59)))), ((int)(((byte)(97)))), ((int)(((byte)(156)))));
            this.tabControlPanel1.Style.BorderSide = ((DevComponents.DotNetBar.eBorderSide)(((DevComponents.DotNetBar.eBorderSide.Left | DevComponents.DotNetBar.eBorderSide.Right)
                        | DevComponents.DotNetBar.eBorderSide.Bottom)));
            this.tabControlPanel1.Style.GradientAngle = 90;
            this.tabControlPanel1.TabIndex = 1;
            this.tabControlPanel1.TabItem = this.tabItem1;
            // 
            // actionComboBox
            // 
            this.actionComboBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.actionComboBox.DisplayMember = "Text";
            this.actionComboBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.actionComboBox.FormattingEnabled = true;
            this.actionComboBox.ItemHeight = 15;
            this.actionComboBox.Items_SelectedItem = null;
            this.actionComboBox.Location = new System.Drawing.Point(15, 18);
            this.actionComboBox.Name = "actionComboBox";
            this.actionComboBox.Size = new System.Drawing.Size(322, 21);
            this.actionComboBox.TabIndex = 24;
            this.actionComboBox.SelectedIndexChanged += new System.EventHandler(this.actionComboBox_SelectedIndexChanged);
            // 
            // actionInfoPanel
            // 
            this.actionInfoPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.actionInfoPanel.BackColor = System.Drawing.Color.Transparent;
            this.actionInfoPanel.CanvasColor = System.Drawing.SystemColors.Control;
            this.actionInfoPanel.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.actionInfoPanel.Controls.Add(this.checkBoxX5);
            this.actionInfoPanel.Controls.Add(this.checkBoxX4);
            this.actionInfoPanel.Controls.Add(this.checkBoxX3);
            this.actionInfoPanel.Controls.Add(this.checkBoxX2);
            this.actionInfoPanel.Controls.Add(this.checkBoxX1);
            this.actionInfoPanel.Controls.Add(this.comboBoxEx5);
            this.actionInfoPanel.Controls.Add(this.labelX2);
            this.actionInfoPanel.Controls.Add(this.comboBoxEx4);
            this.actionInfoPanel.Controls.Add(this.labelX3);
            this.actionInfoPanel.Controls.Add(this.comboBoxEx3);
            this.actionInfoPanel.Controls.Add(this.labelX4);
            this.actionInfoPanel.Controls.Add(this.comboBoxEx2);
            this.actionInfoPanel.Controls.Add(this.labelX5);
            this.actionInfoPanel.Controls.Add(this.comboBoxEx1);
            this.actionInfoPanel.Controls.Add(this.labelX6);
            this.actionInfoPanel.Controls.Add(this.textBoxX1);
            this.actionInfoPanel.Location = new System.Drawing.Point(353, 18);
            this.actionInfoPanel.Name = "actionInfoPanel";
            this.actionInfoPanel.Size = new System.Drawing.Size(488, 528);
            // 
            // 
            // 
            this.actionInfoPanel.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.actionInfoPanel.Style.BackColorGradientAngle = 90;
            this.actionInfoPanel.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.actionInfoPanel.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.actionInfoPanel.Style.BorderBottomWidth = 1;
            this.actionInfoPanel.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.actionInfoPanel.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.actionInfoPanel.Style.BorderLeftWidth = 1;
            this.actionInfoPanel.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.actionInfoPanel.Style.BorderRightWidth = 1;
            this.actionInfoPanel.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.actionInfoPanel.Style.BorderTopWidth = 1;
            this.actionInfoPanel.Style.CornerDiameter = 4;
            this.actionInfoPanel.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.actionInfoPanel.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.actionInfoPanel.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.actionInfoPanel.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.actionInfoPanel.TabIndex = 23;
            this.actionInfoPanel.Text = "动作信息";
            // 
            // checkBoxX5
            // 
            this.checkBoxX5.Location = new System.Drawing.Point(208, 165);
            this.checkBoxX5.Name = "checkBoxX5";
            this.checkBoxX5.Size = new System.Drawing.Size(75, 23);
            this.checkBoxX5.TabIndex = 25;
            this.checkBoxX5.Text = "变量类型";
            // 
            // checkBoxX4
            // 
            this.checkBoxX4.Location = new System.Drawing.Point(208, 126);
            this.checkBoxX4.Name = "checkBoxX4";
            this.checkBoxX4.Size = new System.Drawing.Size(75, 23);
            this.checkBoxX4.TabIndex = 24;
            this.checkBoxX4.Text = "变量类型";
            // 
            // checkBoxX3
            // 
            this.checkBoxX3.Location = new System.Drawing.Point(208, 88);
            this.checkBoxX3.Name = "checkBoxX3";
            this.checkBoxX3.Size = new System.Drawing.Size(75, 23);
            this.checkBoxX3.TabIndex = 23;
            this.checkBoxX3.Text = "变量类型";
            // 
            // checkBoxX2
            // 
            this.checkBoxX2.Location = new System.Drawing.Point(208, 51);
            this.checkBoxX2.Name = "checkBoxX2";
            this.checkBoxX2.Size = new System.Drawing.Size(75, 23);
            this.checkBoxX2.TabIndex = 22;
            this.checkBoxX2.Text = "变量类型";
            // 
            // checkBoxX1
            // 
            this.checkBoxX1.Location = new System.Drawing.Point(208, 15);
            this.checkBoxX1.Name = "checkBoxX1";
            this.checkBoxX1.Size = new System.Drawing.Size(75, 23);
            this.checkBoxX1.TabIndex = 21;
            this.checkBoxX1.Text = "变量类型";
            // 
            // actionTree
            // 
            this.actionTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.actionTree.AllowDrop = true;
            this.actionTree.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.actionTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.actionTree.BackgroundStyle.Class = "TreeBorderKey";
            this.actionTree.DragDropEnabled = false;
            this.actionTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.actionTree.Location = new System.Drawing.Point(15, 54);
            this.actionTree.Name = "actionTree";
            this.actionTree.NodesConnector = this.nodeConnector1;
            this.actionTree.NodeStyle = this.elementStyle1;
            this.actionTree.PathSeparator = ";";
            this.actionTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.actionTree.Size = new System.Drawing.Size(322, 492);
            this.actionTree.Styles.Add(this.elementStyle1);
            this.actionTree.SuspendPaint = false;
            this.actionTree.TabIndex = 22;
            this.actionTree.Text = "actionTree";
            this.actionTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.RefreshActionInfo);
            // 
            // nodeConnector1
            // 
            this.nodeConnector1.LineColor = System.Drawing.SystemColors.ControlText;
            // 
            // elementStyle1
            // 
            this.elementStyle1.Name = "elementStyle1";
            this.elementStyle1.TextColor = System.Drawing.SystemColors.ControlText;
            // 
            // tabItem1
            // 
            this.tabItem1.AttachedControl = this.tabControlPanel1;
            this.tabItem1.Name = "tabItem1";
            this.tabItem1.Text = "系统库";
            // 
            // tabControlPanel2
            // 
            this.tabControlPanel2.Controls.Add(this.returnValue3Box);
            this.tabControlPanel2.Controls.Add(this.returnValue2Box);
            this.tabControlPanel2.Controls.Add(this.labelX10);
            this.tabControlPanel2.Controls.Add(this.labelX9);
            this.tabControlPanel2.Controls.Add(this.codeEditBox);
            this.tabControlPanel2.Controls.Add(this.returnValue1Box);
            this.tabControlPanel2.Controls.Add(this.actionNameBox);
            this.tabControlPanel2.Controls.Add(this.labelX8);
            this.tabControlPanel2.Controls.Add(this.labelX7);
            this.tabControlPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlPanel2.Location = new System.Drawing.Point(0, 26);
            this.tabControlPanel2.Name = "tabControlPanel2";
            this.tabControlPanel2.Padding = new System.Windows.Forms.Padding(1);
            this.tabControlPanel2.Size = new System.Drawing.Size(859, 573);
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
            // returnValue3Box
            // 
            // 
            // 
            // 
            this.returnValue3Box.Border.Class = "TextBoxBorder";
            this.returnValue3Box.Location = new System.Drawing.Point(654, 53);
            this.returnValue3Box.Name = "returnValue3Box";
            this.returnValue3Box.Size = new System.Drawing.Size(180, 21);
            this.returnValue3Box.TabIndex = 10;
            this.returnValue3Box.WatermarkText = "空";
            // 
            // returnValue2Box
            // 
            // 
            // 
            // 
            this.returnValue2Box.Border.Class = "TextBoxBorder";
            this.returnValue2Box.Location = new System.Drawing.Point(372, 54);
            this.returnValue2Box.Name = "returnValue2Box";
            this.returnValue2Box.Size = new System.Drawing.Size(180, 21);
            this.returnValue2Box.TabIndex = 9;
            this.returnValue2Box.WatermarkText = "空";
            // 
            // labelX10
            // 
            this.labelX10.BackColor = System.Drawing.Color.Transparent;
            this.labelX10.Location = new System.Drawing.Point(578, 53);
            this.labelX10.Name = "labelX10";
            this.labelX10.Size = new System.Drawing.Size(70, 20);
            this.labelX10.TabIndex = 8;
            this.labelX10.Text = "返回值3：";
            // 
            // labelX9
            // 
            this.labelX9.BackColor = System.Drawing.Color.Transparent;
            this.labelX9.Location = new System.Drawing.Point(296, 54);
            this.labelX9.Name = "labelX9";
            this.labelX9.Size = new System.Drawing.Size(70, 20);
            this.labelX9.TabIndex = 7;
            this.labelX9.Text = "返回值2：";
            // 
            // tabItem2
            // 
            this.tabItem2.AttachedControl = this.tabControlPanel2;
            this.tabItem2.Name = "tabItem2";
            this.tabItem2.Text = "自定义";
            // 
            // SingleActionForm
            // 
            this.AcceptButton = this.buttonX1;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonX2;
            this.ClientSize = new System.Drawing.Size(893, 679);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.buttonX2);
            this.Controls.Add(this.buttonX1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SingleActionForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "动作";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.SingleActionForm_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.tabControl1)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.tabControlPanel1.ResumeLayout(false);
            this.actionInfoPanel.ResumeLayout(false);
            this.actionInfoPanel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.actionTree)).EndInit();
            this.tabControlPanel2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.AdvTree.Cell cell1;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX1;
        private DevComponents.DotNetBar.LabelX labelX2;
        private DevComponents.DotNetBar.LabelX labelX3;
        private DevComponents.DotNetBar.LabelX labelX4;
        private DevComponents.DotNetBar.LabelX labelX5;
        private DevComponents.DotNetBar.LabelX labelX6;
        private DevComponents.DotNetBar.Controls.ComboBoxEx comboBoxEx1;
        private DevComponents.DotNetBar.Controls.ComboBoxEx comboBoxEx2;
        private DevComponents.DotNetBar.Controls.ComboBoxEx comboBoxEx3;
        private DevComponents.DotNetBar.Controls.ComboBoxEx comboBoxEx4;
        private DevComponents.DotNetBar.Controls.ComboBoxEx comboBoxEx5;
        private DevComponents.DotNetBar.Controls.TextBoxX returnValue1Box;
        private DevComponents.DotNetBar.LabelX labelX8;
        private DevComponents.DotNetBar.Controls.TextBoxX actionNameBox;
        private DevComponents.DotNetBar.LabelX labelX7;
        private DevComponents.DotNetBar.TabControl tabControl1;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel1;
        private DevComponents.DotNetBar.TabItem tabItem1;
        private DevComponents.DotNetBar.TabControlPanel tabControlPanel2;
        private DevComponents.DotNetBar.TabItem tabItem2;
        public luaEditor.luaEditorControl codeEditBox;
        private DevComponents.DotNetBar.LabelX labelX10;
        private DevComponents.DotNetBar.LabelX labelX9;
        private DevComponents.DotNetBar.Controls.TextBoxX returnValue2Box;
        private DevComponents.DotNetBar.Controls.TextBoxX returnValue3Box;
        private DevComponents.DotNetBar.Controls.GroupPanel actionInfoPanel;
        private DevComponents.AdvTree.AdvTree actionTree;
        private DevComponents.AdvTree.NodeConnector nodeConnector1;
        private DevComponents.DotNetBar.ElementStyle elementStyle1;
        private BaseExp.SearchComboBox.SearchComboBoxEx actionComboBox;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxX1;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxX5;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxX4;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxX3;
        private DevComponents.DotNetBar.Controls.CheckBoxX checkBoxX2;
    }
}