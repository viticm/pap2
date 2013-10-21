Imports System.Windows.Forms
Imports System.Resources
Imports System.Drawing

Public Class frmSelImg

    Private Sub cmdCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmdCancel.Click
        Me.Hide()

    End Sub

    Private Sub btnOK_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnOK.Click
        '检查有效性
        If Not IsNumeric(txtID.Text) Then
            MsgBox("请输入有效ID")
            Exit Sub
        End If

        '插入图片
        Dim tString As String

        '备份剪切板
        Dim str As String = ""
        str = System.Windows.Forms.Clipboard.GetText()

        '生成rtf
        tString = isr.tga2rtf(txtID.Text, conn, RootDir & "\ui\Image\icon")

        '恢复剪切板
        'System.Windows.Forms.Clipboard.SetDataObject(obj)
        If str <> "" Then System.Windows.Forms.Clipboard.SetText(str)

        If tString = "" Then
            tString = Module1.frmMain.res1.Text
        Else
            If cbClose.Checked = True Then
                tString = Split(tString, "wmetafile8\picw")(0) & "wmetafile8\picw1005" & "\pich1000\picwgoal" & Split(tString, "\picwgoal")(1)
            Else
                tString = Split(tString, "wmetafile8\picw")(0) & "wmetafile8\picw1004" & "\pich1000\picwgoal" & Split(tString, "\picwgoal")(1)
            End If
        End If
        '保存数据至hash表
        hiddenData.Add(hiddenData.Count + 1, txtID.Text)

        '观察是否需要自动加回车上去
        With Module1.frmMain.txtShow
            If .SelectionStart > 0 Then
                Dim tstart As Integer = 0
                Dim tlen As Integer = 0
                tstart = .SelectionStart
                tlen = .SelectionLength
                .SelectionStart -= 1
                .SelectionLength = 1
                If .SelectionBackColor = Drawing.Color.Yellow Then
                    .SelectionStart += 1
                    .SelectionLength = 0
                    .SelectedText = vbLf
                    tstart += 1
                End If
                .SelectionStart = tstart
                .SelectionLength = tlen
            End If
        End With

        '追加图片
        tString = Split(tString, "\pich1000")(0) & "\pich" & (1000 + hiddenData.Count) & Split(tString, "\pich1000")(1)
        Module1.frmMain.txtShow.SelectedRtf = tString
        Me.Hide()
    End Sub

    Private Sub txtID_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles txtID.KeyDown
        If e.KeyCode = 13 Then
            Call btnOK_Click(sender, e)
        End If
    End Sub

    Private Sub txtID_SizeChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles txtID.SizeChanged

    End Sub

    Private Sub txtID_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtID.TextChanged

    End Sub

    Private Sub txtSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtSelect.Click
        Dim t As IconSelector.Implement = Module1.isDiag
        Dim r As Integer
        conn.Open()
        If txtID.Text = "" Then
            t.IconID = -1
        Else
            t.IconID = txtID.Text
        End If
        If bisInit = False Then
            bisInit = True
            t.Init(conn, (RootDir & "\ui\Image\icon"))
        End If
        r = t.Show()
        If r <> -1 Then txtID.Text = r
        conn.Close()
    End Sub
End Class