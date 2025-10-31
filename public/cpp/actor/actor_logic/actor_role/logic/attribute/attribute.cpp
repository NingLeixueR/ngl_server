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