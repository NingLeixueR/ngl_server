#include "actor_manage_activity.h"
#include "actor_manage_plays.h"
#include "actor_manage_role.h"
#include "actor_gatewayc2g.h"
#include "actor_gatewayg2c.h"
#include "actor_csvserver.h"
#include "actor_csvclient.h"
#include "actor_ranklist.h"
#include "actor_matching.h"
#include "actor_ugpalace.h"
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
	void _auto_actor(
		const TACTOR* aactor, ENUM_ACTOR aenum, const char* aname, const ARG&... arg
	)
	{
		_auto_actor<TACTOR>(aactor, aenum, aname);
		_auto_actor(arg...);
	}

	void auto_actor()
	{
#define em_events_null(NAME) null<NAME>,(ENUM_ACTOR)(ACTOR_EVENTS+ NAME::id_index()), #NAME

		// ### 新增actor需要补全
		_auto_actor(
			null<actor_client>, em_pram(ACTOR_CLIENT)
			, null<actor_create>, em_pram(ACTOR_CREATE)
			, null<actor_server>, em_pram(ACTOR_SERVER)
			, null<actor_cross>, em_pram(ACTOR_CROSS)
			, null<actor_manage_role>, em_pram(ACTOR_MANAGE_ROLE)
			, null<actor_role>, em_pram(ACTOR_ROLE)
			, null<actor_gateway>, em_pram(ACTOR_GATEWAY)
			, null<actor_gatewayc2g>, em_pram(ACTOR_GATEWAY_C2G)
			, null<actor_gatewayg2c>, em_pram(ACTOR_GATEWAY_G2C)
			, null<actor_log>, em_pram(ACTOR_LOG)
			, null<actor_login>, em_pram(ACTOR_LOGIN)
			, null<actor_csvserver>, em_pram(ACTOR_CSVSERVER)
			, null<actor_csvclient>, em_pram(ACTOR_CSVCLIENT)
			, null<actor_robot>, em_pram(ACTOR_ROBOT)
			, null<actor_manage_robot>, em_pram(ACTOR_MANAGE_ROBOT)
			, null<actor_manage_activity>, em_pram(ACTOR_ACTIVITY_MANAGE)
			, null<actor_brief>, em_pram(ACTOR_BRIEF)
			, null<actor_chat>, em_pram(ACTOR_CHAT)
			, null<actor_gm>, em_pram(ACTOR_GM)
			, null<actor_gmclient>, em_pram(ACTOR_GMCLIENT)
			, null<actor_mail>, em_pram(ACTOR_MAIL)
			, null<actor_notice>, em_pram(ACTOR_NOTICE)
			, null<actor_ranklist>, em_pram(ACTOR_RANKLIST)
			, null<actor_matching>, em_pram(ACTOR_MATCHING)
			, null<actor_manage_plays>, em_pram(ACTOR_MANAGE_PLAYS)
			, null<actor_ugpalace>, em_pram(ACTOR_PLAYS_GO_UNDERGROUNDPALACE)
			, null<actor_kcp>, em_pram(ACTOR_KCP)
			, null<actor_calendar>, em_pram(ACTOR_CALENDAR)
			, null<actor_plays>, em_pram(ACTOR_PLAYS)
			, null<actor_keyvalue>, em_pram(ACTOR_KEYVALUE)
			, null<actor_family>, em_pram(ACTOR_FAMILY)
			, null<actor_friends>, em_pram(ACTOR_FRIENDS)
			, em_events_null(actor_events_logic)
			, em_events_null(actor_events_map)
		);
	}

	void tprotocol_customs()
	{
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
			/*200000033*/, np_channel_register<pbdb::db_brief>
			/*200000034*/, np_channel_register_reply<pbdb::db_brief>
			/*200000035*/, np_channel_data<pbdb::db_brief>
			/*200000036*/, np_channel_register<pbdb::db_keyvalue>
			/*200000037*/, np_channel_register_reply<pbdb::db_keyvalue>
			/*200000038*/, np_channel_data<pbdb::db_keyvalue>
			/*200000039*/, np_channel_check
			/*200000040*/, np_roleban
			/*200000041*/, np_gateway_close_session
			// ### 事件相关协议 start ### //
			/*200000042*/, actor_events_logic::np_event_register
			//# actor_events_logic
			/*200000043*/, np_eevents_logic_rolelogin
			/*200000044*/, np_eevents_logic_roleoffline
			//# actor_events_map
			/*200000045*/, np_eevents_map_leaveview
			/*200000046*/, np_eevents_map_enterview
			// ### 事件相关协议 finish ### //
		> (EPROTOCOL_TYPE_CUSTOM);
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
		tdb_account::init(ainstance);
		tdb_family::init(ainstance);
		tdb_familyer::init(ainstance);
		tdb_notice::init(ainstance);
		tdb_rolekv::init(ainstance);
		tdb_guild::init(ainstance);
		tdb_brief::init(ainstance);
		tdb_mail::init(ainstance);
		tdb_task::init(ainstance);
		tdb_role::init(ainstance);
		tdb_bag::init(ainstance);
		tdb_friends::init(ainstance);
	}

	void tcrossdb_init(bool ainstance)
	{
		tdb_ranklist::init(ainstance);
	}

	void init_all_pb()
	{

	}
}//namespace ngl