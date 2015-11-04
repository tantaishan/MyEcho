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
	ClientAPI_OK = 0,				//成功
	ClientAPI_Failed = 1,				//成功
	ClientAPI_Error_Network = 0x1001,	//网络请求错误
	ClientAPI_Error_Json = 0x1002,		//Json解析错误
	ClientAPI_Error_OpenFile = 0x1001,	//打开文件失败
};

enum UpdateType
{
	UPDATE_NORMAL = 1,
	UPDATE_FORCED = 2,
};

typedef struct tagOSEnvInfo
{
	tstring	os_ver;			// 操作系统版本
	tstring	os_bit;			// 操作系统位数
	tstring	ie_ver;			// IE版本
	tstring	dis_pix;		// 屏幕分辨率

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
// 报错报告数据结构

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
// 发送安装卸载数据结构体

typedef struct tagBaseRptData
{
	tstring		mid;	// 机器码
	tstring		aut;	// 验证码
	int				cid;	// 渠道号
	tstring		ver;	// 版本号
	int				stu;	// 服务状态
	int				mcn;	// mid更改次数
	std::wstring	pmid;	// 上一个mid
} BaseRptData;

enum emInstRptType
{
	InstRptType_NormalStart = 0,	// 正常启动
	InstRptType_AutoStart,			// 开机启动
	InstRptType_NewInstall,			// 新安装
	InstRptType_Update,				// 更新
	InstRptType_Uninstall,			// 卸载
	InstRptType_CoverInst,			// 覆盖安装
	InstRptType_AppExit,			// 程序退出
	InstRptType_SvcStart,			// 服务运行
	InstRptType_ExtSvcStart,		// 扩展和服务状态
};


typedef struct tagInstRptData
{
	BaseRptData		baseData;	// 基本数据
	OSEnvInfo		envinfo;
	tstring		date;		// 时间戳
	emInstRptType	type;		// 发送类型
	tstring		other;		// 其他数据
} InstRptData;

//////////////////////////////////////////////////////////////////////////
// 发送客户端用户行为统计数据结构体

typedef struct tagStatisticRptData
{
	BaseRptData		baseData;	// 基本数据
	tstring		actions;	// 行为
} StatisticRptData;

//////////////////////////////////////////////////////////////////////////
// 获取硬盘序列号结构体

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
// 用户环境信息
typedef struct _UserEnvInfo
{
	BaseRptData base;
	Json::Value env;

} UserEnvInfo;

//////////////////////////////////////////////////////////////////////////
// 网卡信息
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
// 进程信息
typedef struct _ProcessInfo
{
	DWORD		dwProcessID;	// PID
	tstring		strProcName;	// Process Name
	tstring		strProcPath;	// Process Full Path

} ProcessInfo;

//////////////////////////////////////////////////////////////////////////
// 磁盘信息
typedef struct _DriverInfo
{
	std::wstring	strPartition;	// 磁盘分区
	DWORD			dwSerialNumber;	// 序列号
	std::wstring	strFileSysName;	// 文件系统
	DOUBLE	totalSpace;		// 总空间
	DOUBLE	userSpace;		// 已用空间
	DOUBLE	freeSpace;		// 空闲空间

} DriverInfo;

///////////////////////////////////////////////////////////////////////////
// 服务状态
namespace SERVICESTATE
{
	const int FAILED = 0;		// 查询错误
	const int NOTEXIST = 1;		// 不存在
	const int INACTIVE = 2;		// 未激活
	const int ACTIVATED = 3;	// 已激活

	const int SCM_LOADAPI_FAILED = 4;	// 加载SCM DLL或函数失败
	const int SCM_OPEN_FAILED = 5;		// SCM打开失败
	const int SERVICE_OPEN_FAILED = 6;	// 服务打开失败
	const int SERVICE_QUERY_FAILED = 7;	// 服务查询失败
	const int SERVICE_BOOTSTART = 8;	// 服务boost start
	const int SERVICE_SYSTEMSTART = 9;	// 服务system start
	const int SERVICE_AUTOSTART = 10;	// 服务自动启动
	const int SERVICE_MANUAL = 11;		// 服务手动启动
	const int SERVICE_DISABLE = 12;		// 服务禁用
};

namespace SERVICE_STARTMODE
{
	const int FAILED = 0;	// 查询错误
	const int MANUAL = 1;	// 手动
	const int AUTO = 2;		// 自动
	const int DISABLED = 3;	// 禁用
	const int UNKNOWN = 4;	// 未知
}