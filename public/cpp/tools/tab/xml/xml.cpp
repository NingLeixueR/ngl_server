#include "ttab_servers.h"
#include "nprotocol.h"
#include "tinyxml2.h"
#include "csvtable.h"
#include "xmlinfo.h"
#include "tools.h"
#include "type.h"
#include "nlog.h"
#include "xml.h"

namespace ngl
{
	bool xml::readxml(const char* aname, tinyxml2::XMLDocument& axml, tinyxml2::XMLElement*& acon)
	{
		if (axml.LoadFile(aname) != tinyxml2::XML_SUCCESS)
		{
			std::cout << "Failed to load XML file[" << aname << "]." << std::endl;
			return false;
		}
		acon = axml.FirstChildElement("con");
		return acon != nullptr;
	}

	tinyxml2::XMLElement* xml::get_child(tinyxml2::XMLElement* aele, const char* astr)
	{
		std::vector<std::string> lvec;
		if (tools::splite(astr, ".", lvec) == false)
		{
			return nullptr;
		}
		tinyxml2::XMLElement* valElement = aele;
		for (const auto& item : lvec)
		{
			valElement = valElement->FirstChildElement(item.c_str());
			if (valElement == nullptr)
			{
				return nullptr;
			}
		}
		return valElement;
	}

	void xml::foreach(tinyxml2::XMLElement* aele, const char* akey, const std::function<void(tinyxml2::XMLElement*)>& afun)
	{
		for (tinyxml2::XMLNode* child = aele->FirstChildElement(); child; child = child->NextSiblingElement())
		{
			tinyxml2::XMLElement* lxele = child->ToElement();
			if (lxele != nullptr && std::string(lxele->Name()) == akey)
			{
				afun(lxele);
			}
		}
	}

	void xml::foreach(tinyxml2::XMLElement* aele, const std::function<void(tinyxml2::XMLElement*)>& afun)
	{
		for (tinyxml2::XMLNode* child = aele->FirstChildElement(); child; child = child->NextSiblingElement())
		{
			tinyxml2::XMLElement* lxele = child->ToElement();
			if (lxele != nullptr)
			{
				afun(lxele);
			}
		}
	}

	void xml::foreach_xmlattr(tinyxml2::XMLElement* aele, const std::function<void(const char*, const char*)>& afun)
	{
		for (const tinyxml2::XMLAttribute* attribute = aele->FirstAttribute(); attribute; attribute = attribute->Next())
		{
			const char* lkey = attribute->Name();
			const char* lval = attribute->Value();
			afun(lkey, lval);
		}
	}

	tinyxml2::XMLDocument	xmlnode::m_doc;
	tinyxml2::XMLElement*	xmlnode::m_con;
	dbserver_info			xmlnode::m_db;
	dbserver_info			xmlnode::m_crossdb;
	xmlinfo					xmlnode::m_publicinfo;
	std::string				xmlnode::m_nodename;
	NODE_TYPE				xmlnode::m_nodetype;
	i32_id					xmlnode::m_nodeid;
	int16_t					xmlnode::m_tid;
	int16_t					xmlnode::m_tcount;
	mail_info				xmlnode::m_mail;
	telnet_info				xmlnode::m_telnet;

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

	i16_area xmlnode::area()
	{
		auto ltab = ttab_servers::instance().tab(m_tid);
		if (ltab == nullptr)
		{
			tools::no_core_dump();
			return -1;
		}
		return ltab->m_area;
	}

	void xmlnode::set_server(const char* aservertypename)
	{
		NODE_TYPE lnodetype = em<NODE_TYPE>::get_enum(aservertypename);
		if (lnodetype == em<NODE_TYPE>::enum_null())
		{
			tools::no_core_dump();
		}
		m_nodename = aservertypename;
		m_nodetype = lnodetype;
	}

	void xmlnode::set_nodeid(int atid, int atcount)
	{
		m_nodeid = nnodeid::nodeid(atid, atcount);
		m_tid = nnodeid::tid(m_nodeid);
		m_tcount = nnodeid::tcount(m_nodeid);
	}

	void xmlnode::load(const std::string& axmlpath, const std::string& aname)
	{
		std::string lxmlname = std::format("{}/config/config_{}.xml", axmlpath, aname);
		if (tools::file_exists(lxmlname) == false)
		{
			lxmlname = std::format("{}/config/config.xml", axmlpath);
		}

		log_error()->print("begin xmlnode read [{}]", lxmlname);

		if (!xml::readxml(lxmlname.c_str(), m_doc, m_con))
		{
			return;
		}

		loadcrossdb();
		loadpublic();
		loadtelnet();
		loadmail();
		loaddb();

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

	bool xmlnode::read_db_arg(const char* aname, dbarg& adbarg)
	{
		std::string lchild = std::format("{}.db_connect", aname);
		tinyxml2::XMLElement* lnode = xml::get_child(m_con, lchild.c_str());
		if (xml::get_xmlattr(lnode, "port", adbarg.m_port) == false)
		{
			return false;
		}
		if (xml::get_xmlattr(lnode, "ip", adbarg.m_ip) == false)
		{
			return false;
		}
		if (xml::get_xmlattr(lnode, "dbname", adbarg.m_dbname) == false)
		{
			return false;
		}
		if (xml::get_xmlattr(lnode, "passworld", adbarg.m_passworld) == false)
		{
			return false;
		}
		if (xml::get_xmlattr(lnode, "account", adbarg.m_account) == false)
		{
			return false;
		}
		return true;
	}

	bool xmlnode::read_mail_arg(const char* aname, mailarg& amailarg)
	{
		std::string lchild = std::format("{}.email", aname);
		tinyxml2::XMLElement* lnode = xml::get_child(m_con, lchild.c_str());
		if (xml::get_xmlattr(lnode, "smtp", amailarg.m_smtp) == false)
		{
			return false;
		}
		if (xml::get_xmlattr(lnode, "email", amailarg.m_email) == false)
		{
			return false;
		}
		if (xml::get_xmlattr(lnode, "passworld", amailarg.m_password) == false)
		{
			return false;
		}
		if (xml::get_xmlattr(lnode, "title", amailarg.m_title) == false)
		{
			return false;
		}
		if (xml::get_xmlattr(lnode, "content", amailarg.m_content) == false)
		{
			return false;
		}

		std::string lrecvmail;
		if (xml::get_xmlattr(lnode, "recvmail", lrecvmail) == false)
		{
			return false;
		}

		if (tools::splite_special(lrecvmail.c_str(), "\\[", "]", amailarg.m_recvs) == false)
		{
			return false;
		}

		return true;
	}

	bool xmlnode::read_telnet_arg(const char* aname, telnetarg& amailarg)
	{
		std::string lchild = std::format("{}.telnet", aname);
		tinyxml2::XMLElement* lnode = xml::get_child(m_con, lchild.c_str());
		if (xml::get_xmlattr(lnode, "account", amailarg.m_account) == false)
		{
			return false;
		}
		if (xml::get_xmlattr(lnode, "passworld", amailarg.m_passworld) == false)
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

	void xmlnode::loadmail()
	{
		read_mail_arg("email", m_mail.m_mailarg);
	}
	
	void xmlnode::loadtelnet()
	{
		read_telnet_arg("telnet", m_telnet.m_telnetarg);
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