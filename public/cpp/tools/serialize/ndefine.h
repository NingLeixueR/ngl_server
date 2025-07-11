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
	const std::array<std::string_view, ESPLIT_STR>& m_parts;
	ngl::json_write& m_json;

	template <typename T>
	bool dosth(ngl::json_write& ijson, const std::string_view& astr, const T& adata)
	{
		ijson.write(astr.data(), adata);
		return true;
	}
public:
	help_writejson(const std::array<std::string_view, ESPLIT_STR>& aparts, ngl::json_write& aijson) :
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
	const std::array<std::string_view, ESPLIT_STR>& m_parts;
	const ngl::json_read& m_json;

	template <typename T>
	bool dosth(const ngl::json_read& ojson, const std::string_view& astr, T& adata)
	{
		return ojson.read(astr.data(), adata);
	}
public:
	help_readjson(const std::array<std::string_view, ESPLIT_STR>& aparts, const ngl::json_read& aijson) :
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

class tools_split_str
{
public:
	static constexpr auto fun(std::string_view str)
	{
		std::array<std::string_view, ESPLIT_STR> parts{};
		size_t count = 0;
		size_t start = 0;

		for (size_t i = 0; i < str.size(); ++i)
		{
			if (str[i] == ',' && count < ESPLIT_STR)
			{
				parts[count++] = str.substr(start, i - start);
				if (str[i + 1] == ' ')
				{
					start = i + 2;
				}
				else
				{
					start = i + 1;
				}
			}
		}

		// 处理最后一部分
		if (start < str.size() && count < ESPLIT_STR)
		{
			parts[count++] = str.substr(start);
		}

		return parts;
	}

	static constexpr auto fun()
	{
		std::array<std::string_view, ESPLIT_STR> parts{};
		return parts;
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
	inline void write(ngl::json_write& ijsn)const					\
	{																\
		constexpr std::array<std::string_view, ESPLIT_STR> parts =	\
			tools_split_str::fun(#__VA_ARGS__);						\
		help_writejson ltemp(parts, ijsn);							\
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
		constexpr std::array<std::string_view, ESPLIT_STR> parts =  \
			tools_split_str::fun(#__VA_ARGS__);						\
		help_readjson ltemp(parts, ijsn);							\
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
	inline void write(ngl::json_write& ijsn)const					\
	{																\
		constexpr std::array<std::string_view, ESPLIT_STR> parts =	\
			tools_split_str::fun(#__VA_ARGS__);						\
		help_writejson ltemp(parts, ijsn);							\
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
		constexpr std::array<std::string_view, ESPLIT_STR> parts =  \
			tools_split_str::fun(#__VA_ARGS__);						\
		help_readjson ltemp(parts, ijsn);							\
		return ltemp.fun(0 __VA_OPT__(,) ##__VA_ARGS__);			\
	}
#endif

#define dprotocoljson(NAME, ...)			\
	def_jsonfunction(__VA_ARGS__)			\
	def_protocol(NAME, __VA_ARGS__)