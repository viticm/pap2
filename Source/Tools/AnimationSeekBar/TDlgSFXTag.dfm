object SFXTagDialog: TSFXTagDialog
  Left = -700
  Top = 151
  Caption = #29305#25928#26631#31614
  ClientHeight = 430
  ClientWidth = 444
  Color = clBtnFace
  Font.Charset = GB2312_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #23435#20307
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 12
  object Lable_SFXTagList: TLabel
    Left = 8
    Top = 16
    Width = 72
    Height = 12
    Caption = #29305#25928#26631#31614#21015#34920
  end
  object Combo_SFXTagList: TComboBox
    Left = 104
    Top = 8
    Width = 185
    Height = 20
    Style = csDropDownList
    ItemHeight = 12
    TabOrder = 0
    OnSelect = Combo_SFXTagListSelect
  end
  object Group_SFXTagContent: TGroupBox
    Left = 8
    Top = 40
    Width = 425
    Height = 248
    Caption = #26631#31614#20869#23481
    TabOrder = 1
    object Label_SFXPlayMode: TLabel
      Left = 12
      Top = 84
      Width = 48
      Height = 12
      Caption = #25773#25918#26041#24335
    end
    object Label_PlaySpeed: TLabel
      Left = 12
      Top = 113
      Width = 48
      Height = 12
      Caption = #25773#25918#36895#24230
    end
    object Label_TagName: TLabel
      Left = 16
      Top = 24
      Width = 48
      Height = 12
      Caption = #26631#31614#21517#23383
    end
    object BindEdit: TButton
      Left = 12
      Top = 49
      Width = 389
      Height = 25
      Caption = #29305#25928#25991#20214#36873#23450
      TabOrder = 0
      OnClick = BindEditClick
    end
    object Combo_PlayMode: TComboBox
      Left = 91
      Top = 81
      Width = 81
      Height = 20
      Style = csDropDownList
      ItemHeight = 12
      TabOrder = 1
      Items.Strings = (
        #24490#29615
        #21333#27425#20445#25345#32467#26463#29366#24577
        #21333#27425)
    end
    object Edit_PlaySpeed: TEdit
      Left = 91
      Top = 113
      Width = 121
      Height = 20
      TabOrder = 2
      Text = '1.0'
    end
    object Check_AdaptSpeedToMotion: TCheckBox
      Left = 12
      Top = 145
      Width = 177
      Height = 17
      Caption = #36319#38543#21160#20316#36895#24230#33258#21160#35843#25972
      TabOrder = 3
    end
    object Check_Interruptable: TCheckBox
      Left = 12
      Top = 169
      Width = 161
      Height = 17
      Caption = #21487#34987#19979#19968#29305#25928#35206#30422
      TabOrder = 4
    end
    object Check_MotionFinishKeep: TCheckBox
      Left = 12
      Top = 193
      Width = 169
      Height = 17
      Caption = #21160#20316#23436#25104#26102#25345#32493
      TabOrder = 5
    end
    object Check_NotifyRepresent: TCheckBox
      Left = 12
      Top = 217
      Width = 137
      Height = 17
      Caption = #36890#30693#34920#29616#36923#36753
      TabOrder = 6
    end
    object Edit_TagName: TEdit
      Left = 80
      Top = 16
      Width = 321
      Height = 20
      TabOrder = 7
      OnKeyDown = Edit_TagNameKeyDown
    end
  end
  object Button_Delete: TButton
    Left = 96
    Top = 399
    Width = 75
    Height = 25
    Caption = #21024#38500
    TabOrder = 2
    OnClick = Button_DeleteClick
  end
  object Button_Copy: TButton
    Left = 184
    Top = 399
    Width = 75
    Height = 25
    Caption = #22797#21046
    TabOrder = 3
    OnClick = Button_CopyClick
  end
  object Group_DataContent: TGroupBox
    Left = 8
    Top = 295
    Width = 425
    Height = 97
    Caption = #31243#24207#36816#34892#20449#24687
    TabOrder = 4
    object Label_BindIndex: TLabel
      Left = 8
      Top = 42
      Width = 72
      Height = 12
      Caption = 'Bind'#25991#20214#32034#24341
    end
    object Label_BindFileName: TLabel
      Left = 8
      Top = 19
      Width = 54
      Height = 12
      Caption = 'Bind'#25991#20214':'
    end
    object Label_BindIndexNum: TLabel
      Left = 92
      Top = 41
      Width = 108
      Height = 12
      Caption = 'Label_BindIndexNum'
    end
    object Label_SFXFileName: TLabel
      Left = 8
      Top = 64
      Width = 54
      Height = 12
      Caption = #29305#25928#25991#20214':'
    end
    object Edit_BindFileName: TEdit
      Left = 72
      Top = 16
      Width = 337
      Height = 20
      Enabled = False
      TabOrder = 0
      Text = 'Edit_BindFileName'
    end
    object Edit_SFXFileName: TEdit
      Left = 72
      Top = 60
      Width = 337
      Height = 20
      Enabled = False
      TabOrder = 1
      Text = 'Edit_SFXFileName'
    end
  end
  object Button_New: TButton
    Left = 8
    Top = 399
    Width = 75
    Height = 25
    Caption = #26032#24314
    TabOrder = 5
    OnClick = Button_NewClick
  end
  object Button_Paste: TButton
    Left = 272
    Top = 399
    Width = 75
    Height = 25
    Caption = #31896#36148
    TabOrder = 6
    OnClick = Button_PasteClick
  end
end
