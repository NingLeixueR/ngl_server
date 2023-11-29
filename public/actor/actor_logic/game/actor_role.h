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
#include "actor_create.h"
#include "net.pb.h"

#include <boost/shared_ptr.hpp>

#include "bag.h"
#include "roleinfo.h"
#include "remakes.h"
#include "autoitem.h"
#include "task.h"
#include "attribute.h"

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
		attribute	 m_attribute;
		i32_serverid m_gatewayid;
		i64_actorid  m_playactorid;
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

		// #### 设置更新角色属性
		void update_attribute(std::pair<EnumModule, attribute_value>& apair)
		{
			m_attribute.updata(apair.first, apair.second);

		}

		template <typename T>
		void send2client(std::shared_ptr<T>& adata)
		{
			actor_base::send_client(m_gatewayid, id_guid(), adata);
		}

		void sync_data_client();

		enum ecross
		{
			ecross_ordinary,			// 本服转发
			ecross_cross_ordinary,		// 跨服转发
			ecross_play,				// 玩法转发(但是转发的actorid已确认)
		};

		// 重载(跨服模块转发)
		template <typename T>
		ecross get_cross(T& adata)
		{
			return ecross_ordinary;
		}

		ecross get_cross(pbnet::PROBUFF_NET_CHAT& adata)
		{
			return adata.m_channelid() == 2 ? ecross_cross_ordinary : ecross_ordinary;
		}

		template <typename T>
		int32_t dataid(T& adata)
		{
			return actor_guid::none_actordataid();
		}

		template <ENUM_ACTOR ACTOR, typename T>
		bool handle_forward(i32_threadid athread, const std::shared_ptr<pack>& apack, T& adata)
		{
			std::shared_ptr<mforward<T>> pro(new mforward<T>(id_guid(), adata));
			i64_actorid lguid;
			switch (get_cross(adata))
			{
			case ecross_ordinary:
				lguid = actor_guid::make(ACTOR, ttab_servers::tab()->m_crossarea, dataid(adata));
				break;
			case ecross_cross_ordinary:
				lguid = actor_guid::make(ACTOR, ttab_servers::tab()->m_area, dataid(adata));
				break;
			case ecross_play:
				lguid = m_playactorid;
				break; 
			default:
				return true;
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
		
		//玩法创建成功  记录玩法actorid
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE& adata);
		
		// 定时器
		bool timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata);
	private:
	};

	


}

