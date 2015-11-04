#pragma once

// Multimedia API's
#include <mmreg.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include <msacm.h>

#define RECORD_STATE_INIT		0
#define	RECORD_STATE_RECING		1
#define RECORD_STATE_PLAYING	2
#define RECORD_STATE_STOPING	3
#define RECORD_STATE_STOP		4

#define INP_BUFFER_SIZE			16384

class WaveRecorder
{
public:
	WaveRecorder();
	~WaveRecorder();
	BOOL SaveRecord(const std::wstring& strPath);
	BOOL StartRecord();
	BOOL StopRecord();
	static LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Attach(HWND hWnd);
	void OnWIM_Close(UINT wParam, LONG lParam);
	void OnWIM_Data(UINT wParam, LONG lParam);
	void OnWIM_Open(WPARAM wParam, LPARAM lParam);
	void OnWOM_Close(UINT wParam, LONG lParam);
	void OnWOM_Done(UINT wParam, LONG lParam);
	void OnWOM_Open(UINT wParam, LONG lParam);

	int GetRecordState() { return nRecordState_; }
private:
	HWND			hWnd_;
	WNDPROC			WndProc_;
	PBYTE			pSaveBuffer_;
	PBYTE			pBuffer1_;
	PBYTE			pBuffer2_;
	int				nRecordState_;
	HWAVEIN			hWaveIn_;
	HWAVEOUT		hWaveOut_;
	DWORD			dwDataLength_;
	WAVEFORMATEX	waveFormat_;
	PWAVEHDR		pWaveHdr1_;
	PWAVEHDR		pWaveHdr2_;
};

