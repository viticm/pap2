unit ProcessList;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, Grids, ValEdit, StdCtrls, Buttons;

type
  TProcessListForm = class(TForm)
    ValueListEditor1: TValueListEditor;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    procedure FormShow(Sender: TObject);
    procedure BitBtn2Click(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  ProcessListForm: TProcessListForm;

implementation
uses TLHelp32, MainForm;

{$R *.dfm}

procedure TProcessListForm.FormShow(Sender: TObject);
var
  hSnapshotHandle:THandle;
  hProcessEntry32:TProcessEntry32;
  ret:boolean;
begin
  ValueListEditor1.Strings.Clear;
  hSnapshotHandle:=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  hProcessEntry32.dwSize:=Sizeof(hProcessEntry32);
  ret:=Process32First(hSnapshotHandle,hProcessEntry32);
  while ret do
    begin
      ValueListEditor1.InsertRow(IntToStr(hProcessEntry32.th32ProcessID), hProcessEntry32.szExeFile, true);
      ret:=Process32Next(hSnapshotHandle,hProcessEntry32);
    end;
  CloseHandle(hSnapshotHandle);

end;

procedure TProcessListForm.BitBtn2Click(Sender: TObject);
begin
  Close;
end;

procedure TProcessListForm.BitBtn1Click(Sender: TObject);
var
  hSnapshotHandle:THandle;
  hModuleEntry32:TModuleEntry32;
  ret:boolean;
  ProcessID:integer;
  BaseAddress:integer;
begin
  ProcessID:=StrToInt(ValueListEditor1.Keys[ValueListEditor1.Selection.Top]);

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
  HostName:=ValueListEditor1.Values[ValueListEditor1.Keys[ValueListEditor1.Selection.Top]];
  Close;
end;

procedure TProcessListForm.FormClose(Sender: TObject;
  var Action: TCloseAction);
begin
  Close;
end;

end.
