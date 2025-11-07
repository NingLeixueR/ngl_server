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

#define def_parmname																\
	static std::vector<const char*>& parms(const char* astr = nullptr)				\
	{																				\
		static std::vector<const char*> tempvec;									\
		if(astr == nullptr)															\
		{																			\
			return tempvec;															\
		}																			\
		static std::string tempstr(astr);											\
		static std::atomic<bool> lregister = true;									\
		if (lregister.exchange(false) && !tempstr.empty())							\
		{																			\
			tempvec = tools::split_str(&tempstr[0], (int32_t)tempstr.size());		\
		}																			\
		return tempvec;																\
	}

#include "njson.h"

enum
{
	ESPLIT_STR = 32,
};

template <bool READ>
class help_json
{
	const std::vector<const char*>& m_parts;
	void* m_json = nullptr;

	template <typename T>
	bool dosth(const char* astr, T& adata)
	{
		if constexpr (READ)
		{
			return ngl::njson::read(*(ngl::njson_read*)m_json, astr, adata);
		}
		else
		{
			ngl::njson::write(*(ngl::njson_write*)m_json, astr, adata);
			return true;
		}
	}
public:
	help_json(const std::vector<const char*>& aparts) :
		m_parts(aparts)
	{}

	void set_json(ngl::njson_write& awjson)
	{
		assert(!READ);
		m_json = &awjson;
	}

	void set_json(ngl::njson_read& arjson)
	{
		assert(READ);
		m_json = &arjson;
	}

	bool fun(int32_t apos)
	{
		return true;
	}

	template <typename T>
	bool fun(int32_t apos, T& adata)
	{
		dosth(m_parts[apos], adata);
		return true;
	}

	template <typename T, typename ...ARG>
	bool fun(int32_t apos, T& adata, ARG&... args)
	{
		if constexpr (sizeof...(ARG) >= 1)
		{
			return fun(apos, adata) && fun(++apos, args...);
		}
		else
		{
			return fun(apos, adata);
		}
	}
};


#define def_jsonfunction_read_parm(...)									\
		inline bool json_read(ngl::njson_read& ijsn, const char* akey)	\
		{																\
			ngl::njson_read ltemp;										\
			if (!ngl::njson::read(ijsn, akey, ltemp.json()))			\
			{															\
				return false;											\
			}															\
			return json_read(ltemp);									\
		}																\
		inline bool json_read(ngl::njson_read& ijsn) 					\
		{																\
			return ngl::njson::read(__VA_ARGS__);						\
		}


#define def_jsonfunction_write_parm(...)										\
		inline void json_write(ngl::njson_write& ijsn, const char* akey)const	\
		{																		\
			ngl::njson_write ltemp;												\
			json_write(ltemp);													\
			ngl::njson::write(ijsn, akey, ltemp.nofree());						\
		}																		\
		inline void json_write(ngl::njson_write& ijsn)const						\
		{																		\
			ngl::njson::write(ijsn, __VA_ARGS__);								\
		}


// 特殊情况使用
#define def_jsonfunction_parm(...)									\
	def_jsonfunction_read_parm(__VA_ARGS__)							\
	def_jsonfunction_write_parm(__VA_ARGS__)


#if defined(WIN32)||defined(WINCE)||defined(WIN64)
#define def_jsonfunction(...)											\
	inline void json_write(ngl::njson_write& ijsn, const char* akey)	\
	{																	\
		ngl::njson_write ltemp;											\
		json_write(ltemp);												\
		ngl::njson::write(ijsn, akey, ltemp.nofree());					\
	}																	\
	inline void json_write(ngl::njson_write& ijsn)						\
	{																	\
		help_json<false> ltemp(parms(#__VA_ARGS__));					\
		ltemp.set_json(ijsn);											\
		ltemp.fun(0, __VA_ARGS__);										\
	}																	\
	inline bool json_read(ngl::njson_read& ijsn, const char* akey)		\
	{																	\
		ngl::njson_read ltemp;											\
		if (!ngl::njson::read(ijsn, akey, ltemp.json()))				\
		{																\
			return false;												\
		}																\
		return json_read(ltemp);										\
	}																	\
	inline bool json_read(ngl::njson_read& ijsn) 						\
	{																	\
		help_json<true> ltemp(parms(#__VA_ARGS__));						\
		ltemp.set_json(ijsn);											\
		return ltemp.fun(0, __VA_ARGS__);								\
	}
#else
#define def_jsonfunction(...)											\
inline void json_write(ngl::njson_write& ijsn, const char* akey)		\
	{																	\
		ngl::njson_write ltemp;											\
		json_write(ltemp);												\
		ngl::njson::write(ijsn, akey, ltemp.nofree());					\
	}																	\
	inline void json_write(ngl::njson_write& ijsn)						\
	{																	\
		help_json<false> ltemp(parms(#__VA_ARGS__));					\
		ltemp.set_json(ijsn);											\
		ltemp.fun(0 __VA_OPT__(,) ##__VA_ARGS__);						\
	}																	\
	inline bool json_read(ngl::njson_read& ijsn, const char* akey)		\
	{																	\
		ngl::njson_read ltemp;											\
		if (!ngl::njson::read(ijsn, akey, ltemp.json()))				\
		{																\
			return false;												\
		}																\
		return json_read(ltemp);										\
	}																	\
	inline bool json_read(ngl::njson_read& ijsn) 						\
	{																	\
		help_json<true> ltemp(parms(#__VA_ARGS__));						\
		ltemp.set_json(ijsn);											\
		return ltemp.fun(0 __VA_OPT__(,) ##__VA_ARGS__);				\
	}
#endif

#define def_nlua_push(...)													\
	void nlua_push(lua_State* aL, const char* aname = nullptr)const			\
	{																		\
		help_nlua<false> ltemp(aL, aname, parms(#__VA_ARGS__));				\
		ltemp.push(__VA_ARGS__);											\
	}

#define def_nlua_pop(...)													\
	bool nlua_pop(lua_State* aL, const char* aname = nullptr)				\
	{																		\
		help_nlua<true> ltemp(aL, aname, parms(#__VA_ARGS__));				\
		return ltemp.pop(__VA_ARGS__);										\
	}


#define def_nlua_function(...)			\
	def_nlua_push(__VA_ARGS__)			\
	def_nlua_pop(__VA_ARGS__)

#define def_nlua_push2(...)													\
	void nlua_push(lua_State* aL, const char* aname = nullptr)const			\
	{																		\
		ngl::nlua_table::table_start_push(aL, aname);						\
		ngl::nlua_table::table_push(aL, __VA_ARGS__);						\
		ngl::nlua_table::table_finish_push(aL, aname);						\
	}

#define def_nlua_pop2(...)													\
	bool nlua_pop(lua_State* aL, const char* aname = nullptr)				\
	{																		\
		ngl::nlua_table::table_start_pop(aL, aname);						\
		bool lret = ngl::nlua_table::table_pop(aL, __VA_ARGS__);			\
		ngl::nlua_table::table_finish_pop(aL, aname);						\
		return lret;														\
	}

#define def_nlua_function2(...)				\
	def_nlua_push2(__VA_ARGS__)				\
	def_nlua_pop2(__VA_ARGS__)


#define dprotocol(NAME, ...)				\
	def_parmname							\
	def_jsonfunction(__VA_ARGS__)			\
	def_protocol(NAME, __VA_ARGS__)			\
	def_nlua_function(__VA_ARGS__)


#define ARGC(...) ARGC_IMPL(__VA_ARGS__, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define ARGC_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, N, ...) N

#define CONCAT(prefix, num) CONCAT_IMPL(prefix, num)
#define CONCAT_IMPL(prefix, num) prefix##num

#define MOVE_CTOR_INIT_0()
#define MOVE_CTOR_INIT_1(m1) m1(std::move(other.m1))
#define MOVE_CTOR_INIT_2(m2, m3) m2(std::move(other.m2)), MOVE_CTOR_INIT_1(m3)

#define MOVE_CTOR_INIT_3(m3, ...) m3(std::move(other.m3)), MOVE_CTOR_INIT_2(__VA_ARGS__)
#define MOVE_CTOR_INIT_4(m4, ...) m4(std::move(other.m4)), MOVE_CTOR_INIT_3(__VA_ARGS__)
#define MOVE_CTOR_INIT_5(m5, ...) m5(std::move(other.m5)), MOVE_CTOR_INIT_4(__VA_ARGS__)
#define MOVE_CTOR_INIT_6(m6, ...) m6(std::move(other.m6)), MOVE_CTOR_INIT_5(__VA_ARGS__)
#define MOVE_CTOR_INIT_7(m7, ...) m7(std::move(other.m7)), MOVE_CTOR_INIT_6(__VA_ARGS__)
#define MOVE_CTOR_INIT_8(m8, ...) m8(std::move(other.m8)), MOVE_CTOR_INIT_7(__VA_ARGS__)
#define MOVE_CTOR_INIT_9(m9, ...) m9(std::move(other.m9)), MOVE_CTOR_INIT_8(__VA_ARGS__)
#define MOVE_CTOR_INIT_10(m10, ...) m10(std::move(other.m10)), MOVE_CTOR_INIT_9(__VA_ARGS__)
#define MOVE_CTOR_INIT_11(m11, ...) m11(std::move(other.m11)), MOVE_CTOR_INIT_10(__VA_ARGS__)
#define MOVE_CTOR_INIT_12(m12, ...) m12(std::move(other.m12)), MOVE_CTOR_INIT_11(__VA_ARGS__)
#define MOVE_CTOR_INIT_13(m13, ...) m13(std::move(other.m13)), MOVE_CTOR_INIT_12(__VA_ARGS__)
#define MOVE_CTOR_INIT_14(m14, ...) MOVE_CTOR_INIT_1(m14), MOVE_CTOR_INIT_13(__VA_ARGS__)
#define MOVE_CTOR_INIT_15(m15, ...) MOVE_CTOR_INIT_1(m15), MOVE_CTOR_INIT_14(__VA_ARGS__)
#define MOVE_CTOR_INIT_16(m16, ...) MOVE_CTOR_INIT_1(m16), MOVE_CTOR_INIT_15(__VA_ARGS__)
#define MOVE_CTOR_INIT_17(m17, ...) MOVE_CTOR_INIT_1(m17), MOVE_CTOR_INIT_16(__VA_ARGS__)
#define MOVE_CTOR_INIT_18(m18, ...) MOVE_CTOR_INIT_1(m18), MOVE_CTOR_INIT_17(__VA_ARGS__)
#define MOVE_CTOR_INIT_19(m19, ...) MOVE_CTOR_INIT_1(m19), MOVE_CTOR_INIT_18(__VA_ARGS__)
#define MOVE_CTOR_INIT_20(m20, ...) MOVE_CTOR_INIT_1(m20), MOVE_CTOR_INIT_19(__VA_ARGS__)

#define DEFINE_MOVE_CONSTRUCTOR(NAME, ...) \
    NAME(NAME&& other) noexcept \
        : CONCAT(MOVE_CTOR_INIT_, ARGC(__VA_ARGS__))(__VA_ARGS__) \
    {} \
    \
	NAME() = default;												\
	NAME(const NAME&) = default;									\
	NAME& operator=(const NAME&) = default;							\
	NAME& operator=(NAME&& other) = default;