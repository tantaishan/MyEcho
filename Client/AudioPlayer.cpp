#include "stdafx.h"
#include "AudioPlayer.h"

AudioPlayer::AudioPlayer()
	: mciPlayer_(NULL),
	wavePlayer_(NULL),
	dwLength_(0),
	dwVolume_(1000)
{
	ZeroMemory(lpszFile_, MAX_PATH);
}

void AudioPlayer::Open(LPCTSTR lpszFile)
{
	StrCpy(lpszFile_, lpszFile);

	LPTSTR lpszExt = ::PathFindExtension(lpszFile_);
	if (lstrcmpi(lpszExt, _T(".amr")) == 0)
	{
		player_ = new WavePlayer;
	}
	else
	{
		player_ = new MCIPlayer;
	}
}

AudioPlayer::~AudioPlayer()
{
	if (player_)
		delete player_;
}

void AudioPlayer::Play()
{
	player_->Play(lpszFile_, &dwLength_);
}

void AudioPlayer::Stop()
{
	player_->Stop();
}

DWORD AudioPlayer::Length()
{
	return dwLength_;
}

DWORD AudioPlayer::Length(LPCTSTR lpszFile)
{
	return player_->Length(lpszFile);
}