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
		static bool readxml(const char* aname, tinyxml2::XMLDocument& axml, tinyxml2::XMLElement*& acon)
		{
			if (axml.LoadFile(aname) != tinyxml2::XML_SUCCESS)
			{
				std::cout << "Failed to load XML file[" << aname << "]." << std::endl;
				return false;
			}
			acon = axml.FirstChildElement("con");
			return acon != nullptr;
		}

		static tinyxml2::XMLElement* get_child(tinyxml2::XMLElement* aele, const char* astr)
		{
			std::vector<std::string> lvec;
			if (tools::splite(astr, ".", lvec) == false)
			{
				return nullptr;
			}
			tinyxml2::XMLElement* valElement = aele;
			for (std::string& item : lvec)
			{
				valElement = valElement->FirstChildElement(item.c_str());
				if (valElement == nullptr)
				{
					return nullptr;
				}
			}
			return valElement;
		}

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

		static void foreach(tinyxml2::XMLElement* aele, const char* akey, const std::function<void(tinyxml2::XMLElement*)>& afun)
		{
			for (tinyxml2::XMLNode* child = aele->FirstChildElement(); child; child = child->NextSiblingElement())
			{
				tinyxml2::XMLElement* lxele = child->ToElement();
				if (lxele != nullptr&& std::string(lxele->Name()) == akey)
				{
					afun(lxele);
				}
			}
		}

		static void foreach(tinyxml2::XMLElement* aele, const std::function<void(tinyxml2::XMLElement*)>& afun)
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

		static void foreach_xmlattr(tinyxml2::XMLElement* aele, const std::function<void(const char*, const char*)>& afun)
		{
			for (const tinyxml2::XMLAttribute* attribute = aele->FirstAttribute(); attribute; attribute = attribute->Next())
			{
				const char* lkey = attribute->Name();
				const char* lval = attribute->Value();
				afun(lkey, lval);
			}
		}
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
		static i32_id				 m_nodeid;

		static void init();

		static NODE_TYPE node_type();

		static void set_server(const char* aservertypename, int anodeid);

		static void load(const std::string& axmlpath, const std::string& aname);

		static void read_config(tinyxml2::XMLElement* apt, xmlinfo& anfo);

		static void read_config(const char* akey, std::map<i32_serverid, xmlinfo>& anfo);

		static bool read_public_config(xmlinfo& anfo);

		static bool read_db_arg(const char* aname, dbarg& m_dbarg);

		static void loaddb();

		static void loadcrossdb();

		static void loadpublic();

		static xmlinfo* get_publicconfig();
	};

}

#define nconfig ngl::xmlnode