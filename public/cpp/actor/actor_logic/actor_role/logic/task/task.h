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
		// # ��������Ƿ����
		static bool check_condition(actor_role* arole, const std::vector<task_condition>& acondition);
		// # ��ȡ���������
		static google::protobuf::Map<int32_t, pbdb::db_task_complete>& complete(actor_role* arole);
		static const google::protobuf::Map<int32_t, pbdb::db_task_complete>& const_complete(actor_role* arole);

		// # ��ȡ����ִ���е�����
		static google::protobuf::Map<int32_t, pbdb::db_task_data>& run(actor_role* arole);
		static const google::protobuf::Map<int32_t, pbdb::db_task_data>& const_run(actor_role* arole);

		// # ��������Ƿ����
		static bool isfinish_task(actor_role* arole, i32_taskid ataskid);

		// # ��������Ƿ��ѽ���
		static bool isreceive_task(actor_role* arole, i32_taskid ataskid);

		// # �������
		static bool finish_task(actor_role* arole, i32_taskid ataskid);

		// # ��������
		static bool receive_task(actor_role* arole, i32_taskid ataskid);

		// # �Ƴ�����
		static bool erase_task(actor_role* arole, i32_taskid ataskid);

		// # �Ƿ��п��Խ�������
		// # ��Ϊĳ���������
		static bool update_change(actor_role* arole, ETask atype, std::set<i32_taskid>* ataskset);
		static bool update_change(actor_role* arole, ETask atype, int32_t avalues);

		// # ��������
		static bool reset_task(actor_role* arole, i32_taskid ataskid);
	};
}// namespace ngl

mk_formatter(pbdb::db_task)