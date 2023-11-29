#include "attribute.h"

namespace ngl
{
	attribute::attribute() :
		m_issync(false)
	{}

	bool attribute::issync()
	{
		return m_issync;
	}

	void attribute::set_issync(bool aissync)
	{
		m_issync = aissync;
	}

	attribute_value& attribute::root()
	{
		return m_moduledata[EnumModule::E_ModuleRoot];
	}

	void attribute::add_module(EnumModule aenum)
	{
		auto itor = m_moduledata.find(aenum);
		if (itor == m_moduledata.end())
			return;
		for (const auto& [key, value] : itor->second.m_crattr)
		{
			ttab_attribute::add(m_moduledata[key].m_orattr, value);
		}
	}

	void attribute::dec_module(EnumModule aenum)
	{
		auto itor = m_moduledata.find(aenum);
		if (itor == m_moduledata.end())
			return;
		ttab_attribute::dec(root().m_attr, itor->second.m_fight);
		for (const auto& [key, value] : itor->second.m_crattr)
		{
			ttab_attribute::dec(m_moduledata[key].m_orattr, value);
			if (key != EnumModule::E_ModuleRoot)
				ttab_attribute::dec(root().m_attr, m_moduledata[key].m_fight);
		}
	}

	bool attribute::is_module_root(EnumModule aenum)
	{
		return aenum == EnumModule::E_ModuleRoot;
	}

	bool attribute::is_module_null(EnumModule aenum)
	{
		return aenum == EnumModule::E_ModuleNull;
	}

	void attribute::update_module(EnumModule aenum)
	{
		auto itor = m_moduledata.find(aenum);
		if (itor == m_moduledata.end())
			return;
		itor->second.update();
		ttab_attribute::add(root().m_attr, itor->second.m_fight);
		for (auto&& [key, value] : itor->second.m_crattr)
		{
			ttab_attribute::add(m_moduledata[key].m_orattr, value);
			if (is_module_root(key) == false)
			{
				m_moduledata[key].update();
				ttab_attribute::add(root().m_attr, m_moduledata[key].m_fight);
			}
		}

		root().update();
	}

	void attribute::init_data(EnumModule aenum, attribute_value& avalue)
	{
		m_moduledata[aenum] = avalue;
		m_moduledata[aenum].m_module = aenum;
	}

	// #### 计算输入的模块属性 生成最终属性与战力
	void attribute::init()
	{
		for (EnumModule i = E_ModuleRoot;
			i < E_ModuleCount;
			i = (EnumModule)(i + 1))
		{
			add_module(i);
		}

		for (auto&& [key, value] : m_moduledata)
		{
			if (key == E_ModuleRoot)
				continue;
			value.update();
			ttab_attribute::add(root().m_attr, value.m_fight);
		}
		root().update();
	}

	void attribute::updata(EnumModule aenum, attribute_value& avalue)
	{
		dec_module(aenum);
		avalue.m_orattr.swap(m_moduledata[aenum].m_orattr);
		m_moduledata[aenum] = avalue;
		m_moduledata[aenum].m_module = aenum;
		update_module(aenum);
		m_issync = true;
	}

	int64_t attribute::fight()
	{
		return root().m_fightscore;
	}

	const map_attr& attribute::get_attribute()
	{
		return root().m_fight;
	}

	//EnumModule aenum
	const map_attr& attribute::get_attribute(EnumModule aenum)
	{
		return m_moduledata[aenum].m_fight;
	}

	void attribute::printf()
	{
		root().printf();
	}



	attribute_value init_attribute_value(EnumModule amodule, double avalues)
	{
		attribute_value lvalue;
		lvalue.set_attr(EnumAttribute::E_Attack, avalues);
		lvalue.set_attr(EnumAttribute::E_Anger, avalues);
		lvalue.set_attr(EnumAttribute::E_Defense, avalues);
		lvalue.set_attr(EnumAttribute::E_Hp, avalues);
		lvalue.set_attr(EnumAttribute::E_Speed, avalues);

		lvalue.set_rattr(EnumAttribute::E_Attack, avalues / 100);
		lvalue.set_rattr(EnumAttribute::E_Anger, avalues / 100);
		lvalue.set_rattr(EnumAttribute::E_Defense, avalues / 100);
		lvalue.set_rattr(EnumAttribute::E_Hp, avalues / 100);
		lvalue.set_rattr(EnumAttribute::E_Speed, avalues / 100);

		
		
		for (EnumModule j = EnumModule::E_ModuleNull;
			j != amodule;
			j = (EnumModule)(j+1))
		{
			if (j == EnumModule::E_ModuleNull)
				continue;
			lvalue.set_father_rattr(j, EnumAttribute::E_Attack, avalues / 100);
			lvalue.set_father_rattr(j, EnumAttribute::E_Anger, avalues / 100);
			lvalue.set_father_rattr(j, EnumAttribute::E_Defense, avalues / 100);
			lvalue.set_father_rattr(j, EnumAttribute::E_Hp, avalues / 100);
			lvalue.set_father_rattr(j, EnumAttribute::E_Speed, avalues / 100);
		}
		return lvalue;
	}
	void test_attribute()
	{
		attribute lattr;

		// ## E_ModuleBuff
		for (EnumModule i = EnumModule::E_ModuleRoot;
			i < EnumModule::E_ModuleCount; i = (EnumModule)(i+1))
		{
			attribute_value lvalue = init_attribute_value(i, 10);
			lattr.init_data(i, lvalue);
		}
		lattr.init();
		lattr.printf();
		{
			attribute_value lvalue = init_attribute_value(EnumModule::E_ModuleEquipHelmet, 10);
			lattr.updata(EnumModule::E_ModuleEquipHelmet, lvalue);
		}
		lattr.printf();
		{
			attribute_value lvalue = init_attribute_value(EnumModule::E_ModuleBuff, -20);
			lattr.updata(EnumModule::E_ModuleBuff, lvalue);
		}
		lattr.printf();

		attribute lattr2;
		for (EnumModule i = EnumModule::E_ModuleRoot;
			i < EnumModule::E_ModuleCount; i = (EnumModule)(i + 1))
		{
			attribute_value lvalue = init_attribute_value(i, i != EnumModule::E_ModuleBuff ? 10 : -20);
			lattr2.init_data(i, lvalue);
		}
		lattr2.init();

		lattr2.printf();
	}
}