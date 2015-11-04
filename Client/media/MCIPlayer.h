#pragma once

#include<mmsystem.h> 
#pragma comment(lib,"winmm.lib") 

#include <Digitalv.h>
#include "WaveDef.h"

class MCIPlayer : public IAudioPlayer
{
public:
	MCIPlayer();
	~MCIPlayer(void);

	// 播放 
	void Play(LPCTSTR lpAudioFile, DWORD* pAudioLen = NULL);
	// 暂停
	void Pause();
	// 继续
	void Resume();
	// 停止
	void Stop();
	// 播放时间
	DWORD Length(LPCTSTR lpAudioFile);
	// 音量
	void SetVolume(DWORD dwVolume); // volume: 0-1000

private:
	MCIDEVICEID m_idDevPlay;
};
