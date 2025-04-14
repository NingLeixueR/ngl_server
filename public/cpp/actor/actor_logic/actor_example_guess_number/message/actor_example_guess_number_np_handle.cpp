#include "actor_example_guess_number.h"
#include "actor_brief.h"

namespace ngl
{
	bool actor_example_guess_number::handle(const message<np_example_entergame_ready>& adata)
	{
		next_setp();
		return true;
	}
}//namespace ngl