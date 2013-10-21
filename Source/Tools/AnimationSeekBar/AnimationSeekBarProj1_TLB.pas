unit AnimationSeekBarProj1_TLB;

// ************************************************************************ //
// WARNING                                                                    
// -------                                                                    
// The types declared in this file were generated from data read from a       
// Type Library. If this type library is explicitly or indirectly (via        
// another type library referring to this type library) re-imported, or the   
// 'Refresh' command of the Type Library Editor activated while editing the   
// Type Library, the contents of this file will be regenerated and all        
// manual modifications will be lost.                                         
// ************************************************************************ //

// PASTLWTR : 1.2
// File generated on 2008-9-10 20:16:29 from Type Library described below.

// ************************************************************************  //
// Type Lib: D:\Sword\Code\Sword3\Source\Tools\AnimationSeekBar\AnimationSeekBarProj1.tlb (1)
// LIBID: {D21AA20F-1554-4056-A286-90CB63149A09}
// LCID: 0
// Helpfile: 
// HelpString: AnimationSeekBarProj1 Library
// DepndLst: 
//   (1) v2.0 stdole, (C:\WINDOWS\system32\stdole2.tlb)
// ************************************************************************ //
{$TYPEDADDRESS OFF} // Unit must be compiled without type-checked pointers. 
{$WARN SYMBOL_PLATFORM OFF}
{$WRITEABLECONST ON}
{$VARPROPSETTER ON}
interface

uses Windows, ActiveX, Classes, Graphics, OleCtrls, StdVCL, Variants;
  

// *********************************************************************//
// GUIDS declared in the TypeLibrary. Following prefixes are used:        
//   Type Libraries     : LIBID_xxxx                                      
//   CoClasses          : CLASS_xxxx                                      
//   DISPInterfaces     : DIID_xxxx                                       
//   Non-DISP interfaces: IID_xxxx                                        
// *********************************************************************//
const
  // TypeLibrary Major and minor versions
  AnimationSeekBarProj1MajorVersion = 1;
  AnimationSeekBarProj1MinorVersion = 0;

  LIBID_AnimationSeekBarProj1: TGUID = '{D21AA20F-1554-4056-A286-90CB63149A09}';

  IID_IAnimationSeekBar: TGUID = '{CDA7C830-16C8-48C3-9CBF-B725D65AFF31}';
  DIID_IAnimationSeekBarEvents: TGUID = '{761D98CA-3136-441E-8941-997675879A5C}';
  CLASS_AnimationSeekBar: TGUID = '{7B134338-E6E6-4E8C-8E06-0C65103196CE}';

// *********************************************************************//
// Declaration of Enumerations defined in Type Library                    
// *********************************************************************//
// Constants for enum TxActiveFormBorderStyle
type
  TxActiveFormBorderStyle = TOleEnum;
const
  afbNone = $00000000;
  afbSingle = $00000001;
  afbSunken = $00000002;
  afbRaised = $00000003;

// Constants for enum TxPrintScale
type
  TxPrintScale = TOleEnum;
const
  poNone = $00000000;
  poProportional = $00000001;
  poPrintToFit = $00000002;

// Constants for enum TxMouseButton
type
  TxMouseButton = TOleEnum;
const
  mbLeft = $00000000;
  mbRight = $00000001;
  mbMiddle = $00000002;

type

// *********************************************************************//
// Forward declaration of types defined in TypeLibrary                    
// *********************************************************************//
  IAnimationSeekBar = interface;
  IAnimationSeekBarDisp = dispinterface;
  IAnimationSeekBarEvents = dispinterface;

// *********************************************************************//
// Declaration of CoClasses defined in Type Library                       
// (NOTE: Here we map each CoClass to its Default Interface)              
// *********************************************************************//
  AnimationSeekBar = IAnimationSeekBar;


// *********************************************************************//
// Declaration of structures, unions and aliases.                         
// *********************************************************************//
  PPUserType1 = ^IFontDisp; {*}

  MotionTagExtraInfo = packed record
    ExtraType: Integer;
    dwData0: Integer;
    dwData1: Integer;
    fData0: Single;
    fData1: Single;
  end;


// *********************************************************************//
// Interface: IAnimationSeekBar
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {CDA7C830-16C8-48C3-9CBF-B725D65AFF31}
// *********************************************************************//
  IAnimationSeekBar = interface(IDispatch)
    ['{CDA7C830-16C8-48C3-9CBF-B725D65AFF31}']
    function Get_Visible: WordBool; safecall;
    procedure Set_Visible(Value: WordBool); safecall;
    function Get_AutoScroll: WordBool; safecall;
    procedure Set_AutoScroll(Value: WordBool); safecall;
    function Get_AutoSize: WordBool; safecall;
    procedure Set_AutoSize(Value: WordBool); safecall;
    function Get_AxBorderStyle: TxActiveFormBorderStyle; safecall;
    procedure Set_AxBorderStyle(Value: TxActiveFormBorderStyle); safecall;
    function Get_Caption: WideString; safecall;
    procedure Set_Caption(const Value: WideString); safecall;
    function Get_Color: OLE_COLOR; safecall;
    procedure Set_Color(Value: OLE_COLOR); safecall;
    function Get_Font: IFontDisp; safecall;
    procedure Set_Font(const Value: IFontDisp); safecall;
    procedure _Set_Font(var Value: IFontDisp); safecall;
    function Get_KeyPreview: WordBool; safecall;
    procedure Set_KeyPreview(Value: WordBool); safecall;
    function Get_PixelsPerInch: Integer; safecall;
    procedure Set_PixelsPerInch(Value: Integer); safecall;
    function Get_PrintScale: TxPrintScale; safecall;
    procedure Set_PrintScale(Value: TxPrintScale); safecall;
    function Get_Scaled: WordBool; safecall;
    procedure Set_Scaled(Value: WordBool); safecall;
    function Get_Active: WordBool; safecall;
    function Get_DropTarget: WordBool; safecall;
    procedure Set_DropTarget(Value: WordBool); safecall;
    function Get_HelpFile: WideString; safecall;
    procedure Set_HelpFile(const Value: WideString); safecall;
    function Get_ScreenSnap: WordBool; safecall;
    procedure Set_ScreenSnap(Value: WordBool); safecall;
    function Get_SnapBuffer: Integer; safecall;
    procedure Set_SnapBuffer(Value: Integer); safecall;
    function Get_DoubleBuffered: WordBool; safecall;
    procedure Set_DoubleBuffered(Value: WordBool); safecall;
    function Get_AlignDisabled: WordBool; safecall;
    function Get_VisibleDockClientCount: Integer; safecall;
    function Get_Enabled: WordBool; safecall;
    procedure Set_Enabled(Value: WordBool); safecall;
    procedure OpenSFXTagEditorDlg(lFrame: Integer); safecall;
    procedure SetRange(lNumFrame: Integer); safecall;
    procedure Refresh; safecall;
    procedure Clear; safecall;
    procedure GetGraduationWidth(out uWidth: SYSINT); safecall;
    procedure SetStartPath(lType: Integer; const strPath: WideString); safecall;
    procedure GetSoundTagNumCandidate(lFrame: Integer; lNumCandidate: Integer); safecall;
    procedure LoadSoundCandidate(lIndex: Integer; const strSoundFileName: WideString; 
                                 fHitRate: Single; fVolume: Single); safecall;
    procedure ShowMenu(lX: Integer; lY: Integer; lFrame: Integer); safecall;
    property Visible: WordBool read Get_Visible write Set_Visible;
    property AutoScroll: WordBool read Get_AutoScroll write Set_AutoScroll;
    property AutoSize: WordBool read Get_AutoSize write Set_AutoSize;
    property AxBorderStyle: TxActiveFormBorderStyle read Get_AxBorderStyle write Set_AxBorderStyle;
    property Caption: WideString read Get_Caption write Set_Caption;
    property Color: OLE_COLOR read Get_Color write Set_Color;
    property Font: IFontDisp read Get_Font write Set_Font;
    property KeyPreview: WordBool read Get_KeyPreview write Set_KeyPreview;
    property PixelsPerInch: Integer read Get_PixelsPerInch write Set_PixelsPerInch;
    property PrintScale: TxPrintScale read Get_PrintScale write Set_PrintScale;
    property Scaled: WordBool read Get_Scaled write Set_Scaled;
    property Active: WordBool read Get_Active;
    property DropTarget: WordBool read Get_DropTarget write Set_DropTarget;
    property HelpFile: WideString read Get_HelpFile write Set_HelpFile;
    property ScreenSnap: WordBool read Get_ScreenSnap write Set_ScreenSnap;
    property SnapBuffer: Integer read Get_SnapBuffer write Set_SnapBuffer;
    property DoubleBuffered: WordBool read Get_DoubleBuffered write Set_DoubleBuffered;
    property AlignDisabled: WordBool read Get_AlignDisabled;
    property VisibleDockClientCount: Integer read Get_VisibleDockClientCount;
    property Enabled: WordBool read Get_Enabled write Set_Enabled;
  end;

// *********************************************************************//
// DispIntf:  IAnimationSeekBarDisp
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {CDA7C830-16C8-48C3-9CBF-B725D65AFF31}
// *********************************************************************//
  IAnimationSeekBarDisp = dispinterface
    ['{CDA7C830-16C8-48C3-9CBF-B725D65AFF31}']
    property Visible: WordBool dispid 201;
    property AutoScroll: WordBool dispid 202;
    property AutoSize: WordBool dispid 203;
    property AxBorderStyle: TxActiveFormBorderStyle dispid 204;
    property Caption: WideString dispid -518;
    property Color: OLE_COLOR dispid -501;
    property Font: IFontDisp dispid -512;
    property KeyPreview: WordBool dispid 205;
    property PixelsPerInch: Integer dispid 206;
    property PrintScale: TxPrintScale dispid 207;
    property Scaled: WordBool dispid 208;
    property Active: WordBool readonly dispid 209;
    property DropTarget: WordBool dispid 210;
    property HelpFile: WideString dispid 211;
    property ScreenSnap: WordBool dispid 212;
    property SnapBuffer: Integer dispid 213;
    property DoubleBuffered: WordBool dispid 214;
    property AlignDisabled: WordBool readonly dispid 215;
    property VisibleDockClientCount: Integer readonly dispid 216;
    property Enabled: WordBool dispid -514;
    procedure OpenSFXTagEditorDlg(lFrame: Integer); dispid 217;
    procedure SetRange(lNumFrame: Integer); dispid 218;
    procedure Refresh; dispid 219;
    procedure Clear; dispid 220;
    procedure GetGraduationWidth(out uWidth: SYSINT); dispid 221;
    procedure SetStartPath(lType: Integer; const strPath: WideString); dispid 222;
    procedure GetSoundTagNumCandidate(lFrame: Integer; lNumCandidate: Integer); dispid 223;
    procedure LoadSoundCandidate(lIndex: Integer; const strSoundFileName: WideString; 
                                 fHitRate: Single; fVolume: Single); dispid 224;
    procedure ShowMenu(lX: Integer; lY: Integer; lFrame: Integer); dispid 225;
  end;

// *********************************************************************//
// DispIntf:  IAnimationSeekBarEvents
// Flags:     (4096) Dispatchable
// GUID:      {761D98CA-3136-441E-8941-997675879A5C}
// *********************************************************************//
  IAnimationSeekBarEvents = dispinterface
    ['{761D98CA-3136-441E-8941-997675879A5C}']
    procedure OnActivate; dispid 201;
    procedure OnClick; dispid 202;
    procedure OnCreate; dispid 203;
    procedure OnDblClick; dispid 204;
    procedure OnDestroy; dispid 205;
    procedure OnDeactivate; dispid 206;
    procedure OnKeyPress(var Key: Smallint); dispid 207;
    procedure OnPaint; dispid 208;
    procedure OnSFXSelect(uID: SYSINT); dispid 209;
    procedure OnSFXTagSave(nTagID: SYSINT; nPlayType: SYSINT; fPlaySpeed: Single; 
                           nAutoAdaptSpeed: SYSINT; nInterruptable: SYSINT; 
                           nKeepMotionFinish: SYSINT; nNotify: SYSINT; 
                           const strTagName: WideString; nBindID: Integer); dispid 210;
    procedure OnSFXTagLoad(nID: SYSINT; out nPlayType: SYSINT; out fPlaySpeed: Single; 
                           out nAutoAdaptSpeed: SYSINT; out nInterruptable: SYSINT; 
                           out nKeepMotionFinish: SYSINT; out nNotify: SYSINT; 
                           out nBindIndex: SYSINT; out strSFXFileName: WideString; 
                           out strTagName: WideString); dispid 211;
    procedure OnSFXTagNew(out nID: SYSINT; nFrame: SYSINT); dispid 212;
    procedure OnSFXTagDelete(nID: SYSINT); dispid 213;
    procedure OnSFXTagFillTagList(nFrame: Integer; nCount: Integer; out strTagName: WideString; 
                                  out nID: Integer; out nReturn: Integer); dispid 214;
    procedure OnQueryExistTagType(lFrame: Integer; out bSFXTag: Integer; out bSoundTag: Integer; 
                                  out bMotionTag: Integer); dispid 215;
    procedure OnNewSFXTag(nFrame: Integer); dispid 216;
    procedure OnNewSoundTag(nFrame: Integer); dispid 217;
    procedure OnNewMotionTag(nFrame: Integer); dispid 218;
    procedure OnSoundTagSave(lID: Integer; lLoop: Integer; lKeep: Integer; lInterruptable: Integer; 
                             lNumCandidate: Integer); dispid 219;
    procedure OnSoundTagLoad(lID: Integer; out bLoop: Integer; out bKeep: Integer; 
                             out bInterruptable: Integer; out lNumCanidate: Integer); dispid 220;
    procedure OnSoundTagDelete(nID: Integer); dispid 221;
    procedure OnMotionTagSave(nID: Integer; lType: Integer; 
                              ExtraInfo: {??MotionTagExtraInfo}OleVariant); dispid 222;
    procedure OnMotionTagLoad(nID: Integer; out lType: Integer; 
                              out ExtraInfo: {??MotionTagExtraInfo}OleVariant); dispid 223;
    procedure OnMotionTagDelete(nID: Integer); dispid 224;
    procedure OnSFXTagCopy(lSrc: Integer; lDest: Integer); dispid 225;
    procedure OnSoundTagCopy(lSrc: Integer; lDest: Integer); dispid 226;
    procedure OnMotionTagCopy(lSrc: Integer; lDest: Integer); dispid 227;
    procedure OnSoundFileSelection(const strFilePath: WideString); dispid 228;
    procedure OnPlaySoundFile(const strSoundFilePath: WideString; fVolume: Single); dispid 229;
    procedure OnMotionDialogDoModal(nFrame: Integer); dispid 230;
    procedure OnKeyFrameMove(lType: Integer; lID: Integer; FrameDest: Integer); dispid 231;
    procedure OnKeyFrameCopy(lType: Integer; lID: Integer; lDest: Integer); dispid 232;
    procedure OnSaveSoundTagCandidate(lFrame: Integer; lIndex: Integer; 
                                      const strSoundFileName: WideString; fHitRate: Single; 
                                      fVolume: Single; fMinDis: Single; fMaxDis: Single); dispid 233;
    procedure OnLoadSoundTagCandidate(lFrame: Integer; lIndex: Integer; 
                                      out strSoundFileName: WideString; out fHitRate: Single; 
                                      out fVolume: Single; out fMinDis: Single; out fMaxDis: Single); dispid 234;
    procedure OnDeleteSoundTagCandidate(lFrame: Integer; lCandidate: Integer); dispid 235;
  end;


// *********************************************************************//
// OLE Control Proxy class declaration
// Control Name     : TAnimationSeekBar
// Help String      : AnimationSeekBar Control
// Default Interface: IAnimationSeekBar
// Def. Intf. DISP? : No
// Event   Interface: IAnimationSeekBarEvents
// TypeFlags        : (34) CanCreate Control
// *********************************************************************//
  TAnimationSeekBarOnKeyPress = procedure(ASender: TObject; var Key: Smallint) of object;
  TAnimationSeekBarOnSFXSelect = procedure(ASender: TObject; uID: SYSINT) of object;
  TAnimationSeekBarOnSFXTagSave = procedure(ASender: TObject; nTagID: SYSINT; nPlayType: SYSINT; 
                                                              fPlaySpeed: Single; 
                                                              nAutoAdaptSpeed: SYSINT; 
                                                              nInterruptable: SYSINT; 
                                                              nKeepMotionFinish: SYSINT; 
                                                              nNotify: SYSINT; 
                                                              const strTagName: WideString; 
                                                              nBindID: Integer) of object;
  TAnimationSeekBarOnSFXTagLoad = procedure(ASender: TObject; nID: SYSINT; out nPlayType: SYSINT; 
                                                              out fPlaySpeed: Single; 
                                                              out nAutoAdaptSpeed: SYSINT; 
                                                              out nInterruptable: SYSINT; 
                                                              out nKeepMotionFinish: SYSINT; 
                                                              out nNotify: SYSINT; 
                                                              out nBindIndex: SYSINT; 
                                                              out strSFXFileName: WideString; 
                                                              out strTagName: WideString) of object;
  TAnimationSeekBarOnSFXTagNew = procedure(ASender: TObject; out nID: SYSINT; nFrame: SYSINT) of object;
  TAnimationSeekBarOnSFXTagDelete = procedure(ASender: TObject; nID: SYSINT) of object;
  TAnimationSeekBarOnSFXTagFillTagList = procedure(ASender: TObject; nFrame: Integer; 
                                                                     nCount: Integer; 
                                                                     out strTagName: WideString; 
                                                                     out nID: Integer; 
                                                                     out nReturn: Integer) of object;
  TAnimationSeekBarOnQueryExistTagType = procedure(ASender: TObject; lFrame: Integer; 
                                                                     out bSFXTag: Integer; 
                                                                     out bSoundTag: Integer; 
                                                                     out bMotionTag: Integer) of object;
  TAnimationSeekBarOnNewSFXTag = procedure(ASender: TObject; nFrame: Integer) of object;
  TAnimationSeekBarOnNewSoundTag = procedure(ASender: TObject; nFrame: Integer) of object;
  TAnimationSeekBarOnNewMotionTag = procedure(ASender: TObject; nFrame: Integer) of object;
  TAnimationSeekBarOnSoundTagSave = procedure(ASender: TObject; lID: Integer; lLoop: Integer; 
                                                                lKeep: Integer; 
                                                                lInterruptable: Integer; 
                                                                lNumCandidate: Integer) of object;
  TAnimationSeekBarOnSoundTagLoad = procedure(ASender: TObject; lID: Integer; out bLoop: Integer; 
                                                                out bKeep: Integer; 
                                                                out bInterruptable: Integer; 
                                                                out lNumCanidate: Integer) of object;
  TAnimationSeekBarOnSoundTagDelete = procedure(ASender: TObject; nID: Integer) of object;
  TAnimationSeekBarOnMotionTagSave = procedure(ASender: TObject; nID: Integer; lType: Integer; 
                                                                 ExtraInfo: {??MotionTagExtraInfo}OleVariant) of object;
  TAnimationSeekBarOnMotionTagLoad = procedure(ASender: TObject; nID: Integer; out lType: Integer; 
                                                                 out ExtraInfo: {??MotionTagExtraInfo}OleVariant) of object;
  TAnimationSeekBarOnMotionTagDelete = procedure(ASender: TObject; nID: Integer) of object;
  TAnimationSeekBarOnSFXTagCopy = procedure(ASender: TObject; lSrc: Integer; lDest: Integer) of object;
  TAnimationSeekBarOnSoundTagCopy = procedure(ASender: TObject; lSrc: Integer; lDest: Integer) of object;
  TAnimationSeekBarOnMotionTagCopy = procedure(ASender: TObject; lSrc: Integer; lDest: Integer) of object;
  TAnimationSeekBarOnSoundFileSelection = procedure(ASender: TObject; const strFilePath: WideString) of object;
  TAnimationSeekBarOnPlaySoundFile = procedure(ASender: TObject; const strSoundFilePath: WideString; 
                                                                 fVolume: Single) of object;
  TAnimationSeekBarOnMotionDialogDoModal = procedure(ASender: TObject; nFrame: Integer) of object;
  TAnimationSeekBarOnKeyFrameMove = procedure(ASender: TObject; lType: Integer; lID: Integer; 
                                                                FrameDest: Integer) of object;
  TAnimationSeekBarOnKeyFrameCopy = procedure(ASender: TObject; lType: Integer; lID: Integer; 
                                                                lDest: Integer) of object;
  TAnimationSeekBarOnSaveSoundTagCandidate = procedure(ASender: TObject; lFrame: Integer; 
                                                                         lIndex: Integer; 
                                                                         const strSoundFileName: WideString; 
                                                                         fHitRate: Single; 
                                                                         fVolume: Single; 
                                                                         fMinDis: Single; 
                                                                         fMaxDis: Single) of object;
  TAnimationSeekBarOnLoadSoundTagCandidate = procedure(ASender: TObject; lFrame: Integer; 
                                                                         lIndex: Integer; 
                                                                         out strSoundFileName: WideString; 
                                                                         out fHitRate: Single; 
                                                                         out fVolume: Single; 
                                                                         out fMinDis: Single; 
                                                                         out fMaxDis: Single) of object;
  TAnimationSeekBarOnDeleteSoundTagCandidate = procedure(ASender: TObject; lFrame: Integer; 
                                                                           lCandidate: Integer) of object;

  TAnimationSeekBar = class(TOleControl)
  private
    FOnActivate: TNotifyEvent;
    FOnClick: TNotifyEvent;
    FOnCreate: TNotifyEvent;
    FOnDblClick: TNotifyEvent;
    FOnDestroy: TNotifyEvent;
    FOnDeactivate: TNotifyEvent;
    FOnKeyPress: TAnimationSeekBarOnKeyPress;
    FOnPaint: TNotifyEvent;
    FOnSFXSelect: TAnimationSeekBarOnSFXSelect;
    FOnSFXTagSave: TAnimationSeekBarOnSFXTagSave;
    FOnSFXTagLoad: TAnimationSeekBarOnSFXTagLoad;
    FOnSFXTagNew: TAnimationSeekBarOnSFXTagNew;
    FOnSFXTagDelete: TAnimationSeekBarOnSFXTagDelete;
    FOnSFXTagFillTagList: TAnimationSeekBarOnSFXTagFillTagList;
    FOnQueryExistTagType: TAnimationSeekBarOnQueryExistTagType;
    FOnNewSFXTag: TAnimationSeekBarOnNewSFXTag;
    FOnNewSoundTag: TAnimationSeekBarOnNewSoundTag;
    FOnNewMotionTag: TAnimationSeekBarOnNewMotionTag;
    FOnSoundTagSave: TAnimationSeekBarOnSoundTagSave;
    FOnSoundTagLoad: TAnimationSeekBarOnSoundTagLoad;
    FOnSoundTagDelete: TAnimationSeekBarOnSoundTagDelete;
    FOnMotionTagSave: TAnimationSeekBarOnMotionTagSave;
    FOnMotionTagLoad: TAnimationSeekBarOnMotionTagLoad;
    FOnMotionTagDelete: TAnimationSeekBarOnMotionTagDelete;
    FOnSFXTagCopy: TAnimationSeekBarOnSFXTagCopy;
    FOnSoundTagCopy: TAnimationSeekBarOnSoundTagCopy;
    FOnMotionTagCopy: TAnimationSeekBarOnMotionTagCopy;
    FOnSoundFileSelection: TAnimationSeekBarOnSoundFileSelection;
    FOnPlaySoundFile: TAnimationSeekBarOnPlaySoundFile;
    FOnMotionDialogDoModal: TAnimationSeekBarOnMotionDialogDoModal;
    FOnKeyFrameMove: TAnimationSeekBarOnKeyFrameMove;
    FOnKeyFrameCopy: TAnimationSeekBarOnKeyFrameCopy;
    FOnSaveSoundTagCandidate: TAnimationSeekBarOnSaveSoundTagCandidate;
    FOnLoadSoundTagCandidate: TAnimationSeekBarOnLoadSoundTagCandidate;
    FOnDeleteSoundTagCandidate: TAnimationSeekBarOnDeleteSoundTagCandidate;
    FIntf: IAnimationSeekBar;
    function  GetControlInterface: IAnimationSeekBar;
  protected
    procedure CreateControl;
    procedure InitControlData; override;
  public
    procedure OpenSFXTagEditorDlg(lFrame: Integer);
    procedure SetRange(lNumFrame: Integer);
    procedure Refresh;
    procedure Clear;
    procedure GetGraduationWidth(out uWidth: SYSINT);
    procedure SetStartPath(lType: Integer; const strPath: WideString);
    procedure GetSoundTagNumCandidate(lFrame: Integer; lNumCandidate: Integer);
    procedure LoadSoundCandidate(lIndex: Integer; const strSoundFileName: WideString; 
                                 fHitRate: Single; fVolume: Single);
    procedure ShowMenu(lX: Integer; lY: Integer; lFrame: Integer);
    property  ControlInterface: IAnimationSeekBar read GetControlInterface;
    property  DefaultInterface: IAnimationSeekBar read GetControlInterface;
    property Visible: WordBool index 201 read GetWordBoolProp write SetWordBoolProp;
    property Active: WordBool index 209 read GetWordBoolProp;
    property DropTarget: WordBool index 210 read GetWordBoolProp write SetWordBoolProp;
    property HelpFile: WideString index 211 read GetWideStringProp write SetWideStringProp;
    property ScreenSnap: WordBool index 212 read GetWordBoolProp write SetWordBoolProp;
    property SnapBuffer: Integer index 213 read GetIntegerProp write SetIntegerProp;
    property DoubleBuffered: WordBool index 214 read GetWordBoolProp write SetWordBoolProp;
    property AlignDisabled: WordBool index 215 read GetWordBoolProp;
    property VisibleDockClientCount: Integer index 216 read GetIntegerProp;
    property Enabled: WordBool index -514 read GetWordBoolProp write SetWordBoolProp;
  published
    property Anchors;
    property  ParentColor;
    property  ParentFont;
    property  Align;
    property  DragCursor;
    property  DragMode;
    property  ParentShowHint;
    property  PopupMenu;
    property  ShowHint;
    property  TabOrder;
    property  OnDragDrop;
    property  OnDragOver;
    property  OnEndDrag;
    property  OnEnter;
    property  OnExit;
    property  OnStartDrag;
    property AutoScroll: WordBool index 202 read GetWordBoolProp write SetWordBoolProp stored False;
    property AutoSize: WordBool index 203 read GetWordBoolProp write SetWordBoolProp stored False;
    property AxBorderStyle: TOleEnum index 204 read GetTOleEnumProp write SetTOleEnumProp stored False;
    property Caption: WideString index -518 read GetWideStringProp write SetWideStringProp stored False;
    property Color: TColor index -501 read GetTColorProp write SetTColorProp stored False;
    property Font: TFont index -512 read GetTFontProp write SetTFontProp stored False;
    property KeyPreview: WordBool index 205 read GetWordBoolProp write SetWordBoolProp stored False;
    property PixelsPerInch: Integer index 206 read GetIntegerProp write SetIntegerProp stored False;
    property PrintScale: TOleEnum index 207 read GetTOleEnumProp write SetTOleEnumProp stored False;
    property Scaled: WordBool index 208 read GetWordBoolProp write SetWordBoolProp stored False;
    property OnActivate: TNotifyEvent read FOnActivate write FOnActivate;
    property OnClick: TNotifyEvent read FOnClick write FOnClick;
    property OnCreate: TNotifyEvent read FOnCreate write FOnCreate;
    property OnDblClick: TNotifyEvent read FOnDblClick write FOnDblClick;
    property OnDestroy: TNotifyEvent read FOnDestroy write FOnDestroy;
    property OnDeactivate: TNotifyEvent read FOnDeactivate write FOnDeactivate;
    property OnKeyPress: TAnimationSeekBarOnKeyPress read FOnKeyPress write FOnKeyPress;
    property OnPaint: TNotifyEvent read FOnPaint write FOnPaint;
    property OnSFXSelect: TAnimationSeekBarOnSFXSelect read FOnSFXSelect write FOnSFXSelect;
    property OnSFXTagSave: TAnimationSeekBarOnSFXTagSave read FOnSFXTagSave write FOnSFXTagSave;
    property OnSFXTagLoad: TAnimationSeekBarOnSFXTagLoad read FOnSFXTagLoad write FOnSFXTagLoad;
    property OnSFXTagNew: TAnimationSeekBarOnSFXTagNew read FOnSFXTagNew write FOnSFXTagNew;
    property OnSFXTagDelete: TAnimationSeekBarOnSFXTagDelete read FOnSFXTagDelete write FOnSFXTagDelete;
    property OnSFXTagFillTagList: TAnimationSeekBarOnSFXTagFillTagList read FOnSFXTagFillTagList write FOnSFXTagFillTagList;
    property OnQueryExistTagType: TAnimationSeekBarOnQueryExistTagType read FOnQueryExistTagType write FOnQueryExistTagType;
    property OnNewSFXTag: TAnimationSeekBarOnNewSFXTag read FOnNewSFXTag write FOnNewSFXTag;
    property OnNewSoundTag: TAnimationSeekBarOnNewSoundTag read FOnNewSoundTag write FOnNewSoundTag;
    property OnNewMotionTag: TAnimationSeekBarOnNewMotionTag read FOnNewMotionTag write FOnNewMotionTag;
    property OnSoundTagSave: TAnimationSeekBarOnSoundTagSave read FOnSoundTagSave write FOnSoundTagSave;
    property OnSoundTagLoad: TAnimationSeekBarOnSoundTagLoad read FOnSoundTagLoad write FOnSoundTagLoad;
    property OnSoundTagDelete: TAnimationSeekBarOnSoundTagDelete read FOnSoundTagDelete write FOnSoundTagDelete;
    property OnMotionTagSave: TAnimationSeekBarOnMotionTagSave read FOnMotionTagSave write FOnMotionTagSave;
    property OnMotionTagLoad: TAnimationSeekBarOnMotionTagLoad read FOnMotionTagLoad write FOnMotionTagLoad;
    property OnMotionTagDelete: TAnimationSeekBarOnMotionTagDelete read FOnMotionTagDelete write FOnMotionTagDelete;
    property OnSFXTagCopy: TAnimationSeekBarOnSFXTagCopy read FOnSFXTagCopy write FOnSFXTagCopy;
    property OnSoundTagCopy: TAnimationSeekBarOnSoundTagCopy read FOnSoundTagCopy write FOnSoundTagCopy;
    property OnMotionTagCopy: TAnimationSeekBarOnMotionTagCopy read FOnMotionTagCopy write FOnMotionTagCopy;
    property OnSoundFileSelection: TAnimationSeekBarOnSoundFileSelection read FOnSoundFileSelection write FOnSoundFileSelection;
    property OnPlaySoundFile: TAnimationSeekBarOnPlaySoundFile read FOnPlaySoundFile write FOnPlaySoundFile;
    property OnMotionDialogDoModal: TAnimationSeekBarOnMotionDialogDoModal read FOnMotionDialogDoModal write FOnMotionDialogDoModal;
    property OnKeyFrameMove: TAnimationSeekBarOnKeyFrameMove read FOnKeyFrameMove write FOnKeyFrameMove;
    property OnKeyFrameCopy: TAnimationSeekBarOnKeyFrameCopy read FOnKeyFrameCopy write FOnKeyFrameCopy;
    property OnSaveSoundTagCandidate: TAnimationSeekBarOnSaveSoundTagCandidate read FOnSaveSoundTagCandidate write FOnSaveSoundTagCandidate;
    property OnLoadSoundTagCandidate: TAnimationSeekBarOnLoadSoundTagCandidate read FOnLoadSoundTagCandidate write FOnLoadSoundTagCandidate;
    property OnDeleteSoundTagCandidate: TAnimationSeekBarOnDeleteSoundTagCandidate read FOnDeleteSoundTagCandidate write FOnDeleteSoundTagCandidate;
  end;

procedure Register;

resourcestring
  dtlServerPage = 'Servers';

  dtlOcxPage = 'ActiveX';

implementation

uses ComObj;

procedure TAnimationSeekBar.InitControlData;
const
  CEventDispIDs: array [0..34] of DWORD = (
    $000000C9, $000000CA, $000000CB, $000000CC, $000000CD, $000000CE,
    $000000CF, $000000D0, $000000D1, $000000D2, $000000D3, $000000D4,
    $000000D5, $000000D6, $000000D7, $000000D8, $000000D9, $000000DA,
    $000000DB, $000000DC, $000000DD, $000000DE, $000000DF, $000000E0,
    $000000E1, $000000E2, $000000E3, $000000E4, $000000E5, $000000E6,
    $000000E7, $000000E8, $000000E9, $000000EA, $000000EB);
  CTFontIDs: array [0..0] of DWORD = (
    $FFFFFE00);
  CControlData: TControlData2 = (
    ClassID: '{7B134338-E6E6-4E8C-8E06-0C65103196CE}';
    EventIID: '{761D98CA-3136-441E-8941-997675879A5C}';
    EventCount: 35;
    EventDispIDs: @CEventDispIDs;
    LicenseKey: nil (*HR:$00000000*);
    Flags: $0000001D;
    Version: 401;
    FontCount: 1;
    FontIDs: @CTFontIDs);
begin
  ControlData := @CControlData;
  TControlData2(CControlData).FirstEventOfs := Cardinal(@@FOnActivate) - Cardinal(Self);
end;

procedure TAnimationSeekBar.CreateControl;

  procedure DoCreate;
  begin
    FIntf := IUnknown(OleObject) as IAnimationSeekBar;
  end;

begin
  if FIntf = nil then DoCreate;
end;

function TAnimationSeekBar.GetControlInterface: IAnimationSeekBar;
begin
  CreateControl;
  Result := FIntf;
end;

procedure TAnimationSeekBar.OpenSFXTagEditorDlg(lFrame: Integer);
begin
  DefaultInterface.OpenSFXTagEditorDlg(lFrame);
end;

procedure TAnimationSeekBar.SetRange(lNumFrame: Integer);
begin
  DefaultInterface.SetRange(lNumFrame);
end;

procedure TAnimationSeekBar.Refresh;
begin
  DefaultInterface.Refresh;
end;

procedure TAnimationSeekBar.Clear;
begin
  DefaultInterface.Clear;
end;

procedure TAnimationSeekBar.GetGraduationWidth(out uWidth: SYSINT);
begin
  DefaultInterface.GetGraduationWidth(uWidth);
end;

procedure TAnimationSeekBar.SetStartPath(lType: Integer; const strPath: WideString);
begin
  DefaultInterface.SetStartPath(lType, strPath);
end;

procedure TAnimationSeekBar.GetSoundTagNumCandidate(lFrame: Integer; lNumCandidate: Integer);
begin
  DefaultInterface.GetSoundTagNumCandidate(lFrame, lNumCandidate);
end;

procedure TAnimationSeekBar.LoadSoundCandidate(lIndex: Integer; const strSoundFileName: WideString; 
                                               fHitRate: Single; fVolume: Single);
begin
  DefaultInterface.LoadSoundCandidate(lIndex, strSoundFileName, fHitRate, fVolume);
end;

procedure TAnimationSeekBar.ShowMenu(lX: Integer; lY: Integer; lFrame: Integer);
begin
  DefaultInterface.ShowMenu(lX, lY, lFrame);
end;

procedure Register;
begin
  RegisterComponents(dtlOcxPage, [TAnimationSeekBar]);
end;

end.
