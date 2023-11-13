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
		account();

		virtual void set_id();

		virtual void initdata();

		i64_accountid& keyvalue_accountid();

		std::map<std::string, data_modified<pbdb::db_account>*>& accountbyaccount(int area);
	};

}