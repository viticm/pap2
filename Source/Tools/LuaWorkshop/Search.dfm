object SearchDlg: TSearchDlg
  Left = 329
  Top = 171
  BorderStyle = bsDialog
  Caption = 'Dialog'
  ClientHeight = 95
  ClientWidth = 312
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 9
    Top = 11
    Width = 81
    Height = 13
    AutoSize = False
    Caption = 'Search Pattern'
  end
  object OKBtn: TButton
    Left = 135
    Top = 60
    Width = 75
    Height = 25
    Caption = 'Find'
    Default = True
    ModalResult = 1
    TabOrder = 1
    OnClick = OKBtnClick
  end
  object CancelBtn: TButton
    Left = 223
    Top = 60
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
    OnClick = CancelBtnClick
  end
  object Edit1: TEdit
    Left = 96
    Top = 8
    Width = 201
    Height = 21
    TabOrder = 0
  end
  object CheckBox1: TCheckBox
    Left = 34
    Top = 33
    Width = 103
    Height = 17
    Caption = 'Case sensitive'
    TabOrder = 3
  end
  object CheckBox2: TCheckBox
    Left = 160
    Top = 33
    Width = 121
    Height = 17
    Caption = 'Whole words only'
    TabOrder = 4
  end
end
