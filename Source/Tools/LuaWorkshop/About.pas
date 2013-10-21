unit About;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, OleCtrls, ShockwaveFlashObjects_TLB, ExtCtrls, jpeg, StdCtrls,
  NetLabel;

type
  TAboutForm = class(TForm)
    Bevel1: TBevel;
    Image1: TImage;
    Image2: TImage;
    Label1: TLabel;
    Label2: TLabel;
    NetLabel1: TNetLabel;
    NetLabel2: TNetLabel;
    Label3: TLabel;
    procedure FormClick(Sender: TObject);
    procedure Image1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  AboutForm: TAboutForm;

implementation

{$R *.dfm}

procedure TAboutForm.FormClick(Sender: TObject);
begin
  Close;
end;

procedure TAboutForm.Image1Click(Sender: TObject);
begin
  Close;
end;

end.
