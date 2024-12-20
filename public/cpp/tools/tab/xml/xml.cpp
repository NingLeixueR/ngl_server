#include "xml.h"
#include "nprotocol.h"
#include "tinyxml2.h"
#include "csvtable.h"
#include "xmlinfo.h"
#include "tools.h"
#include "type.h"
#include "nlog.h"

namespace ngl
{
	tinyxml2::XMLDocument	xmlnode::m_doc;
	tinyxml2::XMLElement*	xmlnode::m_con;
	dbserver_info			xmlnode::m_db;
	dbserver_info			xmlnode::m_crossdb;
	xmlinfo					xmlnode::m_publicinfo;
	std::string				xmlnode::m_nodename;
	NODE_TYPE				xmlnode::m_nodetype;
	i32_id					xmlnode::m_nodeid;

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
		em<NODE_TYPE>::set(CROSSDB, "crossdb");
	}

	NODE_TYPE xmlnode::node_type()
	{
		return m_nodetype;
	}

	void xmlnode::set_server(const char* aservertypename, int anodeid)
	{
		NODE_TYPE lnodetype = em<NODE_TYPE>::get_enum(aservertypename);
		Assert(lnodetype != em<NODE_TYPE>::enum_null());
		m_nodename = aservertypename;
		m_nodetype = lnodetype;
		m_nodeid = anodeid;
	}

	void xmlnode::load(const std::string& axmlpath, const std::string& aname)
	{
		std::string lxmlname = std::format("{}/config/config_{}.xml", axmlpath, aname);
		if (filetools::exist(lxmlname) == false)
		{
			lxmlname = std::format("{}/config/config.xml", axmlpath);
		}

		log_error()->print("begin xmlnode read [{}]", lxmlname);

		if (!xml::readxml(lxmlname.c_str(), m_doc, m_con))
		{
			return;
		}

		loadpublic();
		loaddb();
		loadcrossdb();

		log_error()->print("finish xmlnode read [{}]", lxmlname);
	}

	void xmlnode::read_config(tinyxml2::XMLElement* apt, xmlinfo& anfo)
	{
		xml::foreach_xmlattr(apt, [&anfo](const char* akey, const char* aval)->void
			{
				anfo.data()[akey] = aval;
			});
	}
	void xmlnode::read_config(const char* akey, std::map<i32_serverid, xmlinfo>& anfo)
	{
		std::function<void(tinyxml2::XMLElement*)> lfun = [&anfo](tinyxml2::XMLElement* apt)
			{
				xmlinfo ltemp;
				xmlnode::read_config(apt, ltemp);

				i32_serverid lid = 0;
				if (ltemp.find("id", lid))
				{
					anfo[lid] = ltemp;
				}

				ltemp.plog();
			};
		tinyxml2::XMLElement* lchild = xml::get_child(m_con, akey);
		xml::foreach(lchild, "config", lfun);
	}
	
	bool xmlnode::read_public_config(xmlinfo& anfo)
	{
		std::function<void(const char*, const char*)> lfun = [&anfo](const char* akey, const char* aval)
			{
				anfo.data()[akey] = aval;
			};
		tinyxml2::XMLElement* lchild = xml::get_child(m_con, "public.config");
		if (lchild == nullptr)
		{
			return false;
		}
		xml::foreach_xmlattr(lchild, lfun);
		return true;
	}

	bool xmlnode::read_db_arg(const char* aname, dbarg& m_dbarg)
	{
		std::string lchild = std::format("{}.db_connect", aname);
		tinyxml2::XMLElement* lnode = xml::get_child(m_con, lchild.c_str());
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

	void xmlnode::loaddb()
	{
		read_db_arg("db", m_db.m_dbarg);
	}

	void xmlnode::loadcrossdb()
	{
		read_db_arg("crossdb", m_crossdb.m_dbarg);
	}

	void xmlnode::loadpublic()
	{
		read_public_config(m_publicinfo);
	}

	xmlinfo* xmlnode::get_publicconfig()
	{
		return &m_publicinfo;
	}
}// namespace ngl