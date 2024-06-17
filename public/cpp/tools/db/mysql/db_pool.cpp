#include "db_pool.h"

namespace ngl
{
	std::vector<db*> db_pool::m_vec;

	bool db_pool::init(uint32_t asize, const dbarg& arg)
	{
		m_vec.resize(asize);
		for (uint32_t i = 0; i < asize; ++i)
		{
			m_vec[i] = new db();
			if (!m_vec[i]->connectdb(arg))
				return false;
		}
		return true;
	}

	db* db_pool::get(uint32_t aindex)
	{
		if (aindex >= m_vec.size())
			return nullptr;
		return m_vec[aindex];
	}
}// namespace ngl