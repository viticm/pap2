object AnimationSeekBar: TAnimationSeekBar
  Left = 487
  Top = 382
  Width = 624
  Height = 60
  Caption = 'AnimationSeekBar'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = ActiveFormCreate
  DesignSize = (
    624
    60)
  PixelsPerInch = 96
  TextHeight = 13
  object Panel3: TPanel
    Left = 0
    Top = 1
    Width = 624
    Height = 57
    Anchors = [akLeft, akRight]
    BevelOuter = bvNone
    BiDiMode = bdRightToLeftNoAlign
    Ctl3D = True
    ParentBiDiMode = False
    ParentCtl3D = False
    TabOrder = 0
    DesignSize = (
      624
      57)
    object TimeLineBar1: TTimeLineBar
      Left = 0
      Top = 0
      Width = 645
      Height = 57
      Anchors = [akLeft, akTop, akRight, akBottom]
      OnClick = TimeLineBar1Click
    end
  end
  object PopupMenu_TagType: TPopupMenu
    OnPopup = PopupMenu_TagTypePopup
    Left = 32
    Top = 16
    object MenuItem_SFX: TMenuItem
      Caption = #29305#25928
      OnClick = OnPopMenuSFXClick
    end
    object MenuItem_Sound: TMenuItem
      Caption = #22768#38899
      OnClick = OnPopMenuSoundClick
    end
    object MenuItem_Motion: TMenuItem
      Caption = #21160#20316
      OnClick = OnPopMenuMotion
    end
  end
end
