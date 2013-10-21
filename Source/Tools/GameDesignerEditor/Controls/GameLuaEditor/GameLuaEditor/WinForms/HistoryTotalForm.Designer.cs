namespace GameLuaEditor
{
    partial class HistoryTotalForm
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
            if (Conn.State == System.Data.ConnectionState.Open)
            {
                Conn.Close();
            }
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
            this.checkboxHost = new DevComponents.DotNetBar.Controls.CheckBoxX();
            this.iIHourEnd = new DevComponents.Editors.IntegerInput();
            this.iiHourBegin = new DevComponents.Editors.IntegerInput();
            this.iIMinEnd = new DevComponents.Editors.IntegerInput();
            this.iIMinBegin = new DevComponents.Editors.IntegerInput();
            this.btxQurey = new DevComponents.DotNetBar.ButtonX();
            this.tbxHost = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX6 = new DevComponents.DotNetBar.LabelX();
            this.labelX3 = new DevComponents.DotNetBar.LabelX();
            this.labelX5 = new DevComponents.DotNetBar.LabelX();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.labelX4 = new DevComponents.DotNetBar.LabelX();
            this.dtiEnd = new DevComponents.Editors.DateTimeAdv.DateTimeInput();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.dtiBegin = new DevComponents.Editors.DateTimeAdv.DateTimeInput();
            this.btnQuit = new DevComponents.DotNetBar.ButtonX();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.treeViewQureyResult = new System.Windows.Forms.TreeView();
            this.tbxData = new DevComponents.DotNetBar.Controls.TextBoxX();
            ((System.ComponentModel.ISupportInitialize)(this.iIHourEnd)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.iiHourBegin)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.iIMinEnd)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.iIMinBegin)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dtiEnd)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dtiBegin)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // checkboxHost
            // 
            this.checkboxHost.Location = new System.Drawing.Point(699, 12);
            this.checkboxHost.Name = "checkboxHost";
            this.checkboxHost.Size = new System.Drawing.Size(92, 25);
            this.checkboxHost.TabIndex = 8;
            this.checkboxHost.Text = "操作者筛选:";
            this.checkboxHost.CheckedChanged += new System.EventHandler(this.checkboxHost_CheckedChanged);
            // 
            // iIHourEnd
            // 
            // 
            // 
            // 
            this.iIHourEnd.BackgroundStyle.Class = "DateTimeInputBackground";
            this.iIHourEnd.Location = new System.Drawing.Point(589, 12);
            this.iIHourEnd.MaxValue = 23;
            this.iIHourEnd.MinValue = 0;
            this.iIHourEnd.Name = "iIHourEnd";
            this.iIHourEnd.ShowUpDown = true;
            this.iIHourEnd.Size = new System.Drawing.Size(38, 21);
            this.iIHourEnd.TabIndex = 6;
            // 
            // iiHourBegin
            // 
            // 
            // 
            // 
            this.iiHourBegin.BackgroundStyle.Class = "DateTimeInputBackground";
            this.iiHourBegin.Location = new System.Drawing.Point(222, 12);
            this.iiHourBegin.MaxValue = 23;
            this.iiHourBegin.MinValue = 0;
            this.iiHourBegin.Name = "iiHourBegin";
            this.iiHourBegin.ShowUpDown = true;
            this.iiHourBegin.Size = new System.Drawing.Size(38, 21);
            this.iiHourBegin.TabIndex = 3;
            // 
            // iIMinEnd
            // 
            // 
            // 
            // 
            this.iIMinEnd.BackgroundStyle.Class = "DateTimeInputBackground";
            this.iIMinEnd.Location = new System.Drawing.Point(655, 11);
            this.iIMinEnd.MaxValue = 59;
            this.iIMinEnd.MinValue = 0;
            this.iIMinEnd.Name = "iIMinEnd";
            this.iIMinEnd.ShowUpDown = true;
            this.iIMinEnd.Size = new System.Drawing.Size(38, 21);
            this.iIMinEnd.TabIndex = 7;
            // 
            // iIMinBegin
            // 
            // 
            // 
            // 
            this.iIMinBegin.BackgroundStyle.Class = "DateTimeInputBackground";
            this.iIMinBegin.Location = new System.Drawing.Point(302, 13);
            this.iIMinBegin.MaxValue = 59;
            this.iIMinBegin.MinValue = 0;
            this.iIMinBegin.Name = "iIMinBegin";
            this.iIMinBegin.ShowUpDown = true;
            this.iIMinBegin.Size = new System.Drawing.Size(38, 21);
            this.iIMinBegin.TabIndex = 4;
            // 
            // btxQurey
            // 
            this.btxQurey.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btxQurey.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btxQurey.Location = new System.Drawing.Point(925, 12);
            this.btxQurey.Name = "btxQurey";
            this.btxQurey.Size = new System.Drawing.Size(75, 23);
            this.btxQurey.TabIndex = 1;
            this.btxQurey.Text = "查询";
            this.btxQurey.Click += new System.EventHandler(this.btxQurey_Click);
            // 
            // tbxHost
            // 
            // 
            // 
            // 
            this.tbxHost.Border.Class = "TextBoxBorder";
            this.tbxHost.Location = new System.Drawing.Point(797, 12);
            this.tbxHost.Name = "tbxHost";
            this.tbxHost.Size = new System.Drawing.Size(105, 21);
            this.tbxHost.TabIndex = 9;
            this.tbxHost.Visible = false;
            // 
            // labelX6
            // 
            this.labelX6.Location = new System.Drawing.Point(632, 14);
            this.labelX6.Name = "labelX6";
            this.labelX6.Size = new System.Drawing.Size(39, 23);
            this.labelX6.TabIndex = 28;
            this.labelX6.Text = "分：";
            // 
            // labelX3
            // 
            this.labelX3.Location = new System.Drawing.Point(275, 15);
            this.labelX3.Name = "labelX3";
            this.labelX3.Size = new System.Drawing.Size(35, 23);
            this.labelX3.TabIndex = 24;
            this.labelX3.Text = "分：";
            // 
            // labelX5
            // 
            this.labelX5.Location = new System.Drawing.Point(564, 12);
            this.labelX5.Name = "labelX5";
            this.labelX5.Size = new System.Drawing.Size(34, 23);
            this.labelX5.TabIndex = 23;
            this.labelX5.Text = "时：";
            // 
            // labelX2
            // 
            this.labelX2.Location = new System.Drawing.Point(197, 12);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(34, 23);
            this.labelX2.TabIndex = 25;
            this.labelX2.Text = "时：";
            // 
            // labelX4
            // 
            this.labelX4.Location = new System.Drawing.Point(375, 12);
            this.labelX4.Name = "labelX4";
            this.labelX4.Size = new System.Drawing.Size(67, 23);
            this.labelX4.TabIndex = 27;
            this.labelX4.Text = "结束时间：";
            // 
            // dtiEnd
            // 
            // 
            // 
            // 
            this.dtiEnd.BackgroundStyle.Class = "DateTimeInputBackground";
            this.dtiEnd.ButtonDropDown.Visible = true;
            this.dtiEnd.Location = new System.Drawing.Point(442, 14);
            // 
            // 
            // 
            this.dtiEnd.MonthCalendar.AnnuallyMarkedDates = new System.DateTime[0];
            this.dtiEnd.MonthCalendar.ClearButtonVisible = true;
            // 
            // 
            // 
            this.dtiEnd.MonthCalendar.CommandsBackgroundStyle.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.dtiEnd.MonthCalendar.CommandsBackgroundStyle.BackColorGradientAngle = 90;
            this.dtiEnd.MonthCalendar.CommandsBackgroundStyle.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.dtiEnd.MonthCalendar.CommandsBackgroundStyle.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.dtiEnd.MonthCalendar.CommandsBackgroundStyle.BorderTopColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.dtiEnd.MonthCalendar.CommandsBackgroundStyle.BorderTopWidth = 1;
            this.dtiEnd.MonthCalendar.DisplayMonth = new System.DateTime(2008, 10, 1, 0, 0, 0, 0);
            this.dtiEnd.MonthCalendar.MarkedDates = new System.DateTime[0];
            this.dtiEnd.MonthCalendar.MonthlyMarkedDates = new System.DateTime[0];
            // 
            // 
            // 
            this.dtiEnd.MonthCalendar.NavigationBackgroundStyle.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.dtiEnd.MonthCalendar.NavigationBackgroundStyle.BackColorGradientAngle = 90;
            this.dtiEnd.MonthCalendar.NavigationBackgroundStyle.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.dtiEnd.MonthCalendar.TodayButtonVisible = true;
            this.dtiEnd.Name = "dtiEnd";
            this.dtiEnd.Size = new System.Drawing.Size(109, 21);
            this.dtiEnd.TabIndex = 5;
            this.dtiEnd.Value = new System.DateTime(2008, 10, 21, 17, 27, 49, 921);
            // 
            // labelX1
            // 
            this.labelX1.Location = new System.Drawing.Point(8, 12);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(67, 23);
            this.labelX1.TabIndex = 26;
            this.labelX1.Text = "开始时间：";
            // 
            // dtiBegin
            // 
            // 
            // 
            // 
            this.dtiBegin.BackgroundStyle.Class = "DateTimeInputBackground";
            this.dtiBegin.ButtonDropDown.Visible = true;
            this.dtiBegin.Location = new System.Drawing.Point(75, 14);
            // 
            // 
            // 
            this.dtiBegin.MonthCalendar.AnnuallyMarkedDates = new System.DateTime[0];
            this.dtiBegin.MonthCalendar.ClearButtonVisible = true;
            // 
            // 
            // 
            this.dtiBegin.MonthCalendar.CommandsBackgroundStyle.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2;
            this.dtiBegin.MonthCalendar.CommandsBackgroundStyle.BackColorGradientAngle = 90;
            this.dtiBegin.MonthCalendar.CommandsBackgroundStyle.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground;
            this.dtiBegin.MonthCalendar.CommandsBackgroundStyle.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.dtiBegin.MonthCalendar.CommandsBackgroundStyle.BorderTopColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder;
            this.dtiBegin.MonthCalendar.CommandsBackgroundStyle.BorderTopWidth = 1;
            this.dtiBegin.MonthCalendar.DisplayMonth = new System.DateTime(2008, 10, 1, 0, 0, 0, 0);
            this.dtiBegin.MonthCalendar.MarkedDates = new System.DateTime[0];
            this.dtiBegin.MonthCalendar.MonthlyMarkedDates = new System.DateTime[0];
            // 
            // 
            // 
            this.dtiBegin.MonthCalendar.NavigationBackgroundStyle.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.dtiBegin.MonthCalendar.NavigationBackgroundStyle.BackColorGradientAngle = 90;
            this.dtiBegin.MonthCalendar.NavigationBackgroundStyle.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.dtiBegin.MonthCalendar.TodayButtonVisible = true;
            this.dtiBegin.Name = "dtiBegin";
            this.dtiBegin.Size = new System.Drawing.Size(109, 21);
            this.dtiBegin.TabIndex = 2;
            this.dtiBegin.Value = new System.DateTime(2008, 10, 21, 17, 30, 57, 921);
            this.dtiBegin.Leave += new System.EventHandler(this.dtiBegin_Leave);
            // 
            // btnQuit
            // 
            this.btnQuit.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btnQuit.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btnQuit.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnQuit.Location = new System.Drawing.Point(925, 489);
            this.btnQuit.Name = "btnQuit";
            this.btnQuit.Size = new System.Drawing.Size(75, 23);
            this.btnQuit.TabIndex = 36;
            this.btnQuit.Text = "退出";
            this.btnQuit.Visible = false;
            this.btnQuit.Click += new System.EventHandler(this.btnQuit_Click);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer1.Location = new System.Drawing.Point(12, 41);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.treeViewQureyResult);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.tbxData);
            this.splitContainer1.Size = new System.Drawing.Size(1051, 459);
            this.splitContainer1.SplitterDistance = 349;
            this.splitContainer1.TabIndex = 39;
            // 
            // treeViewQureyResult
            // 
            this.treeViewQureyResult.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeViewQureyResult.Location = new System.Drawing.Point(0, 0);
            this.treeViewQureyResult.Name = "treeViewQureyResult";
            this.treeViewQureyResult.Size = new System.Drawing.Size(349, 459);
            this.treeViewQureyResult.TabIndex = 41;
            this.treeViewQureyResult.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeViewQureyResult_AfterSelect);
            // 
            // tbxData
            // 
            // 
            // 
            // 
            this.tbxData.Border.Class = "TextBoxBorder";
            this.tbxData.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxData.Location = new System.Drawing.Point(0, 0);
            this.tbxData.Multiline = true;
            this.tbxData.Name = "tbxData";
            this.tbxData.ReadOnly = true;
            this.tbxData.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.tbxData.Size = new System.Drawing.Size(698, 459);
            this.tbxData.TabIndex = 2;
            // 
            // HistoryTotalForm
            // 
            this.AcceptButton = this.btxQurey;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnQuit;
            this.ClientSize = new System.Drawing.Size(1075, 512);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.btnQuit);
            this.Controls.Add(this.checkboxHost);
            this.Controls.Add(this.iIHourEnd);
            this.Controls.Add(this.iiHourBegin);
            this.Controls.Add(this.iIMinEnd);
            this.Controls.Add(this.iIMinBegin);
            this.Controls.Add(this.btxQurey);
            this.Controls.Add(this.tbxHost);
            this.Controls.Add(this.labelX6);
            this.Controls.Add(this.labelX3);
            this.Controls.Add(this.labelX5);
            this.Controls.Add(this.labelX2);
            this.Controls.Add(this.labelX4);
            this.Controls.Add(this.dtiEnd);
            this.Controls.Add(this.labelX1);
            this.Controls.Add(this.dtiBegin);
            this.Name = "HistoryTotalForm";
            this.Text = "HistoryTotalForm";
            this.Load += new System.EventHandler(this.HistoryTotalForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this.iIHourEnd)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.iiHourBegin)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.iIMinEnd)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.iIMinBegin)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dtiEnd)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dtiBegin)).EndInit();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.Controls.CheckBoxX checkboxHost;
        private DevComponents.Editors.IntegerInput iIHourEnd;
        private DevComponents.Editors.IntegerInput iiHourBegin;
        private DevComponents.Editors.IntegerInput iIMinEnd;
        private DevComponents.Editors.IntegerInput iIMinBegin;
        private DevComponents.DotNetBar.ButtonX btxQurey;
        private DevComponents.DotNetBar.Controls.TextBoxX tbxHost;
        private DevComponents.DotNetBar.LabelX labelX6;
        private DevComponents.DotNetBar.LabelX labelX3;
        private DevComponents.DotNetBar.LabelX labelX5;
        private DevComponents.DotNetBar.LabelX labelX2;
        private DevComponents.DotNetBar.LabelX labelX4;
        private DevComponents.Editors.DateTimeAdv.DateTimeInput dtiEnd;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.Editors.DateTimeAdv.DateTimeInput dtiBegin;
        private DevComponents.DotNetBar.ButtonX btnQuit;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TreeView treeViewQureyResult;
        private DevComponents.DotNetBar.Controls.TextBoxX tbxData;

    }
}