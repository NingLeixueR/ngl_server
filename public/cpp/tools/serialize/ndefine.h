#pragma once

#include "serialize.h"

#define def_function_pop(...)									\
	bool pop(ngl::unserialize& ser)								\
	{															\
		return ser.pop(__VA_ARGS__);							\
	}															\
	bool ParseFromArray(const void* data, int32_t size)			\
	{															\
		ngl::unserialize lunserialize((const char*)data, size);	\
		return pop(lunserialize);								\
	}


#define def_function_push(...)									\
	bool push(ngl::serialize& ser)const							\
	{															\
		return ser.push(__VA_ARGS__);							\
	}															\
	bool SerializeToArray(void* data, int32_t size) const		\
	{															\
		ngl::serialize lserialize((char*)data, size);			\
		return push(lserialize);								\
	}


#define def_portocol_bytes(...)								\
	int bytes(ngl::serialize_bytes& abytes)const			\
	{														\
		return abytes.bytes(__VA_ARGS__);					\
	}														\
	int ByteSize()const										\
	{														\
		ngl::serialize_bytes lserialize_bytes;				\
		return bytes(lserialize_bytes);						\
	}

#define def_portocol_name(_Name)							\
	static const char* name()								\
	{														\
		return #_Name;										\
	}

///// 简化定义协议类
// --- 名称 ....(成员)
#define def_portocol_function(_Name,...)					\
	def_function_pop	(__VA_ARGS__)						\
	def_function_push	(__VA_ARGS__)						\
	def_portocol_bytes	(__VA_ARGS__)						\
	def_portocol_name	(_Name)								

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

#include "json_write.h"
#include "json_read.h"


enum
{
	ESPLIT_STR = 32,
};

class help_writejson
{
	const std::vector<const char*>& m_parts;
	ngl::json_write& m_json;

	template <typename T>
	bool dosth(ngl::json_write& ijson, const char* astr, const T& adata)
	{
		ijson.write(astr, adata);
		return true;
	}
public:
	help_writejson(const std::vector<const char*>& aparts, ngl::json_write& aijson) :
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
		, const T& adata
	)
	{
		dosth(m_json, m_parts[apos], adata);
		return true;
	}

	template <typename T, typename ...ARG>
	bool fun(
		int32_t apos
		, const T& adata
		, const ARG&... args
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
	const ngl::json_read& m_json;

	template <typename T>
	bool dosth(const ngl::json_read& ojson, const char* astr, T& adata)
	{
		return ojson.read(astr, adata);
	}
public:
	help_readjson(const std::vector<const char*>& aparts, const ngl::json_read& aijson) :
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
		inline bool read(const ngl::json_read& ijsn, const char* akey)	\
		{																\
			ngl::json_read ltemp;										\
			if (ijsn.read(akey, ltemp) == false)						\
			{															\
				return false;											\
			}															\
			return read(ltemp);											\
		}																\
		inline bool read(const ngl::json_read& ijsn) 					\
		{																\
			return ijsn.read(__VA_ARGS__);								\
		}


#define def_jsonfunction_write_parm(...)								\
		inline void write(ngl::json_write& ijsn, const char* akey)const	\
		{																\
			ngl::json_write ltemp;										\
			write(ltemp);												\
			ijsn.write(akey, ltemp.nofree());							\
		}																\
		inline void write(ngl::json_write& ijsn)const					\
		{																\
			ijsn.write(__VA_ARGS__);									\
		}


// 特殊情况使用
#define def_jsonfunction_parm(...)									\
	def_jsonfunction_read_parm(__VA_ARGS__)							\
	def_jsonfunction_write_parm(__VA_ARGS__)


#if defined(WIN32)||defined(WINCE)||defined(WIN64)
#define def_jsonfunction(...)										\
	inline void write(ngl::json_write& ijsn, const char* akey)const	\
	{																\
		ngl::json_write ltemp;										\
		write(ltemp);												\
		ijsn.write(akey, ltemp.nofree());							\
	}																\
	inline std::vector<const char*>& jsonparmvec(const char* astr = nullptr)const	\
	{																\
		static std::vector<const char*> tempvec;					\
		if(astr == nullptr)											\
		{															\
			return tempvec;											\
		}															\
		static std::string tempstr(astr);							\
		static std::atomic<bool> lregister = true;					\
		if (lregister.exchange(false) && !tempstr.empty())			\
		{															\
			ngl::tools::replace(" ", "", tempstr, tempstr);			\
			int32_t lpos = 0;										\
			for (int32_t i = 0; i < tempstr.size(); ++i)			\
			{														\
				if (tempstr[i] == ',')								\
				{													\
					tempstr[i] = '\0';								\
					tempvec.push_back(&tempstr.c_str()[lpos]);		\
					lpos = i + 1;									\
				}													\
			}														\
			tempvec.push_back(&tempstr.c_str()[lpos]);				\
		}															\
		return tempvec;												\
	}																\
	inline void write(ngl::json_write& ijsn)const					\
	{																\
		help_writejson ltemp(jsonparmvec(#__VA_ARGS__), ijsn);		\
		ltemp.fun(0, __VA_ARGS__);									\
	}																\
	inline bool read(const ngl::json_read& ijsn, const char* akey)	\
	{																\
		ngl::json_read ltemp;										\
		if (ijsn.read(akey, ltemp) == false)						\
		{															\
			return false;											\
		}															\
		return read(ltemp);											\
	}																\
	inline bool read(const ngl::json_read& ijsn) 					\
	{																\
		help_readjson ltemp(jsonparmvec(#__VA_ARGS__), ijsn);		\
		return ltemp.fun(0, __VA_ARGS__);							\
	}
#else
#define def_jsonfunction(...)										\
	inline void write(ngl::json_write& ijsn, const char* akey)const	\
	{																\
		ngl::json_write ltemp;										\
		write(ltemp);												\
		ijsn.write(akey, ltemp.nofree());							\
	}																\
	inline std::vector<const char*>& jsonparmvec(const char* astr = nullptr)const	\
	{																\
		static std::vector<const char*> tempvec;					\
		if(astr == nullptr)											\
		{															\
			return tempvec;											\
		}															\
		static std::string tempstr(astr);							\
		static std::atomic<bool> lregister = true;					\
		if (lregister.exchange(false) && !tempstr.empty())			\
		{															\
			int32_t lpos = 0;										\
			for (int32_t i = 0; i < tempstr.size(); ++i)			\
			{														\
				if (tempstr[i] == ',')								\
				{													\
					tempstr[i] = '\0';								\
					tempvec.push_back(&tempstr.c_str()[lpos]);		\
					lpos = i + 1;									\
				}													\
			}														\
			tempvec.push_back(&tempstr.c_str()[lpos]);				\
		}															\
		return tempvec;												\
	}																\
	inline void write(ngl::json_write& ijsn)const					\
	{																\
		help_writejson ltemp(jsonparmvec(#__VA_ARGS__), ijsn);		\
		ltemp.fun(0 __VA_OPT__(,) ##__VA_ARGS__);					\
	}																\
	inline bool read(const ngl::json_read& ijsn, const char* akey)	\
	{																\
		ngl::json_read ltemp;										\
		if (ijsn.read(akey, ltemp) == false)						\
		{															\
			return false;											\
		}															\
		return read(ltemp);											\
	}																\
	inline bool read(const ngl::json_read& ijsn) 					\
	{																\
		help_readjson ltemp(jsonparmvec(#__VA_ARGS__), ijsn);		\
		return ltemp.fun(0 __VA_OPT__(,) ##__VA_ARGS__);			\
	}
#endif

#define dprotocoljson(NAME, ...)			\
	def_jsonfunction(__VA_ARGS__)			\
	def_protocol(NAME, __VA_ARGS__)