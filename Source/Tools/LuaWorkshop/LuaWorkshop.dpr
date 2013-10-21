program LuaWorkshop;

uses
  Windows,
  SysUtils,
  Messages,
  Forms,
  MainForm in 'MainForm.pas' {MainEditor},
  Debugger in 'Debugger.pas',
  ProcessList in 'ProcessList.pas' {ProcessListForm},
  SynLua in 'SynLua.pas',
  Search in 'Search.pas' {SearchDlg},
  About in 'About.pas' {AboutForm},
  Breakpoint in 'Breakpoint.pas';

{$R *.res}

var
  Head, Rear:integer;

  psCaption:PChar;
  hDesktop:HWND;
  hTemp:HWND;
  Counter:integer;
  hApp:HWND;
  OldProp:integer;
  Dummy:cardinal;
  ProcessID:cardinal;
  hProcess:THandle;
  pFileName:PChar;

begin
  CommandLine:=GetCommandLine;
  if (CommandLine <> '') then
    begin
      Head:=Length(CommandLine) - 1;
      while (CommandLine[Head] <> '"') and
            (Head > 0) do
        dec(Head);
      Rear:=Length(CommandLine);
      if (CommandLine[Length(CommandLine)] = '"') then
        begin
          Rear:=Rear - 1;
          Head:=Head + 1;
        end
       else
        begin
          Head:=Head + 2;
        end;
      if (Head > 0) then
        begin
          CommandLine:=Copy(CommandLine, Head, Rear - Head + 1);
        end;
    end;

  psCaption:=AllocMem(255);
  hDesktop:=GetDeskTopWindow();
  hTemp:=GetTopWindow(hDesktop);
  Counter:=0;
  repeat
    ZeroMemory(psCaption, 255);
    GetWindowText(hTemp,psCaption,255);
    if (Copy(psCaption, 1, Length(TitleStr)) = TitleStr) then
      begin
        Counter:=Counter + 1;
        hApp:=hTemp;
      end;
    hTemp:=GetNextWindow(hTemp,GW_HWNDNEXT);
  until (hTemp=0);
  FreeMem(psCaption);

  if Counter >= 1 then
    begin
      GetWindowThreadProcessId(hApp, ProcessID);
      AllowSetForegroundWindow(ProcessID);
      hProcess:=OpenProcess(PROCESS_ALL_ACCESS, true, ProcessID);
      VirtualProtectEx(hProcess, @gFileName, 256, PAGE_READWRITE, @OldProp);
      WriteProcessMemory(hProcess, @gFileName, PChar(CommandLine), 256, Dummy);
      VirtualProtectEx(hProcess, @gFileName, 256, OldProp, @Dummy);

      PostMessage(hApp, WM_USER + 1, 0, 0);
    end
   else
    begin
      Application.Initialize;
      Application.Title := 'Lua Workshop';
      Application.CreateForm(TMainEditor, MainEditor);
      Application.CreateForm(TProcessListForm, ProcessListForm);
      Application.CreateForm(TSearchDlg, SearchDlg);
      Application.CreateForm(TAboutForm, AboutForm);
      Application.Run;
    end;
end.
