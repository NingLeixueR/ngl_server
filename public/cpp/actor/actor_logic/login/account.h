#pragma once

#include "nactor_auto.h"

namespace ngl
{
	class account : public tdb_account::db_modular
	{
		account(const account&) = delete;
		account& operator=(const account&) = delete;

		struct accountname2db
		{
			std::map<std::string, data_modified<pbdb::db_account>*> m_accountbyaccount;
		};
		std::map<int/*area*/, accountname2db> m_data;
		
		i64_accountid m_keyvalue_accountid;
	public:
		account();

		virtual void set_id();

		virtual void initdata();

		i64_accountid& keyvalue_accountid();

		std::map<std::string, data_modified<pbdb::db_account>*>& accountbyaccount(int area);
	};
}//namespace ngl