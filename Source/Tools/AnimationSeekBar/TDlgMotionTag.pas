unit TDlgMotionTag;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, AnimationSeekBarImpl1, ExtCtrls, AnimationSeekBarProj1_TLB;
type
  TMotionTagDialog = class(TForm)
    Label_TagType: TLabel;
    Combo_TagType: TComboBox;
    Button_Delete: TButton;
    Button_Copy: TButton;
    Button_Paste: TButton;
    PanelStrikingEffect: TPanel;
    ComboExtraTagType: TComboBox;
    StaticText1: TStaticText;
    StaticText2: TStaticText;
    EditDamagePercentage: TEdit;
    procedure ComboExtraTagTypeChange(Sender: TObject);
    procedure Combo_TagTypeChange(Sender: TObject);
    procedure Button_PasteClick(Sender: TObject);
    procedure Button_CopyClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
  private
    { Private declarations }
    m_nFrame : Integer;
    m_ExtraInfo : MotionTagExtraInfo;
    procedure UpdateUI(nTagType : Integer);
    procedure UpdateDataFromUI();
    function TagExtraToOleVariant(Data : MotionTagExtraInfo): OleVariant;
    function OleVariantToTagExtra(OleData : OleVariant) : MotionTagExtraInfo;
  public
    { Public declarations }
    m_AnimationSeekBar : AnimationSeekBarImpl1.TAnimationSeekBar;
    procedure OnInitDialog(nFrame : Integer);
  end;

var
  MotionTagDialog: TMotionTagDialog;

implementation
{$R *.dfm}

procedure TMotionTagDialog.UpdateUI(nTagType : Integer);
begin
  Combo_TagType.ItemIndex := nTagType;
  Combo_TagTypeChange(nil);
  case m_ExtraInfo.ExtraType of
    0:
    begin
      //EditDamagePercentage.Text := FloatToStr(m_ExtraInfo.FloatData[0]);
    end;
  end;
end;

procedure TMotionTagDialog.OnInitDialog(nFrame : Integer);
var
  nTagType : Integer;
  OleData : OleVariant;
begin
  m_nFrame := nFrame;
  m_AnimationSeekBar := AnimationSeekBarImpl1.TAnimationSeekBar(Owner);
  if m_AnimationSeekBar.FEvents <> nil then
  begin
    m_AnimationSeekBar.FEvents.OnMotionTagLoad(m_nFrame,
      nTagType,
      OleData);
    m_ExtraInfo := OleVariantToTagExtra(OleData);
    UpdateUI(nTagType);
  end;
end;

procedure TMotionTagDialog.Button_PasteClick(Sender: TObject);
begin
  if m_AnimationSeekBar.FEvents <> nil then
    m_AnimationSeekBar.FEvents.OnSFXTagCopy(m_AnimationSeekBar.m_nSFXTagSrcFrame,
      m_nFrame);
end;

procedure TMotionTagDialog.ComboExtraTagTypeChange(Sender: TObject);
var
  nIndex : Integer;
  PanelList : TList;
  CurrentPanel : TPanel;
begin
  PanelList := TList.Create;
  PanelList.Add(Pointer(PanelStrikingEffect));
  for  nIndex := 0 to ComboExtraTagType.Items.Count - 1 do
  begin
    CurrentPanel := TPanel(PanelList.Items[nIndex]);
    CurrentPanel.Visible := ComboExtraTagType.ItemIndex = nIndex;
  end;
end;

procedure TMotionTagDialog.Combo_TagTypeChange(Sender: TObject);
begin
  ComboExtraTagType.Enabled := (Combo_TagType.ItemIndex = 2);
  if ComboExtraTagType.Enabled then
    ComboExtraTagType.ItemIndex := 0
  else
    ComboExtraTagType.ItemIndex := -1;
  ComboExtraTagTypeChange(nil);
end;

procedure TMotionTagDialog.Button_CopyClick(Sender: TObject);
begin
  m_AnimationSeekBar.m_nMotionTagSrcFrame := m_nFrame;
end;

procedure TMotionTagDialog.FormClose(Sender: TObject;
  var Action: TCloseAction);
var
  OleData : OleVariant;
begin
  if m_AnimationSeekBar.FEvents <> nil then
  begin
    UpdateDataFromUI();
    TagExtraToOleVariant(m_ExtraInfo);
    m_AnimationSeekBar.FEvents.OnMotionTagSave(m_nFrame,
    Combo_TagType.ItemIndex,
    OleData);
  end;
end;

procedure TMotionTagDialog.UpdateDataFromUI();
begin
  if Combo_TagType.ItemIndex = 1 then//User define type
  begin
    case ComboExtraTagType.ItemIndex of
      0://Damage Percentage
      begin
        m_ExtraInfo.ExtraType := 0;
        //m_ExtraInfo.FloatData := StrToFloat(EditDamagePercentage.Text);
      end;
    end;
   end;
end;

function TMotionTagDialog.TagExtraToOleVariant(Data : MotionTagExtraInfo): OleVariant;
var
  P : Pointer;
begin
  Result := VarArrayCreate([0, sizeof(MotionTagExtraInfo)], varByte);
  P := VarArrayLock(Result);
  Move(Data, P^, sizeof(MotionTagExtraInfo));
  VarArrayUnlock(Result);
end;

function TMotionTagDialog.OleVariantToTagExtra(OleData : OleVariant) : MotionTagExtraInfo;
var
  P : Pointer;
begin
  P := VarArrayLock(OleData);
  Move(P^, Result, sizeof(MotionTagExtraInfo));
  VarArrayUnlock(OleData);
end;


end.
