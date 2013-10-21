{-------------------------------------------------------------------------------
The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in compliance
with the License. You may obtain a copy of the License at
http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for
the specific language governing rights and limitations under the License.

The Original Code is: SynHighlighterRuby.pas, released 2001-11-13.
The Initial Author of this file is Stefan Ascher.
All Rights Reserved.
Portions by Jan Verhoeven (http://jansfreeware.com/jfdelphi.htm)

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

$Id: SynHighlighterRuby.pas,v 1.1 2005/06/20 03:15:57 zhujianqiu Exp $

You may retrieve the latest version of this file at the SynEdit home page,
located at http://SynEdit.SourceForge.net

Known Issues:
-------------------------------------------------------------------------------}
{
@abstract(Provides a Ruby highlighter for SynEdit)
@author(Stefan Ascher <stievie2002@yahoo.com>)
@created(21 May 2001)
@lastmod(2001-11-13)
The SynHighlighterVisualLisp unit provides SynEdit with a Ruby highlighter.
}

{$IFNDEF QSYNHIGHLIGHTERRUBY}
unit SynHighlighterRuby;
{$ENDIF}

{$I SynEdit.inc}

interface

uses
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
  cAlphaNumericNameChars = ['A'..'Z', 'a'..'z', '0'..'9', '_'];

type
  TtkTokenKind = (tkComment, tkIdentifier, tkKey, tkNull, tkNumber, tkSecondKey,
    tkSpace, tkString, tkSymbol, tkUnknown);

  TRangeState = (rsUnknown, rsAnsi, rsPasStyle, rsCStyle, rsMultilineString);

  TProcTableProc = procedure of object;

type
  TSynRubySyn = class(TSynCustomHighlighter)
  private
    fRange: TRangeState;
    fLine: PChar;
    fProcTable: array[#0..#255] of TProcTableProc;
    Run: LongInt;
    fTokenPos: Integer;
    FTokenID: TtkTokenKind;
    fLineNumber: Integer;
    fStringAttri: TSynHighlighterAttributes;
    fSymbolAttri: TSynHighlighterAttributes;
    fKeyAttri: TSynHighlighterAttributes;
    fSecondKeyAttri: TSynHighlighterAttributes;
    fNumberAttri: TSynHighlighterAttributes;
    fCommentAttri: TSynHighlighterAttributes;
    fSpaceAttri: TSynHighlighterAttributes;
    fIdentifierAttri: TSynHighlighterAttributes;
    fKeyWords: TStrings;
    fSecondKeys: TStrings;

    FTokenMatch : String;
    procedure BraceOpenProc;
    procedure PointCommaProc;
    procedure CRProc;
    procedure IdentProc;
    procedure LFProc;
    procedure NullProc;
    procedure NumberProc;
    procedure RoundOpenProc;
    procedure SlashProc;
    procedure SpaceProc;
    procedure StringProc;
    procedure UnknownProc;
    procedure MakeMethodTables;
    procedure AnsiProc;
    procedure PasStyleProc;
    procedure CStyleProc;
    procedure SetSecondKeys(const Value: TStrings);
    procedure MultiLineProc;
    procedure StringEndProc;
  protected
    function GetSampleSource: string; override;
  public
    class function GetLanguageName: string; override;
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    function GetDefaultAttribute(Index: integer): TSynHighlighterAttributes;
      override;
    function GetEol: Boolean; override;
    function GetRange: Pointer; override;
    function GetTokenID: TtkTokenKind;
    function IsKeyword(const AKeyword: string): boolean; override;              //mh 2000-11-08
    function IsSecondKeyWord(aToken: string): Boolean;
    procedure SetLine(NewValue: string; LineNumber:Integer); override;
    function GetToken: string; override;
    function GetTokenAttribute: TSynHighlighterAttributes; override;
    function GetTokenKind: integer; override;
    function GetTokenPos: Integer; override;
    procedure Next; override;
    procedure SetRange(Value: Pointer); override;
    procedure ResetRange; override;
  published
    property CommentAttri: TSynHighlighterAttributes read fCommentAttri
      write fCommentAttri;
    property IdentifierAttri: TSynHighlighterAttributes read fIdentifierAttri
      write fIdentifierAttri;
    property KeyAttri: TSynHighlighterAttributes read fKeyAttri write fKeyAttri;
    property SecondKeyAttri: TSynHighlighterAttributes read fSecondKeyAttri
      write fSecondKeyAttri;
    property SecondKeyWords: TStrings read fSecondKeys write SetSecondKeys;
    property NumberAttri: TSynHighlighterAttributes read fNumberAttri
      write fNumberAttri;
    property SpaceAttri: TSynHighlighterAttributes read fSpaceAttri
      write fSpaceAttri;
    property StringAttri: TSynHighlighterAttributes read fStringAttri
      write fStringAttri;
    property SymbolAttri: TSynHighlighterAttributes read fSymbolAttri
      write fSymbolAttri;
  end;

implementation

uses
{$IFDEF SYN_CLX}
  QSynEditStrConst;
{$ELSE}
  SynEditStrConst;
{$ENDIF}

const
  RubyKeysCount = 43;
  RubyKeys: array[1..RubyKeysCount] of string = (
    'ALIAS', 'ATTR', 'BEGIN', 'BREAK', 'CASE', 'CLASS', 'DEF', 'DO', 'ELSE',
    'ELSIF', 'END', 'ENSURE', 'EXIT', 'EXTEND', 'FALSE', 'FOR', 'GETS', 'IF',
    'IN', 'INCLUDE', 'LOAD', 'LOOP', 'MODULE', 'NEXT', 'NIL', 'NOT', 'PRINT',
    'PRIVATE', 'PUBLIC', 'PUTS', 'RAISE', 'REDO', 'REQUIRE', 'RESCUE', 'RETRY',
    'RETURN', 'SELF', 'THEN', 'TRUE', 'UNLESS', 'WHEN', 'WHILE', 'YIELD');

var
  Identifiers: array[#0..#255] of ByteBool;
  mHashTable: array[#0..#255] of Integer;

procedure MakeIdentTable;
var
  I, J: Char;
begin
  for I := #0 to #255 do
  begin
    case I of
      '_', '0'..'9', 'a'..'z', 'A'..'Z':
        Identifiers[I] := True;
      else
        Identifiers[I] := False;
    end;
    J := UpCase(I);
    case I in ['_', 'a'..'z', 'A'..'Z'] of
      True: mHashTable[I] := Ord(J) - 64
      else mHashTable[I] := 0;
    end;
  end;
end;

function TSynRubySyn.IsKeyword(const AKeyword: string): boolean;                //mh 2000-11-08
var
  First, Last, I, Compare: Integer;
  Token: String;
begin
  First := 0;
  Last := fKeywords.Count - 1;
  Result := False;
  Token := UpperCase(AKeyword);

  while First <= Last do begin
    I := (First + Last) shr 1;
    Compare := CompareStr(fKeywords[I], Token);
    if Compare = 0 then begin
      Result := True;
      break;
    end else
      if Compare < 0 then First := I + 1 else Last := I - 1;
  end;
end; { IsKeyWord }

function TSynRubySyn.IsSecondKeyWord(aToken: String): Boolean;
var
  First, Last, I, Compare: Integer;
  Token: String;
begin
  First := 0;
  Last := fSecondKeys.Count - 1;
  Result := False;
  Token := UpperCase(aToken);
  while First <= Last do
  begin
    I := (First + Last) shr 1;
    Compare := CompareStr(fSecondKeys[i], Token);
    if Compare = 0 then
    begin
      Result := True;
      break;
    end
    else
      if Compare < 0 then First := I + 1 else Last := I - 1;
  end;
end; { IsSecondKeyWord }

procedure TSynRubySyn.MakeMethodTables;
var
  I: Char;
begin
  for I := #0 to #255 do
    case I of
      '<': fProcTable[I] := MultiLineProc;
      '#': fProcTable[I] := SlashProc{!@#$AsciiCharProc};
      '{': fProcTable[I] := BraceOpenProc;
      ';': fProcTable[I] := PointCommaProc;
      #13: fProcTable[I] := CRProc;
      'A'..'Z', 'a'..'z', '_': fProcTable[I] := IdentProc;
      #10: fProcTable[I] := LFProc;
      #0: fProcTable[I] := NullProc;
      '0'..'9': fProcTable[I] := NumberProc;
      '(': fProcTable[I] := RoundOpenProc;
      '/': fProcTable[I] := SlashProc;
      #1..#9, #11, #12, #14..#32: fProcTable[I] := SpaceProc;
      #34, #39{!@#$#39}: fProcTable[I] := StringProc;
      else fProcTable[I] := UnknownProc;
    end;
end;

constructor TSynRubySyn.Create(AOwner: TComponent);
var
  i: integer;
begin
  inherited Create(AOwner);
  fKeyWords := TStringList.Create;
  TStringList(fKeyWords).Sorted := True;
  TStringList(fKeyWords).Duplicates := dupIgnore;
  fSecondKeys := TStringList.Create;
  TStringList(fSecondKeys).Sorted := True;
  TStringList(fSecondKeys).Duplicates := dupIgnore;
  if not (csDesigning in ComponentState) then
    for i := 1 to RubyKeysCount do
      fKeyWords.Add(RubyKeys[i]);

  fCommentAttri := TSynHighlighterAttributes.Create(SYNS_AttrComment);
  fCommentAttri.Foreground := clMaroon;
  AddAttribute(fCommentAttri);
  fIdentifierAttri := TSynHighlighterAttributes.Create(SYNS_AttrIdentifier);
  AddAttribute(fIdentifierAttri);
  fKeyAttri := TSynHighlighterAttributes.Create(SYNS_AttrReservedWord);
  fKeyAttri.Foreground := clBlue;
  AddAttribute(fKeyAttri);
  fSecondKeyAttri := TSynHighlighterAttributes.Create(SYNS_AttrSecondReservedWord);
  AddAttribute(fSecondKeyAttri);
  fNumberAttri := TSynHighlighterAttributes.Create(SYNS_AttrNumber);
  fNumberAttri.Foreground := clGreen;
  AddAttribute(fNumberAttri);
  fSpaceAttri := TSynHighlighterAttributes.Create(SYNS_AttrSpace);
  AddAttribute(fSpaceAttri);
  fStringAttri := TSynHighlighterAttributes.Create(SYNS_AttrString);
  fStringAttri.Foreground := clPurple;
  AddAttribute(fStringAttri);
  fSymbolAttri := TSynHighlighterAttributes.Create(SYNS_AttrSymbol);
  fSymbolAttri.Foreground := clBlue;
  AddAttribute(fSymbolAttri);
  SetAttributesOnChange(DefHighlightChange);

  MakeMethodTables;
  fRange := rsUnknown;
  fDefaultFilter := SYNS_FilterRuby;
  FTokenMatch := '';
end; { Create }

destructor TSynRubySyn.Destroy;
begin
  fKeyWords.Free;
  fSecondKeys.Free;
  inherited Destroy;
end; { Destroy }

procedure TSynRubySyn.SetLine(NewValue: String; LineNumber:Integer);
begin
  fLine := PChar(NewValue);
  Run := 0;
  fLineNumber := LineNumber;
  Next;
end; { SetLine }

procedure TSynRubySyn.AnsiProc;
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

  while fLine[Run] <> #0 do
    case fLine[Run] of
      '*':
        if fLine[Run + 1] = ')' then
        begin
          fRange := rsUnKnown;
          inc(Run, 2);
          break;
        end else inc(Run);
      #10: break;

      #13: break;
    else inc(Run);
    end;
end;

procedure TSynRubySyn.MultiLineProc;
var ExtraInc : Integer;
    MatchChar: Char;
begin
  case FLine[Run + 1] of
    '<': begin
           fTokenID := tkSymbol;
           ExtraInc := 0;
           MatchChar := #0;
           if (FLine[Run + 2] = '-') and (FLine[Run + 3] in ['"', '''']) then
           begin
             ExtraInc := 2;
             MatchChar := FLine[Run + 3];
           end;
           if (FLine[Run + 2+ ExtraInc] in cAlphaNumericNameChars) then
           begin
             fTokenID := tkString;
             inc(Run, 2 + ExtraInc);
             //get the named token and search for it's match.  It's possible on multiple lines
             fRange := rsMultilineString;
             FTokenMatch := '';
             while FLine[Run] in cAlphaNumericNameChars do
             begin
               FTokenMatch := FTokenMatch + FLine[Run];
               inc(Run);
             end;
             if (MatchChar <> #0) and (FLine[Run] <> MatchChar) then
             begin
               fRange := rsUnknown;
               FTokenMatch := '';
               fTokenID := tkUnknown;
             end;
           end else UnknownProc;
         end;
  else                                 {less than}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
end;

procedure TSynRubySyn.StringEndProc;
var i : Integer;
    DoEndToken : Boolean;
begin
  fTokenID := tkString;
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
  //Now check to see if the token is a match and end the multi-line string
  if (FTokenMatch <> '') and (FLine[Run] = FTokenMatch[1]) then
  begin
    DoEndToken := True;
    for i := 2 to length(FTokenMatch) do
      if FLine[Run + i - 1] <> FTokenMatch[i] then
      begin
        DoEndToken := False;
        break;
      end;
    if DoEndToken and (FLine[Run + length(FTokenMatch)] in [#9, #32, #10, #13, #0]) then
    begin
      inc(Run, length(FTokenMatch) - 1);
      FTokenMatch := '';
    end else inc(Run);
  end else inc(Run);

  if FTokenMatch = '' then
    fRange := rsUnknown;
end;

procedure TSynRubySyn.PasStyleProc;
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
      '}':
        begin
          fRange := rsUnKnown;
          inc(Run);
          break;
        end;
      #10: break;

      #13: break;
    else inc(Run);
    end;
end;

procedure TSynRubySyn.CStyleProc;
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

  while fLine[Run] <> #0 do
    case fLine[Run] of
      '*':
        if fLine[Run + 1] = '/' then
        begin
          fRange := rsUnKnown;
          inc(Run, 2);
          break;
        end else inc(Run);
      #10: break;

      #13: break;
    else inc(Run);
    end;
end;

procedure TSynRubySyn.BraceOpenProc;
begin
  inc(Run);
  fTokenID := tkSymbol;
end;

procedure TSynRubySyn.PointCommaProc;
begin
  inc(Run);
  fTokenID := tkSymbol;
end;

procedure TSynRubySyn.CRProc;
begin
  fTokenID := tkSpace;
  case FLine[Run + 1] of
    #10: inc(Run, 2);
  else inc(Run);
  end;
end;

procedure TSynRubySyn.IdentProc;
begin
  while Identifiers[fLine[Run]] do inc(Run);
  if IsKeyWord(GetToken) then begin
    fTokenId := tkKey;
    Exit;
  end
  else fTokenId := tkIdentifier;
  if IsSecondKeyWord(GetToken)
    then fTokenId := tkSecondKey
    else fTokenId := tkIdentifier;
end;

procedure TSynRubySyn.LFProc;
begin
  fTokenID := tkSpace;
  inc(Run);
end;

procedure TSynRubySyn.NullProc;
begin
  fTokenID := tkNull;
end;

procedure TSynRubySyn.NumberProc;
begin
  inc(Run);
  fTokenID := tkNumber;
  while FLine[Run] in ['0'..'9', '.', 'e', 'E'] do
  begin
    case FLine[Run] of
      '.':
        if FLine[Run + 1] = '.' then break;
    end;
    inc(Run);
  end;
end;

procedure TSynRubySyn.RoundOpenProc;
begin
  inc(Run);
  fTokenId := tkSymbol;
end;

procedure TSynRubySyn.SlashProc;
begin
  case FLine[Run{ + 1}] of
    '/':
      begin
        inc(Run);
        fTokenId := tkSymbol;
        {Inc(Run, 2);
        fTokenID := tkComment;
        while FLine[Run] <> #0 do
        begin
          case FLine[Run] of
            #10, #13: break;
          end;
          inc(Run);
        end;}
      end;
    '*':
      begin
        inc(Run);
        fTokenId := tkSymbol;
      end;
  else
    begin
      fTokenID := tkComment;
      while FLine[Run] <> #0 do
      begin
        case FLine[Run] of
          #10, #13: break;
        end;
        inc(Run);
      end;
    end;
  end;
end;

procedure TSynRubySyn.SpaceProc;
begin
  inc(Run);
  fTokenID := tkSpace;
  while FLine[Run] in [#1..#9, #11, #12, #14..#32] do inc(Run);
end;

procedure TSynRubySyn.StringProc;
var
  QuoteChar: Char;
begin
// Ha, ha, Strings in Ruby (could be anything)!!!!

//There are three more ways to construct string literals: %q, %Q, and ``here
//documents.''
//
//%q and %Q start delimited single- and double-quoted strings.
//
//%q/general single-quoted string/ » general single-quoted string
//%Q!general double-quoted string! » general double-quoted string
//%Q{Seconds/day: #{24*60*60}}     » Seconds/day: 86400
//
//The character following the ``q'' or ``Q'' is the delimiter. If it is an
//opening bracket, brace, parenthesis, or less-than sign, the string is read
//until the matching close symbol is found. Otherwise the string is read until
//the next occurrence of the same delimiter.

  fTokenID := tkString;
  QuoteChar := FLine[Run];      // either " or '
  if (FLine[Run + 1] = QuoteChar) and (FLine[Run + 2] = QuoteChar)
    then inc(Run, 2);
  repeat
    case FLine[Run] of
      #0, #10, #13: break;
    end;
    inc(Run);
  until FLine[Run] = QuoteChar;
  if FLine[Run] <> #0 then inc(Run);
end;

procedure TSynRubySyn.UnknownProc;
begin
  inc(Run);
  fTokenID := tkUnKnown;
end;

procedure TSynRubySyn.Next;
begin
  fTokenPos := Run;
  case fRange of
    rsAnsi: AnsiProc;
    rsPasStyle: PasStyleProc;
    rsCStyle: CStyleProc;
    rsMultilineString : StringEndProc;    
  else
    fProcTable[fLine[Run]];
  end;
end;

function TSynRubySyn.GetDefaultAttribute(Index: integer): TSynHighlighterAttributes;
begin
  case Index of
    SYN_ATTR_COMMENT: Result := fCommentAttri;
    SYN_ATTR_IDENTIFIER: Result := fIdentifierAttri;
    SYN_ATTR_KEYWORD: Result := fKeyAttri;
    SYN_ATTR_STRING: Result := fStringAttri;
    SYN_ATTR_WHITESPACE: Result := fSpaceAttri;
  else
    Result := nil;
  end;
end;

function TSynRubySyn.GetEol: Boolean;
begin
  Result := False;
  if fTokenId = tkNull then Result := True;
end;

function TSynRubySyn.GetRange: Pointer;
begin
  Result := Pointer(fRange);
end;

function TSynRubySyn.GetToken: string;
var
  Len: LongInt;
begin
  Len := Run - fTokenPos;
  SetString(Result, (FLine + fTokenPos), Len);
end;

function TSynRubySyn.GetTokenID: TtkTokenKind;
begin
  Result := fTokenId;
end;

function TSynRubySyn.GetTokenAttribute: TSynHighlighterAttributes;
begin
  case fTokenID of
    tkComment: Result := fCommentAttri;
    tkIdentifier: Result := fIdentifierAttri;
    tkKey: Result := fKeyAttri;
    tkSecondKey: Result := fSecondKeyAttri;
    tkNumber: Result := fNumberAttri;
    tkSpace: Result := fSpaceAttri;
    tkString: Result := fStringAttri;
    tkSymbol: Result := fSymbolAttri;
    tkUnknown: Result := fSymbolAttri;
  else
    Result := nil;
  end;
end;

function TSynRubySyn.GetTokenKind: integer;
begin
  Result := Ord(fTokenId);
end;

function TSynRubySyn.GetTokenPos: Integer;
begin
  Result := fTokenPos;
end;

procedure TSynRubySyn.ResetRange;
begin
  fRange := rsUnknown;
end;

procedure TSynRubySyn.SetRange(Value: Pointer);
begin
  fRange := TRangeState(Value);
end;

procedure TSynRubySyn.SetSecondKeys(const Value: TStrings);
var
  i: Integer;
begin
  if Value <> nil then
    begin
      Value.BeginUpdate;
      for i := 0 to Value.Count - 1 do
        Value[i] := UpperCase(Value[i]);
      Value.EndUpdate;
    end;
  fSecondKeys.Assign(Value);
  DefHighLightChange(nil);
end;

class function TSynRubySyn.GetLanguageName: string;
begin
  Result := SYNS_LangRuby;
end;

function TSynRubySyn.GetSampleSource: string;
begin
  Result :=
    '# Factorial'+#13#10+
    'def fact(n)'+#13#10+
    '  if n == 0'+#13#10+
    '    1'+#13#10+
    '  else'+#13#10+
    '    n * fact(n-1)'+#13#10+
    '  end'+#13#10+
    'end'+#13#10+
    'print fact(ARGV[0].to_i), "\n"';
end;

initialization
  MakeIdentTable;
{$IFNDEF SYN_CPPB_1}
  RegisterPlaceableHighlighter(TSynRubySyn);
{$ENDIF}
end.

