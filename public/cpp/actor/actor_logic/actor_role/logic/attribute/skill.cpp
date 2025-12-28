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

#include "skill.h"
#include "xmlinfo.h"
#include "unit.h"

namespace ngl
{
	bool skill_data::load(const char* axml)
	{
		//tinyxml2::XMLDocument ldoc;
		//tinyxml2::XMLElement* lcon = nullptr;
		//if (!xml::readxml(axml, ldoc, lcon))
		//{
		//	return;
		//}
		//// base
		//tinyxml2::XMLElement* lbase = xml::get_child(lcon, "base");
		//if (lbase == nullptr)
		//{
		//	return false;
		//}

		//if (!xml::get(lbase, "name", m_base.m_name))
		//{
		//	log_error()->print("skill::load({}) fail base.name");
		//}

		//if (!xml::get(lbase, "describe", m_base.m_describe))
		//{
		//	log_error()->print("skill::load({}) fail base.describe");
		//}

		//tinyxml2::XMLElement* lcooldown = xml::get_child(lbase, "cooldown");
		//if (lcooldown == nullptr)
		//{
		//	return false;
		//}
		//xml::foreach(lcooldown, "lv", [this](tinyxml2::XMLElement* alv)
		//	{
		//		xmlinfo ltemp;
		//		xmlnode::read_config(alv, ltemp);

		//		int32_t llevel = 0;
		//		if (!ltemp.find("level", llevel))
		//		{
		//			log_error()->print("skill::load({}) fail cooldown.level");
		//		}
		//		base::cooldown::lv llv;
		//		if (!ltemp.find("value", llv.m_value))
		//		{
		//			log_error()->print("skill::load({}) fail cooldown.value");
		//		}
		//		m_base.m_cooldown[llevel] = llv;
		//	});

		//if (!xml::get(lbase, "describe", m_base.m_describe))
		//{
		//	log_error()->print("skill::load({}) fail base.describe");
		//}

		//if (!ltemp.find("cooldown", m_base.m_cooldown))
		//{
		//	log_error()->print("skill::load({}) fail base.cooldown");
		//}
		//std::string lconsumestr;
		//if (!ltemp.find("consume", lconsumestr))
		//{
		//	log_error()->print("skill::load({}) fail base.consume");
		//}
		//std::vector<std::string> lvecs;
		//tools::splite(lconsumestr.c_str(), "&", lvecs);
		//for (const auto& item : lvecs)
		//{
		//	std::string lattribute;
		//	int32_t lattributevalue;
		//	tools::splite(item.c_str(), ":", lattribute, lattributevalue);
		//	EnumAttribute ltype = enum_attr_name::type(lattribute.c_str());
		//	if (ltype == enum_attr_name::null_type())
		//	{
		//		continue;
		//	}
		//	if (!m_base.m_consume.contains(ltype))
		//	{
		//		tools::no_core_dump();
		//		return;
		//	}

		//}
		//
		//tinyxml2::XMLElement* lfight = xml::get_child(lcon, "fight");
		//if (lfight == nullptr)
		//{
		//	return false;
		//}
		//xml::foreach(lcon, "step", [this](tinyxml2::XMLElement* aatep)
		//	{
		//		setp lsetp;
		//		xml::foreach(aatep, "lv", [&lsetp](tinyxml2::XMLElement* alv)
		//			{
		//				xmlinfo ltemp;
		//				xmlnode::read_config(alv, ltemp);

		//				setp::lv llv;
		//				if (!ltemp.find("start", llv.m_start))
		//				{
		//					log_error()->print("skill::load({}) fail fight.setp.start");
		//				}
		//				if (!ltemp.find("finish", llv.m_finish))
		//				{
		//					log_error()->print("skill::load({}) fail fight.setp.finish");
		//				}
		//				std::string lbuffstr;
		//				if (!ltemp.find("buff", lbuffstr))
		//				{
		//					log_error()->print("skill::load({}) fail fight.setp.buff");
		//				}
		//				tools::splite(lbuffstr.c_str(), "&", llv.m_buffids);
		//				lsetp.m_lvs.push_back(llv);
		//			}
		//		);
		//		m_setp.push_back(lsetp);
		//	}
		//);
		return true;
	}

	bool skill::release_check()
	{
		dynamic_attribute& ldynamic = m_unit->dynamic();
		// 检查玩家身上的状态
		if (!ldynamic.is_releaseskill())
		{
			return false;
		}

		// 获取冷却时间
		m_data.m_base.m_cooldown;

		// 冷却缩减 万分比
		int32_t lcoolabsolute = ldynamic.get_attribute(E_CooldownAbsolute);
		int32_t lcoolprop = ldynamic.get_attribute(E_CooldownProp);
		/*int32_t lvalue = m_data.m_base.m_cooldown - ((m_data.m_base.m_cooldown / 10000) * lcoolprop) - lcoolabsolute;
		if (lvalue < 0)
		{
			lvalue = 0;
		}

		int32_t lnow = (int32_t)localtime::gettime();
		if (m_cooldownutc + lvalue > lnow)
		{
			return false;
		}*/

		return true;
	}

}//namespace ngl