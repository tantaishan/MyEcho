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

	// ���� 
	void Play(LPCTSTR lpAudioFile, DWORD* pAudioLen = NULL);
	// ��ͣ
	void Pause();
	// ����
	void Resume();
	// ֹͣ
	void Stop();
	// ����ʱ��
	DWORD Length(LPCTSTR lpAudioFile);
	// ����
	void SetVolume(DWORD dwVolume); // volume: 0-1000

private:
	MCIDEVICEID m_idDevPlay;
};
