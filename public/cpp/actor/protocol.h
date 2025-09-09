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
#include "net.h"

#include <functional>
#include <map>

namespace ngl
{
	class protocol
	{
		protocol() = delete;
		protocol(const protocol&) = delete;
		protocol& operator=(const protocol&) = delete;
	public:
		using fun_pack	= std::function<std::shared_ptr<void>(std::shared_ptr<pack>&)>;
		using fun_run	= std::function<bool(std::shared_ptr<pack>&, std::shared_ptr<void>&)>;

		static void push(std::shared_ptr<pack>& apack);

		// # 注册协议
		static void register_protocol(
			i32_protocolnum aprotocolnumber			/*协议号*/
			, ENUM_ACTOR aenumactor					/*处理此消息的actor类型*/
			, const fun_pack& apackfun				/*解包回调*/			
			, const fun_run& arunfun				/*逻辑回调*/
			, const char* aname						/*debug name*/
		);

		// # ACTOR间通信 
		template <typename T>
		static void registry_actor(
			ENUM_ACTOR atype						/*actor类型*/
			, const char* aname						/*debug name*/
		)
		{
			fun_pack lpackfun = [atype](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
			{
				Try
				{
					T* lp = new T();
					std::shared_ptr<void> ltemp(lp);
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
		static void registry_actor_c2g(
			ENUM_ACTOR atype						/*actor类型*/
			, int32_t aprotocolnum					/*协议号*/
			, const char* aname						/*debug name*/
		)
		{
			fun_pack lpackfun = [](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
				{
					Try
					{
						auto lp = new np_actor_forward<T, forward_c2g<forward>>();
						std::shared_ptr<void> ltemp(lp);
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
					nguid lguid(atype, tab_self_area, nconfig::m_nodeid);
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
		static void registry_actor_g2c(
			ENUM_ACTOR atype						/*actor类型*/
			, int32_t aprotocolnum					/*协议号*/
			, const char* aname						/*debug name*/
		)
		{
			fun_pack lpackfun = [](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
				{
					Try
					{
						auto lp = new  np_actor_forward<T, forward_g2c<forward>>();
						std::shared_ptr<void> ltemp(lp);
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
					nguid lguid(atype, tab_self_area, nconfig::m_nodeid);
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
		static void registry_actor_mass(
			ENUM_ACTOR atype						/*actor类型*/
			, int32_t aprotocolnum					/*协议号*/
			, const char* aname						/*debug name*/
		)
		{
			fun_pack lpackfun = [](std::shared_ptr<pack>& apack)->std::shared_ptr<void>
				{
					Try
					{
						np_mass_actor<T>* lp = new np_mass_actor<T>();
						std::shared_ptr<void> ltemp(lp);
						if (structbytes<np_mass_actor<T>>::tostruct(apack, *lp))
						{
							return ltemp;
						}
					}Catch
					return nullptr;
				};
			fun_run lrunfun = [atype](std::shared_ptr<pack>& apack, std::shared_ptr<void>& aptrpram)->bool
				{
					std::shared_ptr<np_mass_actor<T>> ldatapack = std::static_pointer_cast<np_mass_actor<T>>(aptrpram);
					std::set<i64_actorid>& lactorids = ldatapack->m_actorids;
					nguid lrequestguid(apack->m_head.get_request_actor());

					std::shared_ptr<T> ldata = ldatapack->shared_data();
					
					handle_pram lpram = handle_pram::create<T, false>(lactorids, lrequestguid, ldata);
					actor_manage::instance().push_task_id(lactorids, lpram);
					return true;
				};
			register_protocol(aprotocolnum, atype, lpackfun, lrunfun, aname);
		}

		// # 处理telnet命令
		static void telnet_cmd(const std::shared_ptr<pack>& apack);
	};
}// namespace ngl