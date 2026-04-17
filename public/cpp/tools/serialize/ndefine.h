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
// File overview: Serialization macros, type aliases, and protocol ID definitions.

#pragma once

#include "tools/script/lua/serialize_lua.h"
#include "tools/serialize/nserialize.h"
#include "tools/tab/json/njson.h"
#include "lua.hpp"

#include <array>
#include <atomic>
#include <string>
#include <tuple>

#define DEF_PROTOCOL(...)														\
	bool push_format(ngl::ser::serialize_push* aser)const						\
	{																			\
		return ngl::ser::nserialize::push(aser __VA_OPT__(, )__VA_ARGS__);		\
	}																			\
	bool pop_format(ngl::ser::serialize_pop* aser)								\
	{																			\
		return ngl::ser::nserialize::pop(aser __VA_OPT__(, )__VA_ARGS__);		\
	}																			\
	void bytes_format(ngl::ser::serialize_byte* aser)const						\
	{																			\
		ngl::ser::nserialize::bytes(aser __VA_OPT__(, )__VA_ARGS__);			\
	}

#define DEF_RCSV(...) return ngl::rcsv::readcsv(apair __VA_OPT__(, )__VA_ARGS__);

#define NUMARGS(...)  std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value

#define DEF_PARMNAME_(ISMG,...)																			\
static std::array<const char*, NUMARGS(__VA_ARGS__)>& parms()											\
{																										\
	static std::array<const char*, NUMARGS(__VA_ARGS__)> tempvec{};										\
	static std::string tempstr(#__VA_ARGS__);															\
	if(!tempstr.empty())																				\
	{																									\
		static std::once_flag lfirst;																	\
		std::call_once(lfirst, [&]()																	\
		{																								\
			ngl::tools::split_str<NUMARGS(__VA_ARGS__)>(&tempstr[0], (int32_t)tempstr.size(), tempvec);	\
			if constexpr(ISMG)																			\
			{																							\
				for (const char*& item : tempvec)														\
				{																						\
					if (memcmp("m_", item, 2) == 0)														\
					{																					\
						item = &(item[2]);																\
					}																					\
				}																						\
			}																							\
		});																								\
	}																									\
	return tempvec;																						\
}

#define DEF_PARMNAME(...)	DEF_PARMNAME_(false __VA_OPT__(, )__VA_ARGS__)


// Special
#define DEF_JSONFUNCTION_SPECIAL(...)															\
bool json_pop(rapidjson::Value& ajson)															\
{																								\
	return ngl::njson::pop(ajson __VA_OPT__(, )__VA_ARGS__);									\
}																								\
bool json_push(rapidjson::Value& ajson, rapidjson::Document::AllocatorType* aallocator) const	\
{																								\
	return ngl::njson::push(ajson, aallocator __VA_OPT__(, )__VA_ARGS__);						\
}

#define DEF_JSONFUNCTION(...)																	\
bool json_pop(rapidjson::Value& ajson)															\
{																								\
	return ngl::njson::pop(ajson, parms() __VA_OPT__(, )__VA_ARGS__);							\
}																								\
bool json_push(rapidjson::Value& ajson, rapidjson::Document::AllocatorType* aallocator) const	\
{																								\
	return ngl::njson::push(ajson, aallocator, parms() __VA_OPT__(, )__VA_ARGS__);				\
}

#define DEF_NLUA_FUNCTION(...)															\
	void nlua_push(lua_State* aL, const char* aname = nullptr)const						\
	{																					\
		ngl::nlua_table::table_push(aL, aname, parms() __VA_OPT__(, )__VA_ARGS__);		\
	}																					\
	bool nlua_pop(lua_State* aL, const char* aname = nullptr)							\
	{																					\
		return ngl::nlua_table::table_pop(aL, aname, parms() __VA_OPT__(, )__VA_ARGS__);\
	}

#define DEF_NLUA_SPECIAL_FUNCTION(KEYS,...)												\
	void nlua_push(lua_State* aL, const char* aname = nullptr)const						\
	{																					\
		ngl::nlua_table::table_push(aL, aname, KEYS __VA_OPT__(, )__VA_ARGS__);			\
	}																					\
	bool nlua_pop(lua_State* aL, const char* aname = nullptr)							\
	{																					\
		return ngl::nlua_table::table_pop(aL, aname, KEYS __VA_OPT__(, )__VA_ARGS__);	\
	}

#define DPROTOCOL(NAME, ...)								\
	DEF_PARMNAME(__VA_ARGS__)								\
	DEF_JSONFUNCTION(__VA_ARGS__)							\
	DEF_PROTOCOL(__VA_ARGS__)								\
	DEF_NLUA_FUNCTION(__VA_ARGS__)
