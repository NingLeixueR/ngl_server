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
// File overview: Implements logic for task.


#include "actor/actor_logic/actor_role/logic/task/task.h"
#include "actor/actor_logic/actor_role/logic/bag/drop.h"
#include "actor/actor_logic/actor_role/actor_role.h"
#include "actor/actor_logic/actor_mail/actor_mail.h"
#include "actor/tab/ttab_task.h"
#include "actor/generated/pb/db.pb.h"

namespace ngl
{
	class task_check
	{
		static std::array<std::unique_ptr<task_check>, ETaskCount> m_data;
	public:
		// Conditionwhether
		static bool check(actor_role* arole, const task_condition& atab)
		{
			task_check* lcheck = m_data[atab.m_type].get();
			if (lcheck == nullptr)
			{
				return false;
			}
			if (atab.m_condition == ETaskConditionMore)
			{
				return lcheck->values(arole, atab) >= atab.m_parmint;
			}
			else if (atab.m_condition == ETaskConditionLess)
			{
				return lcheck->values(arole, atab) <= atab.m_parmint;
			}
			else if (atab.m_condition == ETaskConditionEqual)
			{
				return lcheck->values(arole, atab) == atab.m_parmint;
			}
			return false;
		}

		static void schedules(actor_role* arole, pbdb::db_task::data_schedule& adata, const task_condition& atab)
		{
			task_check* lcheck = m_data[atab.m_type].get();
			if (lcheck == nullptr)
			{
				return;
			}
			adata.set_mtype(atab.m_type);
			adata.set_msumint(atab.m_parmint);
			adata.set_mvalue(lcheck->values(arole, atab));
		}

		virtual int32_t values(actor_role* arole, const task_condition& atab) = 0;
	};

	// Condition:ETaskRoleLv playerlevel
	class taskcheck_rolelv : 
		public task_check
	{
			virtual int32_t values(actor_role* arole, [[maybe_unused]] const task_condition& atab)
		{
			return arole->m_info.lv();
		}
	};

	// Condition:ETaskRoleVip playerviplevel
	class taskcheck_rolevip : 
		public task_check
	{
			virtual int32_t values(actor_role* arole, [[maybe_unused]] const task_condition& atab)
		{
			return arole->m_info.vip();
		}
	};

	// Condition:ETaskTaskId complete IDtask
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
		for (const task_condition& litem : acondition)
		{
			if (task_check::check(arole, litem) == false)
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
		const auto& lcomplete = const_complete(arole);
		return lcomplete.find(ataskid) != lcomplete.end();
	}

	bool static_task::isreceive_task(actor_role* arole, i32_taskid ataskid)
	{
		const auto& lrun = const_run(arole);
		return lrun.find(ataskid) != lrun.end();
	}

	bool static_task::receive_task(actor_role* arole, i32_taskid ataskid)
	{
		if (ttab_task::instance().repeat(arole, ataskid) == false)
		{// ## Completetask
			// ## Taskbeforefirst whether complete
			if (isfinish_task(arole, ataskid))
			{
				return false;
			}
		}
		
		// ## This taskwhether
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
		ltemp.set_mreceiveutc((int32_t)tools::localtime::gettime());
		ltemp.set_mfinshutc(-1);
		for (const task_condition& litem : *lvec)
		{
			task_check::schedules(arole, *ltemp.mutable_mschedules()->Add(), litem);
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
			// # Completetaskbeforefirst whether complete
			if (isfinish_task(arole, ataskid))
			{
				return false;
			}
		}
		
		// # This taskwhether
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
			// # Sendreward
			const tab_task* tab = ttab_task::instance().tab(ataskid);
			if (tab == nullptr)
			{
				return false;
			}
			if (tab->m_autoreceive)
			{
				if (tab->m_mailid > 0)
				{
					// Sendmail
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
			itor->second.set_mfinshutc((int32_t)tools::localtime::gettime());
			auto& lcomplete = complete(arole);
			*lcomplete[ataskid].add_mhistory() = itor->second;
			lruntask.erase(itor);
			update_change(arole, ETaskTaskId, ataskid);
			return true;
		}
		return false;
	}

	bool static_task::update_change(actor_role* arole, ETask atype, std::set<i32_taskid>* ataskset)
	{
		const auto& lconstrun = const_run(arole);
		for (const i32_taskid ltaskid : *ataskset)
		{
			// ## Taskbeforefirst whether
			auto litrun = lconstrun.find(ltaskid);
			if (litrun != lconstrun.end())
			{
				// ## Task
				auto& lrun = run(arole);
				for (auto itor = lrun.begin(); itor != lrun.end(); ++itor)
				{
					const task_condition* lruncond = ttab_task::instance().condition_complete(itor->first, atype);
					for (pbdb::db_task::data_schedule& lschedule : *itor->second.mutable_mschedules())
					{
						if (lschedule.mtype() == atype)
						{
							if (lruncond != nullptr)
							{
								task_check::schedules(arole, lschedule, *lruncond);
							}
							break;
						}
					}
				}
				finish_task(arole, ltaskid);
				continue;
			}
			
			receive_task(arole, ltaskid);
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
		// ### Whether task
		ttab_task::instance().foreach([&](tab_task& atab)
			{
				// # Taskwhether
				if (static_task::receive_task(nactor(), atab.m_id) == true)
				{// Or
					// # Taskwhether complete
					if (static_task::finish_task(nactor(), atab.m_id))
					{
						return;
					}
				}
			}
		);
	}
}// namespace ngl
