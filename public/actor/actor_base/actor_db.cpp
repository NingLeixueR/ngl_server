#include "actor_db.h"


namespace ngl
{
	std::vector<db*> actor_dbpool::m_data;
	bool actor_dbpool::m_init = false;
	void actor_dbpool::init()
	{
		if (m_init)
			return;
		Try
		{
			m_init = true;
			tab_servers* tab = ttab_servers::tab();

			m_data.resize(tab->m_threadnum);
			for (int i = 0; i < tab->m_threadnum; ++i)
			{
				m_data[i] = new db();
				Assert(m_data[i]->connectdb(nconfig::m_db.m_dbarg));
			}
		}Catch;
	}

	db* actor_dbpool::get(int apos)
	{
		Try
		{
			return m_data.at(apos);
		}
		Catch;
		return nullptr;
	}
}