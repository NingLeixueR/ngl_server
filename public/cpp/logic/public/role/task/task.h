#pragma once

#include "nactor_auto.h"
#include "db.pb.h"
#include "type.h"

namespace ngl
{
	class task: public tdb_task::db_modular
	{
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
	};

	class static_task
	{
	public:
		// # ��������Ƿ����
		static bool check_condition(actor_role* arole, std::vector<task_condition>& acondition);

		static google::protobuf::Map<int32_t, pbdb::db_task_data>&			complete(actor_role* arole);
		static google::protobuf::Map<int32_t, pbdb::db_task_data>&			run(actor_role* arole);
		static const google::protobuf::Map<int32_t, pbdb::db_task_data>&	const_complete(actor_role* arole);
		static const google::protobuf::Map<int32_t, pbdb::db_task_data>&	const_run(actor_role* arole);

		// # �����Ƿ����
		static bool isfinish_task(actor_role* arole, i32_taskid ataskid);
		static bool isreceive_task(actor_role* arole, i32_taskid ataskid);

		static bool finish_task(actor_role* arole, i32_taskid ataskid);
		// # ��������Ƿ���Ա���ȡ
		static void receive_task(actor_role* arole, i32_taskid ataskid);

		// # �Ƿ��п��Խ�������
		// #  ��Ϊĳ���������
		static bool update_change(actor_role* arole, ETask atype, std::set<i32_taskid>* ataskset);
		static bool update_change(actor_role* arole, ETask atype, int32_t avalues);
	};
}// namespace ngl