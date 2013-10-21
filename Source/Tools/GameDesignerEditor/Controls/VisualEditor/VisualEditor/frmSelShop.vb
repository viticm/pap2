Imports System.Windows.Forms
Imports System.Resources
Imports System.Drawing
Public Class frmSelShop

    Private Sub cmdCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmdCancel.Click
        Me.Hide()
    End Sub

    Private Sub btnOK_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnOK.Click

        Dim tstart As Integer = 0
        Dim tlen As Integer = 0

        '检查有效性
        If txtID.Text = "" Then
            MsgBox("请输入编号")
            Exit Sub
        End If


        '检查有效性
        If Not IsNumeric(txtID.Text) Then
            MsgBox("请输入有效ID")
            Exit Sub
        End If

        Dim i As Integer
        Dim tmps As String = ""
        For i = 1 To Len(txtLabel.Text)
            tmps &= charReplace(Mid(txtLabel.Text, i, 1))
        Next
        If tmps <> txtLabel.Text Then
            i = MsgBox("输入文字不符合规范，执行自动修正？", Microsoft.VisualBasic.MsgBoxStyle.YesNo + Microsoft.VisualBasic.MsgBoxStyle.Exclamation, "询问")
            If i = vbYes Then
                txtLabel.Text = tmps
            Else
                Exit Sub
            End If
        End If

        '插入图片
        Dim tString As String
        tString = Module1.frmMain.res6.Text

        '保存数据至hash表
        If txtLabel.Text = "" Then
            hiddenData.Add(hiddenData.Count + 1, txtID.Text)
        Else
            hiddenData.Add(hiddenData.Count + 1, txtID.Text & " " & txtLabel.Text)
        End If

        'MsgBox("out:" & hiddenData(hiddenData.Count))

        With Module1.frmMain.txtShow
            '观察是否需要自动加回车上去
            If .SelectionStart > 0 Then
                tstart = .SelectionStart
                tlen = .SelectionLength
                .SelectionStart -= 1
                .SelectionLength = 1
                If .SelectedText <> vbLf Then
                    .SelectionStart += 1
                    .SelectionLength = 0
                    .SelectedText = vbLf
                    tstart += 1
                End If
                .SelectionStart = tstart
                .SelectionLength = tlen
            End If


            '追加图片
            tString = Split(tString, "\pich1000")(0) & "\pich" & (1000 + hiddenData.Count) & Split(tString, "\pich1000")(1)
            .SelectedRtf = tString
            .SelectionLength = 0

            '选中所有选项追加的文字,并替换


            tstart = .SelectionStart
            Dim tbr As Integer
            .SelectionStart += 1
            While .SelectionBackColor = Color.Yellow
                tbr = .SelectionStart
                .SelectionStart += 1
                If .SelectionStart = tbr Then
                    tbr = -1
                    Exit While
                End If
            End While
            tlen = .SelectionStart - tstart
            If tbr = -1 Then tlen += 1

            .SelectionStart = tstart
            tlen -= 1
            If tlen < 0 Then tlen = 0
            .SelectionLength = tlen

            .SelectionBackColor = Color.Yellow
            bAutoChangeFlag = False
            If txtLabel.Text <> "" Then
                .SelectedText = txtLabel.Text
            Else
                .SelectedText = Module1.getShopName(txtID.Text) '"文字未定义，待读数据库"
            End If
            .SelectionBackColor = Color.FromName("window")


            '观察是否需要再追加回车上去
            .SelectionLength = 1
            If .SelectedText <> vbLf Then
                .SelectionLength = 0
                .SelectedText = vbLf
            End If
            bAutoChangeFlag = True

            Me.Hide()
        End With
    End Sub

    Private Sub txtLabel_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles txtLabel.KeyDown
        If e.KeyCode = 13 Then
            Call btnOK_Click(sender, e)
        End If
    End Sub

    Private Sub txtLabel_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtLabel.TextChanged

    End Sub

    Private Sub txtID_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles txtID.KeyDown
        If e.KeyCode = 13 Then
            Call btnOK_Click(sender, e)
        End If
    End Sub

    Private Sub txtID_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtID.TextChanged

    End Sub
End Class