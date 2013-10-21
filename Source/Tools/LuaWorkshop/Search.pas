unit Search;

interface

uses Windows, SysUtils, Classes, Graphics, Forms, Controls, StdCtrls, 
  Buttons, ExtCtrls;

type
  TSearchDlg = class(TForm)
    OKBtn: TButton;
    CancelBtn: TButton;
    Edit1: TEdit;
    Label1: TLabel;
    CheckBox1: TCheckBox;
    CheckBox2: TCheckBox;
    procedure OKBtnClick(Sender: TObject);
    procedure CancelBtnClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  SearchDlg: TSearchDlg;

implementation

uses MainForm;

{$R *.dfm}

procedure TSearchDlg.OKBtnClick(Sender: TObject);
begin
  MainEditor.SynEditSearch1.Sensitive:=Checkbox1.Checked;
  MainEditor.SynEditSearch1.Whole:=Checkbox2.Checked;
  MainEditor.SynEditSearch1.Pattern:=Edit1.Text;
  Close;
end;

procedure TSearchDlg.CancelBtnClick(Sender: TObject);
begin
  Close;
end;

procedure TSearchDlg.FormShow(Sender: TObject);
begin
  Edit1.SetFocus
end;

end.
