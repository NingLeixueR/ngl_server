/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "tinyxml2.h"
#include "csvtable.h"
#include "xmlinfo.h"
#include "tools.h"
#include "type.h"
#include "nlog.h"

#include <functional>

namespace ngl
{
	class xml
	{
	public:
		static bool readxml(const char* aname, tinyxml2::XMLDocument& axml, tinyxml2::XMLElement*& acon);

		static tinyxml2::XMLElement* get_child(tinyxml2::XMLElement* aele, const char* astr);

		template <typename T>
		static bool get(tinyxml2::XMLElement* aele, const char* akey, T& aval)
		{
			tinyxml2::XMLElement* valElement = aele->FirstChildElement(akey);
			if (!valElement)
			{
				return false;
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
		static bool get_xmlattr(tinyxml2::XMLElement* aele, const char* akey, T& aval)
		{
			if (aele == nullptr)
			{
				return false;
			}
			const char* val = aele->Attribute(akey);
			if (!val) 
			{
				return false;
			}
			aval = tools::lexical_cast<T>(val);
			return true;
		}

		static void foreach(tinyxml2::XMLElement* aele, const char* akey, const std::function<void(tinyxml2::XMLElement*)>& afun);

		static void foreach(tinyxml2::XMLElement* aele, const std::function<void(tinyxml2::XMLElement*)>& afun);

		static void foreach_xmlattr(tinyxml2::XMLElement* aele, const std::function<void(const char*, const char*)>& afun);
	};

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
		static int16_t				 m_tid;			// ��Ӧtab_servers���е�id
		static int16_t				 m_tcount;		// ʵ��id
		static mail_info			 m_mail;
		static telnet_info			 m_telnet;

		static void init();

		static NODE_TYPE node_type();

		static i16_area area();

		static void set_server(const char* aservertypename);

		static void set_nodeid(int atid, int atcount);

		static void load(const std::string& axmlpath, const std::string& aname);

		static void read_config(tinyxml2::XMLElement* apt, xmlinfo& anfo);

		static void read_config(const char* akey, std::map<i32_serverid, xmlinfo>& anfo);

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