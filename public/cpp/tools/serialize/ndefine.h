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
#include "lua.hpp"

#if defined(WIN32)||defined(WINCE)||defined(WIN64)
#define def_portocol_format(...)										\
	bool push_format(ngl::ser::serialize_push* aserialize)const			\
	{																	\
		return ngl::ser::nserialize::push(aserialize, ##__VA_ARGS__);	\
	}																	\
	bool pop_format(ngl::ser::serialize_pop* aserialize)				\
	{																	\
		return ngl::ser::nserialize::pop(aserialize, ##__VA_ARGS__);	\
	}																	\
	void bytes_format(ngl::ser::serialize_byte* aserialize)const		\
	{																	\
		ngl::ser::nserialize::bytes(aserialize, ##__VA_ARGS__);			\
	}
#else
#define def_portocol_format(...)													\
	bool push_format(ngl::ser::serialize_push* aserialize)const						\
	{																				\
		return ngl::ser::nserialize::push(aserialize __VA_OPT__(,) ##__VA_ARGS__);	\
	}																				\
	bool pop_format(ngl::ser::serialize_pop* aserialize)							\
	{																				\
		return ngl::ser::nserialize::pop(aserialize __VA_OPT__(,) ##__VA_ARGS__);	\
	}																				\
	void bytes_format(ngl::ser::serialize_byte* aserialize)const					\
	{																				\
		ngl::ser::nserialize::bytes(aserialize __VA_OPT__(,) ##__VA_ARGS__);		\
	}
#endif


///// 简化定义协议类
// --- 名称 ....(成员)
#define def_portocol_function(_Name,...)	def_portocol_format(__VA_ARGS__)
	

// --- 协议号  协议类型  名称 ....(成员)
#if defined(WIN32)||defined(WINCE)||defined(WIN64)
# define def_protocol(_Name,...)  def_portocol_function(_Name, ##__VA_ARGS__)
#else
# define def_protocol(_Name,...)  def_portocol_function(_Name __VA_OPT__(,) ##__VA_ARGS__)
#endif

#if defined(WIN32)||defined(WINCE)||defined(WIN64)
#define def_rcsv(...) return ngl::rcsv::readcsv(apair, ##__VA_ARGS__);
#else
#define def_rcsv(...) return ngl::rcsv::readcsv(apair __VA_OPT__(,) ##__VA_ARGS__);
#endif



#define def_parmname_(isDEL)													\
static std::vector<const char*>& parms(const char* astr = nullptr)				\
{																				\
	static std::vector<const char*> tempvec;									\
	if (astr == nullptr)														\
	{																			\
		return tempvec;															\
	}																			\
	static std::string tempstr(astr);											\
	static std::atomic lregister = true;										\
	if (lregister.exchange(false) && !tempstr.empty())							\
	{																			\
		tempvec = ngl::tools::split_str(&tempstr[0], (int32_t)tempstr.size());	\
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
	
#define def_parmname	def_parmname_(false)

#include "njson.h"

class help_json
{
	const std::vector<const char*>& m_parts;
	cJSON* m_json = nullptr;

	template <typename T>
	bool fun_pop(const char* astr, T& adata)
	{
		return ngl::njson::pop(m_json, astr, adata);
	}

	template <typename T>
	bool fun_push(const char* astr, T& adata)
	{
		ngl::njson::push(m_json, astr, adata);
		return true;
	}
public:
	help_json(const std::vector<const char*>& aparts, cJSON* ajson) :
		m_parts(aparts)
		, m_json(ajson)
	{}

	bool pop(int32_t apos)
	{
		return true;
	}

	template <typename T>
	bool pop(int32_t apos, T& adata)
	{
		return fun_pop(m_parts[apos], adata);
	}

	template <typename T, typename ...ARG>
	bool pop(int32_t apos, T& adata, ARG&... args)
	{
		if constexpr (sizeof...(ARG) >= 1)
		{
			return pop(apos, adata) && pop(++apos, args...);
		}
		else
		{
			return pop(apos, adata);
		}
	}

	bool push(int32_t apos)
	{
		return true;
	}

	template <typename T>
	bool push(int32_t apos, const T& adata)
	{
		return fun_push(m_parts[apos], adata);
	}

	template <typename T, typename ...ARG>
	bool push(int32_t apos, T& adata, ARG&... args)
	{
		if constexpr (sizeof...(ARG) >= 1)
		{
			return push(apos, adata) && push(++apos, args...);
		}
		else
		{
			return push(apos, adata);
		}
	}
};

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
#if defined(WIN32)||defined(WINCE)||defined(WIN64)
#define def_jsonfunction_special_parm(...)											\
bool json_pop(cJSON* ajson)															\
{																					\
	return ngl::njson::pop(ajson, ##__VA_ARGS__);									\
}																					\
void json_push(cJSON* ajson, const char* akey) const								\
{																					\
	if (akey != nullptr)															\
	{																				\
		ngl::ncjson ltemp;															\
		ngl::njson::push(ltemp.json(), ##__VA_ARGS__);								\
		ngl::njson::push(ajson, akey, ltemp);										\
	}																				\
	else																			\
	{																				\
		ngl::njson::push(ajson, ##__VA_ARGS__);										\
	}																				\
}																					\
def_jsonfunction_function	
#else
#define def_jsonfunction_special_parm(...)											\
bool json_pop(cJSON* ajson)															\
{																					\
	return ngl::njson::pop(ajson __VA_OPT__(,) ##__VA_ARGS__);						\
}																					\
void json_push(cJSON* ajson, const char* akey) const								\
{																					\
	if (akey != nullptr)															\
	{																				\
		ngl::ncjson ltemp;															\
		ngl::njson::push(ltemp.json() __VA_OPT__(,) ##__VA_ARGS__);					\
		ngl::njson::push(ajson, akey, ltemp);										\
	}																				\
	else																			\
	{																				\
		ngl::njson::push(ajson __VA_OPT__(,) ##__VA_ARGS__);						\
	}																				\
}																					\
def_jsonfunction_function	
#endif

	
#if defined(WIN32)||defined(WINCE)||defined(WIN64)
#define def_jsonfunction(...)														\
bool json_pop(cJSON* ajson)															\
{																					\
	help_json ltemp(parms(#__VA_ARGS__), ajson);									\
	return ltemp.pop(0, ##__VA_ARGS__);												\
}																					\
void json_push(cJSON* ajson, const char* akey) const								\
{																					\
	if (akey != nullptr)															\
	{																				\
		ngl::ncjson ltempjson;														\
		help_json ltemp(parms(#__VA_ARGS__), ltempjson.json());						\
		ltemp.push(0, ##__VA_ARGS__);												\
		ngl::njson::push(ajson, akey, ltempjson);									\
	}																				\
	else																			\
	{																				\
		help_json ltemp(parms(#__VA_ARGS__), ajson);								\
		ltemp.push(0, ##__VA_ARGS__);												\
	}																				\
}																					\
def_jsonfunction_function
#else
#define def_jsonfunction(...)														\
bool json_pop(cJSON* ajson)															\
{																					\
	help_json ltemp(parms(#__VA_ARGS__), ajson);									\
	return ltemp.pop(0 __VA_OPT__(,) ##__VA_ARGS__);								\
}																					\
void json_push(cJSON* ajson, const char* akey) const								\
{																					\
	if (akey != nullptr)															\
	{																				\
		ngl::ncjson ltempjson;														\
		help_json ltemp(parms(#__VA_ARGS__), ltempjson.json());						\
		ltemp.push(0 __VA_OPT__(,) ##__VA_ARGS__);									\
		ngl::njson::push(ajson, akey, ltempjson);									\
	}																				\
	else																			\
	{																				\
		help_json ltemp(parms(#__VA_ARGS__), ajson);								\
		ltemp.push(0 __VA_OPT__(,) ##__VA_ARGS__);									\
	}																				\
}																					\
def_jsonfunction_function
#endif

#define def_nlua_function(...)												\
	void nlua_push(lua_State* aL, const char* aname = nullptr)const			\
	{																		\
		help_nlua<false> ltemp(aL, aname, parms(#__VA_ARGS__));				\
		ltemp.push(__VA_ARGS__);											\
	}																		\
	bool nlua_pop(lua_State* aL, const char* aname = nullptr)				\
	{																		\
		help_nlua<true> ltemp(aL, aname, parms(#__VA_ARGS__));				\
		return ltemp.pop(__VA_ARGS__);										\
	}

#if defined(WIN32)||defined(WINCE)||defined(WIN64)
#define def_nlua_tab_function(...)														\
	void nlua_push(lua_State* aL, const char* aname = nullptr)const						\
	{																					\
		ngl::nlua_table::table_start_push(aL, aname);									\
		ngl::nlua_table::table_push(aL, ##__VA_ARGS__);									\
		ngl::nlua_table::table_finish_push(aL, aname);									\
	}																					\
	bool nlua_pop(lua_State* aL, const char* aname = nullptr)							\
	{																					\
		ngl::nlua_table::table_start_pop(aL, aname);									\
		bool lret = ngl::nlua_table::table_pop(aL, ##__VA_ARGS__);						\
		ngl::nlua_table::table_finish_pop(aL, aname);									\
		return lret;																	\
	}
#else
#define def_nlua_tab_function(...)														\
	void nlua_push(lua_State* aL, const char* aname = nullptr)const						\
	{																					\
		ngl::nlua_table::table_start_push(aL, aname);									\
		ngl::nlua_table::table_push(aL __VA_OPT__(,) ##__VA_ARGS__);					\
		ngl::nlua_table::table_finish_push(aL, aname);									\
	}																					\
	bool nlua_pop(lua_State* aL, const char* aname = nullptr)							\
	{																					\
		ngl::nlua_table::table_start_pop(aL, aname);									\
		bool lret = ngl::nlua_table::table_pop(aL __VA_OPT__(,) ##__VA_ARGS__);			\
		ngl::nlua_table::table_finish_pop(aL, aname);									\
		return lret;																	\
	}
#endif


#if defined(WIN32)||defined(WINCE)||defined(WIN64)
#define dprotocol(NAME, ...)								\
	def_parmname											\
	def_jsonfunction(__VA_ARGS__)							\
	def_protocol(NAME, ##__VA_ARGS__)						\
	def_nlua_function(__VA_ARGS__)
#else
#define dprotocol(NAME, ...)								\
	def_parmname											\
	def_jsonfunction(__VA_ARGS__)							\
	def_protocol(NAME __VA_OPT__(,) ##__VA_ARGS__)			\
	def_nlua_function(__VA_ARGS__)
#endif

namespace ngl
{
	void test_json();
}