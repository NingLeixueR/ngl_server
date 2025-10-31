/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "xmlprotocol.h"
#include "xml.h"

namespace ngl
{
	std::map<std::string, int32_t> xmlprotocol::m_protocol;

	void xmlprotocol::read(const std::string& axmlname)
	{
		tinyxml2::XMLDocument ldoc;
		tinyxml2::XMLElement* lcon;
		if (!xml::readxml(axmlname.c_str(), ldoc, lcon))
		{
			return;
		}

		std::function<void(tinyxml2::XMLElement*)> lfun = [](tinyxml2::XMLElement* apt)
			{
				xmlinfo ltemp;
				xmlnode::read_config(apt, ltemp);

				std::string lname;
				int32_t lnumber = 0;
				std::map<std::string, int32_t>& lmap = xmlprotocol::m_protocol;
				if (ltemp.find("name", lname) && ltemp.find("number", lnumber))
				{
					lmap[tools::type_name_handle(lname)] = lnumber;
					lmap[std::format("np_actormodule_forward<{}>", lname)] = lnumber;
					lmap[std::format("np_actor_forward<{},forward_g2c<forward>>", lname)] = lnumber;
					lmap[std::format("np_actor_forward<{0},forward_g2c<{0}>>", lname)] = lnumber;
					lmap[std::format("np_actor_forward<{},forward_c2g<forward>>", lname)] = lnumber;
					lmap[std::format("np_actor_forward<{0},forward_c2g<{0}>>", lname)] = lnumber;
				}
				ltemp.plog();
			};
		xml::foreach(lcon, "config", lfun);
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
			for (const auto& [key, value] : m_protocol)
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