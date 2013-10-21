unit TDlgSoundTag;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, AnimationSeekBarImpl1, TDlgSourceTree, ComCtrls;
var
  m_nFrame : Integer;
  m_AnimationSeekBar : TAnimationSeekBar;
type
  TSoundTagDialog = class(TForm)
    TagContent: TGroupBox;
    Label_FilePath: TLabel;
    Button_FilePath: TButton;
    Check_Loop: TCheckBox;
    Check_MotionFinishKeep: TCheckBox;
    Check_Interruptable: TCheckBox;
    Button_Delete: TButton;
    Button_Play: TButton;
    TrackBar_Volume: TTrackBar;
    Label_Volume: TLabel;
    Lable_HitRate: TLabel;
    Edit_HitRate: TEdit;
    ComboBox_CandidateList: TComboBox;
    Button_Add: TButton;
    SoundCandidate: TGroupBox;
    Button_DeleteCandidate: TButton;
    Label_VolumeCount: TLabel;
    Label1: TLabel;
    Label2: TLabel;
    Edit_MinDis: TEdit;
    Edit_MaxDis: TEdit;
    procedure Button_DeleteCandidateClick(Sender: TObject);
    procedure TrackBar_VolumeChange(Sender: TObject);
    procedure Button_AddClick(Sender: TObject);
    procedure ComboBox_CandidateListChange(Sender: TObject);
    procedure Button_DeleteClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure Button_PasteClick(Sender: TObject);
    procedure Button_CopyClick(Sender: TObject);
    procedure Button_FilePathClick(Sender: TObject);
    procedure Button_PlayClick(Sender: TObject);
  private
    { Private declarations }
    m_nLastCandidate : Integer;
    m_bPlaySound : boolean;
    procedure UpdateaUI(nLoop : Integer;
      nKeep : Integer;
      nInterruptable : Integer);
  public
    { Public declarations }
    //procedure OnCandidateChange(nCandidate : Integer);
    procedure OnInitDialog(nFrame : Integer);
  end;

var
  SoundTagDialog: TSoundTagDialog;

implementation

{$R *.dfm}

//procedure TSoundTagDialog.OnCandidateChange(nCandidate : Integer);
//var
 // strSoundFileName : WideString;
//  fHitRate : Single;
//  fVolume : Single;
//begin
//
//end;

procedure TSoundTagDialog.UpdateaUI(nLoop : Integer;
  nKeep : Integer;
  nInterruptable : Integer);
begin
  Check_Interruptable.Checked := boolean(nInterruptable);
  Check_MotionFinishKeep.Checked := boolean(nKeep);
  Check_Loop.Checked := boolean(nLoop);
end;

procedure TSoundTagDialog.Button_DeleteCandidateClick(Sender: TObject);
var
  nIndex, nIndex2Del : Integer;
begin
  nIndex2Del := ComboBox_CandidateList.ItemIndex;
  nIndex := nIndex2Del;
  if ComboBox_CandidateList.Items.Count > 1 then
  begin
    if nIndex = ComboBox_CandidateList.Items.Count - 1 then
      nIndex := nIndex - 1;
    ComboBox_CandidateList.Items.Delete(nIndex2Del);
    ComboBox_CandidateList.ItemIndex := nIndex;
  end;
  if m_AnimationSeekBar.FEvents <> nil then
  begin
    m_AnimationSeekBar.FEvents.OnDeleteSoundTagCandidate(m_nFrame,
      nIndex2Del);
    m_nLastCandidate := -1;
    ComboBox_CandidateListChange(nil);
  end;
end;

procedure TSoundTagDialog.Button_DeleteClick(Sender: TObject);
begin
  if m_AnimationSeekBar.FEvents <> nil then
    m_AnimationSeekBar.FEvents.OnSoundTagDelete(m_nFrame);
  m_nFrame := -1;
  Close();
end;

procedure TSoundTagDialog.OnInitDialog(nFrame : Integer);
var
  fHitRate, fVolume, fMinDis, fMaxDis : Single;
  strSoundFileName : WideString;
  nLoop, nKeep, nInterruptable, nNumCandidate, nIndex : Integer;
begin
  nLoop := 0;
  nKeep := 0;
  nInterruptable := 0;
  m_nFrame := nFrame;
  m_bPlaySound := false;
  m_nLastCandidate := -1;
  m_AnimationSeekBar := TAnimationSeekBar(Owner);
  TrackBar_Volume.Min := 0;
  Trackbar_Volume.Max := 100;
  if m_AnimationSeekBar.FEvents <> nil then
  begin
    m_AnimationSeekBar.FEvents.OnSoundTagLoad(m_nFrame,
      nLoop,
      nKeep,
      nInterruptable,
      nNumCandidate);
    for nIndex := 0 to nNumCandidate - 1 do
    begin
      m_AnimationSeekBar.FEvents.OnLoadSoundTagCandidate(m_nFrame,
      nIndex,
      strSoundFileName,
      fHitRate,
      fVolume,
      fMinDis,
      fMaxDis);
      ComboBox_CandidateList.Items.Add(string(strSoundFileName));
    end;
    if (ComboBox_CandidateList.Items.Count <> 0) then
    begin
      ComboBox_CandidateList.ItemIndex := 0;
      ComboBox_CandidateListChange(nil);
    end;
    UpdateaUI(nLoop,
      nKeep,
      nInterruptable);
    m_bPlaySound := true;
  end
end;

procedure TSoundTagDialog.TrackBar_VolumeChange(Sender: TObject);
var
  strSoundFilePath : WideString;
  strSoundCandidate : string;
  fVolume : Single;
begin
  if (m_AnimationSeekBar.FEvents <> nil) and m_bPlaySound then
  begin
     if ComboBox_CandidateList.ItemIndex >= 0 then
    begin
      strSoundCandidate := ComboBox_CandidateList.Items[ComboBox_CandidateList.ItemIndex];
      if (Length(strSoundCandidate) <> 0) and
        (m_AnimationSeekBar.FEvents <> nil) then
      begin
        fVolume := TrackBar_Volume.Position;
        strSoundFilePath := WideString(ComboBox_CandidateList.Items[ComboBox_CandidateList.ItemIndex]);
        m_AnimationSeekBar.FEvents.OnPlaySoundFile(strSoundFilePath, fVolume);
        Label_VolumeCount.Caption := floattostr(fVolume);
      end;
    end;
  end;
end;

procedure TSoundTagDialog.FormClose(Sender: TObject;
  var Action: TCloseAction);
var
  nLoop, nKeep, nInterruptable : Integer;
  strSoundFileName : WideString;
  fHitRate, fVolume, fMinDis, fMaxDis : Single;
begin
  nLoop := Integer(Check_Loop.Checked);
  nKeep := Integer(Check_MotionFinishKeep.Checked);
  nInterruptable := Integer(Check_Interruptable.Checked);
  if (m_AnimationSeekBar.FEvents <> nil) and (m_nFrame <> -1) then
  begin
    m_AnimationSeekBar.FEvents.OnSoundTagSave(m_nFrame,
      nLoop,
      nKeep,
      nInterruptable,
      ComboBox_CandidateList.Items.Count);
      strSoundFileName := WideString(ComboBox_CandidateList.Items[ComboBox_CandidateList.ItemIndex]);
    fHitRate := StrToFloat(Edit_HitRate.Text);
    fVolume := TrackBar_Volume.Position;
    fMinDis := StrToFloat(Edit_MinDis.Text);
    fMaxDis := StrToFloat(Edit_MaxDis.Text);
    m_AnimationSeekBar.FEvents.OnSaveSoundTagCandidate(m_nFrame,
      ComboBox_CandidateList.ItemIndex,
      strSoundFileName,
      fHitRate,
      fVolume,
      fMinDis,
      fMaxDis);
  end;
end;

procedure TSoundTagDialog.Button_PasteClick(Sender: TObject);
begin
  if m_AnimationSeekBar.FEvents <> nil then
    m_AnimationSeekBar.FEvents.OnSoundTagCopy(m_AnimationSeekBar.m_nSoundTagSrcFrame,
      m_nFrame);
end;

procedure TSoundTagDialog.Button_AddClick(Sender: TObject);
var
  lLoop, lKeep, lInterruptable, nItemIndex : Integer;
begin
  nItemIndex := ComboBox_CandidateList.ItemIndex;
  lLoop := Integer(Check_Loop.Checked);
  lKeep := Integer(Check_MotionFinishKeep.Checked);
  lInterruptable := Integer(Check_Interruptable.Checked);
  if ComboBox_CandidateList.Items.Count = 5 then
    ShowMessage('声音备选项已达到最大个数')
  else
  begin
    ComboBox_CandidateList.Items.Add('New Sound Candidate');
    if m_AnimationSeekBar.FEvents <> nil then
    begin
      if m_nFrame <> -1 then
        m_AnimationSeekBar.FEvents.OnSoundTagSave(m_nFrame,
          lLoop,
          lKeep,
          lInterruptable,
          ComboBox_CandidateList.Items.Count);
    end;
    if nItemIndex = -1 then
      nItemIndex := 0;
    ComboBox_CandidateList.ItemIndex := nItemIndex;
    ComboBox_CandidateListChange(nil);
  end;
end;

procedure TSoundTagDialog.Button_CopyClick(Sender: TObject);
begin
  m_AnimationSeekBar.m_nSoundTagSrcFrame := m_nFrame;
end;

procedure TSoundTagDialog.Button_FilePathClick(Sender: TObject);
var
strSuffix : string;
dlgSoundFileTree : TDlgSourceView;
SoundFileSuffixList : TList;
nItemIndex : Integer;
begin
  strSuffix := 'wav';
  SoundFileSuffixList := TList.Create();
  SoundFileSuffixList.Add(Pointer(strSuffix));
  dlgSoundFileTree := TDlgSourceView.Create(Self);
  dlgSoundFileTree.OnInitDialog(m_AnimationSeekBar,
    m_AnimationSeekBar.m_strSoundFileStartPath,
    SoundFileSuffixList);
  if dlgSoundFileTree.ShowModal() = mrOK then
  begin
    nItemIndex := ComboBox_CandidateList.ItemIndex;
    ComboBox_CandidateList.Items[ComboBox_CandidateList.ItemIndex] := dlgSoundFileTree.m_strSelectedFilePath;
    ComboBox_CandidateList.ItemIndex := nItemIndex;
  end;
end;

procedure TSoundTagDialog.Button_PlayClick(Sender: TObject);
var
  strSoundFilePath : WideString;
  strSoundCandidate : string;
  fVolume, fMinDis, fMaxDis : Single;
begin
  if ComboBox_CandidateList.ItemIndex >= 0 then
  begin
    strSoundCandidate := ComboBox_CandidateList.Items[ComboBox_CandidateList.ItemIndex];
    if (Length(strSoundCandidate) <> 0) and
      (m_AnimationSeekBar.FEvents <> nil) then
    begin
      fVolume := TrackBar_Volume.Position;
      fMinDis := StrToFloat(Edit_MinDis.Text);
      fMaxDis := StrToFloat(Edit_MaxDis.Text);
      strSoundFilePath := WideString(ComboBox_CandidateList.Items[ComboBox_CandidateList.ItemIndex]);
      m_AnimationSeekBar.FEvents.OnPlaySoundFile(strSoundFilePath, fVolume);
    end;
  end;
end;

procedure TSoundTagDialog.ComboBox_CandidateListChange(Sender: TObject);
var
  strSoundFileName : WideString;
  fHitRate, fVolume, fMinDis, fMaxDis : Single;
begin
  if (m_nLastCandidate <> -1) then
  begin
    fHitRate := StrToFloat(Edit_HitRate.Text);
    fVolume := TrackBar_Volume.Position;
    fMinDis := StrToFloat(Edit_MinDis.Text);
    fMaxDis := StrToFloat(Edit_MaxDis.Text);
    strSoundFileName := WideString(ComboBox_CandidateList.Items[m_nLastCandidate]);
    m_AnimationSeekBar.FEvents.OnSaveSoundTagCandidate(m_nFrame,
      m_nLastCandidate,
      strSoundFileName,
      fHitRate,
      fVolume,
      fMinDis,
      fMaxDis);
  end;
  m_AnimationSeekBar.FEvents.OnLoadSoundTagCandidate(m_nFrame,
    ComboBox_CandidateList.ItemIndex,
    strSoundFileName,
    fHitRate,
    fVolume,
    fMinDis,
    fMaxDis);
  if strSoundFileName = '' then
    strSoundFileName := 'No Name';
  Edit_HitRate.Text := FloatToStr(fHitRate);
  Edit_MinDis.Text := FloatToStr(fMinDis);
  Edit_MaxDis.Text := FloatToStr(fMaxDis);
  m_bPlaySound := false;
  TrackBar_Volume.Position := Trunc(fVolume);
  m_bPlaySound := true;
  m_nLastCandidate := ComboBox_CandidateList.ItemIndex;
end;
end.
