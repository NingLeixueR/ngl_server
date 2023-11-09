#pragma once

#include "db_modular.h"
#include "manage_csv.h"
#include "db.pb.h"
#include "net.pb.h"


namespace ngl
{
	//actor_dbclient<ENUM_DB_ROLE, DB_ROLE> m_role;
	class actor_role;
	using roleinfo_db_modular = db_modular<
		EPROTOCOL_TYPE_PROTOCOLBUFF, 
		ENUM_DB_ROLE, 
		pbdb::db_role,
		actor_role
		>;
	class roleinfo : public roleinfo_db_modular
	{
	public:
		roleinfo() 
			:roleinfo_db_modular()
		{}

		~roleinfo(){}

		virtual void initdata();

		const int32_t m_id();

		const pbdb::db_brief& m_base();

		void sync_actor_roleinfo();

		int32_t lv();
		void change_lv(int avalues);

		const char* name();
		void change_name(const char* aname);

		int32_t gold();
		void change_gold(int avalues);

		int32_t silver();
		void change_silver(int avalues);
	};

	
}