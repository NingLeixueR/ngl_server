#include "actor_login.h"
#include "account.h"

namespace ngl
{
	account::account() :
		m_max_accountid(1)
	{
	}

	void account::set_id()
	{
		m_id = -1;
	}

	void account::initdata()
	{
		log_error()->print("actor_login###loaddb_finish");
		foreach([this](data_modified<pbdb::db_account>& dbaccount)
			{
				m_data[dbaccount.getconst().m_area()].m_data[dbaccount.getconst().m_account()] = &dbaccount;
				m_max_accountid = std::max(m_max_accountid, dbaccount.getconst().m_id());
			});
	}

	i64_accountid& account::max_accountid()
	{
		return m_max_accountid;
	}

	std::map<std::string, data_modified<pbdb::db_account>*>& account::accountbyaccount(int area)
	{
		return m_data[area].m_data;
	}
}//namespace ngl