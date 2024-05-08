#pragma once 

#include "nactortype.h"

namespace ngl
{
	template <typename T>
	class nactor_type
	{
		static ENUM_ACTOR m_type;
	public:
		static void inits(ENUM_ACTOR atype)
		{
			m_type = atype;
		}

		static ENUM_ACTOR type()
		{
			if (m_type == ACTOR_NONE)
			{
				throw "m_type == ACTOR_NONE";
			}
			return m_type;
		}
	};

	template <typename T>
	ENUM_ACTOR nactor_type<T>::m_type = ACTOR_NONE;

	// 根据pbdb::ENUM_DB获取ENUM_ACTOR
	ENUM_ACTOR db_enum(EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB TDBTAB_TYPE);

	void init_nactor_type();
}//namespace ngl