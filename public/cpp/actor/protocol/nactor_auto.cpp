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
// File overview: Implements logic for protocol.


#include "actor/actor_logic/actor_events/events_logic.h"
#include "actor/actor_logic/actor_events/events_map.h"
#include "actor/actor_logic/actor_kcp/actor_kcp.h"
#include "actor/auto/register_channel_db.h"
#include "actor/actor_base/nactortype.h"
#include "actor/auto/auto_actor_enum.h"
#include "actor/auto/nprotocol_auto.h"
#include "actor/actor_base/actor_db.h"
#include "actor/protocol/tprotocol.h"
#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/ntimer.h"
#include "actor/auto/nactor_auto.h"
#include "actor/auto/nactor_auto.h"
#include "actor/actor_base/actor.h"
#include "tools/enum2name.h"
#include "actor/pb/net.pb.h"

namespace ngl
{
	ENUM_ACTOR db_enum(pbdb::ENUM_DB TDBTAB_TYPE)
	{
		// DB actors are laid out contiguously after ACTOR_DB.
		return static_cast<ENUM_ACTOR>(static_cast<int>(ACTOR_DB) + static_cast<int>(TDBTAB_TYPE));
	}

	template <pbdb::ENUM_DB DBTYPE, typename TDB>
	void init_customs_db()
	{
		// Each DB table gets a load / response / save / delete / cache protocol family.
		tprotocol::tp_customs<
			np_actordb_load<DBTYPE, TDB>
			, np_actordb_load_response<DBTYPE, TDB>
			, np_actordb_save<DBTYPE, TDB>
			, np_actordb_delete<DBTYPE, TDB>
			, np_actortime_db_cache<TDB>
		>(-1, 1);
	}

	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB, typename TACTOR>
	void typedb<TDBTAB_TYPE, TDBTAB, TACTOR>::init(bool ainit)
	{
		if (ainit)
		{
			// First phase: define actor type metadata and protocol ids.
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
			// Second phase: force singleton initialization of the DB actor registration holder.
			db_actor::instance();
		}
	}

	void auto_actor()
	{
		// Generated actor enum/type registration lives in auto_*.cpp outputs.
		auto_actor_enum();
		autoactor::func<actor_events_logic, actor_events_map>(
			{ em_events_null(actor_events_logic), em_events_null(actor_events_map) }
		);
	}

	void tprotocol_customs()
	{
		// Generated custom protocols reserve the 200000000 range.
		tprotocol_customs_200000000();

		tprotocol::set_customs_index(100000000);
		// Register script-visible GM protocols and their np_mass_actor<T> wrappers.
		tprotocol::tp_customs_script<
			/*100000001*/np_gm
			/*100000003*/, np_gm_response
		>();

		// Register channel/DB helper protocols emitted by code generation.
		register_channel_db();
		
		tprotocol::set_customs_index(120000000);
		// Module-forwarding and process-switch helpers use a dedicated custom range.
		tprotocol::tp_customs_script<
			/*120000001*/ mforward<np_gm>
			/*120000003*/, mforward<np_gm_response>
			/*120000005*/, np_actorswitch_process<np_actorswitch_process_role>
			/*120000007*/, np_testlua
		>();

		// Event bus protocols are grouped into their own custom ranges.
		tprotocol::set_customs_index(130000000);
		tprotocol::tp_customs_script<
			/*130000001*/ actor_events_logic::np_event_register
			/*130000003*/, actor_events_map::np_event_register
		>();

		// actor_events_logic event payloads.
		tprotocol::set_customs_index(130010000);
		tprotocol::tp_customs_script<
			/*130010001*/ np_eevents_logic_rolelogin
			/*130010003*/, np_eevents_logic_roleoffline
			/*130010005*/, np_eevents_logic_rolevaluechange
		>();

		// actor_events_map event payloads.
		tprotocol::set_customs_index(130020000);
		tprotocol::tp_customs_script<
			/*130020001*/ np_eevents_map_leaveview
			/*130020003*/, np_eevents_map_enterview
		>();
	}

	void event_register()
	{
		actor_events_logic::register_parm<np_eevents_logic_rolelogin>(eevents_logic_rolelogin);
		actor_events_logic::register_parm<np_eevents_logic_roleoffline>(eevents_logic_roleoffline);
		actor_events_logic::register_parm<np_eevents_logic_rolevaluechange>(eevents_logic_rolelevelchange);
		actor_events_logic::register_parm<np_eevents_logic_rolevaluechange>(eevents_logic_rolegoldchange);
	}

	void tdb::tcrossdb_init(bool ainstance)
	{
		tdb_ranklist::init(ainstance);
	}
}//namespace ngl
