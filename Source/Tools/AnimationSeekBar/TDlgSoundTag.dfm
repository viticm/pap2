object SoundTagDialog: TSoundTagDialog
  Left = -866
  Top = 10
  Caption = #22768#38899#26631#31614
  ClientHeight = 297
  ClientWidth = 474
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
  object TagContent: TGroupBox
    Left = 8
    Top = 0
    Width = 447
    Height = 57
    Caption = #26631#31614#20869#23481
    TabOrder = 0
    object Check_Loop: TCheckBox
      Left = 3
      Top = 23
      Width = 81
      Height = 17
      Caption = #24490#29615#25773#25918
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #26032#23435#20307
      Font.Style = []
      ParentFont = False
      TabOrder = 0
    end
    object Check_MotionFinishKeep: TCheckBox
      Left = 217
      Top = 24
      Width = 105
      Height = 17
      Caption = #21160#20316#23436#25104#26102#20445#25345
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = #26032#23435#20307
      Font.Style = []
      ParentFont = False
      TabOrder = 1
    end
    object Check_Interruptable: TCheckBox
      Left = 90
      Top = 24
      Width = 121
      Height = 17
      Caption = #34987#19979#19968#22768#38899#20013#26029
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = #26032#23435#20307
      Font.Style = []
      ParentFont = False
      TabOrder = 2
    end
  end
  object Button_Delete: TButton
    Left = 8
    Top = 250
    Width = 75
    Height = 25
    Caption = #21024#38500
    TabOrder = 1
    OnClick = Button_DeleteClick
  end
  object Button_Play: TButton
    Left = 89
    Top = 250
    Width = 75
    Height = 25
    Caption = #25773#25918
    TabOrder = 2
    OnClick = Button_PlayClick
  end
  object SoundCandidate: TGroupBox
    Left = 8
    Top = 72
    Width = 449
    Height = 172
    Caption = #22768#38899#25991#20214#20449#24687
    TabOrder = 3
    object Lable_HitRate: TLabel
      Left = 8
      Top = 50
      Width = 56
      Height = 13
      Caption = #25773#25918#20960#29575
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #26032#23435#20307
      Font.Style = []
      ParentFont = False
    end
    object Label_Volume: TLabel
      Left = 34
      Top = 75
      Width = 28
      Height = 13
      Caption = #38899#37327
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #26032#23435#20307
      Font.Style = []
      ParentFont = False
    end
    object Label_FilePath: TLabel
      Left = 8
      Top = 24
      Width = 56
      Height = 13
      Caption = #22768#38899#25991#20214
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #26032#23435#20307
      Font.Style = []
      ParentFont = False
    end
    object Label_VolumeCount: TLabel
      Left = 217
      Top = 80
      Width = 3
      Height = 13
    end
    object Label1: TLabel
      Left = 34
      Top = 108
      Width = 24
      Height = 13
      Caption = #26368#36817
    end
    object Label2: TLabel
      Left = 34
      Top = 135
      Width = 24
      Height = 13
      Caption = #26368#36828
    end
    object Button_Add: TButton
      Left = 264
      Top = 133
      Width = 58
      Height = 25
      Caption = #22686#21152
      TabOrder = 0
      OnClick = Button_AddClick
    end
    object Edit_HitRate: TEdit
      Left = 70
      Top = 44
      Width = 121
      Height = 21
      TabOrder = 1
      Text = '0'
    end
    object ComboBox_CandidateList: TComboBox
      Left = 70
      Top = 16
      Width = 235
      Height = 21
      ItemHeight = 13
      TabOrder = 2
      OnChange = ComboBox_CandidateListChange
    end
    object Button_FilePath: TButton
      Left = 311
      Top = 14
      Width = 66
      Height = 25
      Caption = #25171#24320#21015#34920
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #26032#23435#20307
      Font.Style = []
      ParentFont = False
      TabOrder = 3
      OnClick = Button_FilePathClick
    end
    object TrackBar_Volume: TTrackBar
      Left = 67
      Top = 71
      Width = 144
      Height = 26
      TabOrder = 4
      TickStyle = tsNone
      OnChange = TrackBar_VolumeChange
    end
    object Button_DeleteCandidate: TButton
      Left = 328
      Top = 133
      Width = 57
      Height = 25
      Caption = #21024#38500
      TabOrder = 5
      OnClick = Button_DeleteCandidateClick
    end
    object Edit_MinDis: TEdit
      Left = 70
      Top = 103
      Width = 121
      Height = 21
      TabOrder = 6
    end
    object Edit_MaxDis: TEdit
      Left = 70
      Top = 130
      Width = 121
      Height = 21
      TabOrder = 7
    end
  end
end
