object DlgSourceView: TDlgSourceView
  Left = -560
  Top = 78
  Caption = #36164#28304#27983#35272#22120
  ClientHeight = 613
  ClientWidth = 421
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 336
    Top = 16
    Width = 75
    Height = 25
    Caption = #21047#26032
    TabOrder = 0
    OnClick = Button1Click
  end
  object FileTree: TTreeView
    Left = 16
    Top = 16
    Width = 305
    Height = 585
    Indent = 19
    TabOrder = 1
    OnChange = FileTreeChange
  end
  object Button_OK: TButton
    Left = 336
    Top = 48
    Width = 75
    Height = 25
    Caption = #30830#23450
    TabOrder = 2
    OnClick = Button_OKClick
  end
end
