#include "stdafx.h"
#include "WaveRecorder.h"
#include <WindowsX.h>

WaveRecorder::WaveRecorder()
	: hWnd_(NULL),
	WndProc_(NULL),
	pSaveBuffer_(NULL),
	pBuffer1_(NULL),
	pBuffer2_(NULL),
	nRecordState_(0),
	hWaveIn_(NULL),
	hWaveOut_(NULL),
	dwDataLength_(0),
	pWaveHdr1_(NULL),
	pWaveHdr2_(NULL)
{

	//录音机状态初始化
	nRecordState_ = RECORD_STATE_INIT;
	dwDataLength_ = 0;

	pSaveBuffer_ = (PBYTE)malloc(1);
	pWaveHdr1_ = (PWAVEHDR)malloc(sizeof(WAVEHDR));
	pWaveHdr2_ = (PWAVEHDR)malloc(sizeof(WAVEHDR));


	// 为录音准备WAVEFORMAT
	waveFormat_.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat_.nChannels = 1;//声道
	waveFormat_.nBlockAlign = 2;
	waveFormat_.nSamplesPerSec = 8000;//采样频率
	waveFormat_.nAvgBytesPerSec = 16000;
	waveFormat_.wBitsPerSample = 16;//8位
	waveFormat_.cbSize = 0;
}

WaveRecorder::~WaveRecorder()
{
	//SubclassWindow(hWnd_, WndProc_);
	if (pSaveBuffer_)
		free(pSaveBuffer_);
}

void WaveRecorder::Attach(HWND hWnd)
{
	if (hWnd && ::IsWindow(hWnd))
	{
		hWnd_ =  hWnd;
		::SetProp(hWnd_, _T("X"), (HANDLE)this);
		WndProc_ = SubclassWindow(hWnd_, WndProc);
	}
}

BOOL WaveRecorder::StartRecord()
{
	pBuffer1_ = (PBYTE)malloc(INP_BUFFER_SIZE);
	pBuffer2_ = (PBYTE)malloc(INP_BUFFER_SIZE);

	MMRESULT ret = waveInOpen(&hWaveIn_, WAVE_MAPPER, &waveFormat_, (DWORD)hWnd_, NULL, CALLBACK_WINDOW);

		//return FALSE;

	pWaveHdr1_->lpData = (LPSTR)pBuffer1_;
	pWaveHdr1_->dwBufferLength = INP_BUFFER_SIZE;
	pWaveHdr1_->dwLoops = 1;
	pWaveHdr1_->dwUser = 0;
	pWaveHdr1_->dwFlags = 0;
	pWaveHdr1_->dwBytesRecorded = 0;
	pWaveHdr1_->lpNext = NULL;
	if (waveInPrepareHeader(hWaveIn_, pWaveHdr1_, sizeof(WAVEHDR)))
		return FALSE;
	pWaveHdr2_->lpData = (LPSTR)pBuffer2_;
	pWaveHdr2_->dwBufferLength = INP_BUFFER_SIZE;
	pWaveHdr2_->dwLoops = 1;
	pWaveHdr2_->dwUser = 0;
	pWaveHdr2_->dwFlags = 0;
	pWaveHdr2_->dwBytesRecorded = 0;
	pWaveHdr2_->lpNext = NULL;
	if (waveInPrepareHeader(hWaveIn_, pWaveHdr2_, sizeof(WAVEHDR)))
		return FALSE;

	return TRUE;
}

void WaveRecorder::OnWIM_Open(WPARAM wParam, LPARAM lParam)
{
	pSaveBuffer_ = (PBYTE)realloc(pSaveBuffer_, 1);

	nRecordState_ = RECORD_STATE_RECING;

	// Add the buffers
	waveInAddBuffer(hWaveIn_, pWaveHdr1_, sizeof(WAVEHDR));
	waveInAddBuffer(hWaveIn_, pWaveHdr2_, sizeof(WAVEHDR));

	dwDataLength_ = 0;
	waveInStart(hWaveIn_);
}

void WaveRecorder::OnWIM_Data(UINT wParam, LONG lParam)
{
	// Reallocate save buffer memory
	PWAVEHDR pWaveHdr = (PWAVEHDR)lParam;

	pSaveBuffer_ = (PBYTE)realloc(pSaveBuffer_, dwDataLength_ + pWaveHdr->dwBytesRecorded);
	CopyMemory(pSaveBuffer_ + dwDataLength_, pWaveHdr->lpData, pWaveHdr->dwBytesRecorded);

	dwDataLength_ += pWaveHdr->dwBytesRecorded;

	if (nRecordState_ == RECORD_STATE_STOPING) {
		waveInClose(hWaveIn_);
	}

	// Send out a new buffer
	waveInAddBuffer(hWaveIn_, (PWAVEHDR)lParam, sizeof(WAVEHDR));
}

void WaveRecorder::OnWIM_Close(UINT wParam, LONG lParam)
{
	nRecordState_ = RECORD_STATE_STOP;
}

void WaveRecorder::OnWOM_Open(UINT wParam, LONG lParam)
{
	nRecordState_ = RECORD_STATE_PLAYING;

	pWaveHdr1_->lpData = (LPSTR)pSaveBuffer_;
	pWaveHdr1_->dwBufferLength = dwDataLength_;
	pWaveHdr1_->dwBytesRecorded = 0;
	pWaveHdr1_->dwUser = 0;
	pWaveHdr1_->dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
	pWaveHdr1_->dwLoops = 1;
	pWaveHdr1_->lpNext = NULL;
	pWaveHdr1_->reserved = 0;

	waveOutPrepareHeader(hWaveOut_, pWaveHdr1_, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut_, pWaveHdr1_, sizeof (WAVEHDR));
}

void WaveRecorder::OnWOM_Done(UINT wParam, LONG lParam)
{
	waveOutUnprepareHeader(hWaveOut_, pWaveHdr1_, sizeof(WAVEHDR));
	waveOutClose(hWaveOut_);
}

void WaveRecorder::OnWOM_Close(UINT wParam, LONG lParam)
{
	nRecordState_ = RECORD_STATE_STOP;
}

BOOL WaveRecorder::SaveRecord(const std::wstring& strPath)
{
	if (dwDataLength_ <= 0)
		return FALSE;

	DWORD waveHeaderSize = 38;
	DWORD waveFormatSize = 18;

	HANDLE hFile = ::CreateFile(strPath.c_str(), GENERIC_WRITE | GENERIC_READ,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwWritten = 0;
	WriteFile(hFile, "RIFF", 4, &dwWritten, NULL);
	unsigned int Sec = (sizeof pSaveBuffer_ + waveHeaderSize);
	WriteFile(hFile, &Sec, sizeof(Sec), &dwWritten, NULL);
	WriteFile(hFile, "WAVE", 4, &dwWritten, NULL);
	WriteFile(hFile, "fmt ", 4, &dwWritten, NULL);
	WriteFile(hFile, &waveFormatSize, sizeof(waveFormatSize), &dwWritten, NULL);

	WriteFile(hFile, &waveFormat_.wFormatTag, sizeof(waveFormat_.wFormatTag), &dwWritten, NULL);
	WriteFile(hFile, &waveFormat_.nChannels, sizeof(waveFormat_.nChannels), &dwWritten, NULL);
	WriteFile(hFile, &waveFormat_.nSamplesPerSec, sizeof(waveFormat_.nSamplesPerSec), &dwWritten, NULL);
	WriteFile(hFile, &waveFormat_.nAvgBytesPerSec, sizeof(waveFormat_.nAvgBytesPerSec), &dwWritten, NULL);
	WriteFile(hFile, &waveFormat_.nBlockAlign, sizeof(waveFormat_.nBlockAlign), &dwWritten, NULL);
	WriteFile(hFile, &waveFormat_.wBitsPerSample, sizeof(waveFormat_.wBitsPerSample), &dwWritten, NULL);
	WriteFile(hFile, &waveFormat_.cbSize, sizeof(waveFormat_.cbSize), &dwWritten, NULL);
	WriteFile(hFile, "data", 4, &dwWritten, NULL);
	WriteFile(hFile, &dwDataLength_, sizeof(dwDataLength_), &dwWritten, NULL);
	WriteFile(hFile, pSaveBuffer_, dwDataLength_, &dwWritten, NULL);

	::FlushFileBuffers(hFile);
	::CloseHandle(hFile);
	return TRUE;
}

LRESULT WaveRecorder::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WaveRecorder* pThis = (WaveRecorder*)::GetProp(hWnd, _T("X"));
	switch (uMsg)
	{
	case MM_WIM_OPEN:
		pThis->OnWIM_Open(wParam, lParam);
		break;
	case MM_WIM_DATA:
		pThis->OnWIM_Data(wParam, lParam);
		break;
	case MM_WIM_CLOSE:
		pThis->OnWIM_Close(wParam, lParam);
		break;
	case MM_WOM_OPEN:
		pThis->OnWOM_Open(wParam, lParam);
		break;
	case MM_WOM_DONE:
		pThis->OnWOM_Done(wParam, lParam);
		break;
	case  MM_WOM_CLOSE:
		pThis->OnWOM_Close(wParam, lParam);
		break;
	default:
		break;
	}
	return ::CallWindowProc(pThis->WndProc_, hWnd, uMsg, wParam, lParam);
}

BOOL WaveRecorder::StopRecord()
{
	MMRESULT uRes1 = 0, uRes2 = 0;
	if (nRecordState_ == RECORD_STATE_RECING)
	{
		nRecordState_ = RECORD_STATE_STOPING;
		uRes1 = waveInReset(hWaveIn_);
	}

	if (nRecordState_ == RECORD_STATE_PLAYING)
	{
		nRecordState_ = RECORD_STATE_STOPING;
		uRes2 = waveOutReset(hWaveOut_);
	}

	return (uRes1 == 0) && (uRes2 == 0);
}