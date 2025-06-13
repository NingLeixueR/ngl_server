#include "ttab_servers.h"
#include "db_pool.h"
#include "nlog.h"

namespace ngl
{
	std::vector<db*> db_pool::m_vec;

	void db_pool::init(const dbarg& adbarg)
	{
		if (!m_vec.empty())
		{
			return;
		}
		const tab_servers* tab = ttab_servers::instance().tab();
		m_vec.resize(tab->m_threadnum);
		for (int i = 0; i < tab->m_threadnum; ++i)
		{
			m_vec[i] = new db();
			if (!m_vec[i]->connectdb(adbarg))
			{
				tools::no_core_dump();
				continue;
			}
		}
		return;
	}

	db* db_pool::get(int32_t aindex)
	{
		if (aindex >= m_vec.size() && aindex < 0)
		{
			tools::no_core_dump();
			return nullptr;
		}
		return m_vec[aindex];
	}
}// namespace ngl