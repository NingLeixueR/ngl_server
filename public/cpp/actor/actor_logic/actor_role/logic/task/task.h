/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Player task/quest system with progress tracking and completion rewards.

#pragma once

#include "actor/generated/auto/nactor_auto.h"
#include "actor/generated/pb/db.pb.h"
#include "tools/type.h"

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
		// Check whether all conditions are met
		static bool check_condition(actor_role* arole, const std::vector<task_condition>& acondition);

		// Get completed tasks map
		static google::protobuf::Map<int32_t, pbdb::db_task_complete>& complete(actor_role* arole);
		static const google::protobuf::Map<int32_t, pbdb::db_task_complete>& const_complete(actor_role* arole);

		// Get in-progress tasks map
		static google::protobuf::Map<int32_t, pbdb::db_task_data>& run(actor_role* arole);
		static const google::protobuf::Map<int32_t, pbdb::db_task_data>& const_run(actor_role* arole);

		// Check whether the task is already completed
		static bool isfinish_task(actor_role* arole, i32_taskid ataskid);

		// Check whether the task has been received
		static bool isreceive_task(actor_role* arole, i32_taskid ataskid);

		// Complete a task and grant rewards
		static bool finish_task(actor_role* arole, i32_taskid ataskid);

		// Receive (accept) a task
		static bool receive_task(actor_role* arole, i32_taskid ataskid);

		// Remove a task from the active list
		static bool erase_task(actor_role* arole, i32_taskid ataskid);

		// Update tasks when a condition changes (may trigger receive or complete)
		// Checks whether condition completion enables new tasks
		static bool update_change(actor_role* arole, ETask atype, std::set<i32_taskid>* ataskset);
		static bool update_change(actor_role* arole, ETask atype, int32_t avalues);

		// Reset a task (erase and re-receive)
		static bool reset_task(actor_role* arole, i32_taskid ataskid);
	};
}// namespace ngl

mk_formatter(pbdb::db_task)