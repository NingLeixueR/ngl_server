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
#include "xml_serialize.h"

namespace ngl
{
	bool xml::readxml(const char* aname, tinyxml2::XMLDocument& axml)
	{
		if (axml.LoadFile(aname) != tinyxml2::XML_SUCCESS)
		{
			std::cout << "Failed to load XML file[" << aname << "]." << std::endl;
			return false;
		}
		return true;
	}

	bool xml::writexml(const char* aname, tinyxml2::XMLDocument& axml)
	{
		tinyxml2::XMLError lerror = axml.SaveFile(aname);
		return lerror == tinyxml2::XML_SUCCESS;
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

	tinyxml2::XMLElement* xml::get_child(tinyxml2::XMLDocument& axml, const char* astr)
	{
		return axml.FirstChildElement(astr);
	}

	tinyxml2::XMLElement* xml::set_child(tinyxml2::XMLElement* aele, const char* astr)
	{
		return aele->InsertNewChildElement(astr);
	}

	tinyxml2::XMLElement* xml::set_child(tinyxml2::XMLDocument& axml, const char* astr)
	{
		tinyxml2::XMLElement* lelement = axml.NewElement(astr);
		axml.LinkEndChild(lelement);
		return lelement;
	}

	bool xml::foreach(tinyxml2::XMLElement* aele, const char* akey, const std::function<bool(tinyxml2::XMLElement*)>& afun)
	{
		for (tinyxml2::XMLNode* child = aele->FirstChildElement(); child; child = child->NextSiblingElement())
		{
			tinyxml2::XMLElement* lxele = child->ToElement();
			if (lxele != nullptr && std::string(lxele->Name()) == akey)
			{
				if (!afun(lxele))
				{
					return false;
				}
			}
		}
		return true;
	}

	bool xml::foreach(tinyxml2::XMLElement* aele, const std::function<bool(tinyxml2::XMLElement*)>& afun)
	{
		for (tinyxml2::XMLNode* child = aele->FirstChildElement(); child; child = child->NextSiblingElement())
		{
			tinyxml2::XMLElement* lxele = child->ToElement();
			if (lxele != nullptr)
			{
				if (!afun(lxele))
				{
					return false;
				}
			}
		}
		return true;
	}

	bool xml::foreach_xmlattr(tinyxml2::XMLElement* aele, const std::function<bool(const char*, const char*)>& afun)
	{
		for (const tinyxml2::XMLAttribute* attribute = aele->FirstAttribute(); attribute; attribute = attribute->Next())
		{
			const char* lkey = attribute->Name();
			const char* lval = attribute->Value();
			if (!afun(lkey, lval))
			{
				return false;
			}
		}
		return true;
	}

	std::string xml::str(tinyxml2::XMLDocument& axml)
	{
		tinyxml2::XMLPrinter printer;
		axml.Print(&printer);
		return std::string(printer.CStr());
	}
}//namespace ngl