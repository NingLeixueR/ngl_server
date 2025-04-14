#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>& adata)
	{
		tools::print_protojson(*adata.get_data(), true);
		return true;
	}
}//namespace ngl