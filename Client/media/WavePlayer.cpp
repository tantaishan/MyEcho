#include "StdAfx.h"
#include "WavePlayer.h"
#include "AMRDecoder.h"

#define TMSG_ALIVE              1       // thread alive
#define TMSG_CLOSE              0       // thread close
#define THREAD_EXIT     0xABECEDA       // thread exit code

/// buffering
#define BLOCK_SIZE           8192       // queue block size
#define BLOCK_COUNT            20       // queue block count
#define BP_TURN                 1       // blocks per turn


WavePlayer::WavePlayer(void)
	: m_hWaveoutDev(NULL)
	, m_hThreadPlay(NULL)
	, m_hThreadDecode(NULL)
	, m_hEventDecode(NULL)
	, m_ePlayStat(Play_Stop)
	, m_pDecoder(NULL)
	, m_msgDecodeThread(TMSG_CLOSE)
	, m_msgPlayThread(TMSG_CLOSE)
{
	memset(&m_waveData, 0, sizeof(WaveData));

	// Ϊ���Ż��������ڴ��
	m_wBlock.pWaveHdr = allocBlocks(BLOCK_SIZE, BLOCK_COUNT);
	m_wBlock.wfreeblock = BLOCK_COUNT;
	m_wBlock.wcurrblock = 0;
	m_wBlock.wpos = 0;

	m_hEventDecode = CreateEvent(NULL, FALSE, FALSE, NULL);
	ATLASSERT(m_hEventDecode);

	m_pDecoder = new AMRDecoder();
	ATLASSERT(m_pDecoder);

	// ��ʼ���ٽ���
	InitializeCriticalSection(&m_cs);
}

WavePlayer::~WavePlayer(void)
{
	UnInit();

	// �ͷŲ��Ż���
	freeBlocks(m_wBlock.pWaveHdr);
	memset(&m_wBlock, 0, sizeof(WaveBlock));

	if (m_hEventDecode)
	{
		CloseHandle(m_hEventDecode);
		m_hEventDecode = NULL;
	}

	// �����ٽ���
	DeleteCriticalSection(&m_cs);
}

//************************************
// Method:    ���벥�Ż���
// FullName:  WavePlayer::allocBlocks
// Access:    protected 
// Returns:   WAVEHDR*
// Parameter: int size��ÿ�鲨�����ݻ���Ĵ�С
// Parameter: int count���������
//************************************
WAVEHDR* WavePlayer::allocBlocks(int size, int count)
{
	unsigned char* buffer;
	int i;
	WAVEHDR* blocks;
	DWORD totalBufferSize = (size + sizeof(WAVEHDR)) * count;

	//  һ���Է������л��棬���������룬�ͷż�
	if ((buffer = (UCHAR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBufferSize)) == NULL)
		return NULL;

	// ���û�������ָ��
	blocks = (WAVEHDR*)buffer;
	buffer += sizeof(WAVEHDR) * count;
	for (i = 0; i < count; i++)
	{
		blocks[i].dwBufferLength = size;
		blocks[i].lpData = (CHAR *)buffer;
		buffer += size;
	}

	return blocks;
}

//************************************
// Method:    �ͷŲ��Ż���
// FullName:  WavePlayer::freeBlocks
// Access:    protected 
// Returns:   void
// Parameter: WAVEHDR * blockArray
//************************************
void WavePlayer::freeBlocks(WAVEHDR* blockArray)
{
	HeapFree(GetProcessHeap(), 0, blockArray);
}

void WavePlayer::UnInit()
{
	if (m_hThreadPlay)
	{
		// ��ֹ�����߳�
		EnterCriticalSection(&m_cs);
		m_msgPlayThread = TMSG_CLOSE;
		LeaveCriticalSection(&m_cs);
		WaitForSingleObject(m_hThreadPlay, INFINITE);

		CloseHandle(m_hThreadPlay);
		m_hThreadPlay = NULL;
	}

	if (m_hThreadDecode)
	{
		// ��ֹ�����߳�
		EnterCriticalSection(&m_cs);
		m_msgDecodeThread = TMSG_CLOSE;
		LeaveCriticalSection(&m_cs);
		WaitForSingleObject(m_hThreadDecode, INFINITE);

		CloseHandle(m_hThreadDecode);
		m_hThreadDecode = NULL;
	}

	// �ͷŲ������ݲ��ָ���ʼ�����ӣ��Թ��´�ʹ��
	if (m_waveData.pData)
	{
		HeapFree(GetProcessHeap(), 0, m_waveData.pData);
		m_waveData.pData = NULL;
	}
	memset(&m_waveData, 0, sizeof(WaveData));

	// �رղ����豸
	if (m_hWaveoutDev)
	{
		waveOutReset(m_hWaveoutDev);
		waveOutClose(m_hWaveoutDev);
		m_hWaveoutDev = NULL;
	}

	// ���貥�Ż�����α�
	m_wBlock.wfreeblock = BLOCK_COUNT;
	m_wBlock.wcurrblock = 0;
	m_wBlock.wpos = 0;

	m_ePlayStat = Play_Stop;
}

void WavePlayer::Play(LPCTSTR lpszFile, DWORD* pLength)
{
	// ����Ѿ��в��ŵ��ˣ���ֹͣ
	if (m_ePlayStat != Play_Stop)
	{
		Stop();
	}

	// ���ý�����
	if (m_pDecoder == NULL)
	{
		m_pDecoder = new AMRDecoder(lpszFile);
	}
	else
	{
		m_pDecoder->SetFilePathName(lpszFile);
	}

	// ȡ����ʱ��
	if (pLength)
	{
		*pLength = (DWORD)m_pDecoder->GetTimeLength();
	}

	// ������������ݶ��ڴ��
	DWORD dwWaveMaxSize = m_pDecoder->GetDecodedMaxSize();
	EnterCriticalSection(&m_cs);
	m_waveData.wfmtx = m_pDecoder->GetWaveFromatX();
	m_waveData.pData = (LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwWaveMaxSize);
	LeaveCriticalSection(&m_cs);

	// ���ûص�����

	// ���������߳�
	if (m_hThreadDecode == NULL)
	{
		m_msgDecodeThread = TMSG_ALIVE;
		m_hThreadDecode = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DecodeThread, (LPVOID)this, CREATE_SUSPENDED, NULL);
		ATLASSERT(m_hThreadDecode);
		ResumeThread(m_hThreadDecode);
	}

	// �ȴ����뻺���ź�
	WaitForSingleObject(m_hEventDecode, INFINITE);

	// ���������߳�
	if (m_hThreadPlay == NULL)
	{
		m_msgPlayThread = TMSG_ALIVE;
		m_hThreadPlay = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PlayThread, (LPVOID)this, CREATE_SUSPENDED, NULL);
		ATLASSERT(m_hThreadPlay);
		ResumeThread(m_hThreadPlay);
	}

	m_ePlayStat = Play_Playing;
}

void WavePlayer::Pause()
{
	if (m_hWaveoutDev)
		waveOutPause(m_hWaveoutDev);
}

void WavePlayer::Resume()
{
	if (m_hWaveoutDev)
		waveOutRestart(m_hWaveoutDev);
}
void WavePlayer::Stop()
{
	m_ePlayStat = Play_Stop;

	UnInit();

	OnPlayFinished();
}

void WavePlayer::SetVolume(DWORD dwVolume)
{
	waveOutSetVolume(m_hWaveoutDev, dwVolume);
}

DWORD WavePlayer::Length(LPCTSTR lpszFile)
{
	// ���ý�����
	if (m_pDecoder == NULL)
	{
		m_pDecoder = new AMRDecoder(lpszFile);
	}
	else
	{
		m_pDecoder->SetFilePathName(lpszFile);
	}

	DWORD dwTimeLength = 0;
	if (m_pDecoder)
	{
		dwTimeLength = (DWORD)m_pDecoder->GetTimeLength();
	}

	return dwTimeLength;
}

BOOL WavePlayer::WaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	WavePlayer* pPlayer = (WavePlayer*)dwInstance;

	return pPlayer->WaveOutProcImpl(hwo, uMsg, dwParam1, dwParam2);
}

BOOL WavePlayer::WaveOutProcImpl(HWAVEOUT hwo, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
{
	// ���Դ򿪺͹ر��豸ʱ�Ĵ���
	if (uMsg == WOM_DONE)
	{
		// ÿһ�鲥����֮����ζ�ſ��л����������
		EnterCriticalSection(&m_cs);
		++m_wBlock.wfreeblock;
		LeaveCriticalSection(&m_cs);
	}

	return TRUE;
}

unsigned int WavePlayer::PlayThread(LPVOID lp)
{
	WavePlayer *pPlayer = (WavePlayer *)lp;

	return pPlayer->PlayThreadProcImpl();
}

unsigned int WavePlayer::PlayThreadProcImpl()
{
	/// ����Ϊ�Ĵ�����������Ϊ�����ᱻ��Ƶ�ʵ�ʹ�ã����ڱ������Ż�
	register    ThreadMsg       tmsg = TMSG_ALIVE;

	/// �߳�ѭ��
	while (tmsg)
	{
		// ÿ��ѭ���󣬽���CPU����Ȩ�����ڴ˴�����Ϊ������continue���
		Sleep(10);

		/// ���ȼ���߳���Ϣ
		EnterCriticalSection(&m_cs);
		tmsg = m_msgPlayThread;
		LeaveCriticalSection(&m_cs);
		// �߳�Ҫ�������˳��߳�ѭ��
		if (!tmsg)   break;

		// ����豸Ϊ�գ���ʾ��û�д��豸����Ҫ���豸
		if (m_hWaveoutDev == NULL)
		{
			EnterCriticalSection(&m_cs);
			MMRESULT mmres = waveOutOpen(&m_hWaveoutDev, WAVE_MAPPER, &m_waveData.wfmtx, (DWORD_PTR)WaveOutProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
			LeaveCriticalSection(&m_cs);
			if (mmres != MMSYSERR_NOERROR)
			{
				// failed, try again.
				continue;
			}
		}

		// �����л����
		EnterCriticalSection(&m_cs);
		int free = m_wBlock.wfreeblock;
		LeaveCriticalSection(&m_cs);

		// ���û�п��еĻ����ˣ��ȴ�...
		if (free < BP_TURN)
		{
			continue;
		}

		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////
		///                       < ������ѭ�� >                              ///
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////

		WAVEHDR     *current = NULL;

		/// BP_TURNΪÿ��д�벥�Ŷ��еĿ���
		for (unsigned int m = 0; m < BP_TURN; m++)
		{
			/// ��ǰ���в��Ż����
			current = &m_wBlock.pWaveHdr[m_wBlock.wcurrblock];

			// ������Ҫ�����û�б�Unprepare��
			if (current->dwFlags & WHDR_PREPARED)
			{
				waveOutUnprepareHeader(m_hWaveoutDev, current, sizeof(WAVEHDR));
			}

			/// ����ʣ����Ҫ���ŵ�����
			EnterCriticalSection(&m_cs);
			unsigned long left = m_waveData.dwSize - m_wBlock.wpos;
			unsigned int bDecodeFinished = m_waveData.bDecodeFinished;
			LeaveCriticalSection(&m_cs);
			unsigned long chunk = 0;

			if (left >= BLOCK_SIZE)
			{
				chunk = BLOCK_SIZE;
			}
			else if (!bDecodeFinished)
			{
				// ������뻹û�н��������е��������в���������һ������飬�Ȳ�д�뻺��
				break;
			}
			else if (left && left < BLOCK_SIZE)
			{
				chunk = left;
			}
			else
			{
				//////////////////////////////////////////////////////////////////////
				///                 < �������>                                    ///
				//////////////////////////////////////////////////////////////////////

				/// ��ȡ���л��������
				EnterCriticalSection(&m_cs);
				int free = m_wBlock.wfreeblock;
				LeaveCriticalSection(&m_cs);

				/// �����еĻ���鶼�������ˣ�����ζ�Ų����������
				if (free == BLOCK_COUNT)
				{
					/// Unprepare�����
					for (int j = 0; j < m_wBlock.wfreeblock; j++)
					{
						if (m_wBlock.pWaveHdr[j].dwFlags & WHDR_PREPARED)
						{
							waveOutUnprepareHeader(m_hWaveoutDev, &m_wBlock.pWaveHdr[j], sizeof(WAVEHDR));
						}
					}

					// ��ʱ�����������Ĳ�����ɣ��ر��߳�
					tmsg = TMSG_CLOSE;
					// ����������¼�
					OnPlayFinished();
				}

				// ��break��������ѭ����û�������߳�ѭ��
				break;
			}

			/// prepare current wave data block header
			EnterCriticalSection(&m_cs);
			memcpy(current->lpData, &m_waveData.pData[m_wBlock.wpos], chunk);
			LeaveCriticalSection(&m_cs);

			current->dwBufferLength = chunk;   // sizeof block
			m_wBlock.wpos += chunk;   // update position

			/// prepare for playback
			waveOutPrepareHeader(m_hWaveoutDev, current, sizeof(WAVEHDR));

			/// push to the queue
			waveOutWrite(m_hWaveoutDev, current, sizeof(WAVEHDR));

			/// ��С���п����
			EnterCriticalSection(&m_cs);
			m_wBlock.wfreeblock--;
			LeaveCriticalSection(&m_cs);

			/// ʹ��ǰ���п�ָ����һ��
			m_wBlock.wcurrblock++;
			m_wBlock.wcurrblock %= BLOCK_COUNT;
		}

	}/// thread

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	///
	///            < force to close device which are still playing > 
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	if (m_hWaveoutDev)
	{
		waveOutReset(m_hWaveoutDev);

		/// unprepare any blocks that are still prepared
		for (int j = 0; j < BLOCK_COUNT; j++)
		{
			if (m_wBlock.pWaveHdr[j].dwFlags & WHDR_PREPARED)
			{
				waveOutUnprepareHeader(m_hWaveoutDev, &m_wBlock.pWaveHdr[j], sizeof(WAVEHDR));
			}
		}

		waveOutClose(m_hWaveoutDev);
		m_hWaveoutDev = NULL;
	}

	return THREAD_EXIT;
}

unsigned int WavePlayer::DecodeThread(LPVOID lp)
{
	WavePlayer* pThis = (WavePlayer*)lp;

	return pThis->DecodeThreadProcImpl();
}

unsigned int WavePlayer::DecodeThreadProcImpl()
{
	if (m_pDecoder == NULL || !m_pDecoder->IsVaild())
		return 0;

	// ��ʼ���룬��ʼ��������
	if (!m_pDecoder->BeginDecode())
	{
		return 0;
	}

	// ������ʱ�ڴ�飬�洢�����Ĳ�������
	DWORD dwFrameMaxSize = m_pDecoder->GetDecodedFrameMaxSize();
	LPSTR pBufferBase = (LPSTR)malloc(dwFrameMaxSize);
	ATLASSERT(pBufferBase);
	memset(pBufferBase, 0, dwFrameMaxSize);
	if (pBufferBase == NULL) return 0;

	register ThreadMsg tmsg = TMSG_ALIVE;

	DWORD dwSizeAmount = 0;
	while (!m_pDecoder->IsEOF() && tmsg)
	{
		// ���룬ÿ֡
		DWORD dwSize = m_pDecoder->Decode(pBufferBase);
		dwSizeAmount += dwSize;

		// �����������д�뻺�������������߳�ʹ��
		EnterCriticalSection(&m_cs);
		memcpy(m_waveData.pData + m_waveData.dwSize, pBufferBase, dwSize);
		m_waveData.dwSize += dwSize;
		LeaveCriticalSection(&m_cs);

		// ������������������һ�����Ż���ʱ�������źţ�֪ͨ���Կ�ʼ������
		if (dwSizeAmount > BLOCK_SIZE)
		{
			dwSizeAmount = 0;
			SetEvent(m_hEventDecode);
		}

		// ��ʡCPUʱ�䣬��CPU��ʱ��ȥ�ɱ���¶�
		Sleep(1);

		// ����߳��Ƿ񽫱�ǿ���˳�
		EnterCriticalSection(&m_cs);
		tmsg = m_msgDecodeThread;
		LeaveCriticalSection(&m_cs);
	}

	// �����������С����������һ�����Ż��棬ҲҪ�����ź�
	if (dwSizeAmount > 0)
	{
		SetEvent(m_hEventDecode);
	}

	m_waveData.bDecodeFinished = true;

	// �������
	m_pDecoder->EndDecode();

	free(pBufferBase);
	pBufferBase = NULL;

	return 0;
}

void WavePlayer::OnPlayFinished()
{
}