#pragma once

#include "actor/actor_logic/actor_client/actor_client.h"
#include "tools/db/sql/postgresql/npostgresql_manage.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "actor/protocol/nprotocol.h"
#include "tools/log/nlog.h"

#include <string_view>
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

enum class startup_error : int
{
	ok = 0,
	invalid_args = 2,
	invalid_node_type = 3,
	config_not_found = 4,
	config_load_failed = 5,
	tab_server_missing = 6,
	net_config_missing = 7,
	init_server_failed = 8,
	node_start_failed = 9,
};

struct node_boot_opt
{
	// Optional node-specific startup toggles used by start_node().
	std::set<pbnet::ENUM_KCP> m_kcp_types;
	bool m_use_actor_client = true;
	bool m_make_log = true;
	bool m_reg_actor = true;
};

startup_error init_server([[maybe_unused]] int aid, const std::set<pbnet::ENUM_KCP>& akcp = {}, int* atcp_port = nullptr);

void make_log_actor();

node_boot_opt make_node_opt(
	std::initializer_list<pbnet::ENUM_KCP> akcp_types = {},
	bool ause_actor = true,
	bool amake_log = true,
	bool areg_actor = true
);

template <typename tsetup>
startup_error start_node(const char* anode_name, int* atcp_port, const node_boot_opt& aopts, tsetup&& asetup)
{
	ngl::log_error()->print("[{}] start", anode_name);

	// init_server wires up protocols, networking and the actor scheduler.
	startup_error lrc = init_server(nconfig.nodeid(), aopts.m_kcp_types, atcp_port);
	if (lrc != startup_error::ok)
	{
		return lrc;
	}

	if (aopts.m_use_actor_client)
	{
		ngl::actor_client::instance();
	}

	if (aopts.m_make_log)
	{
		make_log_actor();
	}

	using lsetup_ret = std::invoke_result_t<tsetup&>;
	if constexpr (std::is_same_v<lsetup_ret, bool>)
	{
		if (!asetup())
		{
			return startup_error::node_start_failed;
		}
	}
	else
	{
		asetup();
	}

	if (aopts.m_reg_actor && aopts.m_use_actor_client)
	{
		// Once all singleton actors exist locally, publish them to the route layer.
		ngl::actor_client::instance().actor_server_register();
	}

	return startup_error::ok;
}

template <typename tdb_cfg>
bool init_db(const tdb_cfg& adb_cfg)
{
	if (nconfig.dbedb() == ngl::xarg_db::edb_mysql)
	{
		return ngl::nmysql_manage::init() && ngl::nmysql_pool::instance().init(adb_cfg);
	}
	if (nconfig.dbedb() == ngl::xarg_db::edb_postgresql)
	{
		return ngl::npostgresql_manage::init() && ngl::npostgresql_pool::instance().init(adb_cfg);
	}
	return false;
}
