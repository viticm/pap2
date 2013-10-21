object MotionTagDialog: TMotionTagDialog
  Left = -507
  Top = 175
  Caption = #21160#20316#31867#26631#31614
  ClientHeight = 220
  ClientWidth = 343
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Label_TagType: TLabel
    Left = 8
    Top = 11
    Width = 56
    Height = 13
    Caption = #26631#31614#31867#22411
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #26032#23435#20307
    Font.Style = []
    ParentFont = False
  end
  object Combo_TagType: TComboBox
    Left = 72
    Top = 8
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
    OnChange = Combo_TagTypeChange
    Items.Strings = (
      #20934#22791#21160#20316#32467#26463
      #25915#20987#21160#20316#32467#26463
      #29992#25143#23450#20041#31867#22411)
  end
  object Button_Delete: TButton
    Left = 10
    Top = 187
    Width = 75
    Height = 25
    Caption = #21024#38500
    TabOrder = 1
  end
  object Button_Copy: TButton
    Left = 98
    Top = 187
    Width = 75
    Height = 25
    Caption = #22797#21046
    TabOrder = 2
    OnClick = Button_CopyClick
  end
  object Button_Paste: TButton
    Left = 186
    Top = 187
    Width = 75
    Height = 25
    Caption = #31896#36148
    TabOrder = 3
    OnClick = Button_PasteClick
  end
  object PanelStrikingEffect: TPanel
    Left = 8
    Top = 59
    Width = 305
    Height = 119
    BevelOuter = bvNone
    TabOrder = 4
    Visible = False
    object StaticText2: TStaticText
      Left = 11
      Top = 12
      Width = 64
      Height = 17
      Caption = #20260#23475#30334#20998#27604
      TabOrder = 0
    end
    object EditDamagePercentage: TEdit
      Left = 81
      Top = 8
      Width = 121
      Height = 21
      TabOrder = 1
    end
  end
  object ComboExtraTagType: TComboBox
    Left = 72
    Top = 32
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 5
    OnChange = ComboExtraTagTypeChange
    Items.Strings = (
      #20260#23475#30334#20998#27604)
  end
  object StaticText1: TStaticText
    Left = 8
    Top = 34
    Width = 32
    Height = 17
    Caption = #31867#22411
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #26032#23435#20307
    Font.Style = []
    ParentFont = False
    TabOrder = 6
  end
end
