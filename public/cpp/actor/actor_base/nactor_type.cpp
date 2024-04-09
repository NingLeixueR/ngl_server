#include "actor_manage_activity.h"
#include "actor_manage_plays.h"
#include "actor_manage_role.h"
#include "actor_gatewayc2g.h"
#include "actor_gatewayg2c.h"
#include "actor_reloadcsv.h"
#include "actor_ranklist.h"
#include "actor_matching.h"
#include "actor_ugpalace.h"
#include "actor_gateway.h"
#include "actor_notice.h"
#include "actor_server.h"
#include "actor_client.h"
#include "actor_create.h"
#include "actor_login.h"
#include "actor_robot.h"
#include "actor_brief.h"
#include "actor_cross.h"
#include "actor_chat.h"
#include "actor_mail.h"
#include "actor_role.h"
#include "actor_kcp.h"
#include "actor_log.h"
#include "actor_db.h"
#include "actor_gm.h"
#include "nactor_type.h"

namespace ngl
{
	ENUM_ACTOR db_enum(EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB TDBTAB_TYPE)
	{
		return (ENUM_ACTOR)(PROTYPE * 1000 + ACTOR_DB + TDBTAB_TYPE);
	}

	template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	void initdb()
	{
		nactor_type<
			ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, TDBTAB_TYPE, TDBTAB>
		>::inits(db_enum(PROTYPE, TDBTAB_TYPE));
		LogLocalError("init_actor_type [%]-[%]", 
			boost::typeindex::type_id_with_cvr<
				ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, TDBTAB_TYPE, TDBTAB>
				>().pretty_name(), 
			(int)(PROTYPE * 1000 + ACTOR_DB + TDBTAB_TYPE)
		)
	}

	void init_nactor_type()
	{
#define dinit_atype(ACTORCLASS, ACTORTYPE)									\
		nactor_type<ACTORCLASS>::inits(ACTORTYPE);							\
		LogLocalError("init_actor_type [%]-[%]",							\
			boost::typeindex::type_id_with_cvr<ACTORCLASS>().pretty_name(),	\
			(int)(ACTORTYPE)												\
		)

		dinit_atype(actor_client, ACTOR_ADDRESS_CLIENT);
		dinit_atype(actor_create, ACTOR_CREATE);
		dinit_atype(actor_server, ACTOR_ADDRESS_SERVER);
		dinit_atype(actor_cross, ACTOR_CROSS);
		dinit_atype(actor_manage_role, ACTOR_MANAGE_ROLE);
		dinit_atype(actor_role, ACTOR_ROLE);
		dinit_atype(actor_gateway, ACTOR_GATEWAY);
		dinit_atype(actor_gatewayc2g, ACTOR_GATEWAY_CLIENT2GAME);
		dinit_atype(actor_gatewayg2c, ACTOR_GATEWAY_GAME2CLIENT);
		dinit_atype(actor_log, ACTOR_LOG);
		dinit_atype(actor_login, ACTOR_LOGIN);
		dinit_atype(actor_reloadcsv, ACTOR_RELOADCSV);
		dinit_atype(actor_reloadcsv_distribute, ACTOR_RELOADCSV_DISTRIBUTE);
		dinit_atype(actor_robot, ACTOR_ROBOT);
		dinit_atype(actor_manage_robot, ACTOR_MANAGE_ROBOT);
		dinit_atype(actor_manage_activity, ACTOR_ACTIVITY_MANAGE);
		dinit_atype(actor_brief, ACTOR_BRIEF);
		dinit_atype(actor_chat, ACTOR_CHAT);
		dinit_atype(actor_gm, ACTOR_GM);
		dinit_atype(actor_mail, ACTOR_MAIL);
		dinit_atype(actor_notice, ACTOR_NOTICE);
		dinit_atype(actor_ranklist, ACTOR_RANKLIST);
		dinit_atype(actor_matching, ACTOR_MATCHING);
		dinit_atype(actor_manage_plays, ACTOR_MANAGE_PLAYS);
		dinit_atype(actor_ugpalace, ACTOR_PLAYS_GO_UNDERGROUNDPALACE);
		dinit_atype(actor_kcp, ACTOR_KCP);

		initdb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_ACCOUNT, pbdb::db_account>();
		initdb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_ROLE, pbdb::db_role>();
		initdb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_BAG, pbdb::db_bag>();
		initdb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_KEYVALUE, pbdb::db_keyvalue>();
		initdb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_MAIL, pbdb::db_mail>();
		initdb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_GUILD, pbdb::db_guild>();
		initdb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_NOTICE, pbdb::db_notice>();
		initdb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_ACTIVITY, pbdb::db_activity>();
		initdb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_BRIEF, pbdb::db_brief>();
		initdb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_RANKLIST, pbdb::db_ranklist>();
		initdb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_TASK, pbdb::db_task>();
	}
}//namespace ngl