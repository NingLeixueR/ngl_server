#include "actor_testlua.h"
namespace ngl
{
	bool actor_testlua::handle(const message<mforward<pbnet::PROBUFF_NET_TESTLUA>>& adata)
	{
		return true;
	}
}//namespace ngl
