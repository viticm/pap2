Imports System.Windows.Forms
Imports System.Resources
Imports System.Drawing
Imports System.Data.SqlClient

Module Module1
    Public hiddenData As New Hashtable
    Public exitcode As Integer = 0
    Public conn As System.Data.SqlClient.SqlConnection
    Public RootDir As String
    Public bAutoChangeFlag As Boolean = True


    Public frmMain As New frmMain
    Public frmImg As New frmImg
    Public frmSelImg As New frmSelImg
    Public frmSelShop As New frmSelShop
    Public frmSelText As New frmSelText
    Public isr As IconSelector.tgaIO
    Public isDiag As New IconSelector.Implement
    Public bisInit As Boolean = False


    Public Function charReplace(ByVal s As String)
        '需要替换
        s = Replace(s, ",", "，")
        s = Replace(s, ".", "。")
        s = Replace(s, "<", "《")
        s = Replace(s, ">", "》")
        s = Replace(s, "!", "！")
        s = Replace(s, "?", "？")
        s = Replace(s, "-", "—")
        s = Replace(s, "^", "……")
        s = Replace(s, "_", "——")
        s = Replace(s, "@", "·")
        s = Replace(s, "\", "、")
        s = Replace(s, ";", "；")
        s = Replace(s, ":", "：")
        s = Replace(s, ")", "）")
        s = Replace(s, "(", "（")

        '需要消除
        s = Replace(s, "#", "")
        s = Replace(s, " ", "")
        s = Replace(s, "$", "")
        s = Replace(s, "&", "")
        s = Replace(s, "*", "")
        s = Replace(s, "|", "")
        s = Replace(s, "~", "")
        s = Replace(s, "`", "")
        s = Replace(s, "+", "")
        s = Replace(s, "=", "")
        s = Replace(s, "{", "")
        s = Replace(s, "}", "")
        s = Replace(s, "/", "")
        s = Replace(s, "[", "")
        s = Replace(s, "]", "")
        Return s
    End Function

    Public Function getShopName(ByVal ShopID As Integer) As String
        Dim tbl As DataTable = GetDataTable("select * from NPCShopList where ShopID=" & ShopID, conn)
        If tbl.Rows.Count = 0 Then
            Return "商店名称未定义"
        End If
        Dim row As DataRow = tbl.Rows(0)
        Return row("ShopName")
    End Function

    Public Function GetDataTable(ByVal sql As String, ByRef conn As SqlConnection) As DataTable
        Dim adp As SqlDataAdapter = New SqlDataAdapter(sql, conn)
        adp.MissingSchemaAction = MissingSchemaAction.AddWithKey
        Dim ds As DataSet = New DataSet()
        adp.Fill(ds)
        Dim tbl As DataTable = ds.Tables(0)
        Return tbl
    End Function
End Module
