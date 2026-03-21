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
	namespace xml_detail
	{
		bool parse_node_part(int avalue, int16_t& aout)
		{
			if (avalue < static_cast<int>(std::numeric_limits<int16_t>::min()) ||
				avalue > static_cast<int>(std::numeric_limits<int16_t>::max()))
			{
				tools::no_core_dump();
				return false;
			}
			aout = static_cast<int16_t>(avalue);
			return true;
		}

		template <typename T>
		bool pop_required(tinyxml2::XMLElement* aroot, const char* akey, T& aout)
		{
			return xml_serialize<false, T>::pop(aroot, akey, aout);
		}

		template <typename T>
		bool pop_optional(tinyxml2::XMLElement* aroot, const char* akey, T& aout)
		{
			T parsed{};
			if (!xml_serialize<false, T>::pop(aroot, akey, parsed))
			{
				return true;
			}
			aout = std::move(parsed);
			return true;
		}

		template <typename T>
		bool pop_optional_any(tinyxml2::XMLElement* aroot, std::initializer_list<const char*> akeys, T& aout)
		{
			// Accept old and new key names while config files are migrating.
			for (const char* key : akeys)
			{
				T parsed{};
				if (xml_serialize<false, T>::pop(aroot, key, parsed))
				{
					aout = std::move(parsed);
					return true;
				}
			}
			return true;
		}

		bool has_data(const xarg_db& adb)
		{
			return adb.m_port != 0 ||
				!adb.m_ip.empty() ||
				!adb.m_account.empty() ||
				!adb.m_passworld.empty() ||
				!adb.m_dbname.empty();
		}

		bool has_data(const xarg_mail& amail)
		{
			return !amail.m_smtp.empty() ||
				!amail.m_email.empty() ||
				!amail.m_password.empty() ||
				!amail.m_name.empty() ||
				!amail.m_recvs.empty();
		}

		bool has_data(const xarg_telnet& atelnet)
		{
			return !atelnet.m_account.empty() || !atelnet.m_password.empty();
		}

		bool has_data(const xarg_wss& awss)
		{
			return !awss.m_certificate_chain.empty() ||
				!awss.m_private_key.empty() ||
				!awss.m_ca_certificates.empty() ||
				awss.m_verify_peer != 1;
		}

		bool has_data(const xarg_redis& aredis)
		{
			return !aredis.m_ip.empty() || aredis.m_port != 0 || !aredis.m_passworld.empty();
		}

		template <typename T>
		bool push_if_present(tinyxml2::XMLElement* aroot, const char* akey, const T& avalue)
		{
			return has_data(avalue) ? xml_serialize<false, T>::push(aroot, akey, avalue) : true;
		}
	} // namespace xml_detail

	bool xmlnode::xml_pop(const char* axml)
	{
		tinyxml2::XMLDocument ldocument;
		if (!ngl::xml::readxml(axml, ldocument))
		{
			return false;
		}
		tinyxml2::XMLElement* lelement = ngl::xml::get_child(ldocument, "xmlnode");
		if (lelement == nullptr)
		{
			return false;
		}
		return xml_pop(lelement);
	}

	bool xmlnode::xml_push(const char* axml) const
	{
		tinyxml2::XMLDocument ldocument;
		tinyxml2::XMLElement* lelement = xml::set_child(ldocument, "xmlnode");
		if (lelement == nullptr || !xml_push(lelement))
		{
			return false;
		}
		return xml::writexml(axml, ldocument);
	}

	bool xmlnode::xml_pop(tinyxml2::XMLElement* aele)
	{
		if (aele == nullptr)
		{
			return false;
		}

		xarg_db db{};
		xarg_db dbcross{};
		xarg_info publicinfo{};
		xarg_mail mail{};
		xarg_telnet telnet{};
		xarg_wss wss{};
		xarg_redis redis{};

		if (!xml_detail::pop_required(aele, "db", db) || !xml_detail::pop_required(aele, "public", publicinfo))
		{
			return false;
		}
		if (!xml_detail::pop_optional_any(aele, { "dbcross", "crossdb" }, dbcross))
		{
			return false;
		}
		if (!xml_detail::pop_optional(aele, "mail", mail) ||
			!xml_detail::pop_optional(aele, "telnet", telnet) ||
			!xml_detail::pop_optional(aele, "wss", wss) ||
			!xml_detail::pop_optional(aele, "redis", redis))
		{
			return false;
		}

		m_db = std::move(db);
		m_dbcross = std::move(dbcross);
		m_public = std::move(publicinfo);
		m_mail = std::move(mail);
		m_telnet = std::move(telnet);
		m_wss = std::move(wss);
		m_redis = std::move(redis);
		return true;
	}

	bool xmlnode::xml_push(tinyxml2::XMLElement* aele) const
	{
		if (aele == nullptr)
		{
			return false;
		}

		if (!xml_serialize<false, xarg_db>::push(aele, "db", m_db) ||
			!xml_serialize<false, xarg_info>::push(aele, "public", m_public))
		{
			return false;
		}

		return xml_detail::push_if_present(aele, "dbcross", m_dbcross) &&
			xml_detail::push_if_present(aele, "mail", m_mail) &&
			xml_detail::push_if_present(aele, "telnet", m_telnet) &&
			xml_detail::push_if_present(aele, "wss", m_wss) &&
			xml_detail::push_if_present(aele, "redis", m_redis);
	}

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

	bool xmlnode::set_server(const char* aservertypename)
	{
		if (aservertypename == nullptr)
		{
			log_error()->print("xmlnode invalid server type [null]");
			return false;
		}
		NODE_TYPE lnodetype = em<NODE_TYPE>::get_enum(aservertypename);
		if (lnodetype == em<NODE_TYPE>::enum_null())
		{
			log_error()->print("xmlnode invalid server type [{}]", aservertypename);
			return false;
		}
		m_nodename = aservertypename;
		m_nodetype = lnodetype;
		return true;
	}

	void xmlnode::set_nodeid(int atid, int atcount)
	{
		int16_t tid = -1;
		int16_t tcount = -1;
		if (!xml_detail::parse_node_part(atid, tid) || !xml_detail::parse_node_part(atcount, tcount))
		{
			m_nodeid = nnodeid::nodeid(static_cast<int16_t>(-1), static_cast<int16_t>(-1));
			m_tid = -1;
			m_tcount = -1;
			return;
		}
		m_nodeid = nnodeid::nodeid(tid, tcount);
		m_tid = tid;
		m_tcount = tcount;
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
		namespace fs = std::filesystem;

		m_configname = aname;
		const fs::path base(axmlpath);
		const std::array<fs::path, 4> candidates = {
			base / "config" / std::format("config_{}.xml", aname),
			base / std::format("config_{}.xml", aname),
			base / "config" / "config.xml",
			base / "config.xml",
		};

		fs::path config_path;
		// Probe the most specific file first, then fall back to generic config.xml.
		for (const fs::path& candidate : candidates)
		{
			if (tools::file_exists(candidate.string()))
			{
				config_path = candidate;
				break;
			}
		}

		if (config_path.empty())
		{
			m_configfile = candidates.front().string();
			log_error()->print("xmlnode config file not found [{}]", m_configfile);
			return false;
		}

		m_configfile = config_path.string();
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
