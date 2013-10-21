unit AnimationSeekBarImpl1;

{$WARN SYMBOL_PLATFORM OFF}

interface
uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ActiveX, AxCtrls, AnimationSeekBarProj1_TLB, StdVcl, StdCtrls, ExtCtrls,
  Buttons, Gauges, TimeLineBar, Menus;
type
  TAnimationSeekBar = class(TActiveForm, IAnimationSeekBar)
    Panel3: TPanel;
    TimeLineBar1: TTimeLineBar;
    PopupMenu_TagType: TPopupMenu;
    MenuItem_SFX: TMenuItem;
    MenuItem_Sound: TMenuItem;
    MenuItem_Motion: TMenuItem;
    procedure TimeLineBar1Click(Sender: TObject);
    procedure Panel2MouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure ActiveFormCreate(Sender: TObject);
    procedure Shape1MouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Button5Click(Sender: TObject);
    procedure OnSFXMouseDown(Sender: TObject;
      Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure OnMotionMouseDown(Sender: TObject;
      Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure OnSoundMouseDown(Sender: TObject;
      Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure CreateKeyFrameButton(nFrame:Integer;
      KeyFrames: TList);
    procedure Button6Click(Sender: TObject);
    procedure SetViewPort(nLeft : Integer);
    function GetViewPort() : Integer;
    procedure Button1Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Panel4MouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Panel4MouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    //procedure TimeLineBar1Click(Sender: TObject);
    procedure OnPopMenuMotion(Sender: TObject);
    procedure OnPopMenuSFXClick(Sender: TObject);
    procedure OnPopMenuSoundClick(Sender: TObject);
    procedure PopupMenu_TagTypePopup(Sender: TObject);
  private
    { Private declarations }
    KeyFrameButtons : TList;
    m_SelectedSFX : TList;
    m_SelectedSound : TList;
    m_SelectedMotion : TList;
    procedure ActivateEvent(Sender: TObject);
    procedure ClickEvent(Sender: TObject);
    procedure CreateEvent(Sender: TObject);
    procedure MouseMoveEvent(Sender: TObject; Shift: TShiftState; X, Y: Integer);
    procedure DblClickEvent(Sender: TObject);
    procedure DeactivateEvent(Sender: TObject);
    procedure DestroyEvent(Sender: TObject);
    procedure KeyPressEvent(Sender: TObject; var Key: Char);
    procedure PaintEvent(Sender: TObject);
    function IsButtonCreated(nID : Integer) : Boolean;
    procedure DeleteButtonAtFrame(nFrame : Integer);
    procedure UpdateButtonState(nFrame : Integer);
    procedure AddSelection(nType, nID : Integer);
  protected
    procedure ShowMenu(lX, lY, lFrame: Integer); safecall;
    procedure LoadSoundCandidate(lIndex: Integer;
      const strSoundFileName: WideString; fHitRate, fVolume: Single); safecall;
    procedure GetSoundTagNumCandidate(lFrame, lNumCandidate: Integer); safecall;
    procedure SetStartPath(lType: Integer; const strPath: WideString); safecall;
    procedure GetGraduationWidth(out uWidth: SYSINT); safecall;
    procedure Clear; safecall;
    procedure Refresh; safecall;
    procedure SetRange(lNumFrame: Integer); safecall;
    procedure OpenSFXTagEditorDlg(lFrame: Integer); safecall;
    { Protected declarations }
    procedure DefinePropertyPages(DefinePropertyPage: TDefinePropertyPage); override;
    procedure EventSinkChanged(const EventSink: IUnknown); override;
    function Get_Active: WordBool; safecall;
    function Get_AlignDisabled: WordBool; safecall;
    function Get_AutoScroll: WordBool; safecall;
    function Get_AutoSize: WordBool; safecall;
    function Get_AxBorderStyle: TxActiveFormBorderStyle; safecall;
    function Get_Caption: WideString; safecall;
    function Get_Color: OLE_COLOR; safecall;
    function Get_DoubleBuffered: WordBool; safecall;
    function Get_DropTarget: WordBool; safecall;
    function Get_Enabled: WordBool; safecall;
    function Get_Font: IFontDisp; safecall;
    function Get_HelpFile: WideString; safecall;
    function Get_KeyPreview: WordBool; safecall;
    function Get_PixelsPerInch: Integer; safecall;
    function Get_PrintScale: TxPrintScale; safecall;
    function Get_Scaled: WordBool; safecall;
    function Get_ScreenSnap: WordBool; safecall;
    function Get_SnapBuffer: Integer; safecall;
    function Get_Visible: WordBool; safecall;
    function Get_VisibleDockClientCount: Integer; safecall;
    procedure _Set_Font(var Value: IFontDisp); safecall;
    procedure Set_AutoScroll(Value: WordBool); safecall;
    procedure Set_AutoSize(Value: WordBool); safecall;
    procedure Set_AxBorderStyle(Value: TxActiveFormBorderStyle); safecall;
    procedure Set_Caption(const Value: WideString); safecall;
    procedure Set_Color(Value: OLE_COLOR); safecall;
    procedure Set_DoubleBuffered(Value: WordBool); safecall;
    procedure Set_DropTarget(Value: WordBool); safecall;
    procedure Set_Enabled(Value: WordBool); safecall;
    procedure Set_Font(const Value: IFontDisp); safecall;
    procedure Set_HelpFile(const Value: WideString); safecall;
    procedure Set_KeyPreview(Value: WordBool); safecall;
    procedure Set_PixelsPerInch(Value: Integer); safecall;
    procedure Set_PrintScale(Value: TxPrintScale); safecall;
    procedure Set_Scaled(Value: WordBool); safecall;
    procedure Set_ScreenSnap(Value: WordBool); safecall;
    procedure Set_SnapBuffer(Value: Integer); safecall;
    procedure Set_Visible(Value: WordBool); safecall;
  public
    { Public declarations }
    FEvents: IAnimationSeekBarEvents;
    m_nSoundTagSrcFrame, m_nSFXTagSrCFrame, m_nMotionTagSrcFrame : Integer;
    m_strSoundFileStartPath : string;
    procedure Initialize; override;
    procedure OnSFXFileButton(uID : Cardinal);
  end;

implementation

uses ComObj, ComServ, TDlgSFXTag, TDlgSourceTree, TDlgMotionTag, TDlgSoundTag;

{$R *.DFM}

{ TAnimationSeekBar }
var
KeyFrameTotalHeight : Integer = 30;
KeyFrameWidth : Integer = 10;
g_nCurrentNewTagFrameIndex : Integer;
g_dlgSoundTag : TSoundTagDialog;

procedure TAnimationSeekBar.DefinePropertyPages(DefinePropertyPage: TDefinePropertyPage);
begin
  { Define property pages here.  Property pages are defined by calling
    DefinePropertyPage with the class id of the page.  For example,
      DefinePropertyPage(Class_AnimationSeekBarPage); }
end;

procedure TAnimationSeekBar.EventSinkChanged(const EventSink: IUnknown);
begin
  FEvents := EventSink as IAnimationSeekBarEvents;
  inherited EventSinkChanged(EventSink);
end;

procedure TAnimationSeekBar.Initialize;
begin
  inherited Initialize;
  OnActivate := ActivateEvent;
  OnClick := ClickEvent;
  OnMouseMove := MouseMoveEvent;
  OnCreate := CreateEvent;
  OnDblClick := DblClickEvent;
  OnDeactivate := DeactivateEvent;
  OnDestroy := DestroyEvent;
  OnKeyPress := KeyPressEvent;
  OnPaint := PaintEvent;
  KeyFrameButtons := TList.Create();
  TimeLineBar1.SetGraduationWidth(KeyFrameWidth);
  TimeLineBar1.SetGraduationHeight(KeyFrameTotalHeight);
  TimeLineBar1.m_nRangeMin := -1000;
  TimeLineBar1.m_nRangeMax := 1000;
  m_strSoundFileStartPath := 'E:\Sword\sword3-products\trunk\client\data\sound\';
  JITEnable := 1;
  m_SelectedSFX := TList.Create();
  m_SelectedSound := TList.Create();
  m_SelectedMotion := TList.Create();
  g_dlgSoundTag := nil;
end;

function TAnimationSeekBar.Get_Active: WordBool;
begin
  Result := Active;
end;

function TAnimationSeekBar.Get_AlignDisabled: WordBool;
begin
  Result := AlignDisabled;
end;

function TAnimationSeekBar.Get_AutoScroll: WordBool;
begin
  Result := AutoScroll;
end;

function TAnimationSeekBar.Get_AutoSize: WordBool;
begin
  Result := AutoSize;
end;

function TAnimationSeekBar.Get_AxBorderStyle: TxActiveFormBorderStyle;
begin
  Result := Ord(AxBorderStyle);
end;

function TAnimationSeekBar.Get_Caption: WideString;
begin
  Result := WideString(Caption);
end;

function TAnimationSeekBar.Get_Color: OLE_COLOR;
begin
  Result := OLE_COLOR(Color);
end;

function TAnimationSeekBar.Get_DoubleBuffered: WordBool;
begin
  Result := DoubleBuffered;
end;

function TAnimationSeekBar.Get_DropTarget: WordBool;
begin
  Result := DropTarget;
end;

function TAnimationSeekBar.Get_Enabled: WordBool;
begin
  Result := Enabled;
end;

function TAnimationSeekBar.Get_Font: IFontDisp;
begin
  GetOleFont(Font, Result);
end;

function TAnimationSeekBar.Get_HelpFile: WideString;
begin
  Result := WideString(HelpFile);
end;

function TAnimationSeekBar.Get_KeyPreview: WordBool;
begin
  Result := KeyPreview;
end;

function TAnimationSeekBar.Get_PixelsPerInch: Integer;
begin
  Result := PixelsPerInch;
end;

function TAnimationSeekBar.Get_PrintScale: TxPrintScale;
begin
  Result := Ord(PrintScale);
end;

function TAnimationSeekBar.Get_Scaled: WordBool;
begin
  Result := Scaled;
end;

function TAnimationSeekBar.Get_ScreenSnap: WordBool;
begin
  Result := ScreenSnap;
end;

function TAnimationSeekBar.Get_SnapBuffer: Integer;
begin
  Result := SnapBuffer;
end;

function TAnimationSeekBar.Get_Visible: WordBool;
begin
  Result := Visible;
end;

function TAnimationSeekBar.Get_VisibleDockClientCount: Integer;
begin
  Result := VisibleDockClientCount;
end;

procedure TAnimationSeekBar._Set_Font(var Value: IFontDisp);
begin
  SetOleFont(Font, Value);
end;

procedure TAnimationSeekBar.ActivateEvent(Sender: TObject);
begin
  if FEvents <> nil then FEvents.OnActivate;
end;

procedure TAnimationSeekBar.ClickEvent(Sender: TObject);
begin
  if FEvents <> nil then FEvents.OnClick;
end;

procedure TAnimationSeekBar.CreateEvent(Sender: TObject);
begin
  if FEvents <> nil then FEvents.OnCreate;
end;

procedure TAnimationSeekBar.DblClickEvent(Sender: TObject);
begin
  if FEvents <> nil then FEvents.OnDblClick;
end;

procedure TAnimationSeekBar.DeactivateEvent(Sender: TObject);
begin
  if FEvents <> nil then FEvents.OnDeactivate;
end;

procedure TAnimationSeekBar.DestroyEvent(Sender: TObject);
begin
  if FEvents <> nil then FEvents.OnDestroy;
end;

procedure TAnimationSeekBar.KeyPressEvent(Sender: TObject; var Key: Char);
var
  TempKey: Smallint;
begin
  TempKey := Smallint(Key);
  if FEvents <> nil then FEvents.OnKeyPress(TempKey);
  Key := Char(TempKey);
end;

procedure TAnimationSeekBar.PaintEvent(Sender: TObject);
begin
  if FEvents <> nil then FEvents.OnPaint;
end;

procedure TAnimationSeekBar.Set_AutoScroll(Value: WordBool);
begin
  AutoScroll := Value;
end;

procedure TAnimationSeekBar.Set_AutoSize(Value: WordBool);
begin
  AutoSize := Value;
end;

procedure TAnimationSeekBar.Set_AxBorderStyle(
  Value: TxActiveFormBorderStyle);
begin
  AxBorderStyle := TActiveFormBorderStyle(Value);
end;

procedure TAnimationSeekBar.Set_Caption(const Value: WideString);
begin
  Caption := TCaption(Value);
end;

procedure TAnimationSeekBar.Set_Color(Value: OLE_COLOR);
begin
  Color := TColor(Value);
end;

procedure TAnimationSeekBar.Set_DoubleBuffered(Value: WordBool);
begin
  DoubleBuffered := Value;
end;

procedure TAnimationSeekBar.Set_DropTarget(Value: WordBool);
begin
  DropTarget := Value;
end;

procedure TAnimationSeekBar.Set_Enabled(Value: WordBool);
begin
  Enabled := Value;
end;

procedure TAnimationSeekBar.Set_Font(const Value: IFontDisp);
begin
  SetOleFont(Font, Value);
end;

procedure TAnimationSeekBar.Set_HelpFile(const Value: WideString);
begin
  HelpFile := String(Value);
end;

procedure TAnimationSeekBar.Set_KeyPreview(Value: WordBool);
begin
  KeyPreview := Value;
end;

procedure TAnimationSeekBar.Set_PixelsPerInch(Value: Integer);
begin
  PixelsPerInch := Value;
end;

procedure TAnimationSeekBar.Set_PrintScale(Value: TxPrintScale);
begin
  PrintScale := TPrintScale(Value);
end;

procedure TAnimationSeekBar.Set_Scaled(Value: WordBool);
begin
  Scaled := Value;
end;

procedure TAnimationSeekBar.Set_ScreenSnap(Value: WordBool);
begin
  ScreenSnap := Value;
end;

procedure TAnimationSeekBar.Set_SnapBuffer(Value: Integer);
begin
  SnapBuffer := Value;
end;

procedure TAnimationSeekBar.Set_Visible(Value: WordBool);
begin
  Visible := Value;
end;

procedure TAnimationSeekBar.Panel2MouseUp(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  Mouse.Capture := 0;
end;

procedure TAnimationSeekBar.ActiveFormCreate(Sender: TObject);
begin
  DoubleBuffered := True;
end;

procedure TAnimationSeekBar.AddSelection(nType, nID : Integer);
var
  nIndex : Integer;
  bExist : boolean;
begin
  case nType of
  0://SFX
  begin
    bExist := false;
    for  nIndex := 0 to m_SelectedSFX.Count - 1 do
      if Integer(m_SelectedSFX.Items[nIndex]) = nID then
        bExist := true;
    if not bExist then
      m_SelectedSFX.Add(Pointer(nID));
  end;
  1://Sound
  begin
    bExist := false;
    for  nIndex := 0 to m_SelectedSound.Count - 1 do
      if Integer(m_SelectedSound.Items[nIndex]) = nID then
        bExist := true;
    if not bExist then
      m_SelectedSound.Add(Pointer(nID));
  end;
  2://Motion
  begin
    bExist := false;
    for  nIndex := 0 to m_SelectedMotion.Count - 1 do
      if Integer(m_SelectedMotion.Items[nIndex]) = nID then
        bExist := true;
    if not bExist then
      m_SelectedMotion.Add(Pointer(nID));
  end;
  end;
end;

procedure TAnimationSeekBar.OnSFXMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var
  KeyFrameButton : TShape;
  dlgSFX : TSFXTagDialog;
begin
  KeyFrameButton := TShape(Sender);
  if GetKeyState(VK_SHIFT) and 128 = 128 then
  begin
    AddSelection(0, KeyFramebutton.Tag);
  end
  else
  begin
    dlgSFX := TSFXTagDialog.Create(Self);
    g_nCurrentNewTagFrameIndex := KeyFrameButton.Tag;
    dlgSFX.OnInitDialog(KeyFrameButton.Tag);
    dlgSFX.ShowModal();
    UpdateButtonState(KeyFrameButton.Tag);
  end;
end;

procedure TAnimationSeekBar.OnMotionMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var
  KeyFrameButton : TShape;
begin
  KeyFrameButton := TShape(Sender);
  if GetKeyState(VK_SHIFT) and 128 = 128 then
  begin
    AddSelection(2, KeyFramebutton.Tag);
  end
  else
  begin
    g_nCurrentNewTagFrameIndex := KeyFrameButton.Tag;
    FEvents.OnMotionDialogDoModal(g_nCurrentNewTagFrameIndex);
    UpdateButtonState(KeyFrameButton.Tag);
  end;
end;

procedure TAnimationSeekBar.OnSoundMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var
  KeyFrameButton : TShape;
  dlgSound : TSoundTagDialog;
begin
  KeyFrameButton := TShape(Sender);
  if GetKeyState(VK_SHIFT) and 128 = 128 then
  begin
    AddSelection(1, KeyFramebutton.Tag);
  end
  else
  begin
    dlgSound := TSoundTagDialog.Create(Self);
    g_nCurrentNewTagFrameIndex := KeyFrameButton.Tag;
    dlgSound.OnInitDialog(KeyFrameButton.Tag);
    g_dlgSoundTag := dlgSound;
    dlgSound.ShowModal();
    UpdateButtonState(KeyFrameButton.Tag);
    g_dlgSoundTag := nil;
  end;
end;

function TAnimationSeekBar.IsButtonCreated(nID : Integer) : Boolean;
var
  i : Integer;
begin
  IsButtonCreated := false;
  for i := 0 to KeyFrameButtons.Count - 1 do
    if TButton(KeyFrameButtons[i]).Tag = nID then
    begin
      IsButtonCreated := true;
      Break;
    end;
end;

type
  KeyFrameInfo = packed record
  nKeyFrameType : Integer;
  nKeyFrameID   : Integer;
end;

procedure TAnimationSeekBar.CreateKeyFrameButton(nFrame:Integer;
KeyFrames: TList);
var
    NewButton : TShape;
    pKeyFrame : ^KeyFrameInfo;
    Index : Integer;
    nButtonX : Integer;
    StepHeight : Integer;
    Count : Integer;
begin
  Count := KeyFrames.Count;
  if Count = 0 then
    Exit;
  nButtonX := TimeLineBar1.GetFrameX(nFrame);
  if (nButtonX <> -1) then
  begin
    StepHeight := (KeyFrameTotalHeight div Count);
    for Index := 0 to Count - 1 do
    begin
      pKeyFrame := KeyFrames.Items[Index];
      //if (IsButtonCreated(pKeyFrame^.nKeyFrameID)) then
      //  Continue;
      NewButton := TShape.Create(Panel3);
      NewButton.Parent := Panel3;
      NewButton.Left := nButtonX;
      NewButton.Width := KeyFrameWidth;
      NewButton.Top := StepHeight * Index;
      NewButton.Height := StepHeight;
      NewButton.Visible := True;
      NewButton.Tag := pKeyFrame^.nKeyFrameID;
      case pKeyFrame^.nKeyFrameType of
       0:
        begin
          NewButton.OnMouseDown := OnSFXMouseDown;
          NewButton.Brush.Color := clGreen;
        end;
       1:
        begin
          NewButton.OnMouseDown := OnSoundMouseDown;
          NewButton.Brush.Color := clRed;
        end;
       2:
        begin
          NewButton.OnMouseDown := OnMotionMouseDown;
          NewButton.Brush.Color := clBlue;
        end;
     end;
     KeyFrameButtons.Add(NewButton);
   end;
   end;
end;

procedure TAnimationSeekBar.Shape1MouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  ShowMessage('a');
end;

procedure TAnimationSeekBar.DeleteButtonAtFrame(nFrame : Integer);
var
  nIndex : Integer;
  Button : TShape;
  nCount : Integer;
begin
  nCount := KeyFrameButtons.Count;
  if nCount = 0 then
    Exit;
  for nIndex := nCount downto 1 do
  begin
    Button := KeyFrameButtons.Items[nIndex - 1];
    if Button.Tag = nFrame then
    begin
      Button.Destroy();
      KeyFrameButtons.Delete(nIndex - 1);
    end
  end;
end;

procedure TAnimationSeekBar.SetViewPort(nLeft : Integer);
var
  nViewPortOffset : Integer;
  i : Integer;
begin
  nViewPortOffset := TimeLineBar1.GetViewPort();
  TimeLineBar1.SetViewPort(nLeft);
  nLeft := TimeLineBar1.GetViewPort();
  nViewPortOffset := nViewPortOffset - nLeft;
  for i := 0 to KeyFrameButtons.Count - 1 do
  begin
    TButton(KeyFrameButtons[i]).Left := TButton(KeyFrameButtons[i]).Left + nViewPortOffset;
    TButton(KeyFrameButtons[i]).Invalidate();
  end;
end;

function TAnimationSeekBar.GetViewPort() : Integer;
begin
  GetViewPort := TimeLineBar1.GetViewPort();
end;

procedure TAnimationSeekBar.Button5Click(Sender: TObject);
var
j : Integer;
KeyFrames: TList;
pKeyFrame : ^KeyFrameInfo;
begin
  KeyFrames := TList.Create();
  for j := 0 to 2 do
  begin
    New(pKeyFrame);
    pKeyFrame^.nKeyFrameType := j;
    pKeyFrame^.nKeyFrameID := j;
    KeyFrames.Add(pKeyFrame);
  end;
  CreateKeyFrameButton(1, KeyFrames);
  for j := 0 to 2 do
  begin
    dispose(KeyFrames[j]);
  end;
  
end;

procedure TAnimationSeekBar.Button6Click(Sender: TObject);
var
j : Integer;
KeyFrames: TList;
pKeyFrame : ^KeyFrameInfo;
begin
  KeyFrames := TList.Create();
  for j := 3 to 5 do
  begin
    New(pKeyFrame);
    pKeyFrame^.nKeyFrameType := j - 3;
    pKeyFrame^.nKeyFrameID := j;
    KeyFrames.Add(pKeyFrame);
  end;
  CreateKeyFrameButton(15, KeyFrames);
  for j := 0 to 2 do
  begin
    dispose(KeyFrames[j]);
  end;
  SetViewPort(GetViewPort() + 10);
end;

procedure TAnimationSeekBar.Button1Click(Sender: TObject);
begin
  //Panel4.Left := Panel4.Left - KeyFrameWidth;
end;

procedure TAnimationSeekBar.Button4Click(Sender: TObject);
begin
  //Panel4.Left := Panel4.Left + KeyFrameWidth;
end;

procedure TAnimationSeekBar.Panel4MouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  Mouse.Capture := 0;
end;

procedure TAnimationSeekBar.Panel4MouseUp(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  //Mouse.Capture := Panel4.handle;
end;

procedure TAnimationSeekBar.OnSFXFileButton(uID : Cardinal);
begin
   if FEvents <> nil then FEvents.OnSFXSelect(uID);
end;

procedure TAnimationSeekBar.UpdateButtonState(nFrame : Integer);
var
  pKeyFrame : ^KeyFrameInfo;
  KeyFrameList : TList;
  nIndex : Integer;
  nSFXTagExist, nSoundTagExist, nMotionTagExist : Integer;
begin
  DeleteButtonAtFrame(nFrame);
  if FEvents <> nil then
    FEvents.OnQueryExistTagType(nFrame,
      nSFXTagExist,
      nSoundTagExist,
      nMotionTagExist);
  KeyFrameList := TList.Create();
  if nSFXTagExist <> 0 then
  begin
    New(pKeyFrame);
    pKeyFrame^.nKeyFrameType := 0;
    pKeyFrame^.nKeyFrameID := nFrame;
    KeyFrameList.Add(pKeyFrame);
  end;
  if nSoundTagExist <> 0 then
  begin
    New(pKeyFrame);
    pKeyFrame^.nKeyFrameType := 1;
    pKeyFrame^.nKeyFrameID := nFrame;
    KeyFrameList.Add(pKeyFrame);
  end;
  if nMotionTagExist <> 0 then
  begin
    New(pKeyFrame);
    pKeyFrame^.nKeyFrameType := 2;
    pKeyFrame^.nKeyFrameID := nFrame;
    KeyFrameList.Add(pKeyFrame);
  end;
  CreateKeyFrameButton(nFrame, KeyFrameList);
  for nIndex := 0 to KeyFrameList.Count - 1 do
  begin
    dispose(KeyFrameList[nIndex]);
  end;
end;

{procedure TAnimationSeekBar.TimeLineBar1Click(Sender: TObject);
var
  ptMouse : TPoint;
  ptMouseClient : TPoint;
begin
  GetCursorPos(ptMouse);
  ptMouseClient := Panel3.ScreenToClient(ptMouse);
  g_nCurrentNewTagFrameIndex := TimeLineBar1.GetFrameByX(ptMouseClient.x);
  PopupMenu_TagType.Popup(ptMouse.x, ptMouse.y);
  m_SelectedSFX.Clear();
  m_SelectedSound.Clear();
  m_SelectedMotion.Clear();
end;}

procedure TAnimationSeekBar.TimeLineBar1Click(Sender: TObject);
var
  nIndex : Integer;
  nFrameDest : Integer;
  ptMouse : TPoint;
  ptMouseClient : TPoint;
begin
  //TimeLineBar1MouseUp(Sender, mbLeft, ssShift, 0, 0);
  GetCursorPos(ptMouse);
  ptMouseClient := Panel3.ScreenToClient(ptMouse);
  nFrameDest := TimeLineBar1.GetFrameByX(ptMouseClient.x);
  if GetKeyState(VK_SHIFT) and 128 = 128 then
  begin
    for nIndex := 0 to m_SelectedSFX.Count - 1 do
    begin
      if  FEvents <> nil then FEvents.OnKeyFrameMove(0, Integer(m_SelectedSFX.Items[nIndex]), nFrameDest);
      m_SelectedSFX.Items[nIndex] := Pointer(nFrameDest);
    end;
    for nIndex := 0 to m_SelectedSound.Count - 1 do
    begin
      if  FEvents <> nil then FEvents.OnKeyFrameMove(1, Integer(m_SelectedSound.Items[nIndex]), nFrameDest);
      m_SelectedSound.Items[nIndex] := Pointer(nFrameDest);
    end;
    for nIndex := 0 to m_SelectedMotion.Count - 1 do
    begin
      if  FEvents <> nil then FEvents.OnKeyFrameMove(2, Integer(m_SelectedMotion.Items[nIndex]), nFrameDest);
      m_SelectedMotion.Items[nIndex] := Pointer(nFrameDest);
    end;
  end
  else if GetKeyState(VK_CONTROL) and 128 = 128 then
    begin
    for nIndex := 0 to m_SelectedSFX.Count - 1 do
    begin
      if  FEvents <> nil then FEvents.OnKeyFrameCopy(0, Integer(m_SelectedSFX.Items[nIndex]), nFrameDest);
    end;
    for nIndex := 0 to m_SelectedSound.Count - 1 do
    begin
      if  FEvents <> nil then FEvents.OnKeyFrameCopy(1, Integer(m_SelectedSound.Items[nIndex]), nFrameDest);
    end;
    for nIndex := 0 to m_SelectedMotion.Count - 1 do
    begin
      if  FEvents <> nil then FEvents.OnKeyFrameCopy(2, Integer(m_SelectedMotion.Items[nIndex]), nFrameDest);
    end;
  end
  else
  begin
    g_nCurrentNewTagFrameIndex := nFrameDest;
    PopupMenu_TagType.Popup(ptMouse.x, ptMouse.y);
    m_SelectedSFX.Clear();
    m_SelectedSound.Clear();
    m_SelectedMotion.Clear();
  end;
end;

procedure TAnimationSeekBar.OnPopMenuMotion(Sender: TObject);
begin
  if FEvents <> nil then
    FEvents.OnNewMotionTag(g_nCurrentNewTagFrameIndex);
  UpdateButtonState(g_nCurrentNewTagFrameIndex);
end;

procedure TAnimationSeekBar.OnPopMenuSFXClick(Sender: TObject);
begin
  if FEvents <> nil then
    FEvents.OnNewSFXTag(g_nCurrentNewTagFrameIndex);
  UpdateButtonState(g_nCurrentNewTagFrameIndex);
end;


procedure TAnimationSeekBar.OnPopMenuSoundClick(Sender: TObject);
begin
  if FEvents <> nil then
    FEvents.OnNewSoundTag(g_nCurrentNewTagFrameIndex);
  UpdateButtonState(g_nCurrentNewTagFrameIndex);
end;

procedure TAnimationSeekBar.PopupMenu_TagTypePopup(Sender: TObject);
var
  nSFXTagExist, nSoundTagExist, nMotionTagExist : Integer;
begin
  if FEvents <> nil then
    FEvents.OnQueryExistTagType(g_nCurrentNewTagFrameIndex,
      nSFXTagExist,
      nSoundTagExist,
      nMotionTagExist);
  MenuItem_SFX.Enabled := (nSFXTagExist = 0);
  MenuItem_Sound.Enabled := (nSoundTagExist = 0);
  MenuItem_Motion.Enabled := (nMotionTagExist = 0);
end;

procedure TAnimationSeekBar.OpenSFXTagEditorDlg(lFrame: Integer);
var
  dlgSFX : TSFXTagDialog;
begin
  dlgSFX := TSFXTagDialog.Create(Self);
  g_nCurrentNewTagFrameIndex := lFrame;
  dlgSFX.OnInitDialog(lFrame);
  dlgSFX.ShowModal();
  UpdateButtonState(lFrame);
end;

procedure TAnimationSeekBar.Refresh;
var
  I, nStartFrame, nEndFrame: Integer;
begin
  TimeLineBar1.Invalidate();
  nStartFrame := TimeLineBar1.m_nRangeMin div TimeLineBar1.m_nGraduationWidth;
  nEndFrame := TimeLineBar1.m_nRangeMax div TimeLineBar1.m_nGraduationWidth;
  for I := nStartFrame to nEndFrame do
  begin
    UpdateButtonState(I);
  end;

end;

procedure TAnimationSeekBar.SetRange(lNumFrame: Integer);
begin
  TimeLineBar1.m_nRangeMin := 0;
  TimeLineBar1.m_nRangeMax := lNumFrame * TimeLineBar1.m_nGraduationWidth;
end;

procedure TAnimationSeekBar.Clear;
var
  nIndex : Integer;
  Button : TShape;
begin
  for  nIndex:= KeyFrameButtons.Count - 1 downto 0 do
  begin
    Button := KeyFrameButtons.Items[nIndex];
    Button.Destroy;
  end;
  KeyFrameButtons.Clear();
end;

procedure TAnimationSeekBar.GetGraduationWidth(out uWidth: SYSINT);
begin
  uWidth := TimeLineBar1.m_nGraduationWidth;
end;

procedure TAnimationSeekBar.MouseMoveEvent(Sender: TObject; Shift: TShiftState; X, Y: Integer);
begin

end;

procedure TAnimationSeekBar.SetStartPath(lType: Integer;
  const strPath: WideString);
begin
  case lType of
  0://SFX
  begin

  end;
  1://Sound
  begin
    m_strSoundFileStartPath := strPath;
  end;
  end;
end;

procedure TAnimationSeekBar.GetSoundTagNumCandidate(lFrame,
  lNumCandidate: Integer);
begin

end;

procedure TAnimationSeekBar.LoadSoundCandidate(lIndex: Integer;
  const strSoundFileName: WideString; fHitRate, fVolume: Single);
begin
  if g_dlgSoundTag <> nil then
  begin
    if lIndex < g_dlgSoundTag.ComboBox_CandidateList.Items.Count then
    begin
      g_dlgSoundTag.ComboBox_CandidateList.Items[lIndex] := string(strSoundFileName);
      //g_dlgSoundTag.OnCandidateChange(lIndex);
    end;
  end;
end;

procedure TAnimationSeekBar.ShowMenu(lX, lY, lFrame: Integer);
begin
  g_nCurrentNewTagFrameIndex := lFrame;
  PopupMenu_TagType.Popup(lX, lY);
end;

initialization
  TActiveFormFactory.Create(
    ComServer,
    TActiveFormControl,
    TAnimationSeekBar,
    Class_AnimationSeekBar,
    1,
    '',
    OLEMISC_SIMPLEFRAME or OLEMISC_ACTSLIKELABEL,
    tmApartment);
end.
