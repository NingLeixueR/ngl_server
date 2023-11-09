#pragma once

#include <cstdint>
#include <map>
#include <string>
#include "actor_define.h"
#include "csvtable.h"
#include "data.pb.h"
#include "db.pb.h"

namespace ngl
{
	enum ENUM_ACTOR
	{
		ACTOR_NONE = -1,
		//#######################  非单例  #####################
		ACTOR_ROLE = 1,
		ACTOR_ROBOT,										// 机器人
		ACTOR_GUILD,										// 工会	
		//#######################  单例  #######################
		ACTOR_SIGNLE_START,
		ACTOR_ADDRESS_SERVER = ACTOR_SIGNLE_START,				// 单例 actor address server  管理分发地址的中心
		ACTOR_ADDRESS_CLIENT,									// 单例 actor address client 
		ACTOR_LOGIN,
		ACTOR_GATEWAY,											// 网关
		ACTOR_GATEWAY_CLIENT2GAME,								// 网关 [Client]->[Game]
		ACTOR_GATEWAY_GAME2CLIENT,								// 网关 [Game]	->[Client]
		ACTOR_SPROCESS,
		ACTOR_LOG,
		ACTOR_MANAGE_ROLE,
		ACTOR_MANAGE_ROBOT,
		ACTOR_MANAGE_GUILD,										// 工会管理器
		ACTOR_MANAGE_MAP,
		ACTOR_RELOADCSV,										// 接收分发然后重载csv
		ACTOR_RELOADCSV_DISTRIBUTE,								// 分发csv
		ACTOR_NOTICE,											// 公告
		ACTOR_GM,												// GM
		ACTOR_MAIL,												// 邮件
		ACTOR_CHAT,												// 聊天
		ACTOR_RANKLIST,											// 排行榜
		ACTOR_ACTIVITY_MANAGE,									// 活动管理
		ACTOR_BRIEF,											// brief简要信息
		ACTOR_CROSS,											// 跨服
		ACTOR_CROSSDB,											// 跨服数据库
		ACTOR_CROSSCHAT,										// 跨服聊天
		ACTOR_DB,												// 必须是最后一个
		ACTOR_SIGNLE_FINISH = ACTOR_DB + pbdb::ENUM_DB_COUNT,
		ACTOR_COUNT = ACTOR_SIGNLE_FINISH,
	};


	class enum_actor
	{
	public:
		static bool is_signle(ENUM_ACTOR aenum);
	};

	// 通过名字获取枚举
	class actor_typename
	{
		actor_typename();
	public:
		static actor_typename& getInstance()
		{
			static actor_typename ltemp;
			return ltemp;
		}

		static bool name2enum(const std::string& aenumname, ENUM_ACTOR& avalue);
		static const char* enum2name(ENUM_ACTOR aenum);
	};

}

