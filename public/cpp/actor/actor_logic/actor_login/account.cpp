#include "actor_login.h"
#include "account.h"

namespace ngl
{
	account::account() :
		m_max_accountid(1)
	{
	}

	void account::related_actorid()
	{
		set_actorid(nguid::make());
	}

	void account::initdata()
	{
		log_error()->print("actor_login###loaddb_finish {}", data());
		foreach([this](data_modified<pbdb::db_account>& dbaccount)
			{
				i16_area larea = dbaccount.getconst().marea();
				const std::string& laccount = dbaccount.getconst().maccount();
				m_areaofaccount[larea].m_data[laccount] = &dbaccount;
				m_max_accountid = std::max(m_max_accountid, nguid::actordataid(dbaccount.getconst().mid()));
			});
	}

	int32_t& account::max_accountid()
	{
		return m_max_accountid;
	}

	std::map<std::string, data_modified<pbdb::db_account>*>& account::accountbyarea(int area)
	{
		return m_areaofaccount[area].m_data;
	}
}//namespace ngl