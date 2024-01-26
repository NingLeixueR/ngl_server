#pragma once

#include <functional>
#include <map>
#include "threadtools.h"
#include "nlog.h"
#include "type.h"
#include "actor_enum.h"
#include "pack.h"
#include "actor_protocol.h"
#include "handle_pram.h"
#include "structbytes.h"
#include "actor_guid.h"
#include "actor_manage.h"
#include "init_protobuf.h"

namespace ngl
{
	class protocol
	{
	public:
		using typefun_pack = std::function<std::shared_ptr<void>(std::shared_ptr<pack>&)>;
		using typefun_run = std::function<bool(std::shared_ptr<pack>&, std::shared_ptr<void>&)>;
	public:
		static void push(std::shared_ptr<pack>& apack);
		static void register_protocol(
			EPROTOCOL_TYPE atype					// 协议类型
			, i32_protocolnum aprotocolnumber		// 协议号
			, ENUM_ACTOR aenumactor					// actor类型
			, const typefun_pack& apackfun			// 解包回调
			, const typefun_run& arunfun			// 逻辑回调
			, const char* aname);
		static i32_serverid nodeid();


		template <typename T>
		static void log(const char* astr)
		{
			std::string lname;
			LogLocalError("% [%] success"
				, astr
				, tools::type_name<T>(lname)
			);
		}

		//// --- ACTOR间通信 
		template <typename T, EPROTOCOL_TYPE TYPE>
		static void registry_actor(ENUM_ACTOR atype, const char* aname)
		{
			typefun_pack lpackfun = [atype](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
			{
				Try
				{
					std::string lname;
					T* lp = new T();
					std::shared_ptr<void> ltemp(lp);
					if (structbytes<T>::tostruct(apack, *lp))
					{
						protocol::log<T>("pop success");
						return ltemp;
					}
				}Catch;
				protocol::log<T>("pop fail");
				return nullptr;
			};
			std::string lname = aname;
			typefun_run lrunfun = [atype, lname](std::shared_ptr<pack>& apack, std::shared_ptr<void>& aptrpram)->bool
			{
				actor_guid lactorguid(apack->m_head.get_actor());
				actor_guid lrequestactorguid(apack->m_head.get_request_actor());
				std::shared_ptr<T> ldatapack = std::static_pointer_cast<T>(aptrpram);
				handle_pram lpram;
				handle_pram::create<T, false, false>(lpram
					, lactorguid
					, lrequestactorguid
					, ldatapack
				);
				lpram.m_pack = apack;

				if (lactorguid.is_actortypenone() || lactorguid.is_moreactor(atype))
				{// actor type 是否无效  || //发给同类型的所有actor
					actor_manage::getInstance().push_task_type(atype, lpram);
					return true;
				}
				else
				{
					if (lactorguid.type() == atype)
					{
						if (lactorguid.is_actoridnone())// actor id 是否无效
							actor_manage::getInstance().push_task_type(atype, lpram);
						else
							actor_manage::getInstance().push_task_id(lactorguid.id(), lpram, false);
					}
				}
				return true;
			};
			register_protocol(TYPE, init_protobuf::protocol<T>(), atype, lpackfun, lrunfun, aname);
		}

		// 转发[负责转发的actor必须是单例actor]
		template <typename T, bool ISTRUE, EPROTOCOL_TYPE TYPE>
		static void registry_actor_forward(ENUM_ACTOR atype, int32_t aprotocolnum, const char* aname)
		{
			typefun_pack lpackfun = [](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
			{
				Try
				{
					using typeforward = actor_forward<T, TYPE, ISTRUE, ngl::forward>;
					typeforward* lp = new typeforward();
					lp->m_recvpack = apack;
					std::shared_ptr<void> ltemp(lp);
					if (structbytes<typeforward>::tostruct(apack, *lp, true))
					{
						protocol::log<T>("pop success");
						return ltemp;
					}
				}Catch;
				protocol::log<T>("pop fail");
				return nullptr;
			};
			typefun_run lrunfun = [atype](std::shared_ptr<pack>& apack, std::shared_ptr<void>& aptrpram)->bool
			{
				using typeforward = actor_forward<T, TYPE, ISTRUE, ngl::forward>;
				std::shared_ptr<typeforward> ldatapack = std::static_pointer_cast<typeforward>(aptrpram);
				actor_guid lguid(atype, tab_self_area, nconfig::m_nodeid);
				actor_guid lrequestguid(apack->m_head.get_request_actor());
				handle_pram lpram;
				handle_pram::create(lpram, lguid, lrequestguid, ldatapack);
				lpram.m_pack = apack;
				actor_manage::getInstance().push_task_id(lguid, lpram, false);
				return true;
			};			
			register_protocol(TYPE, aprotocolnum, atype, lpackfun, lrunfun, aname);
		}

		// 接收转发的消息
		template <typename T, bool ISTRUE, EPROTOCOL_TYPE TYPE>
		static void registry_actor_recvforward(ENUM_ACTOR atype, int32_t aprotocolnum, const char* aname)
		{
			typefun_pack lpackfun = [](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
			{
				Try
				{
					using typeforward = actor_forward<T, TYPE, ISTRUE, T>;
					typeforward* lp = new typeforward();
					std::shared_ptr<void> ltemp(lp);
					if (structbytes<typeforward>::tostruct(apack, *lp))
					{
						protocol::log<T>("pop success");
						return ltemp;
					}					
				}Catch;
				protocol::log<T>("pop fail");
				return nullptr;
			};
			typefun_run lrunfun = [atype](std::shared_ptr<pack>& apack, std::shared_ptr<void>& aptrpram)->bool
			{
				using typeforward = actor_forward<T, TYPE, ISTRUE, T>;
				actor_guid lrequestguid(apack->m_head.get_request_actor());
				std::shared_ptr<T> ldatapack = std::static_pointer_cast<T>(aptrpram);
				typeforward* lp = (typeforward*)aptrpram.get();
				for (int i = 0; i < lp->m_uid.size() && i < lp->m_area.size(); ++i)
				{
					actor_guid lguid(atype, lp->m_area[i], lp->m_uid[i]);
					handle_pram lpram;
					handle_pram::create<T, false, false>(lpram, lguid, lrequestguid, ldatapack);
					actor_manage::getInstance().push_task_id(lguid, lpram, false);
				}
				return true;
			};
			register_protocol(TYPE, aprotocolnum, atype, lpackfun, lrunfun, aname);			
		}
	};
}