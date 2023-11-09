#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/typeof/typeof.hpp>
#include <functional>
#include <string>
#include <map>

#include "xmlinfo.h"
#include "type.h"
#include "regular.h"
#include "xmlnode.h"
#include "nlog.h"

namespace ngl
{
	class xmlprotocol
	{
		static std::map<std::string, int32_t> m_protocol;

		static void read(const std::string& axmlname)
		{
			boost_ptree	lroot;
			if (!xml::read(axmlname, lroot))
				return;

			std::function<void(boost_ptree&)> lfun = [](boost_ptree& apt)
				{
					xmlinfo ltemp;
					xmlnode::read_config(apt, ltemp);

					std::string lname;
					int32_t lnumber = 0;
					if (ltemp.name(lname) && ltemp.find("number", lnumber))
					{
						xmlprotocol::m_protocol[lname] = lnumber;
					}
					ltemp.log();
				};
			boost_ptree lchild = xml::get_child(lroot, "con");
			xml::_foreach(lchild, "config", lfun);
		}
	public:
		static void load()
		{
			read("./config/gm_protocol.xml");
			read("./config/net_protocol.xml");
		}

		static int32_t protocol(const std::string& aname)
		{
			auto itor = m_protocol.find(aname);
			if (itor == m_protocol.end())
				return -1;
			return itor->second;
		}

		// ### makeproto工具调用 所以不需要索引
		static int32_t free_protocol()
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
			while(lset.find(lvalue) != lset.end())
			{
				++lvalue;
			}
			lset.insert(lvalue);
			return lvalue;
		}
	};
}