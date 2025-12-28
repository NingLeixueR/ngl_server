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

#include "xml.h"

namespace ngl
{
	template <typename T>
	struct xml_serialize
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, T& adata);
	};

	template <>
	struct xml_serialize<int8_t>
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, int8_t& adata)
		{
			if (akey == nullptr)
			{
				return xml::get(aele, adata);
			}
			return xml::get(aele, akey, adata);
		}
	};

	template <>
	struct xml_serialize<int16_t>
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, int16_t& adata)
		{
			if (akey == nullptr)
			{
				return xml::get(aele, adata);
			}
			return xml::get(aele, akey, adata);
		}
	};

	template <>
	struct xml_serialize<int32_t>
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, int32_t& adata)
		{
			if (akey == nullptr)
			{
				return xml::get(aele, adata);
			}
			return xml::get(aele, akey, adata);
		}
	};

	template <>
	struct xml_serialize<int64_t>
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, int64_t& adata)
		{
			if (akey == nullptr)
			{
				return xml::get(aele, adata);
			}
			return xml::get(aele, akey, adata);
		}
	};

	template <>
	struct xml_serialize<uint8_t>
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, uint8_t& adata)
		{
			if (akey == nullptr)
			{
				return xml::get(aele, adata);
			}
			return xml::get(aele, akey, adata);
		}
	};

	template <>
	struct xml_serialize<uint16_t>
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, uint16_t& adata)
		{
			if (akey == nullptr)
			{
				return xml::get(aele, adata);
			}
			return xml::get(aele, akey, adata);
		}
	};

	template <>
	struct xml_serialize<uint32_t>
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, uint32_t& adata)
		{
			if (akey == nullptr)
			{
				return xml::get(aele, adata);
			}
			return xml::get(aele, akey, adata);
		}
	};

	template <>
	struct xml_serialize<uint64_t>
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, uint64_t& adata)
		{
			if (akey == nullptr)
			{
				return xml::get(aele, adata);
			}
			return xml::get(aele, akey, adata);
		}
	};

	template <>
	struct xml_serialize<std::string>
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, std::string& adata)
		{
			if (akey == nullptr)
			{
				return xml::get(aele, adata);
			}
			return xml::get(aele, akey, adata);
		}
	};

	template <typename T>
	struct xml_serialize<std::vector<T>>
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, std::vector<T>& adata)
		{ 
			return xml::foreach(aele, akey, [&adata](tinyxml2::XMLElement* apt)
				{
					T ltemp;
					if constexpr (std::is_class<T>::value)
					{
						if (xml_serialize<T>::xml_pop(apt, tools::type_name<T>().c_str(), ltemp))
						{
							adata.push_back(ltemp);
						}
					}
					else
					{
						if (xml_serialize<T>::xml_pop(apt, ltemp))
						{
							adata.push_back(ltemp);
						}
					}
				});
		}
	};

	template <typename T>
	struct xml_serialize<std::list<T>>
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, std::list<T>& adata)
		{
			return xml::foreach(aele, akey, [&adata](tinyxml2::XMLElement* apt)
				{
					T ltemp;
					if constexpr (std::is_class<T>::value)
					{
						if (xml_serialize<T>::xml_pop(apt, tools::type_name<T>().c_str(), ltemp))
						{
							adata.push_back(ltemp);
						}
					}
					else
					{
						if (xml_serialize<T>::xml_pop(apt, ltemp))
						{
							adata.push_back(ltemp);
						}
					}
				});
		}
	};

	template <typename T>
	struct xml_serialize<std::set<T>>
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, std::set<T>& adata)
		{
			return xml::foreach(aele, akey, [&adata](tinyxml2::XMLElement* apt)
				{
					T ltemp;
					if constexpr (std::is_class<T>::value)
					{
						if (xml_serialize<T>::xml_pop(apt, tools::type_name<T>().c_str(), ltemp))
						{
							adata.push_back(ltemp);
						}
					}
					else
					{
						if (xml_serialize<T>::xml_pop(apt, ltemp))
						{
							adata.push_back(ltemp);
						}
					}
				});
		}
	};

	template <typename TKEY, typename TVALUE>
	struct xml_serialize<std::map<TKEY, TVALUE>>
	{
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, std::map<TKEY, TVALUE>& adata)
		{
			return xml::foreach(aele, akey, [&adata](tinyxml2::XMLElement* apt)
				{
					TKEY lkey;
					TVALUE lvalue;
					if (xml_serialize<TKEY>::xml_pop(apt, "key", lkey) && xml_serialize<TVALUE>::xml_pop(apt, "value", lvalue))
					{
						adata[lkey] = lvalue;
						return true;
					}
					return false;
				});
		}
	};

	class xserialize
	{
	public:
		static bool xml_pop(tinyxml2::XMLElement* aele)
		{
			return true;
		}

		template <typename T>
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, T& adata)
		{
			return xml_serialize<T>::xml_pop(aele, akey, adata);
		}

		template <typename T, typename ...TARGS>
		static bool xml_pop(tinyxml2::XMLElement* aele, const char* akey, T& adata, TARGS&... aargs)
		{
			return xml_pop(aele, akey, adata) && xml_pop(aele, aargs...);
		}
	};
}//namespace ngl

namespace ngl
{
	template <typename T>
	bool xml_serialize<T>::xml_pop(tinyxml2::XMLElement* aele, const char* akey, T& adata)
	{
		if constexpr (std::is_enum<T>::value)
		{
			int32_t lvalue = 0;
			if (xml_serialize<int32_t>::xml_pop(aele, akey, lvalue))
			{
				adata = (T)lvalue;
				return true;
			}
		}
		else
		{
			return adata.xml_pop(xml::get_child(aele, akey));
		}
		return false;
	}

	struct skill_template
	{
		std::string m_name;			// 技能名称
		std::string m_describe;		// 技能描述
		std::map<int32_t, int32_t>	m_cooldown;		// 冷却时间(毫秒)
		std::map<int32_t, std::map<ngl::EnumAttribute, int32_t>> m_consume;  // 释放技能消耗属性值

		bool xml_pop(tinyxml2::XMLElement* aele)
		{
			return xserialize::xml_pop(aele, "m_name", m_name, "m_describe", m_describe, "m_cooldown", m_cooldown, "m_consume", m_consume);
		}

		bool xml_pop()
		{
			tinyxml2::XMLDocument ldocument;
			tinyxml2::XMLElement* lelement;
			xml::readxml("skill_template.xml", ldocument, lelement);
			return xml_pop(lelement);
		}
	};
}//namespace ngl