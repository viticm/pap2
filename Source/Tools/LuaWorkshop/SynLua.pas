{$IFNDEF QSYNHIGHLIGHTERLUA}
unit SynLua;
{$ENDIF}

{$I SynEdit.inc}

interface

uses
  SynCompletionProposal,
  {$IFDEF SYN_CLX}
  QGraphics,
  QSynEditTypes,
  QSynEditHighlighter,
  {$ELSE}
  Graphics,
  SynEditTypes,
  SynEditHighlighter,
  {$ENDIF}
  SysUtils,
  Classes;
const
  SYNS_FilterLUA                =  'LUA Files (*.lua)|*.lua';
  SYNS_LangLUA                  =  'Lua Script';

type
  TtkTokenKind = (tkAsm, tkComment, tkDirective, tkIdentifier, tkKey, tkNull,
    tkNumber, tkSpace, tkString, tkSymbol, tkUnknown,
    tkChar, tkFloat, tkHex, tkOctal); // dj

  TxtkTokenKind = (
    xtkAdd, xtkAddAssign, xtkAnd, xtkAndAssign, xtkArrow, xtkAssign,
    xtkBitComplement, xtkBraceClose, xtkBraceOpen, xtkColon, xtkComma,
    xtkDecrement, xtkDivide, xtkDivideAssign, xtkEllipse, xtkGreaterThan,
    xtkGreaterThanEqual, xtkIncOr, xtkIncOrAssign, xtkIncrement, xtkLessThan,
    xtkLessThanEqual, xtkLogAnd, xtkLogComplement, xtkLogEqual, xtkLogOr,
    xtkMod, xtkModAssign, xtkMultiplyAssign, xtkNotEqual, xtkPoint, xtkQuestion,
    xtkRoundClose, xtkRoundOpen, xtkScopeResolution, xtkSemiColon, xtkShiftLeft,
    xtkShiftLeftAssign, xtkShiftRight, xtkShiftRightAssign, xtkSquareClose,
    xtkSquareOpen, xtkStar, xtkSubtract, xtkSubtractAssign, xtkXor,
    xtkXorAssign);

  TRangeState = (rsUnknown, rsAnsiC, rsAnsiCAsm, rsAnsiCAsmBlock, rsAsm,
    rsAsmBlock, rsDirective, rsDirectiveComment, rsString34, rsString39,
    rsMultiLineString, rsMultiLineDirective); //dj

  TProcTableProc = procedure of object;

  PIdentFuncTableFunc = ^TIdentFuncTableFunc;
  TIdentFuncTableFunc = function: TtkTokenKind of object;

  TSynLua = class(TSynCustomHighlighter)
  private
    fAsmStart: Boolean;
    fRange: TRangeState;
    fLine: PChar;
    fProcTable: array[#0..#255] of TProcTableProc;
    Run: LongInt;
    fStringLen: Integer;
    fToIdent: PChar;
    fTokenPos: Integer;
    FTokenID: TtkTokenKind;
    FExtTokenID: TxtkTokenKind;
    fLineNumber: Integer;
    fIdentFuncTable: array[0..206] of TIdentFuncTableFunc;
    fAsmAttri: TSynHighlighterAttributes;
    fCommentAttri: TSynHighlighterAttributes;
    fDirecAttri: TSynHighlighterAttributes;
    fIdentifierAttri: TSynHighlighterAttributes;
    fInvalidAttri: TSynHighlighterAttributes;
    fKeyAttri: TSynHighlighterAttributes;
    fNumberAttri: TSynHighlighterAttributes;
    fFloatAttri: TSynHighlighterAttributes;
    fHexAttri: TSynHighlighterAttributes;
    fOctalAttri: TSynHighlighterAttributes;
    fSpaceAttri: TSynHighlighterAttributes;
    fStringAttri: TSynHighlighterAttributes;
    fCharAttri: TSynHighlighterAttributes;
    fSymbolAttri: TSynHighlighterAttributes;
    function KeyHash(ToHash: PChar): Integer;
    function KeyComp(const aKey: String): Boolean;
    function Func17: TtkTokenKind;
    function Func21: TtkTokenKind;
    function Func22: TtkTokenKind;
    function Func25: TtkTokenKind;
    function Func26: TtkTokenKind;
    function Func35: TtkTokenKind;
    function Func38: TtkTokenKind;
    function Func42: TtkTokenKind;
    function Func45: TtkTokenKind;
    function Func48: TtkTokenKind;
    function Func51: TtkTokenKind;
    function Func52: TtkTokenKind;
    function Func62: TtkTokenKind;
    function Func71: TtkTokenKind;
    function Func81: TtkTokenKind;
    function Func102: TtkTokenKind;
    function Func110: TtkTokenKind;
    procedure AnsiCProc;
    procedure AndSymbolProc;
    procedure AsciiCharProc;
    procedure AtSymbolProc;
    procedure BraceCloseProc;
    procedure BraceOpenProc;
    procedure CRProc;
    procedure ColonProc;
    procedure CommaProc;
    procedure DirectiveProc;
    procedure DirectiveEndProc; //dj
    procedure EqualProc;
    procedure GreaterProc;
    procedure IdentProc;
    procedure LFProc;
    procedure LowerProc;
    procedure MinusProc;
    procedure ModSymbolProc;
    procedure NotSymbolProc;
    procedure NullProc;
    procedure NumberProc;
    procedure OrSymbolProc;
    procedure PlusProc;
    procedure PointProc;
    procedure QuestionProc;
    procedure RoundCloseProc;
    procedure RoundOpenProc;
    procedure SemiColonProc;
    procedure SlashProc;
    procedure SpaceProc;
    procedure SquareCloseProc;
    procedure SquareOpenProc;
    procedure StarProc;
    procedure StringProc;
    procedure TildeProc;
    procedure XOrSymbolProc;
    procedure UnknownProc;
    function AltFunc: TtkTokenKind;
    procedure InitIdent;
    function IdentKind(MayBe: PChar): TtkTokenKind;
    procedure MakeMethodTables;
    procedure StringEndProc;
  protected
    function GetIdentChars: TSynIdentChars; override;
    function GetExtTokenID: TxtkTokenKind;
    function GetSampleSource: string; override;
  public
    class function GetCapabilities: TSynHighlighterCapabilities; override;
    class function GetLanguageName: string; override;
  public
    bCanAdd:boolean;
    SynCompletionProposal:TSynCompletionProposal;
    constructor Create(AOwner: TComponent); override;
    function GetDefaultAttribute(Index: integer): TSynHighlighterAttributes;
      override;
    function GetEol: Boolean; override;
    function GetRange: Pointer; override;
    function GetTokenID: TtkTokenKind;
    procedure SetLine(NewValue: String; LineNumber:Integer); override;
    function GetToken: String; override;
    function GetTokenAttribute: TSynHighlighterAttributes; override;
    function GetTokenKind: integer; override;
    function GetTokenPos: Integer; override;
    procedure Next; override;
    procedure SetRange(Value: Pointer); override;
    procedure ResetRange; override;
    function UseUserSettings(settingIndex: integer): boolean; override;
    procedure EnumUserSettings(settings: TStrings); override;
    property ExtTokenID: TxtkTokenKind read GetExtTokenID;
  published
    property AsmAttri: TSynHighlighterAttributes read fAsmAttri write fAsmAttri;
    property CommentAttri: TSynHighlighterAttributes read fCommentAttri
      write fCommentAttri;
    property DirecAttri: TSynHighlighterAttributes read fDirecAttri
      write fDirecAttri;
    property IdentifierAttri: TSynHighlighterAttributes read fIdentifierAttri
      write fIdentifierAttri;
    property InvalidAttri: TSynHighlighterAttributes read fInvalidAttri
      write fInvalidAttri;
    property KeyAttri: TSynHighlighterAttributes read fKeyAttri write fKeyAttri;
    property NumberAttri: TSynHighlighterAttributes read fNumberAttri
      write fNumberAttri;
    property FloatAttri: TSynHighlighterAttributes read fFloatAttri
      write fFloatAttri;
    property HexAttri: TSynHighlighterAttributes read fHexAttri
      write fHexAttri;
    property OctalAttri: TSynHighlighterAttributes read fOctalAttri
      write fOctalAttri;
    property SpaceAttri: TSynHighlighterAttributes read fSpaceAttri
      write fSpaceAttri;
    property StringAttri: TSynHighlighterAttributes read fStringAttri
      write fStringAttri;
    property CharAttri: TSynHighlighterAttributes read fCharAttri
      write fCharAttri;
    property SymbolAttri: TSynHighlighterAttributes read fSymbolAttri
      write fSymbolAttri;
  end;

implementation

uses
{$IFDEF SYN_CLX}
  QSynEditStrConst;
{$ELSE}
  Windows,
  SynEditStrConst;
{$ENDIF}

var
  Identifiers: array[#0..#255] of ByteBool;
  mHashTable: array[#0..#255] of Integer;

procedure MakeIdentTable;
var
  I: Char;
begin
  for I := #0 to #255 do
  begin
    Case I of
      '_', '0'..'9', 'a'..'z', 'A'..'Z': Identifiers[I] := True;
    else Identifiers[I] := False;
    end;
    Case I in['_', 'a'..'z', 'A'..'Z'] of
      True:
        begin
          if (I > #64) and (I < #91) then mHashTable[I] := Ord(I) - 64 else
            if (I > #96) then mHashTable[I] := Ord(I) - 95;
        end;
    else mHashTable[I] := 0;
    end;
  end;
end;

procedure TSynLua.InitIdent;
var
  I: Integer;
  pF: PIdentFuncTableFunc;
begin
  pF := PIdentFuncTableFunc(@fIdentFuncTable);
  for I := Low(fIdentFuncTable) to High(fIdentFuncTable) do begin
    pF^ := AltFunc;
    Inc(pF);
  end;
  fIdentFuncTable[17] := Func17;
  fIdentFuncTable[21] := Func21;
  fIdentFuncTable[22] := Func22;
  fIdentFuncTable[25] := Func25;
  fIdentFuncTable[26] := Func26;
  fIdentFuncTable[35] := Func35;
  fIdentFuncTable[38] := Func38;
  fIdentFuncTable[42] := Func42;
  fIdentFuncTable[45] := Func45;
  fIdentFuncTable[48] := Func48;
  fIdentFuncTable[51] := Func51;
  fIdentFuncTable[52] := Func52;
  fIdentFuncTable[62] := Func62;
  fIdentFuncTable[71] := Func71;
  fIdentFuncTable[81] := Func81;
  fIdentFuncTable[102] := Func102;
  fIdentFuncTable[110] := Func110;
end;

function TSynLua.KeyHash(ToHash: PChar): Integer;
begin
  Result := 0;
  while ToHash^ in ['_', '0'..'9', 'a'..'z', 'A'..'Z'] do
  begin
    inc(Result, mHashTable[ToHash^]);
    inc(ToHash);
  end;
  fStringLen := ToHash - fToIdent;
end; { KeyHash }

function TSynLua.KeyComp(const aKey: String): Boolean;
var
  I: Integer;
  Temp: PChar;
begin
  Temp := fToIdent;
  if Length(aKey) = fStringLen then
  begin
    Result := True;
    for i := 1 to fStringLen do
    begin
      if Temp^ <> aKey[i] then
      begin
        Result := False;
        break;
      end;
      inc(Temp);
    end;
  end else Result := False;
end; { KeyComp }

function TSynLua.Func17: TtkTokenKind;
begin
  if KeyComp('if') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func21: TtkTokenKind;
begin
  if KeyComp('do') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func22: TtkTokenKind;
begin
  if KeyComp('and') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func25: TtkTokenKind;
begin
  if KeyComp('in') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func26: TtkTokenKind;
begin
  if KeyComp('end') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func35: TtkTokenKind;
begin
  if KeyComp('or') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func38: TtkTokenKind;
begin
  if KeyComp('nil') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func42: TtkTokenKind;
begin
  if KeyComp('for') then Result := tkKey else
    if KeyComp('break') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func45: TtkTokenKind;
begin
  if KeyComp('else') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func48: TtkTokenKind;
begin
  if KeyComp('local') then
    begin
      Result := tkKey;
    end
   else Result := tkIdentifier;
end;

function TSynLua.Func51: TtkTokenKind;
begin
  if KeyComp('then') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func52: TtkTokenKind;
begin
  if KeyComp('not') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func62: TtkTokenKind;
begin
    if KeyComp('while') then Result := tkKey else
      if KeyComp('elseif') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func71: TtkTokenKind;
begin
  if KeyComp('repeat') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func81: TtkTokenKind;
begin
  if KeyComp('until') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func102: TtkTokenKind;
begin
  if KeyComp('return') then Result := tkKey else Result := tkIdentifier;
end;

function TSynLua.Func110: TtkTokenKind;
begin
  if KeyComp('function') then
    begin
      Result := tkKey
    end
   else Result := tkIdentifier;
end;

function TSynLua.AltFunc: TtkTokenKind;
begin
  Result := tkIdentifier;
end;

function TSynLua.IdentKind(MayBe: PChar): TtkTokenKind;
var
  HashKey: Integer;
begin
  fToIdent := MayBe;
  HashKey := KeyHash(MayBe);
  if HashKey < 207 then Result := fIdentFuncTable[HashKey] else Result := tkIdentifier;
end;

procedure TSynLua.MakeMethodTables;
var
  I: Char;
begin
  for I := #0 to #255 do
    case I of
      '&': fProcTable[I] := AndSymbolProc;
      #39: fProcTable[I] := AsciiCharProc;
      '@': fProcTable[I] := AtSymbolProc;
      '}': fProcTable[I] := BraceCloseProc;
      '{': fProcTable[I] := BraceOpenProc;
      #13: fProcTable[I] := CRProc;
      ':': fProcTable[I] := ColonProc;
      ',': fProcTable[I] := CommaProc;
      '#': fProcTable[I] := DirectiveProc;
      '=': fProcTable[I] := EqualProc;
      '>': fProcTable[I] := GreaterProc;
      '?': fProcTable[I] := QuestionProc;
      'A'..'Z', 'a'..'z', '_': fProcTable[I] := IdentProc;
      #10: fProcTable[I] := LFProc;
      '<': fProcTable[I] := LowerProc;
      '-': fProcTable[I] := MinusProc;
      '%': fProcTable[I] := ModSymbolProc;
      '!': fProcTable[I] := NotSymbolProc;
      #0: fProcTable[I] := NullProc;
      '0'..'9': fProcTable[I] := NumberProc;
      '|': fProcTable[I] := OrSymbolProc;
      '+': fProcTable[I] := PlusProc;
      '.': fProcTable[I] := PointProc;
      ')': fProcTable[I] := RoundCloseProc;
      '(': fProcTable[I] := RoundOpenProc;
      ';': fProcTable[I] := SemiColonProc;
      '/': fProcTable[I] := SlashProc;
      #1..#9, #11, #12, #14..#32: fProcTable[I] := SpaceProc;
      ']': fProcTable[I] := SquareCloseProc;
      '[': fProcTable[I] := SquareOpenProc;
      '*': fProcTable[I] := StarProc;
      #34: fProcTable[I] := StringProc;
      '~': fProcTable[I] := TildeProc;
      '^': fProcTable[I] := XOrSymbolProc;
      else fProcTable[I] := UnknownProc;
    end;
end;

constructor TSynLua.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  fAsmAttri := TSynHighlighterAttributes.Create(SYNS_AttrAssembler);
  AddAttribute(fAsmAttri);
  fCommentAttri := TSynHighlighterAttributes.Create(SYNS_AttrComment);
  fCommentAttri.Foreground:=clGray;
  fCommentAttri.Style:= [fsItalic];
  AddAttribute(fCommentAttri);
  fIdentifierAttri := TSynHighlighterAttributes.Create(SYNS_AttrIdentifier);
  fIdentifierAttri.Foreground:=clMaroon;
  AddAttribute(fIdentifierAttri);
  fInvalidAttri := TSynHighlighterAttributes.Create(SYNS_AttrIllegalChar);
  AddAttribute(fInvalidAttri);
  fKeyAttri := TSynHighlighterAttributes.Create(SYNS_AttrReservedWord);
  fKeyAttri.Foreground:=clBlue;
  fKeyAttri.Style:= [fsBold];
  AddAttribute(fKeyAttri);
  fNumberAttri := TSynHighlighterAttributes.Create(SYNS_AttrNumber);
  fNumberAttri.Foreground:=clred;
  AddAttribute(fNumberAttri);
  fCharAttri := TSynHighlighterAttributes.Create(SYNS_AttrCharacter);
  AddAttribute(fCharAttri);
  fFloatAttri := TSynHighlighterAttributes.Create(SYNS_AttrFloat);
  AddAttribute(fFloatAttri);
  fHexAttri := TSynHighlighterAttributes.Create(SYNS_AttrHexadecimal);
  AddAttribute(fHexAttri);
  fOctalAttri := TSynHighlighterAttributes.Create(SYNS_AttrOctal);
  AddAttribute(fOctalAttri);
  fSpaceAttri := TSynHighlighterAttributes.Create(SYNS_AttrSpace);
  fSpaceAttri.Foreground := clWindow;
  AddAttribute(fSpaceAttri);
  fStringAttri := TSynHighlighterAttributes.Create(SYNS_AttrString);
  fStringAttri.Foreground:=clgreen;
  AddAttribute(fStringAttri);
  fSymbolAttri := TSynHighlighterAttributes.Create(SYNS_AttrSymbol);
  AddAttribute(fSymbolAttri);
  fDirecAttri := TSynHighlighterAttributes.Create(SYNS_AttrPreprocessor);
  AddAttribute(fDirecAttri);
  SetAttributesOnChange(DefHighlightChange);
  InitIdent;
  MakeMethodTables;
  fRange := rsUnknown;
  fAsmStart := False;
  fDefaultFilter := SYNS_FilterLUA;
end; { Create }

procedure TSynLua.SetLine(NewValue: String; LineNumber:Integer);
begin
  fLine := PChar(NewValue);
  Run := 0;
  fLineNumber := LineNumber;
  Next;
end; { SetLine }

procedure TSynLua.AnsiCProc;
begin
  fTokenID := tkComment;
  case FLine[Run] of
    #0:
      begin
        NullProc;
        exit;
      end;
    #10:
      begin
        LFProc;
        exit;
      end;
    #13:
      begin
        CRProc;
        exit;
      end;
  end;

  while FLine[Run] <> #0 do
    case FLine[Run] of
      '*':
        if fLine[Run + 1] = '/' then
        begin
          inc(Run, 2);
          if fRange = rsAnsiCAsm then
            fRange := rsAsm
          else if fRange = rsAnsiCAsmBlock then
            fRange := rsAsmBlock
          else if (fRange = rsDirectiveComment) and
            not (fLine[Run] in [#0, #13, #10]) then
              fRange := rsMultiLineDirective //dj
          else
            fRange := rsUnKnown;
          break;
        end else
          inc(Run);
      #10: break;
      #13: break;
    else inc(Run);
    end;
end;

procedure TSynLua.AndSymbolProc;
begin
  fTokenID := tkSymbol;
  case FLine[Run + 1] of
    '=':                               {and assign}
      begin
        inc(Run, 2);
        FExtTokenID := xtkAndAssign;
      end;
    '&':                               {logical and}
      begin
        inc(Run, 2);
        FExtTokenID := xtkLogAnd;
      end;
  else                                 {and}
    begin
      inc(Run);
      FExtTokenID := xtkAnd;
    end;
  end;
end;

procedure TSynLua.AsciiCharProc;
begin
  fTokenID := tkChar;
  repeat
    if fLine[Run] = '\' then begin
      if fLine[Run + 1] in [#39, '\'] then                                      //ek 2000-04-26
        inc(Run);
    end;
    inc(Run);
  until fLine[Run] in [#0, #10, #13, #39];
  if fLine[Run] = #39 then
    inc(Run);
end;

procedure TSynLua.AtSymbolProc;
begin
  fTokenID := tkUnknown;
  inc(Run);
end;

procedure TSynLua.BraceCloseProc;
begin
  inc(Run);
  fTokenId := tkSymbol;
  FExtTokenID := xtkBraceClose;
  if fRange = rsAsmBlock then fRange := rsUnknown;
end;

procedure TSynLua.BraceOpenProc;
begin
  inc(Run);
  fTokenId := tkSymbol;
  FExtTokenID := xtkBraceOpen;
  if fRange = rsAsm then
  begin
    fRange := rsAsmBlock;
    fAsmStart := True;
  end;
end;

procedure TSynLua.CRProc;
begin
  fTokenID := tkSpace;
  Inc(Run);
  if fLine[Run + 1] = #10 then Inc(Run);
end;

procedure TSynLua.ColonProc;
begin
  fTokenID := tkSymbol;
  Case FLine[Run + 1] of
    ':':                               {scope resolution operator}
      begin
        inc(Run, 2);
        FExtTokenID := xtkScopeResolution;
      end;
  else                                 {colon}
    begin
      inc(Run);
      FExtTokenID := xtkColon;
    end;
  end;
end;

procedure TSynLua.CommaProc;
begin
  inc(Run);
  fTokenID := tkSymbol;
  FExtTokenID := xtkComma;
end;

procedure TSynLua.DirectiveProc; // dj, rewritten to support multiline directives properly
begin
  if Trim(fLine)[1] <> '#' then // '#' is not first char on the line, treat it as an invalid char
  begin
    fTokenID := tkUnknown;
    Inc(Run);
    Exit;
  end;
  fTokenID := tkDirective;
  repeat
    if fLine[Run] = '/' then // comment?
    begin
      if fLine[Run + 1] = '/' then // is end of directive as well
      begin
        fRange := rsUnknown;                                              //ek 2000-04-25
        Exit;
      end
      else
        if fLine[Run + 1] = '*' then // might be embedded only
        begin
          fRange := rsDirectiveComment;
          Exit;
        end;
    end;
    if (fLine[Run] = '\') and (fLine[Run +1 ] = #0) then // a multiline directive
    begin
      Inc(Run);
      fRange := rsMultiLineDirective;
      Exit;
    end;
    Inc(Run);
  until fLine[Run] in [#0, #10, #13];
end;

procedure TSynLua.DirectiveEndProc; // dj, added to support multiline directives properly
begin
  fTokenID := tkDirective;
  case FLine[Run] of
    #0:
      begin
        NullProc;
        Exit;
      end;
    #10:
      begin
        LFProc;
        Exit;
      end;
    #13:
      begin
        CRProc;
        Exit;
      end;
  end;
  fRange := rsUnknown;
  repeat
    case FLine[Run] of
      #0, #10, #13: Break;
      '/': // comment?
        begin
          case fLine[Run + 1] of
            '/': // is end of directive as well
              begin
                fRange := rsUnknown;                                              //ek 2000-04-25
                Exit;
              end;
            '*': // might be embedded only
              begin
                fRange := rsDirectiveComment;
                Exit;
              end;
          end;
        end;
      '\': // yet another line?
        begin
          if fLine[Run + 1] = #0 then
          begin
            Inc(Run);
            fRange := rsMultiLineDirective;
            Exit;
          end;
        end;
    end;
    Inc(Run);
  until fLine[Run] in [#0, #10, #13];
end;

procedure TSynLua.EqualProc;
begin
  fTokenID := tkSymbol;
  case FLine[Run + 1] of
    '=':                               {logical equal}
      begin
        inc(Run, 2);
        FExtTokenID := xtkLogEqual;
      end;
  else                                 {assign}
    begin
      inc(Run);
      FExtTokenID := xtkAssign;
    end;
  end;
end;

procedure TSynLua.GreaterProc;
begin
  fTokenID := tkSymbol;
  Case FLine[Run + 1] of
    '=':                               {greater than or equal to}
      begin
        inc(Run, 2);
        FExtTokenID := xtkGreaterThanEqual;
      end;
    '>':
      begin
        if FLine[Run + 2] = '=' then   {shift right assign}
        begin
          inc(Run, 3);
          FExtTokenID := xtkShiftRightAssign;
        end
        else                           {shift right}
        begin
          inc(Run, 2);
          FExtTokenID := xtkShiftRight;
        end;
      end;
  else                                 {greater than}
    begin
      inc(Run);
      FExtTokenID := xtkGreaterThan;
    end;
  end;
end;

procedure TSynLua.QuestionProc;
begin
  fTokenID := tkSymbol;                {conditional}
  FExtTokenID := xtkQuestion;
  inc(Run);
end;

function GetIdent(s:string; var leftstr:string):string;
var
  Head, Len:integer;
begin
  Result:='';
  leftstr:=s;
  if (s = '') then
    exit;
  Head:=1;
  while (s[Head] in [' ', #9, '(', ')', ',', '.', '0'..'9']) and
    (Head <= Length(s)) do
    inc(Head);
  Len:=0;
  while (s[Head + Len] in ['a'..'z', 'A'..'Z', '0'..'9', '_']) and
    ((Head + Len) <= Length(s)) do
    inc(Len);
  if (Len > 0) then
    begin
      Result:=Copy(s, Head, Len);
      leftstr:=Copy(s, Head + Len, 9999);
    end;
end;

procedure TSynLua.IdentProc;
var
  CurLine:string;
  Name:string;
begin
  fTokenID := IdentKind((fLine + Run));
  inc(Run, fStringLen);
  if (fTokenID = tkIdentifier) and (bCanAdd) then
    begin
      CurLine:=Copy(fLine, Run - fStringLen + 1, fStringLen);
      Name:=GetIdent(CurLine, CurLine);
      if (Name<>'') then
        begin
          if SynCompletionProposal.ItemList.IndexOf(Name) = -1 then
            begin
              SynCompletionProposal.AddItem(Name, Name);
            end;
        end;

    end;
{  while Identifiers[fLine[Run]] do inc(Run);
  if (Assigned(SynCompletionProposal) and bCanAdd)then
    begin
      CurLine:=fLine;
      repeat
        Name:=GetIdent(CurLine, CurLine);
        if (Name<>'') then
          begin
            if SynCompletionProposal.ItemList.IndexOf(Name) = -1 then
              begin
                SynCompletionProposal.AddItem(Name, Name);
              end;
          end;
      until Name = '';
    end;
}
end;

procedure TSynLua.LFProc;
begin
  fTokenID := tkSpace;
  inc(Run);
end;

procedure TSynLua.LowerProc;
begin
  fTokenID := tkSymbol;
  case FLine[Run + 1] of
    '=':                               {less than or equal to}
      begin
        inc(Run, 2);
        FExtTokenID := xtkLessThanEqual;
      end;
    '<':
      begin
        if FLine[Run + 2] = '=' then   {shift left assign}
        begin
          inc(Run, 3);
          FExtTokenID := xtkShiftLeftAssign;
        end
        else                           {shift left}
        begin
          inc(Run, 2);
          FExtTokenID := xtkShiftLeft;
        end;
      end;
  else                                 {less than}
    begin
      inc(Run);
      FExtTokenID := xtkLessThan;
    end;
  end;
end;

procedure TSynLua.MinusProc;
begin
  fTokenID := tkSymbol;
  case FLine[Run + 1] of
    '=':                               {subtract assign}
      begin
        inc(Run, 2);
        FExtTokenID := xtkSubtractAssign;
      end;
    '-':                               {decrement}
      begin
        fTokenID := tkComment;
        inc(Run, 2);
        while not (fLine[Run] in [#0, #10, #13]) do Inc(Run);
      end;
    '>':                               {arrow}
      begin
        inc(Run, 2);
        FExtTokenID := xtkArrow;
      end;
  else                                 {subtract}
    begin
      inc(Run);
      FExtTokenID := xtkSubtract;
    end;
  end;
end;

procedure TSynLua.ModSymbolProc;
begin
  fTokenID := tkSymbol;
  case FLine[Run + 1] of
    '=':                               {mod assign}
      begin
        inc(Run, 2);
        FExtTokenID := xtkModAssign;
      end;
  else                                 {mod}
    begin
      inc(Run);
      FExtTokenID := xtkMod;
    end;
  end;
end;

procedure TSynLua.NotSymbolProc;
begin
  fTokenID := tkSymbol;
  case FLine[Run + 1] of
    '=':                               {not equal}
      begin
        inc(Run, 2);
        FExtTokenID := xtkNotEqual;
      end;
  else                                 {not}
    begin
      inc(Run);
      FExtTokenID := xtkLogComplement;
    end;
  end;
end;

procedure TSynLua.NullProc;
begin
  fTokenID := tkNull;
end;

procedure TSynLua.NumberProc;
var
  idx1: Integer; // token[1]
  i: Integer;
begin
  idx1 := Run;
  Inc(Run);
  fTokenID := tkNumber;
  while FLine[Run] in
    ['0'..'9', 'A'..'F', 'a'..'f', '.', 'u', 'U', 'l', 'L', 'x', 'X', '-', '+'] do
  begin
    case FLine[Run] of
      '.':
        if FLine[Succ(Run)] = '.' then
          Break
        else
          if (fTokenID <> tkHex) then
            fTokenID := tkFloat
          else // invalid
          begin
            fTokenID := tkUnknown;
            Exit;
          end;
      '-', '+':
        begin
          if fTokenID <> tkFloat then // number <> float. an arithmetic operator
            Exit;
          if not (FLine[Pred(Run)] in ['e', 'E']) then
            Exit; // number = float, but no exponent. an arithmetic operator
          if not (FLine[Succ(Run)] in ['0'..'9', '+', '-']) then // invalid
          begin
            Inc(Run);
            fTokenID := tkUnknown;
            Exit;
          end
        end;
      '0'..'7':
        if (Run = Succ(idx1)) and (FLine[idx1] = '0') then // octal number
          fTokenID := tkOctal;
      '8', '9':
        if (FLine[idx1] = '0') and
           ((fTokenID <> tkHex) and (fTokenID <> tkFloat)) then // invalid octal char
             fTokenID := tkUnknown;
      'a'..'d', 'A'..'D':
        if fTokenID <> tkHex then // invalid char
          Break;
      'e', 'E':
        if (fTokenID <> tkHex) then
          if FLine[Pred(Run)] in ['0'..'9'] then // exponent
          begin
            for i := idx1 to Pred(Run) do
              if FLine[i] in ['e', 'E'] then // too many exponents
              begin
                //Run := i;
                fTokenID := tkUnknown;
                Exit;
              end;
            if not (FLine[Succ(Run)] in ['0'..'9', '+', '-']) then
              Break
            else
              fTokenID := tkFloat
          end
          else // invalid char
            Break;
      'f', 'F':
        if fTokenID <> tkHex then
        begin
          for i := idx1 to Pred(Run) do
            if FLine[i] in ['f', 'F'] then // declaration syntax error
            begin
              fTokenID := tkUnknown;
              Exit;
            end;
          if fTokenID = tkFloat then
          begin
            if fLine[Pred(Run)] in ['l', 'L'] then // can't mix
              Break;
          end
          else
            fTokenID := tkFloat;
        end;
      'l', 'L':
        begin
          for i := idx1 to Pred(Run) do
            if FLine[i] in ['l', 'L'] then // declaration syntax error
            begin
              fTokenID := tkUnknown;
              Exit;
            end;
          if fTokenID = tkFloat then
            if fLine[Pred(Run)] in ['f', 'F'] then // can't mix
              Break;
        end;
      'u', 'U':
        if fTokenID = tkFloat then // not allowed
          Break
        else
          for i := idx1 to Pred(Run) do
            if FLine[i] in ['u', 'U'] then // declaration syntax error
            begin
              fTokenID := tkUnknown;
              Exit;
            end;
      'x', 'X':
        if (Run = Succ(idx1)) and   // 0x... 'x' must be second char
           (FLine[idx1] = '0') and  // 0x...
           (FLine[Succ(Run)] in ['0'..'9', 'a'..'f', 'A'..'F']) then // 0x... must be continued with a number
             fTokenID := tkHex
           else // invalid char
           begin
             if (not Identifiers[fLine[Succ(Run)]]) and
                (FLine[Succ(idx1)] in ['x', 'X']) then
             begin
               Inc(Run); // highlight 'x' too
               fTokenID := tkUnknown;
             end;
             Break;
           end;
    end; // case
    Inc(Run);
  end; // while
  if FLine[Run] in ['A'..'Z', 'a'..'z', '_'] then
    fTokenID := tkUnknown;
end;

procedure TSynLua.OrSymbolProc;
begin
  fTokenID := tkSymbol;
  case FLine[Run + 1] of
    '=':                               {or assign}
      begin
        inc(Run, 2);
        FExtTokenID := xtkIncOrAssign;
      end;
    '|':                               {logical or}
      begin
        inc(Run, 2);
        FExtTokenID := xtkLogOr;
      end;
  else                                 {or}
    begin
      inc(Run);
      FExtTokenID := xtkIncOr;
    end;
  end;
end;

procedure TSynLua.PlusProc;
begin
  fTokenID := tkSymbol;
  case FLine[Run + 1] of
    '=':                               {add assign}
      begin
        inc(Run, 2);
        FExtTokenID := xtkAddAssign;
      end;
    '+':                               {increment}
      begin
        inc(Run, 2);
        FExtTokenID := xtkIncrement;
      end;
  else                                 {add}
    begin
      inc(Run);
      FExtTokenID := xtkAdd;
    end;
  end;
end;

procedure TSynLua.PointProc;
begin
  fTokenID := tkSymbol;
  if (FLine[Run + 1] = '.') and (FLine[Run + 2] = '.') then
    begin                              {ellipse}
      inc(Run, 3);
      FExtTokenID := xtkEllipse;
    end
  else
    if FLine[Run + 1] in ['0'..'9'] then // float
    begin
      Dec(Run); // numberproc must see the point
      NumberProc;
    end
  else                                 {point}
    begin
      inc(Run);
      FExtTokenID := xtkPoint;
    end;
end;

procedure TSynLua.RoundCloseProc;
begin
  inc(Run);
  fTokenID := tkSymbol;
  FExtTokenID := xtkRoundClose;
end;

procedure TSynLua.RoundOpenProc;
begin
  inc(Run);
  FTokenID := tkSymbol;
  FExtTokenID := xtkRoundOpen;
end;

procedure TSynLua.SemiColonProc;
begin
  inc(Run);
  fTokenID := tkSymbol;
  FExtTokenID := xtkSemiColon;
  if fRange = rsAsm then fRange := rsUnknown;
end;

procedure TSynLua.SlashProc;
begin
  case FLine[Run + 1] of
    '/':                               {c++ style comments}
      begin
        fTokenID := tkComment;
        inc(Run, 2);
        while not (fLine[Run] in [#0, #10, #13]) do Inc(Run);
      end;
    '*':                               {c style comments}
      begin
        fTokenID := tkComment;
        if fRange = rsAsm then
          fRange := rsAnsiCAsm
        else if fRange = rsAsmBlock then
          fRange := rsAnsiCAsmBlock
        else if fRange <> rsDirectiveComment then                          
          fRange := rsAnsiC;
        inc(Run, 2);
        while fLine[Run] <> #0 do
          case fLine[Run] of
            '*':
              if fLine[Run + 1] = '/' then
              begin
                inc(Run, 2);
                if fRange = rsDirectiveComment then
                  fRange := rsMultiLineDirective //dj
                else if fRange = rsAnsiCAsm then
                  fRange := rsAsm
                else
                  begin
                  if fRange = rsAnsiCAsmBlock then
                    fRange := rsAsmBlock
                  else
                    fRange := rsUnKnown;
                  end;
                break;
              end else inc(Run);
            #10, #13:
              begin
                if fRange = rsDirectiveComment then
                  fRange := rsAnsiC;
                break;
              end;
          else inc(Run);
          end;
      end;
    '=':                               {divide assign}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
        FExtTokenID := xtkDivideAssign;
      end;
  else                                 {divide}
    begin
      inc(Run);
      fTokenID := tkSymbol;
      FExtTokenID := xtkDivide;
    end;
  end;
end;

procedure TSynLua.SpaceProc;
begin
  inc(Run);
  fTokenID := tkSpace;
  while FLine[Run] in [#1..#9, #11, #12, #14..#32] do inc(Run);
end;

procedure TSynLua.SquareCloseProc;
begin
  inc(Run);
  fTokenID := tkSymbol;
  FExtTokenID := xtkSquareClose;
end;

procedure TSynLua.SquareOpenProc;
begin
  inc(Run);
  fTokenID := tkSymbol;
  FExtTokenID := xtkSquareOpen;
end;

procedure TSynLua.StarProc;
begin
  fTokenID := tkSymbol;
  case FLine[Run + 1] of
    '=':                               {multiply assign}
      begin
        inc(Run, 2);
        FExtTokenID := xtkMultiplyAssign;
      end;
  else                                 {star}
    begin
      inc(Run);
      FExtTokenID := xtkStar;
    end;
  end;
end;

procedure TSynLua.StringProc;
begin
  fTokenID := tkString;
  repeat
    if fLine[Run] = '\' then begin
      case fLine[Run + 1] of
        #34, '\':
          Inc(Run);
        #00:
          begin
            Inc(Run);
            fRange := rsMultilineString;
            Exit;
          end;
      end;
    end;
    inc(Run);
  until fLine[Run] in [#0, #10, #13, #34];
  if FLine[Run] = #34 then
    inc(Run);
end;

procedure TSynLua.StringEndProc;
begin
  fTokenID := tkString;

  case FLine[Run] of
    #0:
      begin
        NullProc;
        Exit;
      end;
    #10:
      begin
        LFProc;
        Exit;
      end;
    #13:
      begin
        CRProc;
        Exit;
      end;
  end;

  fRange := rsUnknown;

  repeat
    case FLine[Run] of
      #0, #10, #13: Break;
      '\':
        begin
          case fLine[Run + 1] of
            #34, '\':
              Inc(Run);
            #00:
              begin
                Inc(Run);
                fRange := rsMultilineString;
                Exit;
              end;
          end;
        end;
      #34: Break;
    end;
    inc(Run);
  until fLine[Run] in [#0, #10, #13, #34];
  if FLine[Run] = #34 then
    inc(Run);
end;

procedure TSynLua.TildeProc;
begin
  inc(Run);                            {bitwise complement}
  fTokenId := tkSymbol;
  FExtTokenID := xtkBitComplement;
end;

procedure TSynLua.XOrSymbolProc;
begin
  fTokenID := tkSymbol;
  Case FLine[Run + 1] of
  	'=':                               {xor assign}
      begin
        inc(Run, 2);
        FExtTokenID := xtkXorAssign;
      end;
  else                                 {xor}
    begin
      inc(Run);
      FExtTokenID := xtkXor;
    end;
  end;
end;

procedure TSynLua.UnknownProc;
begin
{$IFDEF SYN_MBCSSUPPORT}
  if FLine[Run] in LeadBytes then // if FLine[Run] is the leadbyte of MBCS char,then jump 2 chars.
    Inc(Run,2)
  else
{$ENDIF}
  Inc(Run);
  fTokenID := tkUnknown;
end;

procedure TSynLua.Next;
begin
  fAsmStart := False;
  fTokenPos := Run;
  case fRange of
    rsAnsiC, rsAnsiCAsm,
    rsAnsiCAsmBlock, rsDirectiveComment: AnsiCProc;
    rsMultiLineDirective: DirectiveEndProc; // dj
    rsMultilineString: StringEndProc;                                           //ek 2001-08-02
  else
    begin
      fRange := rsUnknown;
      fProcTable[fLine[Run]];
    end;
  end;
end;

function TSynLua.GetDefaultAttribute(Index: integer): TSynHighlighterAttributes;
begin
  case Index of
    SYN_ATTR_COMMENT: Result := fCommentAttri;
    SYN_ATTR_IDENTIFIER: Result := fIdentifierAttri;
    SYN_ATTR_KEYWORD: Result := fKeyAttri;
    SYN_ATTR_STRING: Result := fStringAttri;
    SYN_ATTR_WHITESPACE: Result := fSpaceAttri;
    SYN_ATTR_SYMBOL: Result := fSymbolAttri;
  else
    Result := nil;
  end;
end;

function TSynLua.GetEol: Boolean;
begin
  Result := fTokenID = tkNull;
end;

function TSynLua.GetRange: Pointer;
begin
  Result := Pointer(fRange);
end;

function TSynLua.GetToken: String;
var
  Len: LongInt;
begin
  Len := Run - fTokenPos;
  SetString(Result, (FLine + fTokenPos), Len);
end;

function TSynLua.GetTokenID: TtkTokenKind;
begin
  Result := fTokenId;
  if ((fRange = rsAsm) or (fRange = rsAsmBlock)) and not fAsmStart
    and not (fTokenId in [tkComment, tkSpace, tkNull])
  then
    Result := tkAsm;
end;

function TSynLua.GetExtTokenID: TxtkTokenKind;
begin
  Result := FExtTokenID;
end;

function TSynLua.GetTokenAttribute: TSynHighlighterAttributes;
begin
  case fTokenID of
    tkAsm: Result := fAsmAttri;
    tkComment: Result := fCommentAttri;
    tkDirective: Result := fDirecAttri;
    tkIdentifier: Result := fIdentifierAttri;
    tkKey: Result := fKeyAttri;
    tkNumber: Result := fNumberAttri;
    tkFloat: Result := fFloatAttri;
    tkHex: Result := fHexAttri;
    tkOctal: Result := fOctalAttri;
    tkSpace: Result := fSpaceAttri;
    tkString: Result := fStringAttri;
    tkChar: Result := fCharAttri;
    tkSymbol: Result := fSymbolAttri;
    tkUnknown: Result := fInvalidAttri;
    else Result := nil;
  end;
end;

function TSynLua.GetTokenKind: integer;
begin
  Result := Ord(GetTokenID);
end;

function TSynLua.GetTokenPos: Integer;
begin
  Result := fTokenPos;
end;

procedure TSynLua.ReSetRange;
begin
  fRange:= rsUnknown;
end;

procedure TSynLua.SetRange(Value: Pointer);
begin
  fRange := TRangeState(Value);
end;

procedure TSynLua.EnumUserSettings(settings: TStrings);
begin
  { returns the user settings that exist in the registry }
{$IFNDEF SYN_CLX}
  with TBetterRegistry.Create do
  begin
    try
      RootKey := HKEY_LOCAL_MACHINE;
      if OpenKeyReadOnly('\SOFTWARE\Borland\C++Builder') then
      begin
        try
          GetKeyNames(settings);
        finally
          CloseKey;
        end;
      end;
    finally
      Free;
    end;
  end;
{$ENDIF}
end;

function TSynLua.UseUserSettings(settingIndex: integer): boolean;
// Possible parameter values:
//   index into TStrings returned by EnumUserSettings
// Possible return values:
//   true : settings were read and used
//   false: problem reading settings or invalid version specified - old settings
//          were preserved

  {$IFNDEF SYN_CLX}
  function ReadLUABSettings(settingIndex: integer): boolean;

    function ReadLUABSetting(settingTag: string; attri: TSynHighlighterAttributes; key: string): boolean;

      function ReadLUAB1(settingTag: string; attri: TSynHighlighterAttributes; name: string): boolean;
      var
        i: integer;
      begin
        for i := 1 to Length(name) do
          if name[i] = ' ' then name[i] := '_';
        Result := attri.LoadFromBorlandRegistry(HKEY_CURRENT_USER,
             '\SOFTWARE\Borland\C++Builder\'+settingTag+'\Highlight',name,true);
      end; { ReadLUAB1 }

      function ReadLUAB3OrMore(settingTag: string; attri: TSynHighlighterAttributes; key: string): boolean;
      begin
        Result := attri.LoadFromBorlandRegistry(HKEY_CURRENT_USER,
                 '\Software\Borland\C++Builder\'+settingTag+'\Editor\Highlight',
                 key,false);
      end; { ReadLUAB3OrMore }

    begin { ReadLUABSetting }
      try
        if (settingTag[1] = '1')
          then Result := ReadLUAB1(settingTag,attri,key)
          else Result := ReadLUAB3OrMore(settingTag,attri,key);
      except Result := false; end;
    end; { ReadLUABSetting }

  var
    tmpStringAttri    : TSynHighlighterAttributes;
    tmpCharAttri      : TSynHighlighterAttributes;
    tmpNumberAttri    : TSynHighlighterAttributes;
    tmpFloatAttri     : TSynHighlighterAttributes;
    tmpHexAttri       : TSynHighlighterAttributes;
    tmpOctalAttri     : TSynHighlighterAttributes;
    tmpKeyAttri       : TSynHighlighterAttributes;
    tmpSymbolAttri    : TSynHighlighterAttributes;
    tmpAsmAttri       : TSynHighlighterAttributes;
    tmpCommentAttri   : TSynHighlighterAttributes;
    tmpIdentifierAttri: TSynHighlighterAttributes;
    tmpInvalidAttri   : TSynHighlighterAttributes;
    tmpSpaceAttri     : TSynHighlighterAttributes;
    tmpDirecAttri     : TSynHighlighterAttributes;
    s                 : TStringList;

  begin { ReadLUABSettings }
    s := TStringList.Create;
    try
      EnumUserSettings(s);
      if settingIndex >= s.Count then Result := false
      else begin
        tmpStringAttri    := TSynHighlighterAttributes.Create('');
        tmpCharAttri      := TSynHighlighterAttributes.Create('');
        tmpNumberAttri    := TSynHighlighterAttributes.Create('');
        tmpFloatAttri     := TSynHighlighterAttributes.Create('');
        tmpHexAttri       := TSynHighlighterAttributes.Create('');
        tmpOctalAttri     := TSynHighlighterAttributes.Create('');
        tmpKeyAttri       := TSynHighlighterAttributes.Create('');
        tmpSymbolAttri    := TSynHighlighterAttributes.Create('');
        tmpAsmAttri       := TSynHighlighterAttributes.Create('');
        tmpCommentAttri   := TSynHighlighterAttributes.Create('');
        tmpIdentifierAttri:= TSynHighlighterAttributes.Create('');
        tmpInvalidAttri   := TSynHighlighterAttributes.Create('');
        tmpSpaceAttri     := TSynHighlighterAttributes.Create('');
        tmpDirecAttri     := TSynHighlighterAttributes.Create('');
        tmpStringAttri    .Assign(fStringAttri);
        tmpCharAttri      .Assign(fCharAttri);
        tmpNumberAttri    .Assign(fNumberAttri);
        tmpFloatAttri     .Assign(fFloatAttri);
        tmpHexAttri       .Assign(fHexAttri);
        tmpOctalAttri     .Assign(fOctalAttri);
        tmpKeyAttri       .Assign(fKeyAttri);
        tmpSymbolAttri    .Assign(fSymbolAttri);
        tmpAsmAttri       .Assign(fAsmAttri);
        tmpCommentAttri   .Assign(fCommentAttri);
        tmpIdentifierAttri.Assign(fIdentifierAttri);
        tmpInvalidAttri   .Assign(fInvalidAttri);
        tmpSpaceAttri     .Assign(fSpaceAttri);
        tmpDirecAttri     .Assign(fDirecAttri);
        if s[settingIndex][1] = '1'
          then Result := ReadLUABSetting(s[settingIndex],fAsmAttri,'Plain text')
          else Result := ReadLUABSetting(s[settingIndex],fAsmAttri,'Assembler');
        Result := Result                                                         and
                  ReadLUABSetting(s[settingIndex],fCommentAttri,'Comment')       and
                  ReadLUABSetting(s[settingIndex],fIdentifierAttri,'Identifier') and
                  ReadLUABSetting(s[settingIndex],fInvalidAttri,'Illegal Char')  and
                  ReadLUABSetting(s[settingIndex],fKeyAttri,'Reserved word')     and
                  ReadLUABSetting(s[settingIndex],fNumberAttri,'Integer')        and
                  ReadLUABSetting(s[settingIndex],fFloatAttri,'Float')           and
                  ReadLUABSetting(s[settingIndex],fHexAttri,'Hex')               and
                  ReadLUABSetting(s[settingIndex],fOctalAttri,'Octal')           and
                  ReadLUABSetting(s[settingIndex],fSpaceAttri,'Whitespace')      and
                  ReadLUABSetting(s[settingIndex],fStringAttri,'String')         and
                  ReadLUABSetting(s[settingIndex],fCharAttri,'Character')             and
                  ReadLUABSetting(s[settingIndex],fSymbolAttri,'Symbol')         and
                  ReadLUABSetting(s[settingIndex],fDirecAttri,'Preprocessor');
        if not Result then begin
          fStringAttri    .Assign(tmpStringAttri);
          fCharAttri      .Assign(tmpCharAttri);
          fNumberAttri    .Assign(tmpNumberAttri);
          fFloatAttri     .Assign(tmpFloatAttri);
          fHexAttri       .Assign(tmpHexAttri);
          fOctalAttri     .Assign(tmpOctalAttri);
          fKeyAttri       .Assign(tmpKeyAttri);
          fSymbolAttri    .Assign(tmpSymbolAttri);
          fAsmAttri       .Assign(tmpAsmAttri);
          fCommentAttri   .Assign(tmpCommentAttri);
          fIdentifierAttri.Assign(tmpIdentifierAttri);
          fInvalidAttri   .Assign(tmpInvalidAttri);
          fSpaceAttri     .Assign(tmpSpaceAttri);
          fDirecAttri     .Assign(tmpDirecAttri);
        end;
        tmpStringAttri    .Free;
        tmpCharAttri      .Free;
        tmpNumberAttri    .Free;
        tmpFloatAttri     .Free;
        tmpHexAttri       .Free;
        tmpOctalAttri     .Free;
        tmpKeyAttri       .Free;
        tmpSymbolAttri    .Free;
        tmpAsmAttri       .Free;
        tmpCommentAttri   .Free;
        tmpIdentifierAttri.Free;
        tmpInvalidAttri   .Free;
        tmpSpaceAttri     .Free;
        tmpDirecAttri     .Free;
      end;
    finally s.Free; end;
  end; { ReadLUABSettings }
  {$ENDIF}

begin
  {$IFNDEF SYN_CLX}
  Result := ReadLUABSettings(settingIndex);
  {$ELSE}
  Result := False;
  {$ENDIF}
end; { TSynLua.UseUserSettings }

function TSynLua.GetIdentChars: TSynIdentChars;
begin
  Result := TSynValidStringChars;
end;

class function TSynLua.GetLanguageName: string;
begin
  Result := SYNS_LangLUA;
end;

class function TSynLua.GetCapabilities: TSynHighlighterCapabilities;
begin
  Result := inherited GetCapabilities + [hcUserSettings];
end;

function TSynLua.GetSampleSource: string;
begin
  Result := '// Syntax Highlighting'#13#10+
            'void __fastcall TForm1::Button1Click(TObject *Sender)'#13#10+
            '{'#13#10+
            '  int number = 123456;'#13#10+
            '  char c = ''a'';'#13#10+
            '  Caption = "The number is " + IntToStr(i);'#13#10+
            '  for (int i = 0; i <= number; i++)'#13#10+
            '  {'#13#10+
            '    x -= 0xff;'#13#10+
            '    x -= 023;'#13#10+
            '    x += 1.0;'#13#10+
            '    x += @; /* illegal character */'#13#10+
            '  }'#13#10+
            '  #ifdef USE_ASM'#13#10+
            '    asm'#13#10+
            '    {'#13#10+
            '      ASM MOV AX, 0x1234'#13#10+
            '      ASM MOV i, AX'#13#10+
            '    }'#13#10+
            '  #endif'#13#10+
            '}';

end;

initialization
  MakeIdentTable;
{$IFNDEF SYN_LUAB_1}                                                            //mh 2000-07-14
  RegisterPlaceableHighlighter(TSynLua);
{$ENDIF}
end.

