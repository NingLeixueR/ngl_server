#pragma once

#include "serialize.h"

#define def_function_pop(...)									\
	inline bool pop(ngl::unserialize& ser)						\
	{															\
		return ser.pop(__VA_ARGS__);							\
	}															\
	inline bool ParseFromArray(const void* data, int32_t size)	\
	{															\
		ngl::unserialize lunserialize((const char*)data, size);	\
		return pop(lunserialize);								\
	}


#define def_function_push(...)									\
	inline bool push(ngl::serialize& ser)const					\
	{															\
		return ser.push(__VA_ARGS__);							\
	}															\
	inline bool SerializeToArray(void* data, int32_t size) const \
	{															\
		ngl::serialize lserialize((char*)data, size);			\
		return push(lserialize);								\
	}


#define def_portocol_bytes(...)								\
	inline int bytes(ngl::serialize_bytes& abytes)const		\
	{														\
		return abytes.bytes(__VA_ARGS__);					\
	}														\
	inline int ByteSize()const								\
	{														\
		ngl::serialize_bytes lserialize_bytes;				\
		return bytes(lserialize_bytes);						\
	}

#define def_portocol_name(_Name)							\
	static const char* name()								\
	{														\
		return #_Name;										\
	}

///// �򻯶���Э����
// --- ���� ....(��Ա)
#define def_portocol_function(_Name,...)					\
	def_function_pop	(__VA_ARGS__)				\
	def_function_push	(__VA_ARGS__)				\
	def_portocol_bytes	(__VA_ARGS__)				\
	def_portocol_name	(_Name)

// --- Э���  Э������  ���� ....(��Ա)
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