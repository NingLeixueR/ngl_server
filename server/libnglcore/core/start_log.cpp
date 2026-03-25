#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/auto/nactor_auto.h"
#include "tools/tab/xml/xml.h"
#include "tools/log/nlog.h"
#include "init_dbdata.h"
#include "start_node.h"
#include "start_db.h"

startup_error start_log(int* atcp_port)
{
	return start_node("LOG", atcp_port, make_node_opt({}, true, false, true), []()
		{
			if (ngl::sysconfig::logwritelevel() >= ngl::ELOG_MAX || ngl::sysconfig::logwritelevel() <= ngl::ELOG_NONE)
			{
				ngl::tools::no_core_dump();
			}

			int32_t llogtype = ngl::ELOG_DEFAULT | ngl::ELOG_BI;
			ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
			ngl::actor_gmclient::instance();
		});
}
