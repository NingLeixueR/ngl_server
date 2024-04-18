#include "initproto.h"
#include "nprotocol.h"
#include "ntimer.h"
#include "net.pb.h"
#include "actor.h"

namespace ngl
{
	std::multimap<size_t, initproto::pinfo> initproto::m_keyval;
	int32_t initproto::lcustoms = 200000000;

	void initproto::initall()
	{
		init_customs();
	}

	template <pbdb::ENUM_DB DBTYPE, typename TDB>
	void init_customs_db()
	{
		initproto::init_customs(
			EPROTOCOL_TYPE_PROTOCOLBUFF
			, null<np_actordb_load<EPROTOCOL_TYPE_PROTOCOLBUFF, DBTYPE, TDB>>
			, null<np_actordb_load_response<EPROTOCOL_TYPE_PROTOCOLBUFF, DBTYPE, TDB>>
			, null<np_actordb_save<EPROTOCOL_TYPE_PROTOCOLBUFF, DBTYPE, TDB>>
			, null<np_actordb_delete<EPROTOCOL_TYPE_PROTOCOLBUFF, DBTYPE, TDB>>
			, null<np_actortime_db_cache<EPROTOCOL_TYPE_PROTOCOLBUFF, TDB>>
		);
	}

	void initproto::init_customs()
	{
		init_customs(EPROTOCOL_TYPE_CUSTOM
			, null<np_gm>				// 200000001
			, null<np_gm_response>		// 200000002
			, null<mforward<np_gm>>			// 200000003
			, null<mforward<np_gm_response>>  // 200000004
			, null<timerparm>
			, null<np_robot_pram>
			, null<np_actornode_register>
			, null<np_actornode_register_response>
			, null<np_actorclient_node_connect>
			, null<np_actornode_update>
			, null<np_actornode_update_mass>
			, null<np_actornode_connect_task>
			, null<np_actorrole_login>
			, null<np_actorserver_connect>
			, null<np_actor_session_close>
			, null<np_actor_disconnect_close>
			, null<np_actor_logitem>
			, null<np_actor_broadcast>
			, null<np_actor_reloadcsv>
			, null<np_actor_csv_verify_version>
			, null<np_actor_senditem>
			, null<np_actor_roleinfo>
			, null<np_actor_gatewayinfo_updata>
			, null<np_actor_addmail>
			, null<np_actor_activity>
			, null<np_actor_gatewayid_updata>
			, null<np_actorswitch_process<np_actorswitch_process_role>>
			, null<np_actor_kcp>
		);

		init_customs_db<pbdb::ENUM_DB_ACCOUNT, pbdb::db_account>();
		init_customs_db<pbdb::ENUM_DB_ROLE, pbdb::db_role>();
		init_customs_db<pbdb::ENUM_DB_BAG, pbdb::db_bag>();
		init_customs_db<pbdb::ENUM_DB_KEYVALUE, pbdb::db_keyvalue>();
		init_customs_db<pbdb::ENUM_DB_MAIL, pbdb::db_mail>();
		init_customs_db<pbdb::ENUM_DB_GUILD, pbdb::db_guild>();
		init_customs_db<pbdb::ENUM_DB_NOTICE, pbdb::db_notice>();
		init_customs_db<pbdb::ENUM_DB_ACTIVITY, pbdb::db_activity>();
		init_customs_db<pbdb::ENUM_DB_BRIEF, pbdb::db_brief>();
		init_customs_db<pbdb::ENUM_DB_RANKLIST, pbdb::db_ranklist>();
		init_customs_db<pbdb::ENUM_DB_TASK, pbdb::db_task>();
	}
}