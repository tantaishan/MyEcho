#pragma once

#include "interf_enc.h"
#include <memory.h >
#include <string.h >
#include <xstring >
#include <wtypes.h>
#pragma  comment(lib,"AMRNB.lib")
#define AMR_MAGIC_NUMBER "#!AMR\n"

#define PCM_FRAME_SIZE 160 // 8khz 8000*0.02=160
#define MAX_AMR_FRAME_SIZE 32
#define AMR_FRAME_COUNT_PER_SECOND 50
//int amrEncodeMode[] = {4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200}; // amr ±àÂë·½Ê½

class CPcmToAmr
{
public:
	CPcmToAmr(std::wstring strFileName, unsigned int bitrate = 16,unsigned int inputRate =8000);
	~CPcmToAmr(void);

	virtual void ReceiveBuffer(LPSTR lpData, DWORD dwBytesRecorded);
private:
	void ConVetDate(LPSTR lpData, DWORD dwDateSize);
	void CloseFile();


public:
	std::wstring m_strFileName;
private:
	FILE * m_fpFile;
	bool m_bFileEmpty;
	int * m_piEnstate;
	
};
