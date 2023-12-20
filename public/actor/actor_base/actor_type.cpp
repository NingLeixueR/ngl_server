#include "actor_type.h"

#include "actor_client.h"
#include "actor_create.h"
#include "actor_db.h"
#include "actor_server.h"
#include "actor_cross.h"
#include "actor_manage_role.h"
#include "actor_role.h"
#include "actor_gateway.h"
#include "actor_gateway_client2game.h"
#include "actor_gateway_game2client.h"
#include "actor_log.h"
#include "actor_login.h"
#include "actor_reloadcsv.h"
#include "actor_robot.h"
#include "actor_manage_activity.h"
#include "actor_brief.h"
#include "actor_chat.h"
#include "actor_gm.h"
#include "actor_mail.h"
#include "actor_notice.h"
#include "actor_ranklist.h"
#include "actor_matching.h"
#include "actor_manage_plays.h"
#include "actor_plays_go_undergroundpalace.h"

namespace ngl
{
	std::multimap<size_t, ENUM_ACTOR> actor_type::m_keyval;

	template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB>
	void initdb()
	{
		actor_type::inits<
			ngl::actor_db<EPROTOCOL_TYPE_PROTOCOLBUFF, TDBTAB_TYPE, TDBTAB>
		>((ENUM_ACTOR)(PROTYPE * 1000 + ACTOR_DB + TDBTAB_TYPE));
	}

	void actor_type::init()
	{
		inits<actor_client>(ACTOR_ADDRESS_CLIENT);
		inits<actor_create>(ACTOR_CREATE);
		inits<actor_server>(ACTOR_ADDRESS_SERVER);
		inits<actor_cross>(ACTOR_CROSS);
		inits<actor_manage_role>(ACTOR_MANAGE_ROLE);
		inits<actor_role>(ACTOR_ROLE);
		inits<actor_gateway>(ACTOR_GATEWAY);
		inits<actor_gateway_client2game>(ACTOR_GATEWAY_CLIENT2GAME);
		inits<actor_gateway_game2client>(ACTOR_GATEWAY_GAME2CLIENT);
		inits<actor_log>(ACTOR_LOG);
		inits<actor_login>(ACTOR_LOGIN);
		inits<actor_reloadcsv>(ACTOR_RELOADCSV);
		inits<actor_reloadcsv_distribute>(ACTOR_RELOADCSV_DISTRIBUTE);
		inits<actor_robot>(ACTOR_ROBOT);
		inits<actor_manage_robot>(ACTOR_MANAGE_ROBOT);
		inits<actor_manage_activity>(ACTOR_ACTIVITY_MANAGE);
		inits<actor_brief>(ACTOR_BRIEF);
		inits<actor_chat>(ACTOR_CHAT);
		inits<actor_gm>(ACTOR_GM);
		inits<actor_mail>(ACTOR_MAIL);
		inits<actor_notice>(ACTOR_NOTICE);
		inits<actor_ranklist>(ACTOR_RANKLIST);
		inits<actor_matching>(ACTOR_MATCHING);
		inits<actor_manage_plays>(ACTOR_MANAGE_PLAYS);
		inits<actor_plays_go_undergroundpalace>(ACTOR_PLAYS_GO_UNDERGROUNDPALACE);

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
}