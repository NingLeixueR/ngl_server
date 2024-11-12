#pragma once

#include "nactor_auto.h"

namespace ngl
{
	class account : public tdb_account::db_modular
	{
		account(const account&) = delete;
		account& operator=(const account&) = delete;

		struct accountbyname
		{
			std::map<std::string, data_modified<pbdb::db_account>*> m_data;
		};
		std::map<i16_area, accountbyname> m_data;
		
		i64_accountid m_max_accountid;
	public:
		account();

		virtual void set_id();

		virtual void initdata();

		i64_accountid& max_accountid();

		std::map<std::string, data_modified<pbdb::db_account>*>& accountbyarea(int area);
	};
}//namespace ngl