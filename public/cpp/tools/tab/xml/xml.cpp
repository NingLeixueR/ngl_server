#include "xml.h"
#include "nprotocol.h"
#include "tinyxml2.h"
#include "csvtable.h"
#include "xmlinfo.h"
#include "splite.h"
#include "tools.h"
#include "type.h"
#include "nlog.h"

namespace ngl
{
	tinyxml2::XMLDocument xmlnode::m_doc;
	tinyxml2::XMLElement* xmlnode::m_con;
	dbserver_info	xmlnode::m_db;
	xmlinfo			xmlnode::m_publicinfo;
	std::string		xmlnode::m_nodename;
	NODE_TYPE		xmlnode::m_nodetype;
	i32_id			xmlnode::m_nodeid;

	void xmlnode::init()
	{
		em<NODE_TYPE>::set(DB, "db");
		em<NODE_TYPE>::set(ACTORSERVER, "actor");
		em<NODE_TYPE>::set(GAME, "game");
		em<NODE_TYPE>::set(GATEWAY, "gateway");
		em<NODE_TYPE>::set(LOGIN, "login");
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
		log_error()->print("begin xmlnode read [{}]", lxmlname);

		if (!xml::readxml(lxmlname.c_str(), m_doc, m_con))
			return;

		loadpublic();
		loaddb();

		log_error()->print("finish xmlnode read [{}]", lxmlname);
	}

	void xmlnode::read_config(tinyxml2::XMLElement* apt, xmlinfo& anfo)
	{
		xml::foreach_xmlattr(apt, [&anfo](const char* akey, const char* aval)->void
			{
				anfo.m_data.insert(std::make_pair(akey, aval));
			});
	}
	void xmlnode::read_config(const char* akey, std::map<i32_serverid, xmlinfo>& anfo)
	{
		std::function<void(tinyxml2::XMLElement*)> lfun = [&anfo](tinyxml2::XMLElement* apt)
			{
				xmlinfo ltemp;
				xmlnode::read_config(apt, ltemp);

				i32_serverid lid = 0;

				if (ltemp.id(lid))
					anfo[lid] = ltemp;

				ltemp.plog();
			};
		tinyxml2::XMLElement* lchild = xml::get_child(m_con, akey);
		xml::foreach(lchild, "config", lfun);
	}
	
	bool xmlnode::read_public_config(xmlinfo& anfo)
	{
		std::function<void(const char*, const char*)> lfun = [&anfo](const char* akey, const char* aval)
			{
				anfo.m_data.insert(std::make_pair(akey, aval));
			};
		tinyxml2::XMLElement* lchild = xml::get_child(m_con, "public.config");
		if (lchild == nullptr)
			return false;
		xml::foreach_xmlattr(lchild, lfun);
	}

	bool xmlnode::read_db_arg(dbarg& m_dbarg)
	{
		tinyxml2::XMLElement* lnode = xml::get_child(m_con, "db.db_connect");
		if (xml::get_xmlattr(lnode, "port", m_dbarg.m_port) == false)
		{
			return false;
		}
		if (xml::get_xmlattr(lnode, "ip", m_dbarg.m_ip) == false)
		{
			return false;
		}
		if (xml::get_xmlattr(lnode, "dbname", m_dbarg.m_dbname) == false)
		{
			return false;
		}
		if (xml::get_xmlattr(lnode, "passworld", m_dbarg.m_passworld) == false)
		{
			return false;
		}
		if (xml::get_xmlattr(lnode, "account", m_dbarg.m_account) == false)
		{
			return false;
		}
		return true;
	}

	//static void xmlnode::read_net_tab(const char* akey, const char* akey2, std::set<NODE_TYPE>& anet);
	void xmlnode::loaddb()
	{
		read_db_arg(m_db.m_dbarg);
	}

	void xmlnode::loadpublic()
	{
		read_public_config(m_publicinfo);
	}

	xmlinfo* xmlnode::get_publicconfig()
	{
		return &m_publicinfo;
	}
}