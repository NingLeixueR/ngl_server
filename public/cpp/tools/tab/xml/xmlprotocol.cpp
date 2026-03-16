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
// File overview: Implements logic for xml.


#include "tools/tab/xml/xmlprotocol.h"
#include "tools/tab/xml/xml.h"

namespace ngl
{
	std::map<std::string, int32_t> xmlprotocol::m_protocol;

	void xmlprotocol::read(const char* axml)
	{
		if (axml == nullptr || *axml == '\0' || !tools::file_exists(axml))
		{
			return;
		}
		xarg_protocols ltemps;
		if (!ltemps.xml_pop(axml))
		{
			tools::no_core_dump();
			return;
		}

		for (auto& item : ltemps.m_config)
		{
			std::string& lname = item.m_name;
			tools::type_name_handle(lname);
			int32_t lnumber = item.m_number;
			std::map<std::string, int32_t>& lmap = xmlprotocol::m_protocol;
			lmap[lname] = lnumber;
			lmap[std::format("np_actormodule_forward<{}>", lname)] = lnumber;
			lmap[std::format("np_actor_forward<{},forward_g2c<forward>>", lname)] = lnumber;
			lmap[std::format("np_actor_forward<{0},forward_g2c<{0}>>", lname)] = lnumber;
			lmap[std::format("np_actor_forward<{},forward_c2g<forward>>", lname)] = lnumber;
			lmap[std::format("np_actor_forward<{0},forward_c2g<{0}>>", lname)] = lnumber;
		}
	}

	void xmlprotocol::load()
	{
		m_protocol.clear();
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
		std::set<int32_t> lset;
		for (auto& [_key, value] : m_protocol)
		{
			lset.insert(value);
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