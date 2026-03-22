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


#include "actor/actor_logic/actor_role/logic/attribute/attribute.h"

namespace ngl
{
	attribute_value& attribute::root()
	{
		return m_moduledata[EnumModule::E_ModuleRoot];
	}

	void attribute::module_add(EnumModule aenum)
	{
		auto itor = m_moduledata.find(aenum);
		if (itor == m_moduledata.end())
		{
			return;
		}
		for (auto& [_enum, _map] : itor->second.m_crattr)
		{
			tools_attribute::add(m_moduledata[_enum].m_orattr, _map);
		}
	}

	void attribute::module_dec(EnumModule aenum)
	{
		auto itor = m_moduledata.find(aenum);
		if (itor == m_moduledata.end())
		{
			return;
		}
		tools_attribute::dec(root().m_attr, itor->second.m_fight);
		for (auto& [_enum, _map] : itor->second.m_crattr)
		{
			tools_attribute::dec(m_moduledata[_enum].m_orattr, _map);
			if (_enum != EnumModule::E_ModuleRoot)
			{
				tools_attribute::dec(root().m_attr, m_moduledata[_enum].m_fight);
			}
		}
	}

	bool attribute::module_is_root(EnumModule aenum)
	{
		return aenum == EnumModule::E_ModuleRoot;
	}

	bool attribute::module_is_null(EnumModule aenum)
	{
		return aenum == EnumModule::E_ModuleNull;
	}

	void attribute::module_update(EnumModule aenum)
	{
		auto itor = m_moduledata.find(aenum);
		if (itor == m_moduledata.end())
		{
			return;
		}
		itor->second.update();
		tools_attribute::add(root().m_attr, itor->second.m_fight);
		for (auto& [_enum, _map] : itor->second.m_crattr)
		{
			tools_attribute::add(m_moduledata[_enum].m_orattr, _map);
			if (module_is_root(_enum) == false)
			{
				m_moduledata[_enum].update();
				tools_attribute::add(root().m_attr, m_moduledata[_enum].m_fight);
			}
		}
		root().update();
	}

	bool attribute::sync()
	{
		return m_sync;
	}

	void attribute::set_sync(bool async)
	{
		m_sync = async;
	}

	void attribute::init_data(EnumModule aenum, attribute_value& avalue)
	{
		if (aenum == E_ModuleBuff)
		{
			for (int i = E_ModuleBuffStart; i <= E_ModuleBuffFinish; ++i)
			{
				if (!m_moduledata.contains((EnumModule)i))
				{
					m_moduledata[(EnumModule)i] = avalue;
					m_moduledata[(EnumModule)i].m_module = aenum;
				}
			}
		}
		else
		{
			m_moduledata[aenum] = avalue;
			m_moduledata[aenum].m_module = aenum;
		}		
	}

	void attribute::init()
	{
		for (EnumModule i = E_ModuleRoot; i < E_ModuleCount; i = (EnumModule)(i + 1))
		{
			module_add(i);
		}
		for (std::pair<const EnumModule, attribute_value>& lpair : m_moduledata)
		{
			if (lpair.first == E_ModuleRoot)
			{
				continue;
			}
			lpair.second.update();
			tools_attribute::add(root().m_attr, lpair.second.m_fight);
		}
		root().update();
	}

	void attribute::updata(EnumModule aenum, attribute_value& avalue)
	{
		module_dec(aenum);
		avalue.m_orattr = m_moduledata[aenum].m_orattr;
		avalue.m_orattr.swap(m_moduledata[aenum].m_orattr);
		m_moduledata[aenum] = avalue;
		m_moduledata[aenum].m_module = aenum;
		module_update(aenum);
		m_sync = true;
	}

	void attribute::remove(EnumModule aenum)
	{
		module_dec(aenum);
	}

	int64_t attribute::fight()
	{
		return root().fightscore();
	}

	const std::map<EnumAttribute, int64_t>& attribute::get_attribute()
	{
		return root().get_fight();
	}

	const std::map<EnumAttribute, int64_t>& attribute::get_attribute(EnumModule aenum)
	{
		return m_moduledata[aenum].get_fight();
	}

	void attribute::printf()
	{
		root().printf();
	}
}//namespace ngl


namespace ngl
{
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

		for (EnumModule j = EnumModule::E_ModuleNull;j != amodule;j = (EnumModule)(j+1))
		{
			if (j == EnumModule::E_ModuleNull)
			{
				continue;
			}
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
		for (EnumModule i = EnumModule::E_ModuleRoot;i < EnumModule::E_ModuleCount; i = (EnumModule)(i+1))
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
		for (EnumModule i = EnumModule::E_ModuleRoot;i < EnumModule::E_ModuleCount; i = (EnumModule)(i + 1))
		{
			attribute_value lvalue = init_attribute_value(i, i != EnumModule::E_ModuleBuff ? 10 : -20);
			lattr2.init_data(i, lvalue);
		}
		lattr2.init();

		lattr2.printf();
	}
}// namespace ngl
