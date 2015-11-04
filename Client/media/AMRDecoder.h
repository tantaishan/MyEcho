#pragma once
#include <MMSystem.h>

class AMRDecoder
{
public:
	AMRDecoder(void);
	AMRDecoder(LPCTSTR lpszFile);
	virtual ~AMRDecoder(void);

private: // 屏蔽拷贝构造函数和赋值运算
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
	/// 设置需解码文件路径
	virtual void SetFilePathName(LPCTSTR lpszFile);
	/// 获取总时间长度，单位ms
	virtual ULONGLONG GetTimeLength();
	/// 获取解码后的波形格式
	virtual WAVEFORMATEX GetWaveFromatX();
	/// 开始解码，初始化解码器
	virtual BOOL BeginDecode();
	/// 解码，每解码一帧，游标后移至下一帧,返回解码后的帧大小，输出解码后的波形数据
	virtual DWORD Decode(LPSTR& pData);
	/// 判断是否解码结束
	virtual bool IsEOF();
	/// 结束解码，销毁解码器
	virtual void EndDecode();
	/// 判断解码器是否正常
	virtual bool IsVaild();
	/// 获取解码后的波形数据大小，单位byte
	virtual DWORD GetDecodedMaxSize();
	/// 获取解码后的波形数据帧大小，单位byte
	virtual DWORD GetDecodedFrameMaxSize();

private:
	DWORD GetFrameCount();

private:
	LPSTR       m_pBaseAddress;         // 文件映射内存块首地址
	LONGLONG    m_liFileSize;           // 文件映射内存块大小
	ULONG       m_dwFrameCount;         // 帧总数
	LPSTR       m_pCurAddress;          // 解码游标，指示当前解码位置
	LPVOID      m_pvDecoderState;       // 解码器状态指针
	CString     m_sFilePathName;        // 解码文件路径
};
