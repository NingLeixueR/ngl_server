#pragma once

#include "nactortype.h"
#include "csvtable.h"
#include "ndefine.h"
#include "net.pb.h"
#include "db.pb.h"
#include "nlog.h"

#include <cstdint>
#include <string>
#include <map>

namespace ngl
{
	// # actor对应的枚举类型
	enum ENUM_ACTOR
	{
		ACTOR_NONE = -1,
		//# 非单例
		ACTOR_ROLE = 1,
		ACTOR_ROBOT = 2,											// 机器人
		ACTOR_LOG = 3,												// 日志
		ACTOR_EXAMPLE_GUESS_NUMBER = 4,								// 例子小游戏:猜数字-数字炸弹

		//# 单例(包括全局单例，和单进程单例) 
		ACTOR_SIGNLE_START = 20,
		ACTOR_SERVER = ACTOR_SIGNLE_START,							// 单例 actor address server  管理分发地址的中心
		ACTOR_CLIENT = 21,											// 单例 actor address client 
		ACTOR_LOGIN = 22,
		ACTOR_GATEWAY = 23,											// 网关
		ACTOR_GATEWAY_C2G = 24,										// 网关 [Client]->[Game]
		ACTOR_GATEWAY_G2C = 25,										// 网关 [Game]	->[Client]
		ACTOR_CREATE = 26,											// 在指定[Server]上创建[Actor]
		ACTOR_ROLE_MANAGE = 27,										// 负责创建actor_role
		ACTOR_KCP = 28,
		ACTOR_ROBOT_MANAGE = 29,
		ACTOR_CSVSERVER = 30,										// 热更csv server
		ACTOR_CSVCLIENT = 31,										// 热更csv client
		ACTOR_NOTICE = 32,											// 公告
		ACTOR_GM = 33,												// GM			(world 进程 )
		ACTOR_GMCLIENT = 34,										// GM CLIENT	(除了world进程每个进程都应该有此进程)
		ACTOR_MAIL = 35,											// 邮件
		ACTOR_CHAT = 36,											// 聊天
		ACTOR_RANKLIST = 37,										// 排行榜
		ACTOR_ACTIVITY_MANAGE = 38,									// 活动管理
		ACTOR_BRIEF = 39,											// brief简要信息
		ACTOR_KEYVALUE = 40,										// 公共key/value
		ACTOR_FAMILY = 41,											// 军团家族
		ACTOR_FRIENDS = 42,											// 好友
		ACTOR_EVENTS = 43,											// 事件
		ACTOR_EVENTS_MAX_COUNT = ACTOR_EVENTS + 10,					// 事件预留10个
		ACTOR_EXAMPLE_MATCH = 54,									// 例子小游戏:匹配
		ACTOR_EXAMPLE_MANAGE = 55,									// 创建管理例子小游戏
		ACTOR_TESTLUA = 56,											// 测试lua
		ACTOR_TESTLUA2 = 57,										// 测试lua2
		ACTOR_DB = 57,												// 必须是最后一个
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
				log_error()->print("type_enum<{}, {}>::type() == enum_null()"
					, tools::type_name<T>()
					, tools::type_name<ENUMT>()
				);
				Throw("type_enum<{}, {}>::type() == enum_null()"
					, tools::type_name<T>()
					, tools::type_name<ENUMT>()
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

	// 宏与类型的绑定
	void auto_actor();

	// # 注册自定义协议
	void tprotocol_customs();

	// # 注册转发协议
	void tprotocol_forward_pb();

	// # actor event注册
	void event_register();
}//namespace ngl