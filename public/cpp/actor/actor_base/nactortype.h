#pragma once

#include "csvtable.h"
#include "ndefine.h"
#include "net.pb.h"
#include "db.pb.h"

#include <cstdint>
#include <string>
#include <map>

namespace ngl
{
	enum ENUM_ACTOR
	{
		ACTOR_NONE = -1,
		//#######################  非单例  #####################
		ACTOR_ROLE = 1,
		ACTOR_ROBOT,										// 机器人
		ACTOR_GUILD,										// 工会	
		ACTOR_PLAYS,										// 玩法
		ACTOR_PLAYS_GO_UNDERGROUNDPALACE = ACTOR_PLAYS,		// 玩法[勇闯地宫]

		ACTOR_PLAYS_END = ACTOR_PLAYS + pbnet::eplays_count,
		//#######################  单例  #######################
		ACTOR_SIGNLE_START,
		ACTOR_SERVER = ACTOR_SIGNLE_START,				// 单例 actor address server  管理分发地址的中心
		ACTOR_CLIENT,									// 单例 actor address client 
		ACTOR_LOGIN,
		ACTOR_GATEWAY,											// 网关
		ACTOR_GATEWAY_C2G,								// 网关 [Client]->[Game]
		ACTOR_GATEWAY_G2C,								// 网关 [Game]	->[Client]
		ACTOR_CREATE,											// 在指定[Server]上创建[Actor]
		ACTOR_LOG,
		ACTOR_KCP,
		ACTOR_MANAGE_ROLE,
		ACTOR_MANAGE_ROBOT,
		ACTOR_MANAGE_GUILD,										// 工会管理器
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
		ACTOR_MANAGE_PLAYS,										// 创建玩法
		ACTOR_MATCHING,											// 匹配
		ACTOR_CALENDAR,											// 脚本日历
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
	class nactortype
	{
		nactortype() {}
	public:
		static nactortype& getInstance()
		{
			static nactortype ltemp;
			return ltemp;
		}

		static bool name2enum(const std::string& aenumname, ENUM_ACTOR& avalue);
		static const char* enum2name(ENUM_ACTOR aenum);
	};
}//namespace ngl

