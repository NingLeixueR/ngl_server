#pragma once

#include "ttab_specialid.h"
#include "manage_csv.h"
#include "type.h"
#include "nlog.h"
#include "xml.h"

namespace ngl
{
	using i32_taskid = int32_t;
	using i32_rolelv = int32_t;
	using i32_rolevip = int32_t;

	struct ttab_task : public manage_csv<tab_task>
	{
		ttab_task(const ttab_task&) = delete;
		ttab_task& operator=(const ttab_task&) = delete;

		// first	: 根据可接受条件进行分类 
		// second	: 根据可完成条件进行分类 
		using receive_complete = std::pair<std::set<i32_taskid>, std::set<i32_taskid>>;

		ttab_task()
		{}

		
		void set_data(int ataskid, task_condition& aitem, std::map<int32_t, receive_complete>& arc, bool areceive, int amaxvalue)
		{
			if (aitem.m_condition == ETaskConditionEqual)
			{
				arc[aitem.m_parmint].first.insert(ataskid);
			}
			else if (aitem.m_condition == ETaskConditionMore)
			{//>=
				if (amaxvalue == -1)
				{
					log_error()->print("task[{}] [ETaskConditionMore] maxvalue == -1", ataskid);
					return;
				}
				for (int i = aitem.m_parmint; i > amaxvalue; ++i)
				{
					arc[i].first.insert(ataskid);
				}
			}
			else if (aitem.m_condition == ETaskConditionLess)
			{//<=
				for (int i = aitem.m_parmint; i > 0; --i)
				{
					if (amaxvalue == -1)
					{
						log_error()->print("task[{}] [ETaskConditionMore] maxvalue == -1", ataskid);
						return;
					}
					arc[i].first.insert(ataskid);
				}
			}
		}

		static std::map<ETask, int32_t> m_maxval;
		static std::map<ETask, std::map<int32_t, receive_complete>> m_map;

		void reload()final
		{

			m_maxval.clear();
			m_maxval.insert({ ETaskRoleLv, ttab_specialid::m_rolemaxlv });
			m_maxval.insert({ ETaskRoleVip, ttab_specialid::m_rolemaxvip });
			m_maxval.insert({ ETaskTaskId, -1 });

			for (std::pair<const int, tab_task>& pair : tablecsv)
			{
				tab_task& ltask = pair.second;
				for (task_condition& item : ltask.m_taskreceive)
				{
					if (m_maxval.find(item.m_type) == m_maxval.end())
					{
						log_error()->print("ttab_task m_maxval.find({}) == m_maxval.end()", (int32_t)item.m_type);
						continue;
					}
					set_data(ltask.m_id
						, item
						, m_map[item.m_type]
						, true
						, m_maxval[item.m_type]
					);
				}
				for (task_condition& item : ltask.m_taskcomplete)
				{
					if (m_maxval.find(item.m_type) == m_maxval.end())
					{
						log_error()->print("ttab_task taskid[{}] m_maxval.find({}) == m_maxval.end()", ltask.m_id, (int32_t)item.m_type);
						continue;
					}
					set_data(ltask.m_id
						, item
						, m_map[item.m_type]
						, false
						, m_maxval[item.m_type]
					);
				}
			}
		}

		static tab_task* tab(i32_taskid ataskid)
		{
			ttab_task* ttab = allcsv::get<ttab_task>();
			assert(ttab != nullptr);
			auto itor = ttab->tablecsv.find(ataskid);
			return &itor->second;
		}

		// 获取条件关联的所有任务
		static std::set<i32_taskid>* check(ETask atype, int32_t avalues, bool aisreceive)
		{
			auto itor = m_map.find(atype);
			if (itor == m_map.end())
				return nullptr;
			auto itor_set = itor->second.find(avalues);
			if (itor_set == itor->second.end())
				return nullptr;
			receive_complete& lpaor = itor_set->second;
			return aisreceive ? &lpaor.first : &lpaor.second;
		}

		static task_condition* condition_receive(i32_taskid ataskid, ETask atype)
		{
			std::vector<task_condition>* lvec = condition_receive(ataskid);
			if (lvec == nullptr)
				return nullptr;
			for (task_condition& item : *lvec)
			{
				if (item.m_type == atype)
					return &item;
			}
			return nullptr;
		}

		// # 获取任务接取条件
		static std::vector<task_condition>* condition_receive(i32_taskid ataskid)
		{
			tab_task* table = tab(ataskid);
			if (table == nullptr)
				return nullptr;
			return &table->m_taskreceive;
		}

		static task_condition* condition_complete(i32_taskid ataskid, ETask atype)
		{
			std::vector<task_condition>* lvec = condition_complete(ataskid);
			if (lvec == nullptr)
				return nullptr;
			for (task_condition& item : *lvec)
			{
				if (item.m_type == atype)
					return &item;
			}
			return nullptr;
		}

		// # 获取任务完成条件
		static std::vector<task_condition>* condition_complete(i32_taskid ataskid)
		{
			tab_task* table = tab(ataskid);
			if (table == nullptr)
				return nullptr;
			return &table->m_taskcomplete;
		}
	};
}//namespace ngl
