#pragma once
#include "WaveDef.h"
#include <vector>

class AMRDecoder;
class WavePlayer : public IAudioPlayer
{
	/// messages
	typedef unsigned int	WaveMsg;     // wave messages
	typedef unsigned int	ThreadMsg;   // thread messages

	enum PlayState
	{
		Play_Stop = 0,
		Play_Pause,
		Play_Playing,
	};

public:
	WavePlayer(void);
	~WavePlayer(void);

public:
	// 播放
	void Play(LPCTSTR lpszFile, DWORD* pLength);
	// 暂停
	void Pause();
	// 继续
	void Resume();
	// 停止
	void Stop();
	// 播放时间
	DWORD Length(LPCTSTR lpszFile);
	// 音量
	void SetVolume(DWORD dwVolume); // volume: 0-1000

protected:
	static BOOL CALLBACK WaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	BOOL   WaveOutProcImpl(HWAVEOUT hwo, UINT uMsg, DWORD dwParam1, DWORD dwParam2);
	static unsigned int CALLBACK PlayThread(LPVOID lp); // main play thread
	unsigned int PlayThreadProcImpl();
	static unsigned int CALLBACK DecodeThread(LPVOID lp);
	unsigned int DecodeThreadProcImpl();

	/// prototypes
	WAVEHDR*	allocBlocks(int size, int count); /// alloc heap for wave header blocks
	void		freeBlocks(WAVEHDR* blockArray); /// free heap
	void		UnInit();
	void		OnPlayFinished();

private: /// members
	/// thread
	HANDLE				m_hThreadPlay;
	HANDLE				m_hThreadDecode;
	PlayState			m_ePlayStat; /// 播放状态
	/// Event
	HANDLE				m_hEventDecode; /// event for decoding
	/// device
	HWAVEOUT			m_hWaveoutDev;
	WaveBlock			m_wBlock;
	WaveData			m_waveData;
	ThreadMsg			m_msgPlayThread; // thread msg { 1,0 } alive / close
	ThreadMsg			m_msgDecodeThread;
	AMRDecoder*		m_pDecoder;
	CRITICAL_SECTION	m_cs; // critical section
};
