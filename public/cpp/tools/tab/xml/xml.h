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
#pragma once

#include "xml_serialize.h"
#include "csvtable.h"
#include "xmlinfo.h"
#include "tools.h"
#include "type.h"
#include "nlog.h"

namespace ngl
{
	class xmlnode
	{
		xarg_db		m_db;
		xarg_db		m_dbcross;
		xarg_info	m_public;
		xarg_mail	m_mail;
		xarg_telnet	m_telnet;
		xarg_redis	m_redis;

		std::string	m_nodename;
		NODE_TYPE	m_nodetype;
		i32_serverid m_nodeid = 0;		// server id
		int16_t		m_tid = 0;			// 对应tab_servers表中的id
		int16_t		m_tcount = 0;		// 实例id

		std::string m_servername;			// 程序名称

		xmlnode() {}
	public:
		static xmlnode& instance()
		{
			static xmlnode ltemp;
			return ltemp;
		}

		DXMLSERIALIZE(xmlnode, false, m_db, m_dbcross,  m_public, m_mail, m_telnet)

		void init();

		i16_area area();

		void set_server(const char* aservertypename);

		void set_nodeid(int atid, int atcount);

		void set_servername(const std::string& asername);

		std::string& servername();

		void load(const std::string& axmlpath, const std::string& aname);

		xarg_info* info();

		int16_t tcount();

		int16_t	tid();

		i32_serverid nodeid();

		NODE_TYPE nodetype();

		xarg_db& db();

		xarg_db& crossdb();

		xarg_mail& mail();

		xarg_telnet& telnet();

		xarg_redis&	redis();
	};
}//namespace ngl
#define nconfig ngl::xmlnode::instance()

namespace ngl
{
	template <typename T>
	bool xml::get(tinyxml2::XMLElement* aele, const char* akey, T& aval)
	{
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
		if (!val)
		{
			return false;
		}
		aval = tools::lexical_cast<T>(val);
		return true;
	}

	template <typename T>
	bool xml::get(tinyxml2::XMLElement* aele, T& aval)
	{
		return get(aele, nullptr, aval);
	}

	template <typename T>
	bool xml::set(tinyxml2::XMLElement* aele, const char* akey, const T& aval)
	{
		tinyxml2::XMLElement* valElement = aele;
		if (akey != nullptr)
		{
			valElement = set_child(aele, akey);
			if (!valElement)
			{
				return false;
			}
		}
		valElement->SetText(tools::lexical_cast<std::string>(aval).c_str());
		return true;
	}

	template <typename T>
	bool xml::set(tinyxml2::XMLElement* aele, const T& aval)
	{
		return set(aele, nullptr, aval);
	}

	template <typename T>
	bool xml::get_xmlattr(tinyxml2::XMLElement* aele, const char* akey, T& aval)
	{
		const char* val = aele->Attribute(akey);
		if (!val)
		{
			return false;
		}
		aval = tools::lexical_cast<T>(val);
		return true;
	}

	template <typename T>
	bool xml::set_xmlattr(tinyxml2::XMLElement* aele, const char* akey, const T& aval)
	{
		aele->SetAttribute(akey, tools::lexical_cast<std::string>(aval).c_str());
		return true;
	}
}//namespace ngl