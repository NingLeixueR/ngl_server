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
	public:
		using tpptr		= std::shared_ptr<pack>;
		using tvptr		= std::shared_ptr<void>;

		template <typename T>
		using tptr		= std::shared_ptr<T>;

		using fun_pack	= std::function<tvptr(tpptr&)>;
		using fun_run	= std::function<bool(tpptr&, tvptr&)>;

		static void push(tpptr& apack);

		static void register_protocol(
			EPROTOCOL_TYPE atype					// 协议类型
			, i32_protocolnum aprotocolnumber		// 协议号
			, ENUM_ACTOR aenumactor					// actor类型
			, const fun_pack& apackfun				// 解包回调
			, const fun_run& arunfun				// 逻辑回调
			, const char* aname						// debug name
		);

		static i32_serverid nodeid();

		// ACTOR间通信 
		template <typename T, EPROTOCOL_TYPE TYPE>
		static void registry_actor(ENUM_ACTOR atype, const char* aname)
		{
			fun_pack lpackfun = [atype](tpptr& apack)->tvptr
			{
				Try
				{
					T* lp = new T();
					tvptr ltemp(lp);
					if (structbytes<T>::tostruct(apack, *lp))
					{
						return ltemp;
					}
				}Catch;
				return nullptr;
			};
			std::string lname = aname;
			fun_run lrunfun = [atype, lname](tpptr& apack, tvptr& aptrpram)->bool
			{
				nguid lactorguid(apack->m_head.get_actor());
				nguid lrequestactorguid(apack->m_head.get_request_actor());
				std::shared_ptr<T> ldatapack = std::static_pointer_cast<T>(aptrpram);
				handle_pram lpram;
				handle_pram::create<T, false, false>(lpram
					, lactorguid
					, lrequestactorguid
					, ldatapack
				);
				lpram.m_pack = apack;

				actor_manage& lmanages = actor_manage::getInstance();
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
							lmanages.push_task_id(lactorguid.id(), lpram, false);
						}
					}
				}
				return true;
			};
			register_protocol(TYPE, tprotocol::protocol<T>(), atype, lpackfun, lrunfun, aname);
		}

		// 转发[负责转发的actor必须是单例actor]
		template <typename T, bool ISTRUE, EPROTOCOL_TYPE TYPE>
		static void registry_actor_forward(ENUM_ACTOR atype, int32_t aprotocolnum, const char* aname)
		{
			fun_pack lpackfun = [](tpptr& apack)->tvptr
			{
				Try
				{
					using typeforward = np_actor_forward<T, TYPE, ISTRUE, ngl::forward>;
					typeforward* lp = new typeforward();
					lp->m_recvpack = apack;
					tvptr ltemp(lp);
					if (structbytes<typeforward>::tostruct(apack, *lp, true))
					{
						return ltemp;
					}
				}Catch
				return nullptr;
			};
			fun_run lrunfun = [atype](tpptr& apack, tvptr& aptrpram)->bool
			{
				using typeforward = np_actor_forward<T, TYPE, ISTRUE, ngl::forward>;
				std::shared_ptr<typeforward> ldatapack = std::static_pointer_cast<typeforward>(aptrpram);
				nguid lguid(atype, tab_self_area, nconfig::m_nodeid);
				nguid lrequestguid(apack->m_head.get_request_actor());
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
			fun_pack lpackfun = [](tpptr& apack)->tvptr
			{
				Try
				{
					using typeforward = np_actor_forward<T, TYPE, ISTRUE, T>;
					typeforward* lp = new typeforward();
					tvptr ltemp(lp);
					if (apack->m_protocol == ENET_KCP)
					{
						lp->make_data();
						if (structbytes<T>::tostruct(apack, *lp->get_data()))
						{
							i64_actorid lactorid = nets::kcp()->find_actorid(apack->m_id);
							lp->m_uid.push_back(nguid::actordataid(lactorid));
							lp->m_area.push_back(nguid::area(lactorid));
							return ltemp;
						}
					}
					else
					{
						if (structbytes<typeforward>::tostruct(apack, *lp))
							return ltemp;
					}
				}Catch
				return nullptr;
			};
			fun_run lrunfun = [atype](tpptr& apack, tvptr& aptrpram)->bool
			{
				using typeforward = np_actor_forward<T, TYPE, ISTRUE, T>;
				nguid lrequestguid(apack->m_head.get_request_actor());
				std::shared_ptr<T> ldatapack = std::static_pointer_cast<T>(aptrpram);
				typeforward* lp = (typeforward*)aptrpram.get();
				for (int i = 0; i < lp->m_uid.size() && i < lp->m_area.size(); ++i)
				{
					nguid lguid(atype, lp->m_area[i], lp->m_uid[i]);
					handle_pram lpram;
					if (apack->m_protocol == ENET_KCP)
						lpram.m_pack = apack;
					handle_pram::create<T, false, false>(lpram, lguid, lrequestguid, ldatapack);
					actor_manage::getInstance().push_task_id(lguid, lpram, false);
				}
				return true;
			};
			register_protocol(TYPE, aprotocolnum, atype, lpackfun, lrunfun, aname);			
		}
	};
}// namespace ngl