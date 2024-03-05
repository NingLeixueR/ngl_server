#include "account.h"


namespace ngl
{
	account::account() :
		db_modular(),
		m_keyvalue_accountid(1)
	{
	}

	void account::set_id()
	{
		m_id = -1;
	}

	void account::initdata()
	{
		LogLocalError("actor_login###loaddb_finish")
		foreach([this](data_modified<pbdb::db_account>& dbaccount)
			{
				m_data[dbaccount.getconst().m_area()].m_accountbyaccount[dbaccount.getconst().m_account()] = &dbaccount;
				if (m_keyvalue_accountid < dbaccount.getconst().m_id())
					m_keyvalue_accountid = dbaccount.getconst().m_id();
			});
	}

	i64_accountid& account::keyvalue_accountid()
	{
		return m_keyvalue_accountid;
	}

	std::map<std::string, data_modified<pbdb::db_account>*>& account::accountbyaccount(int area)
	{
		return m_data[area].m_accountbyaccount;
	}
}