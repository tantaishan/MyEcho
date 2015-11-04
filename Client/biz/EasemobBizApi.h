#pragma once

#include "../../Core/conv/conv.h"
#include "../Core/curl/curlcpp.h"

namespace Easemob
{
	enum
	{
		ERR_SUCCESS = 200,
		ERR_INVALID_GTANT = 400,
		ERR_UNAUTHORIZED = 401,
		ERR_SERVER_REJECTED = 403,
		ERR_NOT_FOUND = 404,
		ERR_REQUEST_ENTITY_TOO_LARGE = 413,
		ERR_INTERNAL_SERVER_ERROR = 500,
		ERR_NOT_IMPLEMENTED = 501,
		ERR_BAD_GATEWAY = 502,
		ERR_SERVICE_UNAVAILABLE = 503,
		ERR_GATEWAY_TIMEOUT = 504,
	};
	enum
	{
		MESSAGE_TYPE_TEXT,
		MESSAGE_TYPE_IMG,
		MESSAGE_TYPE_AUDIO,
		MESSAGE_TYPE_VIDEO,
	};

	typedef struct _TOKEN
	{
		std::string		access_token;
		int				expires_in;
		std::string		application;
	} TOKEN;

	typedef struct _UPLOAD_FILE
	{
		bool			restrict_access;
		std::string		authorization;
		std::string		file_path;
	} UPLOAD_FILE;

	typedef struct _ENTITY
	{
		std::string		uuid;
		std::string		type;
		std::string		share_secret;
	} ENTITY;

	typedef struct _UPLOAD_RESULT
	{
		std::string			action;
		std::string			application;
		std::string			path;
		std::string			uri;
		__int64				timestamp;
		int					duration;
		std::string			organization;
		std::string			applicationName;
		std::vector<ENTITY> entities;

	} UPLOAD_RESULT;

	// 聊天记录
	typedef struct _CHAT_RECORD
	{
		std::string		type;
		std::string		from;
		std::string		msg_id;
		std::string		chat_type;

		struct BODIES
		{
			std::string		msg;
			std::string		type;
			int				length;
			std::string		url;
			std::string		filename;
			std::string		secret;
			double			lat;
			double			lng;
			std::string		addr;
		};

		std::string		timestamp;
		std::string		to;
		BODIES			bodies;
	} CHAT_RECORD;


	// 文本消息
	typedef struct _TEXT_MESSAGE
	{
		std::string		from;
		std::string		to;
		std::string		msg;
	} TEXT_MESSAGE;

	// 图片类型消息
	typedef struct _IMG_MESSAGE
	{
		std::string		from;
		std::string		to;

		struct BODIES
		{
			std::string		type;
			std::string		url;
			std::string		filename;
			std::string		tumb;
			std::string		secret;
			std::string		thumb_secret;
		};
		BODIES bodies;
	} IMG_MESSAGE;
	
	// 语音消息
	typedef struct _AUDIO_MESSAGE
	{
		std::string		from;
		std::vector<std::string> to;
		struct BODIES
		{
			std::string		type;
			std::string		url;
			std::string		filename;
			int				length;
			std::string		secret;
		};
		BODIES				bodies;
	} AUDIO_MESSAGE;

	// 地址位置消息
	typedef struct _LOC_MESSAGE
	{
		std::string		from;
		std::string		to;
		struct BODIES
		{
			std::string		type;
			std::string		addr;
			double			lat;
			double			lng;
		};
		BODIES			bodies;
	} LOC_MESSAGE;

	// 视频消息
	typedef struct _VIDEO_MESSAGE
	{
	public:
		std::string					from;
		std::vector<std::string>	to;
		struct BODIES
		{
			std::string		type;
			std::string		filename;
			std::string		tumb;
			std::string		length;
			std::string		secret;
			std::string		file_length;
			std::string		thumb_secret;
			std::string		url;
		};
		BODIES			bodies;
	} VIDEO_MESSAGE;

	
	int GetToken(TOKEN& token);
	int UploadFile(const UPLOAD_FILE& upload, UPLOAD_RESULT& result);
	int SendTextMsg(std::string token, TEXT_MESSAGE msg);
	int SendImgMsg(std::string token, IMG_MESSAGE msg);
	int SendAudioMsg(const TOKEN& token, AUDIO_MESSAGE msg);
	int SendLocMsg(std::string token, LOC_MESSAGE msg);
	int SendVideoMsg(std::string token, VIDEO_MESSAGE msg);

	void InitCurl(curlcpp* curl, const std::string url);
	void InitCurl_Json(curlcpp* curl, std::string url);

}

