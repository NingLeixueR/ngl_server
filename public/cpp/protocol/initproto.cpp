#include "initproto.h"
#include "nprotocol.h"
#include "ntimer.h"
#include "net.pb.h"
#include "actor.h"

namespace ngl
{
	std::multimap<size_t, initproto::pinfo> initproto::m_keyval;
	int32_t initproto::lcustoms = 200000000;
}