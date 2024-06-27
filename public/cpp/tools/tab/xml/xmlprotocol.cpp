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
			return;

		std::function<void(tinyxml2::XMLElement*)> lfun = [](tinyxml2::XMLElement* apt)
			{
				xmlinfo ltemp;
				xmlnode::read_config(apt, ltemp);

				std::string lname;
				int32_t lnumber = 0;
				if (ltemp.find("name", lname) && ltemp.find("number", lnumber))
				{
					xmlprotocol::m_protocol[lname] = lnumber;
				}
				ltemp.plog();
			};
		xml::foreach(lcon, "config", lfun);
	}

	void xmlprotocol::load()
	{
		read("./config/net_protocol.xml");
		read("./config/netserver_protocol.xml");
	}

	int32_t xmlprotocol::protocol(const std::string& aname)
	{
		auto itor = m_protocol.find(aname);
		if (itor == m_protocol.end())
			return -1;
		return itor->second;
	}

	// ### makeproto工具调用 所以不需要索引
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