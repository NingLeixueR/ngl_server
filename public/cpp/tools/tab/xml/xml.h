/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project: ngl_server
* License: MIT
*/
// File overview: Declares interfaces for xml.

#pragma once

#include "actor/tab/csvtable.h"
#include "tools/log/nlog.h"
#include "tools/tab/xml/xml_serialize.h"
#include "tools/tab/xml/xmlinfo.h"
#include "tools/tools.h"
#include "tools/type.h"

namespace ngl
{
	class xmlnode
	{
		// Runtime node configuration loaded once from XML and then exposed as a
		// process-wide singleton through `nconfig`.
		xarg_db		m_db;
		xarg_db		m_dbcross;
		xarg_info	m_public;
		xarg_mail	m_mail;
		xarg_telnet	m_telnet;
		xarg_wss	m_wss;
		xarg_redis	m_redis;

		std::string	m_nodename;
		NODE_TYPE	m_nodetype{};
		i32_serverid m_nodeid = 0;
		int16_t		m_tid = 0;
		int16_t		m_tcount = 0;

		std::string m_servername;
		std::string m_configfile;
		std::string m_configname;

	public:
		static xmlnode& instance()
		{
			static xmlnode ltemp;
			return ltemp;
		}

		DXMLSERIALIZE(xmlnode, false, m_db, m_dbcross, m_public, m_mail, m_telnet, m_wss)

		void init();

		i16_area area();

		bool set_server(const char* aservertypename);

		void set_nodeid(int16_t atid, int16_t atcount);

		void set_servername(const std::string& asername);

		std::string& servername();

		bool load(const std::string& axmlpath, const std::string& aname);

		const std::string& nodename() const;

		const std::string& config_file() const;

		const std::string& config_name() const;

		xarg_info* info();

		int16_t tcount();

		int16_t	tid();

		i32_serverid nodeid();

		NODE_TYPE nodetype();

		xarg_db& db();

		xarg_db& crossdb();

		xarg_db::edb dbedb();

		xarg_mail& mail();

		xarg_telnet& telnet();

		xarg_wss& wss();

		xarg_redis& redis();
	};
}//namespace ngl

#define nconfig ngl::xmlnode::instance()

namespace ngl
{
template <typename T>
bool xml::get(tinyxml2::XMLElement* aele, const char* akey, T& aval)
{
		if (aele == nullptr)
		{
			return false;
		}
		tinyxml2::XMLElement* valElement = aele;
		if (akey != nullptr)
		{
			valElement = aele->FirstChildElement(akey);
			if (!valElement)
			{
				return false;
			}
		}
		const char* val = valElement->GetText();
		if (val == nullptr)
		{
			return true;
		}
		try
		{
			// XML stores scalars as text, so parsing goes through the project
			// lexical cast helper.
			T lvalue = tools::lexical_cast<T>(val);
			aval = std::move(lvalue);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	template <typename T>
	bool xml::get(tinyxml2::XMLElement* aele, T& aval)
	{
		return get(aele, nullptr, aval);
	}

template <typename T>
bool xml::set(tinyxml2::XMLElement* aele, const char* akey, const T& aval)
{
		if (aele == nullptr)
		{
			return false;
		}
		tinyxml2::XMLElement* valElement = aele;
		if (akey != nullptr)
		{
			valElement = set_child(aele, akey);
			if (!valElement)
			{
				return false;
			}
		}
		try
		{
			std::string lvalue = tools::lexical_cast<std::string>(aval);
			valElement->SetText(lvalue.c_str());
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	template <typename T>
	bool xml::set(tinyxml2::XMLElement* aele, const T& aval)
	{
		return set(aele, nullptr, aval);
	}

	template <typename T>
	bool xml::get_xmlattr(tinyxml2::XMLElement* aele, const char* akey, T& aval)
	{
		if (aele == nullptr || akey == nullptr)
		{
			return false;
		}
		const char* val = aele->Attribute(akey);
		if (!val)
		{
			return false;
		}
		try
		{
			T lvalue = tools::lexical_cast<T>(val);
			aval = std::move(lvalue);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	template <typename T>
	bool xml::set_xmlattr(tinyxml2::XMLElement* aele, const char* akey, const T& aval)
	{
		if (aele == nullptr || akey == nullptr)
		{
			return false;
		}
		try
		{
			std::string lvalue = tools::lexical_cast<std::string>(aval);
			aele->SetAttribute(akey, lvalue.c_str());
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
}//namespace ngl
