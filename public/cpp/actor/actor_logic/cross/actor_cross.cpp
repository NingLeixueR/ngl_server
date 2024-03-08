#include "actor_cross.h"

namespace ngl
{
	void actor_cross::nregister()
	{
		// ¶¨Ê±Æ÷
		actor_cross::register_timer<actor_cross>(&actor_cross::timer_handle);
	}

	void actor_cross::loaddb_finish(bool adbishave)
	{
		LogLocalError("actor_cross###loaddb_finish#[%]", nguid(id_guid()))
	}
	
	bool actor_cross::timer_handle(message<timerparm>& adata)
	{
		return true;
	}
}//namespace ngl