#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp>

#include "xmlnode.h"
#include "enum2name.h"
#include "splite.h"
#include "tools.h"
#include "nlog.h"
#include "csvtable.h"

namespace ngl
{
	bool xml::read(std::string aname, boost_ptree& apt)
	{
		read_xml(aname, apt);   //读入一个xml文件 
		return true;
	}

	bool xml::write(std::string aname, boost_ptree& apt)
	{
		write_xml(aname, apt); 
		return true;
	}

	std::string xml::data(boost_ptree& apt)
	{
		return apt.data();
	}

	boost_ptree xml::get_child(boost_ptree& apt, const std::string& anode)
	{
		return apt.get_child(anode);
	}

	void xml::add_child(boost_ptree& apt, const std::string& anodename, boost_ptree& anode)
	{
		apt.add_child(anodename, anode);
	}

	boost_ptree::assoc_iterator xml::findchild(boost_ptree& apt, const std::string& anode)
	{
		boost_ptree::assoc_iterator itor = apt.find(anode);
		assert(itor != apt.not_found());
		return itor;
	}

	void xml::_foreach(boost_ptree& apt, const std::string& anode, const std::function<void(boost_ptree& apt)> afun)
	{
		for (BOOST_AUTO(pos, apt.begin()); pos != apt.end(); ++pos)
		{
			if (pos->first == anode)
				afun(pos->second);
		}
	}

	void xml::_foreach(boost_ptree& apt, const std::function<void(boost_ptree& apt, const std::string&)> afun)
	{
		for (BOOST_AUTO(pos, apt.begin()); pos != apt.end(); ++pos)
		{
			afun(pos->second, pos->first);
		}
	}

	void xml::_foreach_xmlattr(boost_ptree& apt, const std::function<void(boost_ptree& apt, const std::string&)> afun)
	{
		for (BOOST_AUTO(pos, apt.begin()); pos != apt.end(); ++pos)
		{
			//std::cout << pos->first << std::endl;
			if (memcmp(pos->first.c_str(), "<xmlattr>", sizeof("<xmlattr>")) == 0)
			{
				_foreach(pos->second, afun);
			}
		}
	}


	boost_ptree				xmlnode::m_root;
	dbserver_info			xmlnode::m_db;
	xmlinfo					xmlnode::m_publicinfo;
	std::string				xmlnode::m_nodename;
	NODE_TYPE				xmlnode::m_nodetype;
	i32_id					xmlnode::m_nodeid;

	void xmlnode::init()
	{
		em<NODE_TYPE>::set(DB,"db");
		em<NODE_TYPE>::set(ACTORSERVER, "actor");
		em<NODE_TYPE>::set(GAME,"game");
		em<NODE_TYPE>::set(GATEWAY,"gateway");
		em<NODE_TYPE>::set(LOGIN,"login");
		em<NODE_TYPE>::set(ROBOT, "robot");
		em<NODE_TYPE>::set(WORLD, "world");
		em<NODE_TYPE>::set(LOG, "log");
		em<NODE_TYPE>::set(RELOADCSV, "reloadcsv");
		em<NODE_TYPE>::set(CROSS, "cross");
		em<NODE_TYPE>::set(PUSHSERVERCONFIG, "pushserverconfig");
	}

	NODE_TYPE xmlnode::node_type()
	{
		return m_nodetype;
	}

	void xmlnode::set_server(const char* aservertypename, int anodeid)
	{
		std::pair<NODE_TYPE, bool> lpair = em<NODE_TYPE>::get_enum(aservertypename);
		//assert(lpair.second == true);
		m_nodename = aservertypename;
		m_nodetype = lpair.first;
		m_nodeid = anodeid;
	}

	void xmlnode::load(const std::string& axml)
	{
		const char* dir = "./config/";
		std::string lxmlname(dir);
		lxmlname += axml;
		lxmlname += ".xml";
		LogLocalError("begin xmlnode read [%]", lxmlname)

		if (!xml::read(lxmlname, m_root))
			return;


		loadpublic();
		loaddb();

		LogLocalError("finish xmlnode read [%]", lxmlname)
	}

	void xmlnode::read_config(boost_ptree& apt, xmlinfo& anfo)
	{
		xml::_foreach_xmlattr(apt, [&anfo](boost_ptree& apt, const std::string& akey)->void
			{
				anfo.m_data.insert(std::make_pair(akey, apt.data()));
			});
	}

	void xmlnode::read_public_config(xmlinfo& anfo)
	{
		std::function<void(boost_ptree&)> lfun = [&anfo](boost_ptree& apt)
		{
			xmlinfo ltemp;
			xmlnode::read_config(apt, ltemp);
			anfo = ltemp;
			ltemp.log();
		};
		boost_ptree lchild = xml::get_child(m_root, "con.public");
		xml::_foreach(lchild, "config", lfun);
	}

	void xmlnode::read_config(const char* akey, std::map<i32_serverid, xmlinfo>& anfo)
	{
		std::function<void(boost_ptree&)> lfun = [&anfo](boost_ptree& apt)
		{
			xmlinfo ltemp;
			xmlnode::read_config(apt, ltemp);

			i32_serverid lid = 0;

			if (ltemp.id(lid))
				anfo[lid] = ltemp;

			ltemp.log();
		};
		boost_ptree lchild = xml::get_child(m_root, akey);
		xml::_foreach(lchild, "config", lfun);
	}
	void xmlnode::read_db_arg(dbarg& m_dbarg)
	{
		boost_ptree llocal = xml::get_child(m_root, "con.db.db_connect");
		m_dbarg.m_port = xml::get_xmlattr<uint32_t>(llocal, "port");
		m_dbarg.m_ip = xml::get_xmlattr<std::string>(llocal, "ip");
		m_dbarg.m_dbname = xml::get_xmlattr<std::string>(llocal, "dbname");
		m_dbarg.m_passworld = xml::get_xmlattr<std::string>(llocal, "passworld");
		m_dbarg.m_account = xml::get_xmlattr<std::string>(llocal, "account");
	}

	void xmlnode::read_net_tab(const char* akey, const char* akey2, std::set<NODE_TYPE>& anet)
	{
		std::function<void(boost_ptree&)> lfun = [&anet](boost_ptree& apt)
		{
			std::string lstr = xml::get_xmlattr<std::string>(apt, "type");
			std::vector<std::string> lvec;
			splite::division(lstr.c_str(), "|", lvec);
			std::function<void(std::string&)> lfun = [&anet](std::string& avalue)
			{
				std::pair<NODE_TYPE, bool> lpair = em<NODE_TYPE>::get_enum(avalue.c_str());
				if (lpair.second)
					anet.insert(lpair.first);
			};
			for (auto& item : lvec)
				lfun(item);
		};
		boost_ptree lchild = xml::get_child(m_root, akey);
		xml::_foreach(lchild, akey2, lfun);
	}

	void xmlnode::loadpublic()
	{
		read_public_config(m_publicinfo);
	}

	void xmlnode::loaddb()
	{
		read_db_arg(m_db.m_dbarg);
	}
}