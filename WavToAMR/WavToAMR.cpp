// wavToamrTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h>

#include "interf_enc.h"
#include <memory.h >
#include <string.h >
//#pragma  comment(lib,"AMRNB.lib")
#define AMR_MAGIC_NUMBER "#!AMR\n"

#define PCM_FRAME_SIZE 160 // 8khz 8000*0.02=160
#define MAX_AMR_FRAME_SIZE 32
#define AMR_FRAME_COUNT_PER_SECOND 50
//int amrEncodeMode[] = {4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200}; // amr ���뷽ʽ

typedef struct
{
	char chChunkID[4];
	int nChunkSize;
}XCHUNKHEADER;

typedef struct
{
	short nFormatTag;
	short nChannels;
	int nSamplesPerSec;
	int nAvgBytesPerSec;
	short nBlockAlign;
	short nBitsPerSample;
}WAVEFORMAT;

typedef struct
{
	short nFormatTag;
	short nChannels;
	int nSamplesPerSec;
	int nAvgBytesPerSec;
	short nBlockAlign;
	short nBitsPerSample;
	short nExSize;
}WAVEFORMATX;

typedef struct
{
	char chRiffID[4];
	int nRiffSize;
	char chRiffFormat[4];
}RIFFHEADER;

typedef struct
{
	char chFmtID[4];
	int nFmtSize;
	WAVEFORMAT wf;
}FMTBLOCK;

// WAVE��Ƶ����Ƶ����8khz 
// ��Ƶ������Ԫ�� = 8000*0.02 = 160 (�ɲ���Ƶ�ʾ���)
// ������ 1 : 160
//        2 : 160*2 = 320
// bps��������(sample)��С
// bps = 8 --> 8λ unsigned char
//       16 --> 16λ unsigned short
//int EncodeWAVEFileToAMRFile(const char* pchWAVEFilename, const char* pchAMRFileName, int nChannels, int nBitsPerSample);

// ��AMR�ļ������WAVE�ļ�
//int DecodeAMRFileToWAVEFile(const char* pchAMRFileName, const char* pchWAVEFilename);

void SkipToPCMAudioData(FILE* fpwave)
{
	RIFFHEADER riff;
	FMTBLOCK fmt;
	XCHUNKHEADER chunk;
	WAVEFORMATX wfx;
	int bDataBlock = 0;
	

	// 1. ��RIFFͷ
	fread(&riff, 1, sizeof(RIFFHEADER), fpwave);

	// 2. ��FMT�� - ��� fmt.nFmtSize>16 ˵����Ҫ����һ��������Сû�ж�
	fread(&chunk, 1, sizeof(XCHUNKHEADER), fpwave);
	if ( chunk.nChunkSize>16 )
	{
		int a = sizeof(WAVEFORMATX);//�ṹ�����ж���İ�
		fread(&wfx, 1, chunk.nChunkSize, fpwave);
	}
	else
	{
		memcpy(fmt.chFmtID, chunk.chChunkID, 4);
		fmt.nFmtSize = chunk.nChunkSize;
		fread(&fmt.wf, 1, sizeof(WAVEFORMAT), fpwave);
	}

	// 3.ת��data�� - ��Щ����fact��ȡ�
	while(!bDataBlock)
	{
		fread(&chunk, 1, sizeof(XCHUNKHEADER), fpwave);
		if ( !memcmp(chunk.chChunkID, "data", 4) )
		{
			bDataBlock = 1;
			break;
		}
		// ��Ϊ�������data��,������������
		fseek(fpwave, chunk.nChunkSize, SEEK_CUR);
	}
}

// ��WAVE�ļ���һ��������PCM��Ƶ֡
// ����ֵ: 0-���� >0: ����֡��С
int ReadPCMFrame(short speech[], FILE* fpwave, int nChannels, int nBitsPerSample)
{
	int nRead = 0;
	int x = 0, y=0;
	unsigned short ush1=0, ush2=0, ush=0;

	// ԭʼPCM��Ƶ֡����
	unsigned char pcmFrame_8b1[PCM_FRAME_SIZE];
	unsigned char pcmFrame_8b2[PCM_FRAME_SIZE<<1];
	unsigned short pcmFrame_16b1[PCM_FRAME_SIZE];
	unsigned short pcmFrame_16b2[PCM_FRAME_SIZE<<1];

	if (nBitsPerSample==8 && nChannels==1)
	{
		nRead = fread(pcmFrame_8b1, (nBitsPerSample/8), PCM_FRAME_SIZE*nChannels, fpwave);
		for(x=0; x<PCM_FRAME_SIZE; x++)
		{
			speech[x] =(short)((short)pcmFrame_8b1[x] << 7);
		}
	}
	else
		if (nBitsPerSample==8 && nChannels==2)
		{
			nRead = fread(pcmFrame_8b2, (nBitsPerSample/8), PCM_FRAME_SIZE*nChannels, fpwave);
			for( x=0, y=0; y<PCM_FRAME_SIZE; y++,x+=2 )
			{
				// 1 - ȡ��������֮������
				speech[y] =(short)((short)pcmFrame_8b2[x+0] << 7);
				// 2 - ȡ��������֮������
				//speech[y] =(short)((short)pcmFrame_8b2[x+1] << 7);
				// 3 - ȡ����������ƽ��ֵ
				//ush1 = (short)pcmFrame_8b2[x+0];
				//ush2 = (short)pcmFrame_8b2[x+1];
				//ush = (ush1 + ush2) >> 1;
				//speech[y] = (short)((short)ush << 7);
			}
		}
		else
			if (nBitsPerSample==16 && nChannels==1)
			{
				nRead = fread(pcmFrame_16b1, (nBitsPerSample/8), PCM_FRAME_SIZE*nChannels, fpwave);
				for(x=0; x<PCM_FRAME_SIZE; x++)
				{
					speech[x] = (short)pcmFrame_16b1[x+0];
				}
			}
			else
				if (nBitsPerSample==16 && nChannels==2)
				{
					nRead = fread(pcmFrame_16b2, (nBitsPerSample/8), PCM_FRAME_SIZE*nChannels, fpwave);
					for( x=0, y=0; y<PCM_FRAME_SIZE; y++,x+=2 )
					{
						//speech[y] = (short)pcmFrame_16b2[x+0];
						speech[y] = (short)((int)((int)pcmFrame_16b2[x+0] + (int)pcmFrame_16b2[x+1])) >> 1;
					}
				}

				// ������������ݲ���һ��������PCM֡, �ͷ���0
				if (nRead<PCM_FRAME_SIZE*nChannels) return 0;

				return nRead;
}

// WAVE��Ƶ����Ƶ����8khz // ��Ƶ������Ԫ�� = 8000*0.02 = 160 (�ɲ���Ƶ�ʾ���)// ������ 1 : 160//        2 : 160*2 = 320
// bps��������(sample)��С// bps = 8 --> 8λ unsigned char
//       16 --> 16λ unsigned short
extern "C" __declspec (dllexport) int WavToAMR(const char* pchWAVEFilename, const char* pchAMRFileName, int nChannels, int nBitsPerSample)
{        
	FILE* fpwave;  
	FILE* fpamr;       
	/* input speech vector */    
	short speech[160];          /* counters */        
	int byte_counter, frames = 0, bytes = 0;         
	/* pointer to encoder state structure */       
	int *enstate;                
	/* requested mode */        
	enum Mode req_mode = MR475;
	int dtx = 0;          /* bitstream filetype */       
	unsigned char amrFrame[MAX_AMR_FRAME_SIZE];       
	fpwave = fopen(pchWAVEFilename, "rb");         
	if (fpwave == NULL)       
	{                
		return 0;     
	}          
	// ��������ʼ��amr�ļ�        
	fpamr = fopen(pchAMRFileName, "wb");        
	if (fpamr == NULL)        
	{                 
		fclose(fpwave);     
		return 0;    
	}        
	/* write magic number to indicate single channel AMR file storage format */    
	bytes = fwrite(AMR_MAGIC_NUMBER, sizeof(char), strlen(AMR_MAGIC_NUMBER), fpamr);    
	/* skip to pcm audio data*/       
	SkipToPCMAudioData(fpwave);        
	enstate = (int *)Encoder_Interface_init(dtx);        
	while(1)       
	{                  
		// read one pcm frame               
		if (!ReadPCMFrame(speech, fpwave, nChannels, nBitsPerSample)) break;           
		frames++;                    /* call encoder */               
		byte_counter = Encoder_Interface_Encode(enstate, req_mode, speech, amrFrame, 0);        
		bytes += byte_counter;                  
		fwrite(amrFrame, sizeof (unsigned char), byte_counter, fpamr );     
	}         
	Encoder_Interface_exit(enstate);       
	fclose(fpamr);       
	fclose(fpwave);       
	return frames;
}

