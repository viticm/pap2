Public Class Doit

    Public Function show(ByVal scode As String, ByRef tconn As System.Data.SqlClient.SqlConnection, ByVal d As String) As String
        conn = tconn
        RootDir = d
        Dim sRet As String = ""
        Module1.exitcode = 0
        Module1.frmMain.txtSource.Text = scode
        Module1.frmMain.dencode()
        Module1.frmMain.ShowDialog()
        If Module1.exitcode = 1 Then
            sRet = Module1.frmMain.txtSource.Text
            While Right(sRet, 5) = "<H28>"
                sRet = Left(sRet, Len(sRet) - 5)
            End While
            While Right(sRet, 2) = "\n"
                sRet = Left(sRet, Len(sRet) - 2)
            End While
            sRet = Replace(sRet, "<G>", "")
            sRet = ReplaceAddG(sRet)
            Return sRet
        Else
            Return scode
        End If
    End Function

    Private Function ReplaceAddG(ByVal strCode As String) As String
        strCode = "<G>" + strCode
        strCode = Replace(strCode, "\n", "\n<G>")
        strCode = Replace(strCode, "\n<G><H28>", "\n<H28><G>")
        If strCode.EndsWith("<G>") Then
            strCode = Left(strCode, Len(strCode) - 3)
        End If
        Return strCode
    End Function
End Class
