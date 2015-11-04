#pragma once

#ifndef __CSTORAGE_SERIAL__
#define __CSTORAGE_SERIAL__

#include <ShlObj.h>


#pragma warning (disable:4200)
//#pragma pack(push,1)
//typedef	struct	_DISK_INPUT_DATA
//{
//	ULONG	BufferSize;
//	UCHAR	FeaturesReg;
//	UCHAR	SectorCountReg;
//	UCHAR	SectorNumberReg;
//	UCHAR	CylLowReg;
//	UCHAR	CylHighReg;
//	UCHAR	DriveHeadReg;
//	UCHAR	CommandReg;
//	UCHAR	Reserved0;
//	UCHAR	DriveNumber; 
//	UCHAR	Reserved1[3];
//	ULONG	Reserved2[4];
//	UCHAR	InBuffer[0]; 
//}DISK_INPUT_DATA,*PDISK_INPUT_DATA;
//typedef	struct	_DISK_OUTPUT_DATA
//{
//	ULONG	BufferSize;
//	UCHAR	DriverError;
//	UCHAR	IDEError;
//	UCHAR	Reserved0[2];
//	ULONG	Reserved1[2];
//	UCHAR	Buffer[0];  
//}DISK_OUTPUT_DATA,*PDISK_OUTPUT_DATA;
//#pragma pack(pop)

class IStorageSerial
{
public:
	virtual	BOOL	QuerySystemVolume(LPTSTR Volume,ULONG VolumeLen) = 0;
	virtual	BOOL	QuerySystemStorage(LPTSTR Volume,PULONG Number) = 0;
	virtual	BOOL	QuerySerialEx(ULONG DeviceNumber,PVOID SerialBase,PULONG SerialSize) = 0;
	virtual	BOOL	QueryFirmwareRevisionEx(ULONG DeviceNumber,PVOID FirmwareRevisionExBase,PULONG FirmwareRevisionExSize) = 0;
	virtual	BOOL	QueryModelNumberEx(ULONG DeviceNumber,PVOID ModelNumberBase,PULONG ModelNumberSize) = 0;
	virtual BOOL	QueryMediaRotationRateEx(ULONG DeviceNumber,USHORT* MediaRotationRate) = 0;
	virtual	ULONG	QuerySerialSize() = 0;
	virtual ULONG	QueryFirmwareRevisionSize() = 0;
	virtual ULONG	QueryModelNumberSize() = 0;
};

class CStorageSerial : public IStorageSerial
{
public:
	CStorageSerial();
	virtual ~CStorageSerial();
	//虚函数重写。
	BOOL	QuerySystemVolume(LPTSTR Volume,ULONG VolumeLen);
	BOOL	QuerySystemStorage(LPTSTR Volume,PULONG Number);
	BOOL	QuerySerialEx(ULONG DeviceNumber,PVOID SerialBase,PULONG SerialSize);
	BOOL	QueryFirmwareRevisionEx(ULONG DeviceNumber,PVOID FirmwareRevisionExBase,PULONG FirmwareRevisionExSize);
	BOOL	QueryModelNumberEx(ULONG DeviceNumber,PVOID ModelNumberBase,PULONG ModelNumberSize);
	BOOL	QueryMediaRotationRateEx(ULONG DeviceNumber,USHORT* MediaRotationRate);
	ULONG	QuerySerialSize();
	ULONG	QueryFirmwareRevisionSize();
	ULONG	QueryModelNumberSize();
	
	//扩展方法。
	BOOL	ObtainSerial(PVOID SerialBase,PULONG SerialSize);
	ULONG	ObtainSerial(LPWSTR SerialBase,ULONG SerialLen);
	ULONG	ObtainSerial(LPSTR SerialBase,ULONG SerialLen);
	BOOL	ObtainFirmwareRevision(IN PVOID FirmwareRevisionBase,IN PULONG FirmwareRevisionSize);
	ULONG	ObtainFirmwareRevision(LPWSTR FirmwareRevisionBase,ULONG FirmwareRevisionLen);
	ULONG	ObtainFirmwareRevision(LPSTR FirmwareRevisionBase,ULONG FirmwareRevisionLen);
	BOOL	ObtainModelNumber(IN PVOID ModelNumberBase,IN PULONG ModelNumberSize);
	ULONG	ObtainModelNumber(LPWSTR ModelNumberBase,ULONG ModelNumberLen);
	ULONG	ObtainModelNumber(LPSTR ModelNumberBase,ULONG ModelNumberLen);
	BOOL	ObtainMediaRotationRate(USHORT* MediaRotationRate);
private:
	const	static ULONG StorageSerialSize = 0x14;
	const	static ULONG StorageSerialLenA = 0x14;
	const	static ULONG StorageSerialLenW = 0x14;
	const	static ULONG StorageFirmwareRevisionSize = 0x8;
	const	static ULONG StorageFirmwareRevisionLenA = 0x8;
	const	static ULONG StorageFirmwareRevisionLenW = 0x8;
	const	static ULONG StorageModelNumberSize = 0x28;
	const	static ULONG StorageModelNumberLenA = 0x28;
	const	static ULONG StorageModelNumberLenW = 0x28;
};


#endif