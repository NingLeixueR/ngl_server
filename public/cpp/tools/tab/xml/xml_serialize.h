/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: XML serialization helpers for reading/writing config structures.

#pragma once

#include "tinyxml2.h"

#include <functional>
#include <algorithm>
#include <ranges>
#include <vector>
#include <string>
#include <list>
#include <set>
#include <map>

namespace ngl
{
	class xml
	{
	public:
		static bool readxml(const char* aname, tinyxml2::XMLDocument& axml);

		static bool writexml(const char* aname, tinyxml2::XMLDocument& axml);

		static tinyxml2::XMLElement* get_child(tinyxml2::XMLElement* aele, const char* astr);

		static tinyxml2::XMLElement* get_child(tinyxml2::XMLDocument& axml, const char* astr);

		static tinyxml2::XMLElement* set_child(tinyxml2::XMLElement* aele, const char* astr);

		static tinyxml2::XMLElement* set_child(tinyxml2::XMLDocument& axml, const char* astr);

		template <typename T>
		static bool get(tinyxml2::XMLElement* aele, const char* akey, T& aval);

		template <typename T>
		static bool get(tinyxml2::XMLElement* aele, T& aval);

		template <typename T>
		static bool set(tinyxml2::XMLElement* aele, const char* akey, const T& aval);

		template <typename T>
		static bool set(tinyxml2::XMLElement* aele, const T& aval);

		template <typename T>
		static bool get_xmlattr(tinyxml2::XMLElement* aele, const char* akey, T& aval);

		template <typename T>
		static bool set_xmlattr(tinyxml2::XMLElement* aele, const char* akey, const T& aval);

		static std::string str(tinyxml2::XMLDocument& axml);

		static bool foreach(tinyxml2::XMLElement* aele, const char* akey, const std::function<bool(tinyxml2::XMLElement*)>& afun);

		static bool foreach(tinyxml2::XMLElement* aele, const std::function<bool(tinyxml2::XMLElement*)>& afun);

		static bool foreach_xmlattr(tinyxml2::XMLElement* aele, const std::function<bool(const char*, const char*)>& afun);
	};

	template <bool ATTR, typename T>
	struct xml_serialize
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, const T& adata);
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, T& adata);
	};

	template <bool ATTR, typename T>
	struct xml_serialize<ATTR, T*>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, const T*& adata)
		{
			if (adata == nullptr)
			{
				return false;
			}
			return xml_serialize<ATTR, T>::push(aele, akey, *adata);
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, T*& adata)
		{
			if (adata == nullptr)
			{
				return false;
			}
			return xml_serialize<ATTR, T>::pop(aele, akey, *adata);
		}
	};

	template <bool ATTR>
	struct xbtype
	{
		template <typename T>
		static bool push(tinyxml2::XMLElement* aele, const char* akey, const T& adata)
		{
			if constexpr (ATTR)
			{
				return xml::set_xmlattr(aele, akey, adata);
			}
			else
			{
				return xml::set(aele, akey, adata);
			}
		}

		template <typename T>
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, T& adata)
		{
			if constexpr (ATTR)
			{
				return xml::get_xmlattr(aele, akey, adata);
			}
			else
			{
				return xml::get(aele, akey, adata);
			}
		}
	};

	template <bool ATTR>
	struct xml_serialize<ATTR, int8_t>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, int8_t adata)
		{
			return xbtype<ATTR>::push(aele, akey, adata);
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, int8_t& adata)
		{
			return xbtype<ATTR>::pop(aele, akey, adata);
		}
	};

	template <bool ATTR>
	struct xml_serialize<ATTR, int16_t>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, int16_t adata)
		{
			return xbtype<ATTR>::push(aele, akey, adata);
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, int16_t& adata)
		{
			return xbtype<ATTR>::pop(aele, akey, adata);
		}
	};

	template <bool ATTR>
	struct xml_serialize<ATTR, int32_t>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, int32_t adata)
		{
			return xbtype<ATTR>::push(aele, akey, adata);
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, int32_t& adata)
		{
			return xbtype<ATTR>::pop(aele, akey, adata);
		}
	};

	template <bool ATTR>
	struct xml_serialize<ATTR, int64_t>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, int64_t adata)
		{
			return xbtype<ATTR>::push(aele, akey, adata);
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, int64_t& adata)
		{
			return xbtype<ATTR>::pop(aele, akey, adata);
		}
	};

	template <bool ATTR>
	struct xml_serialize<ATTR, uint8_t>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, uint8_t adata)
		{
			return xbtype<ATTR>::push(aele, akey, adata);
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, uint8_t& adata)
		{
			return xbtype<ATTR>::pop(aele, akey, adata);
		}
	};

	template <bool ATTR>
	struct xml_serialize<ATTR, uint16_t>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, uint16_t adata)
		{
			return xbtype<ATTR>::push(aele, akey, adata);
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, uint16_t& adata)
		{
			return xbtype<ATTR>::pop(aele, akey, adata);
		}
	};

	template <bool ATTR>
	struct xml_serialize<ATTR, uint32_t>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, uint32_t adata)
		{
			return xbtype<ATTR>::push(aele, akey, adata);
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, uint32_t& adata)
		{
			return xbtype<ATTR>::pop(aele, akey, adata);
		}
	};

	template <bool ATTR>
	struct xml_serialize<ATTR, uint64_t>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, uint64_t adata)
		{
			return xbtype<ATTR>::push(aele, akey, adata);
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, uint64_t& adata)
		{
			return xbtype<ATTR>::pop(aele, akey, adata);
		}
	};

	template <bool ATTR>
	struct xml_serialize<ATTR, std::string>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, const std::string& adata)
		{
			return xbtype<ATTR>::push(aele, akey, adata);
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, std::string& adata)
		{
			return xbtype<ATTR>::pop(aele, akey, adata);
		}
	};

	template <bool ATTR, typename T>
	struct xml_serialize<ATTR, std::vector<T>>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, const std::vector<T>& adata)
		{
			if constexpr (!ATTR)
			{
				for (auto& item : adata)
				{
					tinyxml2::XMLElement* lpele = xml::set_child(aele, akey);
					if (lpele == nullptr)
					{
						return false;
					}
					if (!xml_serialize<ATTR, T>::push(lpele, nullptr, item))
					{
						return false;
					}
				}
				return true;
			}
			else
			{
				return false;
			}			
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, std::vector<T>& adata)
		{
			if constexpr (!ATTR)
			{
				std::vector<T> lparsed;
				if (!xml::foreach(aele, akey, [&lparsed](tinyxml2::XMLElement* apt)
					{
						T ltemp;
						if (!xml_serialize<ATTR, T>::pop(apt, nullptr, ltemp))
						{
							return false;
						}
						lparsed.push_back(std::move(ltemp));
						return true;
					}
				))
				{
					return false;
				}
				adata = std::move(lparsed);
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	template <bool ATTR, typename T>
	struct xml_serialize<ATTR, std::list<T>>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, const std::list<T>& adata)
		{
			if constexpr (!ATTR)
			{
				for (auto& item : adata)
				{
					tinyxml2::XMLElement* lpele = xml::set_child(aele, akey);
					if (lpele == nullptr || !xml_serialize<ATTR, T>::push(lpele, nullptr, item))
					{
						return false;
					}
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, std::list<T>& adata)
		{
			if constexpr (!ATTR)
			{
				std::list<T> lparsed;
				if (!xml::foreach(aele, akey, [&lparsed](tinyxml2::XMLElement* apt)
					{
						T ltemp;
						if (!xml_serialize<ATTR, T>::pop(apt, nullptr, ltemp))
						{
							return false;
						}
						lparsed.push_back(std::move(ltemp));
						return true;
					}
				))
				{
					return false;
				}
				adata = std::move(lparsed);
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	template <bool ATTR, typename T>
	struct xml_serialize<ATTR, std::set<T>>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, const std::set<T>& adata)
		{
			if constexpr (!ATTR)
			{
				for (auto& item : adata)
				{
					tinyxml2::XMLElement* lpele = xml::set_child(aele, akey);
					if (lpele == nullptr)
					{
						return false;
					}
					if (!xml_serialize<ATTR, T>::push(lpele, nullptr, item))
					{
						return false;
					}
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, std::set<T>& adata)
		{
			if constexpr (!ATTR)
			{
				std::set<T> lparsed;
				if (!xml::foreach(aele, akey, [&lparsed](tinyxml2::XMLElement* apt)
					{
						T ltemp;
						if (!xml_serialize<ATTR, T>::pop(apt, nullptr, ltemp))
						{
							return false;
						}
						if (!lparsed.insert(std::move(ltemp)).second)
						{
							return false;
						}
						return true;
					}
				))
				{
					return false;
				}
				adata = std::move(lparsed);
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	template <bool ATTR, typename TKEY, typename TVALUE, typename TCOMP, typename TALLOC>
	struct xml_serialize<ATTR, std::map<TKEY, TVALUE, TCOMP, TALLOC>>
	{
		using map_type = std::map<TKEY, TVALUE, TCOMP, TALLOC>;

		static bool push(tinyxml2::XMLElement* aele, const char* akey, const map_type& adata)
		{
			if constexpr (!ATTR)
			{
				for (auto& item : adata)
				{
					tinyxml2::XMLElement* lpele = xml::set_child(aele, akey);
					if (lpele == nullptr)
					{
						return false;
					}
					if (!xml_serialize<true, TKEY>::push(lpele, "key", item.first))
					{
						return false;
					}
					if (!xml_serialize<ATTR, TVALUE>::push(lpele, nullptr, item.second))
					{
						return false;
					}
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, map_type& adata)
		{
			if constexpr (!ATTR)
			{
				map_type lparsed;
				if (!xml::foreach(aele, akey, [&lparsed](tinyxml2::XMLElement* apt)
					{
						TKEY lkey;
						TVALUE lvalue;
						if (!xml_serialize<true, TKEY>::pop(apt, "key", lkey))
						{
							return false;
						}
						if (!xml_serialize<ATTR, TVALUE>::pop(apt, nullptr, lvalue))
						{
							return false;
						}
						if (!lparsed.emplace(std::move(lkey), std::move(lvalue)).second)
						{
							return false;
						}
						return true;
					}
				))
				{
					return false;
				}
				adata = std::move(lparsed);
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	template <bool ATTR>
	class xserialize
	{
		template <std::size_t... INDEX, typename ...TARGS>
		static bool pop(tinyxml2::XMLElement* aele, std::index_sequence<INDEX...>, const std::array<const char*, sizeof...(TARGS)>& akeys, TARGS&... aargs)
		{
			return (xml_serialize<ATTR, TARGS>::pop(aele, akeys[INDEX], aargs) && ...);
		}

		template <std::size_t... INDEX, typename ...TARGS>
		static bool push(tinyxml2::XMLElement* aele, std::index_sequence<INDEX...>, const std::array<const char*, sizeof...(TARGS)>& akeys, const TARGS&... aargs)
		{
			return (xml_serialize<ATTR, TARGS>::push(aele, akeys[INDEX], aargs) && ...);
		}

	public:

		template <typename ...TARGS>
		static bool pop(tinyxml2::XMLElement* aele, const std::array<const char*, sizeof...(TARGS)>& akeys, TARGS&... aargs)
		{
			return pop(aele, std::make_index_sequence<sizeof...(TARGS)>{}, akeys, aargs...);
		}

		template <typename ...TARGS>
		static bool push(tinyxml2::XMLElement* aele, const std::array<const char*, sizeof...(TARGS)>& akeys, const TARGS&... aargs)
		{
			return push(aele, std::make_index_sequence<sizeof...(TARGS)>{}, akeys, aargs...);
		}
	};
}//namespace ngl

namespace ngl
{
	template <bool ATTR, typename T>
	bool xml_serialize<ATTR, T>::push(tinyxml2::XMLElement* aele, const char* akey, const T& adata)
	{
		if constexpr (ATTR)
		{
			if constexpr (std::is_enum<T>::value)
			{
				return xml_serialize<ATTR, int32_t>::push(aele, akey, (int32_t)adata);
			}
		}
		else
		{
			if constexpr (std::is_enum<T>::value)
			{
				return xml_serialize<ATTR, int32_t>::push(aele, akey, (int32_t)adata);
			}
			else
			{
				if (akey == nullptr)
				{
					return adata.xml_push(aele);
				}
				tinyxml2::XMLElement* lchild = xml::set_child(aele, akey);
				if (lchild == nullptr)
				{
					return false;
				}
				return adata.xml_push(lchild);
			}
		}		
	}

	template <bool ATTR, typename T>
	bool xml_serialize<ATTR, T>::pop(tinyxml2::XMLElement* aele, const char* akey, T& adata)
	{
		if constexpr (ATTR)
		{
			if constexpr (std::is_enum<T>::value)
			{
				int32_t lvalue = 0;
				if (xml_serialize<ATTR, int32_t>::pop(aele, akey, lvalue))
				{
					adata = (T)lvalue;
					return true;
				}
				return false;
			}
		}
		else
		{
			if constexpr (std::is_enum<T>::value)
			{
				int32_t lvalue = 0;
				if (xml_serialize<ATTR, int32_t>::pop(aele, akey, lvalue))
				{
					adata = (T)lvalue;
					return true;
				}
				return false;
			}
			else
			{
				if (akey == nullptr)
				{
					return adata.xml_pop(aele);
				}
				tinyxml2::XMLElement* lchild = xml::get_child(aele, akey);
				if (lchild == nullptr)
				{
					return false;
				}
				return adata.xml_pop(lchild);
			}
		}		
	}
}//namespace ngl

#include "tools/serialize/ndefine.h"

#define DEF_XMLFUNCTION(XMLNAME)														\
	inline bool xml_pop(const char* axml)												\
	{																					\
		tinyxml2::XMLDocument ldocument;												\
		if (!ngl::xml::readxml(axml, ldocument))										\
		{																				\
			return false;																\
		}																				\
		tinyxml2::XMLElement* lelement = ngl::xml::get_child(ldocument, #XMLNAME);		\
		if (lelement == nullptr)														\
		{																				\
			return false;																\
		}																				\
		return xml_pop(lelement);														\
	}																					\
	inline bool xml_push(const char* axml)const											\
	{																					\
		tinyxml2::XMLDocument ldocument;												\
		tinyxml2::XMLElement* lelement = xml::set_child(ldocument, #XMLNAME);			\
		if (!xml_push(lelement))														\
		{																				\
			return false;																\
		}																				\
		return xml::writexml(axml, ldocument);											\
	}

#define DEF_XML(ATTR, ...)																\
	inline bool xml_pop(tinyxml2::XMLElement* aele)										\
	{																					\
		return ngl::xserialize<ATTR>::pop(aele, parms() __VA_OPT__(, )__VA_ARGS__);		\
	}																					\
	inline bool xml_push(tinyxml2::XMLElement* aele)const								\
	{																					\
		return ngl::xserialize<ATTR>::push(aele, parms() __VA_OPT__(, )__VA_ARGS__);	\
	}

#define DXMLSERIALIZE(XMLNAME, ATTR, ...)												\
	DEF_PARMNAME_(true, __VA_ARGS__)													\
	DEF_XMLFUNCTION(XMLNAME)															\
	DEF_XML(ATTR __VA_OPT__(, )__VA_ARGS__)