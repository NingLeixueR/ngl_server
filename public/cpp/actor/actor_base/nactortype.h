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
	// # actor对应的枚举类型
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
		ACTOR_SERVER = ACTOR_SIGNLE_START,						// 单例 actor address server  管理分发地址的中心
		ACTOR_CLIENT,											// 单例 actor address client 
		ACTOR_LOGIN,
		ACTOR_GATEWAY,											// 网关
		ACTOR_GATEWAY_C2G,										// 网关 [Client]->[Game]
		ACTOR_GATEWAY_G2C,										// 网关 [Game]	->[Client]
		ACTOR_CREATE,											// 在指定[Server]上创建[Actor]
		ACTOR_LOG,
		ACTOR_KCP,
		ACTOR_MANAGE_ROLE,
		ACTOR_MANAGE_ROBOT,
		ACTOR_MANAGE_GUILD,										// 工会管理器
		ACTOR_CSVCLIENT,										// 热更csv client
		ACTOR_CSVSERVER,										// 热更csv server
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
		ACTOR_CHANNEL,											// 订阅/发布 
		ACTOR_KEYVALUE,											// 公共key/value
		ACTOR_FAMILY,											// 军团家族
		ACTOR_DB,												// 必须是最后一个
		ACTOR_SIGNLE_FINISH = ACTOR_DB + pbdb::ENUM_DB_COUNT,
		ACTOR_COUNT = ACTOR_SIGNLE_FINISH,
	};

	// # 判断ENUM_ACTOR枚举是否为单例
	class enum_actor
	{
	public:
		static bool is_signle(ENUM_ACTOR aenum)
		{
			return aenum >= ACTOR_SIGNLE_START;
		}
	};

	template <typename T,typename ENUMT>
	class type_enum
	{
		type_enum() = delete;
		type_enum(const type_enum&) = delete;
		type_enum& operator=(const type_enum&) = delete;

		static ENUMT m_type;
	public:
		static void inits(ENUMT atype)
		{
			m_type = atype;
		}

		static ENUMT enum_null()
		{
			return (ENUMT)-1;
		}

		static ENUMT type()
		{
			if (m_type == enum_null())
			{
				log_error()->print(
					"type_enum<{}, {}>::type() == enum_null()",
					tools::type_name<T>(),
					tools::type_name<ENUMT>()
				);
				Throw(
					"type_enum<{}, {}>::type() == enum_null()",
					tools::type_name<T>(),
					tools::type_name<ENUMT>()
				);
			}
			return m_type;
		}
	};

	template <typename T, typename ENUMT>
	ENUMT type_enum<T, ENUMT>::m_type = type_enum<T, ENUMT>::enum_null();

	template <typename TACTOR>
	using nactor_type = type_enum<TACTOR, ENUM_ACTOR>;

	// # 根据pbdb::ENUM_DB获取ENUM_ACTOR
	ENUM_ACTOR db_enum(pbdb::ENUM_DB TDBTAB_TYPE);

	// # [aregister == true] 主要是注册协议,宏与类型的绑定
	// # [aregister == false] 实例化db_actor,db server需要
	void init_nactor_type(bool aregister);
}//namespace ngl