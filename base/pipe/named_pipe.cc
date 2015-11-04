//named_pipe.cc
//2013.07.10

#include "stdafx.h"
#include "named_pipe.h"
#include "debug.h"


namespace pipe
{
	const int kPipeMaxBufferSize = 256*1024;

	PipeBase::PipeBase()
		: is_create_(FALSE)
		, is_connect_(FALSE)
		, handle_pipe_(0)
	{
	}

	void	PipeBase::init()
	{
	}

	int32	PipeBase::sendToQueue(int32 commnd, int32 type, int32 flags, const char* buffer, int32 buffer_size)
	{
		Msg* msg= BuildMsg(commnd, type, flags, buffer, buffer_size);

		base::TAutoLock lock(thread_.lock_);
		que_msg_.push(msg);

		return TRUE;
	}

	int32	PipeBase::send(int32 commnd, int32 type, int32 flags, const char* buffer, int32 buffer_size)
	{
		int32 ret = FALSE;
		if (isOpen())
		{
			Msg* msg= BuildMsg(commnd, type, flags, buffer, buffer_size);
			ret = write(msg, sizeof(pipe::Msg)+ msg->buffer_size );
			Safe_Delete(msg);
		}

		return ret;
	}

	Msg*	PipeBase::BuildMsg(int32 commnd, int32 type, int32 flags, const char* buffer, int32 buffer_size)
	{
		Msg* msg= reinterpret_cast<Msg*>(malloc(sizeof(Msg) + buffer_size) ) ;
		memset(msg, 0, sizeof(Msg) + buffer_size);
		msg->command		= commnd;
		msg->type			= type;
		msg->flags			= flags;
		msg->buffer_size	= buffer_size;
		memcpy(msg->buffer, buffer, buffer_size);

		return msg;
	}

	int32	PipeBase::pipeCreate(const WCHAR* name)
	{
		if(is_create_ )
		{
			return TRUE;
		}
		name_= name;
		//设置权限，任何用户可以访问
		SECURITY_DESCRIPTOR sd;
		SECURITY_ATTRIBUTES gsAttr;
		InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);
		gsAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		gsAttr.bInheritHandle = FALSE;
		gsAttr.lpSecurityDescriptor = &sd;
		handle_pipe_= CreateNamedPipeW(name_.c_str(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE, 5, kPipeMaxBufferSize, kPipeMaxBufferSize, NMPWAIT_USE_DEFAULT_WAIT, &gsAttr);
		if(INVALID_HANDLE_VALUE==  handle_pipe_)
		{
			is_create_= FALSE;
		}
		else
		{
			is_create_= TRUE;
		}
		return is_create_;
	}

	int32	PipeBase::pipeOpen(const WCHAR* name)
	{
		name_= name;
		handle_pipe_= CreateFile(name_.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(INVALID_HANDLE_VALUE==  handle_pipe_)
		{
			is_create_= FALSE;
		}
		else
		{
			is_create_= TRUE;
		}
		DBG(INFO)  << "pipe client open " << (is_create_ ? "ok" : "err");

		return is_create_;
	}

	int32	PipeBase::pipeClose()
	{
		is_create_= FALSE;
		if(handle_pipe_ )
			return ::CloseHandle(handle_pipe_);
		else
			return TRUE;
	}

	int32	PipeBase::isOpen()
	{
		return is_create_;
	}

	int32	PipeBase::accept()
	{
		if(is_connect_ )
		{
			return TRUE;
		}
		is_connect_= ConnectNamedPipe(handle_pipe_, 0);
		DWORD nerr= GetLastError();
		is_connect_= is_connect_?TRUE:(nerr== ERROR_PIPE_CONNECTED);
		return is_connect_;
	}

	int32	PipeBase::connect()
	{
		is_connect_= WaitNamedPipeW(name_.c_str(), NMPWAIT_NOWAIT);
		DWORD nerr= GetLastError();
		return is_connect_;
	}

	int32	PipeBase::disconnect()
	{
		if(is_connect_ )
		{
			is_connect_= FALSE;
			DisconnectNamedPipe(handle_pipe_);
			return TRUE;
		}
		return FALSE;
	}

	size_t	PipeBase::read(void* buffer, size_t buffer_size)
	{
#if 0
		DWORD read_size= 0;
		BOOL bOk= ReadFile(handle_pipe_, buffer, buffer_size, &read_size, 0);
		//DBG(INFO) << "pipe read " << (bOk ? "ok" : "err") << "len[" << read_size << "]";
		return read_size;
#else
		//	copy form ipc.cpp

		BOOL bRet = FALSE;
		DWORD dwReaded = 0;
		DWORD dwTotalBytesAvail = 0;
		DWORD dwBytesLeftThisMessage = 0;

		bRet = ::PeekNamedPipe(handle_pipe_, buffer, buffer_size, &dwReaded, &dwTotalBytesAvail, &dwBytesLeftThisMessage);
		if(bRet && dwReaded){
			dwReaded = 0;
			bRet = ::ReadFile(handle_pipe_, buffer, buffer_size, &dwReaded, 0);
		}
		return dwReaded;
#endif
	}

	size_t	PipeBase::write(const void* buffer, size_t buffer_size)
	{
		DWORD write_size= 0;
		BOOL bOk =  WriteFile(handle_pipe_, buffer, buffer_size, &write_size, 0);
		//DBG(INFO) << "pipe write " << (bOk ? "ok" : "err") << "len[" << write_size << "]";
		return write_size;
	}


	//////////////////// SendPipe ////////////////////
	int32	SendPipe::create(const WCHAR* name)
	{
		int32 ret = FALSE;

		name_= name;
		if (connect())
		{
			ret = pipeOpen(name_.c_str() );
		}

		return ret;
	}

	int32	SendPipe::close()
	{
		return pipeClose();
	}


	//////////////////// AsyncSendPipe ////////////////////
	int32	AsyncSendPipe::create(const WCHAR* name)
	{
		name_= name;
		thread_.hEventHandle_= CreateEvent(0,TRUE,FALSE,0);
		thread_.hThreadHandle_= CreateThread(0, 0, AsyncSendPipe::sendCallback, reinterpret_cast<void*>(this), 0, NULL) ;
		if(0!= thread_.hThreadHandle_ && 0!= thread_.hEventHandle_)
		{
			return TRUE;
		}
		return FALSE;
	}

	int32	AsyncSendPipe::close()
	{
		::SetEvent(thread_.hEventHandle_ );
		if(WAIT_TIMEOUT== WaitForSingleObject(thread_.hThreadHandle_, 500))
		{
			::TerminateThread(thread_.hThreadHandle_, 0);
		}
		thread_.Release();

		return pipeClose();
	}

	DWORD WINAPI	AsyncSendPipe::sendCallback(void* lpvoid)
	{
		AsyncSendPipe* _this= reinterpret_cast<AsyncSendPipe*>(lpvoid);
		for(_this->connect();  WAIT_OBJECT_0!= WaitForSingleObject(_this->thread_.hEventHandle_, 10 ); )
		{
			//_this->connect();
			if(_this->isOpen() || _this->pipeOpen(_this->name_.c_str() ) )
			{
				while(WAIT_OBJECT_0!= WaitForSingleObject(_this->thread_.hEventHandle_, 10 ) )
				{
					base::TAutoLock lock(_this->thread_.lock_);
					for(;!_this->que_msg_.empty();)
					{
						_this->write(_this->que_msg_.front(), sizeof(pipe::Msg)+ _this->que_msg_.front()->buffer_size );
						Safe_Delete(_this->que_msg_.front());
						_this->que_msg_.pop();
						Sleep(10);
					}
				}
			}
		}
		return 0;
	}


	//////////////////// AnsyncRecvPipe ////////////////////

	int32	AnsyncRecvPipe::create(const WCHAR* name)
	{
		name_= name;
		if(pipeCreate(name_.c_str() ) )
		{
			thread_.hEventHandle_= CreateEvent(0,TRUE,FALSE,0);
			thread_.hThreadHandle_= CreateThread(0, 0, AnsyncRecvPipe::recvCallback, reinterpret_cast<void*>(this), 0, NULL) ;

			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	int32	AnsyncRecvPipe::close()
	{
		::SetEvent(thread_.hEventHandle_ );
		if(WAIT_TIMEOUT== WaitForSingleObject(thread_.hThreadHandle_, 500))
		{
			::TerminateThread(thread_.hThreadHandle_, 0);
		}
		thread_.Release();

		disconnect();
		return pipeClose();
	}

	DWORD WINAPI	AnsyncRecvPipe::recvCallback(void* lpvoid)
	{
		AnsyncRecvPipe* _this= reinterpret_cast<AnsyncRecvPipe*>(lpvoid);
		do
		{
			// 等待管道的连接
			if(_this->accept() )
			{
				char* buffer= new char[kPipeMaxBufferSize];
				memset(buffer, 0, kPipeMaxBufferSize);
				while(WAIT_OBJECT_0!= WaitForSingleObject(_this->thread_.hEventHandle_, 10 ) )
				{
					bool bPipeClose = false;

					while(_this->read(buffer, kPipeMaxBufferSize) > 0)
					{
						pipe::Msg* pMsg = reinterpret_cast<pipe::Msg*>(buffer);
						DBG(INFO) << "PipeD::recvCallback -> Data" ;

						// Call callback function
						if (_this->m_callback)
							_this->m_callback(_this, pMsg);

						// Pipe close
						if (kPipeClose == pMsg->command)
						{
							bPipeClose = true;
							break;
						}

						memset(buffer, 0, kPipeMaxBufferSize);
					}

					// Pipe close
					if (bPipeClose)
					{
						_this->disconnect();
						break;
					}
				}
				delete buffer;
			}
		}while(WAIT_OBJECT_0!= WaitForSingleObject(_this->thread_.hEventHandle_, 10 ));
		return 0;
	}
}