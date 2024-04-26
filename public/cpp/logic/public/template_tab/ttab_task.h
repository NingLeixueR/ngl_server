#pragma once

#include "ttab_specialid.h"
#include "manage_csv.h"
#include "xmlnode.h"
#include "type.h"
#include "nlog.h"

namespace ngl
{
	using i32_taskid = int32_t;
	using i32_rolelv = int32_t;
	using i32_rolevip = int32_t;

	struct ttab_task : public manage_csv<tab_task>
	{
		// first	: ���ݿɽ����������з��� 
		// second	: ���ݿ�����������з��� 
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
					LogLocalError("task[%] [ETaskConditionMore] maxvalue == -1", ataskid);
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
						LogLocalError("task[%] [ETaskConditionMore] maxvalue == -1", ataskid);
						return;
					}
					arc[i].first.insert(ataskid);
				}
			}
		}

		static std::map<ETask, int32_t> m_maxval;
		static std::map<ETask, std::map<int32_t, receive_complete>> m_map;

		virtual void reload()
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
						LogLocalError("ttab_task m_maxval.find(%) == m_maxval.end()", (int32_t)item.m_type);
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
						LogLocalError("ttab_task taskid[%] m_maxval.find(%) == m_maxval.end()", ltask.m_id, (int32_t)item.m_type);
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

		// ��ȡ������������������
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

		static task_condition* get_task_condition(i32_taskid ataskid, ETask atype, bool aisreceive)
		{
			std::vector<task_condition>* lvec = get_task_condition(ataskid, aisreceive);
			if (lvec == nullptr)
				return nullptr;
			for (task_condition& item : *lvec)
			{
				if (item.m_type == atype)
					return &item;
			}
			return nullptr;
		}

		// ��ȡ����
		static std::vector<task_condition>* get_task_condition(i32_taskid ataskid, bool aisreceive)
		{
			tab_task* table = tab(ataskid);
			if (table == nullptr)
				return nullptr;
			return aisreceive ? &table->m_taskreceive : &table->m_taskcomplete;
		}
	};
}//namespace ngl