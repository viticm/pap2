namespace AIDiagramPlugin
{
    partial class StateNodeForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(StateNodeForm));
            this.txtName = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.btnDel = new DevComponents.DotNetBar.ButtonX();
            this.btnAdd = new DevComponents.DotNetBar.ButtonX();
            this.allEventTree = new DevComponents.AdvTree.AdvTree();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.nodeConnector1 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle1 = new DevComponents.DotNetBar.ElementStyle();
            this.registedEventTree = new DevComponents.AdvTree.AdvTree();
            this.nodeConnector2 = new DevComponents.AdvTree.NodeConnector();
            this.elementStyle2 = new DevComponents.DotNetBar.ElementStyle();
            this.labelX3 = new DevComponents.DotNetBar.LabelX();
            this.labelX4 = new DevComponents.DotNetBar.LabelX();
            this.buttonX2 = new DevComponents.DotNetBar.ButtonX();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.bNewCustomEvent = new DevComponents.DotNetBar.ButtonX();
            this.bDeleteCustomEvent = new DevComponents.DotNetBar.ButtonX();
            this.textBoxX1 = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.informationBox = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.bRenameCustomEvent = new DevComponents.DotNetBar.ButtonX();
            this.customEventPanel = new DevComponents.DotNetBar.Controls.GroupPanel();
            ((System.ComponentModel.ISupportInitialize)(this.allEventTree)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.registedEventTree)).BeginInit();
            this.customEventPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // txtName
            // 
            // 
            // 
            // 
            this.txtName.Border.Class = "TextBoxBorder";
            this.txtName.Location = new System.Drawing.Point(117, 23);
            this.txtName.Name = "txtName";
            this.txtName.Size = new System.Drawing.Size(504, 21);
            this.txtName.TabIndex = 0;
            this.txtName.WatermarkText = "输入当前状态的名称";
            // 
            // labelX1
            // 
            this.labelX1.AutoSize = true;
            this.labelX1.Location = new System.Drawing.Point(18, 23);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(93, 18);
            this.labelX1.TabIndex = 1;
            this.labelX1.Text = "状态名称(&N)：";
            // 
            // btnDel
            // 
            this.btnDel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnDel.Location = new System.Drawing.Point(285, 123);
            this.btnDel.Name = "btnDel";
            this.btnDel.Size = new System.Drawing.Size(70, 20);
            this.btnDel.TabIndex = 9;
            this.btnDel.Text = "删除 <-";
            this.btnDel.Tooltip = "将当前选中的事件从状态的注册事件中删除";
            this.btnDel.Click += new System.EventHandler(this.btnDel_Click);
            // 
            // btnAdd
            // 
            this.btnAdd.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnAdd.Location = new System.Drawing.Point(285, 87);
            this.btnAdd.Name = "btnAdd";
            this.btnAdd.Size = new System.Drawing.Size(70, 20);
            this.btnAdd.TabIndex = 8;
            this.btnAdd.Text = "添加 ->";
            this.btnAdd.Tooltip = "将当前选中的事件添加到状态的注册事件中";
            this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // allEventTree
            // 
            this.allEventTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.allEventTree.AllowDrop = true;
            this.allEventTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.allEventTree.BackgroundStyle.Class = "TreeBorderKey";
            this.allEventTree.DragDropEnabled = false;
            this.allEventTree.GridLinesColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.allEventTree.HotTracking = true;
            this.allEventTree.ImageIndex = 0;
            this.allEventTree.ImageList = this.imageList1;
            this.allEventTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.allEventTree.Location = new System.Drawing.Point(18, 87);
            this.allEventTree.Name = "allEventTree";
            this.allEventTree.NodesConnector = this.nodeConnector1;
            this.allEventTree.NodeStyle = this.elementStyle1;
            this.allEventTree.PathSeparator = ";";
            this.allEventTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.allEventTree.Size = new System.Drawing.Size(250, 280);
            this.allEventTree.Styles.Add(this.elementStyle1);
            this.allEventTree.SuspendPaint = false;
            this.allEventTree.TabIndex = 13;
            this.allEventTree.Text = "advTree1";
            this.allEventTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.advTree1_AfterNodeSelect);
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "3_15.png");
            this.imageList1.Images.SetKeyName(1, "13_12.png");
            this.imageList1.Images.SetKeyName(2, "10_14.png");
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
            // registedEventTree
            // 
            this.registedEventTree.AccessibleRole = System.Windows.Forms.AccessibleRole.Outline;
            this.registedEventTree.AllowDrop = true;
            this.registedEventTree.BackColor = System.Drawing.SystemColors.Window;
            // 
            // 
            // 
            this.registedEventTree.BackgroundStyle.Class = "TreeBorderKey";
            this.registedEventTree.DragDropEnabled = false;
            this.registedEventTree.GridLinesColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.registedEventTree.HotTracking = true;
            this.registedEventTree.ImageIndex = 0;
            this.registedEventTree.ImageList = this.imageList1;
            this.registedEventTree.LicenseKey = "F962CEC7-CD8F-4911-A9E9-CAB39962FC1F";
            this.registedEventTree.Location = new System.Drawing.Point(371, 87);
            this.registedEventTree.Name = "registedEventTree";
            this.registedEventTree.NodesConnector = this.nodeConnector2;
            this.registedEventTree.NodeStyle = this.elementStyle2;
            this.registedEventTree.PathSeparator = ";";
            this.registedEventTree.SelectionBoxStyle = DevComponents.AdvTree.eSelectionStyle.FullRowSelect;
            this.registedEventTree.Size = new System.Drawing.Size(250, 280);
            this.registedEventTree.Styles.Add(this.elementStyle2);
            this.registedEventTree.SuspendPaint = false;
            this.registedEventTree.TabIndex = 14;
            this.registedEventTree.Text = "advTree2";
            this.registedEventTree.AfterNodeSelect += new DevComponents.AdvTree.AdvTreeNodeEventHandler(this.advTree2_AfterNodeSelect);
            // 
            // nodeConnector2
            // 
            this.nodeConnector2.LineColor = System.Drawing.SystemColors.ControlText;
            // 
            // elementStyle2
            // 
            this.elementStyle2.Name = "elementStyle2";
            this.elementStyle2.TextColor = System.Drawing.SystemColors.ControlText;
            // 
            // labelX3
            // 
            this.labelX3.Location = new System.Drawing.Point(18, 61);
            this.labelX3.Name = "labelX3";
            this.labelX3.Size = new System.Drawing.Size(70, 20);
            this.labelX3.TabIndex = 1;
            this.labelX3.Text = "所有事件：";
            // 
            // labelX4
            // 
            this.labelX4.Location = new System.Drawing.Point(371, 61);
            this.labelX4.Name = "labelX4";
            this.labelX4.Size = new System.Drawing.Size(70, 20);
            this.labelX4.TabIndex = 1;
            this.labelX4.Text = "注册事件：";
            // 
            // buttonX2
            // 
            this.buttonX2.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX2.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonX2.Location = new System.Drawing.Point(551, 622);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new System.Drawing.Size(70, 20);
            this.buttonX2.TabIndex = 16;
            this.buttonX2.Text = "取消(&C)";
            this.buttonX2.Click += new System.EventHandler(this.buttonX2_Click);
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.Location = new System.Drawing.Point(465, 622);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(70, 20);
            this.buttonX1.TabIndex = 15;
            this.buttonX1.Text = "确定(&O)";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // bNewCustomEvent
            // 
            this.bNewCustomEvent.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bNewCustomEvent.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bNewCustomEvent.Location = new System.Drawing.Point(177, 17);
            this.bNewCustomEvent.Name = "bNewCustomEvent";
            this.bNewCustomEvent.Size = new System.Drawing.Size(70, 20);
            this.bNewCustomEvent.TabIndex = 17;
            this.bNewCustomEvent.Text = "新建";
            this.bNewCustomEvent.Tooltip = "创建新的自定义事件";
            this.bNewCustomEvent.Click += new System.EventHandler(this.buttonX3_Click);
            // 
            // bDeleteCustomEvent
            // 
            this.bDeleteCustomEvent.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bDeleteCustomEvent.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bDeleteCustomEvent.Location = new System.Drawing.Point(505, 17);
            this.bDeleteCustomEvent.Name = "bDeleteCustomEvent";
            this.bDeleteCustomEvent.Size = new System.Drawing.Size(70, 20);
            this.bDeleteCustomEvent.TabIndex = 18;
            this.bDeleteCustomEvent.Text = "删除";
            this.bDeleteCustomEvent.Tooltip = "删除当前选中的自定义事件";
            this.bDeleteCustomEvent.Click += new System.EventHandler(this.buttonX4_Click);
            // 
            // textBoxX1
            // 
            // 
            // 
            // 
            this.textBoxX1.Border.Class = "TextBoxBorder";
            this.textBoxX1.Location = new System.Drawing.Point(22, 17);
            this.textBoxX1.Name = "textBoxX1";
            this.textBoxX1.Size = new System.Drawing.Size(138, 21);
            this.textBoxX1.TabIndex = 19;
            this.textBoxX1.WatermarkText = "输入自定义事件的名称";
            // 
            // informationBox
            // 
            this.informationBox.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            // 
            // 
            // 
            this.informationBox.Border.Class = "TextBoxBorder";
            this.informationBox.Location = new System.Drawing.Point(18, 495);
            this.informationBox.Multiline = true;
            this.informationBox.Name = "informationBox";
            this.informationBox.ReadOnly = true;
            this.informationBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.informationBox.Size = new System.Drawing.Size(603, 91);
            this.informationBox.TabIndex = 20;
            // 
            // bRenameCustomEvent
            // 
            this.bRenameCustomEvent.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.bRenameCustomEvent.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.bRenameCustomEvent.Location = new System.Drawing.Point(420, 17);
            this.bRenameCustomEvent.Name = "bRenameCustomEvent";
            this.bRenameCustomEvent.Size = new System.Drawing.Size(70, 20);
            this.bRenameCustomEvent.TabIndex = 21;
            this.bRenameCustomEvent.Text = "重命名";
            this.bRenameCustomEvent.Tooltip = "为当前选中的自定义事件重命名";
            this.bRenameCustomEvent.Click += new System.EventHandler(this.buttonX5_Click);
            // 
            // customEventPanel
            // 
            this.customEventPanel.CanvasColor = System.Drawing.SystemColors.Control;
            this.customEventPanel.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.customEventPanel.Controls.Add(this.textBoxX1);
            this.customEventPanel.Controls.Add(this.bRenameCustomEvent);
            this.customEventPanel.Controls.Add(this.bNewCustomEvent);
            this.customEventPanel.Controls.Add(this.bDeleteCustomEvent);
            this.customEventPanel.Location = new System.Drawing.Point(18, 391);
            this.customEventPanel.Name = "customEventPanel";
            this.customEventPanel.Size = new System.Drawing.Size(603, 81);
            // 
            // 
            // 
            this.customEventPanel.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.customEventPanel.Style.BackColorGradientAngle = 90;
            this.customEventPanel.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.customEventPanel.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.customEventPanel.Style.BorderBottomWidth = 1;
            this.customEventPanel.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.customEventPanel.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.customEventPanel.Style.BorderLeftWidth = 1;
            this.customEventPanel.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.customEventPanel.Style.BorderRightWidth = 1;
            this.customEventPanel.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.customEventPanel.Style.BorderTopWidth = 1;
            this.customEventPanel.Style.CornerDiameter = 4;
            this.customEventPanel.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.customEventPanel.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.customEventPanel.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.customEventPanel.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            this.customEventPanel.TabIndex = 22;
            this.customEventPanel.Text = "自定义事件";
            // 
            // StateNodeForm
            // 
            this.AcceptButton = this.buttonX1;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonX2;
            this.ClientSize = new System.Drawing.Size(641, 664);
            this.Controls.Add(this.customEventPanel);
            this.Controls.Add(this.informationBox);
            this.Controls.Add(this.buttonX2);
            this.Controls.Add(this.buttonX1);
            this.Controls.Add(this.registedEventTree);
            this.Controls.Add(this.allEventTree);
            this.Controls.Add(this.btnDel);
            this.Controls.Add(this.btnAdd);
            this.Controls.Add(this.labelX4);
            this.Controls.Add(this.labelX3);
            this.Controls.Add(this.labelX1);
            this.Controls.Add(this.txtName);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "StateNodeForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "状态";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.StateNodeForm_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.allEventTree)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.registedEventTree)).EndInit();
            this.customEventPanel.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private DevComponents.DotNetBar.Controls.TextBoxX txtName;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.DotNetBar.ButtonX btnDel;
        private DevComponents.DotNetBar.ButtonX btnAdd;
        private DevComponents.AdvTree.AdvTree allEventTree;
        private DevComponents.AdvTree.NodeConnector nodeConnector1;
        private DevComponents.DotNetBar.ElementStyle elementStyle1;
        private DevComponents.AdvTree.AdvTree registedEventTree;
        private DevComponents.AdvTree.NodeConnector nodeConnector2;
        private DevComponents.DotNetBar.ElementStyle elementStyle2;
        private DevComponents.DotNetBar.LabelX labelX3;
        private DevComponents.DotNetBar.LabelX labelX4;
        private DevComponents.DotNetBar.ButtonX buttonX2;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private System.Windows.Forms.ImageList imageList1;
        private DevComponents.DotNetBar.ButtonX bNewCustomEvent;
        private DevComponents.DotNetBar.ButtonX bDeleteCustomEvent;
        private DevComponents.DotNetBar.Controls.TextBoxX textBoxX1;
        private DevComponents.DotNetBar.Controls.TextBoxX informationBox;
        private DevComponents.DotNetBar.ButtonX bRenameCustomEvent;
        private DevComponents.DotNetBar.Controls.GroupPanel customEventPanel;
    }
}