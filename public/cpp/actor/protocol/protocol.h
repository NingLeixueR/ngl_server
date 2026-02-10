/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "actor_manage.h"
#include "threadtools.h"
#include "handle_pram.h"
#include "structbytes.h"
#include "nactortype.h"
#include "nprotocol.h"
#include "tprotocol.h"
#include "nguid.h"
#include "nlog.h"
#include "type.h"
#include "pack.h"
#include "ntcp.h"

#include <functional>
#include <map>

namespace ngl
{
	enum eprotocol_tar
	{};

	class protocol
	{
		protocol() = delete;
		protocol(const protocol&) = delete;
		protocol& operator=(const protocol&) = delete;

	public:
		using fun_pack = std::function<std::shared_ptr<void>(std::shared_ptr<pack>&)>;
		using fun_run = std::function<bool(std::shared_ptr<pack>&, std::shared_ptr<void>&)>;
	private:
		struct pfun
		{
			protocol::fun_pack								m_packfun = nullptr;		// 解包回调
			std::map<ENUM_ACTOR, protocol::fun_run>			m_runfun;					// actor类型对应的逻辑回调
		};
		static std::map<i32_protocolnum, protocol::pfun>	m_protocolfun;	// 协议号关联pfun
		static std::shared_mutex							m_mutex;		// 锁

	public:
		static pfun* find(i32_protocolnum aprotocolnum);

		static const char* name(i32_protocolnum aprotocolnum);

		static void print(const char* amsg, i32_protocolnum aprotocolnum);

		// # 注册网络协议
		static void register_protocol(
			int aprotocolnumber, ENUM_ACTOR aenumactor, const protocol::fun_pack& apackfun, const protocol::fun_run& arunfun, const char* aname
		);

		// # 解析网络数据包[net pack],交付给上层逻辑 
		static void push(std::shared_ptr<pack>& apack);

		// # ACTOR间通信 
		template <typename T>
		static void registry_actor(ENUM_ACTOR atype, const char* aname)
		{
			fun_pack lpackfun = [atype](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
			{
				Try
				{
					auto lp = std::make_shared<T>();
					std::shared_ptr<void> ltemp = std::static_pointer_cast<void>(lp);
					if (structbytes<T>::tostruct(apack, *lp))
					{
						return ltemp;
					}
				}Catch
				return nullptr;
			};
			fun_run lrunfun = [atype](std::shared_ptr<pack>& apack, std::shared_ptr<void>& aptrpram)->bool
			{
				nguid lactorguid(apack->m_head.get_actor());
				nguid lrequestactorguid(apack->m_head.get_request_actor());
				std::shared_ptr<T> ldatapack = std::static_pointer_cast<T>(aptrpram);
				handle_pram lpram = handle_pram::create<T, false, false>(lactorguid, lrequestactorguid, ldatapack);
				lpram.m_pack = apack;

				actor_manage& lmanages = actor_manage::instance();

				if (nconfig.nodetype() == ROBOT && apack->m_head.get_actor() == nguid::make())
				{
					lmanages.push_task_type(atype, lpram);
					return true;
				}

				if (lactorguid.is_actortypenone() || lactorguid.is_moreactor(atype))
				{// actor type 是否无效  || //发给同类型的所有actor
					lmanages.push_task_type(atype, lpram);
					return true;
				}
				else
				{
					if (lactorguid.type() == atype)
					{
						if (lactorguid.is_actoridnone())
						{// actor id 是否无效
							lmanages.push_task_type(atype, lpram);
						}							
						else
						{
							lmanages.push_task_id(lactorguid.id(), lpram);
						}
					}
				}
				return true;
			};
			register_protocol(tprotocol::protocol<T>(), atype, lpackfun, lrunfun, aname);
		}

		// # gateway注册的转发c2g(由[client]>>[gateway]>>[服务器]) 
		template <typename T>
		static void registry_actor_c2g(ENUM_ACTOR atype, int32_t aprotocolnum, const char* aname)
		{
			fun_pack lpackfun = [](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
				{
					Try
					{
						auto lp = std::make_shared<np_actor_forward<T, forward_c2g<forward>>>();
						std::shared_ptr<void> ltemp = std::static_pointer_cast<void>(lp);
						if (structbytes<np_actor_forward<T, forward_c2g<forward>>>::tostruct(apack, *lp, true))
						{
							return ltemp;
						}
					}Catch
					return nullptr;
				};
			fun_run lrunfun = [atype](std::shared_ptr<pack>& apack, std::shared_ptr<void>& aptrpram)->bool
				{
					auto ldatapack = std::static_pointer_cast<np_actor_forward<T, forward_c2g<forward>>>(aptrpram);
					nguid lguid(atype, tab_self_area, nconfig.tcount());
					nguid lrequestguid(apack->m_head.get_request_actor());
					handle_pram lpram = handle_pram::create<T, forward>(lguid, lrequestguid, ldatapack);
					lpram.m_pack = apack;
					actor_manage::instance().push_task_id(lguid, lpram);
					return true;
				};
			register_protocol(aprotocolnum, atype, lpackfun, lrunfun, aname);
		}

		// # gateway注册的转发g2c(由[服务器]->[客户端]) 
		template <typename T>
		static void registry_actor_g2c(ENUM_ACTOR atype, int32_t aprotocolnum, const char* aname)
		{
			fun_pack lpackfun = [](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
				{
					Try
					{
						auto lp = std::make_shared<np_actor_forward<T, forward_g2c<forward>>>();
						std::shared_ptr<void> ltemp = std::static_pointer_cast<void>(lp);
						if (structbytes<np_actor_forward<T, forward_g2c<forward>>>::tostruct(apack, *lp, true))
						{
							return ltemp;
						}
					}Catch
					return nullptr;
				};
			fun_run lrunfun = [atype](std::shared_ptr<pack>& apack, std::shared_ptr<void>& aptrpram)->bool
				{
					auto ldatapack = std::static_pointer_cast<np_actor_forward<T, forward_g2c<forward>>>(aptrpram);
					nguid lguid(atype, tab_self_area, nconfig.tcount());
					nguid lrequestguid(apack->m_head.get_request_actor());
					handle_pram lpram = handle_pram::create<np_actor_forward<T, forward_g2c<forward>>>(lguid, lrequestguid, ldatapack);
					lpram.m_pack = apack;
					actor_manage::instance().push_task_id(lguid, lpram);
					return true;
				};
			register_protocol(aprotocolnum, atype, lpackfun, lrunfun, aname);
		}

		// # 群发消息(解析剥离np_mass_actor<T> 将T投递到对应actor)
		template <typename T>
		static void registry_actor_mass(int32_t aprotocolnum, const char* aname)
		{
			fun_pack lpackfun = [](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
				{
					Try
					{
						auto lp = std::make_shared<np_mass_actor<T>>();
						std::shared_ptr<void> ltemp(lp);
						if (structbytes<np_mass_actor<T>>::tostruct(apack, *lp))
						{
							return ltemp;
						}
					}Catch
					return nullptr;
				};
			fun_run lrunfun = [](std::shared_ptr<pack>& apack, std::shared_ptr<void>& aptrpram)->bool
				{
					std::shared_ptr<np_mass_actor<T>> ldatapack = std::static_pointer_cast<np_mass_actor<T>>(aptrpram);
					std::set<i64_actorid>& lactorids = ldatapack->m_actorids;
					nguid lrequestguid(apack->m_head.get_request_actor());

					std::shared_ptr<T> ldata = ldatapack->shared_data();
					
					handle_pram lpram = handle_pram::create<T, false>(lactorids, lrequestguid, ldata);
					actor_manage::instance().push_task_id(lactorids, lpram);
					return true;
				};
			register_protocol(aprotocolnum, (ENUM_ACTOR)nguid::none_type(), lpackfun, lrunfun, aname);
		}

		// # 处理telnet命令
		static void telnet_cmd(const std::shared_ptr<pack>& apack);
	};
}// namespace ngl