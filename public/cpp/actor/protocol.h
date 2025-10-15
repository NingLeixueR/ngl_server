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
			protocol::fun_pack								m_packfun;		// ����ص�
			std::map<ENUM_ACTOR, protocol::fun_run>			m_runfun;		// actor���Ͷ�Ӧ���߼��ص�
		};
		static std::map<i32_protocolnum, protocol::pfun>	m_protocolfun;	// Э��Ź���pfun
		static std::shared_mutex							m_mutex;		// ��

	public:
		static pfun* find(i32_protocolnum aprotocolnum);

		static const char* name(i32_protocolnum aprotocolnum/*Э���*/);

		static void print(
			const char* amsg						/*��ӡ����*/
			, i32_protocolnum aprotocolnum			/*Э���*/
		);

		// # ע������Э��
		static void register_protocol(
			int aprotocolnumber							/*Э���*/
			, ENUM_ACTOR aenumactor						/*actor����*/
			, const protocol::fun_pack& apackfun		/*����ص�*/
			, const protocol::fun_run& arunfun			/*�߼��ص�*/
			, const char* aname							/*debug name*/
		);

		// # �����������ݰ�[net pack],�������ϲ��߼� 
		static void push(std::shared_ptr<pack>& apack);

		// # ACTOR��ͨ�� 
		template <typename T>
		static void registry_actor(
			ENUM_ACTOR atype						/*actor����*/
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

		// # gatewayע���ת��c2g(��[client]>>[gateway]>>[������]) 
		template <typename T>
		static void registry_actor_c2g(
			ENUM_ACTOR atype						/*actor����*/
			, int32_t aprotocolnum					/*Э���*/
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
					nguid lguid(atype, tab_self_area, nconfig::m_tcount);
					nguid lrequestguid(apack->m_head.get_request_actor());
					handle_pram lpram = handle_pram::create<T, forward>(lguid, lrequestguid, ldatapack);
					lpram.m_pack = apack;
					actor_manage::instance().push_task_id(lguid, lpram);
					return true;
				};
			register_protocol(aprotocolnum, atype, lpackfun, lrunfun, aname);
		}

		// # gatewayע���ת��g2c(��[������]->[�ͻ���]) 
		template <typename T>
		static void registry_actor_g2c(
			ENUM_ACTOR atype						/*actor����*/
			, int32_t aprotocolnum					/*Э���*/
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
					nguid lguid(atype, tab_self_area, nconfig::m_tcount);
					nguid lrequestguid(apack->m_head.get_request_actor());
					handle_pram lpram = handle_pram::create<np_actor_forward<T, forward_g2c<forward>>>(lguid, lrequestguid, ldatapack);
					lpram.m_pack = apack;
					actor_manage::instance().push_task_id(lguid, lpram);
					return true;
				};
			register_protocol(aprotocolnum, atype, lpackfun, lrunfun, aname);
		}

		// # Ⱥ����Ϣ(��������np_mass_actor<T> ��TͶ�ݵ���Ӧactor)
		template <typename T>
		static void registry_actor_mass(
			ENUM_ACTOR atype						/*actor����*/
			, int32_t aprotocolnum					/*Э���*/
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

		// # ����telnet����
		static void telnet_cmd(const std::shared_ptr<pack>& apack);
	};
}// namespace ngl