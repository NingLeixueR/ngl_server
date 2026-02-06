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
#include "ttab_servers.h"
#include "nprotocol.h"
#include "tinyxml2.h"
#include "csvtable.h"
#include "xmlinfo.h"
#include "tools.h"
#include "type.h"
#include "nlog.h"
#include "xml.h"

#include "skill.h"

namespace ngl
{
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

	void xmlnode::set_servername(const std::string& asername)
	{
		m_servername = asername;
	}

	std::string& xmlnode::servername()
	{
		return m_servername;
	}

	void xmlnode::load(const std::string& axmlpath, const std::string& aname)
	{
		std::string lxmlname = std::format("{}/config/config_{}.xml", axmlpath, aname);
		if (tools::file_exists(lxmlname) == false)
		{
			lxmlname = std::format("{}/config/config.xml", axmlpath);
		}

		log_error()->print("begin xmlnode read [{}]", lxmlname);

		xml_pop(lxmlname.c_str());
		log_error()->print("finish xmlnode read [{}]", lxmlname);
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

	xarg_mail& xmlnode::mail()
	{
		return m_mail;
	}

	xarg_telnet& xmlnode::telnet()
	{
		return m_telnet;
	}

	xarg_redis& xmlnode::redis()
	{
		return m_redis;
	}
}// namespace ngl