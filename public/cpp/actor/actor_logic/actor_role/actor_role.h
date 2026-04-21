/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Player role actor — one instance per online player, owns all per-player state and logic sub-modules.

#pragma once

#include "actor/actor_logic/actor_role/logic/attribute/attribute.h"
#include "actor/actor_logic/actor_role/logic/bag/autoitem.h"
#include "actor/actor_logic/actor_role/logic/task/task.h"
#include "actor/actor_logic/actor_create/actor_create.h"
#include "actor/actor_logic/actor_role/logic/roleinfo.h"
#include "actor/actor_logic/actor_events/events_logic.h"
#include "actor/actor_logic/actor_role/logic/bag/drop.h"
#include "actor/actor_logic/actor_role/logic/remakes.h"
#include "actor/actor_logic/actor_role/logic/bag/bag.h"
#include "actor/actor_logic/actor_role/logic/rolekv.h"
#include "actor/actor_base/core/actor_manage.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "actor/actor_base/core/ndbclient.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/generated/pb/example.pb.h"
#include "actor/actor_base/core/ntimer.h"
#include "actor/generated/pb/net.pb.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/protocol/nprotocol.h"
#include "tools/tools/tools_curl.h"
#include "tools/tools/tools_cmd.h"
#include "tools/db/sql/db_data.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	enum ecross
	{
		ecross_ordinary			= 1,		// Forwarding
		ecross_cross_ordinary	= 2,		// Cross-serverforwarding
		ecross_cross_example	= 3,		// Forwarding
		ecross_none				= 4,		// Forwarding
	};

	struct prorechange
	{
		std::string m_orderid;
		int32_t		m_rechargeid = 0;
		int64_t		m_roleid = 0;

		DPROTOCOL(prorechange, m_orderid, m_rechargeid, m_roleid)
	};

	class actor_role : 
		public actor
	{
	public:
		remakes				m_remakes;
		roleinfo			m_info;
		bag					m_bag;
		task				m_task;
		rolekv				m_rolekv;
		attribute			m_attribute;
		i32_serverid		m_gatewayid;
		drop<actor_role>	m_drop;
		std::pair<pbexample::EPLAY_TYPE, i64_actorid> m_example;
	public:
		actor_role(const actor_role&) = delete;
		actor_role& operator=(const actor_role&) = delete;

		actor_role(i16_area aarea, i32_actordataid aroleid, void* adata);

		virtual ~actor_role();

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(int32_t adata);

		virtual void init(); 

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		void login_finish();

		virtual i32_serverid get_getwayserverid();

		// Execute post-handle logic
		virtual void handle_after(handle_pram&);

		virtual void erase_actor_before();

		i64_actorid roleid();

		drop<actor_role>& get_drop()
		{
			return m_drop;
		}

		// Set role attribute for a given module
		void update_attribute(EnumModule amodule, attribute_value& avalue);

		void sync_data_client();

#pragma region forward // Forwardingrelated
		
		// Default forwarding type for a message
		template <typename T>
			ecross forward_type([[maybe_unused]] const T& adata)
		{
			return ecross_ordinary;
		}

		// Chat message forwarding type
		ecross forward_type(const pbnet::PROBUFF_NET_CHAT& adata)
		{
			if (m_info.bantalk())
			{
				return ecross_none;
			}
			switch (adata.mchannelid())
			{
			case pbnet::ENUM_CHAT_CHANNEL::ENUM_CHAT_CHANNEL_ORDINARY:
				return ecross_ordinary;
			case pbnet::ENUM_CHAT_CHANNEL::ENUM_CHAT_CHANNEL_CROSS_ORDINARY:
				return ecross_cross_ordinary;
			case pbnet::ENUM_CHAT_CHANNEL::ENUM_CHAT_CHANNEL_NONE:
				return ecross_none;
			}
			return ecross_none;
		}

		// Ranklist forwarding type
		ecross forward_type(const pbnet::PROBUFF_NET_RANKLIST& adata)
		{
			return adata.miscross() ? ecross_cross_ordinary : ecross_ordinary;
		}

		// Join-example forwarding type
		ecross forward_type(const pbexample::PROBUFF_EXAMPLE_PLAY_JOIN& adata)
		{
			if (m_example.second != 0)
			{
				return ecross_none;
			}
			return tools::equal(adata.mcross(), pbexample::ECROSS_CROSS_ORDINARY) ? ecross_cross_ordinary : ecross_ordinary;
		}

		ecross forward_type(const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE& adata)
		{
			return tools::equal(adata.mcross(), pbexample::ECROSS_CROSS_ORDINARY) ? ecross_cross_ordinary : ecross_ordinary;
		}

		// Get the example forwarding type
		ecross example_type()
		{
			if (m_example.second == nguid::make())
			{
				return ecross_none;
			}
			else
			{
				return ecross_cross_example;
			}
		}

			ecross forward_type([[maybe_unused]] const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER& adata)
			{
				return example_type();
			}

		// Pre-forward hook for specific message types
		template <typename T>
			bool forward_before([[maybe_unused]] const T& adata)
			{
				return true;
			}

		// Pre-forward check before creating a guild
		bool forward_before(const pbnet::PROBUFF_NET_FAMIL_CREATE& adata);

		// Get the forwarding target data ID for a message
		template <typename T>
			int32_t forward_dataid([[maybe_unused]] const T& adata)
			{
				return nguid::none_actordataid();
			}

		template <typename T>
		nguid forward_guid(ENUM_ACTOR aactor, ecross atype, nguid& aguid, const T& adata)
		{
			if (atype == ecross_ordinary)
			{
				aguid = nguid::make(aactor, tab_self_area, forward_dataid(adata));
			}
			else if (atype == ecross_cross_ordinary)
			{
				aguid = nguid::make(aactor, tab_self_cros_area, forward_dataid(adata));
			}
			else if (atype == ecross_cross_example)
			{
				aguid = m_example.second;
			}
			else
			{
				return false;
			}
			return true;
		}

		template <ENUM_ACTOR ACTOR, typename T>
		bool handle_forward(const message<T>& adata)
		{
			const T* lparm = adata.get_data();
			if (lparm == nullptr)
			{
				return false;
			}
			if (forward_before(*lparm) == false)
			{
				return false;
			}
			std::shared_ptr<mforward<T>> pro(nullptr);
			pro = std::make_shared<mforward<T>>(id_guid(), *lparm);
			nguid lguid;
			if (forward_guid<T>(ACTOR, forward_type(*lparm), lguid, *lparm))
			{
				actor::send_actor(lguid, id_guid(), pro);
			}

			return true;
		}
#pragma endregion

		// Reset login time
		void reset_logintime();

		// Send a string message to the client
		void echo_msg(const char* amsg);

		int32_t rechange(std::string& aorderid, int32_t arechargeid, bool agm, bool areporting);

		// Request pending payments on login
		void loginpay();

		void requestgm(const char* aurl, const std::string& aparm, const std::function<void(int32_t, tools::http_parm&)>& acall);

		// Create a recharge order
		void createorder(std::string& aorder, int32_t arechargeid);

		// Check whether this is the first recharge
		bool is_first_recharge(int32_t arechargeid);

		// CMD protocol handler
		using handle_cmd = tools::cmd<actor_role, std::string, const std::shared_ptr<pack>&, actor_role*, const char*>;
		// GM protocol handler
		using handle_gm = tools::cmd<actor_role, std::string, int, ncjson&>;

		// Timer callback handler
		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		// Initialize a newly created role
		void create_init(const std::string& aname);

		bool handle([[maybe_unused]] const message<mforward<np_gm>>& adata);
		bool handle([[maybe_unused]] const message<mforward<np_operator_task>>& adata);
		bool handle([[maybe_unused]] const message<np_actor_disconnect_close>& adata);
		bool handle([[maybe_unused]] const message<np_actor_senditem>& adata);
		bool handle([[maybe_unused]] const message<np_eevents_logic_rolelogin>& adata);
		bool handle([[maybe_unused]] const message<np_example_actorid>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_BAG_SYNC>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_CHANGE_ANGLE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_CMD>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_GET_TIME>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_RECHARGE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ROLE_CREATE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ROLE_SYNC>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_SWITCH_LINE>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>& adata);
	};
}//namespace ngl
