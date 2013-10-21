VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Begin VB.Form frmTest 
   Caption         =   "Form1"
   ClientHeight    =   7545
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   7875
   LinkTopic       =   "Form1"
   ScaleHeight     =   7545
   ScaleWidth      =   7875
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton btnExtractThumbnail 
      Caption         =   "Extract Thumbnail from file"
      Height          =   495
      Left            =   2280
      TabIndex        =   1
      Top             =   6600
      Width           =   2655
   End
   Begin MSComDlg.CommonDialog cdl1 
      Left            =   6720
      Top             =   6480
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.PictureBox pic1 
      Height          =   6015
      Left            =   600
      ScaleHeight     =   5955
      ScaleWidth      =   6795
      TabIndex        =   0
      Top             =   240
      Width           =   6855
   End
End
Attribute VB_Name = "frmTest"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim m_oThumb As THUMBEXTRACTLib.FileThumbExtract
Private Sub btnExtractThumbnail_Click()
On Error GoTo er
cdl1.ShowOpen
TEMPFILE = "C:\tempThumbnail.jpg"
If cdl1.FileName = "" Then Exit Sub
Call m_oThumb.SetPath(cdl1.FileName)
'Call m_oThumb.SetDir("C:\Documents and Settings\PHILIP1\My Documents\PhD - 1")
'Call m_oThumb.SetFile("presentations\ContrastPres.ppt")
Call m_oThumb.SetThumbnailSize(350, 350)
Call m_oThumb.ExtractThumbnail
Call m_oThumb.SaveToFile(TEMPFILE)
pic1.Picture = LoadPicture(TEMPFILE)
Exit Sub
er:
MsgBox Err.Description
End Sub

Private Sub Form_Load()
Set m_oThumb = New THUMBEXTRACTLib.FileThumbExtract
Call m_oThumb.SetDir("c:\basedir")
Call m_oThumb.SetFile("images\notes.scb")
Debug.Print m_oThumb.GetPath & " = " & m_oThumb.GetDir & " + " & m_oThumb.GetFile
End Sub
