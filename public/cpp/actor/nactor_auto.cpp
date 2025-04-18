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

		tprotocol::set_customs_index(210000000);
		tprotocol::tp_customs::template func <
			/*210000001*/ nactor_logitem
			/*210000002*/, np_channel_register<pbdb::db_brief>
			/*210000003*/, np_channel_register_reply<pbdb::db_brief>
			/*210000004*/, np_channel_data<pbdb::db_brief>
			/*210000005*/, np_channel_exit<pbdb::db_brief>
			/*210000006*/, np_channel_register<pbdb::db_keyvalue>
			/*210000007*/, np_channel_register_reply<pbdb::db_keyvalue>
			/*210000008*/, np_channel_data<pbdb::db_keyvalue>
			/*210000009*/, np_channel_exit<pbdb::db_keyvalue>
			/*210000010*/, np_arg_null
		>(EPROTOCOL_TYPE_CUSTOM);
		
		tprotocol::set_customs_index(220000000);
		tprotocol::tp_customs::template func <
			/*220000001*/mforward<np_gm>
			/*220000002*/, mforward<np_gm_response>
			/*220000003*/, np_actorswitch_process<np_actorswitch_process_role>
		>(EPROTOCOL_TYPE_CUSTOM);
		
		tprotocol::set_customs_index(220000000);
		tprotocol::tp_customs::template func <
			// ### 事件相关协议 start ### //
			/*250000001*/ actor_events_logic::np_event_register
			//# actor_events_logic
			/*250000002*/, np_eevents_logic_rolelogin
			/*250000003*/, np_eevents_logic_roleoffline
			//# actor_events_map
			/*250000004*/, np_eevents_map_leaveview
			/*250000005*/, np_eevents_map_enterview
			// ### 事件相关协议 finish ### //
		>(EPROTOCOL_TYPE_CUSTOM);
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

	//void tdb_init(bool ainstance)
	//{
	//	// 新增数据存储需要补全
	//	tdb_ranklist::init(ainstance);
	//	tdb_calendar::init(ainstance);
	//	tdb_activity::init(ainstance);
	//	tdb_keyvalue::init(ainstance);
	//	tdb_familyer::init(ainstance);
	//	tdb_friends::init(ainstance);
	//	tdb_account::init(ainstance);
	//	tdb_family::init(ainstance);
	//	tdb_notice::init(ainstance);
	//	tdb_rolekv::init(ainstance);
	//	tdb_brief::init(ainstance);
	//	tdb_mail::init(ainstance);
	//	tdb_task::init(ainstance);
	//	tdb_role::init(ainstance);
	//	tdb_bag::init(ainstance);
	//}

	void tdb::tcrossdb_init(bool ainstance)
	{
		tdb_ranklist::init(ainstance);
	}
}//namespace ngl