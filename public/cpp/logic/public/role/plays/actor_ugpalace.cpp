#include "actor_ugpalace.h"

namespace ngl
{
	actor_ugpalace::actor_ugpalace(ENUM_ACTOR atype, int32_t adataid, void* data) :
		actor_plays(atype, adataid, data)
	{

	}

	void actor_ugpalace::nregister()
	{
		actor_plays::nregister();
	}
}