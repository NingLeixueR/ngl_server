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
#include "xmlprotocol.h"
#include "xml.h"

namespace ngl
{
	std::map<std::string, int32_t> xmlprotocol::m_protocol;

	void xmlprotocol::read(const char* axml)
	{
		xarg_protocols ltemps;
		if (!ltemps.xml_pop(axml))
		{
			tools::no_core_dump();
			return;
		}

		for (auto& item : ltemps.m_config)
		{
			std::string& lname = item.m_name;
			int32_t lnumber = item.m_number;
			std::map<std::string, int32_t>& lmap = xmlprotocol::m_protocol;
			lmap[tools::type_name_handle(lname)] = lnumber;
			lmap[std::format("np_actormodule_forward<{}>", lname)] = lnumber;
			lmap[std::format("np_actor_forward<{},forward_g2c<forward>>", lname)] = lnumber;
			lmap[std::format("np_actor_forward<{0},forward_g2c<{0}>>", lname)] = lnumber;
			lmap[std::format("np_actor_forward<{},forward_c2g<forward>>", lname)] = lnumber;
			lmap[std::format("np_actor_forward<{0},forward_c2g<{0}>>", lname)] = lnumber;
		}
	}

	void xmlprotocol::load()
	{
		read("./config/net_protocol.xml");
		read("./config/example_protocol.xml");
	}

	int32_t xmlprotocol::protocol(const std::string& aname)
	{
		std::map<std::string, int32_t>& lmap = m_protocol;
		auto itor = lmap.find(aname);
		if (itor == lmap.end())
		{
			return -1;
		}
		return itor->second;
	}

	int32_t xmlprotocol::free_protocol()
	{
		static std::set<int32_t> lset;
		if (lset.empty())
		{
			for (auto& [key, value] : m_protocol)
			{
				lset.insert(value);
			}
		}
		int lvalue = 1;
		while (lset.find(lvalue) != lset.end())
		{
			++lvalue;
		}
		lset.insert(lvalue);
		return lvalue;
	}
}// namespace ngl