@echo off
rem set/p pfx=请输入证书文件名:
rem set/p pwd=请输入密码:

if not exist bin md bin
if not exist tmp md temp
xcopy ..\bin\*.* bin /s /e /y
nsis\NSIS_Unicode\App\NSIS\makensis.exe /X"OutFile temp\build_uninst.exe" /X"SetCompressor /FINAL lzma" uninst.nsi
temp\build_uninst.exe
copy /y "%TEMP%\Uninst.exe" "bin\"
del /f /q "%TEMP%\Uninst.exe"

set "ymd=%date:~,4%-%date:~5,2%-%date:~8,2%"
nsis\NSIS_Unicode\App\NSIS\makensis.exe /X"OutFile MyEcho_%ymd%.exe" /X"SetCompressor /FINAL lzma" install.nsi
rd /q /s bin
rd /q /s temp
rem pause