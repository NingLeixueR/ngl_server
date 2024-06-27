#include "ttab_servers.h"
#include "db_pool.h"
#include "nlog.h"

namespace ngl
{
	std::vector<db*> db_pool::m_vec;

	void db_pool::init()
	{
		Try
		{
			if (!m_vec.empty())
				return;
			tab_servers* tab = ttab_servers::tab();
			m_vec.resize(tab->m_threadnum);
			for (int i = 0; i < tab->m_threadnum; ++i)
			{
				m_vec[i] = new db();
				Assert(m_vec[i]->connectdb(nconfig::m_db.m_dbarg));
			}
			return;
		}Catch;
	}

	db* db_pool::get(int32_t aindex)
	{
		Try
		{
			return m_vec.at(aindex);
		}Catch;
		return nullptr;
	}
}// namespace ngl