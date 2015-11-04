#include "StdAfx.h"
#include "MCIPlayer.h"


MCIPlayer::MCIPlayer()
	: m_idDevPlay(0)
{
}

MCIPlayer::~MCIPlayer(void)
{
}

void MCIPlayer::Play(LPCTSTR lpAudioFile, DWORD* pAudioLen)
{
	ATLASSERT(0 == m_idDevPlay);
	// ���豸���ļ�
	MCI_OPEN_PARMS mciOpen;
	mciOpen.lpstrElementName = lpAudioFile;
	MCIERROR err = mciSendCommand(0, MCI_OPEN, MCI_WAIT | MCI_OPEN_ELEMENT, (DWORD_PTR)&mciOpen);

	if (err != 0)
	{
		return;
	}

	m_idDevPlay = mciOpen.wDeviceID;

	if (pAudioLen)
	{
		MCI_STATUS_PARMS mciStatus;
		mciStatus.dwItem = MCI_STATUS_LENGTH;
		mciSendCommand(mciOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&mciStatus);
		*pAudioLen = mciStatus.dwReturn;
	}
	// ����
	MCI_PLAY_PARMS mciPlay;
	mciSendCommand(mciOpen.wDeviceID, MCI_PLAY, 0, (DWORD_PTR)&mciPlay);
}

void MCIPlayer::Pause()
{
	ATLASSERT(m_idDevPlay);

	MCI_GENERIC_PARMS mciPause;
	mciSendCommand(m_idDevPlay, MCI_PAUSE, 0, (DWORD_PTR)&mciPause);
}

void MCIPlayer::Resume()
{
	ATLASSERT(m_idDevPlay);

	MCI_GENERIC_PARMS mciPause;
	mciSendCommand(m_idDevPlay, MCI_RESUME, 0, (DWORD_PTR)&mciPause);
}

void MCIPlayer::Stop()
{
	ATLASSERT(m_idDevPlay);

	MCI_GENERIC_PARMS mciStop;
	mciSendCommand(m_idDevPlay, MCI_STOP, 0, (DWORD_PTR)&mciStop);
	mciSendCommand(m_idDevPlay, MCI_CLOSE, 0, NULL);
	m_idDevPlay = 0;
}

DWORD MCIPlayer::Length(LPCTSTR lpAudioFile)
{
	// ���豸���ļ�
	MCI_OPEN_PARMS mciOpen;
	mciOpen.lpstrElementName = lpAudioFile;
	MCIERROR err = mciSendCommand(0, MCI_OPEN, MCI_WAIT | MCI_OPEN_ELEMENT, (DWORD_PTR)&mciOpen);
	// �ļ�����ʧ��
	if (err != 0)
	{
		return 0;
	}
	// ��ȡ�ļ����ŵ��ܳ�����Ϣ
	MCI_STATUS_PARMS mciStatus;
	mciStatus.dwItem = MCI_STATUS_LENGTH;
	mciSendCommand(mciOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&mciStatus);
	mciSendCommand(mciOpen.wDeviceID, MCI_CLOSE, 0, NULL);

	return mciStatus.dwReturn;
}

void MCIPlayer::SetVolume(DWORD dwVolume)
{
	ATLASSERT(m_idDevPlay);
	ATLASSERT(dwVolume <= 1000);

	MCI_DGV_SETAUDIO_PARMS mciSetAudio;
	mciSetAudio.dwItem = MCI_DGV_SETAUDIO_VOLUME;
	mciSetAudio.dwValue = dwVolume; // Volume(0-1000)  
	mciSendCommand(m_idDevPlay, MCI_SETAUDIO, MCI_DGV_SETAUDIO_VALUE | MCI_DGV_SETAUDIO_ITEM, (DWORD_PTR)&mciSetAudio);
}
