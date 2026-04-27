/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project Name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
*
* This file is part of the ngl_server project, released under the MIT License.
* You are free to use, modify, and distribute this project in accordance with the license terms,
* including for commercial purposes, provided that you retain the original copyright and license notice.
*
* For full license details, see the LICENSE file in the project root directory:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for xml.


#include "tools/tab/xml/xml_serialize.h"

#include <cstring>

namespace ngl
{
	bool xml::readxml(const char* aname, tinyxml2::XMLDocument& axml)
	{
		if (aname == nullptr || *aname == '\0')
		{
			return false;
		}
		if (axml.LoadFile(aname) != tinyxml2::XML_SUCCESS)
		{
			std::cout << "Failed to load XML file[" << aname << "]: " << axml.ErrorStr() << std::endl;
			return false;
		}
		return true;
	}

	bool xml::writexml(const char* aname, tinyxml2::XMLDocument& axml)
	{
		if (aname == nullptr || *aname == '\0')
		{
			return false;
		}
		return axml.SaveFile(aname) == tinyxml2::XML_SUCCESS;
	}

	tinyxml2::XMLElement* xml::get_child(tinyxml2::XMLElement* aele, const char* astr)
	{
		if (aele == nullptr || astr == nullptr || *astr == '\0')
		{
			return nullptr;
		}

		if (std::strchr(astr, '.') == nullptr)
		{
			return aele->FirstChildElement(astr);
		}

		tinyxml2::XMLElement* current = aele;
		std::string segment;
		segment.reserve(tools::strlen(astr));

		const char* begin = astr;
		while (current != nullptr && *begin != '\0')
		{
			const char* dot = std::strchr(begin, '.');
			const std::size_t length = dot == nullptr ? tools::strlen(begin) : static_cast<std::size_t>(dot - begin);
			if (length == 0)
			{
				return nullptr;
			}
			segment.assign(begin, length);
			current = current->FirstChildElement(segment.c_str());
			if (dot == nullptr)
			{
				return current;
			}
			begin = dot + 1;
		}
		return nullptr;
	}

	tinyxml2::XMLElement* xml::get_child(tinyxml2::XMLDocument& axml, const char* astr)
	{
		if (astr == nullptr || *astr == '\0')
		{
			return nullptr;
		}
		return axml.FirstChildElement(astr);
	}

	tinyxml2::XMLElement* xml::set_child(tinyxml2::XMLElement* aele, const char* astr)
	{
		if (aele == nullptr || astr == nullptr || *astr == '\0')
		{
			return nullptr;
		}
		return aele->InsertNewChildElement(astr);
	}

	tinyxml2::XMLElement* xml::set_child(tinyxml2::XMLDocument& axml, const char* astr)
	{
		if (astr == nullptr || *astr == '\0')
		{
			return nullptr;
		}
		tinyxml2::XMLElement* lelement = axml.NewElement(astr);
		axml.LinkEndChild(lelement);
		return lelement;
	}

	bool xml::foreach(tinyxml2::XMLElement* aele, const char* akey, const std::function<bool(tinyxml2::XMLElement*)>& afun)
	{
		if (aele == nullptr || akey == nullptr)
		{
			return false;
		}
		for (tinyxml2::XMLElement* child = aele->FirstChildElement(akey); child != nullptr; child = child->NextSiblingElement(akey))
		{
			if (!afun(child))
			{
				return false;
			}
		}
		return true;
	}

	bool xml::foreach(tinyxml2::XMLElement* aele, const std::function<bool(tinyxml2::XMLElement*)>& afun)
	{
		if (aele == nullptr)
		{
			return false;
		}
		for (tinyxml2::XMLElement* child = aele->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
		{
			if (!afun(child))
			{
				return false;
			}
		}
		return true;
	}

	bool xml::foreach_xmlattr(tinyxml2::XMLElement* aele, const std::function<bool(const char*, const char*)>& afun)
	{
		if (aele == nullptr)
		{
			return false;
		}
		for (const tinyxml2::XMLAttribute* attribute = aele->FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
		{
			if (!afun(attribute->Name(), attribute->Value()))
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
