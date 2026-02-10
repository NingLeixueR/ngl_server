/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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