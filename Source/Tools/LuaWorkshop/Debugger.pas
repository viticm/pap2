unit Debugger;

interface
uses ExtCtrls;
const
  MAX_LINE = 1024;
  MAX_VARIABLE = 256;

type
  TDebugCommand = (DBC_NONE = 0,
	  DBC_CONTINUE,
	  DBC_GET_LOCAL_VAR,
    DBC_SET_LOCAL_VAR,
    DBC_SET_GLOBAL_VAR,
    DBC_RELOAD_SCRIPT,
	  DBC_INACTIVE = $FEDC);

  TLuaVarType = (LUA_TNONE = -1,
    LUA_TUSERDATA = 0,
    LUA_TNIL,
    LUA_TNUMBER,
    LUA_TSTRING,
    LUA_TTABLE,
    LUA_TFUNCTION);



  TDebugMessage = record
		szFlag:   array [1..4] of char;
		dwCommand:integer;
		dwParam1: integer;
		dwParam2: integer;
		dwParam3: integer;
		dwParam4: integer;
		szParam1: array [0..255] of char;
		szParam2: array [0..255] of char;
  end;

  TDebugInfo = record
		szFlag        :array [1..4] of char;
    dwThis        :integer;
		dwLineCount   :integer;
    dwBreakCount  :integer;
    dwErrorCount  :integer;
    lpFileName    :array [0..15] of integer;
    lpFuncName    :integer;
    lpFuncType    :array [0..15] of integer;
    dwLineCounts  :array [0..15] of integer;
    dwStackLevel  :integer;
	  dwVarCount    :integer;
    dwGlobalTotal :integer;
    dwLocalTotal  :integer;
	  VarNames      :array [1..MAX_VARIABLE, 1..32] of char;
	  VarTypes      :array [1..MAX_VARIABLE] of char;
	  VarIntValues  :array [1..MAX_VARIABLE] of integer;
	  VarStrValues  :array [1..MAX_VARIABLE, 1..256] of char;
    szErrorMsg    :array [1..256] of char;
  end;

  FOnNewFile = procedure (FileName:string) of object;
  FOnNewFunc = procedure (FuncName:string) of object;
  FOnNewLine = procedure (LineCount:integer) of object;
  FOnNewError = procedure (ErrorMsg:string;LineCount:integer) of object;
  FOnNewVar = procedure (VarIndex:integer; VarName:PChar; VarType:TLuaVarType;
      VarIntValue:integer; VarStrValue:PChar) of object;
  FOnBeginRun = procedure (LineCount:integer) of object;
  FOnBeginBreak = procedure (LineCount:integer) of object;
  FOnNewStack = procedure (StackLevel:integer; FileName:string; 
      FuncType:string; LineCount:integer) of object;

  TDebugger = class(TObject)
  public
    OnNewFile:FOnNewFile;
    OnNewFunc:FOnNewFunc;
    OnNewLine:FOnNewLine;
    OnNewError:FOnNewError;
    OnNewGlobalVar:FOnNewVar;
    OnNewLocalVar:FOnNewVar;
    OnBeginRun:FOnBeginRun;
    OnBeginBreak:FOnBeginBreak;
    OnNewStack:FOnNewStack;

    constructor Create;
    destructor Destroy;virtual;

    function Attach(ProcessID:integer;StartAddr:integer):boolean;
    function Detach:boolean;
    function GetFileName:string;

    procedure Run;
    procedure Break;
    procedure Step;
    procedure SetBreakpoint(Line:integer;Enabled:boolean);
    function GetBreakpoint(Line:integer):boolean;
    procedure SetGlobalValue(ValueName:string; ValueType:TLuaVarType;NewValue:string);
    procedure SetLocalValue(ValueName:string; ValueType:TLuaVarType;NewValue:string);
    procedure ReloadScript;
  private
    m_CommandAddr   :integer;
    m_InfoAddr      :integer;
    m_ProcessID     :integer;
    m_hProcess      :THandle;

    m_BreakCount    :integer;
    m_ErrorCount    :integer;
    m_CurrentLine   :integer;
    m_NextLine      :integer;

    m_IsRunning     :boolean;
    m_IsAttached    :boolean;

    m_CurrentFile   :array [0..255] of char;
    m_CurrentFunc   :array [0..255] of char;

    m_Breakpoints   :array [0..MAX_LINE - 1] of boolean;

    m_VarCount      :integer;
    m_Timer         :TTimer;

    function SendMessage(Msg:TDebugMessage):boolean;
    function GetDebugInfo(var DebugInfo:TDebugInfo):boolean;
    procedure OnTime(Sender:TObject);
    function WriteData(Address:integer;Data:PChar; Size:integer):boolean;
    function ReadData(Address:integer;Data:PChar; Size:integer):boolean;
  end;

implementation
uses Windows, Dialogs, SysUtils, Forms;

function TDebugger.WriteData(Address:integer;Data:PChar; Size:integer):boolean;
var
  OldProp:integer;
  Dummy:cardinal;
begin
	VirtualProtectEx(m_hProcess, Pointer(Address), Size, PAGE_READWRITE, @OldProp);
	Result:=WriteProcessMemory(m_hProcess, Pointer(Address), Pointer(Data), Size, Dummy);
	VirtualProtectEx(m_hProcess, Pointer(Address), Size, OldProp, @Dummy);
end;

function TDebugger.ReadData(Address:integer;Data:PChar; Size:integer):boolean;
var
  OldProp:integer;
  Dummy:cardinal;
begin
	VirtualProtectEx(m_hProcess, Pointer(Address), Size, PAGE_READWRITE, @OldProp);
	Result:=ReadProcessMemory(m_hProcess, Pointer(Address), Pointer(Data), Size, Dummy);
	VirtualProtectEx(m_hProcess, Pointer(Address), Size, OldProp, @Dummy);
end;

constructor TDebugger.Create();
begin
  m_CommandAddr:=0;
  m_InfoAddr:=0;
  m_ProcessID:=0;
  m_hProcess:=0;
  m_CurrentLine:=0;
  m_NextLine:=0;
  m_IsRunning:=true;
  m_IsAttached:=false;
  m_VarCount:=0;
  m_Timer:=TTimer.Create(nil);
  m_Timer.Interval:=100;
  m_Timer.Enabled:=false;
  m_Timer.OnTimer:=OnTime;
end;

destructor TDebugger.Destroy();
begin
  Detach;
end;

function TDebugger.Attach(ProcessID:integer;StartAddr:integer):boolean;
var
  bFinished:boolean;
  Data:array [1..4] of char;
  Command:integer;
  Counter:integer;
begin
  m_ProcessID:=ProcessID;
  m_hProcess:=OpenProcess(PROCESS_ALL_ACCESS, false, ProcessID);
  bFinished:=false;
  Counter:=$FFFFFF;
  while (not bFinished) and (Counter <> 0) do
    begin
      ReadData(StartAddr, PChar(@Data), 4);
      if Data = 'LDMS' then
        bFinished:=true
      else
        StartAddr:=StartAddr + 4;
      dec(Counter)
    end;
  m_CommandAddr:=StartAddr;

  bFinished:=false;
  Counter:=$FFFFFF;
  while (not bFinished) and (Counter <> 0) do
    begin
      ReadData(StartAddr, PChar(@Data), 4);
      if Data = 'LDIS' then
        bFinished:=true
      else
        StartAddr:=StartAddr + 4;
      dec(Counter)
    end;
  m_InfoAddr:=StartAddr;

  Command:=0;
  WriteData(m_CommandAddr + 4, @Command, 4);
  m_IsAttached:=true;
  m_Timer.Enabled:=true;
  Result:=true;
end;

function TDebugger.Detach:boolean;
var
  Command:integer;
begin
  Result:=false;
  if (m_IsAttached) then
    begin
      m_Timer.Enabled:=false;
      Sleep(10);
      Command:=integer(DBC_INACTIVE);
      WriteData(m_CommandAddr + 4, @Command, 4);
      CloseHandle(m_hProcess);
      m_IsAttached:=false;
      Result:=true;
    end;

end;

function TDebugger.GetFileName:string;
begin
  Result:=m_CurrentFile;
end;

procedure TDebugger.Run;
begin
  if (m_IsAttached) then
    begin
      m_IsRunning:=true;
      if Assigned(OnBeginRun) then
        OnBeginRun(m_CurrentLine);
    end;
end;

procedure TDebugger.Break;
begin
  if (m_IsAttached) then
    begin
      m_IsRunning:=false;
      if Assigned(OnBeginBreak) then
        OnBeginBreak(m_CurrentLine);      
    end;
end;

procedure TDebugger.Step;
begin
  if (not m_IsRunning and m_IsAttached) then
    begin
      m_NextLine:=m_NextLine + 1;
    end;
end;

function TDebugger.SendMessage(Msg:TDebugMessage):boolean;
begin
  WriteData(m_CommandAddr + 8, @(Msg.dwParam1), sizeof(TDebugMessage) - 8);
  Result:=WriteData(m_CommandAddr + 4, @(Msg.dwCommand), 4);
end;

function TDebugger.GetDebugInfo(var DebugInfo:TDebugInfo):boolean;
begin
  Result:=ReadData(m_InfoAddr, @DebugInfo, sizeof(TDebugInfo));
end;

procedure TDebugger.SetBreakpoint(Line:integer;Enabled:boolean);
begin
  m_Breakpoints[Line]:=Enabled;
end;

function TDebugger.GetBreakpoint(Line:integer):boolean;
begin
  Result:=m_Breakpoints[Line];
end;

procedure TDebugger.SetLocalValue(ValueName:string; ValueType:TLuaVarType;NewValue:string);
var
  DebugMessage:TDebugMessage;
begin
  DebugMessage.dwCommand:=integer(DBC_SET_Local_VAR);
  ZeroMemory(@DebugMessage.szParam1, MAX_VARIABLE);
  CopyMemory(@DebugMessage.szParam1, @ValueName[1], Length(ValueName));
  DebugMessage.dwParam1:=integer(ValueType);
  case ValueType of
    LUA_TNUMBER:
      begin
        DebugMessage.dwParam2:=StrToInt(NewValue);
      end;
    LUA_TSTRING:
      begin
        ZeroMemory(@DebugMessage.szParam2, MAX_VARIABLE);
        CopyMemory(@DebugMessage.szParam2, @NewValue[1], Length(NewValue));
      end;
  end;
  SendMessage(DebugMessage);
end;

procedure TDebugger.SetGlobalValue(ValueName:string; ValueType:TLuaVarType;NewValue:string);
var
  DebugMessage:TDebugMessage;
begin
  DebugMessage.dwCommand:=integer(DBC_SET_GLOBAL_VAR);
  ZeroMemory(@DebugMessage.szParam1, MAX_VARIABLE);
  CopyMemory(@DebugMessage.szParam1, @ValueName[1], Length(ValueName));
  DebugMessage.dwParam1:=integer(ValueType);
  case ValueType of
    LUA_TNUMBER:
      begin
        DebugMessage.dwParam2:=StrToInt(NewValue);
      end;
    LUA_TSTRING:
      begin
        ZeroMemory(@DebugMessage.szParam2, MAX_VARIABLE);
        CopyMemory(@DebugMessage.szParam2, @NewValue[1], Length(NewValue));
      end;
  end;
  SendMessage(DebugMessage);
end;

procedure TDebugger.ReloadScript;
var
  DebugMessage:TDebugMessage;
begin
  if (m_IsAttached) then
    begin
      DebugMessage.dwCommand:=integer(DBC_RELOAD_SCRIPT);
      SendMessage(DebugMessage);
      ShowMessage('Reload successfully!');
    end;
end;

procedure TDebugger.OnTime(Sender:TObject);
var
  i:integer;
  DebugInfo:TDebugInfo;
  DebugMessage:TDebugMessage;
  FileName: array [0..255] of char;
  FuncName: array [0..255] of char;
  FuncType: array [0..255] of char;
begin
try
  GetDebugInfo(DebugInfo);
  if (m_BreakCount < DebugInfo.dwBreakCount) then
    begin
      for i:=1 to DebugInfo.dwStackLevel do
        begin
          ReadData(DebugInfo.lpFileName[i-1], FileName, 256);
          ReadData(DebugInfo.lpFuncType[i-1], FuncType, 256);
          if Assigned(OnNewStack) then
            OnNewStack(i, FileName, FuncType, DebugInfo.dwLineCounts[i-1]);
        end;
      if (DebugInfo.lpFileName[0] <> 0) then
        begin
          ReadData(DebugInfo.lpFileName[0], FileName, 256);
          if (m_CurrentFile <> FileName) then
            begin
              CopyMemory(@m_CurrentFile, @FileName, 256);
              if Assigned(OnNewFile) then
                OnNewFile(m_CurrentFile);
              Break;
            end;
        end;
      if (DebugInfo.lpFuncName <> 0) then
        begin
          ReadData(DebugInfo.lpFuncName, FuncName, 256);
          if (m_CurrentFunc <> FuncName) then
            begin
              CopyMemory(@m_CurrentFunc, @FuncName, 256);
              if Assigned(OnNewFunc) then
                OnNewFunc(m_CurrentFunc);
            end;
        end;

      m_CurrentLine:=DebugInfo.dwLineCount;
      if Assigned(OnNewLine) then
        OnNewLine(m_CurrentLine);

      if (m_CurrentLine < MAX_LINE) then
        begin
          if (m_Breakpoints[m_CurrentLine]) then
            begin
              m_IsRunning:=false;
            end;
        end;

      if (not m_IsRunning) then
        begin
          DebugMessage.dwCommand:=integer(DBC_GET_LOCAL_VAR);
          SendMessage(DebugMessage);
        end;

      m_NextLine:=m_CurrentLine;
    end;

  if (m_IsRunning) then
    begin
      m_NextLine:=m_CurrentLine + 1;
    end;

  if (m_VarCount < DebugInfo.dwVarCount) and
     (not m_IsRunning) then
    begin
      if (Assigned(OnNewGlobalVar)) then
        for i:=1 to DebugInfo.dwGlobalTotal do
          begin
            OnNewGlobalVar(i, @DebugInfo.VarNames[i], TLuaVarType(DebugInfo.VarTypes[i]),
                           DebugInfo.VarIntValues[i], @DebugInfo.VarStrValues[i]);
          end;
      if (Assigned(OnNewLocalVar)) then
        for i:=1 to DebugInfo.dwLocalTotal do
          begin
            OnNewLocalVar(i, @DebugInfo.VarNames[DebugInfo.dwGlobalTotal + i],
                          TLuaVarType(DebugInfo.VarTypes[DebugInfo.dwGlobalTotal + i]),
                          DebugInfo.VarIntValues[DebugInfo.dwGlobalTotal + i],
                          @DebugInfo.VarStrValues[DebugInfo.dwGlobalTotal + i]);
          end;
      m_VarCount:=DebugInfo.dwVarCount;
    end;

  if (m_NextLine > m_CurrentLine) then
    begin
      DebugMessage.dwCommand:=integer(DBC_CONTINUE);
      SendMessage(DebugMessage);
      m_NextLine:=m_CurrentLine;
    end;

  if (m_ErrorCount < DebugInfo.dwErrorCount) then
    begin
      OnNewError(DebugInfo.szErrorMsg, DebugInfo.dwLineCount);
      m_ErrorCount := DebugInfo.dwErrorCount;
    end;

  m_BreakCount:=DebugInfo.dwBreakCount;
except
  on E:Exception do
    begin
      ShowMessage(E.Message);
      m_Timer.Enabled:=false;
    end;
end;
end;

end.
