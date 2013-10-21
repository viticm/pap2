{-------------------------------------------------------------------------------
The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in compliance
with the License. You may obtain a copy of the License at
http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for
the specific language governing rights and limitations under the License.

The Original Code is: SynEditMiscProcs.pas, released 2000-04-07.
The Original Code is based on the mwSupportProcs.pas file from the
mwEdit component suite by Martin Waldenburg and other developers, the Initial
Author of this file is Michael Hieke.
All Rights Reserved.

Contributors to the SynEdit and mwEdit projects are listed in the
Contributors.txt file.

Alternatively, the contents of this file may be used under the terms of the
GNU General Public License Version 2 or later (the "GPL"), in which case
the provisions of the GPL are applicable instead of those above.
If you wish to allow use of your version of this file only under the terms
of the GPL and not to allow others to use your version of this file
under the MPL, indicate your decision by deleting the provisions above and
replace them with the notice and other provisions required by the GPL.
If you do not delete the provisions above, a recipient may use your version
of this file under either the MPL or the GPL.

$Id: SynEditMiscProcs.pas,v 1.1 2005/06/20 03:13:16 zhujianqiu Exp $

You may retrieve the latest version of this file at the SynEdit home page,
located at http://SynEdit.SourceForge.net

Known Issues:
-------------------------------------------------------------------------------}

{$IFNDEF QSYNEDITMISCPROCS}
unit SynEditMiscProcs;
{$ENDIF}

{$I SynEdit.inc}

interface

uses
{$IFDEF SYN_CLX}
  Types,
  kTextDrawer,
  QSynEditTypes,
  QSynEditHighlighter,
  {$IFDEF SYN_CLX}
  QGraphics,
  {$ENDIF}
{$ELSE}
  Windows,
  SynEditTypes,
  SynEditHighlighter,
{$ENDIF}
{$IFDEF SYN_COMPILER_4_UP}
  Math,
{$ENDIF}
  Classes;

type
  PIntArray = ^TIntArray;
  TIntArray = array[0..MaxListSize - 1] of integer;

{$IFNDEF SYN_COMPILER_4_UP}
function Max(x, y: integer): integer;
function Min(x, y: integer): integer;
{$ENDIF}

function MinMax(x, mi, ma: integer): integer;
procedure SwapInt(var l, r: integer);
function MaxPoint(const P1, P2: TPoint): TPoint;
function MinPoint(const P1, P2: TPoint): TPoint;

function GetIntArray(Count: Cardinal; InitialValue: integer): PIntArray;

{$IFNDEF SYN_CLX}
procedure InternalFillRect(dc: HDC; const rcPaint: TRect);
{$ENDIF}

// Converting tabs to spaces: To use the function several times it's better
// to use a function pointer that is set to the fastest conversion function.
type
  TConvertTabsProc = function(const Line: AnsiString;
    TabWidth: integer): AnsiString;

function GetBestConvertTabsProc(TabWidth: integer): TConvertTabsProc;
// This is the slowest conversion function which can handle TabWidth <> 2^n.
function ConvertTabs(const Line: AnsiString; TabWidth: integer): AnsiString;

{begin}                                                                         //mh 2000-10-19
type
  TConvertTabsProcEx = function(const Line: AnsiString; TabWidth: integer;
    var HasTabs: boolean): AnsiString;

function GetBestConvertTabsProcEx(TabWidth: integer): TConvertTabsProcEx;
// This is the slowest conversion function which can handle TabWidth <> 2^n.
function ConvertTabsEx(const Line: AnsiString; TabWidth: integer;
  var HasTabs: boolean): AnsiString;
{end}                                                                           //mh 2000-10-19

function GetExpandedLength(const aStr: string; aTabWidth: integer): integer;

function CharIndex2CaretPos(Index, TabWidth: integer;
  const Line: string): integer;
function CaretPos2CharIndex(Position, TabWidth: integer; const Line: string;
  var InsideTabChar: boolean): integer;

// search for the first char of set AChars in Line, starting at index Start
function StrScanForCharInSet(const Line: string; Start: integer;
  AChars: TSynIdentChars): integer;
// the same, but searching backwards
function StrRScanForCharInSet(const Line: string; Start: integer;
  AChars: TSynIdentChars): integer;

{$IFDEF SYN_MBCSSUPPORT}
type
  TStringType = (stNone, stHalfNumAlpha, stHalfSymbol, stHalfKatakana,
    stWideNumAlpha, stWideSymbol, stWideKatakana, stHiragana, stIdeograph,
    stControl, stKashida);

// search for first multibyte char in Line, starting at index Start
function StrScanForMultiByteChar(const Line: string; Start: Integer): Integer;
// the same, but searching backwards
function StrRScanForMultiByteChar(const Line: string; Start: Integer): Integer;
// convert a type returned from GetStringTypeEx() to something friendlier 
function IsStringType(Value: Word): TStringType;
{$ENDIF}

function GetEOL(Line: PChar): PChar;

// Remove all '/' characters from string by changing them into '\.'.
// Change all '\' characters into '\\' to allow for unique decoding.
function EncodeString(s: string): string;

// Decodes string, encoded with EncodeString.
function DecodeString(s: string): string;

{$IFNDEF SYN_COMPILER_5_UP}
procedure FreeAndNil(var Obj);
{$ENDIF}

{$IFNDEF SYN_COMPILER_3_UP}
procedure Assert(Expr: Boolean);  { stub for Delphi 2 }
{$ENDIF}

{$IFNDEF SYN_COMPILER_3_UP}
function LastDelimiter(const Delimiters, S: string): Integer;
{$ENDIF}

{$IFNDEF SYN_COMPILER_4_UP}
type
  TReplaceFlags = set of (rfReplaceAll, rfIgnoreCase);

function StringReplace(const S, OldPattern, NewPattern: string;
  Flags: TReplaceFlags): string;
{$ENDIF}

{$IFDEF SYN_CLX}
function GetRValue(RGBValue: TColor): byte;
function GetGValue(RGBValue: TColor): byte;
function GetBValue(RGBValue: TColor): byte;
{$ENDIF}

type
  THighlighterAttriProc = function (Highlighter: TSynCustomHighlighter;
    Attri: TSynHighlighterAttributes; UniqueAttriName: string;
    Params: array of Pointer): Boolean of object;

// Enums all child highlighters and their attributes of a TSynMultiSyn through a
// callback function.
// This function also handles nested TSynMultiSyns including their MarkerAttri.
function EnumHighlighterAttris(Highlighter: TSynCustomHighlighter;
  SkipDuplicates: Boolean; HighlighterAttriProc: THighlighterAttriProc;
  Params: array of Pointer): Boolean;

{$IFNDEF SYN_COMPILER_4_UP}
const
  C3_NONSPACING = 1; { nonspacing character }
  C3_DIACRITIC = 2; { diacritic mark }
  C3_VOWELMARK = 4; { vowel mark }
  C3_SYMBOL = 8; { symbols }
  C3_KATAKANA = $0010; { katakana character }
  C3_HIRAGANA = $0020; { hiragana character }
  C3_HALFWIDTH = $0040; { half width character }
  C3_FULLWIDTH = $0080; { full width character }
  C3_IDEOGRAPH = $0100; { ideographic character }
  C3_KASHIDA = $0200; { Arabic kashida character }
  C3_LEXICAL = $0400; { lexical character }
  C3_ALPHA = $8000; { any linguistic char (C1_ALPHA) }
  C3_NOTAPPLICABLE = 0; { ctype 3 is not applicable }
{$ENDIF}



implementation

uses
  SysUtils,
  {$IFDEF SYN_CLX}
  QSynHighlighterMulti;
  {$ELSE}
  SynHighlighterMulti;
  {$ENDIF}

{***}

{$IFNDEF SYN_COMPILER_4_UP}
function Max(x, y: integer): integer;
begin
  if x > y then Result := x else Result := y;
end;

function Min(x, y: integer): integer;
begin
  if x < y then Result := x else Result := y;
end;
{$ENDIF}

function MinMax(x, mi, ma: integer): integer;
begin
  x := Min( x, ma );
  Result := Max( x, mi );
end;

procedure SwapInt(var l, r: integer);
var
  tmp: integer;
begin
  tmp := r;
  r := l;
  l := tmp;
end;

function MaxPoint(const P1, P2: TPoint): TPoint;
begin
  if (P2.y > P1.y) or ((P2.y = P1.y) and (P2.x > P1.x)) then
    Result := P2
  else
    Result := P1;
end;

function MinPoint(const P1, P2: TPoint): TPoint;
begin
  if (P2.y < P1.y) or ((P2.y = P1.y) and (P2.x < P1.x)) then
    Result := P2
  else
    Result := P1;
end;

{***}

function GetIntArray(Count: Cardinal; InitialValue: integer): PIntArray;
var
  p: PInteger;
begin
  Result := AllocMem(Count * SizeOf(integer));
  if Assigned(Result) and (InitialValue <> 0) then begin
    p := PInteger(Result);
    while (Count > 0) do begin
      p^ := InitialValue;
      Inc(p);
      Dec(Count);
    end;
  end;
end;

{$IFNDEF SYN_CLX}
procedure InternalFillRect(dc: HDC; const rcPaint: TRect);
begin
  ExtTextOut(dc, 0, 0, ETO_OPAQUE, @rcPaint, nil, 0, nil);
end;
{$ENDIF}

{***}

// mh: Please don't change; no stack frame and efficient register use.
function GetHasTabs(pLine: PChar; var CharsBefore: integer): boolean;
begin
  CharsBefore := 0;
  if Assigned(pLine) then begin
    while (pLine^ <> #0) do begin
      if (pLine^ = #9) then break;
      Inc(CharsBefore);
      Inc(pLine);
    end;
    Result := (pLine^ = #9);
  end else
    Result := FALSE;
end;

{begin}                                                                         //mh 2000-10-19
function ConvertTabs1Ex(const Line: AnsiString; TabWidth: integer;
  var HasTabs: boolean): AnsiString;
var
  pDest: PChar;
  nBeforeTab: integer;
begin
  Result := Line;  // increment reference count only
  if GetHasTabs(pointer(Line), nBeforeTab) then begin
    HasTabs := TRUE;                                                            //mh 2000-11-08
    pDest := @Result[nBeforeTab + 1]; // this will make a copy of Line
    // We have at least one tab in the string, and the tab width is 1.
    // pDest points to the first tab char. We overwrite all tabs with spaces.
    repeat
      if (pDest^ = #9) then pDest^ := ' ';
      Inc(pDest);
    until (pDest^ = #0);
  end else
    HasTabs := FALSE;
end;

function ConvertTabs1(const Line: AnsiString; TabWidth: integer): AnsiString;
var
  HasTabs: boolean;
begin
  Result := ConvertTabs1Ex(Line, TabWidth, HasTabs);
end;

function ConvertTabs2nEx(const Line: AnsiString; TabWidth: integer;
  var HasTabs: boolean): AnsiString;
var
  i, DestLen, TabCount, TabMask: integer;
  pSrc, pDest: PChar;
begin
  Result := Line;  // increment reference count only
  if GetHasTabs(pointer(Line), DestLen) then begin
    HasTabs := TRUE;                                                            //mh 2000-11-08
    pSrc := @Line[1 + DestLen];
    // We have at least one tab in the string, and the tab width equals 2^n.
    // pSrc points to the first tab char in Line. We get the number of tabs
    // and the length of the expanded string now.
    TabCount := 0;
    TabMask := (TabWidth - 1) xor $7FFFFFFF;
    repeat
      if (pSrc^ = #9) then begin
        DestLen := (DestLen + TabWidth) and TabMask;
        Inc(TabCount);
      end else
        Inc(DestLen);
      Inc(pSrc);
    until (pSrc^ = #0);
    // Set the length of the expanded string.
    SetLength(Result, DestLen);
    DestLen := 0;
    pSrc := PChar(Line);
    pDest := PChar(Result);
    // We use another TabMask here to get the difference to 2^n.
    TabMask := TabWidth - 1;
    repeat
      if (pSrc^ = #9) then begin
        i := TabWidth - (DestLen and TabMask);
        Inc(DestLen, i);
        //This is used for both drawing and other stuff and is meant to be #9 and not #32
        repeat
          pDest^ := #9;
          Inc(pDest);
          Dec(i);
        until (i = 0);
        Dec(TabCount);
        if (TabCount = 0) then begin
          repeat
            Inc(pSrc);
            pDest^ := pSrc^;
            Inc(pDest);
          until (pSrc^ = #0);
          exit;
        end;
      end else begin
        pDest^ := pSrc^;
        Inc(pDest);
        Inc(DestLen);
      end;
      Inc(pSrc);
    until (pSrc^ = #0);
  end else
    HasTabs := FALSE;
end;

function ConvertTabs2n(const Line: AnsiString; TabWidth: integer): AnsiString;
var
  HasTabs: boolean;
begin
  Result := ConvertTabs2nEx(Line, TabWidth, HasTabs);
end;

function ConvertTabsEx(const Line: AnsiString; TabWidth: integer;
  var HasTabs: boolean): AnsiString;
var
  i, DestLen, TabCount: integer;
  pSrc, pDest: PChar;
begin
  Result := Line;  // increment reference count only
  if GetHasTabs(pointer(Line), DestLen) then begin
    HasTabs := TRUE;                                                            //mh 2000-11-08
    pSrc := @Line[1 + DestLen];
    // We have at least one tab in the string, and the tab width is greater
    // than 1. pSrc points to the first tab char in Line. We get the number
    // of tabs and the length of the expanded string now.
    TabCount := 0;
    repeat
      if (pSrc^ = #9) then begin
        DestLen := DestLen + TabWidth - DestLen mod TabWidth;
        Inc(TabCount);
      end else
        Inc(DestLen);
      Inc(pSrc);
    until (pSrc^ = #0);
    // Set the length of the expanded string.
    SetLength(Result, DestLen);
    DestLen := 0;
    pSrc := PChar(Line);
    pDest := PChar(Result);
    repeat
      if (pSrc^ = #9) then begin
        i := TabWidth - (DestLen mod TabWidth);
        Inc(DestLen, i);
        repeat
          pDest^ := #9;
          Inc(pDest);
          Dec(i);
        until (i = 0);
        Dec(TabCount);
        if (TabCount = 0) then begin
          repeat
            Inc(pSrc);
            pDest^ := pSrc^;
            Inc(pDest);
          until (pSrc^ = #0);
          exit;
        end;
      end else begin
        pDest^ := pSrc^;
        Inc(pDest);
        Inc(DestLen);
      end;
      Inc(pSrc);
    until (pSrc^ = #0);
  end else
    HasTabs := FALSE;
end;

function ConvertTabs(const Line: AnsiString; TabWidth: integer): AnsiString;
var
  HasTabs: boolean;
begin
  Result := ConvertTabsEx(Line, TabWidth, HasTabs);
end;

function IsPowerOfTwo(TabWidth: integer): boolean;
var
  nW: integer;
begin
  nW := 2;
  repeat
    if (nW >= TabWidth) then break;
    Inc(nW, nW);
  until (nW >= $10000);  // we don't want 64 kByte spaces...
  Result := (nW = TabWidth);
end;

function GetBestConvertTabsProc(TabWidth: integer): TConvertTabsProc;
begin
  if (TabWidth < 2) then Result := TConvertTabsProc(@ConvertTabs1)
    else if IsPowerOfTwo(TabWidth) then
      Result := TConvertTabsProc(@ConvertTabs2n)
    else
      Result := TConvertTabsProc(@ConvertTabs);
end;

function GetBestConvertTabsProcEx(TabWidth: integer): TConvertTabsProcEx;
begin
  if (TabWidth < 2) then Result := TConvertTabsProcEx(@ConvertTabs1Ex)
    else if IsPowerOfTwo(TabWidth) then
      Result := TConvertTabsProcEx(@ConvertTabs2nEx)
    else
      Result := TConvertTabsProcEx(@ConvertTabsEx);
end;
{end}                                                                           //mh 2000-10-19

function GetExpandedLength(const aStr: string; aTabWidth: integer): integer;
var
  iRun: PChar;
begin
  Result := 0;
  iRun := PChar(aStr);
  while iRun^ <> #0 do
  begin
    if iRun^ = #9 then
      Inc( Result, aTabWidth - (Result mod aTabWidth) )
    else
      Inc( Result );
    Inc( iRun );
  end;
end;

{***}

function CharIndex2CaretPos(Index, TabWidth: integer;
  const Line: string): integer;
var
  iChar: integer;
  pNext: PChar;
begin
// possible sanity check here: Index := Max(Index, Length(Line));
  if Index > 1 then begin
    if (TabWidth <= 1) or not GetHasTabs(pointer(Line), iChar) then
      Result := Index
    else begin
      if iChar + 1 >= Index then
        Result := Index
      else begin
        // iChar is number of chars before first #9
        Result := iChar;
        // Index is *not* zero-based
        Inc(iChar);
        Dec(Index, iChar);
        pNext := @Line[iChar];
        while Index > 0 do begin
          case pNext^ of
            #0:
              begin
                Inc(Result, Index);
                break;
              end;
            #9:
              begin
                // Result is still zero-based
                Inc(Result, TabWidth);
                Dec(Result, Result mod TabWidth);
              end;
            else Inc(Result);
          end;
          Dec(Index);
          Inc(pNext);
        end;
        // done with zero-based computation
        Inc(Result);
      end;
    end;
  end else
    Result := 1;
end;

function CaretPos2CharIndex(Position, TabWidth: integer; const Line: string;
  var InsideTabChar: boolean): integer;
var
  iPos: integer;
  pNext: PChar;
begin
  InsideTabChar := FALSE;
  if Position > 1 then begin
    if (TabWidth <= 1) or not GetHasTabs(pointer(Line), iPos) then
      Result := Position
    else begin
      if iPos + 1 >= Position then
        Result := Position
      else begin
        // iPos is number of chars before first #9
        Result := iPos + 1;
        pNext := @Line[Result];
        // for easier computation go zero-based (mod-operation)
        Dec(Position);
        while iPos < Position do begin
          case pNext^ of
            #0: break;
            #9: begin
                  Inc(iPos, TabWidth);
                  Dec(iPos, iPos mod TabWidth);
                  if iPos > Position then begin
                    InsideTabChar := TRUE;
                    break;
                  end;
                end;
            else Inc(iPos);
          end;
          Inc(Result);
          Inc(pNext);
        end;
      end;
    end;
  end else
    Result := Position;
end;

function StrScanForCharInSet(const Line: string; Start: integer;
  AChars: TSynIdentChars): integer;
var
  p: PChar;
begin
  if (Start > 0) and (Start <= Length(Line)) then
  begin
{$IFDEF SYN_MBCSSUPPORT}
    // don't start on a trail byte
    if ByteType(Line, Start) = mbTrailByte then
    begin
      Inc(Start);
      if Start > Length(Line) then
      begin
        Result := 0;
        Exit;
      end;
    end;
{$ENDIF}
    p := PChar(@Line[Start]);
    repeat
{$IFDEF SYN_MBCSSUPPORT}
      // skip over multibyte characters
      if p^ in LeadBytes then
      begin
        Inc(p);
        Inc(Start);
        if p^ = #0 then
          Break;
      end
      else
{$ENDIF}
      if p^ in AChars then
      begin
        Result := Start;
        exit;
      end;
      Inc(p);
      Inc(Start);
    until p^ = #0;
  end;
  Result := 0;
end;

function StrRScanForCharInSet(const Line: string; Start: integer;
  AChars: TSynIdentChars): integer;
var
  I: Integer;
begin
  Result := 0;
  if (Start > 0) and (Start <= Length(Line)) then begin
{$IFDEF SYN_MBCSSUPPORT}
    if not SysLocale.FarEast then begin
{$ENDIF}
      for I := Start downto 1 do
        if Line[I] in AChars then begin
          Result := I;
          Exit;
        end;
{$IFDEF SYN_MBCSSUPPORT}
    end
    else begin
      // it's a lot faster to start from the beginning and go forward than to go
      // backward and call ByteType on every character
      I := 1;
      while I <= Start do begin
        if Line[I] in LeadBytes then
          Inc(I)
        else if Line[I] in AChars then
          Result := I;
        Inc(I);
      end;
    end;
{$ENDIF}
  end;
end;

{$IFDEF SYN_MBCSSUPPORT}
function StrScanForMultiByteChar(const Line: string; Start: Integer): Integer;
var
  I: Integer;
begin
  if SysLocale.FarEast and (Start > 0) and (Start <= Length(Line)) then begin
    // don't start on a trail byte
    if ByteType(Line, Start) = mbTrailByte then
      Inc(Start);
    for I := Start to Length(Line) do
      if Line[I] in LeadBytes then begin
        Result := I;
        Exit;
      end;
  end;
  Result := 0;
end;
{$ENDIF}

{$IFDEF SYN_MBCSSUPPORT}
function StrRScanForMultiByteChar(const Line: string; Start: Integer): Integer;
var
  I: Integer;
begin
  Result := 0;
  if SysLocale.FarEast and (Start > 0) and (Start <= Length(Line)) then begin
    // it's a lot faster to start from the beginning and go forward than to go
    // backward and call ByteType on every character
    I := 1;
    while I <= Start do begin
      if Line[I] in LeadBytes then begin
        Result := I;
        Inc(I);
      end;
      Inc(I);
    end;
  end;
end;
{$ENDIF}

function GetEOL(Line: PChar): PChar;
begin
  Result := Line;
  if Assigned(Result) then
    while not (Result^ in [#0, #10, #13]) do
      Inc(Result);
end;

{begin}                                                                         //gp 2000-06-24
{$IFOPT R+}{$DEFINE RestoreRangeChecking}{$ELSE}{$UNDEF RestoreRangeChecking}{$ENDIF}
{$R-}
function EncodeString(s: string): string;
var
  i, j: integer;
begin
  SetLength(Result, 2 * Length(s)); // worst case
  j := 0;
  for i := 1 to Length(s) do begin
    Inc(j);
    if s[i] = '\' then begin
      Result[j] := '\';
      Result[j + 1] := '\';
      Inc(j);
    end else if s[i] = '/' then begin
      Result[j] := '\';
      Result[j + 1] := '.';
      Inc(j);
    end else
      Result[j] := s[i];
  end; //for
  SetLength(Result, j);
end; { EncodeString }

function DecodeString(s: string): string;
var
  i, j: integer;
begin
  SetLength(Result, Length(s)); // worst case
  j := 0;
  i := 1;
  while i <= Length(s) do begin
    Inc(j);
    if s[i] = '\' then begin
      Inc(i);
      if s[i] = '\' then
        Result[j] := '\'
      else
        Result[j] := '/';
    end else
      Result[j] := s[i];
    Inc(i);
  end; //for
  SetLength(Result,j);
end; { DecodeString }
{$IFDEF RestoreRangeChecking}{$R+}{$ENDIF}
{end}                                                                           //gp 2000-06-24

{$IFNDEF SYN_COMPILER_5_UP}
procedure FreeAndNil(var Obj);
var
  P: TObject;
begin
  P := TObject(Obj);
  TObject(Obj) := nil;
  P.Free;
end;
{$ENDIF}

{$IFNDEF SYN_COMPILER_3_UP}
procedure Assert(Expr: Boolean);  { stub for Delphi 2 }
begin
end;
{$ENDIF}


{$IFNDEF SYN_COMPILER_3_UP}
function LastDelimiter(const Delimiters, S: string): Integer;
var
  P: PChar;
begin
  Result := Length(S);
  P := PChar(Delimiters);
  while Result > 0 do
  begin
    if (S[Result] <> #0) and (StrScan(P, S[Result]) <> nil) then
{$IFDEF SYN_MBCSSUPPORT}
      if (ByteType(S, Result) = mbTrailByte) then
        Dec(Result)
      else
{$ENDIF}
        Exit;
{$IFDEF SYN_KYLIX}
    begin
      if (ByteType(S, Result) <> mbTrailByte) then
        Exit;
      Dec(Result);
      while ByteType(S, Result) = mbTrailByte do
        Dec(Result);
    end;
{$ENDIF}
    Dec(Result);
  end;
end;
{$ENDIF}

{$IFNDEF SYN_COMPILER_4_UP}
function StringReplace(const S, OldPattern, NewPattern: string;
  Flags: TReplaceFlags): string;
var
  SearchStr, Patt, NewStr: string;
  Offset: Integer;
begin
  if rfIgnoreCase in Flags then
  begin
    SearchStr := AnsiUpperCase(S);
    Patt := AnsiUpperCase(OldPattern);
  end else
  begin
    SearchStr := S;
    Patt := OldPattern;
  end;
  NewStr := S;
  Result := '';
  while SearchStr <> '' do
  begin
    {$IFDEF SYN_COMPILER_3_UP}
    Offset := AnsiPos(Patt, SearchStr);
    {$ELSE}
    Offset := Pos(Patt, SearchStr);     // Pos does not support MBCS
    {$ENDIF}
    if Offset = 0 then
    begin
      Result := Result + NewStr;
      Break;
    end;
    Result := Result + Copy(NewStr, 1, Offset - 1) + NewPattern;
    NewStr := Copy(NewStr, Offset + Length(OldPattern), MaxInt);
    if not (rfReplaceAll in Flags) then
    begin
      Result := Result + NewStr;
      Break;
    end;
    SearchStr := Copy(SearchStr, Offset + Length(Patt), MaxInt);
  end;
end;
{$ENDIF}

{$IFDEF SYN_CLX}
type
  TColorRec = packed record
    Blue: Byte;
    Green: Byte;
    Red: Byte;
    Unused: Byte;
  end;

function GetRValue(RGBValue: TColor): byte;
begin
  Result := TColorRec(RGBValue).Red;
end;

function GetGValue(RGBValue: TColor): byte;
begin
  Result := TColorRec(RGBValue).Green;
end;

function GetBValue(RGBValue: TColor): byte;
begin
  Result := TColorRec(RGBValue).Blue;
end;
{$ENDIF}

{$IFDEF SYN_MBCSSUPPORT}
function IsStringType(Value: Word): TStringType;
begin
  Result := stNone;

  (***  Controls  ***)
  if (Value = C3_SYMBOL) then
    Result := stControl
  (*** singlebyte ***)
  else if ((Value and C3_HALFWIDTH) <> 0) then
  begin
    if (Value = C3_HALFWIDTH) or (Value = (C3_ALPHA or C3_HALFWIDTH)) then
      Result := stHalfNumAlpha { Number & Alphabet }
    else if ((Value and C3_SYMBOL) <> 0) or ((Value and C3_LEXICAL) <> 0) then
      Result := stHalfSymbol { Symbol }
    else if ((Value and C3_KATAKANA) <> 0) then
      Result := stHalfKatakana; { Japanese-KATAKANA }
  end
  (*** doublebyte ***)
  else begin
    if (Value = C3_FULLWIDTH) or (Value = (C3_ALPHA or C3_FULLWIDTH)) then
      Result := stWideNumAlpha { Number & Alphabet }
    else if ((Value and C3_SYMBOL) <> 0) or ((Value and C3_LEXICAL) <> 0) then
      Result := stWideSymbol { Symbol }
    else if ((Value and C3_KATAKANA) <> 0) then
      Result := stWideKatakana { Japanese-KATAKANA }
    else if ((Value and C3_HIRAGANA) <> 0) then
      Result := stHiragana { Japanese-HIRAGANA }
    else if ((Value and C3_IDEOGRAPH) <> 0) then
      Result := stIdeograph; { Ideograph }
  end;
end;
{$ENDIF}

function DeleteTypePrefixAndSynSuffix(S: string): string;
begin
  Result := S;
  if Result[1] in ['T', 't'] then //ClassName is never empty so no AV possible
    if Pos('tsyn', LowerCase(Result)) = 1 then
      Delete(Result, 1, 4)
    else
      Delete(Result, 1, 1);

  if Copy(LowerCase(Result), Length(Result) - 2, 3) = 'syn' then
    SetLength(Result, Length(Result) - 3);
end;

function GetHighlighterIndex(Highlighter: TSynCustomHighlighter;
  HighlighterList: TList): Integer;
var
  i: Integer;
begin
  Result := 1;
  for i := 0 to HighlighterList.Count - 1 do
    if HighlighterList[i] = Highlighter then
      Exit
    else if TObject(HighlighterList[i]).ClassType = Highlighter.ClassType then
      inc(Result);
end;

function InternalEnumHighlighterAttris(Highlighter: TSynCustomHighlighter;
  SkipDuplicates: Boolean; HighlighterAttriProc: THighlighterAttriProc;
  Params: array of Pointer; HighlighterList: TList): Boolean;
var
  i: Integer;
  UniqueAttriName: string;
begin
  Result := True;

  if (HighlighterList.IndexOf(Highlighter) >= 0) then
  begin
    if SkipDuplicates then Exit;
  end
  else
    HighlighterList.Add(Highlighter);

  if Highlighter is TSynMultiSyn then
    with TSynMultiSyn(Highlighter) do
    begin                                         
      Result := InternalEnumHighlighterAttris(DefaultHighlighter, SkipDuplicates,
        HighlighterAttriProc, Params, HighlighterList);
      if not Result then Exit;

      for i := 0 to Schemes.Count - 1 do
      begin
        UniqueAttriName := DeleteTypePrefixAndSynSuffix(Highlighter.ClassName) +
          IntToStr(GetHighlighterIndex(Highlighter, HighlighterList)) + '.' +
          Schemes[i].MarkerAttri.Name + IntToStr(i + 1);

        Result := HighlighterAttriProc(Highlighter, Schemes[i].MarkerAttri,
          UniqueAttriName, Params);
        if not Result then Exit;

        Result := InternalEnumHighlighterAttris(Schemes[i].Highlighter,
          SkipDuplicates, HighlighterAttriProc, Params, HighlighterList);
        if not Result then Exit
      end
    end
  else if Assigned(Highlighter) then
    for i := 0 to Highlighter.AttrCount - 1 do
    begin
      UniqueAttriName := DeleteTypePrefixAndSynSuffix(Highlighter.ClassName) +
        IntToStr(GetHighlighterIndex(Highlighter, HighlighterList)) + '.' +
        Highlighter.Attribute[i].Name;

      Result := HighlighterAttriProc(Highlighter, Highlighter.Attribute[i],
        UniqueAttriName, Params);
      if not Result then Exit
    end
end;

function EnumHighlighterAttris(Highlighter: TSynCustomHighlighter;
  SkipDuplicates: Boolean; HighlighterAttriProc: THighlighterAttriProc;
  Params: array of Pointer): Boolean;
var
  HighlighterList: TList;
begin
  if not Assigned(Highlighter) or not Assigned(HighlighterAttriProc) then
  begin
    Result := False;
    Exit;
  end;

  HighlighterList := TList.Create;
  try
    Result := InternalEnumHighlighterAttris(Highlighter, SkipDuplicates,
      HighlighterAttriProc, Params, HighlighterList)
  finally
    HighlighterList.Free
  end
end;

end.

