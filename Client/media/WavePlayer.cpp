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

	// 为播放缓存申请内存堆
	m_wBlock.pWaveHdr = allocBlocks(BLOCK_SIZE, BLOCK_COUNT);
	m_wBlock.wfreeblock = BLOCK_COUNT;
	m_wBlock.wcurrblock = 0;
	m_wBlock.wpos = 0;

	m_hEventDecode = CreateEvent(NULL, FALSE, FALSE, NULL);
	ATLASSERT(m_hEventDecode);

	m_pDecoder = new AMRDecoder();
	ATLASSERT(m_pDecoder);

	// 初始化临界区
	InitializeCriticalSection(&m_cs);
}

WavePlayer::~WavePlayer(void)
{
	UnInit();

	// 释放播放缓存
	freeBlocks(m_wBlock.pWaveHdr);
	memset(&m_wBlock, 0, sizeof(WaveBlock));

	if (m_hEventDecode)
	{
		CloseHandle(m_hEventDecode);
		m_hEventDecode = NULL;
	}

	// 销毁临界区
	DeleteCriticalSection(&m_cs);
}

//************************************
// Method:    申请播放缓存
// FullName:  WavePlayer::allocBlocks
// Access:    protected 
// Returns:   WAVEHDR*
// Parameter: int size，每块波形数据缓存的大小
// Parameter: int count，缓存块数
//************************************
WAVEHDR* WavePlayer::allocBlocks(int size, int count)
{
	unsigned char* buffer;
	int i;
	WAVEHDR* blocks;
	DWORD totalBufferSize = (size + sizeof(WAVEHDR)) * count;

	//  一次性分配所有缓存，不需多次申请，释放简单
	if ((buffer = (UCHAR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBufferSize)) == NULL)
		return NULL;

	// 设置缓存数据指针
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
// Method:    释放播放缓存
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
		// 终止播放线程
		EnterCriticalSection(&m_cs);
		m_msgPlayThread = TMSG_CLOSE;
		LeaveCriticalSection(&m_cs);
		WaitForSingleObject(m_hThreadPlay, INFINITE);

		CloseHandle(m_hThreadPlay);
		m_hThreadPlay = NULL;
	}

	if (m_hThreadDecode)
	{
		// 终止解码线程
		EnterCriticalSection(&m_cs);
		m_msgDecodeThread = TMSG_CLOSE;
		LeaveCriticalSection(&m_cs);
		WaitForSingleObject(m_hThreadDecode, INFINITE);

		CloseHandle(m_hThreadDecode);
		m_hThreadDecode = NULL;
	}

	// 释放波形数据并恢复初始化样子，以供下次使用
	if (m_waveData.pData)
	{
		HeapFree(GetProcessHeap(), 0, m_waveData.pData);
		m_waveData.pData = NULL;
	}
	memset(&m_waveData, 0, sizeof(WaveData));

	// 关闭播放设备
	if (m_hWaveoutDev)
	{
		waveOutReset(m_hWaveoutDev);
		waveOutClose(m_hWaveoutDev);
		m_hWaveoutDev = NULL;
	}

	// 重设播放缓存块游标
	m_wBlock.wfreeblock = BLOCK_COUNT;
	m_wBlock.wcurrblock = 0;
	m_wBlock.wpos = 0;

	m_ePlayStat = Play_Stop;
}

void WavePlayer::Play(LPCTSTR lpszFile, DWORD* pLength)
{
	// 如果已经有播放的了，先停止
	if (m_ePlayStat != Play_Stop)
	{
		Stop();
	}

	// 设置解码器
	if (m_pDecoder == NULL)
	{
		m_pDecoder = new AMRDecoder(lpszFile);
	}
	else
	{
		m_pDecoder->SetFilePathName(lpszFile);
	}

	// 取播放时间
	if (pLength)
	{
		*pLength = (DWORD)m_pDecoder->GetTimeLength();
	}

	// 申请解码后的数据堆内存块
	DWORD dwWaveMaxSize = m_pDecoder->GetDecodedMaxSize();
	EnterCriticalSection(&m_cs);
	m_waveData.wfmtx = m_pDecoder->GetWaveFromatX();
	m_waveData.pData = (LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwWaveMaxSize);
	LeaveCriticalSection(&m_cs);

	// 设置回调函数

	// 创建解码线程
	if (m_hThreadDecode == NULL)
	{
		m_msgDecodeThread = TMSG_ALIVE;
		m_hThreadDecode = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DecodeThread, (LPVOID)this, CREATE_SUSPENDED, NULL);
		ATLASSERT(m_hThreadDecode);
		ResumeThread(m_hThreadDecode);
	}

	// 等待解码缓存信号
	WaitForSingleObject(m_hEventDecode, INFINITE);

	// 创建播放线程
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
	// 设置解码器
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
	// 忽略打开和关闭设备时的处理
	if (uMsg == WOM_DONE)
	{
		// 每一块播放完之后，意味着空闲缓存块数增加
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
	/// 定义为寄存器变量，因为它将会被高频率的使用，用于编译器优化
	register    ThreadMsg       tmsg = TMSG_ALIVE;

	/// 线程循环
	while (tmsg)
	{
		// 每次循环后，交出CPU控制权，放在此处，因为下面有continue语句
		Sleep(10);

		/// 首先检查线程消息
		EnterCriticalSection(&m_cs);
		tmsg = m_msgPlayThread;
		LeaveCriticalSection(&m_cs);
		// 线程要结束，退出线程循环
		if (!tmsg)   break;

		// 如果设备为空，表示还没有打开设备，需要打开设备
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

		// 检查空闲缓存块
		EnterCriticalSection(&m_cs);
		int free = m_wBlock.wfreeblock;
		LeaveCriticalSection(&m_cs);

		// 如果没有空闲的缓存了，等待...
		if (free < BP_TURN)
		{
			continue;
		}

		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////
		///                       < 播放主循环 >                              ///
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////

		WAVEHDR     *current = NULL;

		/// BP_TURN为每次写入播放队列的块数
		for (unsigned int m = 0; m < BP_TURN; m++)
		{
			/// 当前空闲播放缓存块
			current = &m_wBlock.pWaveHdr[m_wBlock.wcurrblock];

			// 首先需要检查有没有被Unprepare掉
			if (current->dwFlags & WHDR_PREPARED)
			{
				waveOutUnprepareHeader(m_hWaveoutDev, current, sizeof(WAVEHDR));
			}

			/// 计算剩余需要播放的数据
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
				// 如果解码还没有结束，现有的数据量有不足以填满一个缓存块，先不写入缓存
				break;
			}
			else if (left && left < BLOCK_SIZE)
			{
				chunk = left;
			}
			else
			{
				//////////////////////////////////////////////////////////////////////
				///                 < 播放完成>                                    ///
				//////////////////////////////////////////////////////////////////////

				/// 获取空闲缓存块数量
				EnterCriticalSection(&m_cs);
				int free = m_wBlock.wfreeblock;
				LeaveCriticalSection(&m_cs);

				/// 当所有的缓存块都播放完了，才意味着播放真正完成
				if (free == BLOCK_COUNT)
				{
					/// Unprepare缓存块
					for (int j = 0; j < m_wBlock.wfreeblock; j++)
					{
						if (m_wBlock.pWaveHdr[j].dwFlags & WHDR_PREPARED)
						{
							waveOutUnprepareHeader(m_hWaveoutDev, &m_wBlock.pWaveHdr[j], sizeof(WAVEHDR));
						}
					}

					// 此时，才算真正的播放完成，关闭线程
					tmsg = TMSG_CLOSE;
					// 处理播放完成事件
					OnPlayFinished();
				}

				// 此break仅跳出该循环，没有跳出线程循环
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

			/// 减小空闲块计数
			EnterCriticalSection(&m_cs);
			m_wBlock.wfreeblock--;
			LeaveCriticalSection(&m_cs);

			/// 使当前空闲块指向下一个
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

	// 开始解码，初始化解码器
	if (!m_pDecoder->BeginDecode())
	{
		return 0;
	}

	// 申请临时内存块，存储解码后的波形数据
	DWORD dwFrameMaxSize = m_pDecoder->GetDecodedFrameMaxSize();
	LPSTR pBufferBase = (LPSTR)malloc(dwFrameMaxSize);
	ATLASSERT(pBufferBase);
	memset(pBufferBase, 0, dwFrameMaxSize);
	if (pBufferBase == NULL) return 0;

	register ThreadMsg tmsg = TMSG_ALIVE;

	DWORD dwSizeAmount = 0;
	while (!m_pDecoder->IsEOF() && tmsg)
	{
		// 解码，每帧
		DWORD dwSize = m_pDecoder->Decode(pBufferBase);
		dwSizeAmount += dwSize;

		// 将解码后数据写入缓存区，供播放线程使用
		EnterCriticalSection(&m_cs);
		memcpy(m_waveData.pData + m_waveData.dwSize, pBufferBase, dwSize);
		m_waveData.dwSize += dwSize;
		LeaveCriticalSection(&m_cs);

		// 当解码数据量操作了一个播放缓存时，发个信号，通知可以开始播放了
		if (dwSizeAmount > BLOCK_SIZE)
		{
			dwSizeAmount = 0;
			SetEvent(m_hEventDecode);
		}

		// 节省CPU时间，让CPU有时间去干别的事儿
		Sleep(1);

		// 检测线程是否将被强制退出
		EnterCriticalSection(&m_cs);
		tmsg = m_msgDecodeThread;
		LeaveCriticalSection(&m_cs);
	}

	// 如果数据量很小，根本不足一个播放缓存，也要发个信号
	if (dwSizeAmount > 0)
	{
		SetEvent(m_hEventDecode);
	}

	m_waveData.bDecodeFinished = true;

	// 解码结束
	m_pDecoder->EndDecode();

	free(pBufferBase);
	pBufferBase = NULL;

	return 0;
}

void WavePlayer::OnPlayFinished()
{
}