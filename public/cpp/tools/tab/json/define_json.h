#pragma once

#include "json_write.h"
#include "json_read.h"

#define jsonfunc(...)												\
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
	inline bool read(ngl::json_read& ijsn, const char* akey)		\
	{																\
		ngl::json_read ltemp;										\
		if (ijsn.read(akey, ltemp) == false)						\
		return false;												\
		return read(ltemp);											\
	}																\
	inline bool read(ngl::json_read& ijsn)							\
	{																\
		return ijsn.read(__VA_ARGS__);								\
	}
