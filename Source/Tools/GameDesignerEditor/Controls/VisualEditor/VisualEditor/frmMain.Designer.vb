<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMain
    Inherits System.Windows.Forms.Form

    'Form 重写 Dispose，以清理组件列表。
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Windows 窗体设计器所必需的
    Private components As System.ComponentModel.IContainer

    '注意: 以下过程是 Windows 窗体设计器所必需的
    '可以使用 Windows 窗体设计器修改它。
    '不要使用代码编辑器修改它。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
        Me.res8 = New DevComponents.DotNetBar.Controls.TextBoxX
        Me.res7 = New DevComponents.DotNetBar.Controls.TextBoxX
        Me.txtShow = New System.Windows.Forms.RichTextBox
        Me.popMenu = New System.Windows.Forms.ContextMenuStrip(Me.components)
        Me.popInsertImg = New System.Windows.Forms.ToolStripMenuItem
        Me.ToolStripSeparator2 = New System.Windows.Forms.ToolStripSeparator
        Me.popInsertXLable = New System.Windows.Forms.ToolStripMenuItem
        Me.popInsertXImg = New System.Windows.Forms.ToolStripMenuItem
        Me.popInsertXShop = New System.Windows.Forms.ToolStripMenuItem
        Me.ToolStripSeparator3 = New System.Windows.Forms.ToolStripSeparator
        Me.popInsertUname = New System.Windows.Forms.ToolStripMenuItem
        Me.popInsertUbody = New System.Windows.Forms.ToolStripMenuItem
        Me.ToolStripSeparator1 = New System.Windows.Forms.ToolStripSeparator
        Me.popSet = New System.Windows.Forms.ToolStripMenuItem
        Me.EditToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.ToolStripMenuItem1 = New System.Windows.Forms.ToolStripMenuItem
        Me.ToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.LabelItem1 = New DevComponents.DotNetBar.LabelItem
        Me.Bar1 = New DevComponents.DotNetBar.Bar
        Me.btnColorCancel = New DevComponents.DotNetBar.ButtonItem
        Me.btnColorBlue = New DevComponents.DotNetBar.ButtonItem
        Me.btnColorRed = New DevComponents.DotNetBar.ButtonItem
        Me.btnColorGreen = New DevComponents.DotNetBar.ButtonItem
        Me.btnColorGray = New DevComponents.DotNetBar.ButtonItem
        Me.btnCancel = New DevComponents.DotNetBar.ButtonX
        Me.TabControlPanel2 = New DevComponents.DotNetBar.TabControlPanel
        Me.tabShow = New DevComponents.DotNetBar.TabItem(Me.components)
        Me.tabSource = New DevComponents.DotNetBar.TabItem(Me.components)
        Me.TabControlPanel1 = New DevComponents.DotNetBar.TabControlPanel
        Me.txtSource = New System.Windows.Forms.RichTextBox
        Me.btnOK = New DevComponents.DotNetBar.ButtonX
        Me.res2 = New DevComponents.DotNetBar.Controls.TextBoxX
        Me.res1 = New DevComponents.DotNetBar.Controls.TextBoxX
        Me.pannel = New DevComponents.DotNetBar.TabControl
        Me.res6 = New DevComponents.DotNetBar.Controls.TextBoxX
        Me.res5 = New DevComponents.DotNetBar.Controls.TextBoxX
        Me.panelbg = New DevComponents.DotNetBar.PanelEx
        Me.res4 = New DevComponents.DotNetBar.Controls.TextBoxX
        Me.res3 = New DevComponents.DotNetBar.Controls.TextBoxX
        Me.popMenu.SuspendLayout()
        CType(Me.Bar1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.TabControlPanel2.SuspendLayout()
        Me.TabControlPanel1.SuspendLayout()
        CType(Me.pannel, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.pannel.SuspendLayout()
        Me.panelbg.SuspendLayout()
        Me.SuspendLayout()
        '
        'res8
        '
        '
        '
        '
        Me.res8.Border.Class = "TextBoxBorder"
        Me.res8.Location = New System.Drawing.Point(3164, 356)
        Me.res8.Name = "res8"
        Me.res8.Size = New System.Drawing.Size(71, 21)
        Me.res8.TabIndex = 10
        Me.res8.Text = resources.GetString("res8.Text")
        '
        'res7
        '
        '
        '
        '
        Me.res7.Border.Class = "TextBoxBorder"
        Me.res7.Location = New System.Drawing.Point(3164, 310)
        Me.res7.Name = "res7"
        Me.res7.Size = New System.Drawing.Size(71, 21)
        Me.res7.TabIndex = 9
        Me.res7.Text = resources.GetString("res7.Text")
        '
        'txtShow
        '
        Me.txtShow.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.txtShow.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.txtShow.ContextMenuStrip = Me.popMenu
        Me.txtShow.DetectUrls = False
        Me.txtShow.Font = New System.Drawing.Font("楷体_GB2312", 15.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(134, Byte))
        Me.txtShow.Location = New System.Drawing.Point(4, 34)
        Me.txtShow.Name = "txtShow"
        Me.txtShow.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedVertical
        Me.txtShow.Size = New System.Drawing.Size(534, 547)
        Me.txtShow.TabIndex = 4
        Me.txtShow.Text = "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十"
        '
        'popMenu
        '
        Me.popMenu.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.popInsertImg, Me.ToolStripSeparator2, Me.popInsertXLable, Me.popInsertXImg, Me.popInsertXShop, Me.ToolStripSeparator3, Me.popInsertUname, Me.popInsertUbody, Me.ToolStripSeparator1, Me.popSet, Me.EditToolStripMenuItem})
        Me.popMenu.Name = "popMenu"
        Me.popMenu.Size = New System.Drawing.Size(154, 198)
        '
        'popInsertImg
        '
        Me.popInsertImg.Name = "popInsertImg"
        Me.popInsertImg.Size = New System.Drawing.Size(153, 22)
        Me.popInsertImg.Text = "插入图片"
        '
        'ToolStripSeparator2
        '
        Me.ToolStripSeparator2.Name = "ToolStripSeparator2"
        Me.ToolStripSeparator2.Size = New System.Drawing.Size(150, 6)
        '
        'popInsertXLable
        '
        Me.popInsertXLable.Name = "popInsertXLable"
        Me.popInsertXLable.Size = New System.Drawing.Size(153, 22)
        Me.popInsertXLable.Text = "插入文字选项"
        '
        'popInsertXImg
        '
        Me.popInsertXImg.Name = "popInsertXImg"
        Me.popInsertXImg.Size = New System.Drawing.Size(153, 22)
        Me.popInsertXImg.Text = "插入图片选项"
        '
        'popInsertXShop
        '
        Me.popInsertXShop.Name = "popInsertXShop"
        Me.popInsertXShop.Size = New System.Drawing.Size(153, 22)
        Me.popInsertXShop.Text = "插入商店选项"
        '
        'ToolStripSeparator3
        '
        Me.ToolStripSeparator3.Name = "ToolStripSeparator3"
        Me.ToolStripSeparator3.Size = New System.Drawing.Size(150, 6)
        '
        'popInsertUname
        '
        Me.popInsertUname.Name = "popInsertUname"
        Me.popInsertUname.Size = New System.Drawing.Size(153, 22)
        Me.popInsertUname.Text = "插入玩家名字"
        '
        'popInsertUbody
        '
        Me.popInsertUbody.Name = "popInsertUbody"
        Me.popInsertUbody.Size = New System.Drawing.Size(153, 22)
        Me.popInsertUbody.Text = "插入玩家体形"
        '
        'ToolStripSeparator1
        '
        Me.ToolStripSeparator1.Name = "ToolStripSeparator1"
        Me.ToolStripSeparator1.Size = New System.Drawing.Size(150, 6)
        '
        'popSet
        '
        Me.popSet.Name = "popSet"
        Me.popSet.ShortcutKeys = CType((System.Windows.Forms.Keys.Control Or System.Windows.Forms.Keys.P), System.Windows.Forms.Keys)
        Me.popSet.Size = New System.Drawing.Size(153, 22)
        Me.popSet.Text = "属性..."
        '
        'EditToolStripMenuItem
        '
        Me.EditToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ToolStripMenuItem1, Me.ToolStripMenuItem})
        Me.EditToolStripMenuItem.Name = "EditToolStripMenuItem"
        Me.EditToolStripMenuItem.Size = New System.Drawing.Size(153, 22)
        Me.EditToolStripMenuItem.Text = "编辑"
        '
        'ToolStripMenuItem1
        '
        Me.ToolStripMenuItem1.Name = "ToolStripMenuItem1"
        Me.ToolStripMenuItem1.ShortcutKeys = CType((System.Windows.Forms.Keys.Control Or System.Windows.Forms.Keys.C), System.Windows.Forms.Keys)
        Me.ToolStripMenuItem1.Size = New System.Drawing.Size(135, 22)
        Me.ToolStripMenuItem1.Text = "复制"
        '
        'ToolStripMenuItem
        '
        Me.ToolStripMenuItem.Name = "ToolStripMenuItem"
        Me.ToolStripMenuItem.ShortcutKeys = CType((System.Windows.Forms.Keys.Control Or System.Windows.Forms.Keys.V), System.Windows.Forms.Keys)
        Me.ToolStripMenuItem.Size = New System.Drawing.Size(135, 22)
        Me.ToolStripMenuItem.Text = "粘贴"
        '
        'LabelItem1
        '
        Me.LabelItem1.BorderType = DevComponents.DotNetBar.eBorderType.None
        Me.LabelItem1.Name = "LabelItem1"
        Me.LabelItem1.Text = "颜色"
        '
        'Bar1
        '
        Me.Bar1.DockSide = DevComponents.DotNetBar.eDockSide.Document
        Me.Bar1.Items.AddRange(New DevComponents.DotNetBar.BaseItem() {Me.LabelItem1, Me.btnColorCancel, Me.btnColorBlue, Me.btnColorRed, Me.btnColorGreen, Me.btnColorGray})
        Me.Bar1.Location = New System.Drawing.Point(4, 4)
        Me.Bar1.Name = "Bar1"
        Me.Bar1.Size = New System.Drawing.Size(533, 25)
        Me.Bar1.Style = DevComponents.DotNetBar.eDotNetBarStyle.Office2003
        Me.Bar1.TabIndex = 3
        Me.Bar1.TabStop = False
        Me.Bar1.Text = "Bar1"
        '
        'btnColorCancel
        '
        Me.btnColorCancel.ForeColor = System.Drawing.Color.Black
        Me.btnColorCancel.HotFontBold = True
        Me.btnColorCancel.Name = "btnColorCancel"
        Me.btnColorCancel.PopupFont = New System.Drawing.Font("宋体", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(134, Byte))
        Me.btnColorCancel.Stretch = True
        Me.btnColorCancel.Text = "□"
        Me.btnColorCancel.Tooltip = "清除颜色"
        '
        'btnColorBlue
        '
        Me.btnColorBlue.ForeColor = System.Drawing.Color.Blue
        Me.btnColorBlue.HotFontBold = True
        Me.btnColorBlue.Name = "btnColorBlue"
        Me.btnColorBlue.PopupFont = New System.Drawing.Font("宋体", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(134, Byte))
        Me.btnColorBlue.Stretch = True
        Me.btnColorBlue.Text = "■"
        Me.btnColorBlue.Tooltip = "蓝色"
        '
        'btnColorRed
        '
        Me.btnColorRed.ForeColor = System.Drawing.Color.Red
        Me.btnColorRed.HotFontBold = True
        Me.btnColorRed.Name = "btnColorRed"
        Me.btnColorRed.PopupFont = New System.Drawing.Font("宋体", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(134, Byte))
        Me.btnColorRed.Stretch = True
        Me.btnColorRed.Text = "■"
        Me.btnColorRed.Tooltip = "红色"
        '
        'btnColorGreen
        '
        Me.btnColorGreen.ForeColor = System.Drawing.Color.Green
        Me.btnColorGreen.HotFontBold = True
        Me.btnColorGreen.Name = "btnColorGreen"
        Me.btnColorGreen.PopupFont = New System.Drawing.Font("宋体", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(134, Byte))
        Me.btnColorGreen.Stretch = True
        Me.btnColorGreen.Text = "■"
        Me.btnColorGreen.Tooltip = "绿色"
        '
        'btnColorGray
        '
        Me.btnColorGray.ForeColor = System.Drawing.Color.Gray
        Me.btnColorGray.HotFontBold = True
        Me.btnColorGray.Name = "btnColorGray"
        Me.btnColorGray.PopupFont = New System.Drawing.Font("宋体", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(134, Byte))
        Me.btnColorGray.Stretch = True
        Me.btnColorGray.Text = "■"
        Me.btnColorGray.Tooltip = "灰色"
        '
        'btnCancel
        '
        Me.btnCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton
        Me.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.btnCancel.Location = New System.Drawing.Point(468, 621)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.Size = New System.Drawing.Size(75, 23)
        Me.btnCancel.TabIndex = 2
        Me.btnCancel.Text = "取消(&C)"
        '
        'TabControlPanel2
        '
        Me.TabControlPanel2.Controls.Add(Me.res8)
        Me.TabControlPanel2.Controls.Add(Me.res7)
        Me.TabControlPanel2.Controls.Add(Me.txtShow)
        Me.TabControlPanel2.Controls.Add(Me.Bar1)
        Me.TabControlPanel2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TabControlPanel2.Location = New System.Drawing.Point(0, 26)
        Me.TabControlPanel2.Name = "TabControlPanel2"
        Me.TabControlPanel2.Padding = New System.Windows.Forms.Padding(1)
        Me.TabControlPanel2.Size = New System.Drawing.Size(541, 586)
        Me.TabControlPanel2.Style.BackColor1.Color = System.Drawing.Color.FromArgb(CType(CType(142, Byte), Integer), CType(CType(179, Byte), Integer), CType(CType(231, Byte), Integer))
        Me.TabControlPanel2.Style.BackColor2.Color = System.Drawing.Color.FromArgb(CType(CType(223, Byte), Integer), CType(CType(237, Byte), Integer), CType(CType(254, Byte), Integer))
        Me.TabControlPanel2.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine
        Me.TabControlPanel2.Style.BorderColor.Color = System.Drawing.Color.FromArgb(CType(CType(59, Byte), Integer), CType(CType(97, Byte), Integer), CType(CType(156, Byte), Integer))
        Me.TabControlPanel2.Style.BorderSide = CType(((DevComponents.DotNetBar.eBorderSide.Left Or DevComponents.DotNetBar.eBorderSide.Right) _
                    Or DevComponents.DotNetBar.eBorderSide.Bottom), DevComponents.DotNetBar.eBorderSide)
        Me.TabControlPanel2.Style.GradientAngle = 90
        Me.TabControlPanel2.TabIndex = 2
        Me.TabControlPanel2.TabItem = Me.tabShow
        '
        'tabShow
        '
        Me.tabShow.AttachedControl = Me.TabControlPanel2
        Me.tabShow.Name = "tabShow"
        Me.tabShow.Text = "设计"
        '
        'tabSource
        '
        Me.tabSource.AttachedControl = Me.TabControlPanel1
        Me.tabSource.Name = "tabSource"
        Me.tabSource.Text = "源代码"
        '
        'TabControlPanel1
        '
        Me.TabControlPanel1.Controls.Add(Me.txtSource)
        Me.TabControlPanel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TabControlPanel1.Location = New System.Drawing.Point(0, 26)
        Me.TabControlPanel1.Name = "TabControlPanel1"
        Me.TabControlPanel1.Padding = New System.Windows.Forms.Padding(1)
        Me.TabControlPanel1.Size = New System.Drawing.Size(541, 586)
        Me.TabControlPanel1.Style.BackColor1.Color = System.Drawing.Color.FromArgb(CType(CType(142, Byte), Integer), CType(CType(179, Byte), Integer), CType(CType(231, Byte), Integer))
        Me.TabControlPanel1.Style.BackColor2.Color = System.Drawing.Color.FromArgb(CType(CType(223, Byte), Integer), CType(CType(237, Byte), Integer), CType(CType(254, Byte), Integer))
        Me.TabControlPanel1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine
        Me.TabControlPanel1.Style.BorderColor.Color = System.Drawing.Color.FromArgb(CType(CType(59, Byte), Integer), CType(CType(97, Byte), Integer), CType(CType(156, Byte), Integer))
        Me.TabControlPanel1.Style.BorderSide = CType(((DevComponents.DotNetBar.eBorderSide.Left Or DevComponents.DotNetBar.eBorderSide.Right) _
                    Or DevComponents.DotNetBar.eBorderSide.Bottom), DevComponents.DotNetBar.eBorderSide)
        Me.TabControlPanel1.Style.GradientAngle = 90
        Me.TabControlPanel1.TabIndex = 1
        Me.TabControlPanel1.TabItem = Me.tabSource
        '
        'txtSource
        '
        Me.txtSource.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.txtSource.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.txtSource.DetectUrls = False
        Me.txtSource.Location = New System.Drawing.Point(3, 0)
        Me.txtSource.Name = "txtSource"
        Me.txtSource.Size = New System.Drawing.Size(534, 582)
        Me.txtSource.TabIndex = 5
        Me.txtSource.Text = ""
        '
        'btnOK
        '
        Me.btnOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton
        Me.btnOK.Location = New System.Drawing.Point(382, 621)
        Me.btnOK.Name = "btnOK"
        Me.btnOK.Size = New System.Drawing.Size(80, 23)
        Me.btnOK.TabIndex = 1
        Me.btnOK.Text = "确定(&O)"
        '
        'res2
        '
        '
        '
        '
        Me.res2.Border.Class = "TextBoxBorder"
        Me.res2.Location = New System.Drawing.Point(2643, 73)
        Me.res2.Name = "res2"
        Me.res2.Size = New System.Drawing.Size(71, 21)
        Me.res2.TabIndex = 5
        Me.res2.Text = resources.GetString("res2.Text")
        '
        'res1
        '
        '
        '
        '
        Me.res1.Border.Class = "TextBoxBorder"
        Me.res1.Location = New System.Drawing.Point(2643, 46)
        Me.res1.Name = "res1"
        Me.res1.Size = New System.Drawing.Size(71, 21)
        Me.res1.TabIndex = 4
        Me.res1.Text = resources.GetString("res1.Text")
        '
        'pannel
        '
        Me.pannel.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pannel.BackColor = System.Drawing.Color.Transparent
        Me.pannel.CanReorderTabs = True
        Me.pannel.Controls.Add(Me.TabControlPanel2)
        Me.pannel.Controls.Add(Me.TabControlPanel1)
        Me.pannel.Location = New System.Drawing.Point(3, 3)
        Me.pannel.Name = "pannel"
        Me.pannel.SelectedTabFont = New System.Drawing.Font("宋体", 9.0!, System.Drawing.FontStyle.Bold)
        Me.pannel.SelectedTabIndex = 1
        Me.pannel.Size = New System.Drawing.Size(541, 612)
        Me.pannel.TabIndex = 3
        Me.pannel.TabLayoutType = DevComponents.DotNetBar.eTabLayoutType.FixedWithNavigationBox
        Me.pannel.Tabs.Add(Me.tabSource)
        Me.pannel.Tabs.Add(Me.tabShow)
        Me.pannel.Text = "TabControl1"
        '
        'res6
        '
        '
        '
        '
        Me.res6.Border.Class = "TextBoxBorder"
        Me.res6.Location = New System.Drawing.Point(3343, 274)
        Me.res6.Name = "res6"
        Me.res6.Size = New System.Drawing.Size(71, 21)
        Me.res6.TabIndex = 8
        Me.res6.Text = resources.GetString("res6.Text")
        '
        'res5
        '
        '
        '
        '
        Me.res5.Border.Class = "TextBoxBorder"
        Me.res5.Location = New System.Drawing.Point(2643, 213)
        Me.res5.Name = "res5"
        Me.res5.Size = New System.Drawing.Size(71, 21)
        Me.res5.TabIndex = 8
        Me.res5.Text = resources.GetString("res5.Text")
        '
        'panelbg
        '
        Me.panelbg.Controls.Add(Me.res6)
        Me.panelbg.Controls.Add(Me.res5)
        Me.panelbg.Controls.Add(Me.res4)
        Me.panelbg.Controls.Add(Me.res3)
        Me.panelbg.Controls.Add(Me.res2)
        Me.panelbg.Controls.Add(Me.res1)
        Me.panelbg.Controls.Add(Me.pannel)
        Me.panelbg.Controls.Add(Me.btnCancel)
        Me.panelbg.Controls.Add(Me.btnOK)
        Me.panelbg.Dock = System.Windows.Forms.DockStyle.Fill
        Me.panelbg.Location = New System.Drawing.Point(0, 0)
        Me.panelbg.Name = "panelbg"
        Me.panelbg.Size = New System.Drawing.Size(547, 651)
        Me.panelbg.Style.Alignment = System.Drawing.StringAlignment.Center
        Me.panelbg.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground
        Me.panelbg.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2
        Me.panelbg.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder
        Me.panelbg.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText
        Me.panelbg.Style.GradientAngle = 90
        Me.panelbg.TabIndex = 6
        '
        'res4
        '
        '
        '
        '
        Me.res4.Border.Class = "TextBoxBorder"
        Me.res4.Location = New System.Drawing.Point(2643, 151)
        Me.res4.Name = "res4"
        Me.res4.Size = New System.Drawing.Size(71, 21)
        Me.res4.TabIndex = 7
        Me.res4.Text = resources.GetString("res4.Text")
        '
        'res3
        '
        '
        '
        '
        Me.res3.Border.Class = "TextBoxBorder"
        Me.res3.Location = New System.Drawing.Point(2643, 112)
        Me.res3.Name = "res3"
        Me.res3.Size = New System.Drawing.Size(71, 21)
        Me.res3.TabIndex = 6
        Me.res3.Text = resources.GetString("res3.Text")
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.CancelButton = Me.btnCancel
        Me.ClientSize = New System.Drawing.Size(547, 651)
        Me.Controls.Add(Me.panelbg)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.Name = "frmMain"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "Visual Editor(version 1.0.10.0)"
        Me.popMenu.ResumeLayout(False)
        CType(Me.Bar1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.TabControlPanel2.ResumeLayout(False)
        Me.TabControlPanel1.ResumeLayout(False)
        CType(Me.pannel, System.ComponentModel.ISupportInitialize).EndInit()
        Me.pannel.ResumeLayout(False)
        Me.panelbg.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents res8 As DevComponents.DotNetBar.Controls.TextBoxX
    Friend WithEvents res7 As DevComponents.DotNetBar.Controls.TextBoxX
    Friend WithEvents txtShow As System.Windows.Forms.RichTextBox
    Friend WithEvents popMenu As System.Windows.Forms.ContextMenuStrip
    Friend WithEvents ToolStripSeparator1 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents popSet As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents LabelItem1 As DevComponents.DotNetBar.LabelItem
    Friend WithEvents Bar1 As DevComponents.DotNetBar.Bar
    Friend WithEvents btnColorCancel As DevComponents.DotNetBar.ButtonItem
    Friend WithEvents btnColorBlue As DevComponents.DotNetBar.ButtonItem
    Friend WithEvents btnColorRed As DevComponents.DotNetBar.ButtonItem
    Friend WithEvents btnColorGreen As DevComponents.DotNetBar.ButtonItem
    Friend WithEvents btnColorGray As DevComponents.DotNetBar.ButtonItem
    Private WithEvents btnCancel As DevComponents.DotNetBar.ButtonX
    Friend WithEvents TabControlPanel2 As DevComponents.DotNetBar.TabControlPanel
    Friend WithEvents tabShow As DevComponents.DotNetBar.TabItem
    Friend WithEvents tabSource As DevComponents.DotNetBar.TabItem
    Friend WithEvents TabControlPanel1 As DevComponents.DotNetBar.TabControlPanel
    Friend WithEvents txtSource As System.Windows.Forms.RichTextBox
    Private WithEvents btnOK As DevComponents.DotNetBar.ButtonX
    Friend WithEvents res2 As DevComponents.DotNetBar.Controls.TextBoxX
    Public WithEvents res1 As DevComponents.DotNetBar.Controls.TextBoxX
    Friend WithEvents pannel As DevComponents.DotNetBar.TabControl
    Friend WithEvents res6 As DevComponents.DotNetBar.Controls.TextBoxX
    Friend WithEvents res5 As DevComponents.DotNetBar.Controls.TextBoxX
    Private WithEvents panelbg As DevComponents.DotNetBar.PanelEx
    Friend WithEvents res4 As DevComponents.DotNetBar.Controls.TextBoxX
    Friend WithEvents res3 As DevComponents.DotNetBar.Controls.TextBoxX
    Friend WithEvents EditToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ToolStripMenuItem1 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents popInsertImg As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents popInsertXLable As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents popInsertXImg As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents popInsertXShop As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ToolStripSeparator2 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents ToolStripSeparator3 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents popInsertUname As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents popInsertUbody As System.Windows.Forms.ToolStripMenuItem
End Class
