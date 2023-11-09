#pragma once

#include "db_modular.h"

namespace ngl
{
	class actor_login;
	using account_db_modular = db_modular<
		EPROTOCOL_TYPE_PROTOCOLBUFF,
		pbdb::ENUM_DB_ACCOUNT,
		pbdb::db_account,
		actor_login
	>;
	class account : public account_db_modular
	{
		struct accountname2db
		{
			std::map<std::string, data_modified<pbdb::db_account>*> m_accountbyaccount;
		};
		std::map<int/*area*/, accountname2db> m_data;
		
		i64_accountid m_keyvalue_accountid;
	public:
		account() :
			db_modular(),
			m_keyvalue_accountid(1)
		{
		}

		virtual void set_id()
		{
			m_id = -1;
		}

		virtual void initdata()
		{
			LogLocalError("actor_login###loaddb_finish");
			foreach([this](data_modified<pbdb::db_account>& dbaccount)
				{
					m_data[dbaccount.getconst().m_area()].m_accountbyaccount[dbaccount.getconst().m_account()] = &dbaccount;
					if (m_keyvalue_accountid < dbaccount.getconst().m_id())
						m_keyvalue_accountid = dbaccount.getconst().m_id();
				});
		}

		i64_accountid& keyvalue_accountid()
		{
			return m_keyvalue_accountid;
		}

		std::map<std::string, data_modified<pbdb::db_account>*>& accountbyaccount(int area)
		{
			return m_data[area].m_accountbyaccount;
		}
	};

}