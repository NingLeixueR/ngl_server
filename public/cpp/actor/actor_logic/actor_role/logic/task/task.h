#pragma once

#include "nactor_auto.h"
#include "db.pb.h"
#include "type.h"

namespace ngl
{
	class task : 
		public tdb_task::db_modular
	{
	public:
		task() 
		{}

		virtual void initdata();
	};

	class static_task
	{
	public:
		// # 检查条件是否都完成
		static bool check_condition(actor_role* arole, const std::vector<task_condition>& acondition);
		// # 获取已完成任务
		static google::protobuf::Map<int32_t, pbdb::db_task_complete>& complete(actor_role* arole);
		static const google::protobuf::Map<int32_t, pbdb::db_task_complete>& const_complete(actor_role* arole);

		// # 获取正在执行中的任务
		static google::protobuf::Map<int32_t, pbdb::db_task_data>& run(actor_role* arole);
		static const google::protobuf::Map<int32_t, pbdb::db_task_data>& const_run(actor_role* arole);

		// # 检查任务是否完成
		static bool isfinish_task(actor_role* arole, i32_taskid ataskid);

		// # 检查任务是否已接收
		static bool isreceive_task(actor_role* arole, i32_taskid ataskid);

		// # 完成任务
		static bool finish_task(actor_role* arole, i32_taskid ataskid);

		// # 接收任务
		static bool receive_task(actor_role* arole, i32_taskid ataskid);

		// # 移除任务
		static bool erase_task(actor_role* arole, i32_taskid ataskid);

		// # 是否有可以接受任务
		// # 因为某个条件完成
		static bool update_change(actor_role* arole, ETask atype, std::set<i32_taskid>* ataskset);
		static bool update_change(actor_role* arole, ETask atype, int32_t avalues);

		// # 重置任务
		static bool reset_task(actor_role* arole, i32_taskid ataskid);
	};
}// namespace ngl

mk_formatter(pbdb::db_task)