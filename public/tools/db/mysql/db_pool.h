#pragma once

#include "db.h"

namespace ngl
{
	class db_pool
	{
		static std::vector<db*> m_vec;
	public:
		static bool init(uint32_t asize, const dbarg& arg);
		static db* get(uint32_t aindex);
	};
}
