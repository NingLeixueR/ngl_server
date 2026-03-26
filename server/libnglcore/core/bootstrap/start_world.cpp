#include "actor/actor_logic/actor_activity_manage/actor_activity_manage.h"
#include "actor/actor_logic/actor_example_manage/actor_example_manage.h"
#include "actor/actor_logic/actor_example_match/actor_example_match.h"
#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/actor_logic/actor_testlua2/actor_testlua2.h"
#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/actor_logic/actor_keyvalue/actor_keyvalue.h"
#include "actor/actor_logic/actor_ranklist/actor_ranklist.h"
#include "actor/actor_logic/actor_friends/actor_friends.h"
#include "actor/actor_logic/actor_testlua/actor_testlua.h"
#include "actor/actor_logic/actor_family/actor_family.h"
#include "actor/actor_logic/actor_notice/actor_notice.h"
#include "actor/actor_logic/actor_events/actor_events.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"
#include "actor/actor_logic/actor_mail/actor_mail.h"
#include "actor/actor_logic/actor_chat/actor_chat.h"
#include "actor/actor_logic/actor_gm/actor_gm.h"
#include "actor/generated/auto/nactor_auto.h"
#include "tools/tab/xml/xml.h"
#include "init_dbdata.h"
#include "start_node.h"
#include "start_db.h"

startup_error start_world(int* atcp_port)
{
	return start_node("WORLD", atcp_port, make_node_opt(), []()
		{
			ngl::actor_events_logic::instance();
			ngl::actor_gm::instance();
			ngl::actor_mail::instance();
			ngl::actor_chat::instance();
			ngl::actor_brief::instance();
			ngl::actor_notice::instance();
			ngl::actor_keyvalue::instance();
			ngl::actor_ranklist::instance();
			ngl::actor_activity_manage::instance();
			ngl::actor_family::instance();
			ngl::actor_friends::instance();
			ngl::actor_gmclient::instance();
			ngl::actor_example_match::instance();
			ngl::actor_example_manage::instance();
			ngl::actor_testlua::instance();
			ngl::actor_testlua2::instance();
		});
}
