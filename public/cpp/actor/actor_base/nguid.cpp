#include "ttab_servers.h"
#include "nguid.h"

namespace ngl
{
	i64_actorid nguid::make_self(ENUM_ACTOR atype)
	{
		nguid lguid(atype, tab_self_area, none_actordataid());
		return (i64_actorid)lguid;
	}
}//namespace ngl