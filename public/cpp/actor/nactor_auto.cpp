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
#include "actor_gateway.h"
#include "actor_notice.h"
#include "actor_server.h"
#include "actor_client.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "nactor_auto.h"
#include "actor_login.h"
#include "actor_robot.h"
#include "actor_brief.h"
#include "actor_cross.h"
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
		tprotocol::customs::template func<
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
	void auto_actor(const TACTOR* aactor, ENUM_ACTOR aenum, const char* aname)
	{
		em<ENUM_ACTOR>::set(aenum, aname);
		nactor_type<TACTOR>::inits(aenum);
	}

	template <typename TACTOR, typename ...ARG>
	void auto_actor(
		const TACTOR* aactor, ENUM_ACTOR aenum, const char* aname, const ARG&... arg
	)
	{
		auto_actor<TACTOR>(aactor, aenum, aname);
		auto_actor(arg...);
	}

	void init_nactor_type(bool aregister)
	{
		if (aregister == true)
		{
			// ### 新增actor需要补全
			auto_actor(
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
			);

			// 新增内部协议需要补充
			tprotocol::customs::template func <
				/*200000001*/np_gm
				/*200000002*/, np_gm_response
				/*200000003*/, mforward<np_gm>
				/*200000004*/, mforward<np_gm_response>
				/*200000005*/, timerparm
				/*200000006*/, np_robot_pram
				/*200000007*/, np_actornode_register
				/*200000008*/, np_actornode_register_response
				/*200000009*/, np_actorclient_node_connect
				/*200000010*/, np_actornode_update
				/*200000011*/, np_actornode_update_mass
				/*200000012*/, np_actornode_connect_task
				/*200000013*/, np_actorrole_login
				/*200000014*/, np_actorserver_connect
				/*200000015*/, np_actor_session_close
				/*200000016*/, np_actor_disconnect_close
				/*200000017*/, np_actor_logitem
				/*200000018*/, np_actor_broadcast
				/*200000019*/, np_actor_reloadcsv
				/*200000020*/, np_actor_csv_verify_version
				/*200000021*/, np_actor_senditem
				/*200000022*/, np_actor_gatewayinfo_updata
				/*200000023*/, np_actor_addmail
				/*200000024*/, np_actor_activity
				/*200000025*/, np_actor_gatewayid_updata
				/*200000026*/, np_actorswitch_process<np_actorswitch_process_role>
				/*200000027*/, np_actor_kcp
				/*200000028*/, np_calendar
				/*200000029*/, np_actor_close
				/*200000030*/, np_channel_register<pbdb::db_brief>
				/*200000031*/, np_channel_register_reply<pbdb::db_brief>
				/*200000032*/, np_channel_data<pbdb::db_brief>
				/*200000033*/, np_channel_register<pbdb::db_keyvalue>
				/*200000034*/, np_channel_register_reply<pbdb::db_keyvalue>
				/*200000035*/, np_channel_data<pbdb::db_keyvalue>
			> (EPROTOCOL_TYPE_CUSTOM);
		}
		
		// 新增数据存储需要补全
		tdb_ranklist::init(aregister);
		tdb_calendar::init(aregister);
		tdb_activity::init(aregister);
		tdb_keyvalue::init(aregister);
		tdb_account::init(aregister);
		tdb_family::init(aregister);
		tdb_notice::init(aregister);
		tdb_rolekv::init(aregister);
		tdb_brief::init(aregister);
		tdb_guild::init(aregister);
		tdb_brief::init(aregister);
		tdb_mail::init(aregister);
		tdb_task::init(aregister);
		tdb_role::init(aregister);
		tdb_bag::init(aregister);
	}
}

#include "nforward.h"
#include "net.pb.h"

namespace ngl
{
	// # 注册client到game的消息 
	// # 会注册gateway的转发与game的处理
	void nforward::c2g()
	{
		register_c2g<EPROTOCOL_TYPE_PROTOCOLBUFF
			, pbnet::PROBUFF_NET_GET_TIME
			, pbnet::PROBUFF_NET_CMD
			, pbnet::PROBUFF_NET_SWITCH_LINE
		>();

		//// ACTOR_MAIL 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_MAIL
			, pbnet::PROBUFF_NET_MAIL_LIST
			, pbnet::PROBUFF_NET_MAIL_READ
			, pbnet::PROBUFF_NET_MAIL_DRAW
			, pbnet::PROBUFF_NET_MAIL_DEL
		>();

		// ACTOR_NOTICE 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_NOTICE
			, pbnet::PROBUFF_NET_NOTICE
		>();

		// ACTOR_CHAT 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_CHAT
			, pbnet::PROBUFF_NET_CHAT
		>();

		// ACTOR_FAMILY 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_FAMILY
			, pbnet::PROBUFF_NET_JOIN_FAMIL
			, pbnet::PROBUFF_NET_LEAVE_FAMIL
			, pbnet::PROBUFF_NET_FAMIL_LIST
			, pbnet::PROBUFF_NET_CHANGE_FAMILNAME
			, pbnet::PROBUFF_NET_CREATE_FAMIL
		>();

		// ACTOR_RANKLIST 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_RANKLIST
			, pbnet::PROBUFF_NET_RANKLIST
		>();
	}

	// # 注册game到client的消息
	// # 会注册gateway的转发与client的处理
	void nforward::g2c()
	{
		register_g2c<EPROTOCOL_TYPE_PROTOCOLBUFF
			, pbnet::PROBUFF_NET_GET_TIME_RESPONSE
			, pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE
			, pbnet::PROBUFF_NET_CHAT_RESPONSE
			, pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE
			, pbnet::PROBUFF_NET_NOTICE_RESPONSE
			, pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE
			, pbnet::PROBUFF_NET_MAIL_READ_RESPONSE
			, pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE
			, pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE
			, pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE
			, pbnet::PROBUFF_NET_ERROR
			, pbnet::PROBUFF_NET_CREATE_FAMIL_RESPONSE
			, pbnet::PROBUFF_NET_JOIN_FAMIL_RESPONSE
			, pbnet::PROBUFF_NET_LEAVE_FAMIL_RESPONSE
			, pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE
			, pbnet::PROBUFF_NET_CHANGE_FAMILNAME_RESPONSE
			, pbnet::PROBUFF_NET_RANKLIST_RESPONSE
		>();
	}
}//namespace ngl