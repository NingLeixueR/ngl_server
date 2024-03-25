#pragma once

#include "ijson.h"
#include "ojson.h"

#define jsonfunc(...)											\
	inline void write(ngl::ijson& ijsn, const char* akey)const	\
	{															\
		ngl::ijson ltemp;										\
		write(ltemp);											\
		ijsn.write(akey, ltemp.nofree());						\
	}															\
	inline void write(ngl::ijson& ijsn)const					\
	{															\
		ijsn.write(__VA_ARGS__);								\
	}															\
	inline bool read(ngl::ojson& ijsn, const char* akey)		\
	{															\
		ngl::ojson ltemp;										\
		if (ijsn.read(akey, ltemp) == false)					\
		return false;											\
		return read(ltemp);										\
	}															\
	inline bool read(ngl::ojson& ijsn)							\
	{															\
		return ijsn.read(__VA_ARGS__);							\
	}
