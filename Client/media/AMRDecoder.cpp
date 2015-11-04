#include "stdafx.h"
#include <stdio.h>
#include <string>

#define MMS_IO

extern "C"
{
#include "../amr/typedef.h"
#include "../amr/n_proc.h"
#include "../amr/cnst.h"
#include "../amr/mode.h"
#include "../amr/frame.h"
#include "../amr/strfunc.h"
#include "../amr/sp_dec.h"
#include "../amr/d_homing.h"
#include "../amr/sp_dec.h "
}

#include "AMRDecoder.h"

#ifndef AMR_MAGIC_NUMBER
#define AMR_MAGIC_NUMBER "#!AMR\n"
#endif

#define MAX_PACKED_SIZE (MAX_SERIAL_SIZE / 8 + 2)
/* frame size in serial bitstream file (frame type + serial stream + flags) */
#define SERIAL_FRAMESIZE (1+MAX_SERIAL_SIZE+5)

static const int amrFramesSize[16] = { 13, 14, 16, 18, 20, 21, 27, 32, 6, 0, 0, 0, 0, 0, 0, 0 }; // amr每帧大小,byte
static const int amrPackedSize[16] = { 12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0 }; // amr每帧语音数据大小,byte
static const int amrFrameTime = 20;                                                            // 每帧播放时间，单位ms

AMRDecoder::AMRDecoder(void)
	: m_pBaseAddress(NULL)
	, m_pCurAddress(NULL)
	, m_liFileSize(0)
	, m_dwFrameCount(0)
	, m_pvDecoderState(NULL)
{
}

AMRDecoder::AMRDecoder(LPCTSTR lpszFile)
	: m_pBaseAddress(NULL)
	, m_pCurAddress(NULL)
	, m_liFileSize(0)
	, m_dwFrameCount(0)
	, m_pvDecoderState(NULL)
	, m_sFilePathName(lpszFile)
{
	// 构造时，将文件映射到内存

	// 打开文件
	HANDLE hFileAmr = CreateFile(lpszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ATLASSERT(hFileAmr);
	if (hFileAmr == NULL)    return;

	// 取文件大小
	LARGE_INTEGER liFileSize;
	memset(&liFileSize, 0, sizeof(LARGE_INTEGER));
	if (!GetFileSizeEx(hFileAmr, &liFileSize))
	{
		CloseHandle(hFileAmr);
		return;
	}
	m_liFileSize = liFileSize.QuadPart;

	// 创建文件映射对象
	HANDLE hFileMapping = CreateFileMapping(hFileAmr, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(hFileAmr);
	ATLASSERT(hFileMapping);
	if (hFileMapping == NULL) return;

	// 映射
	m_pBaseAddress = (LPSTR)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hFileMapping);
	ATLASSERT(m_pBaseAddress);

	// 设置游标
	m_pCurAddress = m_pBaseAddress;
}

AMRDecoder::~AMRDecoder(void)
{
	if (m_pBaseAddress)
	{
		UnmapViewOfFile(m_pBaseAddress);
		m_pBaseAddress = NULL;
	}

	EndDecode();
}

bool AMRDecoder::IsVaild()
{
	if (m_pBaseAddress == NULL || m_liFileSize == 0)
		return false;

	// 检查amr文件头
	char magic[8];
	memcpy(magic, m_pBaseAddress, strlen(AMR_MAGIC_NUMBER));

	return strncmp(magic, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER)) == 0;
}

ULONGLONG AMRDecoder::GetTimeLength()
{
	if (!IsVaild())  return 0;

	return GetFrameCount() * amrFrameTime;
}

WAVEFORMATEX AMRDecoder::GetWaveFromatX()
{
	WAVEFORMATEX wfmtx;
	memset(&wfmtx, 0, sizeof(WAVEFORMATEX));
	wfmtx.wFormatTag = WAVE_FORMAT_PCM;
	wfmtx.nChannels = 1; // 单声道
	wfmtx.nSamplesPerSec = 8000; // 8khz
	wfmtx.nAvgBytesPerSec = 16000;
	wfmtx.nBlockAlign = 2;
	wfmtx.wBitsPerSample = 16; // 16位
	wfmtx.cbSize = 0;

	return wfmtx;
}

BOOL AMRDecoder::BeginDecode()
{
	if (!IsVaild())  return FALSE;

	Speech_Decode_FrameState *& speech_decoder_state = (Speech_Decode_FrameState*&)m_pvDecoderState;

	/* init decoder */
	if (Speech_Decode_Frame_init(&speech_decoder_state, "Decode"))
	{
		if (speech_decoder_state)
		{
			Speech_Decode_Frame_exit(&speech_decoder_state);
		}

		return FALSE;
	}

	ATLASSERT(m_pvDecoderState);

	m_pCurAddress = m_pBaseAddress + strlen(AMR_MAGIC_NUMBER);

	return TRUE;
}

void AMRDecoder::EndDecode()
{
	if (m_pvDecoderState)
	{
		Speech_Decode_Frame_exit((Speech_Decode_FrameState**)&m_pvDecoderState);
		m_pvDecoderState = NULL;
	}
}

bool AMRDecoder::IsEOF()
{
	return m_pCurAddress >= m_pBaseAddress + m_liFileSize;
}

DWORD AMRDecoder::Decode(LPSTR& pData)
{
	// 如果地址已经超出范围，返回数据大小为0
	if (m_pCurAddress >= m_pBaseAddress + m_liFileSize)
		return 0;

	Word16 serial[SERIAL_FRAMESIZE];   /* coded bits                    */
	Word16 pcmFrame[L_FRAME];          /* Synthesis                     */

	UWord8 toc, q, ft;
	UWord8 packed_bits[MAX_PACKED_SIZE];

	RXFrameType rx_type = (RXFrameType)0;
	Mode mode = (Mode)0;

	Word16 reset_flag = 0;
	Word16 reset_flag_old = 1;

	Speech_Decode_FrameState *speech_decoder_state = (Speech_Decode_FrameState*)m_pvDecoderState;

	// 解码的核心代码
	{
		// toc为每一个Amr帧的首字节数据
		toc = *m_pCurAddress++;
		/* read rest of the frame based on ToC byte */
		q = (toc >> 2) & 0x01;
		ft = (toc >> 3) & 0x0F;
		memcpy(packed_bits, m_pCurAddress, amrPackedSize[ft]);
		m_pCurAddress += amrPackedSize[ft];

		rx_type = UnpackBits(q, ft, packed_bits, &mode, &serial[1]);

		if (rx_type == RX_NO_DATA)
		{
			mode = speech_decoder_state->prev_mode;
		}
		else
		{
			speech_decoder_state->prev_mode = mode;
		}

		/* if homed: check if this frame is another homing frame */
		if (reset_flag_old == 1)
		{
			/* only check until end of first subframe */
			reset_flag = decoder_homing_frame_test_first(&serial[1], mode);
		}
		/* produce encoder homing frame if homed & input=decoder homing frame */
		if ((reset_flag != 0) && (reset_flag_old != 0))
		{
			for (int i = 0; i < L_FRAME; i++)
			{
				pcmFrame[i] = EHF_MASK;
			}
		}
		else
		{
			/* decode frame */
			Speech_Decode_Frame(speech_decoder_state, mode, &serial[1], rx_type, pcmFrame);
		}

		/* if not homed: check whether current frame is a homing frame */
		if (reset_flag_old == 0)
		{
			/* check whole frame */
			reset_flag = decoder_homing_frame_test(&serial[1], mode);
		}
		/* reset decoder if current frame is a homing frame */
		if (reset_flag != 0)
		{
			Speech_Decode_Frame_reset(speech_decoder_state);
		}
		reset_flag_old = reset_flag;
	}

	// 输出
	if (pData)
	{
		memcpy(pData, pcmFrame, sizeof(Word16) * L_FRAME);
	}

	// 返回输出大小，bytes
	return L_FRAME * sizeof(Word16);
}

DWORD AMRDecoder::GetDecodedFrameMaxSize()
{
	return L_FRAME * sizeof(Word16);
}

DWORD AMRDecoder::GetDecodedMaxSize()
{
	return GetFrameCount() * GetDecodedFrameMaxSize();
}

void AMRDecoder::SetFilePathName(LPCTSTR lpszFile)
{
	if (m_sFilePathName.CompareNoCase(lpszFile) == 0)
		return;

	// 关闭以前的映射对象
	if (m_pBaseAddress)
	{
		UnmapViewOfFile(m_pBaseAddress);
		m_pBaseAddress = NULL;
	}

	// 关闭解码器
	EndDecode();

	// 恢复初始值
	m_pCurAddress = NULL;
	m_liFileSize = 0;
	m_dwFrameCount = 0;
	m_pvDecoderState = NULL;

	// 打开文件
	HANDLE hFileAmr = CreateFile(lpszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ATLASSERT(hFileAmr);
	if (hFileAmr == NULL)    return;

	// 取文件大小
	LARGE_INTEGER liFileSize;
	memset(&liFileSize, 0, sizeof(LARGE_INTEGER));
	if (!GetFileSizeEx(hFileAmr, &liFileSize))
	{
		CloseHandle(hFileAmr);
		return;
	}
	m_liFileSize = liFileSize.QuadPart;

	// 创建文件映射对象
	HANDLE hFileMapping = CreateFileMapping(hFileAmr, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(hFileAmr);
	ATLASSERT(hFileMapping);
	if (hFileMapping == NULL) return;

	// 映射
	m_pBaseAddress = (LPSTR)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hFileMapping);
	ATLASSERT(m_pBaseAddress);

	// 设置游标
	m_pCurAddress = m_pBaseAddress;
}

DWORD AMRDecoder::GetFrameCount()
{
	ATLASSERT(IsVaild());

	if (m_dwFrameCount <= 0)
	{
		m_dwFrameCount = 0;

		LPSTR pCur = m_pBaseAddress + strlen(AMR_MAGIC_NUMBER);

		unsigned char toc = 0, ft = 0;
		while (pCur < m_pBaseAddress + m_liFileSize)
		{
			// toc为每一个Amr帧的首字节数据
			toc = *pCur++;
			ft = (toc >> 3) & 0x0F;
			pCur += amrPackedSize[ft];
			++m_dwFrameCount;
		}
	}

	return m_dwFrameCount;
}