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

namespace ngl
{
	class xmlnode
	{
	public:
		static tinyxml2::XMLDocument m_doc;
		static tinyxml2::XMLElement* m_con;
		static dbserver_info		 m_db;
		static dbserver_info		 m_crossdb;
		static xmlinfo				 m_publicinfo;
		static std::string			 m_nodename;
		static NODE_TYPE			 m_nodetype;
		static i32_id				 m_nodeid;		// server id
		static int16_t				 m_tid;			// 对应tab_servers表中的id
		static int16_t				 m_tcount;		// 实例id
		static mail_info			 m_mail;
		static telnet_info			 m_telnet;

		static void init();

		static NODE_TYPE node_type();

		static i16_area area();

		static void set_server(const char* aservertypename);

		static void set_nodeid(int atid, int atcount);

		static void load(const std::string& axmlpath, const std::string& aname);

		static void read_config_xmlattr(tinyxml2::XMLElement* apt, xmlinfo& anfo);

		static void read_config_xmlattr(const char* akey, std::map<i32_serverid, xmlinfo>& anfo);

		static void read_config(tinyxml2::XMLElement* apt, xmlinfo& anfo);

		static bool read_public_config(xmlinfo& anfo);

		static bool read_db_arg(const char* aname, dbarg& m_dbarg);

		static bool read_mail_arg(const char* aname, mailarg& amailarg);

		static bool read_telnet_arg(const char* aname, telnetarg& amailarg);

		static void loaddb();

		static void loadcrossdb();

		static void loadmail();

		static void loadtelnet();

		static void loadpublic();

		static xmlinfo* get_publicconfig();
	};
}//namespace ngl
#define nconfig ngl::xmlnode


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