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
#pragma once

#include "ttab_attribute.h"
#include "manage_csv.h"
#include "nlog.h"
#include "net.h"

#include <functional>
#include <vector>
#include <array>
#include <set>
#include <map>

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
		// # [absolute] 属性绝对值  
		std::map<EnumAttribute, int64_t> m_attr;
		// # 万分比属性[自身提供的]
		std::map<EnumAttribute, float> m_rattr;
		// # 万分比属性[别人提供的]
		std::map<EnumAttribute, float> m_orattr;
		// ## 以下比例属性 是往父链上添加的属性 ##
		// # key:哪个模块加的比例属性
		std::map<EnumModule, std::map<EnumAttribute, float>> m_crattr;		// 往父链的[万分比属性]上添加 
		// # [m_attr+m_rattr] 产生的属性
		std::map<EnumAttribute, int64_t> m_fight;
		// # 战力
		int64_t m_fightscore = 0;

		EnumModule m_module = EnumModule::E_ModuleNull;
	public:
		attribute_value()
		{}
	private:
		void update(std::map<EnumAttribute, int64_t>& aattr, const std::map<EnumAttribute, float>& amr)
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
		std::map<EnumAttribute, int64_t>& get_fight()
		{
			return m_fight;
		}

		int64_t update()
		{
			m_fight.clear();
			m_fight = m_attr;
			std::map<EnumAttribute, float> lrattr(m_rattr);
			ttab_attribute::instance().add(lrattr, m_orattr);
			update(m_fight, lrattr);
			return fight();
		}

		// 打印属性
		void printf()
		{
			log_error()->print("attr # {}", m_fight);
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
			aunitmodule.set_mmtype(m_module);
			for (std::pair<const EnumAttribute, int64_t>& item : m_fight)
			{
				pbnet::UnitAttribute* lptemp = aunitmodule.add_mmodules();
				lptemp->set_mtype(item.first);
				lptemp->set_mvalue(item.second);
			}
		}
	};
}// namespace ngl

template <>
struct std::formatter<ngl::EnumAttribute>
{
	constexpr auto parse(const std::format_parse_context& ctx)const
	{
		return ctx.begin();
	}

	auto format(const ngl::EnumAttribute aval, std::format_context& ctx)const
	{
		return std::format_to(ctx.out(), "{}", ngl::enum_attr_name::name(aval));
	}
};