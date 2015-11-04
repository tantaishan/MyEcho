#pragma once

#include <bitset>
#include "../../Core/conv/conv.h"
#include "../Core/curl/curlcpp.h"
#include "../Core/json/json.h"

#include "../config.h"

//////////////////////////////////////////////////////////////////////////
// AMQP data structure
class AMQP_TASK
{
public:
	std::vector<std::string>	user_list;
};

class AMQP_TASK_MICRO : public AMQP_TASK
{
public:
	static void ParseFromJson(const Json::Value& json, AMQP_TASK_MICRO& task);
public:
	std::string					task_id;
	std::string					lang_src_name;
	std::string					lagn_target_name;
	double						price;

};

class AMQP_TASK_TRANS : public AMQP_TASK
{
public:
	static void ParseFromJson(const Json::Value& json, AMQP_TASK_TRANS& task);

public:
	std::string					id;
	std::string					from;
	int							source_language;
	int							target_language;
};

class AMQP_TRANS_END
{
public:
	static void ParseFromJson(const Json::Value& json, AMQP_TRANS_END& info);

public:
	std::string			to;
	std::string			id;
	std::string			close_time;
	std::string			close_userid;
	int					length;
	std::string			create_time;
	std::string			reply_time;
	std::string			from;
};

//////////////////////////////////////////////////////////////////////////
// Transn biz Urls
namespace BIZ_URL
{
	static const std::string LOGIN = "http://113.57.161.140:8380/IOL_CDSP/usercenter/user/loginME4/";
	//static const std::string kLogin = kBase + "login/";
	static const std::string MODIFY = /*kBase +*/ "modify/";
	static const std::string QUERY_TASK = "http://113.57.161.140:8380/IOL_MT_SERVICE/mctask/getMicorTaskInfo/";
	static const std::string OPERATE_TASK = "http://113.57.161.140:8380/IOL_MT_SERVICE/mctask/operateTask/";
}

namespace BIZ_URL2
{

}


//////////////////////////////////////////////////////////////////////////
// CURL config
namespace CURL_CONFIG
{
	static const std::string DEFAULT_USERAGENT = "User - Agent: Transn MyEcho Client / 1.0";
}

//////////////////////////////////////////////////////////////////////////
// Update
class UPDATE_INFO
{
public:
	std::wstring version;
	std::wstring link;
	std::wstring description;
};

//////////////////////////////////////////////////////////////////////////
// typedef enum

typedef enum _OPERATE_TYPE
{
	OT_RECEIVE = 0,
	OT_GIVEUP,
	OT_COMPLETE,
	OT_FEEDBACK,
} OPERATE_TYPE;

typedef enum _TO_TYPE
{
	TOT_RECEIVE = 0,
	TOT_CLOSE,
} TO_TYPE;

typedef enum _LOGIN_TYPE
{
	LT_PHONE = 0,
	LT_EMAIL,
	LT_QQ,
	LT_WEIBO,

} LOGIN_TYPE;

typedef enum _ERR_CODE
{
	ERR_SUCCESS		= 0,	//	成功
	ERR_PARSEERR	= 3001,	//	解析错误
	ERR_INVALID		= 3002,	//	用户名或密码错误
	ERR_NOTEXIST	= 3003,	//	用户不存在
	ERR_NETWORKERR	= 4000,	//	网络错误
} ERR_CODE;

typedef enum _LAN_CODE
{
	LAN_CHINESE_SIMPLIFIED = 1,
	LAN_ENGLISH = 2,
	LAN_JAPANESE = 3,
	LAN_FRENCH = 4,
	LAN_GERMAN = 5,
	LAN_RUSSIAN = 6,
	LAN_KOREAN = 7,
	LAN_CHINESE_TRADITIONAL = 8,
	LAN_DUTCH = 9,
	LAN_ITALIAN = 10,
	LAN_SPANISH = 11,
	LAN_PORTUGUESE = 12,
	LAN_ARABIC = 13,
	LAN_TURKISH = 14,
} LAN_CODE;

//////////////////////////////////////////////////////////////////////////
//typdef data structure


class TASK_INFO
{
public:
	static bool ParseFromJson(const Json::Value& value, TASK_INFO& info);
public:
	enum TASK_TYPE
	{
		TASK_TYPE_IMG = 1,
		TASK_TYPE_TEXT,
		TASK_TYPE_VOICE,
	};

	int					result;
	std::wstring		questionContent;
	std::wstring		priorityLevel;
	std::wstring		remark;
	std::wstring		taskId;
	std::wstring		modifiedByNickName;
	std::wstring		creator;
	int					targetLanguage;
	std::wstring		modifiedBy;
	std::wstring		description;
	int					taskSrc;
	std::wstring		targetLanguageName;
	std::wstring		customParams;
	int					gold;
	int					sourceLanguage;
	std::wstring		setId;
	BOOL				bStamp;
	BOOL				bAbnormal;
	int					status;
	std::wstring		modifiedTime;
	TASK_TYPE			taskType;
	std::wstring		receiveTime;
	BOOL				bLike;
	int					submitType;
	int					commentNum;
	std::wstring		submitContent;
	int					stampNum;
	int					likeNum;
	std::wstring		createTime;
	std::wstring		sourceLanguageName;
	std::wstring		taskTitle;
	int					setNum;
	std::wstring		currentTime;
};

typedef enum _TASK_TYPE
{
	TT_IMG = 1,
	TT_TEXT,
	TT_VOICE,
} TASK_TYPE;

class TASK_INFO2
{
public:
	TASK_INFO2() : result(0), gold(0), is_paid(false),
		countdown(0), is_unfinish(false)
	{}

	static void ParseFromJson(const Json::Value& json, TASK_INFO2& task);

	int				result;
	int				gold;
	std::string		source_language_name;
	std::string		target_language_name;
	std::string		shareInfo;
	std::string		shareUrl;
	std::string		task_id;
	std::string		question;
	bool			is_paid;
	TASK_TYPE		task_type;
	std::string		picUrl;
	std::string		originalUrl;
	std::string		mdpicUrl;
	std::string		hdpicUrl;
	std::string		mdpicSize;
	std::string		context;
	int				countdown;
	bool			is_unfinish;
};

class USER_DATA
{
public:
	std::wstring				address;
	std::wstring				birthday;
	std::wstring				bloodType;
	std::wstring				constellation;
	std::wstring				createTime;
	std::wstring				creator;
	std::wstring				documentType;
	std::wstring				email;
	int							education;
	int							firstLanguage;
	std::wstring				firstLanguageName;
	int							gender;
	std::wstring				headPhoto;
	std::wstring				id;
	std::wstring				idCrad;
	std::vector<std::wstring>	vtIndustries;
	std::vector<std::wstring>	vtIndustrySkills;
	std::vector<std::wstring>	vtInterpreters;
	BOOL						isOverSea;
	std::vector<std::wstring>	vtLanguageSkills;
	std::wstring				location;
	std::wstring				loginName;
	std::vector<std::wstring>	vtManuscriptSkills;
	std::wstring				mobilePhone;
	int							motherTongue;
	std::wstring				motherTongueName;
	std::wstring				nickName;
	std::vector<std::wstring>	vtOralSkills;
	std::wstring				qqId;
	std::wstring				remark;
	int							rstSrc;
	int							secondLanguage;
	std::wstring				secondLanguageName;
	std::wstring				specialty;
	int							status;
	std::wstring				tplId;
	std::wstring				tplName;
	int							translationType;
	std::wstring				trueName;
	std::wstring				updateTime;
	std::wstring				updator;
	int							userType;
	std::wstring				version;
	bitset<32>					bsLangRights;
	BOOL						bInstantTrans;

public:
	void Clear();
	static void ParseFromJson(const Json::Value& value, USER_DATA& data);
};

typedef struct _FEEDBACK_ITEM
{
	int			code;
	std::string	msg;
}FEEDBACK_ITEM;

typedef struct _LANG_INFO
{
	_LANG_INFO() :
		langId(0),
		right(false) {}
	int			langId;
	std::string	langName;
	bool		right;
} LANG_INFO;

class USER_DATA2
{
public:
	USER_DATA2() :
		result(0),
		code(0)
	{

	}
	static void ParseFromJson(const Json::Value& json, USER_DATA2& userData);
public:
	int result;
	class DATA
	{
	public:
		DATA() :
			user_task_num(0),
			isOversea(false),
			translationType(0),
			user_rankings(0),
			send_switch(0),
			tag_help(false),
			tag_we(false)
		{

		}
	public:
		std::string		userId;
		std::string		nickName;
		std::string		headPhoto;
		int				user_task_num;
		std::string		email;
		std::string		mobilePhone;
		bool			isOversea;
		int				translationType;
		std::string		user_collects;
		int				user_rankings;
		std::string		user_title;
		int				send_switch;
		std::vector<LANG_INFO> languages;
		bool			tag_help;
		bool			tag_we;
	};
	DATA			data;
	int				code;
	std::string		msg;
};

class TRANS_INFO
{
public:
	std::string		conversation_id;
	std::string		customer_id;
	int				duration;
};

class UNFINISH_TASK : public TASK_INFO2, public TRANS_INFO
{
public:
	UNFINISH_TASK() :TASK_INFO2(), is_trans(false){
		is_unfinish = true;
	}
	static void ParseFromJson(const Json::Value& json, UNFINISH_TASK& task);
	bool is_trans;
};


typedef struct _LOGIN_DATA
{
	LOGIN_TYPE		loginType;
	std::string		loginName;
	std::string		password;
	std::string		thirdPartId;
	std::string		thirdNickname;
	std::string		pthoto;
} LOGIN_DATA;

class TASK_OPERATE
{
public:
	class MANUSCRIPT
	{
	public:
		int type;
		int file_type;
		std::string content;
		int status;
	};

	std::string user_id;
	std::string task_id;
	std::string task_feedback;
	OPERATE_TYPE op_type;
	MANUSCRIPT	manuscript;
};

typedef struct _TASK_OPERATE2
{
	std::string		taskId;
	OPERATE_TYPE	type;
	std::string		userId;
	std::string		answer;

} TASK_OPERATE2;

typedef struct _OT_RESULT
{
	int				result;
	std::string		task_id;
	std::string		msg;
	int				code;
} OT_RESULT;

typedef struct _TRANS_OPERATE
{
	TO_TYPE			type;
	std::string		userId;
	std::string		nick_name;
	std::string		conversation_id;
} TRANS_OPERATE;

typedef struct _TRANS_RESULT
{
	int				result;
	std::string		err_msg;
	std::string		conversation_id;
	std::string		reply_time;
	std::string		customer_id;
	std::string		first_sentence;
	std::string		finishMsg;

} TRANS_RESULT;

typedef struct _TASK_FEEDBACK
{
	std::string		userId;
	std::string		task_id;
	int				code;
} TASK_FEEDBACK;

//////////////////////////////////////////////////////////////////////////
// APIs
class TransnBizApi
{
public:
	

public:
	static ERR_CODE Login(const std::string& user, const std::string& pwd, Json::Value& jsnRet);
	static ERR_CODE ModifyLang(const std::string& userId, int nFirst, int nSecond);
	static ERR_CODE QueryTask(const std::string& taskId, Json::Value& ret);
	static ERR_CODE OperateTask(const TASK_OPERATE& op, Json::Value& ret);
	static ERR_CODE GetUnfinishTask(const std::string& userId, TASK_INFO& task);

	static ERR_CODE Login2(const LOGIN_DATA& loginData, USER_DATA2& userData);
	static ERR_CODE OperateTask2(const TASK_OPERATE2& op, OT_RESULT& result);
	static ERR_CODE QueryTask2(const std::string taskid, TASK_INFO2& task);
	static ERR_CODE GetFeedbackItems(const TASK_TYPE& type, std::vector<FEEDBACK_ITEM>& items);
	static ERR_CODE GetUnfinishTask2(const std::string& userId, UNFINISH_TASK& task);

	static ERR_CODE FeedbackTask(const TASK_FEEDBACK& feedback, OT_RESULT& result);

	static ERR_CODE CheckUpdate(UPDATE_INFO& update);

	static ERR_CODE OperateTrans(const TRANS_OPERATE& op, TRANS_RESULT& result);
private:
	static void InitCurl(curlcpp* curl, std::string url);
	static void InitCurl_Json(curlcpp* curl, std::string url);
};

