#include "StdAfx.h"
#include "PcmToAmr.h"
#define Min(a,b) ((a) < (b) ? (a):(b))


CPcmToAmr::CPcmToAmr( std::wstring strFileName, unsigned int bitrate /*= 16*/,unsigned int inputRate /*=8000*/ ):
m_bFileEmpty(TRUE)
{
	m_strFileName = strFileName;
	m_fpFile = NULL;
	_wfopen_s(&m_fpFile, strFileName.c_str(), TEXT("wb"));
	if (m_fpFile == NULL) 
	{
		throw "Can't create MP3 file";
	}
	fwrite(AMR_MAGIC_NUMBER, sizeof(char), strlen(AMR_MAGIC_NUMBER), m_fpFile);//д��amr�ļ�ͷ
	m_piEnstate = (int *)Encoder_Interface_init(0);  

}

CPcmToAmr::~CPcmToAmr( void )
{
	CloseFile();

}


/******************************************************************
��������:ReceiveBuffer
��������:�����ܵ�������ת��Ϊamr����
�������:
�������:
����: ������
��ע:
******************************************************************/
void CPcmToAmr::ReceiveBuffer( LPSTR lpData, DWORD dwBytesRecorded )
{

}

/******************************************************************
��������:ConVetDate
��������:ת��һ֡������
�������:
�������:
����: ������
��ע:
******************************************************************/
void CPcmToAmr::ConVetDate( LPSTR lpData, DWORD dwDateSize )
{
	if (dwDateSize > PCM_FRAME_SIZE)
	{
		int iLog = 0;//warning happened
	}
	short sDate[PCM_FRAME_SIZE] = {0};
	int iTrueSize = Min(dwDateSize, PCM_FRAME_SIZE);
	unsigned char amrFrame[MAX_AMR_FRAME_SIZE];

	for(int i=0; i<iTrueSize; i++)//�˴����ڵ�������,������������п��Բ�����
	{
		sDate[i] = (short)lpData[i + 0];
	
	}
	enum Mode req_mode = MR122;  
	
	int byte_counter = 0;
	byte_counter = Encoder_Interface_Encode(m_piEnstate, req_mode, sDate, amrFrame, 0);                         
	fwrite(amrFrame, sizeof (unsigned char), byte_counter, m_fpFile );     
}     

void CPcmToAmr::CloseFile()
{
	if (!m_fpFile)
	{
		return;
	}
	Encoder_Interface_exit(m_piEnstate); 
	fclose(m_fpFile);
	m_fpFile = NULL;

}


