#include "ttab_specialid.h"

namespace ngl
{
	i32_rolelv	ttab_specialid::m_rolemaxlv = 0;
	i32_rolevip	ttab_specialid::m_rolemaxvip = 0;
	int32_t		ttab_specialid::m_createfamilconsume = 0;
	int32_t		ttab_specialid::m_familsignexp = 0;
	int32_t		ttab_specialid::m_familsigndrop = 0;
	int32_t		ttab_specialid::m_familapplylistcount = 0;
	int32_t		ttab_specialid::m_familjoininterval = 0;
	int32_t		ttab_specialid::m_friendsapplylistcount = 0;
	int32_t		ttab_specialid::m_friendscount = 0;
	int32_t		ttab_specialid::m_ranklistmaxcount = 0;
	int32_t		ttab_specialid::m_example_room_maxtime = 0;
	std::map<int32_t, int32_t>	ttab_specialid::m_example_totalnumber;
	int32_t		ttab_specialid::example_room_readytime = 0;
}