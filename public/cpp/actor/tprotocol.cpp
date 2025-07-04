#include "tprotocol.h"
#include "nprotocol.h"
#include "ntimer.h"
#include "net.pb.h"
#include "actor.h"

namespace ngl
{
	std::map<size_t, tprotocol::pinfo> tprotocol::m_keyval;
	std::map<i32_protocolnum, tprotocol::pinfo*> tprotocol::m_protocol;
	std::map<std::string, tprotocol::pinfo*> tprotocol::m_nameprotocol;
	int32_t tprotocol::m_customs = 200000000;
}//namespace ngl