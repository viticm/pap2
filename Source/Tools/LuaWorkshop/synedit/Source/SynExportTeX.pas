{-------------------------------------------------------------------------------
The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in compliance
with the License. You may obtain a copy of the License at
http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for
the specific language governing rights and limitations under the License.

The Original Code is: SynExportTeX.pas, released 2002-09-12.

The Original Code is partly based on the mwHTMLExport.pas file from the
mwEdit component suite by Martin Waldenburg and other developers, the Initial
Author of this file is Ascher Stefan.
Portions created by Ascher Stefan are Copyright 2002 Ascher Stefan.
All Rights Reserved.

Contributors to the SynEdit project are listed in the Contributors.txt file.

Alternatively, the contents of this file may be used under the terms of the
GNU General Public License Version 2 or later (the "GPL"), in which case
the provisions of the GPL are applicable instead of those above.
If you wish to allow use of your version of this file only under the terms
of the GPL and not to allow others to use your version of this file
under the MPL, indicate your decision by deleting the provisions above and
replace them with the notice and other provisions required by the GPL.
If you do not delete the provisions above, a recipient may use your version
of this file under either the MPL or the GPL.

$Id: SynExportTeX.pas,v 1.1 2005/06/20 03:13:20 zhujianqiu Exp $

You may retrieve the latest version of this file at the SynEdit home page,
located at http://SynEdit.SourceForge.net

Known Issues:
-------------------------------------------------------------------------------}

{$IFNDEF QSYNEXPORTTEX}
unit SynExportTeX;
{$ENDIF}

{$I SynEdit.inc}

interface

uses
{$IFDEF SYN_CLX}
  Qt,
  QGraphics,
  QSynEditExport,
  QSynEditHighlighter,
{$ELSE}
  Windows,
  Graphics,
  SynEditExport,
  SynEditHighlighter,
{$ENDIF}
  Classes;

type
  TSynExporterTeX = class(TSynCustomExporter)
  private
    fMargin: integer;
    fLastToken: TSynHighlighterAttributes;
    function GetNewCommands: string;
    function AttriStyleName(const AName: string): string;
  protected
    fCreateTeXFragment: boolean;
    fTabWidth: integer;
    fPageStyleEmpty: boolean;
    procedure FormatNewLine; override;
    function GetFooter: string; override;
    function GetFormatName: string; override;
    function GetHeader: string; override;
    procedure SetTokenAttribute(IsSpace: Boolean; Attri: TSynHighlighterAttributes); override;
    procedure FormatToken(Token: String); override;
    // overriding these as they are abstract, and though will never be called,
    // this will prevent abstract instance warnings 
    procedure FormatAfterLastAttribute; override;
    procedure FormatAttributeDone(BackgroundChanged: Boolean;
      ForegroundChanged: Boolean; FontStylesChanged: TFontStyles); override;
    procedure FormatAttributeInit(BackgroundChanged: Boolean;
      ForegroundChanged: Boolean; FontStylesChanged: TFontStyles); override;
    procedure FormatBeforeFirstAttribute(BackgroundChanged: Boolean;
      ForegroundChanged: Boolean; FontStylesChanged: TFontStyles); override;
  public
    constructor Create(AOwner: TComponent); override;
  published
    property Margin: integer read fMargin write fMargin default 2;
    property TabWidth: integer read fTabWidth write fTabWidth default 2;
    property Color;
    property CreateTeXFragment: boolean read fCreateTeXFragment
      write fCreateTeXFragment default false;
    property PageStyleEmpty: boolean read fPageStyleEmpty write fPageStyleEmpty
      default false;
    property DefaultFilter;
    property Font;
    property Highlighter;
    property Title;
    property UseBackground;
  end;

implementation

uses
{$IFDEF SYN_CLX}
  QSynEditMiscProcs,
  QSynEditStrConst,
  QSynHighlighterMulti,
{$ELSE}
  SynEditStrConst,
  SynHighlighterMulti,
{$ENDIF}
  SysUtils;

const
{$IFDEF SYN_WIN32}
  NewLine = #13#10;
{$ELSE}
  NewLine = #10;
{$ENDIF}

function ReplaceDecSep(const AIn: string): string;
var
  p: PChar;
begin
  // LaTeX uses a Dot as decimal separator, in some languages a comma is used.
  p := PChar(AIn);
  while p^ <> #0 do begin
    if p^ = #44 then
      Result := Result + #46
    else
      Result := Result + p^;
    Inc(p);
  end;
end;

function ColorToTeX(AColor: TColor): string;
const
  f = '%1.2g';
  f2 = '%s,%s,%s';
var
  RGBColor: longint;
  RValue, GValue, BValue: string;
begin
  RGBColor := ColorToRGB(AColor);
  RValue := ReplaceDecSep(Format(f, [GetRValue(RGBColor) / 255]));
  GValue := ReplaceDecSep(Format(f, [GetGValue(RGBColor) / 255]));
  BValue := ReplaceDecSep(Format(f, [GetBValue(RGBColor) / 255]));
  Result := Format(f2, [RValue, GValue, BValue]);
end;

{ TSynExporterTeX }

constructor TSynExporterTeX.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  fMargin := 2;
  fTabWidth := 2;
  fPageStyleEmpty := false;
  fDefaultFilter := SYNS_FilterTeX;
  // setup array of chars to be replaced
  fReplaceReserved['{'] := '\{';
  fReplaceReserved['}'] := '\}';
  fReplaceReserved['\'] := '\BS ';
  fReplaceReserved['~'] := '\TLD ';
  fReplaceReserved['^'] := '\CIR ';
  fReplaceReserved[' '] := '\SPC ';
  fReplaceReserved[#9]  := '\TAB ';
  fReplaceReserved['-'] := '\HYP ';
  fReplaceReserved['"'] := '\QOT '; 
  fReplaceReserved['@'] := '$@$';
  fReplaceReserved['$'] := '\$';
  fReplaceReserved['&'] := '\&';
  fReplaceReserved['<'] := '$<$';
  fReplaceReserved['>'] := '$>$';
  fReplaceReserved['_'] := '\_';
  fReplaceReserved['#'] := '\#';
  fReplaceReserved['%'] := '\%';
end;

procedure TSynExporterTeX.SetTokenAttribute(IsSpace: Boolean;
  Attri: TSynHighlighterAttributes);
begin
  fLastToken := Attri;
end;

procedure TSynExporterTeX.FormatToken(Token: String);
const
  SToken = '\hspace*{%sem}';
  f = '%1.1g';

  function FindHighFromAttri(Attri: TSynHighlighterAttributes): TSynCustomHighlighter;
  var
    i, j: integer;
  begin
    Result := Highlighter;
    with (Highlighter as TSynMultiSyn) do begin
      for j := 0 to DefaultHighlighter.AttrCount - 1 do
        if DefaultHighlighter.Attribute[j] = Attri then begin
          Result := DefaultHighlighter;
          Exit;
        end;
      for i := 0 to Schemes.Count - 1 do
        for j := 0 to Schemes[i].Highlighter.AttrCount - 1 do
          if Schemes[i].Highlighter.Attribute[j] = Attri then begin
            Result := Schemes[i].Highlighter;
            Exit;
          end;
    end;
  end;

begin
  if not (Highlighter is TSynMultiSyn) then
    AddData(Format('\%s{%s}', [AttriStyleName(fLastToken.Name), Token]))
  else
    AddData(Format('\%s{%s}', [AttriStyleName(
      FindHighFromAttri(fLastToken).LanguageName + fLastToken.Name), Token]));
end;

procedure TSynExporterTeX.FormatNewLine;
begin
  AddData('\\' + NewLine);
end;

 // do nothing with these
procedure TSynExporterTeX.FormatAfterLastAttribute;
begin
end;

procedure TSynExporterTeX.FormatAttributeDone;
begin
end;

procedure TSynExporterTeX.FormatAttributeInit;
begin
end;

procedure TSynExporterTeX.FormatBeforeFirstAttribute;
begin
end;

function TSynExporterTeX.GetFooter: string;
begin
  if not fCreateTeXFragment then
    Result := NewLine + '\end{ttfamily}' + NewLine + '\end{document}'
  else
    Result := NewLine + '\end{ttfamily}';
end;

function TSynExporterTeX.GetFormatName: string;
begin
  Result := SYNS_LangTeX;
end;

// the purpose of this function is to leave only valid characters in AName
function TSynExporterTeX.AttriStyleName(const AName: string): string;
var
  pCurr: PChar;
begin
  Result := '';
  pCurr := PChar(AName);

  while pCurr^ <> #0 do begin
    if pCurr^ in ['A'..'Z', 'a'..'z'] then
      Result := Result + pCurr^;
    Inc(pCurr);
  end;
end;

function TSynExporterTeX.GetNewCommands: string;
const
  FixedCommands = '%% Special Characters' + NewLine +
                  '\newcommand\SPC{\hspace*{0.6em}}' + NewLine +
                  '\newcommand\TAB{\hspace*{%sem}}' + NewLine +
                  '\newcommand\BS{\mbox{\char 92}}' + NewLine +   // Backslash
                  '\newcommand\TLD{\mbox{\char 126}}' + NewLine + // ~
                  '\newcommand\CIR{\mbox{\char 94}}' + NewLine  + // ^
                  '\newcommand\HYP{\mbox{\char 45}}' + NewLine  + // a simple -
                  '\newcommand\QOT{\mbox{\char 34}}' + NewLine  + // "
                  '\newcommand{\uln}[1]{\bgroup \markoverwith{\hbox{\_}}\ULon{{#1}}}' + NewLine +
                  '%% Highlighter Attributes' + NewLine;
  NewCommand    = '\newcommand{\%s}[1]{%s%s%s%s%s#1%s}' + NewLine;
  SBold         = '\textbf{';
  SItalic       = '\textit{';
  SUnderline    = '\uln{';
  SColor        = '\textcolor[rgb]{%s}{';
  SBackColor    = '\colorbox[rgb]{%s}{';
  f = '%1.1g';

  function GetAttriCommand(AHigh: TSynCustomHighlighter; Attri: TSynHighlighterAttributes): string;
  var
    bold, italic, underline, color, backcol: string;
    b: integer;
  begin
    with Attri do begin
      b := 0;
      if fsBold in Style then begin
        bold := SBold;
        Inc(b);
      end else
        bold := '';
      if fsItalic in Style then begin
        italic := SItalic;
        Inc(b);
      end else
        italic := '';
      if fsUnderline in Style then begin
        underline := SUnderline;
        Inc(b);
      end else
        underline := '';
      if (Foreground <> clBlack) and (Foreground <> clNone)  then begin
        color := Format(SColor, [ColorToTeX(Foreground)]);
        Inc(b);
      end else
        color := '';
      if fUseBackground and (Background <> clNone) then begin
        backcol := Format(SBackColor, [ColorToTeX(Background)]);
        Inc(b);
      end else
        backcol := '';
      if not (Highlighter is TSynMultiSyn) then
        Result := Format(NewCommand, [AttriStyleName(Name), bold, italic, color,
          backcol, underline, StringOfChar('}', b)])
      else
        Result := Format(NewCommand, [AttriStyleName(AHigh.LanguageName + Name),
          bold, italic, color, backcol, underline, StringOfChar('}', b)]);
    end;
  end;

  function GetHighlighterCommand(AHigh: TSynCustomHighlighter): string;
  var
    i: integer;
  begin
    Result := '';
    for i := 0 to AHigh.AttrCount - 1 do
      Result := Result + GetAttriCommand(AHigh, AHigh.Attribute[i]);
  end;

var
  i: integer;
  tw: string;

begin
  tw := ReplaceDecSep(Format(f, [fTabWidth * 0.6]));
  Result := Format(FixedCommands, [tw]);
  if (Highlighter is TSynMultiSyn) then begin
    with (Highlighter as TSynMultiSyn) do begin
      Result := Result + GetAttriCommand(Highlighter, Attribute[0]); // Marker
      Result := Result + GetHighlighterCommand(DefaultHighlighter);
      for i := 0 to Schemes.Count - 1 do
        Result := Result + GetHighlighterCommand(Schemes[i].Highlighter)
    end;
  end else
    Result := Result + GetHighlighterCommand(Highlighter);
end;

function TSynExporterTeX.GetHeader: string;
const
  TeXHeader   = '\documentclass[a4paper, %dpt]{article}' + NewLine +
                '\usepackage[a4paper, margin=%dcm]{geometry}' + NewLine +
                '\usepackage[T1]{fontenc}' + NewLine +
                '\usepackage{color}' + NewLine +
                '\usepackage{alltt}' + NewLine +
                '\usepackage{times}' + NewLine +
                '\usepackage{ulem}' + NewLine +
{$IFDEF WIN32}
                // It is recommennded to use AnsiNew on Windows
                '\usepackage[ansinew]{inputenc}' + NewLine +
{$ELSE}
                // and Latin1 on UNIX Systems, see also DE FAQ 8.5.3
                '\usepackage[latin1]{inputenc}' + NewLine +
{$ENDIF}
                '%s' + NewLine + // New Commands
                '\title{%s}' + NewLine +
                '%% Generated by SynEdit TeX exporter' + NewLine + NewLine +
                '\begin{document}%s';
  EmptyPage   = '\pagestyle{empty}';
  TeXDocument = '\begin{ttfamily}' + NewLine +
                '\noindent' + NewLine;
var
  PageStyle: string;
begin
  if not fCreateTeXFragment then begin
    if fPageStyleEmpty then
      PageStyle := NewLine + EmptyPage
    else
      PageStyle := '';
    Result := Format(TeXHeader + NewLine + NewLine,
      [Font.Size, fMargin, GetNewCommands, Title, PageStyle]);
  end;
  Result := Result + TeXDocument;
end;

end.

