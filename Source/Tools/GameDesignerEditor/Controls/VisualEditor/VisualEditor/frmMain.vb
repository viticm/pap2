Imports System.Windows.Forms
Imports System.Resources
Imports System.Drawing

Public Class frmMain
    Private lastTab As Integer = 2
    Private isHaveError As Boolean
    Private isCtrlDown As Boolean = False


    Private Sub tabShow_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles tabShow.Click
        If lastTab = 2 Then Exit Sub
        lastTab = 2
        dencode()
    End Sub

    Private Sub tabSource_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles tabSource.Click
        If lastTab = 1 Then Exit Sub
        lastTab = 1
        encode()
    End Sub

    Private Function ReplaceDelG(ByVal strCode As String) As String
        Return strCode.Replace("<G>", "")
    End Function

    Public Sub dencode()
        '把代码转换为界面
        txtShow.Enabled = False
        txtShow.Text = ""

        '备份剪切板
        'Dim obj As Object = New Object()
        'obj = System.Windows.Forms.Clipboard.GetDataObject()
        'System.Windows.Forms.Clipboard.
        Dim str As String = ""
        str = System.Windows.Forms.Clipboard.GetText()
        txtSource.Text = ReplaceDelG(txtSource.Text)
        hiddenData.Clear()
        Dim i As Integer
        For i = 1 To Len(txtSource.Text)
            Err.Clear()
            DoChange(Mid(txtSource.Text, i, 1))
            If Err.Number > 0 Then
                tabSource.Parent.SelectedTabIndex = 0
                lastTab = 1
                MsgBox(Err.Description)
                txtSource.SelectionStart = i - 1
                txtSource.SelectionLength = 1
                txtSource.SelectionBackColor = Color.Red
                isHaveError = True
                '恢复剪切板
                System.Windows.Forms.Clipboard.SetText(str)
                Exit Sub
            End If
        Next

        '恢复剪切板
        Try
            If str <> "" Then System.Windows.Forms.Clipboard.SetText(str)
        Catch ex As Exception

        End Try

        txtShow.Enabled = True
    End Sub

    Public Sub encode()
        '把界面转换为代码
        txtSource.Text = ""
        txtSource.Enabled = False
        txtShow.SelectionLength = 0
        DoRevChange()
        txtSource.Enabled = True
    End Sub



    Private Sub DoChange(ByVal iasc As String)
        Static sta As Integer = 1
        Static sCode As String
        Dim rCode As String = ""        '图片信息
        Dim rType As Integer            '图片类型
        On Error Resume Next
        Debug.Print(iasc & " " & sta)
        Select Case sta
            Case 1
                Select Case iasc
                    Case "\"
                        sta = 2
                        iasc = ""
                    Case "<"
                        sta = 8
                        iasc = ""
                    Case ">"
                        sta = 1
                        Err.Raise(701, , """ > ""没有"" < ""匹配!")
                    Case " "
                        sta = 1
                        iasc = ""
                    Case Else
                        sta = 1
                End Select
            Case 2
                Select Case iasc
                    Case "n"
                        sta = 3
                        iasc = vbCrLf
                    Case Else
                        sta = 1
                        Err.Raise(702, , "不存在\" & iasc & "!")
                End Select
            Case 3
                Select Case iasc
                    Case "<"
                        sta = 4
                        iasc = ""
                    Case ">"
                        sta = 1
                        Err.Raise(703, , """ > ""没有"" < ""匹配!")
                    Case "\"
                        sta = 1
                        Err.Raise(704, , "\n后不能再有\n!")
                    Case Else
                        sta = 1
                End Select
            Case 4
                Select Case iasc
                    Case "H"
                        sta = 5
                    Case "F"
                        sta = 9
                    Case "T"
                        sta = 32
                    Case "$"
                        sta = 21
                    Case "M"
                        sta = 29
                    Case "N"
                        sta = 37
                    Case "C"
                        sta = 38
                    Case Else
                        sta = 1
                        Err.Raise(705, , "不存在<" & iasc)
                End Select
                iasc = ""
            Case 5
                Select Case iasc
                    Case "2"
                        sta = 6
                        iasc = ""
                    Case Else
                        sta = 1
                        Err.Raise(706, , "目前只有<H28>这种行距控制方式")
                End Select
            Case 6
                Select Case iasc
                    Case "8"
                        sta = 7
                        iasc = ""
                    Case Else
                        sta = 1
                        Err.Raise(707, , "目前只有<H28>这种行距控制方式")
                End Select
            Case 7
                Select Case iasc
                    Case ">"
                        sta = 1
                        iasc = vbCrLf
                    Case Else
                        sta = 1
                        Err.Raise(708, , "目前只有<H28>这种行距控制方式")
                End Select
            Case 8
                Select Case iasc
                    Case "F"
                        sta = 9
                    Case "T"
                        sta = 32
                    Case "$"
                        sta = 21
                    Case "M"
                        sta = 29
                    Case "H"
                        sta = 1
                        Err.Raise(709, , "<H28>不能单独出现")
                    Case "N"
                        sta = 37
                    Case "C"
                        sta = 38
                    Case Else
                        sta = 1
                        Err.Raise(710, , "不存在<" & iasc)
                End Select
                iasc = ""
            Case 9
                Select Case iasc
                    Case "1"
                        sta = 10
                        iasc = ""
                    Case Else
                        sta = 1
                        Err.Raise(711, , "目前只有<F17X>(X=1,2,3,4)这些种着色方式")
                End Select
            Case 10
                Select Case iasc
                    Case "7"
                        sta = 11
                        iasc = ""
                    Case Else
                        sta = 1
                        Err.Raise(712, , "目前只有<F17X>(X=1,2,3,4)这些种着色方式")
                End Select
            Case 11
                Select Case iasc
                    Case "1"
                        sta = 12
                        iasc = ""
                    Case "2"
                        sta = 13
                        iasc = ""
                    Case "3"
                        sta = 14
                        iasc = ""
                    Case "4"
                        sta = 15
                        iasc = ""
                    Case Else
                        sta = 1
                        Err.Raise(713, , "目前只有<F17X>(X=1,2,3,4)这些种着色方式")
                End Select
            Case 12
                Select Case iasc
                    Case " "
                        sta = 16
                        iasc = ""
                        txtShow.SelectionColor = Color.Blue
                    Case Else
                        sta = 1
                        Err.Raise(714, , "目前只有<F17X>(X=1,2,3,4)这些种着色方式")
                End Select
            Case 13
                Select Case iasc
                    Case " "
                        sta = 17
                        iasc = ""
                        txtShow.SelectionColor = Color.Red
                    Case Else
                        sta = 1
                        Err.Raise(715, , "目前只有<F17X>(X=1,2,3,4)这些种着色方式")
                End Select
            Case 14
                Select Case iasc
                    Case " "
                        sta = 18
                        iasc = ""
                        txtShow.SelectionColor = Color.Green
                    Case Else
                        sta = 1
                        Err.Raise(716, , "目前只有<F17X>(X=1,2,3,4)这些种着色方式")
                End Select
            Case 15
                Select Case iasc
                    Case " "
                        sta = 19
                        iasc = ""
                        txtShow.SelectionColor = Color.Gray
                    Case Else
                        sta = 1
                        Err.Raise(717, , "目前只有<F17X>(X=1,2,3,4)这些种着色方式")
                End Select
            Case 16
                Select Case iasc
                    Case ">"
                        sta = 1
                        iasc = ""
                        txtShow.SelectionColor = Color.Black
                    Case "<", "\"
                        sta = 1
                        Err.Raise(718, , "不支持嵌套")
                    Case " "
                        iasc = ""
                    Case Else
                        sta = 16
                End Select
            Case 17
                Select Case iasc
                    Case ">"
                        sta = 1
                        iasc = ""
                        txtShow.SelectionColor = Color.Black
                    Case "<", "\"
                        sta = 1
                        Err.Raise(718, , "不支持嵌套")
                    Case " "
                        iasc = ""
                    Case Else
                        sta = 17
                End Select
            Case 18
                Select Case iasc
                    Case ">"
                        sta = 1
                        iasc = ""
                        txtShow.SelectionColor = Color.Black
                    Case "<", "\"
                        sta = 1
                        Err.Raise(718, , "不支持嵌套")
                    Case " "
                        iasc = ""
                    Case Else
                        sta = 18
                End Select
            Case 19
                '<F174 x
                Select Case iasc
                    Case ">"
                        sta = 1
                        iasc = ""
                        txtShow.SelectionColor = Color.Black
                    Case "<", "\"
                        sta = 1
                        Err.Raise(718, , "不支持嵌套")
                    Case " "
                        iasc = ""
                    Case Else
                        sta = 19
                End Select
            Case 20
                '<T123
                Select Case iasc
                    Case 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
                        sta = 20
                        sCode &= iasc
                        iasc = ""
                    Case "<", "\"
                        sta = 1
                        Err.Raise(718, , "不支持嵌套")
                    Case ">"
                        sta = 1
                        rType = 1
                        rCode = sCode
                        sCode = ""
                        iasc = ""
                    Case Else
                        sta = 1
                        Err.Raise(777, , "语法错误")
                End Select
            Case 21
                '<$
                Select Case iasc
                    Case "C"
                        sta = 22
                        iasc = ""
                    Case "<", "\", " ", ">"
                        sta = 1
                        Err.Raise(777, , "语法错误")
                    Case Else
                        sta = 23
                        sCode &= iasc
                        iasc = ""
                End Select
            Case 22
                '<$C
                Select Case iasc
                    Case " "
                        sta = 24
                        iasc = ""
                    Case Else
                        sta = 1
                        Err.Raise(719, , "<$C后面必须有空格")
                End Select
            Case 23
                '<$aa
                Select Case iasc
                    Case "<", "\", " "
                        sta = 1
                        Err.Raise(777, , "语法错误")
                    Case ">"
                        sta = 1
                        rType = 3
                        rCode = sCode
                        iasc = ""
                        sCode = ""
                    Case Else
                        sCode &= iasc
                        iasc = ""
                End Select
            Case 24
                '<$C_
                Select Case iasc
                    Case ">"
                        sta = 1
                        rType = 2
                        rCode = sCode
                        iasc = ""
                        sCode = ""
                    Case "<", "\", " "
                        sta = 1
                        Err.Raise(777, , "语法错误")
                    Case Else
                        sCode &= iasc
                        iasc = ""
                End Select
            Case 25
                '<T$
                Select Case iasc
                    Case " "
                        sta = 34
                        iasc = ""
                    Case "C"
                        sta = 27
                        iasc = ""
                    Case "<", "\", " ", ">"
                        sta = 1
                        Err.Raise(777, , "语法错误")
                    Case Else
                        sta = 1
                        Err.Raise(720, , "<$")
                End Select
            Case 26
                '<T$_
                Select Case iasc
                    Case ">"
                        sta = 1
                        rType = 4
                        rCode = sCode
                        sCode = ""
                    Case 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
                        sta = 26
                        sCode &= iasc
                    Case Else
                        sta = 1
                        Err.Raise(777, , "语法错误")
                End Select
                iasc = ""
            Case 27
                '<T$C
                Select Case iasc
                    Case " "
                        sta = 35
                    Case Else
                        sta = 1
                        Err.Raise(777, , "语法错误")
                End Select
                iasc = ""
            Case 28
                '<T$C_
                Select Case iasc
                    Case 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
                        sta = 28
                        sCode &= iasc
                    Case ">"
                        rType = 5
                        rCode = sCode
                        sCode = ""
                        sta = 1
                    Case Else
                        sta = 1
                        Err.Raise(777, , "语法错误")
                End Select
                iasc = ""
            Case 29
                Select Case iasc
                    Case 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
                        sta = 30
                        sCode &= iasc
                    Case Else
                        sta = 1
                        Err.Raise(777, , "语法错误")
                End Select
                iasc = ""
            Case 30
                Select Case iasc
                    Case 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
                        sta = 30
                        sCode &= iasc
                    Case " "
                        sta = 36
                        sCode &= iasc
                    Case ">"
                        rType = 6
                        rCode = sCode
                        sCode = ""
                        sta = 1
                    Case Else
                        sta = 1
                        Err.Raise(777, , "语法错误")
                End Select
                iasc = ""
            Case 31
                Select Case iasc
                    Case "<", "\", " "
                        sta = 1
                        Err.Raise(777, , "语法错误")
                    Case ">"
                        rType = 7
                        rCode = sCode
                        sta = 1
                        sCode = ""
                    Case Else
                        sCode &= iasc
                        sta = 31
                End Select
                iasc = ""
            Case 32
                '<T
                Select Case iasc
                    Case 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
                        sta = 20
                        sCode &= iasc
                    Case "$"
                        sta = 25
                    Case "<", "\"
                        sta = 1
                        Err.Raise(718, , "不支持嵌套")
                    Case Else
                        sta = 1
                        Err.Raise(719, , "语法错误")
                End Select
                iasc = ""
            Case 33
            Case 34
                Select Case iasc
                    Case 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
                        sta = 26
                        sCode &= iasc
                    Case Else
                        sta = 1
                        Err.Raise(777, , "语法错误")
                End Select
                iasc = ""
            Case 35
                Select Case iasc
                    Case 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
                        sta = 28
                        sCode &= iasc
                    Case Else
                        sta = 1
                        Err.Raise(777, , "语法错误")
                End Select
                iasc = ""
            Case 36
                Select Case iasc
                    Case "<", "\", " ", ">"
                        sta = 1
                        Err.Raise(777, , "语法错误")
                    Case Else
                        sCode &= iasc
                        sta = 31
                End Select
                iasc = ""
            Case 37
                If iasc <> ">" Then
                    Err.Raise(777, , "只存在<N>")
                Else
                    sta = 1
                    txtShow.SelectedRtf = res7.Text
                End If
                iasc = ""
            Case 38
                If iasc <> ">" Then
                    Err.Raise(777, , "只存在<P>")
                Else
                    sta = 1
                    txtShow.SelectedRtf = res8.Text
                End If
                iasc = ""
        End Select

        txtShow.AppendText(iasc)
        Dim tString As String = ""
        If rCode <> "" Then
            '添加图片
            Select Case rType
                Case 1
                    '图片
                    tString = isr.tga2rtf(rCode, conn, RootDir & "\ui\Image\icon\")
                    If tString = "" Then
                        tString = Module1.frmMain.res1.Text
                    Else
                        tString = Split(tString, "wmetafile8\picw")(0) & "wmetafile8\picw1001" & "\pich1000\picwgoal" & Split(tString, "\picwgoal")(1)
                    End If
                Case 2
                    '文字选项(关闭)
                    tString = res2.Text
                Case 3
                    '文字选项
                    tString = res3.Text
                Case 4
                    '图片选项
                    tString = isr.tga2rtf(rCode, conn, RootDir & "\ui\Image\icon\")
                    If tString = "" Then
                        tString = Module1.frmMain.res4.Text
                    Else
                        tString = Split(tString, "wmetafile8\picw")(0) & "wmetafile8\picw1004" & "\pich1000\picwgoal" & Split(tString, "\picwgoal")(1)
                    End If
                Case 5
                    '图片选项(关闭)
                    tString = isr.tga2rtf(rCode, conn, RootDir & "\ui\Image\icon\")
                    If tString = "" Then
                        tString = Module1.frmMain.res5.Text
                    Else
                        tString = Split(tString, "wmetafile8\picw")(0) & "wmetafile8\picw1005" & "\pich1000\picwgoal" & Split(tString, "\picwgoal")(1)
                    End If
                Case 6, 7
                    '商店选项、商店选项(文字)
                    tString = res6.Text
            End Select

            '保存数据至hash表
            hiddenData.Add(hiddenData.Count + 1, rCode)

            '追加图片
            tString = Split(tString, "\pich1000")(0) & "\pich" & (1000 + hiddenData.Count) & Split(tString, "\pich1000")(1)
            txtShow.SelectedRtf = tString

            '还有一部分需要追加文字
            bAutoChangeFlag = False
            If rType = 2 Or rType = 3 Or rType = 7 Then
                txtShow.SelectionBackColor = Color.Yellow
                Dim ts As String
                If rType = 7 Then ts = Split(rCode, " ")(1) Else ts = rCode
                txtShow.AppendText(ts)
                txtShow.SelectionBackColor = Color.FromName("window")
            End If
            If rType = 6 Then
                txtShow.SelectionBackColor = Color.Yellow
                txtShow.AppendText(Module1.getShopName(rCode)) '"文字未定义，待读数据库")
                txtShow.SelectionBackColor = Color.FromName("window")
            End If
            bAutoChangeFlag = True
            'txtShow.AppendText("[" & rType & "：" & rCode & "]")
        End If
        rCode = ""
    End Sub

    Private Sub DoRevChange()
        Dim sta As Integer = 1
        Dim curColor As Color = Color.Black
        Dim newColor As Color
        Dim ret As String = ""
        Dim LFcount As Integer = 0

        With txtShow
            .SelectionStart = 0
            While True
                Dim tbr As Integer


                tbr = .SelectionStart
                '.SelectionStart += 1
                newColor = .SelectionColor                      '新颜色
                .SelectionStart = tbr

                .SelectionLength = 1

                If .SelectedText <> vbLf Then LFcount = 0 '清回车计数器

                Select Case sta
                    Case 1
                        If .SelectionType = RichTextBoxSelectionTypes.Object Then
                            '遇到图
                            sta = 3
                            ret &= makeImgCode(.SelectedRtf)
                        ElseIf .SelectedText = vbLf Then
                            '遇到回车
                            LFcount += 1
                            If LFcount = 1 Then
                                ret &= "\n"
                            ElseIf LFcount = 2 Then
                                ret &= "<H28>"
                            Else
                                'Append nothing
                            End If
                        ElseIf newColor <> Color.Black Then
                            '遇到颜色
                            sta = 2
                            Select Case newColor
                                Case Color.Blue
                                    ret &= "<F171 " & .SelectedText
                                Case Color.Red
                                    ret &= "<F172 " & .SelectedText
                                Case Color.Green
                                    ret &= "<F173 " & .SelectedText
                                Case Color.Gray
                                    ret &= "<F174 " & .SelectedText
                            End Select
                        Else
                            '遇到本色
                            ret &= .SelectedText
                        End If
                    Case 2  '-彩色
                        If .SelectionType = RichTextBoxSelectionTypes.Object Then
                            '遇到图
                            sta = 3
                            ret &= ">" & makeImgCode(.SelectedRtf)
                        ElseIf .SelectedText = vbLf Then
                            '遇到回车
                            LFcount += 1
                            If LFcount = 1 Then
                                Select Case newColor
                                    Case Color.Blue
                                        ret &= ">\n"
                                    Case Color.Red
                                        ret &= ">\n"
                                    Case Color.Green
                                        ret &= ">\n"
                                    Case Color.Gray
                                        ret &= ">\n"
                                    Case Color.Black
                                        ret &= ">\n"

                                End Select
                                sta = 1
                            ElseIf LFcount = 2 Then
                                ret &= "<H28>"
                            Else
                                'Append nothing
                            End If
                        ElseIf newColor = Color.Black Then
                            '遇到黑色
                            sta = 1
                            ret &= ">" & .SelectedText

                        ElseIf newColor = curColor Then
                            '遇到本颜色
                            ret &= .SelectedText
                        ElseIf newColor <> curColor Then
                            '其它颜色
                            Select Case newColor
                                Case Color.Blue
                                    ret &= "><F171 " & .SelectedText
                                Case Color.Red
                                    ret &= "><F172 " & .SelectedText
                                Case Color.Green
                                    ret &= "><F173 " & .SelectedText
                                Case Color.Gray
                                    ret &= "><F174 " & .SelectedText
                            End Select
                        End If
                    Case 3
                        If .SelectionType = RichTextBoxSelectionTypes.Object Then
                            '遇到图
                            sta = 3
                            ret &= makeImgCode(.SelectedRtf)
                        ElseIf .SelectedText = vbLf Then
                            '遇到回车
                            LFcount += 1
                            If LFcount = 1 Then
                                ret &= "\n"
                            ElseIf LFcount = 2 Then
                                ret &= "<H28>"
                            Else
                                'Append nothing
                            End If
                        ElseIf .SelectionBackColor = Color.Yellow Then
                            'Append nothing
                        ElseIf newColor <> Color.Black Then
                            '遇到颜色
                            sta = 2
                            Select Case newColor
                                Case Color.Blue
                                    ret &= "<F171 " & .SelectedText
                                Case Color.Red
                                    ret &= "<F172 " & .SelectedText
                                Case Color.Green
                                    ret &= "<F173 " & .SelectedText
                                Case Color.Gray
                                    ret &= "<F174 " & .SelectedText
                            End Select
                        Else
                            '遇到本色
                            ret &= .SelectedText
                        End If
                End Select
                curColor = newColor


                tbr = .SelectionStart
                .SelectionStart += 1
                If tbr = .SelectionStart Then
                    Exit While
                End If
            End While

            If curColor <> Color.Black Then ret += ">"
            ret = Replace(ret, "<F171 >", "")
            ret = Replace(ret, "<F172 >", "")
            ret = Replace(ret, "<F173 >", "")
            ret = Replace(ret, "<F174 >", "")
            txtSource.Text = ret
        End With
    End Sub

    Private Function makeImgCode(ByVal imgData As String)
        Dim ts() As String
        Dim tType As Integer
        Dim tKey As Integer
        Dim tString As String = ""

        ts = Split(txtShow.SelectedRtf, "\wmetafile8\picw")
        tType = Split(ts(1), "\")(0) - 1000
        tKey = Split(Split(ts(1), "\")(1), "pich")(1) - 1000

        Select Case tType
            Case 1
                tString &= "<T"
            Case 2
                tString &= "<$C "
            Case 3
                tString &= "<$"
            Case 4
                tString &= "<T$ "
            Case 5
                tString &= "<T$C "
            Case 6
                tString &= "<M"
            Case 7
                tString &= "<N"
            Case 8
                tString &= "<C"
        End Select
        If tType <> 7 Or tType <> 8 Then
            tString &= hiddenData(tKey)
        End If
        tString &= ">"
        Return tString
    End Function

    Private Sub btnColorCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnColorCancel.Click
        txtShow.SelectionColor = Color.Black
    End Sub

    Private Sub btnColorBlue_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnColorBlue.Click
        If CanColorChange() Then txtShow.SelectionColor = Color.Blue
    End Sub

    Private Sub btnColorRed_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnColorRed.Click
        If CanColorChange() Then txtShow.SelectionColor = Color.Red
    End Sub

    Private Sub btnColorGreen_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnColorGreen.Click
        If CanColorChange() Then txtShow.SelectionColor = Color.Green
    End Sub

    Private Sub btnColorGray_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnColorGray.Click
        If CanColorChange() Then txtShow.SelectionColor = Color.Gray
    End Sub

    Private Function CanColorChange() As Boolean
        If txtShow.SelectionType <> 1 And txtShow.SelectionType <> 5 Then
            MsgBox("请选中文字", , "提示")
            Return False
        End If
        If txtShow.SelectionBackColor <> Color.FromName("Window") And txtShow.SelectionBackColor <> Color.White Then
            MsgBox("请不要对选项内容进行颜色操作", , "提示")
            Return False
        End If
        Return True
    End Function

    Private Sub txtShow_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles txtShow.KeyDown
        isCtrlDown = e.Control
    End Sub
    Private Sub txtShow_KeyUp(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles txtShow.KeyUp
        isCtrlDown = e.Control
    End Sub

    Private Sub txtShow_KeyPress(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyPressEventArgs) Handles txtShow.KeyPress
        'Exit Sub
        With txtShow
            Dim tstart As Integer
            Dim tlen As Integer
            If Asc(e.KeyChar) = 13 Then
                '段落间距，控制它不超过2个回车
                tstart = .SelectionStart
                tlen = .SelectionLength
                .SelectionStart -= 1
                .SelectionLength = 1
                While .SelectedText = vbLf And .SelectionStart > 0
                    .SelectionStart -= 1
                End While
                If .SelectionStart = 0 Then
                    .SelectionStart = 1
                Else
                    .SelectionStart += 2
                End If

                If .SelectedText = vbLf Then
                    .SelectionStart += 1
                    While .SelectedText = vbLf
                        .SelectedText = ""
                        tstart -= 1
                    End While
                End If
                .SelectionStart = tstart
                .SelectionLength = tlen

            ElseIf Asc(e.KeyChar) = 34 Then

            ElseIf e.KeyChar = "c" And isCtrlDown Then
                MsgBox("c")
            ElseIf e.KeyChar = "v" And isCtrlDown Then
                MsgBox("p")
            End If
            'MsgBox(Asc(e.KeyChar))
        End With
    End Sub



    Private Sub txtShow_MouseClick(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles txtShow.MouseClick
        'Me.Text = txtShow.SelectionColor.ToString & txtShow.SelectionBackColor.ToString
    End Sub

    Private Sub txtShow_MouseDoubleClick(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles txtShow.MouseDoubleClick
        If txtShow.SelectionType = RichTextBoxSelectionTypes.Object Then
            Call popSet_Click(sender, e)
        End If
    End Sub

    Private Sub btnOK_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnOK.Click
        If txtShow.TextLength <= 512 Then
            If pannel.SelectedTabIndex = 1 Then
                encode()
            End If
            If (InStr(txtSource.Text, vbLf) > 0) Then
                MsgBox("源代码中包含回车!已经被过滤...")
                txtSource.Text = Replace(txtSource.Text, vbLf, "")
            End If
            Module1.exitcode = 1
            Me.Hide()
        End If
    End Sub

    Private Sub btnCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCancel.Click

        Module1.exitcode = 0
        Me.Hide()
    End Sub


    Private Sub txtSource_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtSource.TextChanged
        If isHaveError = True Then
            With txtSource
                Dim pos1 = .SelectionStart
                Dim pos2 = .SelectionLength
                .SelectAll()
                .SelectionBackColor = Color.White
                .SelectionStart = pos1
                .SelectionLength = pos2
            End With
            isHaveError = False
        End If
    End Sub


    Private Sub popMenu_Opening(ByVal sender As System.Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles popMenu.Opening
        If txtShow.SelectionType = RichTextBoxSelectionTypes.Object Then
            'popInsert.Enabled = False
            popInsertImg.Enabled = False
            popInsertUbody.Enabled = False
            popInsertUname.Enabled = False
            popInsertXImg.Enabled = False
            popInsertXLable.Enabled = False
            popInsertXShop.Enabled = False
            popSet.Enabled = True
        ElseIf txtShow.SelectionLength = 0 Then
            popInsertImg.Enabled = True
            popInsertUbody.Enabled = True
            popInsertUname.Enabled = True
            popInsertXImg.Enabled = True
            popInsertXLable.Enabled = True
            popInsertXShop.Enabled = True
            popSet.Enabled = False
        Else
            popInsertImg.Enabled = False
            popInsertUbody.Enabled = False
            popInsertUname.Enabled = False
            popInsertXImg.Enabled = False
            popInsertXLable.Enabled = False
            popInsertXShop.Enabled = False
            popSet.Enabled = False
        End If
    End Sub

    Private Sub popInsertImg_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles popInsertImg.Click
        Module1.frmImg.txtID.Text = ""
        Module1.frmImg.ShowDialog()
    End Sub

    Private Sub popSet_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles popSet.Click
        Dim ts() As String
        Dim tType As Integer
        Dim tKey As Integer

        ts = Split(txtShow.SelectedRtf, "\wmetafile8\picw")
        tType = Split(ts(1), "\")(0) - 1000
        tKey = Split(Split(ts(1), "\")(1), "pich")(1)
        tKey -= 1000

        Select Case tType
            Case 1
                '图片
                Module1.frmImg.txtID.Text = hiddenData(tKey)
                Module1.frmImg.ShowDialog()
            Case 2
                '文字选项(关)
                Module1.frmSelText.cbClose.Checked = True
                Module1.frmSelText.txtLabel.Text = hiddenData(tKey)
                Module1.frmSelText.ShowDialog()
            Case 3
                '文字选项
                Module1.frmSelText.cbClose.Checked = False
                Module1.frmSelText.txtLabel.Text = hiddenData(tKey)
                Module1.frmSelText.ShowDialog()
            Case 4
                '图片选项
                Module1.frmSelImg.cbClose.Checked = False
                Module1.frmSelImg.txtID.Text = hiddenData(tKey)
                Module1.frmSelImg.ShowDialog()
            Case 5
                '图片选项(关)
                Module1.frmSelImg.cbClose.Checked = True
                Module1.frmSelImg.txtID.Text = hiddenData(tKey)
                Module1.frmSelImg.ShowDialog()
            Case 6
                '商店选项
                Dim tmps As String
                tmps = hiddenData(tKey)
                If InStr(tmps, " ") > 0 Then
                    '有文字
                    Module1.frmSelShop.txtLabel.Text = Split(tmps, " ")(1)
                    Module1.frmSelShop.txtID.Text = Split(tmps, " ")(0)
                Else
                    '无文字
                    Module1.frmSelShop.txtLabel.Text = ""
                    Module1.frmSelShop.txtID.Text = tmps
                End If
                Module1.frmSelShop.ShowDialog()
        End Select
    End Sub

    Private Sub txtShow_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtShow.TextChanged
        'Exit Sub
        Dim tstart As Integer
        Dim tlen As Integer

        If bAutoChangeFlag = False Then Exit Sub

        With txtShow
            '观察前一个
            tstart = .SelectionStart
            tlen = .SelectionLength
            If .SelectionStart > 0 Then
                .SelectionStart -= 1
                .SelectionLength = 1
                If .SelectionType = RichTextBoxSelectionTypes.Object Then
                    .SelectionStart = tstart
                    .SelectionLength = tlen
                    Exit Sub
                End If

                Dim s As String = ""
                s = .SelectedText
                s = charReplace(s)
                
                '顺便禁止选项编辑先
                'If .SelectionBackColor = Color.Yellow Then
                's = ""
                'End If

                '处理双引号配对
                If s = Chr(34) Then
                    Dim tstart1 As Integer, tlen1 As Integer
                    tstart1 = .SelectionStart
                    tlen1 = .SelectionLength
                    Dim i As Integer
                    Dim bleft As Boolean = True
                    Dim tmps As String = ""
                    .SelectionStart = 0
                    .SelectionLength = tstart
                    tmps = .SelectedText

                    For i = 1 To Len(tmps)
                        If Asc(Mid(tmps, i, 1)) = -24144 Or Asc(Mid(tmps, i, 1)) = -24143 Then
                            bleft = Not bleft
                        End If
                    Next
                    s = IIf(bleft, Chr(-24144), Chr(-24143))

                    .SelectionStart = tstart1
                    .SelectionLength = tlen1
                End If

                '处理单引号配对
                If s = Chr(39) Then
                    Dim tstart1 As Integer, tlen1 As Integer
                    tstart1 = .SelectionStart
                    tlen1 = .SelectionLength
                    Dim i As Integer
                    Dim bleft As Boolean = True
                    Dim tmps As String = ""
                    .SelectionStart = 0
                    .SelectionLength = tstart
                    tmps = .SelectedText

                    For i = 1 To Len(tmps)
                        If Asc(Mid(tmps, i, 1)) = -24146 Or Asc(Mid(tmps, i, 1)) = -24145 Then
                            bleft = Not bleft
                        End If
                    Next
                    s = IIf(bleft, Chr(-24146), Chr(-24145))

                    .SelectionStart = tstart1
                    .SelectionLength = tlen1
                End If

                If s <> .SelectedText Then
                    If s = "" Then
                        .SelectedText = ""
                    Else
                        .SelectedText = CStr(s)
                    End If
                End If

                If Len(s) = 2 Then tstart += 1

                .SelectionStart = tstart
                .SelectionLength = tlen
            End If

            '观察再前一个，用于自动完成“句号-〉省略号”，以及检查%d%s格式。
            tstart = .SelectionStart
            tlen = .SelectionLength

            tstart = .SelectionStart
            tlen = .SelectionLength
            If .SelectionStart > 1 Then
                .SelectionStart -= 2
                .SelectionLength = 2
                If .SelectionType <> 5 Then
                    .SelectionStart = tstart
                    .SelectionLength = tlen
                    Exit Sub
                End If

                Dim s As String = ""
                s = .SelectedText
                If s = "。。" Or s = "。." Then
                    s = "……"
                    .SelectedText = s
                End If
                If Mid(s, 1, 1) = "%" And Len(s) = 2 Then
                    If Mid(s, 2, 1) <> "s" And Mid(s, 2, 1) <> "d" Then
                        s = "%"
                        .SelectedText = s
                    End If
                End If
            End If
            .SelectionStart = tstart
            .SelectionLength = tlen

            '***********************************************
            '       研究是否编辑了选项,同步更新hash表
            '***********************************************
            If .SelectionBackColor = Color.Yellow Then
                tstart = .SelectionStart
                tlen = .SelectionLength

                '向前找到图片
                While True
                    If .SelectionStart = 0 Then Exit Sub
                    .SelectionStart -= 1
                    .SelectionLength = 1
                    If .SelectionType = RichTextBoxSelectionTypes.Object Then Exit While
                End While

                '分析图片
                Dim ts() As String
                Dim tType As Integer
                Dim tKey As Integer
                ts = Split(.SelectedRtf, "\wmetafile8\picw")
                tType = Split(ts(1), "\")(0) - 1000
                tKey = Split(Split(ts(1), "\")(1), "pich")(1)
                tKey -= 1000

                '向后找到所有选项文字
                .SelectionStart += 1
                Dim tbr As Integer
                Dim tstart1 As Integer, tlen1 As Integer
                tstart1 = .SelectionStart
                .SelectionStart += 1
                While .SelectionBackColor = Color.Yellow
                    tbr = .SelectionStart
                    .SelectionStart += 1
                    If .SelectionStart = tbr Then
                        tbr = -1
                        Exit While
                    End If
                End While
                tlen1 = .SelectionStart - tstart1
                If tbr = -1 Then tlen1 += 1

                .SelectionStart = tstart1
                tlen1 -= 1
                If tlen1 < 0 Then tlen1 = 0
                .SelectionLength = tlen1 + 1

                '修改hash表数据
                If tType = 6 Then
                    hiddenData(tKey) = Split(hiddenData(tKey), " ")(0) & " " & Replace(.SelectedText, vbLf, "")
                Else
                    hiddenData(tKey) = Replace(.SelectedText, vbLf, "")
                End If
                .SelectionStart = tstart
                .SelectionLength = tlen
            End If
        End With
        'Me.Text = "Visual Editor -- 共输入字数：" & txtShow.Text.Length.ToString()
        Dim iCanStillInput As Integer
        iCanStillInput = 512 - txtShow.Text.Length
        Me.Text = "Visual Editor -- 还可以输入字数：" & iCanStillInput.ToString()
        btnOK.Enabled = Me.txtShow.TextLength <= 512
    End Sub



    Private Sub popInsertXLable_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles popInsertXLable.Click
        Module1.frmSelText.txtLabel.Text = ""
        Module1.frmSelText.ShowDialog()
    End Sub

    Private Sub popInsertXImg_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles popInsertXImg.Click
        Module1.frmSelImg.txtID.Text = ""
        Module1.frmSelImg.ShowDialog()
    End Sub

    Private Sub popInsertXShop_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles popInsertXShop.Click
        Module1.frmSelShop.txtID.Text = ""
        Module1.frmSelShop.txtLabel.Text = ""
        Module1.frmSelShop.ShowDialog()
    End Sub

    Private Sub popInsertUname_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles popInsertUname.Click
        txtShow.SelectedRtf = res7.Text
    End Sub

    Private Sub popInsertUbody_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles popInsertUbody.Click
        txtShow.SelectedRtf = res8.Text
    End Sub

    Private Sub ToolStripMenuItem1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripMenuItem1.Click
        If txtShow.SelectedText <> "" Then
            System.Windows.Forms.Clipboard.SetText(txtShow.SelectedText)
        End If
    End Sub

    Private Sub ToolStripMenuItem_Click_1(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripMenuItem.Click
        txtShow.SelectedText = System.Windows.Forms.Clipboard.GetText()
    End Sub
End Class
