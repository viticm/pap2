object ProcessListForm: TProcessListForm
  Left = 271
  Top = 118
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Process List'
  ClientHeight = 453
  ClientWidth = 432
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ValueListEditor1: TValueListEditor
    Left = 0
    Top = 0
    Width = 432
    Height = 401
    Align = alTop
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goColSizing, goRowSelect, goThumbTracking]
    TabOrder = 0
    ColWidths = (
      150
      276)
  end
  object BitBtn1: TBitBtn
    Left = 224
    Top = 416
    Width = 75
    Height = 25
    Caption = 'Attach'
    TabOrder = 1
    OnClick = BitBtn1Click
  end
  object BitBtn2: TBitBtn
    Left = 320
    Top = 416
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = BitBtn2Click
  end
end
