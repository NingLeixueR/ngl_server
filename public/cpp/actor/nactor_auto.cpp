#include "events_logic.h"
#include "nactor_auto.h"
#include "events_map.h"
#include "nactortype.h"
#include "enum2name.h"
#include "actor_kcp.h"
#include "tprotocol.h"
#include "nprotocol.h"
#include "actor_db.h"
#include "ntimer.h"
#include "net.pb.h"
#include "actor.h"

#include "auto_actor_enum.h"
#include "nprotocol_auto.h"
#include "nactor_auto.h"

namespace ngl
{
	ENUM_ACTOR db_enum(pbdb::ENUM_DB TDBTAB_TYPE)
	{
		return (ENUM_ACTOR)(EPROTOCOL_TYPE_PROTOCOLBUFF * 1000 + ACTOR_DB + TDBTAB_TYPE);
	}

	template <pbdb::ENUM_DB DBTYPE, typename TDB>
	void init_customs_db()
	{
		tprotocol::tp_customs::template func<
			np_actordb_load<DBTYPE, TDB>
			, np_actordb_load_response<DBTYPE, TDB>
			, np_actordb_save<DBTYPE, TDB>
			, np_actordb_delete<DBTYPE, TDB>
			, np_actortime_db_cache<TDB>
		>(EPROTOCOL_TYPE_PROTOCOLBUFF);
	}

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB, typename TACTOR>
	void typedb<TDBTAB_TYPE, TDBTAB, TACTOR>::init(bool ainit)
	{
		if (ainit == true)
		{
			using type_actor_db = ngl::actor_db<TDBTAB_TYPE, TDBTAB>;
			ENUM_ACTOR lenum = db_enum(TDBTAB_TYPE);
			nactor_type<type_actor_db>::inits(lenum);
			std::string ldbname("actor_");
			std::string ltemp = tools::type_name<TDBTAB>();
			if (auto pos = ltemp.rfind(":"); pos != std::string_view::npos)
			{
				ldbname += ltemp.substr(pos + 1);
			}
			else
			{
				ldbname += ltemp;
			}
			em<ENUM_ACTOR>::set(lenum, ldbname.c_str());
			init_customs_db<TDBTAB_TYPE, TDBTAB>();
		}
		else
		{
			db_actor::getInstance();
		}
	}

	void auto_actor()
	{
		auto_actor_enum();
		_auto_actor(
			em_events_null(actor_events_logic)
			, em_events_null(actor_events_map)
		);
	}

	void tprotocol_customs()
	{
		tprotocol_customs_200000000();

		tprotocol::set_customs_index(220000000);
		tprotocol::tp_customs::template func <
			/*220000001*/np_gm
			/*220000002*/, np_gm_response
			/*220000003*/, nactor_logitem
			/*220000004*/, np_channel_register<pbdb::db_brief>
			/*220000005*/, np_channel_register_reply<pbdb::db_brief>
			/*220000006*/, np_channel_data<pbdb::db_brief>
			/*220000007*/, np_channel_exit<pbdb::db_brief>
			/*220000008*/, np_channel_register<pbdb::db_keyvalue>
			/*220000009*/, np_channel_register_reply<pbdb::db_keyvalue>
			/*220000010*/, np_channel_data<pbdb::db_keyvalue>
			/*220000011*/, np_channel_exit<pbdb::db_keyvalue>
			/*220000012*/, np_arg_null
		>(EPROTOCOL_TYPE_CUSTOM);
		
		tprotocol::set_customs_index(230000000);
		tprotocol::tp_customs::template func <
			/*230000001*/mforward<np_gm>
			/*230000002*/, mforward<np_gm_response>
			/*230000003*/, np_actorswitch_process<np_actorswitch_process_role>
		>(EPROTOCOL_TYPE_CUSTOM);

		// ### 事件相关协议 start ### //
		tprotocol::set_customs_index(240000000);
		tprotocol::tp_customs::template func <
			/*240000001*/ actor_events_logic::np_event_register
		>(EPROTOCOL_TYPE_CUSTOM);

		//# actor_events_logic
		tprotocol::set_customs_index(240001000);
		tprotocol::tp_customs::template func <
			/*240001001*/ np_eevents_logic_rolelogin
			/*240001002*/, np_eevents_logic_roleoffline
			/*240001003*/, np_eevents_logic_rolelevelchange
		>(EPROTOCOL_TYPE_CUSTOM);

		//# actor_events_map
		tprotocol::set_customs_index(240002000);
		tprotocol::tp_customs::template func <
			/*240002001*/ np_eevents_map_leaveview
			/*240002002*/, np_eevents_map_enterview
		>(EPROTOCOL_TYPE_CUSTOM);
		// ### 事件相关协议 finish ### //
	}

	template <typename PB>
	using type_forward_pb = ngl::np_actor_forward<PB, EPROTOCOL_TYPE_PROTOCOLBUFF, true, ngl::forward>;

	template <typename PB>
	void help_tprotocol_forward_pb(const PB*)
	{
		tprotocol::tp_customs::template func<
			ngl::np_actor_forward<PB, EPROTOCOL_TYPE_PROTOCOLBUFF, true, ngl::forward>
			, ngl::np_actor_forward<PB, EPROTOCOL_TYPE_PROTOCOLBUFF, false, ngl::forward>
			, ngl::np_actor_forward<PB, EPROTOCOL_TYPE_PROTOCOLBUFF, true, PB>
			, ngl::np_actor_forward<PB, EPROTOCOL_TYPE_PROTOCOLBUFF, false, PB>
		>(EPROTOCOL_TYPE_PROTOCOLBUFF);
	}

	template <typename PB, typename ...ARG>
	void help_tprotocol_forward_pb(const PB* apb, const ARG*... arg)
	{
		help_tprotocol_forward_pb<PB>(apb);
		help_tprotocol_forward_pb<ARG...>(arg...);
	}

	void event_register()
	{
		actor_events_logic::register_parm<np_eevents_logic_rolelogin>(eevents_logic_rolelogin);
		actor_events_logic::register_parm<np_eevents_logic_roleoffline>(eevents_logic_roleoffline);
	}

	void tdb::tcrossdb_init(bool ainstance)
	{
		tdb_ranklist::init(ainstance);
	}
}//namespace ngl