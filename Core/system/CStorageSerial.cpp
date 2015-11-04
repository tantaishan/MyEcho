/*--------------------------------------------------------------------------------*/
#include "CStorageSerial.h"
#include <Shlwapi.h>
#include <tchar.h>
#include "../types.h"

#pragma comment(lib, "shlwapi.lib")


/*--------------------------------------------------------------------------------*/
CStorageSerial::CStorageSerial()
{

}
/*--------------------------------------------------------------------------------*/
CStorageSerial::~CStorageSerial()
{


}
/*--------------------------------------------------------------------------------*/
BOOL	CStorageSerial::QuerySystemVolume(LPTSTR Volume,ULONG VolumeLen)
{
	BOOL	Result = FALSE;
	LPTSTR	Backslash = NULL;
	TCHAR	SystemDirectory[256] = {0};

	GetSystemDirectory(SystemDirectory,sizeof(SystemDirectory)/sizeof(TCHAR));
	PathStripToRoot(SystemDirectory);
	Backslash = _tcsstr(SystemDirectory,_T("\\"));
	if(Backslash)
	{
		*Backslash = TCHAR(0);
		_tcscpy_s(Volume,VolumeLen,SystemDirectory);
		Result = TRUE;
	}
	return	(Result);
}
/*--------------------------------------------------------------------------------*/
BOOL	CStorageSerial::QuerySystemStorage(LPTSTR Volume,PULONG DeviceNumber)
{
	BOOL	Result = FALSE;
	HANDLE	FileHandle = INVALID_HANDLE_VALUE;
	ULONG	NumberOfReturnBytes = 0;
	TCHAR	DeviceName[64] = {0};
	STORAGE_DEVICE_NUMBER	StorageDeviceNumber = {0};

	if(IsUserAnAdmin())
	{
		_stprintf_s(DeviceName,sizeof(DeviceName)/sizeof(TCHAR),_T("\\\\.\\%s"),Volume);
		FileHandle = CreateFile(DeviceName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,NULL);
		if(FileHandle != INVALID_HANDLE_VALUE)
		{
			Result = DeviceIoControl(FileHandle,
				IOCTL_STORAGE_GET_DEVICE_NUMBER,
				NULL,
				0,
				&StorageDeviceNumber,
				sizeof(StorageDeviceNumber),
				&NumberOfReturnBytes,
				NULL);
			if(Result)
			{
				*DeviceNumber = StorageDeviceNumber.DeviceNumber;
			}
			CloseHandle(FileHandle);
		}
	}
	return	(Result);
}
/*--------------------------------------------------------------------------------*/
BOOL	CStorageSerial::QuerySerialEx(ULONG DeviceNumber,PVOID SerialBase,PULONG SerialSize)
{
	BOOL	Result = FALSE;
	HANDLE	FileHandle = INVALID_HANDLE_VALUE;
	ULONG	NumberOfReturnBytes = 0;
	TCHAR	DeviceName[64] = {0};
	PUCHAR	DiskSerialPtr = NULL;
	UCHAR	ProcessStr = NULL;
	UCHAR	InputData[1024] = {0};
	UCHAR	OutputData[1024] = {0};

	if(IsUserAnAdmin())
	{
		_stprintf_s(DeviceName,sizeof(DeviceName)/sizeof(TCHAR),_T("\\\\.\\PhysicalDrive%d"),DeviceNumber);
		FileHandle = CreateFile(DeviceName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,NULL);
		if(FileHandle != INVALID_HANDLE_VALUE)
		{
			((PDISK_INPUT_DATA)&InputData)->CommandReg = 0xEC;
			Result = DeviceIoControl(FileHandle,
				0x0007C088,
				(PVOID)&InputData,
				sizeof(DISK_INPUT_DATA),
				(PVOID)&OutputData,
				sizeof(DISK_OUTPUT_DATA) + 0x200,
				&NumberOfReturnBytes,
				NULL);
			if(Result)
			{
				//×ª»»ÎªINTELË³Ðò.
				DiskSerialPtr = ((PUCHAR)&OutputData + sizeof(DISK_OUTPUT_DATA) + 20);
				for(int i=0; i < 20; i+=2) 
				{ 
					ProcessStr = DiskSerialPtr[i]; 
					DiskSerialPtr[i] = DiskSerialPtr[i+1]; 
					DiskSerialPtr[i+1] = ProcessStr; 
				} 
				__try
				{
					if(*SerialSize > CStorageSerial::StorageSerialSize)
					{
						RtlMoveMemory((PVOID)SerialBase,(PVOID)DiskSerialPtr,CStorageSerial::StorageSerialSize);
						*SerialSize = CStorageSerial::StorageSerialSize;
						Result = TRUE;
					}
					else
					{
						Result = FALSE;
					}
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					Result = FALSE;
				}
				
			}
			CloseHandle(FileHandle);
		}

	}

	return	(Result);
}
/*--------------------------------------------------------------------------------*/
BOOL	CStorageSerial::QueryFirmwareRevisionEx(ULONG DeviceNumber,PVOID FirmwareRevisionExBase,PULONG FirmwareRevisionExSize)
{
	BOOL	Result = FALSE;
	HANDLE	FileHandle = INVALID_HANDLE_VALUE;
	ULONG	NumberOfReturnBytes = 0;
	TCHAR	DeviceName[64] = {0};
	PUCHAR	FirmwareRevisionPtr = NULL;
	UCHAR	ProcessStr = NULL;
	UCHAR	InputData[1024] = {0};
	UCHAR	OutputData[1024] = {0};

	if(IsUserAnAdmin())
	{
		_stprintf_s(DeviceName,sizeof(DeviceName)/sizeof(TCHAR),_T("\\\\.\\PhysicalDrive%d"),DeviceNumber);
		FileHandle = CreateFile(DeviceName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,NULL);
		if(FileHandle != INVALID_HANDLE_VALUE)
		{
			((PDISK_INPUT_DATA)&InputData)->CommandReg = 0xEC;
			Result = DeviceIoControl(FileHandle,
				0x0007C088,
				(PVOID)&InputData,
				sizeof(DISK_INPUT_DATA),
				(PVOID)&OutputData,
				sizeof(DISK_OUTPUT_DATA) + 0x200,
				&NumberOfReturnBytes,
				NULL);
			if(Result)
			{
				FirmwareRevisionPtr = ((PUCHAR)&OutputData + sizeof(DISK_OUTPUT_DATA) + 0x2E);
				for(int i=0; i < 8; i += 2) 
				{ 
					ProcessStr = FirmwareRevisionPtr[i]; 
					FirmwareRevisionPtr[i] = FirmwareRevisionPtr[i+1]; 
					FirmwareRevisionPtr[i+1] = ProcessStr; 
				} 
				__try
				{
					if(*FirmwareRevisionExSize > CStorageSerial::StorageFirmwareRevisionSize)
					{
						RtlMoveMemory((PVOID)FirmwareRevisionExBase,(PVOID)FirmwareRevisionPtr,CStorageSerial::StorageFirmwareRevisionSize);
						*FirmwareRevisionExSize = CStorageSerial::StorageFirmwareRevisionSize;
						Result = TRUE;
					}
					else
					{
						Result = FALSE;
					}
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					Result = FALSE;
				}

			}
			CloseHandle(FileHandle);
		}

	}

	return	(Result);
}
/*--------------------------------------------------------------------------------*/
BOOL	CStorageSerial::QueryModelNumberEx(ULONG DeviceNumber,PVOID ModelNumberBase,PULONG ModelNumberSize)
{
	BOOL	Result = FALSE;
	HANDLE	FileHandle = INVALID_HANDLE_VALUE;
	ULONG	NumberOfReturnBytes = 0;
	TCHAR	DeviceName[64] = {0};
	PUCHAR	ModelNumberPtr = NULL;
	UCHAR	ProcessStr = NULL;
	UCHAR	InputData[1024] = {0};
	UCHAR	OutputData[1024] = {0};

	if(IsUserAnAdmin())
	{
		_stprintf_s(DeviceName,sizeof(DeviceName)/sizeof(TCHAR),_T("\\\\.\\PhysicalDrive%d"),DeviceNumber);
		FileHandle = CreateFile(DeviceName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,NULL);
		if(FileHandle != INVALID_HANDLE_VALUE)
		{
			((PDISK_INPUT_DATA)&InputData)->CommandReg = 0xEC;
			Result = DeviceIoControl(FileHandle,
				0x0007C088,
				(PVOID)&InputData,
				sizeof(DISK_INPUT_DATA),
				(PVOID)&OutputData,
				sizeof(DISK_OUTPUT_DATA) + 0x200,
				&NumberOfReturnBytes,
				NULL);
			if(Result)
			{
				ModelNumberPtr = ((PUCHAR)&OutputData + sizeof(DISK_OUTPUT_DATA) + 0x36);
				for(int i=0; i < 0x28; i += 2) 
				{ 
					ProcessStr = ModelNumberPtr[i]; 
					ModelNumberPtr[i] = ModelNumberPtr[i+1]; 
					ModelNumberPtr[i+1] = ProcessStr; 
				} 
				__try
				{
					if(*ModelNumberSize > CStorageSerial::StorageModelNumberSize)
					{
						RtlMoveMemory((PVOID)ModelNumberBase,(PVOID)ModelNumberPtr,CStorageSerial::StorageModelNumberSize);
						*ModelNumberSize = CStorageSerial::StorageModelNumberSize;
						Result = TRUE;
					}
					else
					{
						Result = FALSE;
					}
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					Result = FALSE;
				}

			}
			CloseHandle(FileHandle);
		}

	}

	return	(Result);
}
/*--------------------------------------------------------------------------------*/
BOOL	CStorageSerial::QueryMediaRotationRateEx(ULONG DeviceNumber,USHORT* MediaRotationRate)
{
	BOOL	Result = FALSE;
	HANDLE	FileHandle = INVALID_HANDLE_VALUE;
	ULONG	NumberOfReturnBytes = 0;
	TCHAR	DeviceName[64] = {0};
	PUCHAR	DiskSerialPtr = NULL;
	UCHAR	ProcessStr = NULL;
	UCHAR	InputData[1024] = {0};
	UCHAR	OutputData[1024] = {0};

	if(IsUserAnAdmin())
	{
		_stprintf_s(DeviceName,sizeof(DeviceName)/sizeof(TCHAR),_T("\\\\.\\PhysicalDrive%d"),DeviceNumber);
		FileHandle = CreateFile(DeviceName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,NULL);
		if(FileHandle != INVALID_HANDLE_VALUE)
		{
			((PDISK_INPUT_DATA)&InputData)->CommandReg = 0xEC;
			Result = DeviceIoControl(FileHandle,
				0x0007C088,
				(PVOID)&InputData,
				sizeof(DISK_INPUT_DATA),
				(PVOID)&OutputData,
				sizeof(DISK_OUTPUT_DATA) + 0x200,
				&NumberOfReturnBytes,
				NULL);
			if(Result)
			{
				__try
				{
					if(MediaRotationRate)
					{
						*MediaRotationRate = *(USHORT*)((PUCHAR)&OutputData + sizeof(DISK_OUTPUT_DATA) + 0x1B2);
						Result = TRUE;
					}
					else
					{
						Result = FALSE;
					}
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					Result = FALSE;
				}

			}
			CloseHandle(FileHandle);
		}

	}

	return	(Result);
}
/*--------------------------------------------------------------------------------*/
BOOL CStorageSerial::ObtainSerial(IN PVOID SerialBase,IN PULONG SerialSize)
{
	BOOL	Result = FALSE;
	ULONG	DeviceNumber = 0;
	TCHAR	Volume[64] = {0};

	Result = QuerySystemVolume(Volume,sizeof(Volume)/sizeof(TCHAR));
	if(Result)
	{
		DeviceNumber = 0xFFFFEEEE;
		Result = QuerySystemStorage(Volume,&DeviceNumber);
		if(Result)
		{
			Result = QuerySerialEx(DeviceNumber,SerialBase,SerialSize);
		}
	}
	return	(Result);
}
/*--------------------------------------------------------------------------------*/
ULONG	CStorageSerial::ObtainSerial(LPWSTR SerialBase,ULONG SerialLen)
{
	ULONG	NumberOfReturn = 0;
	BOOL	Result = FALSE;
	UCHAR	DeviceSerial[64] = {0};
	ULONG	DeviceSerialSize = 0;

	if(SerialLen > CStorageSerial::StorageSerialLenW)
	{
		DeviceSerialSize = sizeof(DeviceSerial);
		Result = ObtainSerial((PVOID)&DeviceSerial,(PULONG)&DeviceSerialSize);
		if(Result)
		{
			__try
			{
				NumberOfReturn = MultiByteToWideChar(CP_ACP,0,(LPSTR)&DeviceSerial,DeviceSerialSize,SerialBase,SerialLen);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				NumberOfReturn = 0;
			}

		}
	}
	return	(NumberOfReturn);
}
/*--------------------------------------------------------------------------------*/
ULONG	CStorageSerial::ObtainSerial(LPSTR SerialBase,ULONG SerialLen)
{
	ULONG	NumberOfReturn = 0;
	BOOL	Result = FALSE;
	UCHAR	DeviceSerial[64] = {0};
	ULONG	DeviceSerialSize = 0;

	if(SerialLen > CStorageSerial::StorageSerialLenA)
	{
		DeviceSerialSize = sizeof(DeviceSerial);
		Result = ObtainSerial((PVOID)&DeviceSerial,(PULONG)&DeviceSerialSize);
		if(Result)
		{
			__try
			{
				RtlMoveMemory(SerialBase,&DeviceSerial,CStorageSerial::StorageSerialLenA);
				NumberOfReturn = CStorageSerial::StorageSerialLenA;
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				NumberOfReturn = 0;
			}
		}
	}
	return	(NumberOfReturn);
}
/*--------------------------------------------------------------------------------*/
BOOL CStorageSerial::ObtainFirmwareRevision(IN PVOID FirmwareRevisionBase,IN PULONG FirmwareRevisionSize)
{
	BOOL	Result = FALSE;
	ULONG	DeviceNumber = 0;
	TCHAR	Volume[64] = {0};

	Result = QuerySystemVolume(Volume,sizeof(Volume)/sizeof(TCHAR));
	if(Result)
	{
		DeviceNumber = 0xFFFFEEEE;
		Result = QuerySystemStorage(Volume,&DeviceNumber);
		if(Result)
		{
			Result = QueryFirmwareRevisionEx(DeviceNumber,FirmwareRevisionBase,FirmwareRevisionSize);
		}
	}
	return	(Result);
}
/*--------------------------------------------------------------------------------*/
ULONG	CStorageSerial::ObtainFirmwareRevision(LPWSTR FirmwareRevisionBase,ULONG FirmwareRevisionLen)
{
	ULONG	NumberOfReturn = 0;
	BOOL	Result = FALSE;
	UCHAR	DeviceFirmwareRevision[64] = {0};
	ULONG	DeviceFirmwareRevisionSize = 0;

	if(FirmwareRevisionLen > CStorageSerial::StorageFirmwareRevisionLenW)
	{
		DeviceFirmwareRevisionSize = sizeof(DeviceFirmwareRevision);
		Result = ObtainFirmwareRevision((PVOID)&DeviceFirmwareRevision,(PULONG)&DeviceFirmwareRevisionSize);
		if(Result)
		{
			__try
			{
				NumberOfReturn = MultiByteToWideChar(CP_ACP,0,(LPSTR)&DeviceFirmwareRevision,DeviceFirmwareRevisionSize,FirmwareRevisionBase,FirmwareRevisionLen);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				NumberOfReturn = 0;
			}

		}
	}
	return	(NumberOfReturn);
}
/*--------------------------------------------------------------------------------*/
ULONG	CStorageSerial::ObtainFirmwareRevision(LPSTR FirmwareRevisionBase,ULONG FirmwareRevisionLen)
{
	ULONG	NumberOfReturn = 0;
	BOOL	Result = FALSE;
	UCHAR	DeviceFirmwareRevision[64] = {0};
	ULONG	DeviceFirmwareRevisionSize = 0;

	if(FirmwareRevisionLen > CStorageSerial::StorageFirmwareRevisionLenA)
	{
		DeviceFirmwareRevisionSize = sizeof(DeviceFirmwareRevision);
		Result = ObtainFirmwareRevision((PVOID)&DeviceFirmwareRevision,(PULONG)&DeviceFirmwareRevisionSize);
		if(Result)
		{
			__try
			{
				RtlMoveMemory(FirmwareRevisionBase,&DeviceFirmwareRevision,CStorageSerial::StorageFirmwareRevisionLenA);
				NumberOfReturn = CStorageSerial::StorageFirmwareRevisionLenA;
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				NumberOfReturn = 0;
			}
		}
	}
	return	(NumberOfReturn);
}
/*--------------------------------------------------------------------------------*/
BOOL CStorageSerial::ObtainModelNumber(IN PVOID ModelNumberBase,IN PULONG ModelNumberSize)
{
	BOOL	Result = FALSE;
	ULONG	DeviceNumber = 0;
	TCHAR	Volume[64] = {0};

	Result = QuerySystemVolume(Volume,sizeof(Volume)/sizeof(TCHAR));
	if(Result)
	{
		DeviceNumber = 0xFFFFEEEE;
		Result = QuerySystemStorage(Volume,&DeviceNumber);
		if(Result)
		{
			Result = QueryModelNumberEx(DeviceNumber,ModelNumberBase,ModelNumberSize);
		}
	}
	return	(Result);
}
/*--------------------------------------------------------------------------------*/
ULONG	CStorageSerial::ObtainModelNumber(LPWSTR ModelNumberBase,ULONG ModelNumberLen)
{
	ULONG	NumberOfReturn = 0;
	BOOL	Result = FALSE;
	UCHAR	DeviceModelNumber[64] = {0};
	ULONG	DeviceModelNumberSize = 0;

	if(ModelNumberLen > CStorageSerial::StorageModelNumberLenW)
	{
		DeviceModelNumberSize = sizeof(DeviceModelNumber);
		Result = ObtainModelNumber((PVOID)&DeviceModelNumber,(PULONG)&DeviceModelNumberSize);
		if(Result)
		{
			__try
			{
				NumberOfReturn = MultiByteToWideChar(CP_ACP,0,(LPSTR)&DeviceModelNumber,DeviceModelNumberSize,ModelNumberBase,ModelNumberLen);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				NumberOfReturn = 0;
			}

		}
	}
	return	(NumberOfReturn);
}
/*--------------------------------------------------------------------------------*/
ULONG	CStorageSerial::ObtainModelNumber(LPSTR ModelNumberBase,ULONG ModelNumberLen)
{
	ULONG	NumberOfReturn = 0;
	BOOL	Result = FALSE;
	UCHAR	DeviceModelNumber[64] = {0};
	ULONG	DeviceModelNumberSize = 0;

	if(ModelNumberLen > CStorageSerial::StorageModelNumberLenA)
	{
		DeviceModelNumberSize = sizeof(DeviceModelNumber);
		Result = ObtainModelNumber((PVOID)&DeviceModelNumber,(PULONG)&DeviceModelNumberSize);
		if(Result)
		{
			__try
			{
				RtlMoveMemory(ModelNumberBase,&DeviceModelNumber,CStorageSerial::StorageModelNumberLenA);
				NumberOfReturn = CStorageSerial::StorageModelNumberLenA;
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				NumberOfReturn = 0;
			}
		}
	}
	return	(NumberOfReturn);
}
/*--------------------------------------------------------------------------------*/
BOOL	CStorageSerial::ObtainMediaRotationRate(USHORT* MediaRotationRate)
{
	BOOL	Result = FALSE;
	ULONG	DeviceNumber = 0;
	TCHAR	Volume[64] = {0};

	Result = QuerySystemVolume(Volume,sizeof(Volume)/sizeof(TCHAR));
	if(Result)
	{
		DeviceNumber = 0xFFFFEEEE;
		Result = QuerySystemStorage(Volume,&DeviceNumber);
		if(Result)
		{
			Result = QueryMediaRotationRateEx(DeviceNumber,MediaRotationRate);
		}
	}
	return	(Result);
}
/*--------------------------------------------------------------------------------*/
ULONG	CStorageSerial::QuerySerialSize()
{
	return	(CStorageSerial::StorageSerialSize);
}
/*--------------------------------------------------------------------------------*/
ULONG	CStorageSerial::QueryFirmwareRevisionSize()
{
	return	(CStorageSerial::StorageFirmwareRevisionSize);
}
/*--------------------------------------------------------------------------------*/
ULONG	CStorageSerial::QueryModelNumberSize()
{
	return	(CStorageSerial::StorageModelNumberSize);
}
/*--------------------------------------------------------------------------------*/