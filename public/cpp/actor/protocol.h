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

		// # ע��Э��
		// parm i32_protocolnum aprotocolnumber		Э���
		// parm ENUM_ACTOR aenumactor				actor����
		// parm const fun_pack& apackfun			����ص�
		// parm const fun_run& arunfun				�߼��ص�
		// parm const char* aname					debug name
		static void register_protocol(
			i32_protocolnum aprotocolnumber
			, ENUM_ACTOR aenumactor
			, const fun_pack& apackfun
			, const fun_run& arunfun
			, const char* aname
		);

		// # ACTOR��ͨ�� 
		template <typename T>
		static void registry_actor(ENUM_ACTOR atype, const char* aname)
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
				{// actor type �Ƿ���Ч  || //����ͬ���͵�����actor
					lmanages.push_task_type(atype, lpram);
					return true;
				}
				else
				{
					if (lactorguid.type() == atype)
					{
						if (lactorguid.is_actoridnone())
						{// actor id �Ƿ���Ч
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

		// # ת�� c2g
		template <typename T>
		static void registry_actor_c2g_gateway(ENUM_ACTOR atype, int32_t aprotocolnum, const char* aname)
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

		// # ת�� g2c
		template <typename T>
		static void registry_actor_g2c_gateway(ENUM_ACTOR atype, int32_t aprotocolnum, const char* aname)
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

		// ����ת������Ϣ
		template <typename T>
		static void registry_actor_mass(ENUM_ACTOR atype, int32_t aprotocolnum, const char* aname)
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
					handle_pram lpram = handle_pram::create<T, false>(lactorids, lrequestguid, ldatapack);
					actor_manage::instance().push_task_id(lactorids, lpram);
					return true;
				};
			register_protocol(aprotocolnum, atype, lpackfun, lrunfun, aname);
		}

		static void cmd(const std::shared_ptr<pack>& apack);
	};
}// namespace ngl