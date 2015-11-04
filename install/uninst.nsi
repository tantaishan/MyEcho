
!addplugindir "plugins"

/* 包含头文件 */
!include "nsDialogs.nsh"
!include "FileFunc.nsh"
!include "MUI.nsh"
!include "LogicLib.nsh"
!include "WinMessages.nsh"
!include "MUI2.nsh"
!include "WordFunc.nsh"
!include "Library.nsh"
!include "LogicLib.nsh"
;!include "UAC.nsh"
!include "include\StrStr.nsh"
!include "uninst.nsh"

/* MUI设置*/
!define MUI_ABORTWARNING
!define MUI_ICON			"resource\install.ico"
!define MUI_UNICON			"resource\uninst.ico"

/* 参数变量赋值 */
Name				"${PRODUCT_NAME_CN} ${PRODUCT_VERSION}"
InstallDir			"$PROGRAMFILES\${PRODUCT_NAME_EN}"
InstallDirRegKey	HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails		hide
ShowUnInstDetails	hide
AutoCloseWindow		true
RequestExecutionLevel admin

!insertmacro MUI_LANGUAGE "SimpChinese"
;!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

SilentInstall silent
SilentUninstall silent

Section "MainSection" SEC01
	WriteUninstaller "$TEMP\Uninst.exe"
SectionEnd

/* 卸载区段 */
Section Uninstall
	SetShellVarContext all
	
	KillProcDLL::KillProc "MyEcho.exe"
	
	Delete "$DESKTOP\${PRODUCT_NAME_CN}.lnk" 
	RMDir /r "$SMPROGRAMS\${PRODUCT_NAME_CN}"
	RMDir /r "$INSTDIR"

	DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
	DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}" 
	;DeleteRegKey HKLM "${PRODUCT_SUB_KEY}"
	SetAutoClose true
SectionEnd

Function un.onInit
	System::Call "kernel32::CreateMutex(i 0, i 0, t '$(^Name)') i .r0 ?e"
	Pop $0
	StrCmp $0 0 launch
	StrLen $0 "$(^Name)"
	IntOp $0 $0 + 1
loop:
	FindWindow $1 '#32770' '' 0 $1
	IntCmp $1 0 +4
	System::Call "user32::GetWindowText(i r1, t .r2, i r0) i."
	StrCmp $2 "$(^Name)" 0 loop
	System::Call "user32::SetForegroundWindow(i r1) i."
	Abort
launch:
	ReadRegStr $0 ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "instdir"
		${If} $0 != ""
			StrCpy $INSTDIR $0
		${EndIf}
	MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "您确实要完全移除 $(^Name) ，及其所有的组件？" IDYES +2
	Abort
check:
 FindProcDLL::FindProc "${MAIN_PROC}"
	Pop $R0
	IntCmp $R0 1 0 NoRun
	MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "您正在运行${PRODUCT_NAME_CN}，是否强制关闭？" IDYES RetryUnInstall IDNO NotUnInstall
		RetryUnInstall:
			KillProcDLL::KillProc "${MAIN_PROC}"
		goto NoRun
		NotUnInstall:
			Quit
	NoRun:
FunctionEnd

Function un.onUninstSuccess
	HideWindow
	MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) 已成功地从您的计算机移除。"
FunctionEnd