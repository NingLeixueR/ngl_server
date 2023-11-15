#pragma once

#include "db.pb.h"
#include "db_modular.h"
#include "type.h"

namespace ngl
{
	class actor_role;
	using task_db_modular = db_modular<
		EPROTOCOL_TYPE_PROTOCOLBUFF,
		pbdb::ENUM_DB_TASK,
		pbdb::db_task,
		actor_role
	>;

	class task
		: public task_db_modular
	{
		// 检查任务是否可以接取
		bool check_receive(int32_t ataskid);
		// 检查是否完成任务
		bool check_complete(pbdb::db_task::data_schedule& adata);
		bool check_complete(pbdb::db_task::data& adata);
		bool check_complete(int32_t ataskid);

	public:
		task() 
		{}

		pbdb::db_task& get_task(bool achange = true)
		{
			return db()->get(achange);
		}

		const pbdb::db_task& get_consttask()
		{
			return db()->getconst();
		}

		virtual void initdata();

		// avalue 改变值
		void task_condition(ETask atype, const std::vector<int32_t>& avalue);
	};


}