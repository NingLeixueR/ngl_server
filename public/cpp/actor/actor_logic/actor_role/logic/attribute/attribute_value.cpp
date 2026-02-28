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
		return std::make_pair(em<EnumAttribute>::name(atype), em<EnumAttribute>::name(atype, 1));
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
		for (auto [_eattr, _value] : ar)
		{
			al[_eattr] += _value;
		}
	}

	void tools_attribute::add(std::map<EnumAttribute, float>& al, const std::map<EnumAttribute, float>& ar)
	{
		for (auto [_eattr, _value] : ar)
		{
			al[_eattr] += _value;
		}
	}

	void tools_attribute::dec(std::map<EnumAttribute, int64_t>& al, const std::map<EnumAttribute, int64_t>& ar)
	{
		for (auto [_eattr, _value] : ar)
		{
			al[_eattr] -= _value;
		}
	}

	void tools_attribute::dec(std::map<EnumAttribute, float>& al, const std::map<EnumAttribute, float>& ar)
	{
		for (auto [_eattr, _value] : ar)
		{
			al[_eattr] -= _value;
		}
	}

	int64_t tools_attribute::fight(EnumAttribute atype, double avalues)
	{
		const tab_attribute* tab = ttab_attribute::instance().attr(atype);
		if (tab == nullptr)
		{
			return 0;
		}
		return tab->m_fightcoefficient * avalues;
	}
}//namespace ngl

namespace ngl
{
	void attribute_value::update(std::map<EnumAttribute, int64_t>& aattr, const std::map<EnumAttribute, float>& amr)
	{
		for (auto [_eattr, _value] : amr)
		{
			auto itor = aattr.find(_eattr);
			if (itor != aattr.end())
			{
				itor->second += itor->second * _value;
				if (m_module == EnumModule::E_ModuleRoot)
				{
					itor->second = ttab_attribute::instance().uplowlimit(_eattr, itor->second);
				}
			}
		}
	}

	int64_t attribute_value::fight()
	{
		m_fightscore = 0;
		for (auto [_eattr, _value] : m_fight)
		{
			m_fightscore += tools_attribute::fight(_eattr, (double)_value);
		}
		return m_fightscore;
	}

	std::map<EnumAttribute, int64_t>& attribute_value::get_fight()
	{
		return m_fight;
	}

	int64_t attribute_value::update()
	{
		m_fight.clear();
		m_fight = m_attr;
		std::map<EnumAttribute, float> lrattr(m_rattr);
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
		m_crattr[amodule][atype] = (float)avalues;
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
		for (std::pair<const EnumAttribute, int64_t>& item : m_fight)
		{
			pbdb::ATTRIBUTE* lptemp = aunitmodule.add_mmodules();
			lptemp->set_mtype(item.first);
			lptemp->set_mvalue(item.second);
		}
	}
}//namespace ngl