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

#include "serialize_lua.h"
#include "nserialize.h"
#include "njson.h"
#include "lua.hpp"

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

#define DEF_PARMNAME_(ISMG,...)																		\
static std::array<const char*, NUMARGS(__VA_ARGS__)>& parms()										\
{																									\
	static std::array<const char*, NUMARGS(__VA_ARGS__)> tempvec;									\
	static std::string tempstr(#__VA_ARGS__);														\
	static std::atomic lregister = true;															\
	if (lregister.exchange(false) && !tempstr.empty())												\
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
	}																								\
	return tempvec;																					\
}

#define DEF_PARMNAME(...)	DEF_PARMNAME_(false __VA_OPT__(, )__VA_ARGS__)

#define DEF_JSON_FUNCTION															\
bool json_pop(const char* ajson, const char* akey)									\
{																					\
	ngl::ncjson ltemp(ajson);														\
	if (akey != nullptr)															\
	{																				\
		cJSON* ret = cJSON_GetObjectItem(ltemp.json(), akey);						\
		if (nullptr == ret)															\
		{																			\
			return false;															\
		}																			\
		return json_pop(ret);														\
	}																				\
	return json_pop(ltemp.json());													\
}																					\
void json_push(std::string& ajson, const char* akey) const							\
{																					\
	ngl::ncjson ltemp;																\
	json_push(ltemp.json(), akey);													\
	ajson = ltemp.str();															\
}

// 特殊情况使用
#define DEF_JSONFUNCTION_SPECIAL(...)											\
bool json_pop(cJSON* ajson)															\
{																					\
	return ngl::njson::pop(ajson __VA_OPT__(, )__VA_ARGS__);						\
}																					\
void json_push(cJSON* ajson, const char* akey) const								\
{																					\
	if (akey != nullptr)															\
	{																				\
		ngl::ncjson ltemp;															\
		ngl::njson::push(ltemp.json() __VA_OPT__(, )__VA_ARGS__);					\
		ngl::njson::push(ajson, {akey}, ltemp);										\
	}																				\
	else																			\
	{																				\
		ngl::njson::push(ajson __VA_OPT__(, )__VA_ARGS__);							\
	}																				\
}																					\
DEF_JSON_FUNCTION

#define DEF_JSONFUNCTION(...)														\
bool json_pop(cJSON* ajson)															\
{																					\
	return ngl::njson::pop(ajson, parms() __VA_OPT__(, )__VA_ARGS__);				\
}																					\
void json_push(cJSON* ajson, const char* akey) const								\
{																					\
	if (akey != nullptr)															\
	{																				\
		ngl::ncjson ltempjson;														\
		ngl::njson::push(ltempjson.json(), parms() __VA_OPT__(, )__VA_ARGS__);		\
		ngl::njson::push(ajson, {akey}, ltempjson);									\
	}																				\
	else																			\
	{																				\
		ngl::njson::push(ajson, parms() __VA_OPT__(, )__VA_ARGS__);					\
	}																				\
}																					\
DEF_JSON_FUNCTION

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