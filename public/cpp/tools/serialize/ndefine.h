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
#define def_rcsv(...)													\
	inline bool rcsv(ngl::csvpair& apair)								\
	{																	\
		return ngl::rcsv::readcsv(apair, ##__VA_ARGS__);				\
	}
#define def_rcsv2(...) return ngl::rcsv::readcsv(apair, ##__VA_ARGS__);
#else
#define def_rcsv(...)													\
	inline bool rcsv(ngl::csvpair& apair)								\
	{																	\
		return ngl::rcsv::readcsv(apair __VA_OPT__(,) ##__VA_ARGS__);	\
	}
#define def_rcsv2(...) return ngl::rcsv::readcsv(apair __VA_OPT__(,) ##__VA_ARGS__);
#endif

#define def_parmname														\
	static std::vector<const char*>& parms(const char* astr = nullptr)		\
	{																		\
		static std::vector<const char*> tempvec;							\
		if(astr == nullptr)													\
		{																	\
			return tempvec;													\
		}																	\
		static std::string tempstr(astr);									\
		static std::atomic<bool> lregister = true;							\
		if (lregister.exchange(false) && !tempstr.empty())					\
		{																	\
			tempvec = tools::split_str(&tempstr[0], tempstr.size());		\
		}																	\
		return tempvec;														\
	}

#include "njson.h"

enum
{
	ESPLIT_STR = 32,
};

class help_writejson
{
	const std::vector<const char*>& m_parts;
	ngl::njson_write& m_json;

	template <typename T>
	bool dosth(ngl::njson_write& ijson, const char* astr, T& adata)
	{
		ngl::njson::write(ijson, astr, adata);
		return true;
	}
public:
	help_writejson(const std::vector<const char*>& aparts, ngl::njson_write& aijson) :
		m_parts(aparts),
		m_json(aijson)
	{}

	bool fun(int32_t apos)
	{
		return true;
	}

	template <typename T>
	bool fun(
		int32_t apos
		, T& adata
	)
	{
		dosth(m_json, m_parts[apos], adata);
		return true;
	}

	template <typename T, typename ...ARG>
	bool fun(
		int32_t apos
		, T& adata
		, ARG&... args
	)
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

class help_readjson
{
	const std::vector<const char*>& m_parts;
	ngl::njson_read& m_json;

	template <typename T>
	bool dosth(ngl::njson_read& ojson, const char* astr, T& adata)
	{
		return ngl::njson::read(ojson, astr, adata);
	}
public:
	help_readjson(const std::vector<const char*>& aparts, ngl::njson_read& aijson) :
		m_parts(aparts),
		m_json(aijson)
	{}

	bool fun(int32_t apos)
	{
		return true;
	}

	template <typename T>
	bool fun(int32_t apos, T& adata)
	{
		dosth(m_json, m_parts[apos], adata);
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
		help_writejson ltemp(parms(#__VA_ARGS__), ijsn);				\
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
		help_readjson ltemp(parms(#__VA_ARGS__), ijsn);					\
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
		help_writejson ltemp(parms(#__VA_ARGS__), ijsn);				\
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
		help_readjson ltemp(parms(#__VA_ARGS__), ijsn);					\
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

#define def_nlua_function2(...)			\
	def_nlua_push2(__VA_ARGS__)			\
	def_nlua_pop2(__VA_ARGS__)


#define dprotocol(NAME, ...)				\
	def_parmname							\
	def_jsonfunction(__VA_ARGS__)			\
	def_protocol(NAME, __VA_ARGS__)			\
	def_nlua_function(__VA_ARGS__)