#pragma once


//////////////////////////////////////////////////////////////////////////
// ���������
//////////////////////////////////////////////////////////////////////////
namespace main
{
	//Ĭ��Ӧ�ó�������
#define MAIN_FILENAME						TEXT("")
#define MAIN_APPNAME						TEXT("")
#define MAIN_USERAGENT						""

	namespace Url
	{
#define MAIN_URL_HOME					TEXT("")
	}
}


//////////////////////////////////////////////////////////////////////////
// �������
//////////////////////////////////////////////////////////////////////////
namespace svc
{
#define	SVC_SERVER_VERSION					TEXT("")

#define SVC_GLOBAL_EVENTNAME				TEXT("")

#define SVC_SERVICEAPPNAME					TEXT("")
#define SVC_SERVICENAME						TEXT("")
#define SVC_SERVICEDISPLAYNAME				TEXT("")
#define SVC_SERVICEDESCRIPTION				TEXT("")

#define SVC_TIMEPATH						TEXT("\\time_svc_") SVC_SERVER_VERSION
#define SVC_VERSIONPATH						TEXT("\\version_svc_") SVC_SERVER_VERSION
#define SVC_TOKENPATH						TEXT("\\token_svc_") SVC_SERVER_VERSION

	namespace Url
	{
#define SVC_URL_TOKEN					TEXT("")
#define SVC_URL_VERSION					TEXT("")
#define SVC_URL_TIME					TEXT("")
#define SVC_REQUEST_FORMAT				TEXT("?ver=%s&osver=%s&osbuild=%s&ossp=%s&bit=%s&mid=%s&cid=%d&REV=%s")
	}
}


//////////////////////////////////////////////////////////////////////////
// ͳ�����
//////////////////////////////////////////////////////////////////////////
namespace report
{
#define REPORT_FILENAME						TEXT("Stats.exe")

	// Report����
#define REPORT_CMD_INSTALL					TEXT("install")
#define REPORT_CMD_UNINST					TEXT("uninst")
#define REPORT_CMD_STARTUP					TEXT("startup")
#define REPORT_CMD_AUTORUN					TEXT("autorun")
#define REPORT_CMD_APPEXIT					TEXT("appexit")
#define REPORT_CMD_UPDATE					TEXT("update")
#define REPORT_CMD_COVER					TEXT("cover")
#define REPORT_CMD_STATISTIC				TEXT("statistic")
#define REPORT_CMD_USERENV					TEXT("userenv")
#define REPORT_CMD_SVCSTART					TEXT("svcstart")
#define REPORT_CMD_BLOCKLIST				TEXT("blocklist")
#define REPORT_CMD_EXTSVC					TEXT("ext_svc")

	namespace Url
	{
#define REPORT_URL_INSTALL				""
#define REPORT_URL_STATS				""
#define REPORT_URL_USERENV				""
#define REPORT_URL_BLOCKLIST			""
	}
}

//////////////////////////////////////////////////////////////////////////
//  ��չ��ʶ
//////////////////////////////////////////////////////////////////////////
namespace update
{
#define UPDATE_FILENAME						TEXT("Update.exe")

#define UPDATE_CONFIGFILE					TEXT("\\_update")
#define UPDATE_SECTION						TEXT("update")
#define UPDATE_VERSION						TEXT("version")
#define UPDATE_INSPATH						TEXT("path")

	namespace Url
	{
#define UPDATE_URL						""
	}
}

//////////////////////////////////////////////////////////////////////////
//  ��չ��ʶ
//////////////////////////////////////////////////////////////////////////
namespace ext
{
	//��ƷΨһ��ʶ��
#define NC_PRODUCT_GUID						TEXT("")

	//////////////////////////////////////////////////////////////////////////
	//CLSID Ψһ��ʶ
#ifndef NC_PRODUCT_CLSID
#define NC_PRODUCT_CLSID
	// {7E25CCF6-BB17-4E6C-8C8E-05917E1D5B3F}
	DEFINE_GUID(CLSID_NC_ContextMenu,
		0x7e25ccf6, 0xbb17, 0x4e6c, 0x8c, 0x8e, 0x5, 0x91, 0x7e, 0x1d, 0x5b, 0x3f);
#endif //NC_PRODUCT_CLSID

	//////////////////////////////////////////////////////////////////////////
	//�����ļ��Ҽ��˵�
#define NC_ATTACH_TYPE_FORMAT				TEXT("%s\\shellex\\ContextMenuHandlers\\%s")
	//����ԱȨ����չ֧��
#define NC_APP_PROVED_KEY_PATH				TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved")
	//�������URL��ʽ
#define NC_BUNDLE_RQ_FORMAT					TEXT("")
}


//////////////////////////////////////////////////////////////////////////
//  ע����ʶ
//////////////////////////////////////////////////////////////////////////
namespace reg
{
#define REG_APPROOT							HKEY_LOCAL_MACHINE
	//��Ʒע�������
#define REG_PRODUCT_MAIN_KEY				TEXT("")
	//��·��
#define REG_APPPATH							TEXT("")
	//��Ʒ�Ҽ��˵�����
#define REG_PRODUCT_MENU_HELP_TEXT			TEXT("")
	//��չ����
#define REG_SHELL_EXT_NAME					TEXT("")
	//Ĭ�Ϲ�������
#define REG_DEFAULT_ASSOC_NAME				TEXT("")
	//Ӧ�ó���װĿ¼ ����
#define REG_DEFAULT_APP_INSTALL_PATH_KEY	TEXT("")
	//��ȫ����־
#define REG_SECURITY_CHECK_MARK				TEXT("Version")

#define REG_MID								TEXT("mid")
#define REG_PREMID							TEXT("pmid")
#define REG_AUT								TEXT("aut")
#define REG_CODE							TEXT("code")
#define REG_PREVER							TEXT("prever")
#define REG_MCN								TEXT("mcn")
#define REG_AUTPWD							TEXT("Jizip.com2015")
#define REG_MAXMCN							5
#define REG_DEFCODE							6000

#define REG_DISKFIRMWARE					TEXT("set_0001")
#define REG_DISKMODE						TEXT("set_0002")
#define REG_DISKSERIAL						TEXT("set_0003")
#define REG_DISKROTATION					TEXT("set_0004")
#define REG_PASSWORD						"K!M$Y"
}


namespace ini
{
#define INI_MAINNODE						"config"
#define INI_VERSION							"version"
#define INI_CODE							"code"
#define INI_FILENAME						"version.dat"
#define INI_PASSWORD						""
}
