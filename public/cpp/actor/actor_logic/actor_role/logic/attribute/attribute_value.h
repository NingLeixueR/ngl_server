#pragma once

#include <functional>
#include <vector>
#include <array>
#include <set>
#include <map>

#include "ttab_attribute.h"
#include "manage_csv.h"
#include "nlog.h"
#include "net.h"

namespace ngl
{
	class enum_attr_name
	{
		static std::array<std::string, E_Count> m_arr;
	public:
		static void init()
		{
			m_arr[E_Attack]		= "攻击";
			m_arr[E_Defense]	= "防御";
			m_arr[E_Hp]			= "血量";
			m_arr[E_Anger]		= "怒气";
			m_arr[E_Speed]		= "速度";
		}

		static const char* name(EnumAttribute atype)
		{
			return m_arr[atype].c_str();
		}
	};

	struct attribute_value
	{
		friend class attribute;
	private:
		// ### [absolute] 属性绝对值  
		map_attrvalue m_attr;
		// ### 万分比属性[自身提供的]
		map_attrratio m_rattr;
		// ### 万分比属性[别人提供的]
		map_attrratio m_orattr;
		// ########### 以下比例属性 是往父链上添加的属性 ##########
		// ### key:哪个模块加的比例属性
		map_moduleratio m_crattr;		// 往父链的[万分比属性]上添加 
		// #### [m_attr+m_rattr] 产生的属性
		map_attrvalue m_fight;
		// ### 战力
		int64_t m_fightscore;

		EnumModule m_module;
	public:
		attribute_value() :
			m_fightscore(0),
			m_module(EnumModule::E_ModuleNull)
		{}
	private:
		void update(map_attrvalue& aattr, const map_attrratio& amr)
		{
			for (const auto& [key, value] : amr)
			{
				auto itor = aattr.find(key);
				if (itor != aattr.end())
				{
					itor->second += itor->second * value;
					if (m_module == EnumModule::E_ModuleRoot)
					{
						itor->second = ttab_attribute::instance().uplowlimit(key, itor->second);
					}
				}
			}
		}

		int64_t fight(EnumAttribute atype, double avalues)
		{
			const tab_attribute* tab = ttab_attribute::instance().attr(atype);
			if (tab == nullptr)
			{
				return 0;
			}
			return tab->m_fightcoefficient * avalues;
		}

		int64_t fight()
		{
			m_fightscore = 0;
			for (const auto& [key, value] : m_fight)
			{
				m_fightscore += fight(key, (double)value);
			}
			return m_fightscore;
		}
	public:
		map_attrvalue& get_fight()
		{
			return m_fight;
		}

		int64_t update()
		{
			m_fight.clear();
			m_fight = m_attr;
			map_attrratio lrattr(m_rattr);
			ttab_attribute::instance().add(lrattr, m_orattr);
			update(m_fight, lrattr);
			return fight();
		}

		// 打印属性
		void printf()
		{
			auto lstream = log_error();
			(*lstream) << "##############" << std::endl;
			for (const auto& [key, values] : m_fight)
			{
				(*lstream) << std::format("[{}]:[{}]", enum_attr_name::name(key), values) << std::endl;
			}
			(*lstream) << "fight:" << m_fightscore << std::endl;
			(*lstream) << "##############" << std::endl;
			(*lstream).print("");
		}

		// 清空属性
		void clear()
		{
			m_attr.clear();
			m_rattr.clear();
			m_crattr.clear();
		}


		// 添加属性
		void set_attr(EnumAttribute atype, double avalues)
		{
			m_attr[atype] = (int64_t)avalues;
		}

		// 添加比例属性属性
		void set_rattr(EnumAttribute atype, double avalues)
		{
			m_rattr[atype] = (float)avalues;
		}

		// 给父结点添加属性
		void set_father_rattr(EnumModule amodule, EnumAttribute atype, double avalues)
		{
			m_crattr[amodule][atype] = (float)avalues;
		}

		// 获取属性
		double get_attr(EnumAttribute atype)
		{
			return (double)m_attr[atype];
		}

		// 获取比例属性属性
		double get_rattr(EnumAttribute atype)
		{
			return m_rattr[atype];
		}

		// 获取父结点添属性
		double get_father_rattr(EnumModule amodule, EnumAttribute atype)
		{
			return m_crattr[amodule][atype];
		}

		void topb(pbnet::UnitModule& aunitmodule)
		{
			aunitmodule.set_m_mtype(m_module);
			for (std::pair<const EnumAttribute, int64_t>& item : m_fight)
			{
				pbnet::UnitAttribute* lptemp = aunitmodule.add_m_modules();
				lptemp->set_m_type(item.first);
				lptemp->set_m_value(item.second);
			}
		}
	};
}// namespace ngl