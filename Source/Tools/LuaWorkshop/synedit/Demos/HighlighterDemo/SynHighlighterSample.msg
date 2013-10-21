TSynLuaSyn   	{first Identifier is considered to be the Class Name }
tk              {second Identifier is considered to be the Identifier Prefix }
IdentStart '_', 'a'..'z', 'A'..'Z':: '_', '0'..'9', 'a'..'z', 'A'..'Z'::

TOKENTYPES
Identifier
Comment Style=[fsItalic]|Foreground=clNavy
Space
String Foreground=clRed
Key Style=[fsBold]
Test Background=clSilver|Foreground=clBlue|Style=[fsUnderline, fsItalic]
|><|

KEYS
Hello
World
|><|

KEYS Test
SynEdit
|><|

CHARS

'A'..'Z', 'a'..'z', '_':: Ident
BeginProc
  fTokenID := IdentKind((fLine + Run));
  inc(Run, fStringLen);
  while Identifiers[fLine[Run]] do
    Inc(Run);
EndProc

|><|

ENCLOSEDBY

Comment,BraceComment,{,},MultiLine
Comment,CStyleComment,/*,*/,MultiLine
String,String,","
|><|

