#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "db.pb.h"
#include "net.pb.h"

namespace ngl
{
	class roleinfo : 
		public tdb_role::db_modular
	{
		roleinfo(const roleinfo&) = delete;
		roleinfo& operator=(const roleinfo&) = delete;
	public:
		roleinfo() 
		{}

		~roleinfo(){}

		virtual void initdata();

		const i64_actorid roleid();

		pbdb::db_brief* get_brief();
		const pbdb::db_brief* get_constbrief();

		void change();

		void change_event(eevents_logic type, int abegvalue, int32_t anowvalue);

		// # 等级
		int32_t lv();
		void change_lv(int avalues);

		// # vip
		int32_t vip();
		void change_vip(int avalues);

		// # 名字
		const char* name();
		void rename(const char* aname);

		// # 金币
		int32_t gold();
		void change_gold(int avalues);

		// # 银币
		int32_t silver();
		void change_silver(int avalues);

		// # 禁言
		int32_t notalkutc();
		void change_notalkutc(int avalues);
		bool bantalk();
	};
}//namespace ngl

mk_formatter(pbdb::db_role)