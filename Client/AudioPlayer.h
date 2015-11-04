#pragma once
#include "media/WavePlayer.h"
#include "media/MCIPlayer.h"

class AudioPlayer
{
public:
	AudioPlayer();
	~AudioPlayer();

	void Open(LPCTSTR lpszFile);
	void Play();
	void Stop();
	DWORD Length();
	DWORD Length(LPCTSTR lpszFile);

private:
	TCHAR lpszFile_[MAX_PATH];
	MCIPlayer* mciPlayer_;
	WavePlayer* wavePlayer_;
	IAudioPlayer* player_;
	DWORD dwLength_;
	DWORD dwVolume_;
};

