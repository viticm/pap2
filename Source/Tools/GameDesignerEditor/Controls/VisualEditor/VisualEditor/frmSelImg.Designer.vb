<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmSelImg
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
        Me.cbClose = New DevComponents.DotNetBar.Controls.CheckBoxX
        Me.Label1 = New System.Windows.Forms.Label
        Me.txtID = New DevComponents.DotNetBar.Controls.TextBoxX
        Me.GroupPanel1 = New DevComponents.DotNetBar.Controls.GroupPanel
        Me.txtSelect = New DevComponents.DotNetBar.ButtonX
        Me.cmdCancel = New DevComponents.DotNetBar.ButtonX
        Me.PanelEx1 = New DevComponents.DotNetBar.PanelEx
        Me.btnOK = New DevComponents.DotNetBar.ButtonX
        Me.GroupPanel1.SuspendLayout()
        Me.PanelEx1.SuspendLayout()
        Me.SuspendLayout()
        '
        'cbClose
        '
        Me.cbClose.Location = New System.Drawing.Point(18, 49)
        Me.cbClose.Name = "cbClose"
        Me.cbClose.Size = New System.Drawing.Size(78, 22)
        Me.cbClose.TabIndex = 5
        Me.cbClose.Text = "自动关闭"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(16, 18)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(47, 12)
        Me.Label1.TabIndex = 3
        Me.Label1.Text = "图片ID:"
        '
        'txtID
        '
        '
        '
        '
        Me.txtID.Border.Class = "TextBoxBorder"
        Me.txtID.Location = New System.Drawing.Point(69, 9)
        Me.txtID.Name = "txtID"
        Me.txtID.Size = New System.Drawing.Size(99, 21)
        Me.txtID.TabIndex = 1
        '
        'GroupPanel1
        '
        Me.GroupPanel1.CanvasColor = System.Drawing.SystemColors.Control
        Me.GroupPanel1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007
        Me.GroupPanel1.Controls.Add(Me.txtSelect)
        Me.GroupPanel1.Controls.Add(Me.cbClose)
        Me.GroupPanel1.Controls.Add(Me.Label1)
        Me.GroupPanel1.Controls.Add(Me.txtID)
        Me.GroupPanel1.Location = New System.Drawing.Point(12, 12)
        Me.GroupPanel1.Name = "GroupPanel1"
        Me.GroupPanel1.Size = New System.Drawing.Size(331, 97)
        '
        '
        '
        Me.GroupPanel1.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2
        Me.GroupPanel1.Style.BackColorGradientAngle = 90
        Me.GroupPanel1.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground
        Me.GroupPanel1.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid
        Me.GroupPanel1.Style.BorderBottomWidth = 1
        Me.GroupPanel1.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder
        Me.GroupPanel1.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid
        Me.GroupPanel1.Style.BorderLeftWidth = 1
        Me.GroupPanel1.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid
        Me.GroupPanel1.Style.BorderRightWidth = 1
        Me.GroupPanel1.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid
        Me.GroupPanel1.Style.BorderTopWidth = 1
        Me.GroupPanel1.Style.CornerDiameter = 4
        Me.GroupPanel1.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded
        Me.GroupPanel1.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center
        Me.GroupPanel1.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText
        Me.GroupPanel1.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near
        Me.GroupPanel1.TabIndex = 6
        '
        'txtSelect
        '
        Me.txtSelect.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton
        Me.txtSelect.Location = New System.Drawing.Point(202, 9)
        Me.txtSelect.Name = "txtSelect"
        Me.txtSelect.Size = New System.Drawing.Size(36, 21)
        Me.txtSelect.TabIndex = 6
        Me.txtSelect.Text = "..."
        '
        'cmdCancel
        '
        Me.cmdCancel.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton
        Me.cmdCancel.Location = New System.Drawing.Point(259, 115)
        Me.cmdCancel.Name = "cmdCancel"
        Me.cmdCancel.Size = New System.Drawing.Size(85, 26)
        Me.cmdCancel.TabIndex = 8
        Me.cmdCancel.Text = "取消(&C)"
        '
        'PanelEx1
        '
        Me.PanelEx1.CanvasColor = System.Drawing.SystemColors.Control
        Me.PanelEx1.Controls.Add(Me.cmdCancel)
        Me.PanelEx1.Controls.Add(Me.btnOK)
        Me.PanelEx1.Controls.Add(Me.GroupPanel1)
        Me.PanelEx1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.PanelEx1.Location = New System.Drawing.Point(0, 0)
        Me.PanelEx1.Name = "PanelEx1"
        Me.PanelEx1.Size = New System.Drawing.Size(353, 151)
        Me.PanelEx1.Style.Alignment = System.Drawing.StringAlignment.Center
        Me.PanelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground
        Me.PanelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarBackground2
        Me.PanelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.BarDockedBorder
        Me.PanelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.ItemText
        Me.PanelEx1.Style.GradientAngle = 90
        Me.PanelEx1.TabIndex = 2
        '
        'btnOK
        '
        Me.btnOK.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton
        Me.btnOK.Location = New System.Drawing.Point(168, 115)
        Me.btnOK.Name = "btnOK"
        Me.btnOK.Size = New System.Drawing.Size(85, 26)
        Me.btnOK.TabIndex = 7
        Me.btnOK.Text = "确定(&O)"
        '
        'frmSelImg
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(353, 151)
        Me.Controls.Add(Me.PanelEx1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.Name = "frmSelImg"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "图片选项"
        Me.GroupPanel1.ResumeLayout(False)
        Me.GroupPanel1.PerformLayout()
        Me.PanelEx1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents cbClose As DevComponents.DotNetBar.Controls.CheckBoxX
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents txtID As DevComponents.DotNetBar.Controls.TextBoxX
    Friend WithEvents GroupPanel1 As DevComponents.DotNetBar.Controls.GroupPanel
    Friend WithEvents cmdCancel As DevComponents.DotNetBar.ButtonX
    Friend WithEvents PanelEx1 As DevComponents.DotNetBar.PanelEx
    Friend WithEvents btnOK As DevComponents.DotNetBar.ButtonX
    Friend WithEvents txtSelect As DevComponents.DotNetBar.ButtonX
End Class
