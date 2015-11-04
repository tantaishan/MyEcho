
/* 全局变量 */
!define FALSE "0"
!define TRUE "1"
Var PreVer
Var bCover

/* 添加插件目录 */
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
!include "install.nsh"

/* MUI设置*/
!define MUI_ABORTWARNING
!define MUI_ICON			"resource\install.ico"
!define MUI_UNICON			"resource\uninst.ico"

/* 参数变量赋值 */
Name					"${PRODUCT_NAME_CN} ${PRODUCT_VERSION}"
InstallDir				"$PROGRAMFILES\${PRODUCT_NAME_EN}"
InstallDirRegKey		HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails			show
ShowUnInstDetails		show
AutoCloseWindow			true
RequestExecutionLevel	admin

/* 安装页 */
; Welcome page
;!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "resource\Licence.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\${MAIN_PROC}"
;!insertmacro MUI_PAGE_FINISH
; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES
; Language files
!insertmacro MUI_LANGUAGE "SimpChinese"
; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

Function KillActiveProc
	 /* 关闭所有辅助进程 */
	KillProcDLL::KillProc "MyEcho.exe"
	;Sleep 500
FunctionEnd

Function .onInit
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
	StrCpy $bCover ${FALSE}
	ReadRegStr $0 HKLM ${PRODUCT_SUB_KEY} "mid"
	${If} $0 != ""
		StrCpy $bCover ${TRUE}
	${EndIf}
	
	${GetParameters} $R0
	${StrStr} $R1 $R0 "/S"
	${If} $R1 != ""
		Goto SlientInst
	${Else}
		Goto NorInst
	${EndIf}
NorInst:
	Goto CheckCover
	Goto CheckRun
CheckCover:
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion"
	${If} $0 != ""
		StrCpy $PreVer $0
		Var /GLOBAL localCheckVersion
		${VersionCompare} "$0" "${PRODUCT_VERSION}" $localCheckVersion
		${If} $localCheckVersion == "0"
		StrCmp $localCheckVersion "0" 0 CheckRun
		MessageBox MB_YESNO "您已经安装当前版本的${PRODUCT_NAME_CN}，是否覆盖安装？" IDYES true IDNO false
			true:
				;StrCpy $bCover ${TRUE}
				Goto CheckRun
			false:
				Quit
		${Else}
			Goto CheckRun
		${EndIf}
	${EndIf}
	
CheckRun:
	StrCpy $R0 ""
	FindProcDLL::FindProc "${MAIN_PROC}"
	Pop $R0 
	IntCmp $R0 1 0 NoRun
	MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "您正在运行MyEcho，是否强制关闭？" IDYES RetryInstall IDNO NotInstall
		RetryInstall:
			KillProcDLL::KillProc "${MAIN_PROC}"
		goto NoRun
		NotInstall:
			Quit
	NoRun:
		Call KillActiveProc
SlientInst:
	FindProcDLL::FindProc "${MAIN_PROC}"
	Pop $R0
	IntCmp $R0 1 0 NoRun1
	KillProcDLL::KillProc "${MAIN_PROC}"
	NoRun1:
		Call KillActiveProc
FunctionEnd
	
Function .onInstSuccess

FunctionEnd

/* 安装区段 */
Section "MainSection" SEC01
	SetOutPath "$INSTDIR"
	SetOverwrite try
	File "bin\*.exe"
	File "bin\*.dll"
	File "bin\*.dat"
	File "bin\*.pak"
	SetOutPath "$INSTDIR\locales"
	File "bin\locales\*.*"
	SetOutPath "$INSTDIR\html"
	File /r "bin\html\*.*"
SectionEnd
	
Section -AdditionalIcons
	SetShellVarContext all
	CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME_CN}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME_CN}\${PRODUCT_NAME_CN}.lnk" "$INSTDIR\${MAIN_PROC}"
	CreateShortCut "$DESKTOP\${PRODUCT_NAME_CN}.lnk" "$INSTDIR\${MAIN_PROC}"
	;WriteIniStr "$INSTDIR\${PRODUCT_NAME_CN}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
	;CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME_CN}\Website.lnk" "$INSTDIR\${PRODUCT_NAME_CN}.url"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME_CN}\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
	SetShellVarContext all
	;Exec '"$INSTDIR\${COOLKAN_SVC}" -s'
	WriteRegStr HKLM "${PRODUCT_SUB_KEY}" "prever" $PreVer
	;WriteRegStr HKLM "${PRODUCT_SUB_KEY}" "version" "${PRODUCT_VERSION}"
	WriteRegStr HKLM "${PRODUCT_SUB_KEY}" "instdir" $INSTDIR
	;WriteUninstaller "$INSTDIR\uninst.exe"
	WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\${MAIN_PROC}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\CoolKan.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd