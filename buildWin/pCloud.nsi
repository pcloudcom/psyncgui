Name "pCloud"

InstallDir "$PROGRAMFILES\pCloud Drive"
RequestExecutionLevel admin
Caption "pCloud Drive application Installer"
InstallDirRegKey HKCU "Software\pCloud\pCloudInstall" "Install_Dir"
LicenseText "Terms of service: "
LicenseData "license.rtf"

ShowInstDetails nevershow
ShowUninstDetails nevershow

;--------------------------------

; Pages
Page license
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles
;--------------------------------

!include nsProcess.nsh
!include LogicLib.nsh
!include x64.nsh
!include WinVer.nsh

!ifdef INNER
  OutFile "inst.exe"
!else
  !system '$\"${NSISDIR}\makensis$\" /DINNER pCloud.nsi' = 0
  !system "inst.exe" = 2

  !system 'sign.bat "$%TEMP%\pcloud-uninst.exe"' = 0

  OutFile "PCloudInstall.exe"
!endif

Function .onInit
!ifdef INNER
  WriteUninstaller "$%TEMP%\pcloud-uninst.exe"
  Quit
!endif

  IntOp $0 ${SF_SELECTED} | ${SF_RO}

  ${If} ${RunningX64}
    ${If} ${IsWin2003}
    ${ElseIf} ${IsWinVista}
    ${ElseIf} ${IsWin2008}
    ${ElseIf} ${IsWin2008R2}
    ${ElseIf} ${IsWin7}
	${ElseIf} ${IsWin8}
    ${Else}
      MessageBox MB_OK "Your OS is not supported. pCloud supports Windows 2003, Vista, 2008, 2008R2, Win7 and 8 for x64."
      Abort
    ${EndIf}
  ${Else}
    ${If} ${IsWinXP}
    ${ElseIf} ${IsWin2003}
    ${ElseIf} ${IsWinVista}
    ${ElseIf} ${IsWin2008}
    ${ElseIf} ${IsWin7}
	${ElseIf} ${IsWin8}
    ${Else}
      MessageBox MB_OK "Your OS is not supported. pCloud supports Windows XP, 2003, Vista, 2008, Win7 and 8 for x86."
      Abort
    ${EndIf}
  ${EndIf}
FunctionEnd


Section "Install"
  SetOutPath $INSTDIR
  IfFileExists "$INSTDIR\pCloud.exe" Installed

  ReadRegStr $R3 HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PSync" "UninstallString"
;  MessageBox MB_OK "psync installer found: $R3"
	StrCpy $R4 $R3 "" 1
	StrCpy $R5 $R4 -1
 ;MessageBox MB_OK "psync installer found: r4 $R4 r5 $R5"
/*${If} ${FileExists} "$R3"
	nsExec::Exec $R3
	MessageBox MB_OK "psync installer found $R3"
${Else}
	MessageBox MB_OK "psync installer not found $R3"
${EndIf}
*/

 IfFileExists $R5 UninstPsync	

  WriteRegStr HKLM "SOFTWARE\PCloud\pCloud" "Install_Dir" "$INSTDIR"

  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\PCloud" "DisplayName" "pCloud Drive
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\PCloud" "UninstallString" '"$INSTDIR\pcloud-uninst.exe"'
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\PCloud" "NoModify" 1
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\PCloud" "NoRepair" 1

!ifndef INNER
  File "$%TEMP%\pcloud-uninst.exe"
!endif
  
  File libeay32.dll
  File libgcc_s_dw2-1.dll
  File libssl32.dll
  File libstdc++-6.dll
  File libwinpthread-1.dll
  File pthreadGC2.dll
  File pthreadGCE2.dll
  File ssleay32.dll
  File SimpleExt.dll 
  File MSVCR100D.dll
  File MSVCP100D.dll
  File pcloud.ico
	
  
  ${If} ${IsWinXP}
    File .\XP\VCRedist.exe
    MessageBox MB_OK|MB_ICONINFORMATION "The redistributable package will be installed. It could take several minutes. Please be patient."
    nsExec::Exec '"$INSTDIR\VCRedist.exe" /q'
  ${EndIf}
  
  File DokanInstall.exe
  File pCloud.exe
  
  ClearErrors
  nsExec::Exec '"$INSTDIR\DokanInstall.exe" /S'
  IfErrors 0 noError
    MessageBox MB_OK|MB_ICONEXCLAMATION "There is a problem installing Dokan driver."
    Quit
  noError:
	
  Delete  "$INSTDIR\DokanInstall.exe"

  CreateDirectory "$SMPROGRAMS\pCloud Drive"
  CreateShortCut "$SMPROGRAMS\pCloud Drive\pCloud Drive.lnk" "$INSTDIR\pCloud.exe" "" ""
  CreateShortCut "$DESKTOP\pCloud Drive.lnk" "$INSTDIR\pCloud.exe" "" ""
  CreateShortCut "$SMPROGRAMS\pCloud Drive\uninstall.lnk" "$INSTDIR\pcloud-uninst.exe" "" ""

  MessageBox MB_YESNO|MB_ICONQUESTION "Do you want to run pCloud automatically when Windows starts?" IDNO NoStartup
	WriteRegStr "HKLM" "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "pCloud" "$INSTDIR\pCloud.exe"
  NoStartup:

!ifndef INNER
  nsExec::Exec 'regsvr32 "$INSTDIR\SimpleExt.dll" /s'
;  Exec "$INSTDIR\pSync.exe"
!endif

  MessageBox MB_YESNO|MB_ICONQUESTION "A computer restart is required. Do you want to restart now?" IDNO NoReboot
    Reboot
  NoReboot:
  Quit

  UninstPsync:
  MessageBox MB_YESNO|MB_ICONQUESTION "There is a pSync already installed on your system. Uninstall old version now?" IDNO NoReboot
  nsExec::Exec $R5
   Exec "$EXEPATH"
   Quit

  Installed:
    MessageBox MB_YESNO|MB_ICONQUESTION "There is a pCloud already installed on your system. Uninstall old version now?" IDNO NoReboot
    nsExec::Exec $INSTDIR\pcloud-uninst.exe
    Exec "$EXEPATH"
    Quit

SectionEnd ; end the section

UninstallText "This will uninstall pCloud. Hit next to continue."

!ifdef INNER
Section "Uninstall"

  ${nsProcess::FindProcess} "pCloud.exe" $R0
  StrCmp $R0 0 0 +2
  MessageBox MB_YESNO|MB_ICONEXCLAMATION 'The pCloud application is running. It will be closed.' IDNO NoStop
  ${nsProcess::KillProcess} "pCloud.exe" $R0

  Delete "$SMPROGRAMS\pCloud Drive\pCloud Drive.lnk"
  Delete "$DESKTOP\pCloud Drive.lnk"
  Delete "$SMPROGRAMS\pCloud Drive\uninstall.lnk"
  RMDir "$SMPROGRAMS\pCloud Drive"

  DeleteRegKey   HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\pCloud"
  DeleteRegKey   HKCU "SOFTWARE\pCloud"
  DeleteRegKey 	 HKLM "SOFTWARE\pCloud"
  DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "pCloud"

  
  ${If} ${IsWinXP}
	MessageBox MB_OK|MB_ICONINFORMATION "The redistributable package will be removed. It could take several minutes. Please be patient."
    nsExec::Exec '"$INSTDIR\VCRedist.exe" /qu'
  ${EndIf}
    
  ${nsProcess::FindProcess} "explorer.exe" $R1
  ${nsProcess::KillProcess} "explorer.exe" $R1
  nsExec::Exec  'regsvr32 "$INSTDIR\SimpleExt.dll" /u /s' = 0

  Delete "$INSTDIR\*.*"
  RMDir "$INSTDIR"
  Exec '"$PROGRAMFILES\Dokan\DokanLibrary\DokanUninstall.exe" /S'
  ;nsExec::Exec "explorer.exe"	
  System::Call "explorer.exe"

NoStop:
  Quit
SectionEnd
!endif
