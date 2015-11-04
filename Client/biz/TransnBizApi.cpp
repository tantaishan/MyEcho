#include "stdafx.h"
#include "TransnBizApi.h"

using namespace conv;

void AMQP_TASK_MICRO::ParseFromJson(const Json::Value& json, AMQP_TASK_MICRO& task)
{
	task.task_id = js2s(json, "task_id");
	task.lang_src_name = js2s(json, "lang_src_name");
	task.lagn_target_name = js2s(json, "lang_target_name");
	task.price = json["price"].asInt();
	const Json::Value& user_list = json["userList"];
	for (unsigned int i = 0; i < user_list.size(); ++i)
	{
		std::string id = user_list[i].asString();
		task.user_list.push_back(id);
	}
}

void AMQP_TASK_TRANS::ParseFromJson(const Json::Value& json, AMQP_TASK_TRANS& task)
{
	task.id = js2s(json, "id");
	task.source_language = js2i(json, "source_language");
	task.target_language = js2i(json, "target_language");
	task.from = js2s(json, "from");
	const Json::Value& userList = json["userList"];
	for (unsigned int i = 0; i < userList.size(); ++i)
	{
		std::string id = userList[i].asString();
		task.user_list.push_back(id);
	}
}

void AMQP_TRANS_END::ParseFromJson(const Json::Value& json, AMQP_TRANS_END& info)
{
	info.to = js2s(json, "to");
	info.id = js2s(json, "id");
	info.close_time = js2s(json, "close_time");
	info.close_userid = js2s(json, "close_userid");
	info.length = js2i(json, "length");
	info.create_time = js2s(json, "create_time");
	info.reply_time = js2s(json, "reply_time");
	info.from = js2s(json, "from");
}

bool TASK_INFO::ParseFromJson(const Json::Value& value, TASK_INFO& info)
{
	using namespace conv;
	info.result = js2i(value, "result");
	const Json::Value& data = value["data"];
	info.questionContent = js2ws(data, "question_content");
	info.priorityLevel = js2ws(data, "priority_level");
	info.remark = js2ws(data, "remark");
	info.taskId = js2ws(data, "task_id");
	info.modifiedByNickName = js2ws(data, "modified_by_nickname");
	info.creator = js2ws(data, "creator");
	info.targetLanguage = js2i(data, "target_language");
	info.modifiedBy = js2ws(data, "modified_by");
	info.description = js2ws(data, "description");
	info.taskSrc = js2i(data, "task_src");
	info.targetLanguageName = js2ws(data, "targe_language_name");
	info.customParams = js2ws(data, "custom_params");
	info.gold = js2i(data, "gold");
	info.sourceLanguage = js2i(data, "source_language");
	info.setId = js2ws(data, "set_id");
	info.bStamp = js2s(data, "stamp") == "false" ? FALSE : TRUE;
	info.bAbnormal = js2s(data, "is_abnormal") == "false" ? FALSE : TRUE;
	info.status = js2i(data, "status");
	info.modifiedTime = js2ws(data, "modified_time");
	info.taskType = (TASK_TYPE)js2i(data, "task_type");
	info.receiveTime = js2ws(data, "receive_time");
	info.bLike = js2s(data, "like") == "false" ? FALSE : TRUE;
	info.submitType = js2i(data, "submit_type");
	info.commentNum = js2i(data, "commentNum");
	info.submitContent = js2ws(data, "submit_content");
	info.stampNum = js2i(data, "stampNum");
	info.likeNum = js2i(data, "likeNum");
	info.createTime = js2ws(data, "create_time");
	info.sourceLanguageName = js2ws(data, "source_language_name");
	info.taskTitle = js2ws(data, "task_title");
	info.setNum = js2i(data, "set_num");
	info.currentTime = js2ws(data, "current_time");
	return true;
}


void TASK_INFO2::ParseFromJson(const Json::Value& json, TASK_INFO2& task)
{
	using namespace conv;
	task.result = js2i(json, "result");
	const Json::Value& data = json["data"];
	task.gold = js2i(data, "gold");
	task.source_language_name = js2s(data, "source_language_name");
	task.target_language_name = js2s(data, "target_language_name");
	task.shareInfo = js2s(data, "shareInfo");
	task.shareUrl = js2s(data, "shareUrl");
	task.task_id = js2s(data, "task_id");
	task.question = js2s(data, "question");
	task.is_paid = js2i(data, "is_paid") == 0 ? false : true;
	task.task_type = (TASK_TYPE)js2i(data, "task_type");
	task.picUrl = js2s(data, "picUrl");
	task.originalUrl = js2s(data, "originalUrl");
	task.mdpicUrl = js2s(data, "mdpicUrl");
	task.hdpicUrl = js2s(data, "hdpicUrl");
	task.mdpicSize = js2s(data, "mdpicSize");
	task.context = js2s(data, "context");
	task.countdown = js2i(data, "countdown");
}

void USER_DATA::ParseFromJson(const Json::Value& value, USER_DATA& data)
{
	using namespace conv;
	const Json::Value& params = value["response_params"];
	data.address = js2ws(params, "address");
	data.birthday = js2ws(params, "birthday");
	data.bloodType = js2ws(params, "bloodType");
	data.constellation = js2ws(params, "constellation");
	data.createTime = js2ws(params, "createTime");
	data.creator = js2ws(params, "creator");
	//data.m_strDocumentType = js2ws(params, "documentType");
	data.email = js2ws(params, "eMail");
	data.education = js2i(params, "education");
	data.firstLanguage = js2i(params, "firstLanguage");
	data.firstLanguageName = js2ws(params, "firstLanguageName");
	data.gender = js2i(params, "gender");
	data.headPhoto = js2ws(params, "headPhoto");
	data.id = js2ws(params, "id");
	data.idCrad = js2ws(params, "idCard");
	//data.m_arIndustries
	//data.m_arIndustrySkills
	//data.m_arInterpreters
	data.isOverSea = js2i(params, "isOversea");
	//data.m_arLanguageSkills
	data.location = js2ws(params, "location");
	data.loginName = js2ws(params, "loginName");
	//data.m_arManuscriptSkills
	data.mobilePhone = js2ws(params, "mobilePhone");
	data.motherTongue = js2i(params, "motherTongue");
	data.motherTongueName = js2ws(params, "motherTongueName");
	data.nickName = js2ws(params, "nickName");
	//data.m_arOralSkills
	data.qqId = js2ws(params, "qqId");
	data.remark = js2ws(params, "remark");
	data.rstSrc = js2i(params, "rstSrc");
	data.secondLanguage = js2i(params, "secondLanguage");
	data.secondLanguageName = js2ws(params, "secondLanguageName");
	data.specialty = js2ws(params, "specialty");
	data.status = js2i(params, "status");
	data.tplId = js2ws(params, "tplId");
	data.tplName = js2ws(params, "tplName");
	data.translationType = js2i(params, "translationType");
	data.trueName = js2ws(params, "trueName");
	data.updateTime = js2ws(params, "updateTime");
	data.updator = js2ws(params, "updator");
	data.userType = js2i(params, "userType");
	data.version = js2ws(params, "version");
	const Json::Value& rights = params["userRights"];
	for (unsigned int i = 0; i < rights.size(); ++i)
	{
		int nLangId = js2i(rights[i], "LANGUAGE");
		data.bsLangRights.set(nLangId);
		if (js2s(rights[i], "FUNCTION_ID") == "f005")
		{
			data.bInstantTrans = js2i(rights[i], "TURNON");
		}
	}
}

void USER_DATA::Clear()
{
	address.clear();
	birthday.clear();
	bloodType.clear();
	constellation.clear();
	createTime.clear();
	creator.clear();
	documentType.clear();
	email.clear();
	education = 0;
	firstLanguage = 0;
	firstLanguageName.clear();
	gender = 0;
	headPhoto.clear();
	id.clear();
	idCrad.clear();
	vtIndustries.clear();
	vtIndustrySkills.clear();
	vtInterpreters.clear();
	isOverSea = FALSE;
	vtLanguageSkills.clear();
	location.clear();
	loginName.clear();
	vtManuscriptSkills.clear();
	mobilePhone.clear();
	motherTongue = 0;
	motherTongueName.clear();
	nickName.clear();
	vtOralSkills.clear();
	qqId.clear();
	remark.clear();
	rstSrc = 0;
	secondLanguage = 0;
	secondLanguageName.clear();
	specialty.clear();
	status = 0;
	tplId.clear();
	tplName.clear();
	translationType = 0;
	trueName.clear();
	updateTime.clear();
	updator.clear();
	userType = 0;
	version.clear();
	bsLangRights.reset();
	bInstantTrans = FALSE;
}


void USER_DATA2::ParseFromJson(const Json::Value& json, USER_DATA2& userData)
{
	using namespace conv;
	userData.result = js2i(json, "result");
	if (userData.result == 0)
	{
		userData.code = js2i(json, "code");
		userData.msg = js2s(json, "msg");
	}
	else
	{
		const Json::Value& jsnData = json["data"];
		DATA& data = userData.data;
		data.userId = js2s(jsnData, "userId");
		data.nickName = js2s(jsnData, "nickName");
		data.headPhoto = js2s(jsnData, "headPhoto");
		data.user_task_num = js2i(jsnData, "user_task_num");
		data.email = js2s(jsnData, "eMail");
		data.mobilePhone = js2s(jsnData, "mobilePhone");
		data.isOversea = js2i(jsnData, "isOversea") == 0 ? false : true;
		data.translationType = js2i(jsnData, "translationType");
		data.user_collects = js2s(jsnData, "user_collects");
		data.user_rankings = js2i(jsnData, "user_rankings");
		data.user_title = js2s(jsnData, "user_title");
		data.send_switch = js2i(jsnData, "send_switch");

		const Json::Value& languages = jsnData["languages"];
		for (unsigned int i = 0; i < languages.size(); i++)
		{
			LANG_INFO lang;
			lang.langId = js2i(languages[i], "langId");
			lang.langName = js2s(languages[i], "langName");
			lang.right = js2s(languages[i], "right") == "true" ? true : false;
			data.languages.push_back(lang);
		}

		data.tag_help = js2s(jsnData, "tag_help") == "true" ? true : false;
		data.tag_we = js2s(jsnData, "tag_we") == "true" ? true : false;
	}
}

void UNFINISH_TASK::ParseFromJson(const Json::Value& json, UNFINISH_TASK& task)
{
	using namespace conv;
	task.result = js2i(json, "result");
	const Json::Value& micro = json["data"]["unfinish_microtask"];
	if (!micro.empty())
	{
		task.gold = js2i(micro, "gold");
		task.source_language_name = js2s(micro, "source_language_name");
		task.target_language_name = js2s(micro, "target_language_name");
		task.shareInfo = js2s(micro, "shareInfo");
		task.shareUrl = js2s(micro, "shareUrl");
		task.task_id = js2s(micro, "task_id");
		task.question = js2s(micro, "question");
		task.is_paid = js2i(micro, "is_paid") == 0 ? false : true;
		task.task_type = (TASK_TYPE)js2i(micro, "task_type");
		task.originalUrl = js2s(micro, "originalUrl");
		task.picUrl = js2s(micro, "picUrl");
		task.mdpicUrl = js2s(micro, "mdpicUrl");
		task.hdpicUrl = js2s(micro, "hdpicUrl");
		task.mdpicSize = js2s(micro, "mdpicSize");
		task.context = js2s(micro, "context");
		task.countdown = js2i(micro, "countdown");
		task.is_trans = false;
	}

	const Json::Value& conversation = json["data"]["unfinish_conversation"];
	if (!conversation.empty())
	{
		task.conversation_id = js2s(conversation, "conversation_id");
		task.customer_id = js2s(conversation, "customer_id");
		task.duration = js2i(conversation, "duration");
		task.is_trans = true;
	}
	
}


void TransnBizApi::InitCurl(curlcpp* curl, std::string url)
{
	curl->set_url(url);
	curl->enable_error();
	curl->reset_headers();
	curl->add_header(CURL_CONFIG::DEFAULT_USERAGENT);
	curl->add_header("Accept: text/html, application/xml;q=0.9, application/xhtml+xml, */*");
	curl->add_header("Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7");
	curl->add_header("Accept-Language: zh-cn,zh;q=0.5");
	curl->add_header("Content-Type: application/x-www-form-urlencoded;charset=UTF-8");
	curl->add_header("Connection: Keep-Alive");
}

void TransnBizApi::InitCurl_Json(curlcpp* curl, std::string url)
{
	curl->set_url(url);
	curl->enable_error();
	curl->reset_headers();
	curl->add_header(CURL_CONFIG::DEFAULT_USERAGENT);
	curl->add_header("Accept: text/html, application/xml;q=0.9, application/xhtml+xml, */*");
	curl->add_header("Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7");
	curl->add_header("Accept-Language: zh-cn,zh;q=0.5");
	curl->add_header("Content-Type: application/json;charset=UTF-8");
	curl->add_header("Connection: Keep-Alive");
}


ERR_CODE TransnBizApi::Login(const std::string& user, const std::string& pwd, Json::Value& jsnRet)
{
	Json::Value jsnData;
	jsnRet.clear();
	//Json::Value jsnRes;
	jsnData["login_name"] = user;
	jsnData["login_pwd"] = pwd;
	jsnData["login_type"] = i2s(0);
	jsnData["rst_src"] = i2s(1);
	std::string url(BIZ_URL::LOGIN);

	ERR_CODE nErrCode = ERR_SUCCESS;

	curlcpp curl(false);
	//curl.set_url
	InitCurl(&curl, url);
	//curl.set_post_data(jsnData.toStyledString());
	std::string response;
	if (curl.post(std::string("data=") + jsnData.toStyledString(), response) == 0)
	{
		std::string str = base::stdcxx_utf8ts(response);
		Json::Reader reader;
		try
		{
			if (!reader.parse(str, jsnRet))
				throw std::runtime_error("json parse failed");
			if (!jsnRet["error_code"].isNull())
			{
				nErrCode = (ERR_CODE)atoi(jsnRet["error_code"].asString().c_str());
			}
		}
		catch (...)
		{
			nErrCode = ERR_PARSEERR;
		}
	}
	else
	{ 
		nErrCode = ERR_NETWORKERR;
	}

	return nErrCode;
}

ERR_CODE TransnBizApi::Login2(const LOGIN_DATA& loginData, USER_DATA2& userData)
{
	Json::Value param, jsnRet;
	ERR_CODE nErrCode = ERR_SUCCESS;
	switch (loginData.loginType)
	{
	case LT_PHONE:
		param["loginType"] = "phone";
		break;
	case LT_EMAIL:
		param["loginType"] = "email";
		break;
	case LT_QQ:
		param["loginType"] = "qq";
		break;
	case LT_WEIBO:
		param["loginType"] = "weibo";
		break;
	default:
		break;
	}
	param["loginName"] = loginData.loginName;
	param["password"] = loginData.password;
	param["thirdPartId"] = loginData.thirdPartId;
	param["thirdNickname"] = loginData.thirdNickname;
	param["photo"] = loginData.pthoto;

	curlcpp curl(false);
	//curl.set_url
	InitCurl_Json(&curl, biz_url_login);
	//curl.set_post_data(jsnData.toStyledString());
	std::string response;
	if (curl.post(param.toStyledString(), response) == 0)
	{
		std::string str = base::stdcxx_utf8ts(response);
		Json::Reader reader;
		if (reader.parse(str, jsnRet)) {
			USER_DATA2::ParseFromJson(jsnRet, userData);
			if (userData.result == 0) {
				nErrCode = (ERR_CODE)userData.code;
			}
		}
		else {
			nErrCode = ERR_PARSEERR;
		}
	} else {
		nErrCode = ERR_NETWORKERR;
	}

	return nErrCode;

}

ERR_CODE TransnBizApi::ModifyLang(const std::string& id, int nFirst, int nSecond)
{
	Json::Value jsnData, jsnRet;
	//Json::Value jsnRes;
	jsnData["user_id"] = id;
	jsnData["first_language"] = i2s(nFirst);
	jsnData["second_language"] = i2s(nSecond);

	std::string url(BIZ_URL::MODIFY);

	ERR_CODE nErrCode = ERR_SUCCESS;

	curlcpp curl(false);
	//curl.set_url
	InitCurl(&curl, url);
	//curl.set_post_data(jsnData.toStyledString());
	std::string response;
	if (curl.post(std::string("data=") + jsnData.toStyledString(), response) == 0)
	{
		std::string str = base::stdcxx_utf8ts(response);
		Json::Reader reader;
		try
		{
			if (!reader.parse(str, jsnRet))
				throw std::runtime_error("json parse failed");
			if (!jsnRet["error_code"].isNull())
			{
				nErrCode = (ERR_CODE)atoi(jsnRet["error_code"].asString().c_str());
			}
		}
		catch (...)
		{
			nErrCode = ERR_PARSEERR;
		}
	}
	else
	{
		nErrCode = ERR_NETWORKERR;
	}

	return nErrCode;
}

ERR_CODE TransnBizApi::QueryTask(const std::string& id, Json::Value& ret)
{
	Json::Value param;
	//Json::Value jsnRes;
	param["task_id"] = id;
	param["type"] = "img";

	std::string url(BIZ_URL::QUERY_TASK);

	ERR_CODE nErrCode = ERR_SUCCESS;

	curlcpp curl(false);
	//curl.set_url
	InitCurl_Json(&curl, url);
	//curl.set_post_data(jsnData.toStyledString());
	std::string response;
	if (curl.post(param.toStyledString(), response) == 0)
	{
		std::string str = base::stdcxx_utf8ts(response);
		Json::Reader reader;
		try
		{
			if (!reader.parse(str, ret))
				throw std::runtime_error("json parse failed");
			if (!ret["error_code"].isNull())
			{
				nErrCode = (ERR_CODE)atoi(ret["error_code"].asString().c_str());
			}
		}
		catch (...)
		{
			nErrCode = ERR_PARSEERR;
		}
	}
	else
	{
		nErrCode = ERR_NETWORKERR;
	}

	return nErrCode;
}

ERR_CODE TransnBizApi::FeedbackTask(const TASK_FEEDBACK& feedback, OT_RESULT& result)
{
	Json::Value param;
	param["userId"] = feedback.userId;
	param["task_id"] = feedback.task_id;
	param["code"] = conv::i2s(feedback.code);

	std::string url(biz_url_feedback);

	ERR_CODE nErrCode = ERR_SUCCESS;

	curlcpp curl(false);
	//curl.set_url
	InitCurl_Json(&curl, url);
	//curl.set_post_data(jsnData.toStyledString());
	std::string response;
	if (curl.post(param.toStyledString(), response) == 0)
	{
		std::string str = base::stdcxx_utf8ts(response);
		Json::Reader reader;
		Json::Value ret;
		if (reader.parse(str, ret))
		{
			result.result = js2i(ret, "result");
			result.task_id = js2s(ret["data"], "task_id");
			result.msg = js2s(ret, "msg");
			result.code = js2i(ret, "code");
		}
		else
		{
			nErrCode = ERR_PARSEERR;
		}
	}
	else
	{
		nErrCode = ERR_NETWORKERR;
	}

	return nErrCode;
}

ERR_CODE TransnBizApi::CheckUpdate(UPDATE_INFO& update)
{
	return ERR_SUCCESS;
}

ERR_CODE TransnBizApi::OperateTask(const TASK_OPERATE& op, Json::Value& ret)
{
	Json::Value param;
	param["user_id"] = op.user_id;
	param["task_id"] = op.task_id;
	switch (op.op_type)
	{
	case OT_RECEIVE:
		param["operateType"] = "RECEIVE";
		break;
	case OT_COMPLETE:
	{
		param["operateType"] = "COMPLETE";
		Json::Value manuscript;
		manuscript["type"] = i2s(op.manuscript.type);
		manuscript["file_type"] = i2s(op.manuscript.file_type);
		manuscript["content"] = op.manuscript.content;
		break;
	}
	case OT_GIVEUP:
		param["operateType"] = "GIVEUP";
		break;
	case OT_FEEDBACK:
		param["operateType"] = "OVERRUN";
		param["task_feedback"] = op.task_feedback;
		break;
	default:
		break;
	}

	std::string url(BIZ_URL::OPERATE_TASK);

	ERR_CODE nErrCode = ERR_SUCCESS;

	curlcpp curl(false);
	//curl.set_url
	InitCurl_Json(&curl, url);
	//curl.set_post_data(jsnData.toStyledString());
	std::string response;
	if (curl.post(param.toStyledString(), response) == 0)
	{
		std::string str = base::stdcxx_utf8ts(response);
		Json::Reader reader;
		try
		{
			if (!reader.parse(str, ret))
				throw std::runtime_error("json parse failed");
			if (!ret["error_code"].isNull())
			{
				nErrCode = (ERR_CODE)atoi(ret["error_code"].asString().c_str());
			}
		}
		catch (...)
		{
			nErrCode = ERR_PARSEERR;
		}
	}
	else
	{
		nErrCode = ERR_NETWORKERR;
	}

	return nErrCode;
	
}

ERR_CODE TransnBizApi::GetUnfinishTask(const std::string& userId, TASK_INFO& task)
{
	Json::Value param;
	//Json::Value jsnRes;
	param["user_id"] = userId;

	std::string url(BIZ_URL::QUERY_TASK);

	ERR_CODE nErrCode = ERR_SUCCESS;

	curlcpp curl(false);
	//curl.set_url
	InitCurl_Json(&curl, url);
	//curl.set_post_data(jsnData.toStyledString());
	std::string response;
	Json::Value ret;
	if (curl.post(param.toStyledString(), response) == 0)
	{
		std::string str = base::stdcxx_utf8ts(response);
		Json::Reader reader;
		try
		{
			if (!reader.parse(str, ret))
				throw std::runtime_error("json parse failed");
			if (!ret["error_code"].isNull())
			{
				nErrCode = (ERR_CODE)atoi(ret["error_code"].asString().c_str());
			}
		}
		catch (...)
		{
			nErrCode = ERR_PARSEERR;
		}
	}
	else
	{
		nErrCode = ERR_NETWORKERR;
	}

	return nErrCode;
}

ERR_CODE TransnBizApi::OperateTask2(const TASK_OPERATE2& op, OT_RESULT& result)
{
	Json::Value param;
	param["userId"] = op.userId;
	param["task_id"] = op.taskId;
	std::string url(biz_url_helptask);
	switch (op.type)
	{
	case OT_RECEIVE:
		url.append("receiveMicroTask/");
		break;
	case OT_COMPLETE:
	{
		url.append("completeMicroTask/");
		param["answer"] = op.answer;
		break;
	}
		
	case OT_GIVEUP:
		url.append("giveupMicroTask/");
		break;
	default:
		break;
	}

	ERR_CODE nErrCode = ERR_SUCCESS;
	curlcpp curl(false);
	//curl.set_url
	InitCurl_Json(&curl, url);
	//curl.set_post_data(jsnData.toStyledString());
	std::string response;
	Json::Value jsnRet;
	if (curl.post(param.toStyledString(), response) == 0)
	{
		std::string str = base::stdcxx_utf8ts(response);
		Json::Reader reader;
		
		if (reader.parse(str, jsnRet))
		{
			result.result = js2i(jsnRet, "result");
			result.task_id = js2s(jsnRet["data"], "task_id");
			result.msg = js2s(jsnRet, "msg");
			result.code = js2i(jsnRet, "code");
		}
		else
		{
			nErrCode = ERR_PARSEERR;
		}
	}
	else
	{
		nErrCode = ERR_NETWORKERR;
	}

	return nErrCode;

}

ERR_CODE TransnBizApi::QueryTask2(const std::string taskid, TASK_INFO2& task)
{
	Json::Value param;
	//Json::Value jsnRes;
	param["task_id"] = taskid;

	std::string url(biz_url_query_task);

	ERR_CODE nErrCode = ERR_SUCCESS;
	curlcpp curl(false);
	//curl.set_url
	InitCurl_Json(&curl, url);
	//curl.set_post_data(jsnData.toStyledString());
	std::string response;
	if (curl.post(param.toStyledString(), response) == 0)
	{
		std::string str = base::stdcxx_utf8ts(response);
		Json::Reader reader;
		Json::Value ret;
		if (reader.parse(str, ret))
		{
			TASK_INFO2::ParseFromJson(ret, task);
		}
		else
		{
			nErrCode = ERR_PARSEERR;
		}
	}
	else
	{
		nErrCode = ERR_NETWORKERR;
	}

	return nErrCode;
}

ERR_CODE TransnBizApi::GetUnfinishTask2(const std::string& userId, UNFINISH_TASK& task)
{
	Json::Value param;
	//Json::Value jsnRes;
	param["userId"] = userId;

	std::string url(biz_url_unfinish_task);

	ERR_CODE nErrCode = ERR_SUCCESS;
	curlcpp curl(false);
	//curl.set_url
	InitCurl_Json(&curl, url);
	//curl.set_post_data(jsnData.toStyledString());
	std::string response;
	if (curl.post(param.toStyledString(), response) == 0)
	{
		std::string str = base::stdcxx_utf8ts(response);
		Json::Reader reader;
		Json::Value ret;
		if (reader.parse(str, ret))
		{
			UNFINISH_TASK::ParseFromJson(ret, task);
		}
		else
		{
			nErrCode = ERR_PARSEERR;
		}
	}
	else
	{
		nErrCode = ERR_NETWORKERR;
	}

	return nErrCode;
}

ERR_CODE TransnBizApi::OperateTrans(const TRANS_OPERATE& op, TRANS_RESULT& result)
{
	Json::Value param;
	param["userId"] = op.userId;
	std::string url(biz_url_helptask);
	switch (op.type)
	{
	case TOT_RECEIVE:
		url.append("receiveConversationTranslation/");
		param["nick_name"] = op.nick_name;
		break;
	case TOT_CLOSE:
	{
		url.append("closeConversationTranslation/");
		param["conversation_id"] = op.conversation_id;
		break;
	}
	default:
		break;
	}

	ERR_CODE nErrCode = ERR_SUCCESS;
	curlcpp curl(false);
	//curl.set_url
	InitCurl_Json(&curl, url);
	//curl.set_post_data(jsnData.toStyledString());
	std::string response;
	Json::Value jsnRet;
	if (curl.post(param.toStyledString(), response) == 0)
	{
		std::string str = base::stdcxx_utf8ts(response);
		Json::Reader reader;

		if (reader.parse(str, jsnRet))
		{
			result.result = js2i(jsnRet, "result");
			if (result.result == 0)
			{
				result.err_msg = js2s(jsnRet, "msg");
			}
			else
			{
				Json::Value& data = jsnRet["data"];
				if (op.type == TOT_RECEIVE)
				{
					result.conversation_id = js2s(data, "conversation_id");
					result.reply_time = js2s(data, "reply_time");
					result.customer_id = js2s(data, "customer_id");
					result.first_sentence = js2s(data, "first_sentence");
				}
				else if (op.type == TOT_CLOSE)
				{
					result.conversation_id = js2s(data, "conversation_id");
					result.finishMsg = js2s(data, "finishMsg");
				}
				else
				{

				}
			}
		}
		else
		{
			nErrCode = ERR_PARSEERR;
		}
	}
	else
	{
		nErrCode = ERR_NETWORKERR;
	}

	return nErrCode;
}

ERR_CODE TransnBizApi::GetFeedbackItems(const TASK_TYPE& type, std::vector<FEEDBACK_ITEM>& items)
{
	Json::Value param;
	//Json::Value jsnRes;
	param["task_type"] = (int)type;

	std::string url(biz_url_get_feedback_item);

	ERR_CODE nErrCode = ERR_SUCCESS;
	curlcpp curl(false);
	//curl.set_url
	InitCurl_Json(&curl, url);
	//curl.set_post_data(jsnData.toStyledString());
	std::string response;
	if (curl.post(param.toStyledString(), response) == 0)
	{
		std::string str = base::stdcxx_utf8ts(response);
		Json::Reader reader;
		Json::Value ret;
		if (reader.parse(str, ret))
		{
			Json::Value& data = ret["data"];
			for (unsigned int i = 0; i < data.size(); i++)
			{
				FEEDBACK_ITEM item;
				item.code = conv::js2i(data[i], "code");
				item.msg = conv::js2s(data[i], "msg");
				items.push_back(item);
			}
		}
		else
		{
			nErrCode = ERR_PARSEERR;
		}
	}
	else
	{
		nErrCode = ERR_NETWORKERR;
	}

	return nErrCode;
}