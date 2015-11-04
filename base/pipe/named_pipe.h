//named_pipe.h
//2013.07.10

#ifndef __NAMED_PIPE_H__
#define __NAMED_PIPE_H__


#include <iostream>
#include <string>
#include <queue>

#include "../base/lock/lock.h"

namespace pipe
{

	enum Cmd
	{
		kCmdCommand=0x10,
		kCmdData,
		kPipeClose
	};

#pragma pack(1)
	struct Msg
	{
		int32	command;
		int32	type;
		int32	flags;
		int32	buffer_size;
		WCHAR	buffer[1];
	};
#pragma pack()


	class PipeBase
	{
	public:
		explicit PipeBase();
		void	init();
		virtual int32	create(const WCHAR* name)= 0;
		virtual int32	close()= 0;
		int32	sendToQueue(int32 commnd, int32 type, int32 flags, const char* buffer, int32 buffer_size);
		int32	send(int32 commnd, int32 type, int32 flags, const char* buffer, int32 buffer_size);

	protected:
		int32	pipeCreate(const WCHAR* name);
		int32	pipeOpen(const WCHAR* name);
		int32	pipeClose();
		int32	isOpen();
		int32	accept();
		int32	connect();
		int32	disconnect();
		size_t	read(void* buffer, size_t buffer_size);
		size_t	write(const void* buffer, size_t buffer_size);

		static Msg* BuildMsg(int32 commnd, int32 type, int32 flags, const char* buffer, int32 buffer_size);

	public:
		base::ThreadParam	thread_;
		std::queue<Msg* >	que_msg_;

	protected:
		HANDLE	handle_pipe_;
		int32	is_create_;
		int32	is_connect_;
		std::wstring	name_;
	};

	class SendPipe: public PipeBase
	{
	public:
		explicit SendPipe(){};
		virtual int32	create(const WCHAR* name);
		virtual int32	close();
	};

	class AsyncSendPipe: public PipeBase
	{
	public:
		explicit AsyncSendPipe(){};
		virtual int32	create(const WCHAR* name);
		virtual int32	close();

	private:
		static DWORD WINAPI	sendCallback(void* lpvoid);
	};


	class AnsyncRecvPipe;
	typedef DWORD (*LPANSYN_RECV_PIPE_CALLBACK)(AnsyncRecvPipe* pPipe, Msg* pMsg);
	class AnsyncRecvPipe: public PipeBase
	{
	public:
		explicit AnsyncRecvPipe(LPANSYN_RECV_PIPE_CALLBACK callback){ m_callback = callback; };
		virtual int32	create(const WCHAR* name);
		virtual int32	close();

	private:
		static DWORD WINAPI	recvCallback(void* lpvoid);
		LPANSYN_RECV_PIPE_CALLBACK m_callback;
	};
}

#endif//__NAMED_PIPE_H__
