#pragma once
#include <MMSystem.h>

class AMRDecoder
{
public:
	AMRDecoder(void);
	AMRDecoder(LPCTSTR lpszFile);
	virtual ~AMRDecoder(void);

private: // ���ο������캯���͸�ֵ����
	AMRDecoder(const AMRDecoder&)
	{
		ATLASSERT(FALSE);
	}
	AMRDecoder& operator=(const AMRDecoder&)
	{
		ATLASSERT(FALSE);
		return *this;
	}

public:
	/// ����������ļ�·��
	virtual void SetFilePathName(LPCTSTR lpszFile);
	/// ��ȡ��ʱ�䳤�ȣ���λms
	virtual ULONGLONG GetTimeLength();
	/// ��ȡ�����Ĳ��θ�ʽ
	virtual WAVEFORMATEX GetWaveFromatX();
	/// ��ʼ���룬��ʼ��������
	virtual BOOL BeginDecode();
	/// ���룬ÿ����һ֡���α��������һ֡,���ؽ�����֡��С����������Ĳ�������
	virtual DWORD Decode(LPSTR& pData);
	/// �ж��Ƿ�������
	virtual bool IsEOF();
	/// �������룬���ٽ�����
	virtual void EndDecode();
	/// �жϽ������Ƿ�����
	virtual bool IsVaild();
	/// ��ȡ�����Ĳ������ݴ�С����λbyte
	virtual DWORD GetDecodedMaxSize();
	/// ��ȡ�����Ĳ�������֡��С����λbyte
	virtual DWORD GetDecodedFrameMaxSize();

private:
	DWORD GetFrameCount();

private:
	LPSTR       m_pBaseAddress;         // �ļ�ӳ���ڴ���׵�ַ
	LONGLONG    m_liFileSize;           // �ļ�ӳ���ڴ���С
	ULONG       m_dwFrameCount;         // ֡����
	LPSTR       m_pCurAddress;          // �����αָ꣬ʾ��ǰ����λ��
	LPVOID      m_pvDecoderState;       // ������״ָ̬��
	CString     m_sFilePathName;        // �����ļ�·��
};
