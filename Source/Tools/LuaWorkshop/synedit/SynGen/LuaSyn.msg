{Grammar for a C++ HighLighter}
{Copyright 1998 Michael Trier.  All right reserved}
{mailto:mtrier@pobox.com}{No syntax check takes Place}
{ DISCLAIMER:  This is provided as is, expressly without a warranty of any kind.}
{ You use it at your own risc. }

TLuaSyn   {first Identifier is considered to be the Class Name }
tk        {second Identifier is considered to be the Identifier Prefix }
Sensitive {: Optional}
IdentStart '_', 'a'..'z', 'A'..'Z':: '_', '0'..'9', 'a'..'z', 'A'..'Z'::

KEYS  { all between KEYS and |><| is considered to be a keyword }
and
do
else 
elseif 
end
function
if
local
nil 
not
or
repeat 
return
then
until
while

|><| { token names }
Asm
Comment
Directive
Identifier
Key
Null
Number
Space
String
Symbol
Unknown
|><|

CHARS

#0:: Null
BeginProc
  fTokenID := tkNull;
  fEol := True;
EndProc

#10:: LF
BeginProc
  fTokenID := tkSpace;
  inc(Run);
EndProc

#13:: CR
BeginProc
  fTokenID := tkSpace;
  Case FLine[Run + 1] of
    #10: inc(Run, 2);
  else inc(Run);
  end;
EndProc

#1..#9, #11, #12, #14..#32:: Space
BeginProc
  inc(Run);
  fTokenID := tkSpace;
  while FLine[Run] in [#1..#9, #11, #12, #14..#32] do inc(Run);
EndProc

#34::  String
BeginProc
  fTokenID := tkString;
  if (FLine[Run + 1] = #34) and (FLine[Run + 2] = #34) then inc(Run, 2);
  repeat
    case FLine[Run] of
      #0, #10, #13: break;
      #92:
        if FLine[Run + 1] = #10 then inc(Run);
    end;
    inc(Run);
  until FLine[Run] = #34;
  if FLine[Run] <> #0 then inc(Run);
EndProc

#39::  AsciiChar
BeginProc
  fTokenID := tkString;
  repeat
    case FLine[Run] of
      #0, #10, #13: break;
    end;
    inc(Run);
  until FLine[Run] = #39;
  if FLine[Run] <> #0 then inc(Run);
EndProc

'0'..'9':: Number
BeginProc
  inc(Run);
  fTokenID := tkNumber;
  while FLine[Run] in
      ['0'..'9', '.', 'u', 'U', 'l', 'L', 'x', 'X', 'e', 'E', 'f', 'F'] do
  begin
    case FLine[Run] of
      '.':
        if FLine[Run + 1] = '.' then break;
    end;
    inc(Run);
  end;
EndProc

'A'..'Z', 'a'..'z', '_':: Ident
BeginProc
  fTokenID := IdentKind((fLine + Run));
  inc(Run, fStringLen);
  while Identifiers[fLine[Run]] do inc(Run);
EndProc

'!':: NotSymbol
BeginProc
  case FLine[Run + 1] of
    '=':                               {not equal}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
  else                                 {not}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
EndProc

'#':: Directive
BeginProc
  fTokenID := tkDirective;
  repeat
    case FLine[Run] of
      #0, #10, #13: break;
    end;
    inc(Run);
  until FLine[Run] = #0;
EndProc

'{':: BraceOpen
BeginProc
  inc(Run);
  fTokenId := tkSymbol;
EndProc

'}':: BraceClose
BeginProc
  inc(Run);
  fTokenId := tkSymbol;
EndProc

'~':: Tilde
BeginProc
  inc(Run);
  fTokenId := tkSymbol;
EndProc

'|':: OrSymbol
BeginProc
  case FLine[Run + 1] of
    '=':                               {or assign}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
    '|':                               {logical or}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
  else                                 {or}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
EndProc

'%':: ModSymbol
BeginProc
  case FLine[Run + 1] of
    '=':                               {mod assign}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
  else                                 {mod}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
EndProc

'&':: AndSymbol
BeginProc
  case FLine[Run + 1] of
    '=':                               {and assign}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
    '&':                               {logical and}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
  else                                 {and}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
EndProc

'(':: RoundOpen
BeginProc
  inc(Run);
  FTokenID := tkSymbol;
  inc(FRoundCount);
EndProc

')':: RoundClose
BeginProc
  inc(Run);
  fTokenID := tkSymbol;
  dec(FRoundCount);
EndProc

'*':: Star
BeginProc
  case FLine[Run + 1] of
    '=':                               {multiply assign}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
  else                                 {star}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
EndProc

'+':: Plus
BeginProc
  case FLine[Run + 1] of
    '=':                               {add assign}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
    '+':                               {increment}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
  else                                 {subtract}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
EndProc

',':: Comma
BeginProc
  inc(Run);
  fTokenID := tkSymbol;
EndProc

'-':: Minus
BeginProc
  case FLine[Run + 1] of
    '=':                               {subtract assign}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
    '-':                               {decrement}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
    '>':                               {arrow}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
  else                                 {subtract}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
EndProc

'.':: Point
BeginProc
  if (FLine[Run + 1] = '.') and (FLine[Run + 2] = '.') then
    begin                              {ellipse}
      inc(Run, 3);
      fTokenID := tkSymbol;
    end
  else                                 {point}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
EndProc

'-':: Slash
BeginProc
  Inc(Run);
  if fLine[Run] = '-' then
  begin
    fTokenID := tkComment;
    repeat
      Inc(Run);
    until fLine[Run] in [#0, #10, #13];
  end else
    fTokenID := tkUnknown;
EndProc

'/':: Slash
BeginProc
  case FLine[Run + 1] of
    '=':                               {division assign}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
  else                                 {division}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
EndProc

':':: Colon
BeginProc
  Case FLine[Run + 1] of
    ':':                               {scope resolution operator}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
  else                                 {colon}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
EndProc

';':: SemiColon
BeginProc
  inc(Run);
  fTokenID := tkSymbol;
EndProc

'<':: Lower
BeginProc
  case FLine[Run + 1] of
    '=':                               {less than or equal to}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
    '<':
      begin
        if FLine[Run + 2] = '=' then   {shift left assign}
          inc(Run, 3)
        else                           {shift left}
          inc(Run, 2);
        fTokenID := tkSymbol;
      end;
  else                                 {less than}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
EndProc

'=':: Equal
BeginProc
  case FLine[Run + 1] of
    '=':                               {logical equal}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
  else                                 {assign}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
EndProc

'>':: Greater
BeginProc
  Case FLine[Run + 1] of
    '=':                               {greater than or equal to}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
    '>':
      begin
        if FLine[Run + 2] = '=' then   {shift right assign}
          inc(Run, 3)
        else                           {shift right}
          inc(Run, 2);
        fTokenID := tkSymbol;
      end;
  else                                 {greater than}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
EndProc

'@':: AtSymbol
BeginProc
  fTokenID := tkSymbol;
  inc(Run);
EndProc

'[':: SquareOpen
BeginProc
  inc(Run);
  fTokenID := tkSymbol;
  inc(FSquareCount);
EndProc

']':: SquareClose
BeginProc
  inc(Run);
  fTokenID := tkSymbol;
  dec(FSquareCount);
EndProc

'^':: XOrSymbol
BeginProc
  Case FLine[Run + 1] of
    '=':                               {xor assign}
      begin
        inc(Run, 2);
        fTokenID := tkSymbol;
      end;
  else                                 {xor}
    begin
      inc(Run);
      fTokenID := tkSymbol;
    end;
  end;
EndProc
|><|

