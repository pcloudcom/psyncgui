copy "C:\Users\MobileInno\Desktop\sync\psyncfsgui\psyncgui\build-psyncgui-qt_static-Debug\debug\psyncgui.exe" .\pCloud.exe /Y
strip pCloud.exe

call sign.bat pCloud.exe

"C:\Program Files\NSIS\makensis.exe" pCloud.nsi
call sign.bat PCloudInstall.exe

del inst.exe

pause
