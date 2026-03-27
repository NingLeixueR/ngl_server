/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project: ngl_server
* License: MIT
*/
// File overview: Implements logic for xml.


#include "actor/protocol/nprotocol.h"
#include "actor/tab/ttab_servers.h"
#include "tools/tab/xml/xml.h"

#include <array>
#include <cstring>
#include <filesystem>
#include <limits>
#include <string>

namespace ngl
{
	void xmlnode::init()
	{
		tools::em<NODE_TYPE>::set(DB, "db");
		tools::em<NODE_TYPE>::set(ACTORSERVER, "actor");
		tools::em<NODE_TYPE>::set(GAME, "game");
		tools::em<NODE_TYPE>::set(GATEWAY, "gateway");
		tools::em<NODE_TYPE>::set(LOGIN, "login");
		tools::em<NODE_TYPE>::set(ROBOT, "robot");
		tools::em<NODE_TYPE>::set(WORLD, "world");
		tools::em<NODE_TYPE>::set(LOG, "log");
		tools::em<NODE_TYPE>::set(RELOADCSV, "reloadcsv");
		tools::em<NODE_TYPE>::set(CROSS, "cross");
		tools::em<NODE_TYPE>::set(PUSHSERVERCONFIG, "pushserverconfig");
		tools::em<NODE_TYPE>::set(CROSSDB, "crossdb");
	}

	i16_area xmlnode::area()
	{
		auto ltab = ttab_servers::instance().tab(m_tid);
		if (ltab == nullptr)
		{
			log_error()->print("xmlnode area missing tab tid [{}]", m_tid);
			return -1;
		}
		return ltab->m_area;
	}

	bool xmlnode::set_server(const char* aservertypename)
	{
		if (aservertypename == nullptr)
		{
			log_error()->print("xmlnode invalid server type [null]");
			return false;
		}
		NODE_TYPE lnodetype = tools::em<NODE_TYPE>::get_enum(aservertypename);
		if (lnodetype == tools::em<NODE_TYPE>::enum_null())
		{
			log_error()->print("xmlnode invalid server type [{}]", aservertypename);
			return false;
		}
		m_nodename = aservertypename;
		m_nodetype = lnodetype;
		return true;
	}

	void xmlnode::set_nodeid(int16_t atid, int16_t atcount)
	{
		m_tid = atid;
		m_tcount = atcount;
		m_nodeid = nnodeid::nodeid(atid, atcount);
	}

	void xmlnode::set_servername(const std::string& asername)
	{
		m_servername = asername;
	}

	std::string& xmlnode::servername()
	{
		return m_servername;
	}

	bool xmlnode::load(const std::string& axmlpath, const std::string& aname)
	{
		m_configname = aname;
		m_configfile = std::format("{}/config/{}.xml", axmlpath, aname);

		log_info()->print("begin xmlnode read [{}]", m_configfile);
		const bool ok = xml_pop(m_configfile.c_str());
		log_info()->print("finish xmlnode read [{}] ok={}", m_configfile, ok);
		return ok;
	}

	const std::string& xmlnode::nodename() const
	{
		return m_nodename;
	}

	const std::string& xmlnode::config_file() const
	{
		return m_configfile;
	}

	const std::string& xmlnode::config_name() const
	{
		return m_configname;
	}

	xarg_info* xmlnode::info()
	{
		return &m_public;
	}

	int16_t xmlnode::tcount()
	{
		return m_tcount;
	}

	int16_t	xmlnode::tid()
	{
		return m_tid;
	}

	i32_serverid xmlnode::nodeid()
	{
		return m_nodeid;
	}

	NODE_TYPE xmlnode::nodetype()
	{
		return m_nodetype;
	}

	xarg_db& xmlnode::db()
	{
		return m_db;
	}

	xarg_db& xmlnode::crossdb()
	{
		return m_dbcross;
	}

	xarg_db::edb xmlnode::dbedb()
	{
		if (NODE_TYPE::CROSSDB == nconfig.nodetype())
		{
			return crossdb().m_db;
		}
		if (NODE_TYPE::DB == nconfig.nodetype())
		{
			return db().m_db;
		}
		return static_cast<xarg_db::edb>(-1);
	}

	xarg_mail& xmlnode::mail()
	{
		return m_mail;
	}

	xarg_telnet& xmlnode::telnet()
	{
		return m_telnet;
	}

	xarg_wss& xmlnode::wss()
	{
		return m_wss;
	}

	xarg_redis& xmlnode::redis()
	{
		return m_redis;
	}

}// namespace ngl
