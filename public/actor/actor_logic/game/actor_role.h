#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "actor_protocol.h"
#include "actor_timer.h"
#include "actor_switchprocess.h"
#include "net.pb.h"

#include <boost/shared_ptr.hpp>

#include "bag.h"
#include "roleinfo.h"
#include "remakes.h"

namespace ngl
{
	class actor_manage_role;
	class actor_role : public actor
	{
	public:
		remakes m_remakes;
		roleinfo m_info;
		bag m_bag;
		i32_serverid m_gatewayid;
	public:
		actor_role(i16_area aarea, i32_actordataid aroleid, void* adata) :
			actor(
				actorparm
				{
					.m_parm
					{
						.m_type = ACTOR_ROLE,
						.m_area = aarea,
						.m_id = aroleid,
						.m_manage_dbclient = true
					},
					.m_weight = 0x7fffffff,
					.m_broadcast = true,
				}),
			m_gatewayid(((actor_switch_process_role*)(adata))->m_gatewayid)
		{
			assert(aarea == ttab_servers::tab()->m_area);
		}

		virtual i32_serverid get_getwayserverid()
		{
			return m_gatewayid;
		}

		virtual void init()
		{
			m_info.set(this);
			m_bag.set(this);
		}
		static void actor_register();

		virtual ~actor_role() {}

		virtual void loaddb_finish(bool adbishave);

		enum { ACTOR_TYPE = ACTOR_ROLE };

		int roleid()
		{
			return m_info.get()->getconst().m_id();
		}

		template <typename T>
		void send2client(std::shared_ptr<T>& adata)
		{
			actor_base::send_client(m_gatewayid, id_guid(), adata);
		}

		void sync_data_client()
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>();
			*pro->mutable_m_role() = m_info.get()->getconst();
			*pro->mutable_m_bag() = m_bag.get()->getconst();
			send2client(pro);
			LogLocalError("[sync]###[%]", m_info.get()->getconst().m_base().m_name());
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_ROLE_SYNC& adata)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>();
			*pro->mutable_m_role() = m_info.get()->getconst();
			*pro->mutable_m_bag() = m_bag.get()->getconst();
			send2client(pro);
			LogLocalError("[sync]###[%]", m_info.get()->getconst().m_base().m_name());
			return true;
		}
		// CMD 协议
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_CMD& adata);

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_GET_TIME& adata)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>();
			pro->set_m_utc(localtime::gettime());
			send2client(pro);
			//LogLocalError("######Get Server Time##[%][%]", m_info.id(), m_info.db()->name());
			return true;
		}

		// PROBUFF_PROTOCOLNUM_LOGIC_SWITCH_LINE		= 18;			// [请求]切换线路
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_SWITCH_LINE& adata)
		{
			tab_servers* tab = ttab_servers::node_tnumber(GAME, adata.m_line());
			if (tab == nullptr)
				return false;
			i32_sessionid lsession = nserver->get_sessionid(tab->m_id);
			if (lsession == -1)
			{
				LogLocalError("LOGIC_SWITCH_LINE Error line[%] severid[%]", adata.m_line(), tab->m_id);
				return false;
			}
			actor_switch_process_role pro;
			pro.m_create = false;
			pro.m_gatewayid = m_gatewayid;
			actor_switchprocess::switch_process(id_guid(), nconfig::m_nodeid, tab->m_id, pro);
			//LogLocalError("######Switch Line##[%][%]", m_info.id(), m_info.db()->name());
			//crossprocess<actor_role>(lserverid);
			
			return true;
		}

#define foward_module(MODULE, TYPE)														\
	bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, TYPE& adata)	\
	{																					\
		std::shared_ptr<mforward<TYPE>> pro(new mforward<TYPE>(id_guid(), adata));		\
		send_actor(actor_guid::make_self(MODULE), pro);									\
		return true;																	\
	}

		foward_module(ACTOR_NOTICE, pbnet::PROBUFF_NET_GET_NOTICE)
		foward_module(ACTOR_MAIL, pbnet::PROBUFF_NET_MAIL_LIST)
		foward_module(ACTOR_MAIL, pbnet::PROBUFF_NET_MAIL_READ)
		foward_module(ACTOR_MAIL, pbnet::PROBUFF_NET_MAIL_DRAW)
		foward_module(ACTOR_MAIL, pbnet::PROBUFF_NET_MAIL_DEL)


#define foward_cross_module(MODULE, TYPE, ISCROSSFUN)									\
	bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, TYPE& adata)	\
	{																					\
		std::shared_ptr<mforward<TYPE>> pro(new mforward<TYPE>(id_guid(), adata));		\
		if(ISCROSSFUN(adata))															\
		{																				\
			send_actor(actor_guid::make(MODULE, ttab_servers::tab()->m_crossarea, actor_guid::none_actordataid()), pro);	\
		}																				\
		else																			\
		{																				\
			send_actor(actor_guid::make(MODULE, ttab_servers::tab()->m_area, actor_guid::none_actordataid()), pro);			\
		}																				\
		return true;																	\
	}
		bool is_cross(pbnet::PROBUFF_NET_CHAT& adata)
		{
			return adata.m_channelid() == 2;
		}
		foward_cross_module(ACTOR_CHAT, pbnet::PROBUFF_NET_CHAT, is_cross)


		//actor_send_item
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_send_item& adata)
		{
			d_remakes(this, adata.m_src);
			m_bag.add_item(adata.m_item);
			return true;
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_disconnect_close& adata)
		{
			erase_actor_byid();
			return true;
		}
		
		// 定时器
		bool timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata);

	private:
	};

	


}

