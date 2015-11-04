
!include "version.nsh"

/* 宏定义 */
!define PRODUCT_NAME_CN			"MyEcho"
!define PRODUCT_NAME_EN			"MyEcho"
!define SETUP_NAME				"MyEcho安装程序"
!define PRODUCT_PUBLISHER		"Wuhan Transn Technology Co., LTD."
!define PRODUCT_WEB_SITE		"http://www.transn.com"
!define PRODUCT_DIR_REGKEY 		"Software\Microsoft\Windows\CurrentVersion\App Paths\CoolKan.exe"
!define PRODUCT_UNINST_KEY 		"Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME_EN}"
!define PRODUCT_UNINST_ROOT_KEY	"HKLM "
!define PRODUCT_ROOT_KEY		"HKLM"
!define PRODUCT_SUB_KEY			"SOFTWARE\MyEcho"
!define COMPANY_NAME			"Wuhan Transn Technology Co., LTD."
!define LEGAL_COPYRIGHT			"(C)Wuhan Transn Technology Co., LTD. All Rights Reserverd."
;!define INTERNAL_NAME			"setup.exe"
!define ORIGINAL_FILENAME		"Setup.exe"
!define MAIN_PROC				"MyEcho.exe"

/* 版本信息 */
VIAddVersionKey ProductName "${SETUP_NAME}"					;产品名称
VIProductVersion "${PRODUCT_VERSION}"						;版本号，格式为 X.X.X.X (若使用则本条必须)
VIAddVersionKey FileDescription "${SETUP_NAME}"				;文件描述(标准信息)
VIAddVersionKey FileVersion "${PRODUCT_VERSION}"			;文件版本(标准信息)
VIAddVersionKey ProductVersion "${PRODUCT_VERSION}"			;产品版本
;VIAddVersionKey Comments "${Name} ${Ver}"					;备注
VIAddVersionKey CompanyName "${COMPANY_NAME}"				;公司名
VIAddVersionKey LegalCopyright "${LEGAL_COPYRIGHT}"			;合法版权
;VIAddVersionKey InternalName "${INTERNAL_NAME}"			;内部名称
;VIAddVersionKey LegalTrademarks "${PRODUCT_PUBLISHER}"		;合法商标
VIAddVersionKey OriginalFilename "${ORIGINAL_FILENAME}"		;源文件名
;VIAddVersionKey PrivateBuild "XX"							;个人内部版本说明
;VIAddVersionKey SpecialBuild "XX"							;特殊内部本本说明
