#include "init_protobuf.h"
#include "gm.pb.h"
#include "net.pb.h"
#include "actor_protocol.h"
#include "actor_timer.h"
#include "actor.h"

namespace ngl
{
	std::multimap<size_t, init_protobuf::pinfo> init_protobuf::m_keyval;
	int32_t init_protobuf::lcustoms = 200000000;

	void init_protobuf::initall()
	{
		init_customs();
	}

	template <ENUM_DB DBTYPE, typename TDB>
	void init_customs_db()
	{
		init_protobuf::init_customs(
			EPROTOCOL_TYPE_PROTOCOLBUFF
			, null<actor_db_load<EPROTOCOL_TYPE_PROTOCOLBUFF, DBTYPE, TDB>>
			, null<actor_db_load_response<EPROTOCOL_TYPE_PROTOCOLBUFF, DBTYPE, TDB>>
			, null<actor_db_save<EPROTOCOL_TYPE_PROTOCOLBUFF, DBTYPE, TDB>>
			, null<actor_db_delete<EPROTOCOL_TYPE_PROTOCOLBUFF, DBTYPE, TDB>>
			, null<actor_time_db_cache<EPROTOCOL_TYPE_PROTOCOLBUFF, TDB>>
		);
	}

	void init_protobuf::init_customs()
	{
		init_customs(EPROTOCOL_TYPE_CUSTOM
			, null<timerparm>
			, null<robot_pram>
			, null<actor_node_register>
			, null<actor_node_register_response>
			, null<actor_client_node_connect>
			, null<actor_node_update>
			, null<actor_node_update_mass>
			, null<actor_node_actor_connect_task>
			, null<actor_role_login>
			, null<actor_server_connect>
			, null<actor_session_close>
			, null<actor_disconnect_close>
			, null<actor_logitempro>
			, null<actor_broadcast>
			, null<actor_reloadcsv_pro>
			, null<actor_reloadcsv_verify_version>
			, null<actor_send_item>
			, null<actor_roleinfo>
			, null<actor_gateway_info_updata>
			, null<actor_protocol_add_mail>
			, null<actor_protocol_open_activity>
			, null<actor_protocol_close_activity>
			, null<actor_gateway_id_updata>
			, null<actor_switch_process<actor_switch_process_role>>
		);

		init_customs_db<ENUM_DB_ACCOUNT, pbdb::db_account>();
		init_customs_db<ENUM_DB_ROLE, pbdb::db_role>();
		init_customs_db<ENUM_DB_BAG, pbdb::db_bag>();
		init_customs_db<ENUM_DB_KEYVALUE, pbdb::db_keyvalue>();
		init_customs_db<ENUM_DB_MAIL, pbdb::db_mail>();
		init_customs_db<ENUM_DB_GUILD, pbdb::db_guild>();
		init_customs_db<ENUM_DB_NOTICE, pbdb::db_notice>();
		init_customs_db<ENUM_DB_ACTIVITY, pbdb::db_activity>();
		init_customs_db<ENUM_DB_BRIEF, pbdb::db_brief>();
		init_customs_db<ENUM_DB_RANKLIST, pbdb::db_ranklist>();
	}
}