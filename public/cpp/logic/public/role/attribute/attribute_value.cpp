#include "attribute_value.h"

namespace ngl
{
	attribute_value::attribute_value() :
		m_fightscore(0),
		m_module(EnumModule::E_ModuleNull)
	{}

	void attribute_value::update(map_attribute& aattr, const map_ratio& amr)
	{
		for (const auto& [key, value] : amr)
		{
			auto itor = aattr.find(key);
			if (itor != aattr.end())
			{
				itor->second += itor->second * value;
				if (m_module == EnumModule::E_ModuleRoot)
					itor->second = ttab_attribute::uplowlimit(key, itor->second);
			}
		}
	}

	int64_t attribute_value::fight(EnumAttribute atype, double avalues)
	{
		tab_attribute* tab = ttab_attribute::attr(atype);
		if (tab == nullptr)
			return 0;
		return tab->m_fightcoefficient * avalues;
	}

	int64_t attribute_value::fight()
	{
		m_fightscore = 0;
		for (const auto& [key, value] : m_fight)
		{
			m_fightscore += fight(key, value);
		}
		return m_fightscore;
	}

	int64_t attribute_value::update()
	{
		m_fight.clear();
		m_fight = m_attr;
		map_ratio lrattr(m_rattr);
		ttab_attribute::add(lrattr, m_orattr);
		update(m_fight, lrattr);
		return fight();
	}

	void attribute_value::printf()
	{
		auto lstream = log();
		(*lstream) << "##############" << std::endl;
		std::map<EnumAttribute, std::string> lmapname
		{
			{E_Null, "无"},
			{E_Attack, "攻击"},
			{E_Defense, "防御"},
			{E_Hp, "血量"},
			{E_Anger, "怒气"},
			{E_Speed, "速度"},
		};
		for (auto&& [key, values] : m_fight)
		{
			(*lstream) << "[" << lmapname[key] << "]:[" << values << "]" << std::endl;
		}
		(*lstream) << "fight:" << m_fightscore << std::endl;
		(*lstream) << "##############" << std::endl;
		(*lstream).error("");
	}

	void attribute_value::clear()
	{
		m_attr.clear();
		m_rattr.clear();
		m_crattr.clear();
	}

	// 添加属性
	void attribute_value::set_attr(EnumAttribute atype, double avalues)
	{
		m_attr[atype] = avalues;
	}

	void attribute_value::set_rattr(EnumAttribute atype, double avalues)
	{
		m_rattr[atype] = avalues;
	}

	void attribute_value::set_father_rattr(EnumModule amodule, EnumAttribute atype, double avalues)
	{
		m_crattr[amodule][atype] = avalues;
	}

	double attribute_value::get_attr(EnumAttribute atype)
	{
		return m_attr[atype];
	}

	double attribute_value::get_rattr(EnumAttribute atype)
	{
		return m_rattr[atype];
	}

	double attribute_value::get_father_rattr(EnumModule amodule, EnumAttribute atype)
	{
		return m_crattr[amodule][atype];
	}
}// namespace ngl