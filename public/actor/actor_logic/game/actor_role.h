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
#include "autoitem.h"
#include "task.h"

namespace ngl
{
	class actor_manage_role;
	class actor_role : public actor
	{
	public:
		remakes m_remakes;
		roleinfo m_info;
		bag m_bag;
		task m_task;
		i32_serverid m_gatewayid;
	public:
		actor_role(i16_area aarea, i32_actordataid aroleid, void* adata);

		virtual i32_serverid get_getwayserverid();

		virtual void init();

		static void actor_register();

		virtual ~actor_role();

		virtual void loaddb_finish(bool adbishave);
		// 执行handle之后调用
		virtual void handle_after();

		enum { ACTOR_TYPE = ACTOR_ROLE };

		i64_actorid roleid();

		template <typename T>
		void send2client(std::shared_ptr<T>& adata)
		{
			actor_base::send_client(m_gatewayid, id_guid(), adata);
		}

		void sync_data_client();

#define foward_module(MODULE, TYPE)														\
	bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, TYPE& adata)	\
	{																					\
		std::shared_ptr<mforward<TYPE>> pro(new mforward<TYPE>(id_guid(), adata));		\
		send_actor(actor_guid::make_self(MODULE), pro);									\
		return true;																	\
	}
		
#define foward_cross_module(MODULE, TYPE, ISCROSSFUN)									\
	bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, TYPE& adata)	\
	{																					\
		std::shared_ptr<mforward<TYPE>> pro(new mforward<TYPE>(id_guid(), adata));		\
		if(ISCROSSFUN(adata))															\
		{																				\
			send_actor(																	\
				actor_guid::make(														\
					MODULE																\
					, ttab_servers::tab()->m_crossarea									\
					, actor_guid::none_actordataid()									\
				), pro);																\
		}																				\
		else																			\
		{																				\
			send_actor(																	\
				actor_guid::make(														\
					MODULE																\
					, ttab_servers::tab()->m_area										\
					, actor_guid::none_actordataid()									\
				), pro);																\
		}																				\
		return true;																	\
	}
		// 模块转发  
		// foward_module(区服内部模块转发)
		foward_module(ACTOR_NOTICE, pbnet::PROBUFF_NET_GET_NOTICE)
		foward_module(ACTOR_MAIL, pbnet::PROBUFF_NET_MAIL_LIST)
		foward_module(ACTOR_MAIL, pbnet::PROBUFF_NET_MAIL_READ)
		foward_module(ACTOR_MAIL, pbnet::PROBUFF_NET_MAIL_DRAW)
		foward_module(ACTOR_MAIL, pbnet::PROBUFF_NET_MAIL_DEL)

		// foward_cross_module(跨服模块转发)
		bool is_cross(pbnet::PROBUFF_NET_CHAT& adata)
		{
			return adata.m_channelid() == 2;
		}
		foward_cross_module(ACTOR_CHAT, pbnet::PROBUFF_NET_CHAT, is_cross)

		// CMD 协议
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_CMD& adata);

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_ROLE_SYNC& adata);

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_GET_TIME& adata);

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_SWITCH_LINE& adata);

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_send_item& adata);

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_disconnect_close& adata);
		
		// 定时器
		bool timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata);
	private:
	};

	


}

