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
// File overview: Implements logic for attribute.


#include "actor/actor_logic/actor_role/logic/attribute/attribute_value.h"

namespace ngl
{
	void enum_attr_name::init()
	{
		em<EnumAttribute>::set(E_Attack, "attack");
		em<EnumAttribute>::set(E_Defense, "defense");
		em<EnumAttribute>::set(E_Hp, "hp");
		em<EnumAttribute>::set(E_Anger, "anger");
		em<EnumAttribute>::set(E_Speed, "speed");
		em<EnumAttribute>::set(E_CooldownAbsolute, "cooldownabsolute");
		em<EnumAttribute>::set(E_CooldownProp, "cooldownprop");

		em<EnumAttribute>::set(E_Attack, "攻击", 1);
		em<EnumAttribute>::set(E_Defense, "防御", 1);
		em<EnumAttribute>::set(E_Hp, "血量", 1);
		em<EnumAttribute>::set(E_Anger, "怒气", 1);
		em<EnumAttribute>::set(E_Speed, "速度", 1);
		em<EnumAttribute>::set(E_CooldownAbsolute, "冷却缩减-绝对值", 1);
		em<EnumAttribute>::set(E_CooldownProp, "冷却缩减-万分比", 1);
	}

	std::pair<const char*, const char*> enum_attr_name::name(EnumAttribute atype)
	{
		return { em<EnumAttribute>::name(atype), em<EnumAttribute>::name(atype, 1) };
	}

	EnumAttribute enum_attr_name::type(const char* aname)
	{
		EnumAttribute lvalue = em<EnumAttribute>::get_enum(aname);
		if (lvalue != null_type())
		{
			return lvalue;
		}
		return em<EnumAttribute>::get_enum(aname, 1);
	}

	EnumAttribute enum_attr_name::null_type()
	{
		return em<EnumAttribute>::enum_null();
	}

}// namespace ngl

namespace ngl
{
	void tools_attribute::add(std::map<EnumAttribute, int64_t>& al, const std::map<EnumAttribute, int64_t>& ar)
	{
		for (const auto& [lattr, lvalue] : ar)
		{
			al[lattr] += lvalue;
		}
	}

	void tools_attribute::add(std::map<EnumAttribute, float>& al, const std::map<EnumAttribute, float>& ar)
	{
		for (const auto& [lattr, lvalue] : ar)
		{
			al[lattr] += lvalue;
		}
	}

	void tools_attribute::dec(std::map<EnumAttribute, int64_t>& al, const std::map<EnumAttribute, int64_t>& ar)
	{
		for (const auto& [lattr, lvalue] : ar)
		{
			al[lattr] -= lvalue;
		}
	}

	void tools_attribute::dec(std::map<EnumAttribute, float>& al, const std::map<EnumAttribute, float>& ar)
	{
		for (const auto& [lattr, lvalue] : ar)
		{
			al[lattr] -= lvalue;
		}
	}

	int64_t tools_attribute::fight(EnumAttribute atype, double avalues)
	{
		const tab_attribute* tab = ttab_attribute::instance().attr(atype);
		if (tab == nullptr)
		{
			return 0;
		}
		return static_cast<int64_t>(tab->m_fightcoefficient * avalues);
	}
}//namespace ngl

namespace ngl
{
	void attribute_value::update(std::map<EnumAttribute, int64_t>& aattr, const std::map<EnumAttribute, float>& amr)
	{
		for (const auto& [lattr, lvalue] : amr)
		{
			auto lit = aattr.find(lattr);
			if (lit != aattr.end())
			{
				lit->second += static_cast<int64_t>(lit->second * lvalue);
				if (m_module == EnumModule::E_ModuleRoot)
				{
					lit->second = ttab_attribute::instance().uplowlimit(lattr, static_cast<int32_t>(lit->second));
				}
			}
		}
	}

	int64_t attribute_value::fight()
	{
		m_fightscore = 0;
		for (const auto& [lattr, lvalue] : m_fight)
		{
			m_fightscore += tools_attribute::fight(lattr, static_cast<double>(lvalue));
		}
		return m_fightscore;
	}

	std::map<EnumAttribute, int64_t>& attribute_value::get_fight()
	{
		return m_fight;
	}

	int64_t attribute_value::update()
	{
		m_fight = m_attr;
		std::map<EnumAttribute, float> lrattr = m_rattr;
		tools_attribute::add(lrattr, m_orattr);
		update(m_fight, lrattr);
		return fight();
	}

	void attribute_value::printf()
	{
		log_error()->print("attr # {}", m_fight);
	}

	void attribute_value::clear()
	{
		m_attr.clear();
		m_rattr.clear();
		m_crattr.clear();
	}

	void attribute_value::set_attr(EnumAttribute atype, double avalues)
	{
		m_attr[atype] = (int64_t)avalues;
	}

	double attribute_value::get_attr(EnumAttribute atype)
	{
		return (double)m_attr[atype];
	}

	void attribute_value::set_rattr(EnumAttribute atype, double avalues)
	{
		m_rattr[atype] = (float)avalues;
	}

	double attribute_value::get_rattr(EnumAttribute atype)
	{
		return m_rattr[atype];
	}

	void attribute_value::set_father_rattr(EnumModule amodule, EnumAttribute atype, double avalues)
	{
		auto& lrattr = m_crattr[amodule];
		lrattr[atype] = static_cast<float>(avalues);
	}

	double attribute_value::get_father_rattr(EnumModule amodule, EnumAttribute atype)
	{
		return m_crattr[amodule][atype];
	}

	int64_t attribute_value::fightscore()
	{
		return m_fightscore;
	}

	void attribute_value::topb(pbdb::UNIT_MODULE& aunitmodule)
	{
		aunitmodule.set_mmtype(m_module);
		for (const auto& litem : m_fight)
		{
			pbdb::ATTRIBUTE* lptemp = aunitmodule.add_mmodules();
			lptemp->set_mtype(litem.first);
			lptemp->set_mvalue(litem.second);
		}
	}
}//namespace ngl
