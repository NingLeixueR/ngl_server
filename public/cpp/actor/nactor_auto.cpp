#include "actor_example_guess_number.h"
#include "actor_manage_activity.h"
#include "actor_example_manage.h"
#include "actor_example_match.h"
#include "actor_manage_role.h"
#include "actor_gatewayc2g.h"
#include "actor_gatewayg2c.h"
#include "actor_csvserver.h"
#include "actor_csvclient.h"
#include "actor_ranklist.h"
#include "actor_calendar.h"
#include "actor_gmclient.h"
#include "actor_gateway.h"
#include "actor_events.h"
#include "actor_notice.h"
#include "actor_server.h"
#include "actor_client.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "events_logic.h"
#include "nactor_auto.h"
#include "actor_login.h"
#include "actor_robot.h"
#include "actor_brief.h"
#include "actor_cross.h"
#include "events_map.h"
#include "actor_chat.h"
#include "actor_mail.h"
#include "actor_role.h"
#include "nactortype.h"
#include "enum2name.h"
#include "actor_kcp.h"
#include "actor_log.h"
#include "tprotocol.h"
#include "nprotocol.h"
#include "actor_db.h"
#include "actor_gm.h"
#include "ntimer.h"
#include "net.pb.h"
#include "actor.h"

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

	template <typename TACTOR>
	void _auto_actor(const TACTOR* aactor, ENUM_ACTOR aenum, const char* aname)
	{
		em<ENUM_ACTOR>::set(aenum, aname);
		nactor_type<TACTOR>::inits(aenum);
	}

	template <typename TACTOR, typename ...ARG>
	void _auto_actor(const TACTOR* aactor, ENUM_ACTOR aenum, const char* aname, const ARG&... arg)
	{
		_auto_actor<TACTOR>(aactor, aenum, aname);
		_auto_actor(arg...);
	}

	void auto_actor()
	{
#define em_events_null(NAME) null<NAME>,(ENUM_ACTOR)(ACTOR_EVENTS+ NAME::id_index()), #NAME

#define dautoactor(NAME, DEF) null<NAME>, em_pram(DEF)
		// ### 新增actor需要补全
		_auto_actor(
			dautoactor(actor_client, ACTOR_CLIENT)
			, dautoactor(actor_create, ACTOR_CREATE)
			, dautoactor(actor_server, ACTOR_SERVER)
			, dautoactor(actor_cross, ACTOR_CROSS)
			, dautoactor(actor_manage_role, ACTOR_MANAGE_ROLE)
			, dautoactor(actor_role, ACTOR_ROLE)
			, dautoactor(actor_gateway, ACTOR_GATEWAY)
			, dautoactor(actor_gatewayc2g, ACTOR_GATEWAY_C2G)
			, dautoactor(actor_gatewayg2c, ACTOR_GATEWAY_G2C)
			, dautoactor(actor_log, ACTOR_LOG)
			, dautoactor(actor_login, ACTOR_LOGIN)
			, dautoactor(actor_csvserver, ACTOR_CSVSERVER)
			, dautoactor(actor_csvclient, ACTOR_CSVCLIENT)
			, dautoactor(actor_robot, ACTOR_ROBOT)
			, dautoactor(actor_manage_robot, ACTOR_MANAGE_ROBOT)
			, dautoactor(actor_manage_activity, ACTOR_ACTIVITY_MANAGE)
			, dautoactor(actor_brief, ACTOR_BRIEF)
			, dautoactor(actor_chat, ACTOR_CHAT)
			, dautoactor(actor_gm, ACTOR_GM)
			, dautoactor(actor_gmclient, ACTOR_GMCLIENT)
			, dautoactor(actor_mail, ACTOR_MAIL)
			, dautoactor(actor_notice, ACTOR_NOTICE)
			, dautoactor(actor_ranklist, ACTOR_RANKLIST)
			, dautoactor(actor_kcp, ACTOR_KCP)
			, dautoactor(actor_calendar, ACTOR_CALENDAR)
			, dautoactor(actor_keyvalue, ACTOR_KEYVALUE)
			, dautoactor(actor_family, ACTOR_FAMILY)
			, dautoactor(actor_friends, ACTOR_FRIENDS)
			, dautoactor(actor_example_match, ACTOR_EXAMPLE_MATCH)
			, dautoactor(actor_example_manage, ACTOR_EXAMPLE_MANAGE)
			, dautoactor(actor_example_guess_number, ACTOR_EXAMPLE_GUESS_NUMBER)
			, em_events_null(actor_events_logic)
			, em_events_null(actor_events_map)
		);
	}

	void tprotocol_customs()
	{
		tprotocol::set_customs_index(200000000);
		// 新增内部协议需要补充
		tprotocol::tp_customs::template func <
			/*200000001*/np_gm
			/*200000002*/, np_gm_response
			/*200000003*/, mforward<np_gm>
			/*200000004*/, mforward<np_gm_response>
			/*200000005*/, timerparm
			/*200000006*/, np_robot_pram
			/*200000007*/, np_connect_actor_server
			/*200000008*/, np_actor_server_register
			/*200000009*/, np_actornode_register
			/*200000010*/, np_actornode_register_response
			/*200000011*/, np_actorclient_node_connect
			/*200000012*/, np_actornode_update
			/*200000013*/, np_actornode_update_server
			/*200000014*/, np_actornode_update_mass
			/*200000015*/, np_actornode_connect_task
			/*200000016*/, np_actorrole_login
			/*200000017*/, np_actorserver_connect
			/*200000018*/, np_actor_session_close
			/*200000019*/, np_actor_disconnect_close
			/*200000020*/, np_logitem
			/*200000021*/, np_actor_broadcast
			/*200000022*/, np_actor_reloadcsv
			/*200000023*/, np_actor_csv_verify_version
			/*200000024*/, np_actor_senditem
			/*200000025*/, np_actor_gatewayinfo_updata
			/*200000026*/, np_actor_addmail
			/*200000027*/, np_actor_activity
			/*200000028*/, np_actor_gatewayid_updata
			/*200000029*/, np_actorswitch_process<np_actorswitch_process_role>
			/*200000030*/, np_actor_kcp
			/*200000031*/, np_calendar
			/*200000032*/, np_actor_close
			/*200000033*/, np_channel_check
			/*200000034*/, np_roleban
			/*200000035*/, np_gateway_close_session
			/*200000036*/, np_login_request_info
			/*200000037*/, np_create_example
			/*200000038*/, np_example_entergame_ready
			/*200000039*/, np_example_actorid
			/*200000040*/, np_example_equit
		> (EPROTOCOL_TYPE_CUSTOM);

		tprotocol::set_customs_index(210000000);
		tprotocol::tp_customs::template func <
			/*210000001*/ np_channel_register<pbdb::db_brief>
			/*210000002*/, np_channel_register_reply<pbdb::db_brief>
			/*210000003*/, np_channel_data<pbdb::db_brief>
			/*210000004*/, np_channel_exit<pbdb::db_brief>
			/*210000005*/, np_channel_register<pbdb::db_keyvalue>
			/*210000006*/, np_channel_register_reply<pbdb::db_keyvalue>
			/*210000007*/, np_channel_data<pbdb::db_keyvalue>
			/*210000008*/, np_channel_exit<pbdb::db_keyvalue>
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
	void help_tprotocol_forward_pb(const PB* apb)
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

	void tdb_init(bool ainstance)
	{
		// 新增数据存储需要补全
		tdb_ranklist::init(ainstance);
		tdb_calendar::init(ainstance);
		tdb_activity::init(ainstance);
		tdb_keyvalue::init(ainstance);
		tdb_familyer::init(ainstance);
		tdb_friends::init(ainstance);
		tdb_account::init(ainstance);
		tdb_family::init(ainstance);
		tdb_notice::init(ainstance);
		tdb_rolekv::init(ainstance);
		tdb_guild::init(ainstance);
		tdb_brief::init(ainstance);
		tdb_mail::init(ainstance);
		tdb_task::init(ainstance);
		tdb_role::init(ainstance);
		tdb_bag::init(ainstance);
	}

	void tcrossdb_init(bool ainstance)
	{
		tdb_ranklist::init(ainstance);
	}
}//namespace ngl