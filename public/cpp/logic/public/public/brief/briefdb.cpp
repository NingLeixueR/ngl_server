#include "actor_brief.h"
#include "briefdb.h"

namespace ngl
{
	void briefdb::sync_module(i64_actorid aguid, std::shared_ptr<np_actor_roleinfo>& apro)
	{
		actor()->send_actor(aguid, apro);
	}
}