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
# define def_portocol(_Name,...)  def_portocol_function(_Name, ##__VA_ARGS__)
#else
# define def_portocol(_Name,...)  def_portocol_function(_Name __VA_OPT__(,) ##__VA_ARGS__)
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

#define def_jsonfunction(...)										\
	inline void write(ngl::json_write& ijsn, const char* akey)const	\
	{																\
		ngl::json_write ltemp;										\
		write(ltemp);												\
		ijsn.write(akey, ltemp.nofree());							\
	}																\
	inline void write(ngl::json_write& ijsn)const					\
	{																\
		ijsn.write(__VA_ARGS__);									\
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
		return ijsn.read(__VA_ARGS__);								\
	}