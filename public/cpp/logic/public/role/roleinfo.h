#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "db.pb.h"
#include "net.pb.h"

namespace ngl
{
	class roleinfo : public tdb_role::db_modular
	{
		roleinfo(const roleinfo&) = delete;
		roleinfo& operator=(const roleinfo&) = delete;
	public:
		roleinfo() 
		{}

		~roleinfo(){}

		virtual void initdata();

		const int32_t m_id();

		const pbdb::db_role& get_constrole();

		const pbdb::db_brief& get_constbrief();

		void sync_actor_roleinfo();

		int32_t lv();
		void change_lv(int avalues);

		int32_t vip();
		void change_vip(int avalues);

		const char* name();
		void change_name(const char* aname);

		int32_t gold();
		void change_gold(int avalues);

		int32_t silver();
		void change_silver(int avalues);

		int32_t notalkutc();
		void change_notalkutc(int avalues);
	};

	
}