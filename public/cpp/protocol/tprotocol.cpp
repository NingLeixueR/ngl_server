#include "tprotocol.h"
#include "nprotocol.h"
#include "ntimer.h"
#include "net.pb.h"
#include "actor.h"

namespace ngl
{
	std::multimap<size_t, tprotocol::pinfo> tprotocol::m_keyval;
	int32_t tprotocol::lcustoms = 200000000;
}