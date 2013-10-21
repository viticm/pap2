unit MainForm;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ComCtrls, ExtCtrls, Menus, Grids, ValEdit, CheckLst,
  Debugger, SynEdit, SynMemo, SynCompletionProposal, SynEditHighlighter,
  SynHighlighterCpp, SynEditTypes, SynLua, SynEditKeyCmds,
  SynEditMiscClasses, SynEditSearch, SynEditAutoComplete;

const
  TitleStr = 'Lua Workshop Ver 0.4.1';
  
type
  TMainEditor = class(TForm)
    MainMenu1: TMainMenu;
    File1: TMenuItem;
    Debug1: TMenuItem;
    Help1: TMenuItem;
    Exit1: TMenuItem;
    OpenDialog1: TOpenDialog;
    SaveDialog1: TSaveDialog;
    InfoList: TValueListEditor;
    Attach1: TMenuItem;
    Step1: TMenuItem;
    Break1: TMenuItem;
    Run1: TMenuItem;
    Detach1: TMenuItem;
    About1: TMenuItem;
    GlobalVarList: TValueListEditor;
    LocalVarList: TValueListEditor;
    Save1: TMenuItem;
    Open1: TMenuItem;
    SynEditSearch1: TSynEditSearch;
    SynCompletionProposal1: TSynCompletionProposal;
    SynCompletionProposal2: TSynCompletionProposal;
    QuickAttach1: TMenuItem;
    Edit1: TMenuItem;
    Undo1: TMenuItem;
    Redo1: TMenuItem;
    N2: TMenuItem;
    Compile1: TMenuItem;
    Compile2: TMenuItem;
    Reload1: TMenuItem;
    N1: TMenuItem;
    N3: TMenuItem;
    Search1: TMenuItem;
    SearchAgain1: TMenuItem;
    Window1: TMenuItem;
    DebugWindow1: TMenuItem;
    TreeWindow1: TMenuItem;
    PageControl1: TPageControl;
    New1: TMenuItem;
    DefaultMemo: TSynMemo;
    TreeView1: TTreeView;
    Close1: TMenuItem;
    RefreshTree1: TMenuItem;
    N5: TMenuItem;
    PageControl2: TPageControl;
    TabSheet1: TTabSheet;
    Memo1: TMemo;
    TabSheet2: TTabSheet;
    TabSheet3: TTabSheet;
    WatchList: TValueListEditor;
    StackList: TListBox;
    Help2: TMenuItem;
    N4: TMenuItem;
    procedure Attach1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure Run1Click(Sender: TObject);
    procedure Break1Click(Sender: TObject);
    procedure Step1Click(Sender: TObject);
    procedure DefaultMemoGutterClick(Sender: TObject; Button: TMouseButton; X,
      Y, Line: Integer; Mark: TSynEditMark);
    procedure Detach1Click(Sender: TObject);
    procedure Exit1Click(Sender: TObject);
    procedure Open1Click(Sender: TObject);
    procedure Save1Click(Sender: TObject);
    procedure DefaultMemoKeyUp(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure QuickAttach1Click(Sender: TObject);
    procedure Undo1Click(Sender: TObject);
    procedure Redo1Click(Sender: TObject);
    procedure Compile2Click(Sender: TObject);
    procedure Search1Click(Sender: TObject);
    procedure SearchAgain1Click(Sender: TObject);
    procedure GlobalVarListKeyPress(Sender: TObject; var Key: Char);
    procedure LocalVarListKeyPress(Sender: TObject; var Key: Char);
    procedure Reload1Click(Sender: TObject);
    procedure DebugWindow1Click(Sender: TObject);
    procedure New1Click(Sender: TObject);
    procedure PageControl1Change(Sender: TObject);
    procedure PageControl1MouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure TreeView1DblClick(Sender: TObject);
    procedure TreeWindow1Click(Sender: TObject);
    procedure Close1Click(Sender: TObject);
    procedure RefreshTree1Click(Sender: TObject);
    procedure TreeView1Edited(Sender: TObject; Node: TTreeNode;
      var S: String);
    procedure About1Click(Sender: TObject);
    procedure WatchListDblClick(Sender: TObject);
    procedure WatchListDragOver(Sender, Source: TObject; X, Y: Integer;
      State: TDragState; var Accept: Boolean);
    procedure WatchListDragDrop(Sender, Source: TObject; X, Y: Integer);
    procedure StackListDblClick(Sender: TObject);
    procedure Help2Click(Sender: TObject);
  private
    SynLua1:TSynLua;
    procedure OnNewFile(FileName:string);
    procedure OnNewFunc(FuncName:string);
    procedure OnNewLine(LineCount:integer);
    procedure OnNewError(ErrorMsg:string;LineCount:integer);
    procedure OnNewGlobalVar(VarIndex:integer; VarName:PChar; VarType:TLuaVarType;
      VarIntValue:integer; VarStrValue:PChar);
    procedure OnNewLocalVar(VarIndex:integer; VarName:PChar; VarType:TLuaVarType;
      VarIntValue:integer; VarStrValue:PChar);
    procedure OnBeginRun(LineCount:integer);
    procedure OnBeginBreak(LineCount:integer);
    procedure OnNewStack(StackLevel:integer; FileName:string;
      FuncType:string; LineCount:integer);

    procedure OnSpecialLineColors(Sender: TObject; Line: integer;
                                  var Special: boolean; var FG, BG: TColor);
    { Private declarations }
  public
    procedure WndProc(var Message: TMessage);override;
    { Public declarations }
  end;

var
  MainEditor        : TMainEditor;
  LuaDebugger       : TDebugger;
  HostName          : string;
  EngineName        : string = 'engine.dll';
  CommandLine       : string;
  gFileName : array [0..255] of char;

procedure SwitchToThisWindow(WinHandle:HWND;AltTab:BOOL);stdcall;external 'user32.dll';
function AllowSetForegroundWindow(ProcessID:DWORD):BOOL;stdcall;external  'user32.dll';
  
implementation

uses ProcessList, TLHelp32, SynEditTextBuffer, Search, MMSystem, About;
var
  LastY:integer;
  OldLocalList      : TStringList;
  OldGlobalList     : TStringList;

  nSearchCount : integer = 0;
  nCurLine : integer = 0;
  nErrorLine : integer = 0;
  CurrentMemo : TSynMemo;
  RootPath : TStringList;

{$R *.dfm}

function CheckFileName(AFileName:string):boolean;
var
  i:integer;
  FileName:^string;
begin
  Result:=false;
  for i:=0 to MainEditor.PageControl1.PageCount - 1 do
    begin
      FileName:=Pointer(TSynMemo(MainEditor.PageControl1.Pages[i].Tag).Tag);
      if FileName^ = AFileName then
        begin
          MainEditor.PageControl1.ActivePageIndex:=i;
          Result:=true;
          exit;
        end;
    end;
end;

procedure ShowDir(Path:string; ParentNode:TTreeNode);
var
  SearchRec:TSearchRec;
  ret:integer;
  TreeNode:TTreeNode;
begin
  ret:=FindFirst(Path + '\*.*', faAnyFile, SearchRec);
  while ret=0 do
    begin
      if (SearchRec.Name <> '.') and (SearchRec.Name <> '..') then
        begin
//          ShowMessage('File Name : ' + SearchRec.Name + ' : ' + IntToStr(SearchRec.Attr));
          if ((SearchRec.Attr and faDirectory) > 0) or
             (ExtractFileExt(SearchRec.Name) = '.lua') then
            TreeNode:=MainEditor.TreeView1.Items.AddChild(ParentNode, SearchRec.Name);
          if (SearchRec.Attr and faDirectory) > 0 then
            begin
//              ShowMessage('Enter Dir');
              ShowDir(Path + '\' + SearchRec.Name, TreeNode);
            end;
        end;
      ret:=FindNext(SearchRec);
    end;

  FindClose(SearchRec);
end;

procedure RefreshTree;
var
  TreeNode:TTreeNode;
  i:integer;
begin
  MainEditor.TreeView1.Items.Clear;
  for i:=0 to RootPath.Count - 1 do
    begin
      TreeNode:=MainEditor.TreeView1.Items.Add(nil, ExtractFileName(RootPath[i]));
      ShowDir(RootPath[i], TreeNode);
    end;
  MainEditor.TreeView1.Items.GetFirstNode.Expand(false);
end;

procedure LoadFromFile;
var
  Rule:TextFile;
  Line:string;
  Display:string;
  Insert:string;
  DataKind:integer; //1 RootPath 2 Keyword 3 Param List
begin
  AssignFile(Rule, ExtractFilePath(Application.ExeName)+'Rule.txt');
  {$I-}
  Reset(Rule);
  {$I+}
  if IOResult <> 0 then
    begin
      ShowMessage('Read rule.txt error');
      Exit;
    end;
  DataKind:=0;
  RootPath.Clear;

  while not Eof(Rule) do
    begin
      Readln(Rule, Line);
      if (Line = '----Engine Name----') then
        begin
          DataKind:=4;
          Readln(Rule, Line);
        end;
      if (Line = '----Param Hints----') then
        begin
          DataKind:=3;
          MainEditor.SynCompletionProposal2.ClearList;
          Readln(Rule, Line);
        end;
      if (Line = '----Keyword----') then
        begin
          DataKind:=2;
          Readln(Rule, Line);
        end;
      if (Line = '----Root Path----') then
        begin
          DataKind:=1;
          Readln(Rule, Line);
        end;
      if (DataKind = 4) then
        begin
          EngineName:=Line;
          DataKind := 0;
        end;
      if (DataKind = 2) or (DataKind = 3) then
        begin
          Insert:=Line;
          Display:='';
          while (not Eof(Rule)) and (Line <> '[><]') do
            begin
              Readln(Rule, Line);
              if (Line <> '[><]') then
                begin
                  if (Display = '') then
                    Display:=Line
                   else
                    Display:=Display + #13 + #10 + Line;
                  if (Line <> '') and (DataKind = 3) then
                    MainEditor.SynCompletionProposal2.AddItem(Line, Insert);

                end;
            end;
          if DataKind = 2 then
            MainEditor.SynCompletionProposal1.AddItem(Display, Insert)
           else
            MainEditor.SynCompletionProposal1.AddItem(Insert, Insert);
        end;
      if (DataKind = 1) then
        RootPath.Add(Line);
    end;
  CloseFile(Rule);
end;

procedure TMainEditor.Attach1Click(Sender: TObject);
begin
  ProcessListForm.ShowModal;
end;

procedure TMainEditor.WndProc(var Message: TMessage);
var
  CommandLine:string;
  FileName:^string;
begin
  if Message.Msg = (WM_USER + 1) then
    begin
      CommandLine:=gFileName;
      if (CommandLine <> '') and (not CheckFileName(CommandLine)) then
        begin
          New1Click(nil);
          SynLua1.bCanAdd:=true;
          CurrentMemo.Lines.LoadFromFile(CommandLine);
          SynLua1.bCanAdd:=false;
          FileName:=Pointer(CurrentMemo.Tag);
          FileName^:=CommandLine;
          LoadFromFile;
          PageControl1.ActivePage.Caption:=ExtractFileName(CommandLine);
          if (FileGetAttr(CommandLine) and faReadOnly) > 0 then
            CurrentMemo.ReadOnly:=true;
        end;
      SwitchToThisWindow(Application.Handle, false);
    end;
  inherited;
end;

procedure TMainEditor.FormCreate(Sender: TObject);
var
  FileName:^string;

begin
  MainEditor.Caption:=TitleStr;

  DebugWindow1Click(Sender);
  RootPath:=TStringList.Create;
  LuaDebugger:=TDebugger.Create;
  LuaDebugger.OnNewFile:=MainEditor.OnNewFile;
  LuaDebugger.OnNewFunc:=MainEditor.OnNewFunc;
  LuaDebugger.OnNewLine:=MainEditor.OnNewLine;
  LuaDebugger.OnNewError:=MainEditor.OnNewError;
  LuaDebugger.OnNewGlobalVar:=MainEditor.OnNewGlobalVar;
  LuaDebugger.OnNewLocalVar:=MainEditor.OnNewLocalVar;
  LuaDebugger.OnBeginRun:=MainEditor.OnBeginRun;
  LuaDebugger.OnBeginBreak:=MainEditor.OnBeginBreak;
  LuaDebugger.OnNewStack:=MainEditor.OnNewStack;

  OldGlobalList:=TStringList.Create;
  OldLocalList:=TStringList.Create;

  InfoList.InsertRow('File Name', '', true);
  InfoList.InsertRow('Function Name', '', true);

  LoadFromFile;
  RefreshTree;

          if (CommandLine <> '') then
            begin
              New1Click(Sender);
              SynLua1.bCanAdd:=true;
              CurrentMemo.Lines.LoadFromFile(CommandLine);
              SynLua1.bCanAdd:=false;
              FileName:=Pointer(CurrentMemo.Tag);
              FileName^:=CommandLine;
              LoadFromFile;
              PageControl1.ActivePage.Caption:=ExtractFileName(CommandLine);
              if (FileGetAttr(CommandLine) and faReadOnly) > 0 then
                CurrentMemo.ReadOnly:=true;
            end;

end;


procedure TMainEditor.Run1Click(Sender: TObject);
begin
  LuaDebugger.Run;
end;

procedure TMainEditor.Break1Click(Sender: TObject);
begin
  LuaDebugger.Break;
end;

procedure TMainEditor.Step1Click(Sender: TObject);
begin
  LuaDebugger.Step;
end;

procedure TMainEditor.OnNewFile(FileName:string);
var
  PageIndex:integer;
  s:^string;
begin
  if CheckFileName(FileName) then
    exit;
  for PageIndex:=0 to PageControl1.PageCount - 1 do
    begin
      s:=Pointer(PageControl1.Pages[PageIndex].Tag);
      if (s^ = FileName) then
        begin
          PageControl1.ActivePage:=PageControl1.Pages[PageIndex];
          break;
        end;
    end;
    begin
      New1Click(nil);

      s:=Pointer(CurrentMemo.Tag);
      s^:=FileName;

      SynLua1.bCanAdd:=true;
      try
        CurrentMemo.Lines.LoadFromFile(FileName);
      except
        on E:Exception do
          begin
            Memo1.Lines.Add(E.Message);
            PageControl2.ActivePageIndex:=0;
            PageControl1.ActivePage.Free;
            exit;
          end;
      end;
      SynLua1.bCanAdd:=false;
      LoadFromFile;
      PageControl1.ActivePage.Caption:=ExtractFileName(FileName);
      if (FileGetAttr(FileName) and faReadOnly) > 0 then
        CurrentMemo.ReadOnly:=true;

    end;
  InfoList.Strings.Clear;
  InfoList.InsertRow('File Name', FileName, true);
  InfoList.InsertRow('Function Name', '', true);
  GlobalVarList.Strings.Clear;
  LocalVarList.Strings.Clear;

end;

procedure TMainEditor.OnNewFunc(FuncName:string);
begin
  InfoList.Values['Function Name']:=FuncName;
  LocalVarList.Strings.Clear;
end;

procedure TMainEditor.OnNewLine(LineCount:integer);
begin
  if (PageControl1.PageCount > 0) and Assigned(CurrentMemo) then
    begin
      if (CurrentMemo.Lines.Count > LineCount) then
        CurrentMemo.CaretY:=LineCount;
      nCurline:=LineCount;
      CurrentMemo.Refresh;
    end;
end;

procedure TMainEditor.OnNewError(ErrorMsg:string; LineCount:integer);
begin
  Memo1.Lines.Add('Line ' + IntToStr(LineCount) + ' : ' + ErrorMsg);
  PageControl2.ActivePageIndex:=0;
  nErrorLine:=LineCount;
  if PageControl1.PageCount > 0 then
    begin
      CurrentMemo.GotoLineAndCenter(LineCount);
      CurrentMemo.Refresh;
    end;
end;

procedure TMainEditor.OnNewGlobalVar(VarIndex:integer; VarName:PChar;
  VarType:TLuaVarType; VarIntValue:integer; VarStrValue:PChar);
var
  RowIndex:integer;
  NewVarName:string;
begin
  if (VarName = nil) then
    exit;
  if VarIndex = 1 then
    begin
      OldGlobalList.Assign(GlobalVarList.Strings);
      GlobalVarList.Strings.Clear;
    end;
  NewVarName:=VarName;
  while LocalVarList.FindRow(NewVarName, RowIndex) do
    NewVarName:='#' + NewVarName;
  case VarType of
    LUA_TNONE:
      begin
        GlobalVarList.InsertRow(NewVarName, 'Inavailable', true);
        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:='Inavailable';
      end;
    LUA_TNUMBER:
      begin
        if (OldGlobalList.Values[NewVarName] <> '') and (VarIndex <> 1) then
          begin
            if (OldGlobalList.Values[NewVarName] <>
                IntToStr(VarIntValue)) and (GlobalVarList.Row > 0) then
              begin
                GlobalVarList.Strings.Insert(0, NewVarName + '(int)=' + IntToStr(VarIntValue));
              end
             else
              GlobalVarList.InsertRow(NewVarName + '(int)', IntToStr(VarIntValue), true);
          end
         else
          GlobalVarList.InsertRow(NewVarName + '(int)', IntToStr(VarIntValue), true);

        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:=IntToStr(VarIntValue);
      end;
    LUA_TSTRING:
      begin
        if (OldGlobalList.Values[NewVarName] <> '') and (VarIndex <> 1) then
          begin
            if (OldGlobalList.Values[NewVarName] <>
                IntToStr(VarIntValue)) and (GlobalVarList.Row > 0) then
              begin
                GlobalVarList.Strings.Insert(0, NewVarName + '(str)=' + VarStrValue);
              end
             else
              GlobalVarList.InsertRow(NewVarName + '(str)', VarStrValue, true);
          end
         else
          GlobalVarList.InsertRow(NewVarName + '(str)', VarStrValue, true);

        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:=VarStrValue;
      end;
    LUA_TNIL:
      begin
        GlobalVarList.InsertRow(NewVarName, 'nil', true);
        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:='nil';
      end;
    LUA_TFUNCTION:
      begin
        GlobalVarList.InsertRow(NewVarName, 'function', true);
        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:='function';
      end;
    LUA_TTABLE:
      begin
        LocalVarList.InsertRow(NewVarName, 'table', true);
        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:='table';
      end;
    LUA_TUSERDATA:
      begin
        LocalVarList.InsertRow(NewVarName, 'userdata', true);
        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:='userdata';
      end;
  end;
end;

procedure TMainEditor.OnNewLocalVar(VarIndex:integer; VarName:PChar;
  VarType:TLuaVarType; VarIntValue:integer; VarStrValue:PChar);
var
  RowIndex:integer;
  NewVarName:string;
begin
  if (VarName = nil) then
    exit;
  if VarIndex = 1 then
    begin
      OldLocalList.Assign(LocalVarList.Strings);
      LocalVarList.Strings.Clear;
    end;
  NewVarName:=VarName;
  while LocalVarList.FindRow(NewVarName, RowIndex) do
    NewVarName:='#' + NewVarName;
  case VarType of
    LUA_TNONE:
      begin
        LocalVarList.InsertRow(NewVarName, 'Inavailable', true);
        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:='Inavailable';
      end;
    LUA_TNUMBER:
      begin
        if (OldLocalList.Values[NewVarName] <> '') and (VarIndex <> 1) then
          begin
            if (OldLocalList.Values[NewVarName] <>
                IntToStr(VarIntValue)) and (LocalVarList.Row > 0) then
              begin
                LocalVarList.Strings.Insert(0, NewVarName + '(int)=' + IntToStr(VarIntValue))
              end
             else
              LocalVarList.InsertRow(NewVarName + '(int)', IntToStr(VarIntValue), true);
          end
         else
          LocalVarList.InsertRow(NewVarName + '(int)', IntToStr(VarIntValue), true);

        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:=IntToStr(VarIntValue);
      end;
    LUA_TSTRING:
      begin
        if (OldLocalList.Values[NewVarName] <> '') and (VarIndex <> 1) then
          begin
            if (OldLocalList.Values[NewVarName] <>
                IntToStr(VarIntValue)) and (LocalVarList.Row > 0) then
              begin
                LocalVarList.Strings.Insert(0, NewVarName + '(str)=' + VarStrValue);
              end
             else
              LocalVarList.InsertRow(NewVarName + 'str', VarStrValue, true);
          end
         else
          LocalVarList.InsertRow(NewVarName + 'str', VarStrValue, true);

        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:=VarStrValue;
      end;
    LUA_TNIL:
      begin
        LocalVarList.InsertRow(NewVarName, 'nil', true);
        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:='nil';
      end;
    LUA_TFUNCTION:
      begin
        LocalVarList.InsertRow(NewVarName, 'function', true);
        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:='function';
      end;
    LUA_TTABLE:
      begin
        LocalVarList.InsertRow(NewVarName, 'table', true);
        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:='table';
      end;
    LUA_TUSERDATA:
      begin
        LocalVarList.InsertRow(NewVarName, 'userdata', true);
        if (WatchList.FindRow(NewVarName, RowIndex)) then
          WatchList.Values[NewVarName]:='userdata';
      end;
  end;
end;

procedure TMainEditor.OnBeginRun(LineCount:integer);
begin
  MainEditor.Caption:='Lua Workshop Ver 0.4.1 - Running';
end;

procedure TMainEditor.OnBeginBreak(LineCount:integer);
var
  WinDir:PChar;
begin
//  WinDir:=AllocMem(256);
//  GetWindowsDirectory(WinDir, 255);
  MainEditor.Caption:='Lua Workshop Ver 0.4.1 - Stopped';
//  PlaySound(PChar(WinDir + '\chord.wav'), 0, SND_FILENAME);
//  FreeMem(WinDir);
//  MainEditor.Hide;
//  MainEditor.Show;
  MainEditor.Activate;
end;

procedure TMainEditor.OnNewStack(StackLevel:integer; FileName:string;
                     FuncType:string; LineCount:integer);
begin
  if (StackLevel = 1) then
    StackList.Items.Clear;
  StackList.Items.Add(FileName + '  |  ' + FuncType + '  | Line  ' + IntToStr(LineCount));
end;


procedure TMainEditor.OnSpecialLineColors(Sender: TObject; Line: integer;
  var Special: boolean; var FG, BG: TColor);
begin
  FG:=$FFFFFF;
  BG:=$FFFFFF;
  if LuaDebugger.GetBreakpoint(Line) then
    begin
      Special:=true;
      FG:=FG and clwhite;
      BG:=BG and clred;
    end;
  if (Line = nCurLine) then
    begin
      Special:=true;
      FG:=FG and clwhite;
      BG:=BG and clnavy;
    end;
  if (Line = nErrorLine) then
    begin
      Special:=true;
      FG:=FG and clred;
      BG:=BG and cllime;
    end;
end;

procedure TMainEditor.DefaultMemoGutterClick(Sender: TObject;
  Button: TMouseButton; X, Y, Line: Integer; Mark: TSynEditMark);
begin
  if LuaDebugger.GetBreakpoint(Line) then
    LuaDebugger.SetBreakpoint(Line, false)
   else
    LuaDebugger.SetBreakpoint(Line, true);
  if Assigned(CurrentMemo) then
    CurrentMemo.Refresh;
end;

procedure TMainEditor.Detach1Click(Sender: TObject);
begin
  if (LuaDebugger.Detach) then
    ShowMessage('Detached!');
end;

procedure TMainEditor.Exit1Click(Sender: TObject);
begin
  Close;
end;

procedure TMainEditor.New1Click(Sender: TObject);
var
  NewTabSheet:TTabSheet;
  NewSynMemo:TSynMemo;
  FileName:^string;
begin
  NewTabSheet:=TTabSheet.Create(PageControl1);
  NewTabSheet.PageControl:=PageControl1;
  NewSynMemo:=TSynMemo.Create(NewTabSheet);
  NewSynMemo.Font.Assign(DefaultMemo.Font);
  NewSynMemo.Gutter.Assign(DefaultMemo.Gutter);
  NewSynMemo.Options:=DefaultMemo.Options;
  NewSynMemo.WantTabs:=true;
  NewSynMemo.TabWidth:=4;
  NewSynMemo.Align:=alClient;
  NewSynMemo.SearchEngine:=DefaultMemo.SearchEngine;
  NewSynMemo.Parent:=NewTabSheet;
  NewSynMemo.Visible:=true;
  NewSynMemo.OnGutterClick:=DefaultMemo.OnGutterClick;
  NewSynMemo.OnKeyUp:=DefaultMemo.OnKeyUp;
  New(FileName);
  FileName^:='';
  NewSynMemo.Tag:=integer(FileName);
  PageControl1.ActivePage:=NewTabSheet;
  NewTabSheet.Tag:=integer(NewSynMemo);
  NewTabSheet.Caption:='Untitled';
  CurrentMemo:=NewSynMemo;

  SynCompletionProposal1.Editor:=CurrentMemo;
  SynCompletionProposal2.Editor:=CurrentMemo;
  SynCompletionProposal1.ClearList;
  SynCompletionProposal2.ClearList;
  SynLua1:=TSynLua.Create(self);
  LoadFromFile;
  SynLua1.SynCompletionProposal:=SynCompletionProposal1;
  CurrentMemo.Highlighter:=SynLua1;
  CurrentMemo.OnSpecialLineColors:=MainEditor.OnSpecialLineColors;
      
end;

procedure TMainEditor.Open1Click(Sender: TObject);
var
  FileName:^string;
begin
  if (OpenDialog1.Execute) then
    begin
      if CheckFileName(OpenDialog1.FileName) then
        exit;
      New1Click(Sender);
      FileName:=Pointer(CurrentMemo.Tag);
      FileName^:=OpenDialog1.FileName;
      SynLua1.bCanAdd:=true;
      CurrentMemo.Lines.LoadFromFile(OpenDialog1.FileName);
      SynLua1.bCanAdd:=false;
      LoadFromFile;
      PageControl1.ActivePage.Caption:=ExtractFileName(OpenDialog1.FileName);
      if (FileGetAttr(OpenDialog1.FileName) and faReadOnly) > 0 then
        CurrentMemo.ReadOnly:=true;
    end;
end;

procedure TMainEditor.Save1Click(Sender: TObject);
var
  FileName:^string;
begin
  if (not Assigned(CurrentMemo)) or (PageControl1.PageCount <= 0) then
    exit;
  FileName:=Pointer(CurrentMemo.Tag);
  if (FileName^ <> '') then
    begin
      CurrentMemo.Lines.SaveToFile(FileName^);
      CurrentMemo.Modified:=false;
    end
   else
    begin
      if (SaveDialog1.Execute) then
        begin
          FileName^:=SaveDialog1.FileName;
          CurrentMemo.Lines.SaveToFile(FileName^);
          CurrentMemo.Modified:=false;
        end
       else
        Exit;
    end;
end;

procedure TMainEditor.DefaultMemoKeyUp(Sender: TObject; var Key: Word;
  Shift: TShiftState);
var
  Stream:TMemoryStream;
  x, y, topline:integer;
  UndoList, RedoList:TSynEditUndoList;
begin
  if (CurrentMemo.ReadOnly) then
    begin

    end;

  if (LastY <> CurrentMemo.CaretY) and (chr(Key) in [#13, 'a'..'z', '0'..'9']) then
    begin
      SynCompletionProposal1.ClearList;
      LoadFromFile;
      SynLua1.bCanAdd:=true;
      Stream:=TMemoryStream.Create;
      UndoList:=TSynEditUndoList.Create;
      RedoList:=TSynEditUndoList.Create;
      CurrentMemo.Lines.SaveToStream(Stream);
      x:=CurrentMemo.CaretX;
      y:=CurrentMemo.CaretY;
      topline:=CurrentMemo.TopLine;
      UndoList.Assign(CurrentMemo.UndoList);
      RedoList.Assign(CurrentMemo.RedoList);
      CurrentMemo.Lines.LoadFromStream(Stream);
      CurrentMemo.UndoList.Assign(UndoList);
      CurrentMemo.RedoList.Assign(RedoList);
      CurrentMemo.CaretX:=x;
      CurrentMemo.CaretY:=y;
      CurrentMemo.TopLine:=topline;
      Stream.Free;
      SynLua1.bCanAdd:=false;
    end;

  LastY:=CurrentMemo.CaretY;
  if (((Key >= 65) and (Key <= 91)) or (Key = 32) or (Key = VK_DELETE)
      or (Key = 8)) and ((Shift = []) or (Shift = [ssShift])) then
    begin
      if (SynCompletionProposal1.GetCurrentInput(CurrentMemo) <> '') then
        SynCompletionProposal1.CurrentString:=SynCompletionProposal1.GetCurrentInput(CurrentMemo)
       else
          SynCompletionProposal1.Deactivate;
      if (SynCompletionProposal1.FForm.FAssignedList.Count > 0) and
         (not SynCompletionProposal1.FForm.Showing) then
        SynCompletionProposal1.ActivateCompletion
       else if SynCompletionProposal1.FForm.Showing then
        begin
          SynCompletionProposal1.Deactivate;
        end;

      if (SynCompletionProposal2.GetCurrentInput(CurrentMemo) <> '') then
        SynCompletionProposal2.CurrentString:=SynCompletionProposal2.GetCurrentInput(CurrentMemo)
       else
          SynCompletionProposal2.Deactivate;
    end;

  if (Key = ord('9')) and (Shift = [ssShift]) and
     (not SynCompletionProposal2.FForm.Showing) then
    SynCompletionProposal2.ActivateCompletion;

  if (SynCompletionProposal2.FForm.Showing) then
    SynCompletionProposal2.FForm.Repaint;

  if ((Key = ord('0')) and (Shift = [ssShift]) or (Key = VK_UP) or
      (Key = VK_DOWN)) and
     (SynCompletionProposal2.FForm.Showing) then
    SynCompletionProposal2.Deactivate;

end;

procedure TMainEditor.QuickAttach1Click(Sender: TObject);
var
  hSnapshotHandle:THandle;
  hModuleEntry32:TModuleEntry32;
  hProcessEntry32:TProcessEntry32;
  ret:boolean;
  ProcessID:integer;
  BaseAddress:integer;
begin
  if (HostName = '') then
    HostName:='GameServer.exe';
  hSnapshotHandle:=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  hProcessEntry32.dwSize:=Sizeof(hProcessEntry32);
  ret:=Process32First(hSnapshotHandle,hProcessEntry32);
  ProcessID:=0;
  while ret do
    begin
      if (hProcessEntry32.szExeFile = HostName) then
        ProcessID:=hProcessEntry32.th32ProcessID;
      ret:=Process32Next(hSnapshotHandle,hProcessEntry32);
    end;
  CloseHandle(hSnapshotHandle);
  if (ProcessID = 0) then
    begin
      ShowMessage('Attach process failed!');
      exit;
    end;

  hSnapshotHandle:=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessID);
  hModuleEntry32.dwSize:=Sizeof(TModuleEntry32);
  ret:=Module32First(hSnapshotHandle,hModuleEntry32);
  BaseAddress:=0;
  while (ret) do
    begin
      if (UpperCase(hModuleEntry32.szModule) = UpperCase(EngineName)) then
        BaseAddress:=integer(hModuleEntry32.modBaseAddr);
      ret:=Module32Next(hSnapshotHandle,hModuleEntry32);
    end;
  CloseHandle(hSnapshotHandle);

  if (BaseAddress = 0) then
    begin
      ShowMessage('Attach module failed!');
      exit;
    end;
  ret:=LuaDebugger.Attach(ProcessID, BaseAddress);
  if (not ret) then
    ShowMessage('Cannot attach!');
  ShowMessage('Attached!');
end;

procedure TMainEditor.Undo1Click(Sender: TObject);
begin
  if Assigned(CurrentMemo) and (PageControl1.PageCount > 0) and (CurrentMemo.Focused) then
    CurrentMemo.Undo;
end;

procedure TMainEditor.Redo1Click(Sender: TObject);
begin
  if Assigned(CurrentMemo) and (PageControl1.PageCount > 0) and (CurrentMemo.Focused) then
    CurrentMemo.Redo;
end;

procedure TMainEditor.Compile2Click(Sender: TObject);
var
  hReadPipe, hWritePipe:THandle;
  Output:array [0..1024 * 10] of char;
  sa:SECURITY_ATTRIBUTES;
  sui:^TStartupInfo;
  pi:^TProcessInformation;
  RealReadSize:Cardinal;
  ScriptName:string;
  i:integer;
  Token:string;
  s:string;
  LineNumber:string;
  nLine:integer;
  FileName:^string;
begin
  if (not Assigned(CurrentMemo)) or (PageControl1.PageCount <= 0) then
    exit;
  FileName:=Pointer(CurrentMemo.Tag);
  if CurrentMemo.Modified then
    begin
      if (FileName^ <> '') then
        CurrentMemo.Lines.SaveToFile(FileName^)
       else if CurrentMemo.Modified then
        begin
          if (SaveDialog1.Execute) then
            begin
              FileName^:=SaveDialog1.FileName;
              CurrentMemo.Lines.SaveToFile(FileName^);
            end
           else
            Exit;
        end
       else
        Exit;
    end;
  sa.nLength:=sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor:=nil;
  sa.bInheritHandle:=true;
  if not CreatePipe(hReadPipe, hWritePipe, @sa, 1024) then
    exit;

  sui:=AllocMem(sizeof(TStartupInfo));
  sui.cb:=sizeof(TStartupInfo);
  sui.lpReserved:=nil;
  sui.lpDesktop:=nil;
  sui.lpTitle:=nil;
  sui.wShowWindow:=SW_HIDE;
  sui.hStdOutput:=hWritePipe;
  sui.hStdError:=hWritePipe;
  sui.dwFlags:=STARTF_USESHOWWINDOW or STARTF_USESTDHANDLES;

  ScriptName:=' ' + FileName^;
  pi:=AllocMem(sizeof(TProcessInformation));
  CreateProcess(PChar(ExtractFilePath(Application.ExeName)+'compile.exe '),
                PChar('-t' + ScriptName), nil, nil,
                true, NORMAL_PRIORITY_CLASS, nil, nil, sui^, pi^);
  if pi.hProcess=0 then
    begin
      raise Exception.Create('Cannot call lua compile.exe');
    end;
  CloseHandle(hWritePipe);
  RealReadSize:=0;
  Memo1.Lines.Clear;
  PageControl2.ActivePageIndex:=0;
  ZeroMemory(@Output, 1024);
  ReadFile(hReadPipe, Output, 1024, RealReadSize, nil);
  Memo1.Lines.Add(Output);

  CloseHandle(hReadPipe);
  CloseHandle(pi.hProcess);

  if strlen(Output) = 0  then
    begin
      nErrorLine:=0;
      Memo1.Lines.Add('Compiled successfully!');
      CurrentMemo.Refresh;
      Exit;
    end;

  i:=0;
  Token:='';
  LineNumber:='';
  nLine:=0;
  while (i < 1024 * 10) and (Output[i] <> #10) do
    inc(i);
  if (i >= 1024 * 10) then
    begin
      exit;
    end;
  while (i < 1024 * 10) and (Output[i] <> '`') do
      inc(i);
  if (i >= 1024 * 10) then
    begin
      exit;
    end;
  if Output[i] = '`' then
    begin
      inc(i);
      while (i < 1024 * 10) and (Output[i] <> '''') do
        begin
          Token:=Token + Output[i];
          inc(i);
        end;
        if (i >= 1024 * 10) then
          begin
            ShowMessage('Cannot get line number');
            exit;
          end;
    end;
  s:=Output;
  if Pos('line', s) <> 0 then
    begin
      i:=Pos('line', s) + 5;
      while s[i] <> ' ' do
        begin
          LineNumber:=LineNumber + s[i];
          inc(i);
        end;
    end;
  if (LineNumber <> '') then
    nLine:=StrToInt(LineNumber);
  nErrorLine:=nLine;
  CurrentMemo.GotoLineAndCenter(nLine);
  CurrentMemo.CaretX:=Pos(Token, CurrentMemo.Lines[nLine - 1]);
  CurrentMemo.Refresh;

  Sleep(100);
  FreeMem(sui);
  FreeMem(pi);
end;

procedure TMainEditor.Search1Click(Sender: TObject);
begin
  if (not Assigned(CurrentMemo)) or (PageControl1.PageCount <= 0) then
    exit;
  SearchDlg.ShowModal;
  if (SynEditSearch1.Pattern = '') then
    Exit;
  SynEditSearch1.FindAll(CurrentMemo.Text);
  nSearchCount:=0;
  if (SynEditSearch1.ResultCount > 0) then
    begin
      CurrentMemo.CaretXY:=CurrentMemo.CharIndexToRowCol(SynEditSearch1.Results[0]);
      CurrentMemo.CaretX:=CurrentMemo.CaretX - 1;
      nSearchCount:=1;
    end
   else
    ShowMessage('Not found!');

end;

procedure TMainEditor.SearchAgain1Click(Sender: TObject);
begin
  if (not Assigned(CurrentMemo)) or (PageControl1.PageCount <= 0) then
    exit;
  if (nSearchCount > 0) then
    begin
      SynEditSearch1.FindAll(CurrentMemo.Text);
      if (SynEditSearch1.ResultCount > 0) and (nSearchCount < SynEditSearch1.ResultCount) then
        begin
          CurrentMemo.CaretXY:=CurrentMemo.CharIndexToRowCol(SynEditSearch1.Results[nSearchCount]);
          CurrentMemo.CaretX:=CurrentMemo.CaretX - 1;
          inc(nSearchCount);
        end
       else
        nSearchCount:=0;
    end;
end;

procedure TMainEditor.GlobalVarListKeyPress(Sender: TObject; var Key: Char);
var
  VarName:string;
  VarType:string;
begin
  if (Key = #13) and ((Pos('(int)', GlobalVarList.Keys[GlobalVarList.Selection.Top]) <> 0) or
     (Pos('(str)', GlobalVarList.Keys[GlobalVarList.Selection.Top]) <> 0)) then
    begin
      VarName:=Copy(GlobalVarList.Keys[GlobalVarList.Selection.Top], 1,
                    Pos('(', GlobalVarList.Keys[GlobalVarList.Selection.Top]) - 1);
      VarType:=Copy(GlobalVarList.Keys[GlobalVarList.Selection.Top],
                    Pos('(', GlobalVarList.Keys[GlobalVarList.Selection.Top]) + 1,
                    Length(GlobalVarList.Keys[GlobalVarList.Selection.Top]) -
                      Pos('(', GlobalVarList.Keys[GlobalVarList.Selection.Top]) - 1);
      if VarType = 'int' then
        LuaDebugger.SetGlobalValue(VarName, LUA_TNUMBER,
            GlobalVarList.Values[GlobalVarList.Keys[GlobalVarList.Selection.Top]]);
      if VarType = 'str' then
        LuaDebugger.SetGlobalValue(VarName, LUA_TSTRING,
            GlobalVarList.Values[GlobalVarList.Keys[GlobalVarList.Selection.Top]]);
      ShowMessage('OK');
    end
end;

procedure TMainEditor.LocalVarListKeyPress(Sender: TObject; var Key: Char);
var
  VarName:string;
  VarType:string;
begin
  if (Key = #13) and ((Pos('(int)', LocalVarList.Keys[LocalVarList.Selection.Top]) <> 0) or
     (Pos('(str)', LocalVarList.Keys[LocalVarList.Selection.Top]) <> 0)) then
    begin
      VarName:=Copy(LocalVarList.Keys[LocalVarList.Selection.Top], 1,
                    Pos('(', LocalVarList.Keys[LocalVarList.Selection.Top]) - 1);
      VarType:=Copy(LocalVarList.Keys[LocalVarList.Selection.Top],
                    Pos('(', LocalVarList.Keys[LocalVarList.Selection.Top]) + 1,
                    Length(LocalVarList.Keys[LocalVarList.Selection.Top]) -
                      Pos('(', LocalVarList.Keys[LocalVarList.Selection.Top]) - 1);
      if VarType = 'int' then
        LuaDebugger.SetLocalValue(VarName, LUA_TNUMBER,
            LocalVarList.Values[LocalVarList.Keys[LocalVarList.Selection.Top]]);
      if VarType = 'str' then
        LuaDebugger.SetLocalValue(VarName, LUA_TSTRING,
            LocalVarList.Values[LocalVarList.Keys[LocalVarList.Selection.Top]]);
      ShowMessage('OK');
    end
end;

procedure TMainEditor.Reload1Click(Sender: TObject);
begin
  LuaDebugger.ReloadScript;
end;

procedure TMainEditor.DebugWindow1Click(Sender: TObject);
begin
  if (DebugWindow1.Checked) then
    begin
      MainEditor.InfoList.Hide;
      MainEditor.LocalVarList.Hide;
      MainEditor.GlobalVarList.Hide;
      MainEditor.PageControl1.Width:=MainEditor.PageControl1.Width +
          InfoList.Width + 4;
      MainEditor.PageControl2.Width:=MainEditor.PageControl2.Width +
          InfoList.Width + 4;
      DebugWindow1.Checked:=false;
    end
   else
    begin
      MainEditor.InfoList.Show;
      MainEditor.LocalVarList.Show;
      MainEditor.GlobalVarList.Show;
      MainEditor.PageControl1.Width:=MainEditor.PageControl1.Width -
          InfoList.Width - 4;
      MainEditor.PageControl2.Width:=MainEditor.PageControl2.Width -
          InfoList.Width - 4;

      DebugWindow1.Checked:=true;
    end;
end;


procedure TMainEditor.PageControl1Change(Sender: TObject);
begin
  if (PageControl1.PageCount = 0) then
    begin
      CurrentMemo:=nil;
      exit;
    end;
  CurrentMemo:=TSynMemo(PageControl1.ActivePage.Tag);
  
  SynCompletionProposal1.Editor:=CurrentMemo;
  SynCompletionProposal2.Editor:=CurrentMemo;
  SynCompletionProposal1.ClearList;
  SynCompletionProposal2.ClearList;
  LoadFromFile;
  SynLua1.SynCompletionProposal:=SynCompletionProposal1;
  CurrentMemo.Highlighter:=SynLua1;
  CurrentMemo.OnSpecialLineColors:=MainEditor.OnSpecialLineColors;
  CurrentMemo.SetFocus;
end;

procedure TMainEditor.PageControl1MouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var
  ret:Word;
  FileName:^string;
begin
  if (Button = mbRight) and (PageControl1.IndexOfTabAt(X, Y) >= 0) then
    begin
      FileName:=Pointer(TSynMemo(PageControl1.Pages[PageControl1.IndexOfTabAt(X, Y)].Tag).Tag);
      if (CurrentMemo.Modified) then
        begin
          ret:=MessageDlg('Save?', mtConfirmation, [mbYes, mbNo, mbCancel], 0);
          if (ret = 6) then
            begin
              if (FileName^ <> '') then
                TSynMemo(PageControl1.Pages[PageControl1.IndexOfTabAt(X, Y)].Tag).
                    Lines.SaveToFile(FileName^)
               else
                begin
                  if (SaveDialog1.Execute) then
                    begin
                      FileName^:=SaveDialog1.FileName;
                      TSynMemo(PageControl1.Pages[PageControl1.IndexOfTabAt(X, Y)].Tag).
                          Lines.SaveToFile(FileName^);
                    end
                   else
                    Exit;
                end;
              PageControl1.Pages[PageControl1.IndexOfTabAt(X, Y)].Free;
            end;
          if (ret = 7) then
            begin
              PageControl1.Pages[PageControl1.IndexOfTabAt(X, Y)].Free;
            end;
          if (ret = 2) then
            begin
            end;
        end
       else
         PageControl1.Pages[PageControl1.IndexOfTabAt(X, Y)].Free;

    end;
end;

procedure TMainEditor.FormClose(Sender: TObject; var Action: TCloseAction);
var
  ret:integer;
  i:integer;
  FileName:^string;
  AnyModified:boolean;
begin
  AnyModified:=false;
  i:=0;
  while (not AnyModified) and (i < PageControl1.PageCount) do
    begin
      if TSynMemo(PageControl1.Pages[i].Tag).Modified then
        AnyModified:=true;
      i:=i+1;
    end;
  if (AnyModified) then
    begin
      ret:=MessageDlg('Save All?', mtConfirmation, [mbYes, mbNo, mbCancel], 0);
      if (ret = 6) then
        begin
          for i:=0 to PageControl1.PageCount - 1 do
            begin
              PageControl1.ActivePage:=PageControl1.Pages[i];
              CurrentMemo:=TSynMemo(PageControl1.Pages[i].Tag);
              FileName:=Pointer(CurrentMemo.Tag);
              if (FileName^ <> '') then
                CurrentMemo.Lines.SaveToFile(FileName^)
               else
                begin
                  if (SaveDialog1.Execute) then
                    begin
                      FileName^:=SaveDialog1.FileName;
                      CurrentMemo.Lines.SaveToFile(FileName^);
                    end
                   else
                    Exit;
                end;
            end;
          Application.Terminate;
        end;
      if (ret = 7) then
        begin
          Application.Terminate;
        end;
      if (ret = 2) then
        begin
          Action:=caNone;
        end;
    end
   else
     Application.Terminate;

end;



procedure TMainEditor.TreeView1DblClick(Sender: TObject);
var
  Path:string;
  TreeNode:TTreeNode;
  FileName:^string;
  i:integer;
begin
  TreeNode:=TreeView1.Selected;
  if (TreeNode = nil) or (TreeNode = TreeView1.Items.GetFirstNode) or
     (TreeNode.getFirstChild <> nil) then
    exit;
  Path:=TreeNode.Text;
  TreeNode:=TreeNode.Parent;
  while TreeNode.Parent <> nil do
    begin
      Path:=TreeNode.Text + '\' + Path;
      TreeNode:=TreeNode.Parent;
    end;
  i:=0;
  while TreeNode.getPrevSibling <> nil do
    begin
      TreeNode:=TreeNode.getPrevSibling;
      i:=i+1;
    end;
  Path:=RootPath[i] + '\' + Path;

  if DirectoryExists(Path) then
    Exit;

  if CheckFileName(Path) then
    exit;
  New1Click(Sender);

  FileName:=Pointer(CurrentMemo.Tag);
  FileName^:=Path;
  SynLua1.bCanAdd:=true;
  CurrentMemo.Lines.LoadFromFile(Path);
  SynLua1.bCanAdd:=false;
  LoadFromFile;
  PageControl1.ActivePage.Caption:=ExtractFileName(Path);
  if (FileGetAttr(Path) and faReadOnly) > 0 then
    CurrentMemo.ReadOnly:=true;
end;

procedure TMainEditor.TreeWindow1Click(Sender: TObject);
begin
  if (TreeWindow1.Checked) then
    begin
      MainEditor.TreeView1.Hide;
      MainEditor.PageControl1.Left:=MainEditor.PageControl1.Left -
          TreeView1.Width - 4;
      MainEditor.PageControl1.Width:=MainEditor.PageControl1.Width +
          TreeView1.Width + 4;
      TreeWindow1.Checked:=false;
    end
   else
    begin
      MainEditor.TreeView1.Show;
      MainEditor.PageControl1.Left:=MainEditor.PageControl1.Left +
          TreeView1.Width + 4;
      MainEditor.PageControl1.Width:=MainEditor.PageControl1.Width -
          TreeView1.Width - 4;

      TreeWindow1.Checked:=true;
    end;
end;

procedure TMainEditor.Close1Click(Sender: TObject);
var
  ret:Word;
  FileName:^string;
begin
  FileName:=Pointer(CurrentMemo.Tag);
  if (CurrentMemo.Modified) then
    begin
      ret:=MessageDlg('Save?', mtConfirmation, [mbYes, mbNo, mbCancel], 0);
      if (ret = 6) then
        begin
          if (FileName^ <> '') then
            CurrentMemo.Lines.SaveToFile(FileName^)
           else
            begin
              if (SaveDialog1.Execute) then
                begin
                  FileName^:=SaveDialog1.FileName;
                  CurrentMemo.Lines.SaveToFile(FileName^);
                end
               else
                Exit;
            end;
          PageControl1.ActivePage.Free;
        end;
      if (ret = 7) then
        begin
          PageControl1.ActivePage.Free;
        end;
      if (ret = 2) then
        begin
        end;
    end
   else
     PageControl1.ActivePage.Free;
end;

procedure TMainEditor.RefreshTree1Click(Sender: TObject);
begin
  RefreshTree;
end;

procedure TMainEditor.TreeView1Edited(Sender: TObject; Node: TTreeNode;
  var S: String);
var
  Path:string;
  TreeNode:TTreeNode;
  i:integer;
begin
  if (Node = nil) or (Node = TreeView1.Items.GetFirstNode) or
     (Node.getFirstChild <> nil) then
    exit;
  Path:='';
  TreeNode:=Node;
  TreeNode:=TreeNode.Parent;
  while TreeNode.Parent <> nil do
    begin
      Path:=TreeNode.Text + '\' + Path;
      TreeNode:=TreeNode.Parent;
    end;
  i:=0;
  while TreeNode.GetPrev <> nil do
    begin
      TreeNode:=TreeNode.GetPrev;
      i:=i+1;
    end;
  Path:=RootPath[i] + '\' + Path;

  if not DirectoryExists(Path + Node.Text) then
    begin
      if (not RenameFile(Path + Node.Text, Path + S)) then
        ShowMessage('Failed to change file name!')
       else
        ShowMessage('File name is changed from ''' + Node.Text + ''' to ''' + S + '''.'); 
    end; 
end;

procedure TMainEditor.About1Click(Sender: TObject);
begin
  AboutForm.Show;
end;


procedure TMainEditor.WatchListDblClick(Sender: TObject);
begin
  WatchList.InsertRow('', '', true);
end;

procedure TMainEditor.WatchListDragOver(Sender, Source: TObject; X,
  Y: Integer; State: TDragState; var Accept: Boolean);
begin
  Accept:=false;
  if (Source is TSynMemo) then
    Accept:=true;
end;

procedure TMainEditor.WatchListDragDrop(Sender, Source: TObject; X,
  Y: Integer);
begin
  if (Source is TSynMemo) then
    begin
      WatchList.InsertRow((Source as TSynMemo).SelText, '', true);
    end;
end;

procedure TMainEditor.StackListDblClick(Sender: TObject);
var
  FileName:string;
  LineCount:integer;
  i:integer;
  s:^string;
begin
  for i:=0 to StackList.Items.Count - 1 do
    if StackList.Selected[i] then
      begin
        FileName:=Copy(StackList.Items[i], 1, Pos('  |  ', StackList.Items[i]) - 1);
        LineCount:=StrToInt(Copy(StackList.Items[i], Pos('Line', StackList.Items[i]) + 5, 99));
        if CheckFileName(FileName) then
          begin
            if Assigned(CurrentMemo) and (CurrentMemo.Lines.Count > LineCount) then
              begin
                CurrentMemo.CaretY:=LineCount;
              end;
            exit;
          end;
        New1Click(Sender);
        s:=Pointer(CurrentMemo.Tag);
        s^:=FileName;
        SynLua1.bCanAdd:=true;
        try
          CurrentMemo.Lines.LoadFromFile(FileName);
        except
          on E:Exception do
            begin
              Memo1.Lines.Add(E.Message);
              PageControl2.ActivePageIndex:=0;
              PageControl1.ActivePage.Free;
              exit;
            end;
        end;
        SynLua1.bCanAdd:=false;
        LoadFromFile;
        PageControl1.ActivePage.Caption:=ExtractFileName(FileName);
        if (FileGetAttr(FileName) and faReadOnly) > 0 then
          CurrentMemo.ReadOnly:=true;

        CurrentMemo.CaretY:=LineCount;
        if not CurrentMemo.Focused then
          CurrentMemo.SetFocus;
      end;
end;

procedure TMainEditor.Help2Click(Sender: TObject);
var
  Path:string;
  FileName:^string;
begin
  Path:=ExtractFilePath(Application.ExeName) + 'Readme.txt';
  if CheckFileName(Path) then
    exit;
  New1Click(Sender);

  FileName:=Pointer(CurrentMemo.Tag);
  FileName^:=Path;
  SynLua1.bCanAdd:=true;
  CurrentMemo.Lines.LoadFromFile(Path);
  SynLua1.bCanAdd:=false;
  LoadFromFile;
  PageControl1.ActivePage.Caption:=ExtractFileName(Path);
  if (FileGetAttr(Path) and faReadOnly) > 0 then
    CurrentMemo.ReadOnly:=true;
end;

end.
