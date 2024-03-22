#pragma once

#include "ijson.h"
#include "ojson.h"

#define jsonfunc(...)											\
	inline void add(ngl::ijson& ijsn, const char* akey)const	\
	{															\
		ngl::ijson ltemp;										\
		add(ltemp);												\
		ijsn.add(akey, ltemp.nofree());							\
	}															\
	inline void add(ngl::ijson& ijsn)const						\
	{															\
		ijsn.add(__VA_ARGS__);									\
	}															\
	inline bool dec(ngl::ojson& ijsn, const char* akey)			\
	{															\
		ngl::ojson ltemp;										\
		if (ijsn.dec(akey, ltemp) == false)						\
		return false;											\
		return dec(ltemp);										\
	}															\
	inline bool dec(ngl::ojson& ijsn)							\
	{															\
		return ijsn.dec(__VA_ARGS__);							\
	}
