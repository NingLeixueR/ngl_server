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

		static const char* str(tinyxml2::XMLDocument& axml);

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

	template <bool ATTR>
	struct xbtype
	{
		template <typename T>
		static bool push(tinyxml2::XMLElement* aele, const char* akey, T adata)
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
				for (const auto& item : adata)
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
				return xml::foreach(aele, akey, [&adata](tinyxml2::XMLElement* apt)
					{
						T ltemp;
						if (!xml_serialize<ATTR, T>::pop(apt, nullptr, ltemp))
						{
							return false;
						}
						adata.push_back(ltemp);
						return true;
					}
				);
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
				for (const auto& item : adata)
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
				return xml::foreach(aele, akey, [&adata](tinyxml2::XMLElement* apt)
					{
						T ltemp;
						if (!xml_serialize<ATTR, T>::pop(apt, nullptr, ltemp))
						{
							return false;
						}
						adata.push_back(ltemp);
						return true;
					}
				);
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
				for (const auto& item : adata)
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
				return xml::foreach(aele, akey, [&adata](tinyxml2::XMLElement* apt)
					{
						T ltemp;
						if (!xml_serialize<ATTR, T>::pop(apt, nullptr, ltemp))
						{
							return false;
						}
						adata.insert(ltemp);
						return true;
					}
				);
			}
			else
			{
				return false;
			}
		}
	};

	template <bool ATTR, typename TKEY, typename TVALUE>
	struct xml_serialize<ATTR, std::map<TKEY, TVALUE>>
	{
		static bool push(tinyxml2::XMLElement* aele, const char* akey, const std::map<TKEY, TVALUE>& adata)
		{
			if constexpr (!ATTR)
			{
				for (const auto& item : adata)
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
			return false;
		}
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, std::map<TKEY, TVALUE>& adata)
		{
			if constexpr (!ATTR)
			{
				return xml::foreach(aele, akey, [&adata](tinyxml2::XMLElement* apt)
					{
						TKEY lkey;
						TVALUE lvalue;
						if (!xml_serialize<true, TKEY>::pop(apt, "key", lkey))
						{
							return false;
						}
						if (xml_serialize<ATTR, TVALUE>::pop(apt, nullptr, lvalue))
						{
							adata[lkey] = lvalue;
							return true;
						}
						return false;
					}
				);
			}
			return false;
		}
	};

	template <bool ATTR>
	struct xserialize
	{
		static bool pop(tinyxml2::XMLElement* aele)
		{
			return true;
		}

		template <typename T>
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, T& adata)
		{
			return xml_serialize<ATTR, T>::pop(aele, akey, adata);
		}

		template <typename T, typename ...TARGS>
		static bool pop(tinyxml2::XMLElement* aele, const char* akey, T& adata, TARGS&... aargs)
		{
			return pop(aele, akey, adata) && pop(aele, aargs...);
		}

		static bool push(tinyxml2::XMLElement* aele)
		{
			return true;
		}

		template <typename T>
		static bool push(tinyxml2::XMLElement* aele, const char* akey, const T& adata)
		{
			return xml_serialize<ATTR, T>::push(aele, akey, adata);
		}

		template <typename T, typename ...TARGS>
		static bool push(tinyxml2::XMLElement* aele, const char* akey, const T& adata, const TARGS&... aargs)
		{
			return push(aele, akey, adata) && push(aele, aargs...);
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
				return adata.xml_push(xml::set_child(aele, akey));
			}
		}		
		return false;
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
			}
			else
			{
				if (akey == nullptr)
				{
					return adata.xml_pop(aele);
				}
				return adata.xml_pop(xml::get_child(aele, akey));
			}
		}		
		return false;
	}
}//namespace ngl

#include "ndefine.h"

#define def_xmlfunction(XMLNAME)														\
	inline bool xml_pop(const char* axml)												\
	{																					\
		tinyxml2::XMLDocument ldocument;												\
		ngl::xml::readxml(axml, ldocument);												\
		tinyxml2::XMLElement* lelement = ngl::xml::get_child(ldocument, #XMLNAME);		\
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

#define def_xmlspecial(ATTR, ...)																			\
	inline bool xml_pop(tinyxml2::XMLElement* aele)															\
	{																										\
		nhelp<tinyxml2::XMLElement, ngl::xserialize<ATTR>> ltemp(parms(#__VA_ARGS__), aele);				\
		return ltemp.pop( __VA_ARGS__);																		\
	}																										\
	inline bool xml_push(tinyxml2::XMLElement* aele)const													\
	{																										\
		nhelp<tinyxml2::XMLElement, ngl::xserialize<ATTR>> ltemp(parms(#__VA_ARGS__), aele);				\
		return ltemp.push(__VA_ARGS__);																		\
	}


#if defined(WIN32)||defined(WINCE)||defined(WIN64)
#define def_xml(ATTR, ...)												\
	inline bool xml_pop(tinyxml2::XMLElement* aele)						\
	{																	\
		return ngl::xserialize<ATTR>::pop(aele, ##__VA_ARGS__);			\
	}																	\
	inline bool xml_push(tinyxml2::XMLElement* aele)const				\
	{																	\
		return ngl::xserialize<ATTR>::push(aele, ##__VA_ARGS__);		\
	}
#else
#define def_xml(ATTR, ...)																	\
	inline bool xml_pop(tinyxml2::XMLElement* aele)											\
	{																						\
		return ngl::xserialize<ATTR>::pop(aele __VA_OPT__(,) ##__VA_ARGS__);				\
	}																						\
	inline bool xml_push(tinyxml2::XMLElement* aele)const									\
	{																						\
		return ngl::xserialize<ATTR>::push(aele __VA_OPT__(,) ##__VA_ARGS__);				\
	}
#endif

#if defined(WIN32)||defined(WINCE)||defined(WIN64)
#define dxmlserialize(XMLNAME, ATTR, ...)				\
	def_parmname_(true)									\
	def_xmlfunction(XMLNAME)							\
	def_xmlspecial(ATTR, ##__VA_ARGS__)
#else
#define dxmlserialize(XMLNAME, ATTR, ...)				\
	def_parmname_(true)									\
	def_xmlfunction(XMLNAME)							\
	def_xmlspecial(ATTR __VA_OPT__(,) ##__VA_ARGS__)
#endif