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
		remakes		 m_remakes;
		roleinfo	 m_info;
		bag			 m_bag;
		task		 m_task;
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

		// 重载(跨服模块转发)
		template <typename T>
		bool is_cross(T& adata)
		{
			return false;
		}

		bool is_cross(pbnet::PROBUFF_NET_CHAT& adata)
		{
			return adata.m_channelid() == 2;
		}

		bool dataid(pbnet::PROBUFF_NET_CHAT& adata)
		{
			return actor_guid::none_actordataid();
		}

		template <ENUM_ACTOR ACTOR, typename T>
		bool handle_forward(i32_threadid athread, const std::shared_ptr<pack>& apack, T& adata)
		{
			std::shared_ptr<mforward<T>> pro(new mforward<T>(id_guid(), adata));
			i64_actorid lguid;
			if (is_cross(adata))
			{
				lguid = actor_guid::make(ACTOR, ttab_servers::tab()->m_crossarea, dataid(adata));
			}
			else
			{
				lguid = actor_guid::make(ACTOR, ttab_servers::tab()->m_area, dataid(adata));
			}
			send_actor(lguid, pro);
			return true;
		}

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

