#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	using i32_taskid = int32_t;
	using i32_rolelv = int32_t;
	using i32_rolevip = int32_t;

	struct ttab_task :
		public manage_csv<tab_task>
	{
		ttab_task(const ttab_task&) = delete;
		ttab_task& operator=(const ttab_task&) = delete;
		using type_tab = tab_task;

		// first	: 根据可接受条件进行分类 
		// second	: 根据可完成条件进行分类 
		using receive_complete = std::pair<std::set<i32_taskid>, std::set<i32_taskid>>;
		static std::map<ETask, int32_t> m_maxval;
		static std::map<ETask, std::map<int32_t, receive_complete>> m_map;

		ttab_task()
		{}

		static const std::map<int, tab_task>& tablecsv()
		{
			const ttab_task* ttab = allcsv::get<ttab_task>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_task* tab(int32_t aid)
		{
			const auto& lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		void set_data(int ataskid, task_condition& aitem, std::map<int32_t, receive_complete>& arc, bool areceive)
		{
			if (aitem.m_condition == ETaskConditionEqual)
			{
				arc[aitem.m_parmint].first.insert(ataskid);
			}
			else if (aitem.m_condition == ETaskConditionMore)
			{//>=
				auto itor = m_maxval.find(aitem.m_type);
				assert(itor != m_maxval.end());
				for (int i = aitem.m_parmint; i <= itor->second; ++i)
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

		void reload()final
		{

			m_maxval.clear();
			m_maxval.insert({ ETaskRoleLv, ttab_specialid::m_rolemaxlv });
			m_maxval.insert({ ETaskRoleVip, ttab_specialid::m_rolemaxvip });
			m_maxval.insert({ ETaskTaskId, -1 });

			for (std::pair<const int, tab_task>& pair : m_tablecsv)
			{
				tab_task& ltask = pair.second;
				for (task_condition& item : ltask.m_taskreceive)
				{
					set_data(ltask.m_id, item, m_map[item.m_type], true);
				}
				for (task_condition& item : ltask.m_taskcomplete)
				{
					set_data(ltask.m_id, item, m_map[item.m_type], false);
				}
			}
		}

		// 获取条件关联的所有任务
		static std::set<i32_taskid>* check(ETask atype, int32_t avalues, bool aisreceive)
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

		static const task_condition* condition_receive(i32_taskid ataskid, ETask atype)
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

		// # 获取任务接取条件
		static const std::vector<task_condition>* condition_receive(i32_taskid ataskid)
		{
			const tab_task* table = tab(ataskid);
			if (table == nullptr)
			{
				return nullptr;
			}
			return &table->m_taskreceive;
		}

		static const task_condition* condition_complete(i32_taskid ataskid, ETask atype)
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

		// # 获取任务完成条件
		static const std::vector<task_condition>* condition_complete(i32_taskid ataskid)
		{
			const tab_task* table = tab(ataskid);
			if (table == nullptr)
			{
				return nullptr;
			}
			return &table->m_taskcomplete;
		}

		// # 任务是否可重复完成
		static bool repeat(actor_role* rd, i32_taskid ataskid)
		{
			const tab_task* table = tab(ataskid);
			if (table == nullptr)
			{
				return false;
			}
			if (table->m_type == ETaskTypeRepeat || table->m_type == ETaskTypeDaily)
			{
				int32_t lmaxcount = tools::lexical_cast<int32_t>(table->m_typeparm);
				const auto& lcomplete = rd->m_task.get_consttask().m_completeddatas();
				auto itor = lcomplete.find(ataskid);
				if (itor != lcomplete.end())
				{
					return true;
				}
				int32_t lcount = 0;
				if (table->m_type == ETaskTypeDaily)
				{
					for (const auto& item : itor->second.m_history())
					{
						item.m_finshutc();
						if (localtime::issameday(item.m_finshutc()))
						{
							++lcount;
						}
					}
				}
				else
				{
					lcount = itor->second.m_history().size();
				}
				return lcount < lmaxcount;
			}
			return false;
		}
	};
}//namespace ngl
