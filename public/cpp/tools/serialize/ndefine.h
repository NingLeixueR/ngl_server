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

#if defined(WIN32)||defined(WINCE)||defined(WIN64)
#define SAFE_MACRO(...) ,##__VA_ARGS__
#else
#define SAFE_MACRO(...) __VA_OPT__(,)__VA_ARGS__
#endif



#define def_portocol_format(...)												\
	bool push_format(ngl::ser::serialize_push* aser)const						\
	{																			\
		return ngl::ser::nserialize::push(aser SAFE_MACRO(__VA_ARGS__));		\
	}																			\
	bool pop_format(ngl::ser::serialize_pop* aser)								\
	{																			\
		return ngl::ser::nserialize::pop(aser SAFE_MACRO(__VA_ARGS__));			\
	}																			\
	void bytes_format(ngl::ser::serialize_byte* aser)const						\
	{																			\
		ngl::ser::nserialize::bytes(aser SAFE_MACRO(__VA_ARGS__));				\
	}

// 简化定义协议类
// --- 名称 ....(成员)
#define def_portocol_function(_Name,...)	def_portocol_format(__VA_ARGS__)
	
// --- 协议号  协议类型  名称 ....(成员)
# define def_protocol(_Name,...)  def_portocol_function(_Name SAFE_MACRO(__VA_ARGS__))

#define def_rcsv(...) return ngl::rcsv::readcsv(apair SAFE_MACRO(__VA_ARGS__));

#define NUMARGS(...)  std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value

#define def_parmname_(isDEL, ...)												\
static std::array<const char*, NUMARGS(__VA_ARGS__)>& parms()					\
{																				\
	static std::array<const char*, NUMARGS(__VA_ARGS__)> tempvec;				\
	static std::string tempstr(#__VA_ARGS__);									\
	static std::atomic lregister = true;										\
	if (lregister.exchange(false) && !tempstr.empty())							\
	{																			\
		ngl::tools::split_str(&tempstr[0], (int32_t)tempstr.size(), tempvec);	\
		if constexpr (isDEL)													\
		{																		\
			for (const char*& item : tempvec)									\
			{																	\
				if (memcmp("m_", item, 2) == 0)									\
				{																\
					item = &(item[2]);											\
				}																\
			}																	\
		}																		\
	}																			\
	return tempvec;																\
}
	
#define def_parmname(...)	def_parmname_(false, __VA_ARGS__)

#define def_jsonfunction_function													\
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
#define def_jsonfunction_special_parm(...)											\
bool json_pop(cJSON* ajson)															\
{																					\
	return ngl::njson::pop(ajson SAFE_MACRO(__VA_ARGS__));							\
}																					\
void json_push(cJSON* ajson, const char* akey) const								\
{																					\
	if (akey != nullptr)															\
	{																				\
		ngl::ncjson ltemp;															\
		ngl::njson::push(ltemp.json() SAFE_MACRO(__VA_ARGS__));						\
		ngl::njson::push(ajson, {akey}, ltemp);										\
	}																				\
	else																			\
	{																				\
		ngl::njson::push(ajson  SAFE_MACRO(__VA_ARGS__));							\
	}																				\
}																					\
def_jsonfunction_function

#define def_jsonfunction(...)														\
bool json_pop(cJSON* ajson)															\
{																					\
	return ngl::njson::pop(ajson, parms()  SAFE_MACRO(__VA_ARGS__));				\
}																					\
void json_push(cJSON* ajson, const char* akey) const								\
{																					\
	if (akey != nullptr)															\
	{																				\
		ngl::ncjson ltempjson;														\
		ngl::njson::push(ltempjson.json(), parms() SAFE_MACRO(__VA_ARGS__));		\
		ngl::njson::push(ajson, {akey}, ltempjson);									\
	}																				\
	else																			\
	{																				\
		ngl::njson::push(ajson, parms()  SAFE_MACRO(__VA_ARGS__));					\
	}																				\
}																					\
def_jsonfunction_function


#define def_nlua_function(...)															\
	void nlua_push(lua_State* aL, const char* aname = nullptr)const						\
	{																					\
		ngl::nlua_table::table_push(aL, aname, parms() SAFE_MACRO(__VA_ARGS__));		\
	}																					\
	bool nlua_pop(lua_State* aL, const char* aname = nullptr)							\
	{																					\
		return ngl::nlua_table::table_pop(aL, aname, parms() SAFE_MACRO(__VA_ARGS__));	\
	}

#define def_nlua_special_function(KEYS,...)												\
	void nlua_push(lua_State* aL, const char* aname = nullptr)const						\
	{																					\
		ngl::nlua_table::table_push(aL, aname, KEYS SAFE_MACRO(__VA_ARGS__));			\
	}																					\
	bool nlua_pop(lua_State* aL, const char* aname = nullptr)							\
	{																					\
		return ngl::nlua_table::table_pop(aL, aname, KEYS SAFE_MACRO(__VA_ARGS__));		\
	}

#define dprotocol(NAME, ...)								\
	def_parmname(__VA_ARGS__)								\
	def_jsonfunction(__VA_ARGS__)							\
	def_protocol(NAME SAFE_MACRO(__VA_ARGS__))				\
	def_nlua_function(__VA_ARGS__)

namespace ngl
{
	void test_json();
}