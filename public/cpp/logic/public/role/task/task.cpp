#include "actor_role.h"
#include "ttab_task.h"
#include "db.pb.h"
#include "task.h"

namespace ngl
{
	class task_check
	{
		static std::array<task_check*, ETaskCount> m_data;
	public:

		// 检查条件是否满足
		static bool check(actor_role* arole, task_condition& atab)
		{
			if (atab.m_condition == ETaskConditionMore)
			{
				return m_data[atab.m_type]->values(arole, atab) >= atab.m_parmint;
			}
			else if (atab.m_condition == ETaskConditionLess)
			{
				return m_data[atab.m_type]->values(arole, atab) <= atab.m_parmint;
			}
			else if (atab.m_condition == ETaskConditionEqual)
			{
				return m_data[atab.m_type]->values(arole, atab) == atab.m_parmint;
			}
			return false;
		}

		static void schedules(actor_role* arole, pbdb::db_task::data_schedule& adata, task_condition& atab)
		{
			adata.set_m_type(atab.m_type);
			adata.set_m_sumint(atab.m_parmint);
			adata.set_m_value(m_data[atab.m_type]->values(arole, atab));
		}

		virtual int32_t values(actor_role* arole, task_condition& atab) = 0;
	};

	// 条件检查:ETaskRoleLv 玩家等级
	class taskcheck_rolelv : public task_check
	{
		virtual int32_t values(actor_role* arole, task_condition& atab)
		{
			return arole->m_info.lv();
		}
	};

	// 条件检查:ETaskRoleVip 玩家vip等级
	class taskcheck_rolevip : public task_check
	{
		virtual int32_t values(actor_role* arole, task_condition& atab)
		{
			return arole->m_info.vip();
		}
	};

	// 条件检查:ETaskTaskId 完成某ID任务
	class taskcheck_taskid : public task_check
	{
		virtual int32_t values(actor_role* arole, task_condition& atab)
		{
			auto& lmap = arole->m_task.get_consttask().m_completeddatas();
			return lmap.find(atab.m_parmint) != lmap.end()? atab.m_parmint :-1;
		}
	};

	std::array<task_check*, ETaskCount> task_check::m_data =
	{
		(task_check*)new taskcheck_rolelv(),
		(task_check*)new taskcheck_rolevip(),
		(task_check*)new taskcheck_taskid()
	};

	bool static_task::check_condition(actor_role* arole, std::vector<task_condition>& acondition)
	{
		for (task_condition& item : acondition)
		{
			if (task_check::check(arole, item) == false)
				return false;
		}
		return true;
	}

	google::protobuf::Map<int32_t, pbdb::db_task_data>& static_task::complete(actor_role* arole)
	{
		return *arole->m_task.get_task().mutable_m_completeddatas();
	}

	google::protobuf::Map<int32_t, pbdb::db_task_data>& static_task::run(actor_role* arole)
	{
		return *arole->m_task.get_task().mutable_m_rundatas();
	}

	const google::protobuf::Map<int32_t, pbdb::db_task_data>& static_task::const_complete(actor_role* arole)
	{
		return arole->m_task.get_task().m_completeddatas();
	}

	const google::protobuf::Map<int32_t, pbdb::db_task_data>& static_task::const_run(actor_role* arole)
	{
		return arole->m_task.get_task().m_rundatas();
	}

	bool static_task::isfinish_task(actor_role* arole, i32_taskid ataskid)
	{
		auto& lcomplete = const_complete(arole);
		return lcomplete.find(ataskid) != lcomplete.end();
	}

	bool static_task::isreceive_task(actor_role* arole, i32_taskid ataskid)
	{
		auto& lrun = const_run(arole);
		return lrun.find(ataskid) != lrun.end();
	}

	void static_task::receive_task(actor_role* arole, i32_taskid ataskid)
	{
		//## 接收任务前先查看是否已经完成了
		if (isfinish_task(arole, ataskid))
			return;
		//## 此任务是否已经被接收
		if (isreceive_task(arole, ataskid))
			return;

		std::vector<task_condition>* lvec = ttab_task::get_task_condition(ataskid, true);
		if (lvec == nullptr)
			return;

		if (check_condition(arole, *lvec) == false)
			return;

		arole->m_task.get_consttask();
		pbdb::db_task::data ltemp;
		ltemp.set_m_taskid(ataskid);
		ltemp.set_m_receiveutc(localtime::gettime());
		ltemp.set_m_finshutc(-1);
		for (auto& item : *lvec)
		{
			task_check::schedules(arole, *ltemp.mutable_m_schedules()->Add(), item);
		}
		arole->m_task.get()->get().mutable_m_rundatas()->insert({ ataskid, ltemp });
	}

	void static_task::finish_task(actor_role* arole, i32_taskid ataskid)
	{
		//## 接收任务前先查看是否已经完成了
		if (isfinish_task(arole, ataskid))
			return;
		//## 此任务是否已经被接收
		if (isreceive_task(arole, ataskid) == false)
			return;
		std::vector<task_condition>* lvecfinish = ttab_task::get_task_condition(ataskid, false);
		if (lvecfinish != nullptr && check_condition(arole, *lvecfinish))
		{
			auto itor = run(arole).find(ataskid);
			complete(arole).insert({ ataskid, itor->second });
			run(arole).erase(itor);
			update_change(arole, ETaskTaskId, ataskid);
		}
	}

	bool static_task::update_change(actor_role* arole, ETask atype, std::set<i32_taskid>* ataskset)
	{
		auto& lconst_complete = const_complete(arole);
		auto& lconst_rundatas = const_run(arole);
		for (i32_taskid taskid : *ataskset)
		{
			//## 接收任务前先查看是否已经接收过了
			auto itorrun = lconst_rundatas.find(taskid);
			if (itorrun != lconst_rundatas.end())
			{
				//## 已接收任务更新进度
				for (int i = 0; i < itorrun->second.m_schedules_size(); ++i)
				{
					auto& lcomplete = run(arole);
					for (auto itor = lcomplete.begin(); itor != lcomplete.end(); ++itor)
					{
						for (pbdb::db_task::data_schedule& lschedule : *itor->second.mutable_m_schedules())
						{
							if (lschedule.m_type() == atype)
							{
								task_condition* lpcondition = ttab_task::get_task_condition(taskid, atype, false);
								if (lpcondition != nullptr)
								{
									task_check::schedules(arole, lschedule, *lpcondition);
								}
								break;
							}
						}
					}
				}
				finish_task(arole, taskid);
				continue;
			}
			
			receive_task(arole, taskid);
		}
		return true;
	}

	bool static_task::update_change(actor_role* arole, ETask atype, int32_t avalues)
	{
		{
			std::set<i32_taskid>* ltaskset = ttab_task::check(atype, avalues, true);
			if (ltaskset == nullptr)
				return false;
			update_change(arole, atype, ltaskset);
		}
		{
			std::set<i32_taskid>* ltaskset = ttab_task::check(atype, avalues, false);
			if (ltaskset == nullptr)
				return false;
			update_change(arole, atype, ltaskset);
		}
		return true;
	}

	void task::initdata()
	{
		pbdb::db_task& ltask = db()->get(false);
		auto lcompleteddatas = ltask.mutable_m_completeddatas();
		auto lrundatas = ltask.mutable_m_rundatas();

		std::cout << "###已完成的任务[===" << std::endl;
		std::cout << "[===" << std::endl;
		for (const auto& [_id, _data] : *lcompleteddatas)
		{
			std::cout << _id << std::endl;
		}
		std::cout << "===]" << std::endl;
		std::cout << "###正在进行的任务[===" << std::endl;
		std::cout << "[===" << std::endl;
		for (const auto& [_id, _data] : *lrundatas)
		{
			std::cout << _id << std::endl;
		}
		std::cout << "===]" << std::endl;

		actor_role* lrole = actor();
		// ### 检查是否有可接受的任务
		auto tabs = allcsv::get<manage_csv<tab_task>>();
		tabs->foreach([this, lrole](tab_task& atask)
			{
				if (static_task::isfinish_task(lrole, atask.m_id))
					return;
				if (static_task::isreceive_task(lrole, atask.m_id))
				{
					// 是否可完成
					std::vector<task_condition>* lvec = ttab_task::get_task_condition(atask.m_id, false);
					if (lvec == nullptr)
						return;
					if (static_task::check_condition(lrole, *lvec) == false)
						return;
					pbdb::db_task& ldb = lrole->m_task.get_task();
					
					auto itor = ldb.mutable_m_rundatas()->find(atask.m_id);
					if (itor == ldb.mutable_m_rundatas()->end())
						return;

					itor->second.clear_m_schedules();
					for (auto& item : *lvec)
					{
						task_check::schedules(lrole, *itor->second.add_m_schedules(), item);
					}

					ldb.mutable_m_completeddatas()->insert({ atask.m_id, itor->second });
					ldb.mutable_m_rundatas()->erase(itor);
					return;
				}

				static_task::receive_task(lrole, atask.m_id);
			});
	}

}// namespace ngl