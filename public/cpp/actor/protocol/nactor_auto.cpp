/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/

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
		return (ENUM_ACTOR)(ACTOR_DB + TDBTAB_TYPE);
	}

	template <pbdb::ENUM_DB DBTYPE, typename TDB>
	void init_customs_db()
	{
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
			db_actor::instance();
		}
	}

	void auto_actor()
	{
		auto_actor_enum();
		autoactor::func<actor_events_logic, actor_events_map>(
			{ em_events_null(actor_events_logic), em_events_null(actor_events_map) }
		);
	}

	void tprotocol_customs()
	{
		tprotocol_customs_200000000();

		tprotocol::set_customs_index(100000000);
		// 会注册T与np_mass_actor<T>
		tprotocol::tp_customs_script<
			/*100000001*/np_gm
			/*100000003*/, np_gm_response
		>();

		// 占用110000000->120000000
		register_channel_db();
		
		tprotocol::set_customs_index(120000000);
		tprotocol::tp_customs_script<
			/*120000001*/ mforward<np_gm>
			/*120000003*/, mforward<np_gm_response>
			/*120000005*/, np_actorswitch_process<np_actorswitch_process_role>
			/*120000007*/, np_testlua
		>();

		// ### 事件相关协议 start ### //
		tprotocol::set_customs_index(130000000);
		tprotocol::tp_customs_script<
			/*130000001*/ actor_events_logic::np_event_register
			/*130000003*/, actor_events_map::np_event_register
		>();

		//# actor_events_logic
		tprotocol::set_customs_index(130010000);
		tprotocol::tp_customs_script<
			/*130010001*/ np_eevents_logic_rolelogin
			/*130010003*/, np_eevents_logic_roleoffline
			/*130010005*/, np_eevents_logic_rolevaluechange
		>();

		//# actor_events_map
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