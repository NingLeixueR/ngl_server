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

#include "actor/actor_logic/actor_role/logic/task/task.h"
#include "actor/actor_logic/actor_role/logic/bag/drop.h"
#include "actor/actor_logic/actor_role/actor_role.h"
#include "actor/actor_logic/actor_mail/actor_mail.h"
#include "actor/tab/ttab_task.h"
#include "actor/pb/db.pb.h"

namespace ngl
{
	class task_check
	{
		static std::array<std::unique_ptr<task_check>, ETaskCount> m_data;
	public:
		// 检查条件是否满足
		static bool check(actor_role* arole, const task_condition& atab)
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

		static void schedules(actor_role* arole, pbdb::db_task::data_schedule& adata, const task_condition& atab)
		{
			adata.set_mtype(atab.m_type);
			adata.set_msumint(atab.m_parmint);
			adata.set_mvalue(m_data[atab.m_type]->values(arole, atab));
		}

		virtual int32_t values(actor_role* arole, const task_condition& atab) = 0;
	};

	// 条件检查:ETaskRoleLv 玩家等级
	class taskcheck_rolelv : 
		public task_check
	{
		virtual int32_t values(actor_role* arole, const task_condition& atab)
		{
			return arole->m_info.lv();
		}
	};

	// 条件检查:ETaskRoleVip 玩家vip等级
	class taskcheck_rolevip : 
		public task_check
	{
		virtual int32_t values(actor_role* arole, const task_condition& atab)
		{
			return arole->m_info.vip();
		}
	};

	// 条件检查:ETaskTaskId 完成某ID任务
	class taskcheck_taskid : 
		public task_check
	{
		virtual int32_t values(actor_role* arole, const task_condition& atab)
		{
			MODIFIED_RETURN_CONST(lptaskconst, arole->m_task.get(), -1);
			auto& lmap = lptaskconst->mcompleteddatas();
			return lmap.find(atab.m_parmint) != lmap.end()? atab.m_parmint :-1;
		}
	};

	std::array<std::unique_ptr<task_check>, ETaskCount> task_check::m_data =
	{
		std::make_unique<taskcheck_rolelv>(),
		std::make_unique<taskcheck_rolevip>(),
		std::make_unique<taskcheck_taskid>(),
	};

	bool static_task::check_condition(actor_role* arole, const std::vector<task_condition>& acondition)
	{
		for (const task_condition& item : acondition)
		{
			if (task_check::check(arole, item) == false)
			{
				return false;
			}
		}
		return true;
	}

	google::protobuf::Map<int32_t, pbdb::db_task_complete>& static_task::complete(actor_role* arole)
	{
		MODIFIED_DUMP(lptask, arole->m_task.get());
		return *lptask->mutable_mcompleteddatas();
	}

	google::protobuf::Map<int32_t, pbdb::db_task_data>& static_task::run(actor_role* arole)
	{
		MODIFIED_DUMP(lptask, arole->m_task.get());
		return *lptask->mutable_mrundatas();
	}

	const google::protobuf::Map<int32_t, pbdb::db_task_complete>& static_task::const_complete(actor_role* arole)
	{
		MODIFIED_DUMP_CONST(lptaskconst, arole->m_task.get());
		return lptaskconst->mcompleteddatas();
	}

	const google::protobuf::Map<int32_t, pbdb::db_task_data>& static_task::const_run(actor_role* arole)
	{
		MODIFIED_DUMP_CONST(lptaskconst, arole->m_task.get());
		return lptaskconst->mrundatas();
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

	bool static_task::receive_task(actor_role* arole, i32_taskid ataskid)
	{
		if (ttab_task::instance().repeat(arole, ataskid) == false)
		{//## 不可重复完成任务
			//## 接收任务前先查看是否已经完成了
			if (isfinish_task(arole, ataskid))
			{
				return false;
			}
		}
		
		//## 此任务是否已经被接收
		if (isreceive_task(arole, ataskid))
		{
			return true;
		}

		const std::vector<task_condition>* lvec = ttab_task::instance().condition_receive(ataskid);
		if (lvec == nullptr)
		{
			return false;
		}

		if (check_condition(arole, *lvec) == false)
		{
			return false;
		}

		pbdb::db_task::data ltemp;
		ltemp.set_mtaskid(ataskid);
		ltemp.set_mreceiveutc((int32_t)localtime::gettime());
		ltemp.set_mfinshutc(-1);
		for (auto& item : *lvec)
		{
			task_check::schedules(arole, *ltemp.mutable_mschedules()->Add(), item);
		}
		MODIFIED_RETUAN(lptask, arole->m_task.get(), false);
		lptask->mutable_mrundatas()->insert({ ataskid, ltemp });
		return true;
	}

	bool static_task::reset_task(actor_role* arole, i32_taskid ataskid)
	{
		erase_task(arole, ataskid);
		receive_task(arole, ataskid);
		return true;
	}

	bool static_task::erase_task(actor_role* arole, i32_taskid ataskid)
	{
		finish_task(arole, ataskid);
		MODIFIED_RETUAN(lptask, arole->m_task.get(), false);
		lptask->mutable_mrundatas()->erase(ataskid);
		return true;
	}

	bool static_task::finish_task(actor_role* arole, i32_taskid ataskid)
	{
		if (ttab_task::instance().repeat(arole, ataskid) == false)
		{
			// # 完成任务前先查看是否已经完成了
			if (isfinish_task(arole, ataskid))
			{
				return false;
			}
		}
		
		// # 此任务是否已经接收
		if (isreceive_task(arole, ataskid) == false)
		{
			return false;
		}
		const std::vector<task_condition>* lvecfinish = ttab_task::instance().condition_complete(ataskid);
		if (lvecfinish != nullptr && check_condition(arole, *lvecfinish))
		{
			auto& lruntask = run(arole);
			auto itor = lruntask.find(ataskid);
			if (itor == lruntask.end())
			{
				return false;
			}
			// # 发送奖励
			const tab_task* tab = ttab_task::instance().tab(ataskid);
			if (tab == nullptr)
			{
				return false;
			}
			if (tab->m_autoreceive)
			{
				if (tab->m_mailid > 0)
				{
					// 发送邮件
					if (actor_mail::sendmail(arole->id_guid(), tab->m_mailid, tab->m_dropid, "") == false)
					{
						log_error()->print("task[{}] actor_mail::sendmail({},{},{})"
							, ataskid, arole->id_guid(), tab->m_mailid, tab->m_dropid
						);
						return false;
					}
					itor->second.set_mreceive(true);
				}
			}
			itor->second.set_mfinshutc((int32_t)localtime::gettime());
			*complete(arole)[ataskid].add_mhistory() = itor->second;
			run(arole).erase(itor);
			update_change(arole, ETaskTaskId, ataskid);
			return true;
		}
		return false;
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
				for (int i = 0; i < itorrun->second.mschedules_size(); ++i)
				{
					auto& lcomplete = run(arole);
					for (auto itor = lcomplete.begin(); itor != lcomplete.end(); ++itor)
					{
						for (pbdb::db_task::data_schedule& lschedule : *itor->second.mutable_mschedules())
						{
							if (lschedule.mtype() == atype)
							{
								const task_condition* lpcondition = ttab_task::instance().condition_complete(taskid, atype);
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
			std::set<i32_taskid>* ltaskset = ttab_task::instance().check(atype, avalues, true);
			if (ltaskset == nullptr)
			{
				return false;
			}
			update_change(arole, atype, ltaskset);
		}
		{
			std::set<i32_taskid>* ltaskset = ttab_task::instance().check(atype, avalues, false);
			if (ltaskset == nullptr)
			{
				return false;
			}
			update_change(arole, atype, ltaskset);
		}
		return true;
	}

	void task::initdata()
	{
		log_error()->print("task load finish {}", data());
		// ### 检查是否有可接受的任务
		ttab_task::instance().foreach([&](tab_task& atab)
			{
				// # 检查任务是否可接收
				if (static_task::receive_task(nactor(), atab.m_id) == true)
				{// 可接受或者已接受
					// # 检查任务是否可完成
					if (static_task::finish_task(nactor(), atab.m_id))
					{
						return;
					}
				}
			}
		);
	}
}// namespace ngl