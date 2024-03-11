#pragma once

#include <vector>
#include <list>
#include <map>

#include "type.h"

namespace ngl
{
	class actor_role;
	
	struct autoitem
	{
		std::map<int32_t, int32_t> m_delstackitems;
		std::list<int32_t> m_delnostackitems;
		std::map<int32_t, int32_t> m_addstackitems;
		std::list<int32_t> m_addnostackitems;
	public:
		void add(int32_t atid, int32_t acount);
		void add(int32_t aid);
		void del(int32_t aid, int32_t acount);
		void del(int32_t aid);
	};
}// namespace ngl