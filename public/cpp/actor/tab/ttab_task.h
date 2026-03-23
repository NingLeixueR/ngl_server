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
// File overview: Declares interfaces for tab.

#pragma once

#include "actor/tab/ttab_specialid.h"
#include "tools/log/nlog.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/tab/xml/xml.h"
#include "tools/type.h"

namespace ngl
{
	using i32_taskid = int32_t;
	using i32_rolelv = int32_t;
	using i32_rolevip = int32_t;

	struct ttab_task :
		public csv<tab_task>
	{
		ttab_task(const ttab_task&) = delete;
		ttab_task& operator=(const ttab_task&) = delete;

		// First: condition
		// Second: completecondition
		using receive_complete = std::pair<std::set<i32_taskid>, std::set<i32_taskid>>;
		std::map<ETask, int32_t> m_maxval;
		std::map<ETask, std::map<int32_t, receive_complete>> m_map;

		void reload()final
		{
			std::cout << "[ttab_task] reload" << std::endl;
			m_maxval.clear();
			m_maxval.insert({ ETaskRoleLv, ttab_specialid::instance().m_rolemaxlv });
			m_maxval.insert({ ETaskRoleVip, ttab_specialid::instance().m_rolemaxvip });
			m_maxval.insert({ ETaskTaskId, -1 });

			foreach([&](tab_task& atab)
				{
					for (task_condition& item : atab.m_taskreceive)
					{
						set_data(atab.m_id, item, m_map[item.m_type], true);
					}
					for (task_condition& item : atab.m_taskcomplete)
					{
						set_data(atab.m_id, item, m_map[item.m_type], false);
					}
				}
			);
		}
	public:
		using type_tab = tab_task;

		ttab_task() = default;

		static ttab_task& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_task>();
			}
			return *ncsv::get<ttab_task>();
		}

		// # std::map<int, tab_task>& tabs()
		// # tab_task* tab(int aid)

		void set_data(int ataskid, task_condition& aitem, std::map<int32_t, receive_complete>& arc, bool areceive)
		{
			if (aitem.m_condition == ETaskConditionEqual)
			{
				if (areceive)
				{
					arc[aitem.m_parmint].first.insert(ataskid);
				}
				else
				{
					arc[aitem.m_parmint].second.insert(ataskid);
				}
			}
			else if (aitem.m_condition == ETaskConditionMore)
			{//>=
				auto litor = m_maxval.find(aitem.m_type);
				if (litor == m_maxval.end())
				{
					log_error()->print(
						"ttab_task::set_data invalid type [{}] task [{}]",
						static_cast<int32_t>(aitem.m_type),
						ataskid);
					return;
				}
				for (int i = aitem.m_parmint; i <= litor->second; ++i)
				{
					if (areceive)
					{
						arc[i].first.insert(ataskid);
					}
					else
					{
						arc[i].second.insert(ataskid);
					}
				}
			}
			else if (aitem.m_condition == ETaskConditionLess)
			{//<=
				for (int i = aitem.m_parmint; i > 0; --i)
				{
					if (areceive)
					{
						arc[i].first.insert(ataskid);
					}
					else
					{
						arc[i].second.insert(ataskid);
					}
				}
			}
		}

		// Getcondition alltask
		std::set<i32_taskid>* check(ETask atype, int32_t avalues, bool aisreceive)
		{
			auto itor = m_map.find(atype);
			if (itor == m_map.end())
			{
				return nullptr;
			}
			auto itor_set = itor->second.find(avalues);
			if (itor_set == itor->second.end())
			{
				return nullptr;
			}
			receive_complete& lpaor = itor_set->second;
			return aisreceive ? &lpaor.first : &lpaor.second;
		}

		const task_condition* condition_receive(i32_taskid ataskid, ETask atype)
		{
			const std::vector<task_condition>* lvec = condition_receive(ataskid);
			if (lvec == nullptr)
			{
				return nullptr;
			}
			for (const task_condition& item : *lvec)
			{
				if (item.m_type == atype)
				{
					return &item;
				}
			}
			return nullptr;
		}

		// # Gettask condition
		const std::vector<task_condition>* condition_receive(i32_taskid ataskid)
		{
			const tab_task* table = tab(ataskid);
			if (table == nullptr)
			{
				return nullptr;
			}
			return &table->m_taskreceive;
		}

		const task_condition* condition_complete(i32_taskid ataskid, ETask atype)
		{
			const std::vector<task_condition>* lvec = condition_complete(ataskid);
			if (lvec == nullptr)
			{
				return nullptr;
			}
			for (const task_condition& item : *lvec)
			{
				if (item.m_type == atype)
				{
					return &item;
				}
			}
			return nullptr;
		}

		// # Gettaskcompletecondition
		const std::vector<task_condition>* condition_complete(i32_taskid ataskid)
		{
			const tab_task* table = tab(ataskid);
			if (table == nullptr)
			{
				return nullptr;
			}
			return &table->m_taskcomplete;
		}

		// # Taskwhether complete
		bool repeat(actor_role* rd, i32_taskid ataskid)
		{
			const tab_task* table = tab(ataskid);
			if (table == nullptr)
			{
				return false;
			}
			if (table->m_type == ETaskTypeRepeat || table->m_type == ETaskTypeDaily)
			{
				int32_t lmaxcount = tools::lexical_cast<int32_t>(table->m_typeparm);

				MODIFIED_RETURN_CONST(lptaskconst, rd->m_task.get(), false);
				const auto& lcomplete = lptaskconst->mcompleteddatas();
				auto itor = lcomplete.find(ataskid);
				if (itor != lcomplete.end())
				{
					return true;
				}
				int32_t lcount = 0;
				if (table->m_type == ETaskTypeDaily)
				{
					for (auto& item : itor->second.mhistory())
					{
						item.mfinshutc();
						if (localtime::issameday(item.mfinshutc()))
						{
							++lcount;
						}
					}
				}
				else
				{
					lcount = itor->second.mhistory().size();
				}
				return lcount < lmaxcount;
			}
			return false;
		}
	};
}//namespace ngl
