#include "attribute.h"

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
		for (EnumModule i = EnumModule::E_ModuleRoot;
			i < EnumModule::E_ModuleCount; i = (EnumModule)(i + 1))
		{
			attribute_value lvalue = init_attribute_value(i, i != EnumModule::E_ModuleBuff ? 10 : -20);
			lattr2.init_data(i, lvalue);
		}
		lattr2.init();

		lattr2.printf();
	}
}// namespace ngl