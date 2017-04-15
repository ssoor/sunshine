unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls;

type
  TForm1 = class(TForm)
    Label1: TLabel;
    Edit1: TEdit;
    Button1: TButton;
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}

procedure TForm1.Button1Click(Sender: TObject);
begin
 {$I VMProtectBegin.inc}
 if StrToIntDef(Edit1.Text, 0) mod 17=13 then
  MessageDlg('Correct password', mtInformation, [mbOK], 0)
 else
  begin
   MessageDlg('Incorrect password', mtError, [mbOK], 0);
   Edit1.SetFocus;
  end;
 {$I VMProtectEnd.inc}
end;

end.
