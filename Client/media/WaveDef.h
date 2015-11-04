#pragma once
#include <MMReg.h>
#include <MMSystem.h>

struct WaveData
{
	WAVEFORMATEX	wfmtx;
	LPSTR			pData;
	DWORD			dwSize;         // size in bytes
	unsigned int	bDecodeFinished; // decode finished or not
};

struct WaveBlock
{
	/// wave block
	unsigned long	wpos;         /// current play position
	WAVEHDR*		pWaveHdr;     /// wave block
	volatile int	wfreeblock;   /// free blocks left
	int				wcurrblock;   /// current block
};

class IAudioPlayer
{
public:
	virtual void Play(LPCTSTR lpAudioFile, DWORD* pAudioLen = NULL) = 0;
	virtual void Pause() = 0;
	virtual void Resume() = 0;
	virtual void Stop() = 0;
	virtual DWORD Length(LPCTSTR lpAudioFile) = 0;
	virtual void SetVolume(DWORD dwVolume) = 0;
};

