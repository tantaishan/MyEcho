#include "stdafx.h"
#include "EasemobBizApi.h"

namespace Easemob
{
	namespace URL
	{
		static const std::string API_SERVER_HOST = "https://a1.easemob.com";
		static const std::string GET_TOKEN = "http://a1.easemob.com/te-8/te/token";
		static const std::string CHAT_FILES = "http://a1.easemob.com/te-8/te/chatfiles";
		static const std::string MESSAGES = "http://a1.easemob.com/te-8/te/messages";
	}

	static const std::string USER_AGENT = "User - Agent: Transn MyEcho Client / 1.0";
	static const std::string APPKEY = "te-8#te";
	static const std::string APP_CLIENT_ID = "YXA6Zun3IH6_EeSYevMjxar8iA";
	static const std::string APP_CLIENT_SECRET = "YXA6wqGiWULsR-H53dYkI89ogcs7MzY";

	std::string ReadFileContent(std::string filepath)
	{
		std::string result;
		FILE* pf = 0;
		char* buffer = 0;
		int size = 0;
		do
		{
			fopen_s(&pf, filepath.c_str(), "rb");
			if (!pf)
				return result;
			fseek(pf, 0, SEEK_END);
			size = ftell(pf);
			fseek(pf, 0, SEEK_SET);
			buffer = new char[size];
			size_t r = fread(buffer, 1, size, pf);
			if (size != r)
			{
				delete[] buffer;
				buffer = 0;
				return result;
			}
		} while (false);
		if (pf)
		{
			fclose(pf);
			pf = 0;
		}
		if (buffer)
		{
			result = std::string(buffer, size);
			delete[] buffer;
			buffer = 0;
		}

		return result;
	}

	void InitCurl(curlcpp* curl, const std::string url)
	{
		curl->set_url(url);
		curl->enable_error();
		curl->reset_headers();
		curl->add_header(USER_AGENT);
		curl->add_header("Accept: text/html, application/xml;q=0.9, application/xhtml+xml, */*");
		curl->add_header("Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7");
		curl->add_header("Accept-Language: zh-cn,zh;q=0.5");
		curl->add_header("Content-Type: application/x-www-form-urlencoded;charset=UTF-8");
		curl->add_header("Connection: Keep-Alive");
	}

	void InitCurl_Json(curlcpp* curl, std::string url)
	{
		curl->set_url(url);
		curl->enable_error();
		curl->reset_headers();
		curl->add_header(USER_AGENT);
		curl->add_header("Accept: text/html, application/xml;q=0.9, application/xhtml+xml, */*");
		curl->add_header("Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7");
		curl->add_header("Accept-Language: zh-cn,zh;q=0.5");
		curl->add_header("Content-Type: application/json;charset=UTF-8");
		curl->add_header("Connection: Keep-Alive");
	}
	
	int GetToken(TOKEN& token)
	{
		Json::Value body;
		body["grant_type"] = "client_credentials";
		body["client_id"] = APP_CLIENT_ID;
		body["client_secret"] = APP_CLIENT_SECRET;

		curlcpp curl(false);
		//curl.set_url
		InitCurl_Json(&curl, URL::GET_TOKEN);
		//curl.set_post_data(jsnData.toStyledString());
		std::string response;
		int nRet = curl.post(body.toStyledString(), response);
		if (nRet == 0)
		{
			Json::Reader reader;
			Json::Value result;
			if (reader.parse(response, result))
			{
				token.access_token = result["access_token"].asString();
				token.application = result["application"].asString();
				token.expires_in = result["expires_in"].asInt();
			}
		}
		return nRet;
	}

	int UploadFile(const UPLOAD_FILE& upload, UPLOAD_RESULT& result)
	{
		int nRet = 0;
		//Json::Value request;
		//request["restrict-access"] = upload.restrict_access;
		std::string authorization("Bearer ");
		authorization.append(upload.authorization);
		//request["Authorization"] = authorization;

		curlcpp curl(false);

		//InitCurl(&curl, URL::CHAT_FILES);
		curl.set_url(URL::CHAT_FILES);
		curl.enable_error();
		curl.reset_headers();
		curl.add_header("Content-Type: multipart/form-data");
		curl.add_header("restrict-access", upload.restrict_access ? "true" : "false");
		curl.add_header("Authorization", authorization);
		std::string request = ReadFileContent(upload.file_path);
		std::string response;
		nRet = curl.post_multipart(request, response);
		if (nRet == 0)
		{
			Json::Value json;
			Json::Reader reader;
			if (reader.parse(response, json))
			{
				result.action = json["action"].asString();
				result.application = json["application"].asString();
				result.path = json["path"].asString();
				result.uri = json["uri"].asString();

				Json::Value& entities = json["entities"];
				for (unsigned int i = 0; i < entities.size(); i++)
				{
					ENTITY entity;
					entity.uuid = entities[i]["uuid"].asString();
					entity.type = entities[i]["type"].asString();
					entity.share_secret = entities[i]["share-secret"].asString();
					result.entities.push_back(entity);
				}

				result.timestamp = json["timestamp"].asUInt64();
				result.duration = json["duration"].asInt();
				result.organization = json["organization"].asString();
				result.applicationName = json["applicationName"].asString();
			}
		}
		return nRet;
	}

	int SendTextMsg(std::string token, TEXT_MESSAGE msg)
	{
		return ERR_SUCCESS;
	}

	int SendImgMsg(std::string token, IMG_MESSAGE msg)
	{
		return ERR_SUCCESS;
	}

	
	int SendLocMsg(std::string token, LOC_MESSAGE msg)
	{
		return ERR_SUCCESS;
	}

	int SendAudioMsg(const TOKEN& token, AUDIO_MESSAGE msg)
	{
		int nRet = 0;
		Json::Value request;
		request["target_type"] = "users";
		Json::Value users;
		for (unsigned int i = 0; i < msg.to.size(); i++)
			users.append(msg.to[i]);
		request["target"] = users;
		Json::Value audio_msg;
		audio_msg["type"] = "audio";

		std::string msg_url(URL::CHAT_FILES);
		msg_url.append("/");
		msg_url.append(msg.bodies.url);

		audio_msg["url"] = msg_url;

		audio_msg["filename"] = msg.bodies.filename;
		audio_msg["length"] = msg.bodies.length;
		audio_msg["secret"] = msg.bodies.secret;

		request["msg"] = audio_msg;
		request["from"] = msg.from;

		curlcpp curl(false);
		//curl.set_url
		InitCurl_Json(&curl, URL::MESSAGES);

		std::string authorization("Bearer ");
		authorization.append(token.access_token);
		curl.add_header("Authorization", authorization);

		//curl.set_post_data(jsnData.toStyledString());
		std::string response;
		nRet = curl.post(request.toStyledString(), response);
		if (nRet == 0)
		{
			Json::Reader reader;
			Json::Value result;
			if (reader.parse(response, result))
			{

			}
		}
		return nRet;
	}

	int SendVideoMsg(std::string token, VIDEO_MESSAGE msg)
	{
		return ERR_SUCCESS;
	}

}