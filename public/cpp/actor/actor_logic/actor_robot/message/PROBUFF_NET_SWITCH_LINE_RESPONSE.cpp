#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata)
	{
		tools::print_protojson(*adata.get_data(), true);
		return true;
	}
}//namespace ngl