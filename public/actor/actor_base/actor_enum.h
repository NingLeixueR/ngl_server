#pragma once

#include <cstdint>
#include <map>
#include <string>
#include "actor_define.h"
#include "csvtable.h"

namespace ngl
{
	class enum_actor
	{
	public:
		static bool is_signle(ENUM_ACTOR aenum);
	};

	// 通过名字获取枚举
	class actor_typename
	{
		actor_typename();
	public:
		static actor_typename& getInstance()
		{
			static actor_typename ltemp;
			return ltemp;
		}

		static bool name2enum(const std::string& aenumname, ENUM_ACTOR& avalue);
		static const char* enum2name(ENUM_ACTOR aenum);
	};

}

