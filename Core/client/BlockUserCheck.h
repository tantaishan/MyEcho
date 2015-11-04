#pragma once

#include <bitset>
#include <vector>
#include "../thread/Locker.h"

typedef std::bitset<32> BlockRes;


class BlockUserCheck
{
public:
	static BlockUserCheck* Instance();
	BOOL Check();
	BOOL CheckAll(BlockRes& wndRes, BlockRes& procRes, BlockRes& destopRes, BlockRes& progRes, BlockRes& menuRes, BlockRes& vmRes);

private:
	friend BOOL CALLBACK EnumWindowProc(HWND hwnd,LPARAM lParam);
	VOID EnumFolderFile(LPCTSTR lpFolderPath, ULONG NumberOfRecursion, std::vector<std::wstring>& VtDeskTopList);
	BOOL GetCurrentUserFolder(std::wstring& msUserSid, std::wstring& msFolder, LPCTSTR lpFolderName);
	BOOL LookupExplorerUserSid(std::wstring &msUserSid);

	BOOL TestWndBlock(BlockRes* pRes = NULL);
	BOOL TestProcessBlock(BlockRes* pRes = NULL);
	BOOL TestDeskTopBlock(BlockRes* pRes = NULL);
	BOOL TestProgramBlock(BlockRes* pRes = NULL);
	BOOL TestStartMenuBlock(BlockRes* pRes = NULL);
	BOOL TestVMBlock(BlockRes* pRes = NULL);

private:
	BlockUserCheck();
	~BlockUserCheck();
	class Garbo
	{
	public:
		~Garbo()
		{
			if (BlockUserCheck::instance_)
				delete BlockUserCheck::instance_;
		}
	};

	static Garbo Garbo;
	static BlockUserCheck* instance_;
	static Locker singletonLocker;

private:
	std::vector<std::wstring>	m_VtBlockProcess;
	std::vector<std::wstring>	m_VtBlockProgram;
	std::vector<std::wstring>	m_VtBlockStartMenu;
	std::vector<std::wstring>	m_VtBlockDeskTop;
	std::vector<std::wstring>	m_VtBlockHardDisc;
};

