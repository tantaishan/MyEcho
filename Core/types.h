#pragma once
#include <wtypes.h>
#include "string/tstring.h"
#include "json/json.h"

#ifdef _UNICODE

typedef	std::wstring		hsstring;
typedef	wchar_t*			hststr;
typedef	const wchar_t*		hsctstr;
typedef	wchar_t				hstchar;

#else

typedef	std::string			hsstring;
typedef	char*				hststr;
typedef	const char*			hsctstr;
typedef	char				hstchar;

#endif

enum ClientAPI_ErrorCode
{
	ClientAPI_OK = 0,				//�ɹ�
	ClientAPI_Failed = 1,				//�ɹ�
	ClientAPI_Error_Network = 0x1001,	//�����������
	ClientAPI_Error_Json = 0x1002,		//Json��������
	ClientAPI_Error_OpenFile = 0x1001,	//���ļ�ʧ��
};

enum UpdateType
{
	UPDATE_NORMAL = 1,
	UPDATE_FORCED = 2,
};

typedef struct tagOSEnvInfo
{
	tstring	os_ver;			// ����ϵͳ�汾
	tstring	os_bit;			// ����ϵͳλ��
	tstring	ie_ver;			// IE�汾
	tstring	dis_pix;		// ��Ļ�ֱ���

} OSEnvInfo;

typedef struct tagUpdateInfo
{
	OSEnvInfo os_info;
	tstring version_number;
	tstring version_explain;
	UpdateType update_type;
	tstring file_32;

} UpdateInfo;

//////////////////////////////////////////////////////////////////////////
// ���������ݽṹ

typedef struct _ReportErrData
{
	tstring	uid;
	tstring	content;
	tstring	contract;
	tstring	client_version;
	tstring	os_version;
	tstring	ie_version;
	tstring	attachment_path;

} ReportErrData;

//////////////////////////////////////////////////////////////////////////
// ���Ͱ�װж�����ݽṹ��

typedef struct tagBaseRptData
{
	tstring		mid;	// ������
	tstring		aut;	// ��֤��
	int				cid;	// ������
	tstring		ver;	// �汾��
	int				stu;	// ����״̬
	int				mcn;	// mid���Ĵ���
	std::wstring	pmid;	// ��һ��mid
} BaseRptData;

enum emInstRptType
{
	InstRptType_NormalStart = 0,	// ��������
	InstRptType_AutoStart,			// ��������
	InstRptType_NewInstall,			// �°�װ
	InstRptType_Update,				// ����
	InstRptType_Uninstall,			// ж��
	InstRptType_CoverInst,			// ���ǰ�װ
	InstRptType_AppExit,			// �����˳�
	InstRptType_SvcStart,			// ��������
	InstRptType_ExtSvcStart,		// ��չ�ͷ���״̬
};


typedef struct tagInstRptData
{
	BaseRptData		baseData;	// ��������
	OSEnvInfo		envinfo;
	tstring		date;		// ʱ���
	emInstRptType	type;		// ��������
	tstring		other;		// ��������
} InstRptData;

//////////////////////////////////////////////////////////////////////////
// ���Ϳͻ����û���Ϊͳ�����ݽṹ��

typedef struct tagStatisticRptData
{
	BaseRptData		baseData;	// ��������
	tstring		actions;	// ��Ϊ
} StatisticRptData;

//////////////////////////////////////////////////////////////////////////
// ��ȡӲ�����кŽṹ��

typedef	struct	_DISK_INPUT_DATA
{
	ULONG	BufferSize;
	UCHAR	FeaturesReg;
	UCHAR	SectorCountReg;
	UCHAR	SectorNumberReg;
	UCHAR	CylLowReg;
	UCHAR	CylHighReg;
	UCHAR	DriveHeadReg;
	UCHAR	CommandReg;
	UCHAR	Reserved0;
	UCHAR	DriveNumber; 
	UCHAR	Reserved1[3];
	ULONG	Reserved2[4];
	//UCHAR	InBuffer[0]; 
} DISK_INPUT_DATA,*PDISK_INPUT_DATA;

typedef	struct	_DISK_OUTPUT_DATA
{
	ULONG	BufferSize;
	UCHAR	DriverError;
	UCHAR	IDEError;
	UCHAR	Reserved0[2];
	ULONG	Reserved1[2];
	//UCHAR	Buffer[0];  
} DISK_OUTPUT_DATA,*PDISK_OUTPUT_DATA;

//////////////////////////////////////////////////////////////////////////
// �û�������Ϣ
typedef struct _UserEnvInfo
{
	BaseRptData base;
	Json::Value env;

} UserEnvInfo;

//////////////////////////////////////////////////////////////////////////
// ������Ϣ
typedef struct _IpAddressInfo
{
	std::string ipAddress;
	std::string ipMask;
	std::string gateway;

	const _IpAddressInfo& operator=(const _IpAddressInfo& that)
	{
		ipAddress = that.ipAddress;
		ipMask = that.ipMask;
		gateway = that.gateway;
		return *this;
	}

} IpAddressInfo;

typedef struct _NetworkAdapterInfo
{
	std::string adapterName;
	std::string description;
	std::string type;
	std::wstring address;
	std::vector<IpAddressInfo> ipList;
} NetworkAdapterInfo;

//////////////////////////////////////////////////////////////////////////
// ������Ϣ
typedef struct _ProcessInfo
{
	DWORD		dwProcessID;	// PID
	tstring		strProcName;	// Process Name
	tstring		strProcPath;	// Process Full Path

} ProcessInfo;

//////////////////////////////////////////////////////////////////////////
// ������Ϣ
typedef struct _DriverInfo
{
	std::wstring	strPartition;	// ���̷���
	DWORD			dwSerialNumber;	// ���к�
	std::wstring	strFileSysName;	// �ļ�ϵͳ
	DOUBLE	totalSpace;		// �ܿռ�
	DOUBLE	userSpace;		// ���ÿռ�
	DOUBLE	freeSpace;		// ���пռ�

} DriverInfo;

///////////////////////////////////////////////////////////////////////////
// ����״̬
namespace SERVICESTATE
{
	const int FAILED = 0;		// ��ѯ����
	const int NOTEXIST = 1;		// ������
	const int INACTIVE = 2;		// δ����
	const int ACTIVATED = 3;	// �Ѽ���

	const int SCM_LOADAPI_FAILED = 4;	// ����SCM DLL����ʧ��
	const int SCM_OPEN_FAILED = 5;		// SCM��ʧ��
	const int SERVICE_OPEN_FAILED = 6;	// �����ʧ��
	const int SERVICE_QUERY_FAILED = 7;	// �����ѯʧ��
	const int SERVICE_BOOTSTART = 8;	// ����boost start
	const int SERVICE_SYSTEMSTART = 9;	// ����system start
	const int SERVICE_AUTOSTART = 10;	// �����Զ�����
	const int SERVICE_MANUAL = 11;		// �����ֶ�����
	const int SERVICE_DISABLE = 12;		// �������
};

namespace SERVICE_STARTMODE
{
	const int FAILED = 0;	// ��ѯ����
	const int MANUAL = 1;	// �ֶ�
	const int AUTO = 2;		// �Զ�
	const int DISABLED = 3;	// ����
	const int UNKNOWN = 4;	// δ֪
}